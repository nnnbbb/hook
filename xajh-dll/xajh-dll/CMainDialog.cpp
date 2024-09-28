// CMainDialog.cpp: 实现文件
//

#include "pch.h"
#include "resource.h"
#include "CMainDialog.h"
#include "afxdialogex.h"

// CMainDialog 对话框
IMPLEMENT_DYNAMIC(CMainDialog, CDialogEx)


CMainDialog::CMainDialog(CWnd* pParent /*=nullptr*/)
    : CDialogEx(IDD_DIALOG1, pParent), m_content(_T("")) {
}
CMainDialog::~CMainDialog() {
}

BOOL CMainDialog::OnInitDialog() {
    CDialogEx::OnInitDialog();
    // MakeWindowTopMost();
    // 设置窗口的位置和大小
    int x = 1040;      // X坐标
    int y = 900;       // Y坐标
    int width = 600;   // 宽度
    int height = 400;  // 高度
    // SetWindowPos(NULL, x, y, width, height, SWP_NOZORDER | SWP_SHOWWINDOW);
    MoveWindow(x, y, width, height);

    HookMainThread();

    InitHotKey(m_hWnd);
    return TRUE;
}
void CMainDialog::OnDestroy() {
    CDialogEx::OnDestroy();
    UnHookMainThread();
    UnInitHotKey(m_hWnd);
}

void CMainDialog::OnHotKey(UINT nHotKeyId, UINT nKey1, UINT nKey2) {
    SetHotKey(nHotKeyId, nKey1, nKey2);

    CDialogEx::OnHotKey(nHotKeyId, nKey1, nKey2);
}

// 窗口置顶
void CMainDialog::MakeWindowTopMost() {
    SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
}

void CMainDialog::DoDataExchange(CDataExchange* pDX) {
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT1, m_content);
    DDX_Control(pDX, IDC_CHECK1, m_autoAttack);
    DDX_Control(pDX, IDC_CHECK_MSG, m_setMainThreadMsg);
}



BEGIN_MESSAGE_MAP(CMainDialog, CDialogEx)
// 添加 OnDestroy 消息映射
ON_WM_DESTROY()
// 添加热键消息映射
ON_WM_HOTKEY()
ON_BN_CLICKED(IDC_BUTTON1, &CMainDialog::OnBnClickedBagList)
ON_BN_CLICKED(IDC_BUTTON2, &CMainDialog::OnBnClickedSpeak)
ON_BN_CLICKED(IDC_BUTTON3, &CMainDialog::OnBnClickedAroundNPC)
ON_BN_CLICKED(IDC_BUTTON4, &CMainDialog::OnBnClickedWxList)
ON_BN_CLICKED(IDC_CHECK1, &CMainDialog::OnBnClickedAutoAttack)
ON_BN_CLICKED(IDC_CHECK_MSG, &CMainDialog::OnBnClickedHookMainThreadMsg)
ON_BN_CLICKED(IDC_BUTTON5, &CMainDialog::OnBnClickedTakeDrugs)
ON_BN_CLICKED(IDC_BUTTON6, &CMainDialog::OnBnClickedFindWay)
ON_BN_CLICKED(IDC_BUTTON7, &CMainDialog::OnBnClickedAroundGoods)
END_MESSAGE_MAP()

BOOL CMainDialog::PreTranslateMessage(MSG* pMsg) {
    // 检查是否是键盘按下事件
    // if (pMsg->message == WM_KEYDOWN) {
    //    // 检查是否是你想要的按键，比如 F1
    //    if (pMsg->wParam == VK_F6)  // VK_F1 对应 F1 键
    //    {
    //        OnF6KeyDown();  // 调用你的函数
    //        return TRUE;    // 消息已经处理
    //    }
    //    // 如果你想要检测组合键，比如 Ctrl+S
    //    if (pMsg->wParam == 'S' && GetKeyState(VK_CONTROL) < 0)  // 'S' 是 S 键，VK_CONTROL 是 Ctrl 键
    //    {
    //        OnF6KeyDown();  // 调用你的函数
    //        return TRUE;    // 消息已经处理
    //    }
    //}
    return CDialogEx::PreTranslateMessage(pMsg);  // 调用父类的 PreTranslateMessage
}


// CMainDialog 消息处理程序
std::string WStringToString(const std::wstring& wstr) {
    // 计算所需的缓冲区大小
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr);
    std::string str(size_needed, 0);

    // 执行转换
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), static_cast<int>(wstr.size()), &str[0], size_needed, nullptr, nullptr);

    return str;
}

void GetGoodsName(DWORD GoodsId, wchar_t* utf16_str, int length) {
    DWORD Tmp = 0;
    DWORD NameAddr;

    _asm {
        mov ecx, dword ptr ds : [0x14C0BF0]
        mov ecx, dword ptr ds : [ecx + 0x1A8]
        lea   eax, Tmp
        push  eax
        push  0
        push  GoodsId  // 物品id
        mov eax, 0x00C4A660
        call eax
        add eax, 4
        mov NameAddr, eax
    }
    memcpy(utf16_str, (PVOID)NameAddr, length);
}

void PrintHex(const wchar_t* utf16_str, int length) {
    for (int i = 0; i < length; i++) {
        if (utf16_str[i] != 0) {
            std::cout << "0x" << std::hex << std::setw(4) << std::setfill('0') << utf16_str[i] << " ";
        }
    }
    std::cout << std::endl;
}

// 设置控制台输出编码
void SetConsoleUTF8() {
    int res = _setmode(_fileno(stdout), _O_U8TEXT);
}

// 获取控制台输出编码
void GetConsoleCodePage() {
    UINT codePage = GetConsoleOutputCP();

    std::cout << "Console Output Code Page: " << codePage << std::endl;
}


void CMainDialog::OnBnClickedBagList() {
    int length = 20;
    wchar_t utf16_str[20] = {};
    // [[[[[[[14C0BF0]+ 24] + 90] + 8] + 14] + 2c] + eax * 4]
    DWORD******* BaseAddr = (DWORD*******)0x14C0BF0;
    DWORD* BagArrAddr = BaseAddr[0][0x24 / 4][0x90 / 4][0x8 / 4][0x14 / 4][0x2c / 4];
    for (size_t i = 0; i < 200; i++) {
        if (BagArrAddr[i] == 0) {
            continue;
        }
        DWORD Number = *(DWORD*)(BagArrAddr[i] + 0x70);
        DWORD Id = *(DWORD*)(BagArrAddr[i] + 0xc);

        GetGoodsName(Id, utf16_str, length);

        // 输出utf16_str数组的内容
        // std::wcout << L"UTF-16 String Content: " << std::endl;
        // GetGoodsName(0x14786, utf16_str, length);
        PrintHex(utf16_str, length);

        CString str;
        str.Format(L"%s(%d)\r\n", utf16_str, Number);
        PrintCString(str);

        // AfxMessageBox(str);
        m_content.Append(str);
        UpdateData(FALSE);
    }
}


void CMainDialog::OnBnClickedSpeak() {
    UpdateData(TRUE);
    LPCWSTR content = m_content.GetString();
    Speak(content);
}


void CMainDialog::OnBnClickedAroundNPC() {
    // [[[[[14C2050+24]+94]+4]+C]+74]
    // [[[[[[[[14C2050+24]+94]+4]+C]+74]+1c] i * 4] + 4]
    DWORD****** npcArrayBaseAddr = (DWORD******)(0x14C2050 + 0x24);
    DWORD* npcArrayStruct = npcArrayBaseAddr[0][0x94 / 4][0x4 / 4][0xC / 4][0x74 / 4];
    DWORD objectBeginAddr = npcArrayStruct[0x1C / 4];
    DWORD len = npcArrayStruct[0x24 / 4];
    CString str;
    for (size_t i = 0; i < len; i++) {
        DWORD npcStruct = *(DWORD*)(objectBeginAddr + i * 4);

        if (npcStruct == 0) {
            continue;
        }
        DWORD npcRealStruct = *(DWORD*)(npcStruct + 0x4);  // NPC结构体指针

        DWORD npcId = *(DWORD*)(npcRealStruct + 0x140);
        // 获取虚函数表
        DWORD virtualFunTable = ((DWORD*)npcRealStruct)[0];
        // 获取对象函数地址
        DWORD getNameFunc = *(DWORD*)(virtualFunTable + 0x88);
        const wchar_t* npcName = L"";
        // LPWSTR npcName = const_cast<LPWSTR>(L"");
        _asm {
             mov ecx, npcRealStruct
             mov eax, getNameFunc
             call eax
             mov npcName, eax
        }
        // group 2不可攻击 9可以攻击
        DWORD group = GetNpcGroup(npcRealStruct);
        if (group == 9) {
            m_content.Append(L"可攻击对象");
        }
        str.Format(L"addr = 0x%X NPCID为: %d(%X) Name: %s\r\n", npcRealStruct, npcId, npcId, npcName);
        m_content.Append(str);
        // PrintCString(str);
    }
    UpdateData(FALSE);
}

DWORD GetSkillName(DWORD skillId) {
    DWORD skillObj = 0;
    _asm {
        // ecx
        mov eax, 0x004AEFB0;
        call eax;
        mov eax, dword ptr ds:[eax+0x28];
        mov ecx, eax;
        // call
        push skillId;
        mov eax, 0x00535760;
        call eax;
        mov skillObj, eax;
    }
    if (skillObj != 0) {
        DWORD skillName = *(DWORD*)(skillObj + 0x20);
        return skillName;
    }
    return 0;
}
void CMainDialog::OnBnClickedWxList() {
    DWORD mpObj = 0;
    _asm {
        mov eax, 0x004AEFA0;
        call eax;
        mov eax, dword ptr ds:[eax+0x7A6];
        mov ecx, dword ptr ds:[0x014C0BF0];
        mov ecx, dword ptr ds:[ecx+0x20C];
        push eax;
        mov eax, 0x00809030;
        call eax;
        mov mpObj, eax;
    }
    CString str;
    // 武学列表
    DWORD wxArrary = mpObj + 0x620;
    for (size_t i = 0; i < 5; i++) {
        DWORD wxAddress = wxArrary + i * 4;
        if (wxAddress == 0) {
            continue;
        }
        DWORD wxID = *(DWORD*)wxAddress;
        DWORD flag = 0;
        DWORD wxBigObj = 0;
        _asm {
            lea ecx, flag;
            push ecx;
            push 0;
            push wxID;
            // ecx
            mov ecx, 0x014C0BF0;
            mov ecx, [ecx];
            mov ecx, dword ptr ds:[ecx+0x1A8];
            mov eax, 0x00C4A660;
            call eax;
            mov wxBigObj, eax;
        }
        if (flag != 0x61) {
            continue;
        }
        DWORD wxName = wxBigObj + 0x4;
        DWORD skillArrary = wxBigObj + 0x164;
        if (wxID != 0) {
            str.Format(L"~~~~~~Name = %s wxID = 0x%08X~~~~~\r\n", (wchar_t*)wxName, wxID);
            m_content.Append(str);
        }

        for (size_t j = 0; j < 0x20; j++) {
            DWORD* skill = (DWORD*)(skillArrary + j * 8);
            DWORD skillId = *skill;
            if (skillId != 0) {
                DWORD skillName = GetSkillName(skillId);
                str.Format(L"wxName = %s ", (wchar_t*)wxName);
                str.Format(L"skillName = %s, 当前武学技能ID为:%X\r\n", (wchar_t*)skillName, skillId);
                m_content.Append(str);
            }
        }
    }

    UpdateData(FALSE);
}


HANDLE attackThreadHandle = 0;
void CMainDialog::OnBnClickedAutoAttack() {
    if (m_autoAttack.GetCheck()) {
        attackThreadHandle = ::CreateThread(NULL, 0, autoAttackThread, 0, 0, NULL);
    } else {
        // 使用 TerminateThread 函数来强制终止一个线程，但这个函数会绕过线程的清理操作，可能导致资源泄漏和死锁等问题。
        TerminateThread(attackThreadHandle, 0);
        CloseHandle(attackThreadHandle);
    }
}

void CMainDialog::OnBnClickedHookMainThreadMsg() {
}

void CMainDialog::OnBnClickedFindWay() {
    typedef struct _CORRDINATES {
        float x;
        float z;
        float y;
    } CORRDINATES, *PCORRDINATES;

    CORRDINATES coordinates = {};
    coordinates.x = -150.0;
    coordinates.y = 50.0;
    coordinates.z = 0;
    DWORD mapId = 0x44;
    _asm { 
        push 0
        push 1
        push 0x3F4CCCCD
        push 0

        // 坐标
        lea eax, coordinates
        push eax;

        // ecx
        mov eax, 0x4AEFA0
        call eax
        mov ecx, dword ptr ds:[eax+0x274]

        // 地图id
        push mapId 
        push 0
        push 0
        push 0
        push 0
        mov eax, 0x730500
        call eax
    }
}
void CMainDialog::OnBnClickedTakeDrugs() {
    SendMessageToWindow(0X400);
}


void CMainDialog::OnBnClickedAroundGoods() {
    AroundGoods();
}
