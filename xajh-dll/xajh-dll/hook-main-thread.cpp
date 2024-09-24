#include "pch.h"
#include "hook-main-thread.h"

HWND GetMainThreadHwnd() {
    HWND windowHwnd = *(HWND*)(0x014C2000);
    return windowHwnd;
}

HWND mainThreadHandle = GetMainThreadHwnd();
LONG_PTR gamePreProc;

LRESULT CALLBACK GameProc(
    _In_ HWND hwnd,
    _In_ UINT uMsg,
    _In_ WPARAM wParam,
    _In_ LPARAM lParam
) {
    switch (uMsg) {
        case 0x400: {
            LPCWSTR content = L"zzz";
            Speak(content);
            ChoiceMonster();
            SetEvent((HANDLE)lParam);
            break;
        }
        default:
            break;
    }
    return ::CallWindowProcW((WNDPROC)gamePreProc, hwnd, uMsg, wParam, lParam);
}

void SendMessageToWindow(_In_ UINT MsgCode) {
    HANDLE eventHandle = CreateEvent(NULL, FALSE, FALSE, L"Event");
    // ID > WM_USER(0X400)
    ::PostMessage(mainThreadHandle, MsgCode, 0, (LPARAM)eventHandle);
    NOT_NULL(eventHandle);
    WaitForSingleObject(eventHandle, 3000);
    CloseHandle(eventHandle);
}

void HookMainThread() {
    printf("Hook Main Thread Msg\n");
    gamePreProc = ::SetWindowLongPtrW(mainThreadHandle, GWLP_WNDPROC, (LONG_PTR)GameProc);
}
void UnHookMainThread() {
    ::SetWindowLongPtrW(mainThreadHandle, GWLP_WNDPROC, (LONG_PTR)gamePreProc);
}
