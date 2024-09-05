// inject-dll.cpp: 定义 DLL 的初始化例程。
//

#include "framework.h"
#include "pch.h"

#include "CMainDlg.h"
#include "inject-dll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CinjectdllApp

CMainDlg* MainDlg{};

BEGIN_MESSAGE_MAP(CinjectdllApp, CWinApp)
END_MESSAGE_MAP()

// CinjectdllApp 构造


CinjectdllApp::CinjectdllApp() {
}

CinjectdllApp theApp;
HANDLE ShowWindowThread;



DWORD WINAPI ShowWindow(LPVOID lparam) {
    MainDlg->DoModal();
    delete MainDlg;
    FreeLibraryAndExitThread(theApp.m_hInstance, 65533);
    return 0;
}

// CinjectdllApp 初始化
BOOL CinjectdllApp::InitInstance() {
    CWinApp::InitInstance();
    MainDlg = new CMainDlg;
    this->m_pMainWnd = MainDlg;
    ShowWindowThread = ::CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ShowWindow, 0, 0, NULL);
    return TRUE;
}
