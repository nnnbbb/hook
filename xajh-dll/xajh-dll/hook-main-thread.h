#pragma once
#include <stdexcept>
#include "game-call.h"
#include "utils.hpp"

void HookMainThread();

void UnHookMainThread();

void SendMessageToWindow(_In_ UINT MsgCode);