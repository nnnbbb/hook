// xajh-dll.cpp: 定义 DLL 的初始化例程。
//

#include "pch.h"
#include "framework.h"
#include "CMainDialog.h"
#include "xajh-dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
// TODO:  如果此 DLL 相对于 MFC DLL 是动态链接的，
//		则从此 DLL 导出的任何调入
//		MFC 的函数必须将 AFX_MANAGE_STATE 宏添加到
//		该函数的最前面。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此处为普通函数体
//		}
//
//		此宏先于任何 MFC 调用
//		出现在每个函数中十分重要。  这意味着
//		它必须作为以下项中的第一个语句:
//		出现，甚至先于所有对象变量声明，
//		这是因为它们的构造函数可能生成 MFC
//		DLL 调用。
//
//		有关其他详细信息，
//		请参阅 MFC 技术说明 33 和 58。
//

// CxajhdllApp

BEGIN_MESSAGE_MAP(CxajhdllApp, CWinApp)
END_MESSAGE_MAP()


// CxajhdllApp 构造

CxajhdllApp::CxajhdllApp() {
    // TODO:  在此处添加构造代码，
    // 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的 CxajhdllApp 对象
CxajhdllApp theApp;
CMainDialog* MainDialog{};

FILE* stdinNew = nullptr;
FILE* stdoutNew = nullptr;
FILE* stderrNew = nullptr;

VOID SetupConsole() {  // 分配控制台
    AllocConsole();
    freopen_s(&stdinNew, "CONIN$", "r+", stdin);
    freopen_s(&stdoutNew, "CONOUT$", "w+", stdout);
    freopen_s(&stderrNew, "CONOUT$", "w+", stderr);
    SetConsoleTitle(L"Xajh Console");
    HWND hConsole = GetConsoleWindow();
    if (hConsole) {
        // 获取当前窗口样式
        LONG style = GetWindowLong(hConsole, GWL_STYLE);
        // 移除关闭按钮样式
        style &= ~WS_SYSMENU;
        // 设置新的窗口样式
        SetWindowLong(hConsole, GWL_STYLE, style);
    }
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

VOID ReleaseWindow() {
    delete MainDialog;
    DestroyConsole();
    FreeLibraryAndExitThread(theApp.m_hInstance, 65533);
}

DWORD WINAPI ShowWindow(LPVOID lparam) {
    MainDialog->DoModal();
    ReleaseWindow();
    return 0;
}

void tasm(const ZyanU8 data[], ZyanU32* runtime_address, ZyanU32* error_address, size_t data_size) {
    ZyanUSize offset = 0;
    ZydisDisassembledInstruction instruction;

    char output[100];  // Assuming the output won't exceed 100 characters

    while (offset < data_size) {
        if (ZYAN_SUCCESS(ZydisDisassembleIntel(
                ZYDIS_MACHINE_MODE_LONG_COMPAT_32,
                *runtime_address,
                data + offset,
                data_size - offset,
                &instruction
            ))) {
            // Construct the output string
            sprintf_s(
                output,
                sizeof(output),
                "%08X   %s",
                (ZyanU32)*runtime_address,
                instruction.text
            );

            // Check if runtime_address equals a specific address
            if (*runtime_address == *error_address) {
                strcat_s(output, sizeof(output), " <====== Error Address");
            }

            strcat_s(output, sizeof(output), "\n");
            printf("%s", output);

            offset += instruction.info.length;
            *runtime_address += instruction.info.length;
        } else {
            printf("Failed to disassemble the instruction at offset %zu\n", offset);
            break;
        }
    }
}


long __stdcall callback(_EXCEPTION_POINTERS* Exception) {
    void* exception_address = Exception->ExceptionRecord->ExceptionAddress;
    void* start_address = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(exception_address) - 16);
    printf("Exception_address = %p\n\n", (ZyanU32*)exception_address);

    ZyanU8 data[32];
    memcpy(data, start_address, 32);

    ZyanU32 runtime_address = reinterpret_cast<ZyanU32>(start_address);
    size_t data_size = sizeof(data) / sizeof(data[0]);

    tasm(data, &runtime_address, (ZyanU32*)&exception_address, data_size);

    system("pause");
    ReleaseWindow();
    return EXCEPTION_CONTINUE_EXECUTION;
}
// CxajhdllApp 初始化
BOOL CxajhdllApp::InitInstance() {
    CWinApp::InitInstance();

    MainDialog = new CMainDialog;
    this->m_pMainWnd = MainDialog;
    SetupConsole();
    ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShowWindow, 0, 0, NULL);

    // 设置自定义的未捕获异常过滤器
    SetUnhandledExceptionFilter(callback);

    return TRUE;
}
