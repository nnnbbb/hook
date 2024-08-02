#include "pe-tools.h"
#include "_global.h"
#include "pe.h"

unsigned char* FileData = 0;
ULONG FileSize = 0;

void PeDeinitialize() { RtlFreeMemory(FileData); }

NTSTATUS PeInitialize() {
    UNICODE_STRING FileName;
    OBJECT_ATTRIBUTES ObjectAttributes;
    RtlInitUnicodeString(&FileName, L"\\SystemRoot\\system32\\ntdll.dll");
    InitializeObjectAttributes(&ObjectAttributes, &FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    HANDLE FileHandle;
    IO_STATUS_BLOCK IoStatusBlock;
    NTSTATUS NtStatus =
        ZwCreateFile(&FileHandle, GENERIC_READ, &ObjectAttributes, &IoStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL,
                     FILE_SHARE_READ, FILE_OPEN, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);

    if (NT_SUCCESS(NtStatus)) {
        FILE_STANDARD_INFORMATION StandardInformation = {0};
        NtStatus = ZwQueryInformationFile(FileHandle, &IoStatusBlock, &StandardInformation,
                                          sizeof(FILE_STANDARD_INFORMATION), FileStandardInformation);
        if (NT_SUCCESS(NtStatus)) {
            FileSize = StandardInformation.EndOfFile.LowPart;
            Log("FileSize of ntdll.dll is %08X!\r\n", StandardInformation.EndOfFile.LowPart);
            FileData = (unsigned char*)RtlAllocateMemory(TRUE, FileSize);

            LARGE_INTEGER ByteOffset;
            ByteOffset.LowPart = ByteOffset.HighPart = 0;
            NtStatus = ZwReadFile(FileHandle, NULL, NULL, NULL, &IoStatusBlock, FileData, FileSize, &ByteOffset, NULL);

            if (!NT_SUCCESS(NtStatus)) {
                RtlFreeMemory(FileData);
                Log("ZwReadFile failed with status %08X...\r\n", NtStatus);
            }
        } else {
            Log("ZwQueryInformationFile failed with status %08X...\r\n", NtStatus);
        }
        ZwClose(FileHandle);
    } else {
        Log("ZwCreateFile failed with status %08X...\r\n", NtStatus);
    }
    return NtStatus;
}

int GetExportSsdtIndex(const char* ExportName) {

    ULONG_PTR ExportOffset = PE::GetExportOffset(FileData, FileSize, ExportName);
    if (ExportOffset == PE_ERROR_VALUE) return -1;

    int SsdtOffset = -1;
    unsigned char* ExportData = FileData + ExportOffset;
    for (int i = 0; i < 32 && ExportOffset + i < FileSize; i++) {
        if (ExportData[i] == 0xC2 || ExportData[i] == 0xC3)  // RET
            break;
        if (ExportData[i] == 0xB8)  // mov eax,X
        {
            SsdtOffset = *(int*)(ExportData + i + 1);
            break;
        }
    }

    if (SsdtOffset == -1) {
        Log("SSDT Offset for %s not found...\r\n", ExportName);
    }

    return SsdtOffset;
}