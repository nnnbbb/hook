#include <Windows.h>
#include "pack.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <locale>
#include <codecvt>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

// 将代码段数据段合并到一起
#pragma comment(linker, "/merge:.data=.text")
#pragma comment(linker, "/merge:.rdata=.text")
#pragma comment(linker, "/section:.text,RWE")


void packStart();

PACKINFO g_PackInfo = {
    (DWORD)packStart
};


typedef HMODULE(WINAPI* MyLoadLibraryExA)(
    _In_ LPCSTR lpLibFileName,
    _Reserved_ HANDLE hFile,
    _In_ DWORD dwFlags
);


typedef FARPROC(WINAPI* MYGetProcAddress)(
    _In_ HMODULE hModule,
    _In_ LPCSTR lpProcName
);


typedef HMODULE(WINAPI* MyGetModuleHandleA)(
    LPCSTR lpModuleName
);


typedef BOOL(WINAPI* MyVirtualProtect)(
    _In_ LPVOID lpAddress,
    _In_ SIZE_T dwSize,
    _In_ DWORD flNewProtect,
    _Out_ PDWORD lpflOldProtect
);


typedef int(WINAPI* MyMessageBoxA)(
    _In_opt_ HWND hWnd,
    _In_opt_ LPCSTR lpText,
    _In_opt_ LPCSTR lpCaption,
    _In_ UINT uType
);

MyLoadLibraryExA g_MyLoadLibraryExA = NULL;
MYGetProcAddress g_MYGetProcAddress = NULL;
MyGetModuleHandleA g_MyGetModuleHandleA = NULL;
MyVirtualProtect g_MyVirtualProtect = NULL;
MyMessageBoxA g_MyMessageBoxA = NULL;


template <class T>
void _print(T arg) {
    std::cout << arg << " ";
}

template <typename... Args>
void log(Args... args) {
    time_t t;
    struct tm tmp;
    char dt[50];
    time(&t);
    localtime_s(&tmp, &t);
    strftime(dt, sizeof(dt), "[%Y-%m-%d %H:%M:%S]", &tmp);

    std::cout << dt << " ";  // Print timestamp to console

    // std::string path = fs::current_path().string() + "/log.txt";
    std::string path = "PE-Shell-Protect.log";
    // Write to log file
    std::ofstream file(path, std::ios_base::app | std::ios::out);  // 以utf-8编码格式打开文件
    if (file.is_open()) {
        file << dt << " ";                                           // Write timestamp
        int _[] = {0, ((_print(args), file << args << " "), 0)...};  // Expand args
        file << std::endl;                                           // End of log entry
        file.close();                                                // Close file
    }
}
#define CHECK_NOT_NULL(value) \
    do {                      \
        if (value == 0) {     \
            return 0;         \
        }                     \
    } while (0)


DWORD MyGetProcAddress(DWORD hModule, LPCSTR funName) {

    // 获取DOS头Nt头
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeader = (PIMAGE_NT_HEADERS)(pDosHeader->e_lfanew + hModule);
    // 获取导出表
    DWORD exportTableRVa = pNtHeader->OptionalHeader.DataDirectory[0].VirtualAddress;
    PIMAGE_EXPORT_DIRECTORY exportTable = (PIMAGE_EXPORT_DIRECTORY)(exportTableRVa + hModule);

    // 找到导出名称表，序号表，地址表
    DWORD* nameTable = (DWORD*)(exportTable->AddressOfNames + hModule);
    DWORD* funTable = (DWORD*)(exportTable->AddressOfFunctions + hModule);
    WORD* numberTable = (WORD*)(exportTable->AddressOfNameOrdinals + hModule);
    for (size_t i = 0; i < exportTable->NumberOfNames; i++) {
        // 获取函数名字
        char* name = (char*)(nameTable[i] + hModule);
        if (strcmp(name, funName) == 0) {
            return funTable[numberTable[i]] + hModule;
        }
    }
    return 0;
}


// 获取 kernel32 或者 kernelbase.dll 基址
DWORD GetImportantModule() {

    DWORD dwBase = 0;
    _asm {
        mov eax,    dword ptr fs:[0x30]
        mov eax,    dword ptr[eax+0xC]
        mov eax,    dword ptr[eax+0x1C]
        mov eax,    [eax]
        mov eax,    dword ptr[eax+0x8]
        mov dwBase, eax
    }
    return dwBase;
}


BOOL GetFunctions() {
    DWORD kernlBase = GetImportantModule();
    // 获取LoadlilraryExA
    g_MyLoadLibraryExA = (MyLoadLibraryExA)MyGetProcAddress(kernlBase, "LoadLibraryExA");
    // 获取kernel32基址
    HMODULE kernel32Base = g_MyLoadLibraryExA("Kernel32.dll", 0, 0);
    HMODULE user32Base = g_MyLoadLibraryExA("User32.dll", 0, 0);

    g_MYGetProcAddress = (MYGetProcAddress)MyGetProcAddress((DWORD)kernel32Base, "GetProcAddress");
    g_MyGetModuleHandleA = (MyGetModuleHandleA)g_MYGetProcAddress(kernel32Base, "GetModuleHandleA");
    g_MyVirtualProtect = (MyVirtualProtect)g_MYGetProcAddress(kernel32Base, "VirtualProtect");
    g_MyMessageBoxA = (MyMessageBoxA)g_MYGetProcAddress(user32Base, "MessageBoxA");
    return TRUE;
}

BOOL DecodeSections() {
    int key = 0x51;
    HMODULE hModule = g_MyGetModuleHandleA(0);
    PIMAGE_DOS_HEADER pDosHeader = (PIMAGE_DOS_HEADER)hModule;
    PIMAGE_NT_HEADERS pNtHeaders = (PIMAGE_NT_HEADERS)((DWORD)hModule + pDosHeader->e_lfanew);
    PIMAGE_SECTION_HEADER firstSectionHeader = IMAGE_FIRST_SECTION(pNtHeaders);
    char* sectionBuff = (char*)(firstSectionHeader->VirtualAddress + (DWORD)hModule);

    DWORD oldProtect = 0;
    g_MyVirtualProtect(sectionBuff, firstSectionHeader->SizeOfRawData, PAGE_EXECUTE_READWRITE, &oldProtect);

    for (size_t i = 0; i < firstSectionHeader->SizeOfRawData; i++) {
        sectionBuff[i] = sectionBuff[i] ^ key;
    }

    g_MyVirtualProtect(sectionBuff, firstSectionHeader->SizeOfRawData, oldProtect, &oldProtect);

    return TRUE;
}

_declspec(naked) void packStart() {
    _asm pushad;
    GetImportantModule();
    GetFunctions();
    DecodeSections();
    g_MyMessageBoxA(0, "壳代码执行", "提示", MB_OK);
    _asm popad;
    _asm jmp g_PackInfo.oldOEP;
}