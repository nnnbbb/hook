#pragma once
#include <Windows.h>
#include <iostream>

class Utils {
   private:
    char* FileBuff;
    DWORD FileSize;
    PIMAGE_DOS_HEADER DosHeader;
    PIMAGE_NT_HEADERS NtHeader;
    PIMAGE_OPTIONAL_HEADER OptionHeader;
    PIMAGE_FILE_HEADER FileHeader;
    DWORD ImportTableSize;
    PIMAGE_IMPORT_DESCRIPTOR NewImportTable;

   public:
    Utils();
    ~Utils();

    BOOL LoadFile(const char* path);
    // BOOL InitFileInfo();
    BOOL InitFileInfo();
    BOOL InsertSection(
        const char* sectionName,
        DWORD codeSize,
        char* codeBuffer,
        DWORD dwCharateristic
    );
    DWORD GetAlignSize(DWORD realSize, DWORD alignSize);
    PIMAGE_SECTION_HEADER GetLastSection();
    BOOL SaveFile(const char* path);
    BOOL EncodeSections();
    DWORD GetJmpOldVA();
    BOOL SetNewOep(DWORD oepRva);
    BOOL FixDllRelocation(DWORD ImageBase);
    BOOL GetImportTable();
    DWORD RvaToFoa(DWORD Rva);
    DWORD FoaToRva(DWORD Foa);
    DWORD GetAllSectionsSize(DWORD ImageBase);
};