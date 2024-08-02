#include <Windows.h>
#include "utils.h"

typedef struct _PACKINFO {
    DWORD newOEP;
    DWORD oldOEP;
} PACKINFO, *PPACKINFO;

int main() {
    Utils utils;
    utils.LoadFile("E:/BaiduNetdiskDownload/hack/InstDrv.exe");
    utils.EncodeSections();

    HMODULE PackModule = LoadLibraryA("pack.dll");
    if (PackModule == 0) {
        MessageBoxA(0, "pack dll ¼ÓÔØÊ§°Ü", "ÌáÊ¾", MB_OK);
        return 0;
    }
    PPACKINFO packInfo = (PPACKINFO)GetProcAddress(PackModule, "g_PackInfo");
    packInfo->oldOEP = utils.GetJmp0ldVA();

    PIMAGE_DOS_HEADER dllDosHeader = (PIMAGE_DOS_HEADER)PackModule;
    PIMAGE_NT_HEADERS dllNtHeaders = (PIMAGE_NT_HEADERS)(dllDosHeader->e_lfanew + (DWORD)PackModule);
    PIMAGE_SECTION_HEADER dllFirstSectionHeader = IMAGE_FIRST_SECTION(dllNtHeaders);
    char* sectionBuff = (char*)(dllFirstSectionHeader->VirtualAddress + (DWORD)PackModule);

    utils.InsertSection("51hook", dllFirstSectionHeader->Misc.VirtualSize, sectionBuff, 0xE00000E0);
    utils.FixRelocation((DWORD)PackModule);

    DWORD oepRva = packInfo->newOEP - (DWORD)PackModule;
    DWORD offset = oepRva - dllFirstSectionHeader->VirtualAddress;
    utils.SetNewOep(offset);

    utils.SaveFile("E:/BaiduNetdiskDownload/hack/InstDrv.pack.exe");
    return 0;
}