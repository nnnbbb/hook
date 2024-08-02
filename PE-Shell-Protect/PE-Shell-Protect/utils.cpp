#include "utils.h"


Utils::Utils() {
    FileBuff = NULL;
    DosHeader = NULL;
    NtHeader = NULL;
    OptionHeader = NULL;
    FileHeader = NULL;
    FileSize = NULL;
}


Utils::~Utils() {}


BOOL Utils::LoadFile(const char* path) {
    HANDLE handle = CreateFileA(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    FileSize = GetFileSize(handle, NULL);
    FileBuff = new char[FileSize]{};

    printf("FileSize = %d\n", FileSize);

    BOOL status = ReadFile(handle, FileBuff, FileSize, NULL, NULL);
    printf("status = %d\n", status);
    if (status == FALSE) {
        printf("status = %d\n", status);
        MessageBoxA(0, "�ļ���ʧ�ܣ�", "��ʾ", MB_OK);
        return FALSE;
    }
    InitFileInfo();

    return TRUE;
}


BOOL Utils::InitFileInfo() {
    DosHeader = (PIMAGE_DOS_HEADER)FileBuff;
    NtHeader = (PIMAGE_NT_HEADERS)(DosHeader->e_lfanew + FileBuff);
    FileHeader = &NtHeader->FileHeader;
    OptionHeader = &NtHeader->OptionalHeader;
    return 0;
}


BOOL Utils::InsertSection(const char* SectionName, DWORD CodeSize, char* CodeBuff, DWORD Characteristics) {
    // ��ȡ������PE�ļ���С
    DWORD NewFileSize = GetAlignSize(FileSize + CodeSize, OptionHeader->FileAlignment);
    // �����µĻ��������PE�ļ�
    char* NewFileBuff = new char[NewFileSize]{};
    memcpy_s(NewFileBuff, NewFileSize, FileBuff, FileSize);
    FileSize = NewFileSize;
    delete[] FileBuff;
    FileBuff = NewFileBuff;
    InitFileInfo();
    // �������������������ͷ
    PIMAGE_SECTION_HEADER PlastSectionHeader = GetLastSection();

    PlastSectionHeader++;
    // ��������ͷ��������
    // �����ڴ��С
    PlastSectionHeader->Misc.VirtualSize = GetAlignSize(CodeSize, OptionHeader->SectionAlignment);
    // ������������
    strcpy_s((char*)PlastSectionHeader->Name, 8, SectionName);
    // �ļ���С
    PlastSectionHeader->SizeOfRawData = GetAlignSize(CodeSize, OptionHeader->FileAlignment);
    // ����virtualAddress
    PIMAGE_SECTION_HEADER plastSectionHeader2 = GetLastSection();
    PlastSectionHeader->VirtualAddress = plastSectionHeader2->VirtualAddress +
                                         GetAlignSize(plastSectionHeader2->Misc.VirtualSize, OptionHeader->SectionAlignment);

    PlastSectionHeader->PointerToRawData = plastSectionHeader2->PointerToRawData + plastSectionHeader2->SizeOfRawData;
    PlastSectionHeader->Characteristics = Characteristics;
    // �޸�numberOfSections
    FileHeader->NumberOfSections++;
    // size0fImage
    OptionHeader->SizeOfImage += GetAlignSize(CodeSize, OptionHeader->SectionAlignment);

    // �����ǵĿǴ���ŵ��µ�����
    char* SectionAddr = GetLastSection()->PointerToRawData + FileBuff;
    memcpy(SectionAddr, CodeBuff, CodeSize);
    return TRUE;
}


DWORD Utils::GetAlignSize(DWORD RealSize, DWORD AlignSize) {
    if (RealSize % AlignSize == 0) {
        return RealSize;
    }
    return (RealSize / AlignSize + 1) * AlignSize;
}


PIMAGE_SECTION_HEADER Utils::GetLastSection() {
    PIMAGE_SECTION_HEADER FirstSectionHeader = IMAGE_FIRST_SECTION(NtHeader);

    return FirstSectionHeader + (FileHeader->NumberOfSections - 1);
}


BOOL Utils::SaveFile(const char* Path) {
    HANDLE handle = CreateFileA(Path, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    DWORD realWrite = 0;
    BOOL success = WriteFile(handle, FileBuff, FileSize, &realWrite, NULL);
    if (success == FALSE) {
        MessageBoxA(0, "�ļ�д��ʧ�ܣ�", "��ʾ", MB_OK);
        return FALSE;
    }
    CloseHandle(handle);
    return TRUE;
}


BOOL Utils::EncodeSections() {
    PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION(NtHeader);
    int Key = 0x51;
    char* Data = SectionHeader->PointerToRawData + FileBuff;

    for (size_t i = 0; i < SectionHeader->SizeOfRawData; i++) {
        Data[i] = Data[i] ^ Key;
    }
    return TRUE;
}

DWORD Utils::GetJmp0ldVA() {
    DWORD jmpVA = OptionHeader->AddressOfEntryPoint + OptionHeader->ImageBase;
    return jmpVA;
}

BOOL Utils::SetNewOep(DWORD OepRva) {
    OptionHeader->AddressOfEntryPoint = GetLastSection()->VirtualAddress + OepRva;
    return TRUE;
}

// �޸��ض�λ��
BOOL Utils::FixRelocation(DWORD ImageBase) {
    PIMAGE_DOS_HEADER DllDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS DllNtHeader = (PIMAGE_NT_HEADERS)(DllDosHeader->e_lfanew + ImageBase);
    PIMAGE_OPTIONAL_HEADER DllOptionHeader = &DllNtHeader->OptionalHeader;
    IMAGE_DATA_DIRECTORY dataDirctory = DllOptionHeader->DataDirectory[5];
    PIMAGE_BASE_RELOCATION DllRelocation = (PIMAGE_BASE_RELOCATION)(dataDirctory.VirtualAddress + ImageBase);
    PIMAGE_SECTION_HEADER DllFistSectionHeader = IMAGE_FIRST_SECTION(DllNtHeader);

    DWORD OldProtect;

    while (DllRelocation->SizeOfBlock != 0) {
        // DWORD Number = (DllRelocation->SizeOfBlock - 8) / 2;
        DWORD Number = (DllRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;

        char* BeginAddr = (char*)DllRelocation;
        BeginAddr += 8;
        for (size_t i = 0; i < Number; i++) {
            WORD* prelocRva = (WORD*)BeginAddr;

            if ((*prelocRva & 0X3000) == 0X3000) {
                // ȡwordǰ12λ+��ƫ��=rva;
                WORD RepairRva = (*prelocRva & 0x0FFF) + DllRelocation->VirtualAddress;
                // ��ȡ��Ҫ�ض�λ�����ĵ�ַ
                DWORD* RelRepairAddr = (DWORD*)(ImageBase + RepairRva);

                // ��ȡ��������ε���ʼ��ַ
                PIMAGE_SECTION_HEADER LastSection = GetLastSection();
                DWORD NewFileSection = (DWORD)(LastSection->PointerToRawData + FileBuff);
                DWORD NewSectionAddr = LastSection->VirtualAddress + OptionHeader->ImageBase;

                // ��ȡ����������ӵ������еĵ�ַfileBuff�еĵ�ַ
                DWORD DestAddr = (DWORD)RelRepairAddr - (DllFistSectionHeader->VirtualAddress + ImageBase) + NewFileSection;

                *(DWORD*)DestAddr = (*(DWORD*)DestAddr - ImageBase) - DllFistSectionHeader->VirtualAddress + NewSectionAddr;
            }
            BeginAddr += 2;
        }
        DllRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)DllRelocation + DllRelocation->SizeOfBlock);
    }
    return TRUE;
}



