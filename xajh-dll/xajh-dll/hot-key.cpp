#include "pch.h"
#include "hot-key.h"


VOID InitHotKey(HWND m_hWnd) {
    // 注册 Ctrl+S 作为全局热键
    // 参数：窗口句柄，热键 ID，组合键，虚拟键
    // RegisterHotKey(m_hWnd, CRTL_S_KEY_DOWN, MOD_CONTROL, 'S');  // 注册 Ctrl + S

    // RegisterHotKey(m_hWnd, F6_KEY_DOWN, 0, VK_F1);  // 注册 F1 键
    RegisterHotKey(m_hWnd, F6_KEY_DOWN, 0, VK_F6);  // 注册 F6 键
}

VOID UnInitHotKey(HWND m_hWnd) {
    // 注销注册的热键
    // UnregisterHotKey(m_hWnd, CRTL_S_KEY_DOWN);  // 释放 Ctrl+S
    UnregisterHotKey(m_hWnd, F6_KEY_DOWN);  // 释放 F1
}

VOID SetHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2) {
    switch (nHotKeyId) {
        case CRTL_S_KEY_DOWN:  // Ctrl+S 热键
            MouseSelected();
            break;
        case F6_KEY_DOWN:  // F6 热键
            MouseSelected();
            break;
    }
}