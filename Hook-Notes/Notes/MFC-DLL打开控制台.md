https://github.com/microsoft/terminal/issues/14416#issuecomment-1322256935
```cpp
FILE* stdinNew = nullptr;
FILE* stdoutNew = nullptr;
FILE* stderrNew = nullptr;

VOID SetupConsole() {  // 分配控制台
    AllocConsole();
    freopen_s(&stdinNew, "CONIN$", "r+", stdin);
    freopen_s(&stdoutNew, "CONOUT$", "w+", stdout);
    freopen_s(&stderrNew, "CONOUT$", "w+", stderr);
    printf("Hello World！\n");
}

VOID DestroyConsole() {
    if (stdinNew) {
        fclose(stdinNew);
    }
    if (stdoutNew) {
        fclose(stdoutNew);
    }

    if (stderrNew) {
        fclose(stderrNew);
    }
    FreeConsole();
}
```
