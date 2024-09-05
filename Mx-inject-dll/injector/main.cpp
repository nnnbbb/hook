#include <Windows.h>
#include "iostream"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <tlhelp32.h>
#include <processsnapshot.h>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>

template <class T>
void _print(T arg) {
    std::cout << arg << " ";
}
/*
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
*/
// 根据进程名得到进程PID
DWORD GetPidByName(const WCHAR* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            WCHAR* szName = pe32.szExeFile;
            // printf("szName = %ls\n", szName);
            if (wcscmp(szName, name) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return pid;
}

HANDLE free(DWORD dwProcID, LPVOID lpModuleAddress) {
    // 获取待卸载进程的句柄
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcID);

    // 获取kernel32.dll的模块句柄
    HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");

    // 获取FreeLibrary函数的起始地址
    LPTHREAD_START_ROUTINE lpStartAddress = (LPTHREAD_START_ROUTINE)GetProcAddress(hKernel32, "FreeLibrary");

    // 在待卸载进程中创建远程线程（卸载动态链接库模块）
    HANDLE hRemoteThread = CreateRemoteThread(hProcess, NULL, 0, lpStartAddress, lpModuleAddress, 0, NULL);

    // 等待远程线程结束，防止线程阻塞，等待时间：两秒钟
    WaitForSingleObject(hRemoteThread, 2000);

    CloseHandle(hRemoteThread);
    CloseHandle(hProcess);

    return hRemoteThread;
}


int main(int argc, char* argv[]) {
    // HWND Hwnd = FindWindowA(NULL, "x32dbg [管理员]");
    DWORD Pid = GetPidByName(L"mxmain.dll");

    HANDLE ProcessId = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

    if (ProcessId == 0) {
        MessageBoxA(0, "打开进程失败！", "提示", MB_OK);
        return 0;
    }

    char DllName[] = "E:/BaiduNetdiskDownload/hack/code/Mx-inject-dll/Release/mx-inject-dll.dll";
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
    printf("Buffer = 0x%x\n", Buffer);
    system("pause");

    // free(Pid, Buffer);

    VirtualFreeEx(ProcessId, Buffer, 0, MEM_RELEASE);
    CloseHandle(RemoteThead);
    CloseHandle(ProcessId);

    return 0;
}
