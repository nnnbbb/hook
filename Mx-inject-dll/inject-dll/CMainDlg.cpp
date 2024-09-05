// CMainDlg.cpp: 实现文件
//

#include "pch.h"
#include "inject-dll.h"
#include "CMainDlg.h"
#include <afxdialogex.h>
#include "pvz.h"

#define SHOW_CONSOLE

Pvz pvz;

// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_DIALOG1, pParent),
                                                 v_auto_get_sun(FALSE) {}

CMainDlg::~CMainDlg() {}

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


VOID Setup() {
    Speedhack::Setup();

#ifdef SHOW_CONSOLE
    SetupConsole();
#endif  // SHOW_CONSOLE
}

VOID Cleanup() {
    Speedhack::Detach();

#ifdef SHOW_CONSOLE
    DestroyConsole();
#endif  // SHOW_CONSOLE
}


BOOL CMainDlg::OnInitDialog() {
    // 调用基类函数以完成对话框的初始化
    CDialogEx::OnInitDialog();

    // 在对话框创建时执行的自定义代码
    // AfxMessageBox(_T("对话框已创建并初始化！"));

    // 其他初始化操作
    Setup();
    return TRUE;
}

void CMainDlg::OnDestroy() {
    // 在窗口销毁前执行一些操作
    // AfxMessageBox(_T("窗口正在销毁！"));
    Cleanup();
    // 执行默认的销毁操作
    CDialogEx::OnDestroy();
}

void CMainDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);

    // slider
    DDX_Control(pDX, IDC_SLIDER3, m_slider1);
    m_slider1.SetRange(1, 100);
    m_slider1.SetPos(1);

    DDX_Control(pDX, IDC_STATIC_TEXT1, m_staticText1);
    SetDlgItemText(IDC_STATIC_TEXT1, 0);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)

ON_WM_DESTROY()  // 绑定 OnDestroy 事件

ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CMainDlg::OnNMCustomdrawSlider3)

ON_EN_CHANGE(IDC_EDIT1, &CMainDlg::OnEnChangeEdit1)

ON_BN_CLICKED(IDC_BUTTON3, &CMainDlg::OnBnClickedButton3)

ON_WM_HSCROLL()

END_MESSAGE_MAP()

// CMainDlg 消息处理程序


void CMainDlg::OnNMCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult) {
    LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
    // TODO:  在此添加控件通知处理程序代码

    *pResult = 0;
}


void CMainDlg::OnEnChangeEdit1() {
    // TODO:  如果该控件是 RICHEDIT 控件，它将不
    // 发送此通知，除非重写 CDialogEx::OnInitDialog()
    // 函数并调用 CRichEditCtrl().SetEventMask()，
    // 同时将 ENM_CHANGE 标志“或”运算到掩码中。

    // TODO:  在此添加控件通知处理程序代码
}


void CMainDlg::OnBnClickedButton3() {
    // TODO: 在此添加控件通知处理程序代码
    CEdit* pBoxOne;
    pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT1);
    CString str;
    pBoxOne->GetWindowText(str);
    CT2A pszConvertedAnsiString(str);
    LPCSTR pszAnsiString = pszConvertedAnsiString;

    // MessageBoxA(0, pszAnsiString, "提示", MB_OK);
    double Speed = _tcstod(str, nullptr);

    Speedhack::SetSpeed(Speed);
}

void MessageBoxCString(CString cstr) {
    CT2A ConvertedAnsiString(cstr);
    LPCSTR AnsiString = ConvertedAnsiString;
    MessageBoxA(0, AnsiString, "提示", MB_OK);
}

void CMainDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
    // TODO: 在此添加消息处理程序代码和/或调用默认值
    CSliderCtrl* pSlider = reinterpret_cast<CSliderCtrl*>(pScrollBar);

    // Check which slider sent the notification
    if (pSlider == &m_slider1) {

        int pos = m_slider1.GetPos();
        CString cstr;
        // 转换为字符串
        cstr.Format(_T("%d"), pos);
        // 设置静态文本框显示的内容
        SetDlgItemText(IDC_STATIC_TEXT1, cstr);

        // MessageBoxCString(cstr);

        // 设置编辑框值
        CEdit* pBoxOne;
        pBoxOne = (CEdit*)GetDlgItem(IDC_EDIT1);
        pBoxOne->SetWindowText(cstr);

        Speedhack::SetSpeed(pos);
    }
}

// CDialogEx::OnHScroll(nSBCode, nPos, pScrollBar);
