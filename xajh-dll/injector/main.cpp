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
#include <thread>

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>

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

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_TIMER:
            // 关闭窗口
            DestroyWindow(hwnd);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void ShowAutoCloseMessageBox(const std::wstring& message, int durationInSeconds) {
    // 创建一个简单的窗口类
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"AutoCloseMsgBoxClass";
    RegisterClass(&wc);

    // 创建一个窗口
    HWND hwnd = CreateWindowEx(
        0,
        L"AutoCloseMsgBoxClass",
        L"提示",
        WS_OVERLAPPED | WS_CAPTION,  // 使用WS_OVERLAPPED样式并不包括WS_SYSMENU样式
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        300,
        100,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // 创建一个静态文本控件来显示消息
    HWND hStatic = CreateWindowEx(
        0,
        L"STATIC",
        message.c_str(),
        WS_VISIBLE | WS_CHILD,
        10, 10, 280, 40,
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // 设置定时器
    SetTimer(hwnd, 1, durationInSeconds * 1000, NULL);

    // 显示窗口
    ShowWindow(hwnd, SW_SHOW);

    // 进入消息循环，等待WM_CLOSE消息
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // 清理
    KillTimer(hwnd, 1);
}

void AutoCloseMessageBox(const std::wstring& message, int durationInSeconds = 5) {
    // 创建一个线程来显示MessageBox
    std::thread([message, durationInSeconds]() {
        ShowAutoCloseMessageBox(message, durationInSeconds);
    }).detach();  // 分离线程，允许其独立运行
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    wchar_t title[255];
    GetWindowText(hwnd, title, sizeof(title) / sizeof(title[0]));

    if (std::wstring(title) == reinterpret_cast<LPCWSTR>(lParam)) {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        return FALSE;  // 找到特定标题的窗口后关闭并返回FALSE，终止枚举
    }

    return TRUE;  // 继续枚举窗口
}

void CloseWindowByTitle(const std::wstring& windowTitle) {
    EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(windowTitle.c_str()));
}

std::string GetCurrentExecutableDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string fullPath(buffer);
    std::string directory = fullPath.substr(0, fullPath.find_last_of("\\/"));

    return directory;
}
// 拼接路径和文件名
std::string PathJoin(const std::string& path, const std::string& fileName) {
    if (path.empty() || path.back() == '\\' || path.back() == '/') {
        return path + fileName;
    } else {
        return path + "\\" + fileName;
    }
}

void InjectDll(
    const std::wstring& processName,
    const std::string& dllName,
    const std::wstring& successMessage = L"注入成功"
) {
    DWORD pid = GetPidByName(processName.c_str());

    HANDLE processId = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (processId == 0) {
        AutoCloseMessageBox(L"打开进程失败！");
        system("pause");
        return;
    }

    std::string exeDirectory = GetCurrentExecutableDirectory();
    std::string dllPath = PathJoin(exeDirectory, dllName);

    int dllLen = dllPath.length();
    LPVOID buffer = VirtualAllocEx(processId, 0, dllLen, MEM_COMMIT, PAGE_READWRITE);
    if (buffer == NULL) {
        AutoCloseMessageBox(L"申请内存失败！");
        return;
    }

    BOOL Success = WriteProcessMemory(processId, buffer, dllPath.c_str(), dllLen, NULL);
    if (Success == FALSE) {
        AutoCloseMessageBox(L"写入内存失败！");
        return;
    }

    HANDLE remoteThread = CreateRemoteThread(
        processId, NULL, 0,
        (LPTHREAD_START_ROUTINE)LoadLibraryA,
        buffer, 0, NULL
    );

    if (remoteThread == NULL) {
        AutoCloseMessageBox(L"创建远程线程失败！");
        return;
    }

    AutoCloseMessageBox(successMessage);

    WaitForSingleObject(remoteThread, -1);

    VirtualFreeEx(processId, buffer, 0, MEM_RELEASE);
    CloseHandle(remoteThread);
    CloseHandle(processId);
}

int main(int argc, char* argv[]) {
    if (argc >= 2) {
        CloseWindowByTitle(L"XAJH");
        return 0;
    }

    InjectDll(L"xajh.exe", "xajh.dll");
    
    std::this_thread::sleep_for(std::chrono::milliseconds(5500));
    return 0;
}
