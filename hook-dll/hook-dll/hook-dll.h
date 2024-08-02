#pragma once
#include "Windows.h"
#include "iostream"

extern "C" _declspec(dllexport) BOOL InstallHook();
extern "C" _declspec(dllexport) BOOL UnInstallHook();