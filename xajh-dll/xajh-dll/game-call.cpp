#include "pch.h"
#include "game-call.h"
#include "log.hpp"
#include "utils.h"

void Speak(LPCWSTR content) {
    _asm {
		push 0
		push content;  // 喊话内容
		push 0x100  ;  // 喊话频道

        // ecx来源
		mov  eax, 0x008C23C0
		call eax
		mov ecx, eax;
        // call
		mov  eax, 0x00CC3010
		call eax
    }
}

// npc阵营
DWORD GetNpcGroup(DWORD npcStruct) {
    // [[[0x14C2050+24]+8C]+1A84]
    // 鼠标指针
    DWORD*** baseAddress = (DWORD***)(0x14C2050 + 0x24);
    DWORD mouseAddress = baseAddress[0][0x8C / 4][0x1A84 / 4];

    DWORD npcIdLow = *(DWORD*)(npcStruct + 0x140);
    DWORD npcIdHigh = *(DWORD*)(npcStruct + 0x144);

    DWORD tmp = 0;
    DWORD group = 0;

    _asm {
        push 0          ;  // 固定0 
        lea  eax, tmp
        push eax        ;  // 随意局部变量

        push npcStruct  ;  // npc对象结构  +140是npcid
        push npcIdHigh  ;  // 固定值 0x01000000
        push npcIdLow   ;  // ipcid
        mov  ecx, mouseAddress

        mov  eax, 0x00741800
        call eax
        mov  group, eax;
    }
    return group;
}
void ChoiceObject(DWORD64 npcId) {
    DWORD idLow = static_cast<DWORD>(npcId);         // 低 32 位
    DWORD idHigh = static_cast<DWORD>(npcId >> 32);  // 高 32 位

    _asm {
        // npc对象+140处偏移 npcid
        push idHigh
        push idLow  
        mov eax, 0x00C8F2A0
        call eax
        add esp,8
    }
}

void PrintRoleInformation() {
    ROLE_INFORMATION role = GetRoleInformation();
    printf("role x -> %f\n", role.x);
    printf("role y -> %f\n", role.y);
    printf("role hp -> %f\n", role.hp);
    printf("role mp -> %f\n", role.mp);
}

ROLE_INFORMATION GetRoleInformation() {
    // [[[0x14C0BF0]+0x24]+0x8C]+0x3c x
    // [[[0x14C0BF0]+0x24]+0x8C]+0x44 y
    // [[[0x14C0BF0]+0x24]+0x8C]+7d9
    DWORD roleBaseAddr = 0;
    _asm {
        mov eax, 0x008C23E0
        call eax
        mov roleBaseAddr, eax
    }
    ROLE_INFORMATION role = {};
    role.x = *(float*)(roleBaseAddr + 0x3c);
    role.y = *(float*)(roleBaseAddr + 0x44);
    role.hp = *(float*)(roleBaseAddr + 0x7d5);
    role.mp = *(float*)(roleBaseAddr + 0x7d9);
    return role;
}

void ChoiceMonster() {
    DWORD****** npcArrayBaseAddr = (DWORD******)(0x14C2050 + 0x24);
    DWORD* npcArrayStruct = npcArrayBaseAddr[0][0x94 / 4][0x4 / 4][0xC / 4][0x74 / 4];
    DWORD objectBeginAddr = npcArrayStruct[0x1C / 4];
    DWORD len = npcArrayStruct[0x24 / 4];
    ROLE_INFORMATION role = GetRoleInformation();

    double minDistance = INFINITY;
    DWORD64 choiceId = 0;
    for (size_t i = 0; i < len; i++) {
        DWORD npcStruct = *(DWORD*)(objectBeginAddr + i * 4);

        if (npcStruct == 0) {
            continue;
        }
        DWORD npcRealStruct = *(DWORD*)(npcStruct + 0x4);  // NPC结构体指针

        float npcX = *(float*)(npcRealStruct + 0x3c);
        float npcY = *(float*)(npcRealStruct + 0x44);

        double xminus = (double)role.x - (double)npcX;
        double yminus = (double)role.y - (double)npcY;
        double distance = sqrt(pow(xminus, 2) + pow(yminus, 2));

        // group 2不可攻击 9可以攻击
        DWORD group = GetNpcGroup(npcRealStruct);
        if (distance < minDistance && group == 9) {
            minDistance = distance;
            choiceId = *(DWORD64*)(npcRealStruct + 0x140);
        }
    }
    ChoiceObject(choiceId);
}
/*
当前选中对象血量
[[[[14C2050+24]+8]+0x324]+368]+290+1C
[[[[14C2050+24]+8]+0x324]+368] 如果有值表示选中目标
*/
u32 GetChoiceMonsterHPRate() {
    auto addr = (DWORD****)(0x14C2050 + 0x24);
    auto choiceObject = addr[0][0x8 / 4][0x324 / 4][0x368 / 4];
    if (choiceObject != 0) {
        auto hp = *(u32*)(choiceObject + 0x290 + 0x1C);
        return hp;
    }
    return -1;
}

bool Contains(const std::vector<u32>& vec, u32 value) {
    return std::find(vec.begin(), vec.end(), value) != vec.end();
}

std::vector<u32> g_vector;

void HookFunction() {
    _asm {
        pushad
        pushfd
    }
    u32 tmp = 0;
    _asm { 
        mov tmp, ebp;
    }
    u32 retAddr = *(u32*)(tmp + 4);

    if (Contains(g_vector, retAddr) == false) {
        g_vector.push_back(retAddr);
        printf("retAddr -> 0x%x\n", retAddr);
    }
    _asm {
        popfd;
        popad;
        mov eax, dword ptr ds:[ecx+8]
        mov eax, dword ptr ds:[eax+0x7A2]
    }
    if (
        retAddr == 0xc27d89 ||  // 任务访问
        retAddr == 0xc26e54 ||
        retAddr == 0xaafeb3 ||
        retAddr == 0xc298f1 ||
        retAddr == 0xaa984e ||
        retAddr == 0xc1c662
    ) {
        _asm mov eax, 1
    }

    // NOT_NULL(tmp);
    // printf("tmp -> 0x%x\n", tmp);
}

bool ApplyHook(LPVOID HookAddress, const char* HookCode, size_t codeLength) {
    DWORD Protect = 0;
    if (HookAddress != NULL) {
        if (VirtualProtect(HookAddress, codeLength, PAGE_EXECUTE_READWRITE, &Protect)) {
            memcpy(HookAddress, HookCode, codeLength);
            VirtualProtect(HookAddress, codeLength, Protect, &Protect);
            return true;
        }
    }

    return false;
}
bool isHook = false;
void Hook() {
    void (*funcPtr)() = HookFunction;
    unsigned char jmpHookCode[] = {0xe9, 0x00, 0x00, 0x00, 0x00};
    u32 fnAddrOffset = (u32)funcPtr - 5 - 0xC0F810;
    memcpy(&jmpHookCode[1], &fnAddrOffset, 4);
    ApplyHook((LPVOID)0xC0F810, (char*)jmpHookCode, sizeof(jmpHookCode));
    printf("addr -> %p\n", funcPtr);
    // for (unsigned char c : jmpHookCode) {
    //     std::cout << "0x" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(c) << ' ';
    // }
    // std::cout << std::endl;

    // 打印函数地址
    std::cout << "Address of myFunction: " << reinterpret_cast<void*>(funcPtr) << std::endl;
    printf("Hooking\n");
}
void Unhook() {
    unsigned char code[] = {0x8B, 0x41, 0x08, 0x8B, 0x80, 0xA2, 0x07, 0x00, 0x00};
    ApplyHook((LPVOID)0xC0F810, (char*)code, sizeof(code));
    printf("UnHooking\n");
}

void HookFn() {
    if (isHook == false) {
        Hook();
        isHook = true;
    } else {
        Unhook();
        isHook = false;
    }
}

void CustomFree(void* p) {
    // 加载 msvcr80.dll
    HMODULE hModule = LoadLibrary(L"msvcr80.dll");
    if (hModule == NULL) {
        // std::cerr << "Failed to load msvcr80.dll" << std::endl;
        return;
    }

    // 获取 free 函数的地址
    void (*customFree)(void*) = (void (*)(void*))GetProcAddress(hModule, "free");
    if (customFree == NULL) {
        // std::cerr << "Failed to get address of free function" << std::endl;
        FreeLibrary(hModule);
        return;
    }
    // printf("customFree -> 0x%p\n", p);

    customFree(p);

    // 释放 msvcr80.dll
    FreeLibrary(hModule);
}

void GetTaskList() {
    u32 taskList[4]{};
    _asm {
        // edx
        lea edx, taskList
        push edx;
        // ebx
        mov eax, 0x4AEFB0;
        call eax;
        mov eax, dword ptr ds:[eax+0x30];
        push eax;
        // ecx
        mov ecx, 0x014C2244;
        mov ecx, [ecx];
        // call
        mov eax, 0xC2D760;
        call eax
    }
    u32 taskStart = taskList[1];
    u32 taskEnd = taskList[2];
    printf("TaskStart -> 0x%x\n", taskStart);
    printf("TaskEnd -> 0x%x\n", taskEnd);
    CString str;

    while (taskStart < taskEnd) {
        u32 taskAddress = *(u32*)taskStart;
        u32 taskName = taskAddress + 8;
        u32 id = *(u32*)(taskAddress + 4);
        printf("Task id -> 0x%x ", id);
        PrintWChar(taskName);
        taskStart += 4;
    }
    // CustomFree((void*)taskStart);
    // ::free((void*)taskStart);
    ::operator delete((void*)taskStart);
}

void MouseSelected() {
    __try {
        FN_ADDRESS(GetTaskList);
        GetTaskList();
    } __except (1) {
        printf("err -> \n");
    }
    // [[[[[0x14C0BF0]+0x24]+0xC]+0x4]+0x40]
    DWORD***** mouseSelectBaseAddr = (DWORD*****)(0x14C0BF0);
    DWORD npcBaseAddr = mouseSelectBaseAddr[0][0x24 / 4][0xC / 4][0x4 / 4][0x40 / 4];
    if (npcBaseAddr != 0) {
        printf("npcBaseAddr -> %x\n", npcBaseAddr);
        float x = *(float*)(npcBaseAddr + 0x3C);
        float y = *(float*)(npcBaseAddr + 0x44);
        u32 hpRate = GetChoiceMonsterHPRate();
        printf("npc hp -> %d\n", hpRate);
    }
}

void AroundGoods() {
    u32 goodsBeginAddr = 0;
    u32 goodsArrLen = 0;
    _asm {
        mov eax, 0x004AEF90
        call eax
        mov eax, [eax+0x78]
        lea edi, [eax+0x14]
        // 数组起始地址
        mov eax, [edi+0x8]
        mov goodsBeginAddr, eax;
        // 数组长度
        mov eax, [edi + 0x14]
        mov goodsArrLen, eax
    }
    CString str;

    for (size_t i = 0; i < goodsArrLen; i++) {
        u32 linkDataAddr = *(u32*)(goodsBeginAddr + i * 4);
        if (linkDataAddr == 0) {
            continue;
        }
        u32 linkData = 0;
        do {
            linkData = *(u32*)(linkDataAddr + 0);
            u32 goodsDataAddr = *(u32*)(linkDataAddr + 4);
            u64 goodsID = *(u64*)(goodsDataAddr + 0x140);

            if (linkData != 0) {
                linkDataAddr = *(u32*)(linkDataAddr + 0);
            }
            u32 idLow = static_cast<u32>(goodsID);         // 低 32 位
            u32 idHigh = static_cast<u32>(goodsID >> 32);  // 高 32 位
            u32 name = *(u32*)(goodsDataAddr + 0x4B4);
            printf("goods idLow = 0x%X ", idLow);
            printf("goods addr = 0x%X ", goodsDataAddr);
            printf("name addr = 0x%X ", goodsDataAddr + 0x4B4);
            str.Format(L"name = %s \r\n", (wchar_t*)(name + 0xE));
            PrintCString(str);

        } while (linkData);
    }
    printf("goodsBeginAddr -> 0x%x\n", goodsBeginAddr);
    printf("goodsArrLen -> 0x%x\n", goodsArrLen);
}