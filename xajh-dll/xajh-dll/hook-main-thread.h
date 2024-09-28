#pragma once
#include <stdexcept>
#include "game-call.h"
#include "utils.h"

void HookMainThread();

void UnHookMainThread();

void SendMessageToWindow(_In_ UINT MsgCode);