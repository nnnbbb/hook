// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include "speedhack.h"

DWORD WINAPI MainThread(LPVOID lpParam)
{
    Speedhack::Setup();
    Speedhack::SetSpeed(5.0); //slows down to half speed

    return 0;
}

BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
        CreateThread(0, 0x1000, &MainThread, NULL, 0, NULL);
    }

    return TRUE;
}
