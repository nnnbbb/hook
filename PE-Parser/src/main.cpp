// main.c:
//    main block of PErser program

#include <iostream>

#include "Windows.h"
#include "headers.h"

#define PE_ERROR_VALUE (ULONG) - 1
#define Log printf

static ULONG RvaToOffset(PIMAGE_NT_HEADERS pnth, ULONG Rva, ULONG FileSize) {
    PIMAGE_SECTION_HEADER psh = IMAGE_FIRST_SECTION(pnth);
    USHORT NumberOfSections = pnth->FileHeader.NumberOfSections;
    for (int i = 0; i < NumberOfSections; i++) {
        if (psh->VirtualAddress <= Rva) {
            if ((psh->VirtualAddress + psh->Misc.VirtualSize) > Rva) {
                Rva -= psh->VirtualAddress;
                Rva += psh->PointerToRawData;
                return Rva < FileSize ? Rva : PE_ERROR_VALUE;
            }
        }
        psh++;
    }
    return PE_ERROR_VALUE;
}

// NT/PE 头
ULONG GetExportOffset(const unsigned char* FileData) {
    // Verify DOS Header
    PIMAGE_DOS_HEADER pdh = (PIMAGE_DOS_HEADER)FileData;
    if (pdh->e_magic != IMAGE_DOS_SIGNATURE) {
        Log("[TITANHIDE] Invalid IMAGE_DOS_SIGNATURE!\r\n");
        return PE_ERROR_VALUE;
    }

    // Verify PE Header
    PIMAGE_NT_HEADERS pnth = (PIMAGE_NT_HEADERS)(FileData + pdh->e_lfanew);
    Log("Signature = 0x%x\n", pnth->Signature);
    Log("Machine = 0x%x\n", pnth->FileHeader.Machine);
    Log("Characteristics = 0x%x\n", pnth->FileHeader.Characteristics);
    Log("NumberOfSections = %d\n", pnth->FileHeader.NumberOfSections);
    Log("SizeOfOptionalHeader = 0x%x\n", pnth->FileHeader.SizeOfOptionalHeader);
    if (pnth->Signature != IMAGE_NT_SIGNATURE) {
        Log("[TITANHIDE] Invalid IMAGE_NT_SIGNATURE!\r\n");
        return PE_ERROR_VALUE;
    }
}

// 区段信息
ULONG GetPageBase(PVOID lpHeader) {
    IMAGE_DOS_HEADER* pdh = (IMAGE_DOS_HEADER*)lpHeader;
    if (pdh->e_magic != IMAGE_DOS_SIGNATURE) {
        return 0;
    }
    IMAGE_NT_HEADERS* pnth = (IMAGE_NT_HEADERS*)((unsigned char*)lpHeader + pdh->e_lfanew);
    if (pnth->Signature != IMAGE_NT_SIGNATURE) {
        return 0;
    }
    IMAGE_SECTION_HEADER* imageSectionHeader = IMAGE_FIRST_SECTION(pnth);
    for (size_t i = 0; i < pnth->FileHeader.NumberOfSections; i++) {
        char name[9]{0};
        memcpy_s(name, 9, imageSectionHeader->Name, 8);
        printf("image section name = %s\n", name);
        imageSectionHeader++;
    }
}

// 重定位表
void RelocationTable(const unsigned char* FileData, ULONG FileSize) {
    PIMAGE_DOS_HEADER ImageDosHeader = (PIMAGE_DOS_HEADER)FileData;
    PIMAGE_NT_HEADERS ImageNtHeaders = (PIMAGE_NT_HEADERS)(FileData + ImageDosHeader->e_lfanew);

    // Verify Export Directory
    PIMAGE_DATA_DIRECTORY ImageDataDirctory = NULL;
    if (ImageNtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        ImageDataDirctory = ((PIMAGE_NT_HEADERS64)ImageNtHeaders)->OptionalHeader.DataDirectory;
    } else {
        ImageDataDirctory = ((PIMAGE_NT_HEADERS32)ImageNtHeaders)->OptionalHeader.DataDirectory;
    }

    ULONG RelocationRva = ImageDataDirctory[IMAGE_DIRECTORY_ENTRY_BASERELOC].VirtualAddress;
    // ULONG RelocationSize = ImageDataDirctory[IMAGE_DIRECTORY_ENTRY_BASERELOC].Size;

    ULONG RelocationOffset = RvaToOffset(ImageNtHeaders, RelocationRva, FileSize);
    PIMAGE_BASE_RELOCATION BaseRelocation = (PIMAGE_BASE_RELOCATION)(FileData + RelocationOffset);
    DWORD RelocationSize = (BaseRelocation->SizeOfBlock - 8) / 2;
    PWORD RelocationBlockOffset = (PWORD)BaseRelocation + 4;
    while (true) {
        if (BaseRelocation->VirtualAddress == 0) {
            break;
        }

        for (size_t i = 0; i < RelocationSize; i++) {
            if (((*RelocationBlockOffset) & 0x3000) == 0x3000) {
                ULONG rva = ((*RelocationBlockOffset & 0x0fff) + BaseRelocation->VirtualAddress);
                printf("offset = 0x%08x rva = 0x%08x\n", *RelocationBlockOffset, rva);
            }
            RelocationBlockOffset++;
        }
        BaseRelocation = (PIMAGE_BASE_RELOCATION)((ULONGLONG)BaseRelocation + (ULONGLONG)BaseRelocation->SizeOfBlock);
    }
}

// 导入表
void ImportDirectory(const unsigned char* FileData, ULONG FileSize) {
    PIMAGE_DOS_HEADER ImageDosHeader = (PIMAGE_DOS_HEADER)FileData;
    PIMAGE_NT_HEADERS ImageNtHeaders = (PIMAGE_NT_HEADERS)(FileData + ImageDosHeader->e_lfanew);

    // Verify Export Directory
    PIMAGE_DATA_DIRECTORY ImageDataDirctory = NULL;
    if (ImageNtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        ImageDataDirctory = ((PIMAGE_NT_HEADERS64)ImageNtHeaders)->OptionalHeader.DataDirectory;
    } else {
        ImageDataDirctory = ((PIMAGE_NT_HEADERS32)ImageNtHeaders)->OptionalHeader.DataDirectory;
    }

    ULONG ImportDirRva = ImageDataDirctory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress;
    ULONG ImportDirSize = ImageDataDirctory[IMAGE_DIRECTORY_ENTRY_IMPORT].Size;

    ULONG ImportDirOffset = RvaToOffset(ImageNtHeaders, ImportDirRva, FileSize);

    PIMAGE_IMPORT_DESCRIPTOR ImportDescriptor = (PIMAGE_IMPORT_DESCRIPTOR)(FileData + ImportDirOffset);
    char* DllName = (char*)(RvaToOffset(ImageNtHeaders, ImportDescriptor->Name, FileSize) + FileData);

    while (ImportDescriptor->OriginalFirstThunk) {
        char* ImportDllName = (char*)(RvaToOffset(ImageNtHeaders, ImportDescriptor->Name, FileSize) + FileData);
        printf("Import Dll Name = %s\n", ImportDllName);
        PIMAGE_THUNK_DATA ThunkData =
            (PIMAGE_THUNK_DATA)(RvaToOffset(ImageNtHeaders, ImportDescriptor->OriginalFirstThunk, FileSize) + FileData);
        if (ThunkData->u1.Ordinal) {
            if (ThunkData->u1.Ordinal & 0x80000000) {
                printf("ThunkData Ordinal = %llu\n", ThunkData->u1.Ordinal & 0x7FFFFFFF);
            } else {
                PIMAGE_IMPORT_BY_NAME ImportName =
                    (PIMAGE_IMPORT_BY_NAME)(RvaToOffset(ImageNtHeaders, ThunkData->u1.AddressOfData, FileSize) +
                                            FileData);
                printf("Import Name = %s\n", ImportName->Name);
            }

            ThunkData++;
        }
        ImportDescriptor++;
    }
}

// 导出表
ULONG ExportDirectory(const unsigned char* FileData, ULONG FileSize) {
    PIMAGE_DOS_HEADER ImageDosHeader = (PIMAGE_DOS_HEADER)FileData;
    PIMAGE_NT_HEADERS ImageNtHeaders = (PIMAGE_NT_HEADERS)(FileData + ImageDosHeader->e_lfanew);

    // Verify Export Directory
    PIMAGE_DATA_DIRECTORY ImageDataDirctory = NULL;
    if (ImageNtHeaders->OptionalHeader.Magic == IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        ImageDataDirctory = ((PIMAGE_NT_HEADERS64)ImageNtHeaders)->OptionalHeader.DataDirectory;
    } else {
        ImageDataDirctory = ((PIMAGE_NT_HEADERS32)ImageNtHeaders)->OptionalHeader.DataDirectory;
    }

    ULONG ExportDirRva = ImageDataDirctory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    ULONG ExportDirSize = ImageDataDirctory[IMAGE_DIRECTORY_ENTRY_EXPORT].Size;

    ULONG ExportDirOffset = RvaToOffset(ImageNtHeaders, ExportDirRva, FileSize);
    if (ExportDirOffset == PE_ERROR_VALUE) {
        Log("[TITANHIDE] Invalid Export Directory!\r\n");
        return PE_ERROR_VALUE;
    }

    PIMAGE_EXPORT_DIRECTORY ExportDir = (PIMAGE_EXPORT_DIRECTORY)(FileData + ExportDirOffset);
    char* DllName = (char*)(RvaToOffset(ImageNtHeaders, ExportDir->Name, FileSize) + FileData);
    printf("dll name = %s\n", DllName);

    ULONG NumberOfNames = ExportDir->NumberOfNames;

    ULONG AddressOfFunctionsOffset = RvaToOffset(ImageNtHeaders, ExportDir->AddressOfFunctions, FileSize);

    ULONG AddressOfNameOrdinalsOffset = RvaToOffset(ImageNtHeaders, ExportDir->AddressOfNameOrdinals, FileSize);

    ULONG AddressOfNamesOffset = RvaToOffset(ImageNtHeaders, ExportDir->AddressOfNames, FileSize);

    if (AddressOfFunctionsOffset == PE_ERROR_VALUE || AddressOfNameOrdinalsOffset == PE_ERROR_VALUE ||
        AddressOfNamesOffset == PE_ERROR_VALUE) {
        Log("[TITANHIDE] Invalid Export Directory Contents!\r\n");
        return PE_ERROR_VALUE;
    }
    ULONG* AddressOfFunctions = (ULONG*)(FileData + AddressOfFunctionsOffset);
    USHORT* AddressOfNameOrdinals = (USHORT*)(FileData + AddressOfNameOrdinalsOffset);
    ULONG* AddressOfNames = (ULONG*)(FileData + AddressOfNamesOffset);

    // Find Export
    ULONG ExportOffset = PE_ERROR_VALUE;
    for (ULONG i = 0; i < NumberOfNames; i++) {
        ULONG CurrentNameOffset = RvaToOffset(ImageNtHeaders, AddressOfNames[i], FileSize);
        if (CurrentNameOffset == PE_ERROR_VALUE) {
            continue;
        }
        const char* CurrentName = (const char*)(FileData + CurrentNameOffset);
        ULONG CurrentFunctionRva = AddressOfFunctions[AddressOfNameOrdinals[i]];
        if (CurrentFunctionRva >= ExportDirRva && CurrentFunctionRva < ExportDirRva + ExportDirSize)
            continue;  // we ignore forwarded exports
                       // printf("CurrentName = %s\n", CurrentName);
    }
    return 0;
}

int main(int argc, char* argv[]) {
    // const char* path = "E:/Program Files/OpenArk64.exe";
    const char* path2 = "C:/Windows/System32/ntdll.dll";
    const char* path3 = "E:/多益网络/梦想世界3/Bin/VoiceUtil.dll";
    HANDLE handle = CreateFileA(path3, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
    DWORD fileSize = GetFileSize(handle, NULL);
    char* FileData = new char[fileSize];
    DWORD size = 0;
    bool r = ReadFile(handle, FileData, fileSize, &size, NULL);

    GetExportOffset((unsigned char*)FileData);
    GetPageBase(FileData);
    ExportDirectory((unsigned char*)FileData, fileSize);
    ImportDirectory((unsigned char*)FileData, fileSize);
    RelocationTable((unsigned char*)FileData, fileSize);

    delete[] FileData;
    CloseHandle(handle);
    // load_file(argc, argv);
    return 0;
}