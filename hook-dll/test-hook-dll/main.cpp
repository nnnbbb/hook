#include "Windows.h"
#include "iostream"

typedef BOOL (*InstallHook)();
typedef BOOL (*UnInstallHook)();
int main(int argc, char* argv[]) {
    HMODULE HookModule = LoadLibraryA("hook-dll.dll");
    if (HookModule == 0) {
        return 0;
    }
    InstallHook MyInstallHook = (InstallHook)GetProcAddress(HookModule, "InstallHook");
    UnInstallHook MyUnInstallHook = (UnInstallHook)GetProcAddress(HookModule, "UnInstallHook");

    if (MyInstallHook()) {
        printf("hook ³É¹¦£¡");
    }
    system("pause");
    MyUnInstallHook();

    return 0;
}