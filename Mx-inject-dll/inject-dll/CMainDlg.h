#pragma once

#include "afxdialogex.h"

// CMainDlg 对话框

class CMainDlg : public CDialogEx {
    DECLARE_DYNAMIC(CMainDlg)

   public:
    CMainDlg(CWnd* pParent = nullptr);  // 标准构造函数
    virtual ~CMainDlg();
    
    virtual BOOL OnInitDialog();  // 声明 OnInitDialog 函数
    afx_msg void OnDestroy();     // 声明 OnDestroy 函数


// 对话框数据
#ifdef AFX_DESIGN_TIME
    enum { IDD = IDD_DIALOG1 };
#endif

   protected:
    virtual void DoDataExchange(CDataExchange* pDX);  // DDX/DDV 支持

    DECLARE_MESSAGE_MAP()
   public:
    BOOL v_auto_get_sun;
    BOOL v_card_clickable;

    CSliderCtrl m_slider1;
    afx_msg void OnNMCustomdrawSlider3(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEnChangeEdit1();
    afx_msg void OnBnClickedButton3();
    afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
    CStatic m_staticText1;
};
