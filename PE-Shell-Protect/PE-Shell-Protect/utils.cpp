#include "utils.h"


Utils::Utils() {
    FileBuff = NULL;
    DosHeader = NULL;
    NtHeader = NULL;
    OptionHeader = NULL;
    FileHeader = NULL;
    FileSize = NULL;
    ImportTableSize = NULL;
    NewImportTable = NULL;
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
        MessageBoxA(0, "文件打开失败！", "提示", MB_OK);
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
    CodeSize += ImportTableSize;
    // 获取对齐后的PE文件大小
    DWORD NewFileSize = GetAlignSize(FileSize + CodeSize, OptionHeader->FileAlignment);
    // 创建新的缓冲区存放PE文件
    char* NewFileBuff = new char[NewFileSize]{};
    memcpy_s(NewFileBuff, NewFileSize, FileBuff, FileSize);
    FileSize = NewFileSize;
    delete[] FileBuff;
    FileBuff = NewFileBuff;
    InitFileInfo();
    // 给新增的区段添加区段头
    PIMAGE_SECTION_HEADER PlastSectionHeader = GetLastSection();

    PlastSectionHeader++;
    // 给新区段头设置属性
    // 设置内存大小
    PlastSectionHeader->Misc.VirtualSize = GetAlignSize(CodeSize, OptionHeader->SectionAlignment);
    // 设置区段名称
    strcpy_s((char*)PlastSectionHeader->Name, 8, SectionName);
    // 文件大小
    PlastSectionHeader->SizeOfRawData = GetAlignSize(CodeSize, OptionHeader->FileAlignment);
    // 设置virtualAddress
    PIMAGE_SECTION_HEADER plastSectionHeader2 = GetLastSection();
    PlastSectionHeader->VirtualAddress = plastSectionHeader2->VirtualAddress +
                                         GetAlignSize(plastSectionHeader2->Misc.VirtualSize, OptionHeader->SectionAlignment);

    PlastSectionHeader->PointerToRawData = plastSectionHeader2->PointerToRawData + plastSectionHeader2->SizeOfRawData;
    PlastSectionHeader->Characteristics = Characteristics;
    // 修改numberOfSections
    FileHeader->NumberOfSections++;
    // size0fImage
    OptionHeader->SizeOfImage += GetAlignSize(CodeSize, OptionHeader->SectionAlignment);

    // 将我们的壳代码放到新的区段
    char* SectionAddr = GetLastSection()->PointerToRawData + FileBuff;
    memcpy(SectionAddr, CodeBuff, CodeSize);

    // 移动导入表
    SectionAddr += CodeSize;
    memcpy(SectionAddr, NewImportTable, ImportTableSize);
    OptionHeader->DataDirectory[1].VirtualAddress = FoaToRva(SectionAddr - FileBuff);
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
        MessageBoxA(0, "文件写入失败！", "提示", MB_OK);
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

DWORD Utils::GetJmpOldVA() {
    DWORD jmpVA = OptionHeader->AddressOfEntryPoint + OptionHeader->ImageBase;
    return jmpVA;
}

BOOL Utils::SetNewOep(DWORD OepRva) {
    OptionHeader->AddressOfEntryPoint = GetLastSection()->VirtualAddress + OepRva;
    return TRUE;
}

// 修复重定位表
BOOL Utils::FixDllRelocation(DWORD ImageBase) {
    PIMAGE_DOS_HEADER DllDosHeader = (PIMAGE_DOS_HEADER)ImageBase;
    PIMAGE_NT_HEADERS DllNtHeader = (PIMAGE_NT_HEADERS)(DllDosHeader->e_lfanew + ImageBase);
    PIMAGE_OPTIONAL_HEADER DllOptionHeader = &DllNtHeader->OptionalHeader;
    IMAGE_DATA_DIRECTORY DataDirctory = DllOptionHeader->DataDirectory[5];
    PIMAGE_BASE_RELOCATION DllRelocation = (PIMAGE_BASE_RELOCATION)(DataDirctory.VirtualAddress + ImageBase);
    PIMAGE_SECTION_HEADER DllFistSectionHeader = IMAGE_FIRST_SECTION(DllNtHeader);

    DWORD OldProtect;

    while (DllRelocation->SizeOfBlock != 0) {
        // DWORD Number = (DllRelocation->SizeOfBlock - 8) / 2;
        DWORD Number = (DllRelocation->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / 2;

        char* BeginAddr = (char*)DllRelocation;
        BeginAddr += 8;
        for (size_t i = 0; i < Number; i++) {
            WORD* PrelocRva = (WORD*)BeginAddr;

            if ((*PrelocRva & 0X3000) == 0X3000) {
                // 取word前12位+大偏移=rva;
                WORD RepairRva = (*PrelocRva & 0x0FFF) + DllRelocation->VirtualAddress;
                // 获取需要重定位变量的地址
                DWORD* RelRepairAddr = (DWORD*)(ImageBase + RepairRva);

                // 获取新添加区段的起始地址
                PIMAGE_SECTION_HEADER LastSection = GetLastSection();
                DWORD NewFileSection = (DWORD)(LastSection->PointerToRawData + FileBuff);
                DWORD NewSectionAddr = LastSection->VirtualAddress + OptionHeader->ImageBase;

                // 获取变量在新添加的区段中的地址fileBuff中的地址
                DWORD DestAddr = (DWORD)RelRepairAddr - (DllFistSectionHeader->VirtualAddress + ImageBase) + NewFileSection;

                *(DWORD*)DestAddr = (*(DWORD*)DestAddr - ImageBase) - DllFistSectionHeader->VirtualAddress + NewSectionAddr;
            }
            BeginAddr += 2;
        }
        DllRelocation = (PIMAGE_BASE_RELOCATION)((DWORD)DllRelocation + DllRelocation->SizeOfBlock);
    }
    return TRUE;
}


BOOL Utils::GetImportTable() {
    char* Offset = RvaToFoa(OptionHeader->DataDirectory[1].VirtualAddress) + FileBuff;
    PIMAGE_IMPORT_DESCRIPTOR ImportTable = (PIMAGE_IMPORT_DESCRIPTOR)(Offset);
    PIMAGE_IMPORT_DESCRIPTOR FitstTable = (PIMAGE_IMPORT_DESCRIPTOR)(Offset);
    size_t TableLen = 0;
    while (ImportTable->Name != NULL) {
        TableLen++;
        ImportTable++;
    }
    if (TableLen == 0) {
        return TRUE;
    }
    TableLen++;
    size_t ImportDescriptorSize = sizeof(IMAGE_IMPORT_DESCRIPTOR);
    ImportTableSize = ImportDescriptorSize * TableLen;
    NewImportTable = new IMAGE_IMPORT_DESCRIPTOR[TableLen]{};
    memcpy(NewImportTable, FitstTable, (ImportDescriptorSize * (TableLen - 1)));
    return TRUE;
}


DWORD Utils::RvaToFoa(DWORD Rva) {
    DWORD Foa = 0;
    PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION(NtHeader);
    while (SectionHeader->Name != NULL) {
        // 判断当前地址属于哪个区段
        if (Rva >= SectionHeader->VirtualAddress &&
            Rva < (SectionHeader->VirtualAddress + SectionHeader->Misc.VirtualSize)) {
            Foa = Rva - SectionHeader->VirtualAddress + SectionHeader->PointerToRawData;
            break;
        }
        SectionHeader++;
    }

    return Foa;
}


DWORD Utils::FoaToRva(DWORD Foa) {
    DWORD Rva = 0;
    PIMAGE_SECTION_HEADER SectionHeader = IMAGE_FIRST_SECTION(NtHeader);
    while (SectionHeader->Name != NULL) {
        // 判断当前地址属于哪个区段
        if (Foa >= SectionHeader->PointerToRawData &&
            Foa < (SectionHeader->PointerToRawData + SectionHeader->SizeOfRawData)) {

            Rva = Foa + SectionHeader->VirtualAddress - SectionHeader->PointerToRawData;
            break;
        }
        SectionHeader++;
    }

    return Rva;
}