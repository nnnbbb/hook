#include <Windows.h>
#include "iostream"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <chrono>
#include <ctime>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>
/*
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

    //std::string path = fs::current_path().string() + "/log.txt";
    std::string path = "E:/BaiduNetdiskDownload/hack/code/inject-dll/Debug/log.txt";

    // Write to log file
    std::ofstream file(path, std::ios_base::app);
    if (file.is_open()) {
        file << dt << " ";                                               // Write timestamp
        int dummy[] = {0, ((_print(args), file << args << " "), 0)...};  // Expand args
        file << std::endl;                                               // End of log entry
        file.close();                                                    // Close file
    }
}
DWORD _stdcall AutoPlant(LPVOID lparam) {
    DWORD Plant = *(DWORD*)0x00755EAC;
    DWORD v = *(DWORD*)(Plant + 0x868);
    //_asm {
    //    mov ecx,dword ptr ds:[0x00755EAC]
    //    mov ecx,[ecx+0x868]
    //    push 0xFFFFFFFF
    //    push 0  // 植物id  
    //    mov eax,0  // y
    //    push 0  // x
    //    push ecx
    //    mov esi,0x00418D70
    //    call esi
    //}
    return 0;
}

int main(int argc, char* argv[]) {
    //HWND Hwnd = FindWindowA(NULL, "x32dbg [管理员]");
    HWND Hwnd = FindWindowA(NULL, "Plants vs. Zombies");
    DWORD Pid = 0;
    if (Hwnd == NULL) {
        MessageBoxA(0, "游戏未运行！", "提示", MB_OK);
        return 0;
    }
    GetWindowThreadProcessId(Hwnd, &Pid);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
    LPVOID lpA1locAddr = VirtualAllocEx(hProcess, 0, 0x1024, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hProcess, lpA1locAddr, AutoPlant, 0x1024, NULL);
    HANDLE hThead = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)lpA1locAddr, 0, 0, 0);
    system("pause");
    //// WaitForSingleObject(hThead, -1);
    //BOOL r = CloseHandle(hThead);
    //if (r) {
    //    MessageBoxA(0, "线程退出！", "提示", MB_OK);
    //}
    //CloseHandle(hProcess);
    return 0;
}
*/

int main(int argc, char* argv[]) {
    //HWND Hwnd = FindWindowA(NULL, "x32dbg [管理员]");
    HWND Hwnd = FindWindowA(NULL, "Plants vs. Zombies");
    DWORD Pid = 0;
    if (Hwnd == NULL) {
        MessageBoxA(0, "游戏未运行！", "提示", MB_OK);
        return 0;
    }
    GetWindowThreadProcessId(Hwnd, &Pid);
    HANDLE ProcessId = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

    if (ProcessId == 0) {
        MessageBoxA(0, "打开进程失败！", "提示", MB_OK);
        return 0;
    }

    char DllName[] = "E:/BaiduNetdiskDownload/hack/code/inject-dll/Debug/inject-dll.dll";
    int DllLen = sizeof(DllName);
    LPVOID Buffer = VirtualAllocEx(ProcessId, 0, DllLen, MEM_COMMIT, PAGE_READWRITE);
    if (Buffer == NULL) {
        MessageBoxA(0, "申请内存失败！", "提示", MB_OK);
        return 0;
    }

    BOOL Success = WriteProcessMemory(ProcessId, Buffer, DllName, DllLen, NULL);
    if (Success == FALSE) {
        MessageBoxA(0, "写入内存失败！", "提示", MB_OK);
        return 0;
    }

    HANDLE WINAPI RemoteThead =
        CreateRemoteThread(ProcessId, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, Buffer, 0, NULL);

    if (RemoteThead == NULL) {
        MessageBoxA(0, "创建远程线程失败！", "提示", MB_OK);
        return 0;
    }
    MessageBoxA(0, "注入DLL成功", "提示", MB_OK);

    WaitForSingleObject(RemoteThead, -1);
    VirtualFreeEx(ProcessId, Buffer, 0, MEM_RELEASE);
    CloseHandle(RemoteThead);
    CloseHandle(ProcessId);

    return 0;
}
