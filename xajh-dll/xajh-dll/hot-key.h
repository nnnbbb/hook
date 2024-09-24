#pragma once
#include "game-call.h"
constexpr auto CRTL_S_KEY_DOWN = 1;
constexpr auto F6_KEY_DOWN = 2;

VOID InitHotKey(HWND m_hWnd);

VOID UnInitHotKey(HWND m_hWnd);

VOID SetHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);