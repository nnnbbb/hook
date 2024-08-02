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
    DWORD GetJmp0ldVA();
    BOOL SetNewOep(DWORD oepRva);
    BOOL FixRelocation(DWORD imageBase);
};