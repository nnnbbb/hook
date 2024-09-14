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


// CMainDialog 对话框

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
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
   public:
    CString m_content;
    afx_msg void OnBnClickedButton1();
    afx_msg void OnBnClickedButton2();
    afx_msg void OnBnClickedAroundNPC();
    afx_msg void OnBnClickedButton4();
};
