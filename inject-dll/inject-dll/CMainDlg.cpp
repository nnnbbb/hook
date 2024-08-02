// CMainDlg.cpp: 实现文件
//

#include "pch.h"
#include "inject-dll.h"
#include "CMainDlg.h"
#include <afxdialogex.h>
#include "pvz.h"

Pvz pvz;

// CMainDlg 对话框

IMPLEMENT_DYNAMIC(CMainDlg, CDialogEx)

CMainDlg::CMainDlg(CWnd* pParent /*=nullptr*/) : CDialogEx(IDD_DIALOG1, pParent), v_auto_get_sun(FALSE) {}

CMainDlg::~CMainDlg() {}

void InitCombo_box(CComboBox& combo_box) {
    combo_box.AddString(_T("豌豆射手"));
    combo_box.AddString(_T("向日葵"));
    combo_box.AddString(_T("樱桃炸弹"));
    combo_box.AddString(_T("坚果"));
    combo_box.AddString(_T("土豆地雷"));
    combo_box.AddString(_T("寒冰射手"));
    combo_box.AddString(_T("大嘴花"));
    combo_box.AddString(_T("双重射手"));
    combo_box.AddString(_T("小喷菇"));
    combo_box.AddString(_T("阳光菇"));
    combo_box.AddString(_T("大喷菇"));
    combo_box.AddString(_T("咬咬碑"));
    combo_box.AddString(_T("迷惑菇"));
    combo_box.AddString(_T("胆小菇"));
    combo_box.AddString(_T("冰川菇"));
    combo_box.AddString(_T("末日菇"));
    combo_box.AddString(_T("莲叶"));
    combo_box.AddString(_T("窝瓜"));
    combo_box.AddString(_T("三重射手"));
    combo_box.AddString(_T("缠绕水草"));
    combo_box.AddString(_T("火爆辣椒"));
    combo_box.AddString(_T("地刺"));
    combo_box.AddString(_T("火炬树桩"));
    combo_box.AddString(_T("高坚果"));
    combo_box.AddString(_T("水兵菇"));
    combo_box.AddString(_T("路灯花"));
    combo_box.AddString(_T("仙人掌"));
    combo_box.AddString(_T("三叶草"));
    combo_box.AddString(_T("双向射手"));
    combo_box.AddString(_T("星星果"));
    combo_box.AddString(_T("南瓜头"));
    combo_box.AddString(_T("磁力菇"));
    combo_box.AddString(_T("卷心菜投手"));
    combo_box.AddString(_T("花盆"));
    combo_box.AddString(_T("玉米投手"));
    combo_box.AddString(_T("咖啡豆"));
    combo_box.AddString(_T("大蒜"));
    combo_box.AddString(_T("罗卜伞"));
    combo_box.AddString(_T("金盏花"));
    combo_box.AddString(_T("西瓜投手"));
    combo_box.AddString(_T("机枪射手"));
    combo_box.AddString(_T("双子向日葵"));
    combo_box.AddString(_T("多嘴小蘑菇"));
    combo_box.AddString(_T("猫尾草"));
    combo_box.AddString(_T("冰西瓜"));
    combo_box.AddString(_T("吸金菇"));
    combo_box.AddString(_T("钢地刺"));
    combo_box.AddString(_T("玉米加农炮"));
    combo_box.AddString(_T("模仿者(变身时消失)"));
    combo_box.AddString(_T("红坚果"));
    combo_box.AddString(_T("巨坚果"));
    combo_box.AddString(_T("幼苗"));
    combo_box.AddString(_T("左向双重射手"));
    combo_box.AddString(_T("(改消失)"));
    combo_box.AddString(_T("转向(点消失)"));
    combo_box.AddString(_T("消坑(点消失)"));
    combo_box.AddString(_T("太阳牌(点消失)"));
    combo_box.AddString(_T("钻石牌(点消失)"));
    combo_box.AddString(_T("潜水僵尸(花苗?)"));
    combo_box.AddString(_T("奖杯(僵尸头?)"));
    combo_box.AddString(_T("普通僵尸(花苗)"));
    combo_box.AddString(_T("路障僵尸"));
    combo_box.AddString(_T("撑杆僵尸"));
    combo_box.AddString(_T("铁桶僵尸"));
    combo_box.AddString(_T("梯子僵尸"));
    combo_box.AddString(_T("矿工僵尸"));
    combo_box.AddString(_T("飞贼僵尸"));
    combo_box.AddString(_T("橄榄球僵尸"));
    combo_box.AddString(_T("气球僵尸"));
    combo_box.AddString(_T("铁网门僵尸"));
    combo_box.AddString(_T("雪橇车僵尸"));
    combo_box.AddString(_T("蹦蹦僵尸"));
    combo_box.AddString(_T("舞者僵尸"));
    combo_box.AddString(_T("巨人僵尸"));
    combo_box.AddString(_T("小鬼僵尸"));
}

void CMainDlg::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_CHECK1, v_auto_get_sun);
    DDX_Check(pDX, IDC_CHECK2, v_card_clickable);

    DDX_Control(pDX, IDC_COMBO1, v_combo_box1);
    InitCombo_box(v_combo_box1);
    v_combo_box1.SetCurSel(7);
}

BEGIN_MESSAGE_MAP(CMainDlg, CDialogEx)
ON_BN_CLICKED(IDC_CHECK1, &CMainDlg::OnBnClickedCheck1)
ON_BN_CLICKED(IDC_BUTTON1, &CMainDlg::OnBnClickedButton1)
ON_BN_CLICKED(IDC_CHECK2, &CMainDlg::OnBnClickedCheck2)

ON_BN_CLICKED(IDC_BUTTON2, &CMainDlg::OnBnClickedButton2)
ON_CBN_SELCHANGE(IDC_COMBO1, &CMainDlg::OnCbnSelchangeCombo1)
END_MESSAGE_MAP()

// CMainDlg 消息处理程序

void CMainDlg::OnBnClickedCheck1() {
    UpdateData(TRUE);
    BOOL result = FALSE;
    if (v_auto_get_sun == 1) {
        result = pvz.OpenAutoGetSun();
        MessageBoxA(0, "打开自动收集阳光", "提示", MB_OK);
    } else {
        result = pvz.CloseAutoGetSun();
        MessageBoxA(0, "关闭自动收集阳光", "提示", MB_OK);
    }
    if (result == FALSE) {
        MessageBoxA(0, "操作失败", "提示", MB_OK);
    }
}

void CMainDlg::OnBnClickedButton1() {
    BOOL result = pvz.AddSun(1000);

    if (result == TRUE) {
        MessageBoxA(0, "操作成功", "提示", MB_OK);
    } else {
        MessageBoxA(0, "操作失败", "提示", MB_OK);
    }
}

void CMainDlg::OnBnClickedCheck2() {
    UpdateData(TRUE);
    BOOL result = FALSE;
    if (v_card_clickable == 1) {
        result = pvz.OpenCardClickable();
        MessageBoxA(0, "打开所有卡片可点击", "提示", MB_OK);
    } else {
        result = pvz.CloseCardClickable();
        MessageBoxA(0, "关闭所有卡片可点击", "提示", MB_OK);
    }
    if (result == FALSE) {
        MessageBoxA(0, "操作失败", "提示", MB_OK);
    }
}

void CMainDlg::OnBnClickedButton2() {
    int PlantId = v_combo_box1.GetCurSel();
    pvz.FullScreenPlant(PlantId);
}

void CMainDlg::OnCbnSelchangeCombo1() {
    // TODO: 在此添加控件通知处理程序代码
}
