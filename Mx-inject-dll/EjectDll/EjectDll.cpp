// EjectDll.exe

#include "windows.h"
#include "tlhelp32.h"
#include "tchar.h"
#include <iostream>
#include <wchar.h>
#include <shlwapi.h>  // for PathStripPath


#define DEF_PROC_NAME (L"notepad.exe")
#define DEF_DLL_NAME (L"myhack.dll")

DWORD FindProcessID(LPCTSTR szProcessName) {
    DWORD dwPID = 0xFFFFFFFF;
    HANDLE hSnapShot = INVALID_HANDLE_VALUE;
    PROCESSENTRY32 pe;

    // 获取系统快照
    pe.dwSize = sizeof(PROCESSENTRY32);
    hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);

    // 查找进程
    Process32First(hSnapShot, &pe);
    do {
        if (!_tcsicmp(szProcessName, (LPCTSTR)pe.szExeFile)) {
            dwPID = pe.th32ProcessID;
            break;
        }
    } while (Process32Next(hSnapShot, &pe));

    CloseHandle(hSnapShot);

    return dwPID;
}

BOOL SetPrivilege(LPCTSTR lpszPrivilege, BOOL bEnablePrivilege) {
    TOKEN_PRIVILEGES tp;
    HANDLE hToken;
    LUID luid;

    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)) {
        _tprintf(L"OpenProcessToken error: %u\n", GetLastError());
        return FALSE;
    }

    if (!LookupPrivilegeValue(NULL,           // lookup privilege on local system
                              lpszPrivilege,  // privilege to lookup
                              &luid))         // receives LUID of privilege
    {
        _tprintf(L"LookupPrivilegeValue error: %u\n", GetLastError());
        return FALSE;
    }

    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    // Enable the privilege or disable all privileges.
    if (!AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), (PTOKEN_PRIVILEGES)NULL, (PDWORD)NULL)) {
        _tprintf(L"AdjustTokenPrivileges error: %u\n", GetLastError());
        return FALSE;
    }

    if (GetLastError() == ERROR_NOT_ALL_ASSIGNED) {
        _tprintf(L"The token does not have the specified privilege. \n");
        return FALSE;
    }

    return TRUE;
}

// 将 WCHAR 字符串转换为 char 字符串
std::string wcharToChar(const WCHAR* wideStr) {
    int charLength = WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, NULL, 0, NULL, NULL);
    char* multiByteStr = new char[charLength];
    WideCharToMultiByte(CP_UTF8, 0, wideStr, -1, multiByteStr, charLength, NULL, NULL);
    std::string result(multiByteStr);
    delete[] multiByteStr;
    return result;
}

BOOL EjectDll(DWORD dwPID, LPCTSTR szDllName) {
    BOOL bMore = FALSE, bFound = FALSE;
    HANDLE hSnapshot, hProcess, hThread;
    HMODULE hModule = NULL;
    MODULEENTRY32 me = {sizeof(me)};
    LPTHREAD_START_ROUTINE pThreadProc;

    // dwPID = notepad 进程ID
    // 使用TH32CS_SNAPMODULE参数，获取加载到notepad进程的DLL名称
    hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwPID);

    bMore = Module32First(hSnapshot, &me);
    for (; bMore; bMore = Module32Next(hSnapshot, &me)) {


        if (
            //!_tcsicmp((LPCTSTR)me.szModule, szDllName) ||
            //!_tcsicmp((LPCTSTR)me.szExePath, szDllName)
            wcscmp(me.szModule, szDllName) == 0 ||
            wcscmp(me.szExePath, szDllName) == 0
        ) {
            std::wcout << L"szModule: " << me.szModule << std::endl;
            // std::wcout << L"szExePath: " << me.szExePath << std::endl;
            // 调用方法将宽字符转换为 char 字符串
            std::string charStr = wcharToChar(me.szExePath);

            // 输出转换后的 char 字符串
            printf("szExePath: %s\n", charStr.c_str());

            bFound = TRUE;
            break;
        }
    }

    if (!bFound) {
        CloseHandle(hSnapshot);
        return FALSE;
    }

    if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPID))) {
        _tprintf(L"OpenProcess(%d) failed!!! [%d]\n", dwPID, GetLastError());
        return FALSE;
    }

    hModule = GetModuleHandle(L"kernel32.dll");
    // 获取FreeLibrary函数加载地址，并使用CreateRemoteThread进行调用
    pThreadProc = (LPTHREAD_START_ROUTINE)GetProcAddress(hModule, "FreeLibrary");
    hThread = CreateRemoteThread(hProcess, NULL, 0, pThreadProc, me.modBaseAddr, 0, NULL);
    WaitForSingleObject(hThread, INFINITE);

    CloseHandle(hThread);
    CloseHandle(hProcess);
    CloseHandle(hSnapshot);

    return TRUE;
}

int main(int argc, TCHAR* argv[]) {

    DWORD dwPID = 0xFFFFFFFF;

    // 查找process
    dwPID = FindProcessID(L"mxmain.dll");
    if (dwPID == 0xFFFFFFFF) {
        _tprintf(L"There is no <%s> process!\n", DEF_PROC_NAME);
        return 1;
    }
    // EjectDll(dwPID, L"mx-inject-dll.dll");
    EjectDll(dwPID, L"EPStableCommon.dll");
    system("pause");
    //_tprintf(L"PID of \"%s\" is %d\n", DEF_PROC_NAME, dwPID);

    //// 更改 privilege
    // if (!SetPrivilege(SE_DEBUG_NAME, TRUE))
    //     return 1;

    //// 注入 dll
    // if (EjectDll(dwPID, DEF_DLL_NAME))
    //     _tprintf(L"EjectDll(%d, \"%s\") success!!!\n", dwPID, DEF_DLL_NAME);
    // else
    //     _tprintf(L"EjectDll(%d, \"%s\") failed!!!\n", dwPID, DEF_DLL_NAME);

    return 0;
}