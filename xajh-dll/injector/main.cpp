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

// ���ݽ������õ�����PID
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
    static HBRUSH hBrush = NULL;

    switch (uMsg) {
        case WM_CREATE:
            // ��������ˢ
            hBrush = CreateSolidBrush(RGB(255, 255, 255));  // ��ɫ����
            break;
        case WM_CTLCOLORSTATIC: {
            HDC hdcStatic = (HDC)wParam;
            SetTextColor(hdcStatic, RGB(0, 0, 0));      // ��ɫ����
            SetBkColor(hdcStatic, RGB(255, 255, 255));  // ��ɫ����
            return (INT_PTR)hBrush;
        }
        case WM_TIMER:
            // �رմ���
            DestroyWindow(hwnd);
            break;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            // ɾ������ˢ
            if (hBrush) {
                DeleteObject(hBrush);
            }
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}


void ShowAutoCloseMessageBox(const std::wstring& message, int durationInSeconds) {
    // ����һ���򵥵Ĵ�����
    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = L"AutoCloseMsgBoxClass";
    RegisterClass(&wc);

    // ����һ������
    HWND hwnd = CreateWindowEx(
        0,
        L"AutoCloseMsgBoxClass",
        L"",
        WS_OVERLAPPED | WS_CAPTION,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        150,
        100,
        NULL,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );
    SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);

    // ������̬�ı��ؼ�����ʾ��Ϣ
    HWND hStatic = CreateWindowEx(
        0,
        L"STATIC",
        message.c_str(),
        WS_VISIBLE | WS_CHILD,
        0, 0, 0, 0,  // ��ʱ����λ�úͳߴ�Ϊ0
        hwnd,
        NULL,
        GetModuleHandle(NULL),
        NULL
    );

    // ���þ�̬�ı��ؼ�������Ϊ CodeNewRoman Nerd Font Mono
    HFONT hFont = CreateFont(
        18,                             // ����߶�
        0,                              // ������
        0,                              // ������б�Ƕ�
        0,                              // ������б����
        FW_NORMAL,                      // ��������
        FALSE,                          // б��
        FALSE,                          // �»���
        FALSE,                          // ɾ����
        DEFAULT_CHARSET,                // �ַ���
        OUT_PS_ONLY_PRECIS,             // �������
        CLIP_STROKE_PRECIS,             // ��������
        PROOF_QUALITY,                  // �������
        DEFAULT_PITCH | FF_SWISS,       // �������
        L"CodeNewRoman Nerd Font Mono"  // ��������
    );

    SendMessage(hStatic, WM_SETFONT, (WPARAM)hFont, TRUE);

    // �����ı���ʵ�ʳߴ�
    HDC hdc = GetDC(hStatic);
    SelectObject(hdc, hFont);
    RECT textRect = {0, 0, 0, 0};
    DrawText(hdc, message.c_str(), -1, &textRect, DT_CALCRECT);
    ReleaseDC(hStatic, hdc);

    int textWidth = textRect.right - textRect.left;
    int textHeight = textRect.bottom - textRect.top;

    // ��ȡ���ڿͻ����ߴ�
    RECT rect;
    GetClientRect(hwnd, &rect);
    int windowWidth = rect.right - rect.left;
    int windowHeight = rect.bottom - rect.top;

    // ���㾲̬�ı��ؼ���λ�ã�ʹ�����
    int staticX = (windowWidth - textWidth) / 2;
    int staticY = (windowHeight - textHeight) / 2;

    // ���þ�̬�ı��ؼ���λ�úͳߴ�
    SetWindowPos(hStatic, NULL, staticX, staticY, textWidth, textHeight, SWP_NOZORDER);

    // ���ö�ʱ��
    SetTimer(hwnd, 1, durationInSeconds * 1000, NULL);

    // ��ʾ����
    ShowWindow(hwnd, SW_SHOW);

    // ������Ϣѭ�����ȴ�WM_CLOSE��Ϣ
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // ����
    KillTimer(hwnd, 1);
    DeleteObject(hFont);  // ɾ���������
}


void AutoCloseMessageBox(const std::wstring& message, int durationInSeconds = 5) {
    // ����һ���߳�����ʾMessageBox
    std::thread([message, durationInSeconds]() {
        ShowAutoCloseMessageBox(message, durationInSeconds);
    }).detach();  // �����̣߳��������������
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    wchar_t title[255];
    GetWindowText(hwnd, title, sizeof(title) / sizeof(title[0]));

    if (std::wstring(title) == reinterpret_cast<LPCWSTR>(lParam)) {
        SendMessage(hwnd, WM_CLOSE, 0, 0);
        return FALSE;  // �ҵ��ض�����Ĵ��ں�رղ�����FALSE����ֹö��
    }

    return TRUE;  // ����ö�ٴ���
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
// ƴ��·�����ļ���
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
    const std::wstring& successMessage = L"ע��ɹ�"
) {
    DWORD pid = GetPidByName(processName.c_str());

    HANDLE processId = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

    if (processId == 0) {
        AutoCloseMessageBox(L"�򿪽���ʧ�ܣ�");
        system("pause");
        return;
    }

    std::string exeDirectory = GetCurrentExecutableDirectory();
    std::string dllPath = PathJoin(exeDirectory, dllName);

    int dllLen = dllPath.length();
    LPVOID buffer = VirtualAllocEx(processId, 0, dllLen, MEM_COMMIT, PAGE_READWRITE);
    if (buffer == NULL) {
        AutoCloseMessageBox(L"�����ڴ�ʧ�ܣ�");
        return;
    }

    BOOL Success = WriteProcessMemory(processId, buffer, dllPath.c_str(), dllLen, NULL);
    if (Success == FALSE) {
        AutoCloseMessageBox(L"д���ڴ�ʧ�ܣ�");
        return;
    }

    HANDLE remoteThread = CreateRemoteThread(
        processId, NULL, 0,
        (LPTHREAD_START_ROUTINE)LoadLibraryA,
        buffer, 0, NULL
    );

    if (remoteThread == NULL) {
        AutoCloseMessageBox(L"����Զ���߳�ʧ�ܣ�");
        return;
    }

    AutoCloseMessageBox(successMessage);

    WaitForSingleObject(remoteThread, -1);

    VirtualFreeEx(processId, buffer, 0, MEM_RELEASE);
    CloseHandle(remoteThread);
    CloseHandle(processId);
}
void GetSystemFonts() {
    std::vector<std::wstring> fonts;

    LOGFONT lf = {0};
    lf.lfCharSet = DEFAULT_CHARSET;

    HDC hdc = GetDC(NULL);
    EnumFontFamiliesEx(
        hdc,
        &lf,
        [](const LOGFONT* lpelfe, const TEXTMETRIC* lpntme, DWORD FontType, LPARAM lParam) -> int {
            std::vector<std::wstring>* pFonts = reinterpret_cast<std::vector<std::wstring>*>(lParam);
            pFonts->push_back(lpelfe->lfFaceName);
            return 1;  // ����ö��
        },
        reinterpret_cast<LPARAM>(&fonts), 0
    );
    ReleaseDC(NULL, hdc);

    for (const auto& font : fonts) {
        std::wcout << font << std::endl;
    }
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
