#pragma once
#include "afxdialogex.h"
#include <iostream>
#include <locale>
#include <codecvt>
#include <windows.h>
#include <iomanip>
#include <io.h>
#include <fcntl.h>
#include <atlconv.h>  // 包含ATL的转换宏
#include "game-call.h"
#include "attack-monster.h"
#include "auto-attack.h"
#include "Zydis/Zydis.h"
#include "game-call.h"
#include "hook-main-thread.h"
#include "hot-key.h"

// CMainDialog 对话框
extern LONG_PTR gamePreProc;

class CMainDialog : public CDialogEx {
    DECLARE_DYNAMIC(CMainDialog)

   public:
    CMainDialog(CWnd* pParent = nullptr);  // 标准构造函数
    virtual ~CMainDialog();

    virtual BOOL OnInitDialog();

    void MakeWindowTopMost();


// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

   protected:
    // DDX/DDV 支持
    virtual void DoDataExchange(CDataExchange* pDX);
    afx_msg void OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2);

    // 程序内快捷键 重载 PreTranslateMessage
    virtual BOOL PreTranslateMessage(MSG* pMsg);

    // 添加 OnDestroy 方法的声明
    afx_msg void OnDestroy();

    DECLARE_MESSAGE_MAP()
   public:
    CString m_content;
    CButton m_autoAttack;
    CButton m_setMainThreadMsg;
    afx_msg void OnBnClickedBagList();
    afx_msg void OnBnClickedSpeak();
    afx_msg void OnBnClickedAroundNPC();
    afx_msg void OnBnClickedWxList();
    afx_msg void OnBnClickedAutoAttack();
    afx_msg void OnBnClickedHookMainThreadMsg();
    afx_msg void OnBnClickedTakeDrugs();
    afx_msg void OnBnClickedFindWay();
    afx_msg void OnBnClickedAroundGoods();
};
