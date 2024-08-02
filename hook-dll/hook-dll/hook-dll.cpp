#include "hook-dll.h"

HINSTANCE g_hInstance = NULL;
HHOOK g_hook = NULL;

void log(const char* format, ...) {
    CHAR strinfo[1024] = {0};  // 可根据需要调整数组大小
    va_list args;
    va_start(args, format);
    vsnprintf_s(strinfo, _countof(strinfo), _TRUNCATE, format, args);
    va_end(args);
    OutputDebugStringA(strinfo);
}

LRESULT CALLBACK KeyboardProc(_In_ int code, _In_ WPARAM wParam, _In_ LPARAM lParam) {
    if (code == HC_ACTION && (lParam & 0x80000000) != 0) {
        BYTE KeyState[256]{0};
        if (GetKeyboardState(KeyState)) {
            LONG keyinfo = lParam;
            UINT keyCode = (keyinfo >> 16) & 0x00ff;
            WCHAR wkeyCode = 0;
            ToAscii((UINT)wParam, keyCode, KeyState, (LPWORD)&wkeyCode, 0);

            log("51hook_%c", wkeyCode);
            return 0;
        }
    }
    return CallNextHookEx(g_hook, code, wParam, lParam);
}

BOOL InstallHook() {
    HHOOK g_hook = SetWindowsHookExA(WH_KEYBOARD, KeyboardProc, g_hInstance, 0);

    if (g_hook) {
        return TRUE;
    }

    return FALSE;
}

BOOL UnInstallHook() {
    return UnhookWindowsHookEx(g_hook);
}

BOOL DllMain(HINSTANCE hInstance, DWORD dwCallReason, LPVOID lpReseverd) {

    if (DLL_PROCESS_ATTACH == dwCallReason) {
        g_hInstance = hInstance;
    }
    return TRUE;
}