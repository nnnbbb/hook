#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>
#include "Stop-Threads.h"
#include "speedhack.h"

// ���ݽ������õ�����PID
DWORD GetPidByName(const WCHAR* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            WCHAR* szName = pe32.szExeFile;
            // printf("szName = %ls\n", szName);
            if (wcscmp(szName, name) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return pid;
}

PVOID g_SaveAddr = NULL;

DWORD R4(HANDLE TargetProcess, LPCVOID TargetAddress) {
    DWORD RealSize = 0;
    DWORD ReadBuffer = 0;

    ReadProcessMemory(
        TargetProcess,
        TargetAddress,  // Ŀ����̵�ַ  //�˵�ַ����Ϸ�� ��Ŀ�����
        &ReadBuffer,    // ��д����ֵ�ĵ�ַ //��ǰ���̵ĵ�ַ
        4,              // д��4���ֽ� DWORD �޷������� sizeof(int)=4  sizeof(�ɻ�����)=4
        &RealSize       // ʵ��д����ֽ����� ����ĵ�ַ LPDWORD //DWORD*
    );
    return ReadBuffer;
}

VOID W4(HANDLE TargetProcess, LPVOID TargetAddress, DWORD* Value) {

    DWORD RealSize = 0;
    WriteProcessMemory(
        TargetProcess,
        TargetAddress,  // Ŀ����̵�ַ  //�˵�ַ����Ϸ�� ��Ŀ�����
        Value,          // ��д����ֵ�ĵ�ַ //��ǰ���̵ĵ�ַ
        4,              // д��4���ֽ� DWORD �޷������� sizeof(int)=4  sizeof(�ɻ�����)=4
        &RealSize
    );
}

DWORD WINAPI MainThread(LPVOID lpParam) {

    return 0;
}


DWORD WINAPI ThreadFunc() {
    DWORD Pid = GetPidByName(L"mxmain.dll");
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);

    while (true) {
        if (g_SaveAddr) {
            // [[[09CF0000]+18]+24]+1B1

            DWORD VirtualAddress1 = R4(hProcess, (PVOID)g_SaveAddr);
            DWORD VirtualAddress2 = R4(hProcess, (PVOID)(VirtualAddress1 + 0x18));
            DWORD VirtualAddress3 = R4(hProcess, (PVOID)(VirtualAddress2 + 0x24));
            DWORD VirtualAddress4 = VirtualAddress3 + 0x1B1;
            DWORD value = R4(hProcess, (PVOID)VirtualAddress4);  // �� 0x1000001 �ر� 0x1000000
            // printf("[*] value = 0x%x\n", value);

            if (value) {
                StopThreads();
            }
            DWORD v = 1;
            // W4(hProcess, (PVOID)VirtualAddress4, &v);
        }
        Sleep(2000);
    }


    return 0;
}

int main(int argc, char* argv[]) {
    // �õ�����PID
    DWORD Pid = GetPidByName(L"mxmain.dll");
    printf("Pid = %d\n", Pid);
    // �򿪵�ǰ����
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, Pid);
    if (hProcess == NULL) {
        return FALSE;
    }

    printf("[*] ��Ŀ����� \n");

    // �����ڴ�ռ�
    PVOID mFuncAddr = VirtualAllocEx(hProcess, NULL, 128, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    // �����ַ�ĵ�ַ
    PVOID SaveAddr = VirtualAllocEx(hProcess, NULL, 4, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    g_SaveAddr = SaveAddr;
    printf("[*] mFuncAddr %p\n", mFuncAddr);
    printf("[*] SaveAddr %p\n", SaveAddr);
    if (mFuncAddr == NULL) {
        return FALSE;
    }
    char JmpHookCode[] = {0xe9, 0x00, 0x00, 0x00, 0x00};
    char HookCode[] = {
        /*  0 */ 0x8B, 0x75, 0x08, 0x85, /* 3  */
        /*  4 */ 0xF6, 0x60, 0x9C, 0xB9, /* 7  */
        /*  8 */ 0x00, 0x7D, 0x19, 0x00, /* 11 */
        /* 12 */ 0x39, 0xCD, 0x75, 0x09, /* 15 */
        /* 16 */ 0x8B, 0x55, 0x08, 0x89, /* 19 */
        /* 20 */ 0x15,
        /* ��ʼ21 �����ַ�ĵ�ַ*/ 0x00, 0x00, 0x00, 0x00, /* ����24 �����ַ�ĵ�ַ*/
        /*25*/ 0x9D, 0x61, 0xE9,
        /* ��ʼ28 ����ȥ�ĵ�ַ  */ 0x00, 0x00, 0x00, 0x00 /* ����ȥ�ĵ�ַ*/
    };
    // �������ַ�ĵ�ַ �ĵ�ַд��hook code
    memcpy(&HookCode[21], &SaveAddr, 4);

    // ҪHook�Ļ���ַ
    DWORD BaseAddress = 0x00EC1814;

    //                   Ҫ��ת��Ŀ���ַ - ��ת����ʼ��ַ - (jmpָ��1���ֽ� + ��ַ4���ֽ�)
    DWORD JmpToOffset = (DWORD)mFuncAddr - BaseAddress - 5;
    printf("[*] JmpToOffset 0x%x\n", JmpToOffset);
    // ��ת��ȥ��ƫ�Ƽ���
    DWORD JmpBackOffset = (BaseAddress + 5) - ((DWORD)mFuncAddr + 27 + 5);

    // hook ��ת���Լ��Ĵ����ִ��
    memcpy(&JmpHookCode[1], &JmpToOffset, 4);
    WriteProcessMemory(hProcess, (char*)BaseAddress, JmpHookCode, sizeof(JmpHookCode), NULL);

    // д������
    memcpy(&HookCode[28], &JmpBackOffset, 4);
    WriteProcessMemory(hProcess, (char*)mFuncAddr, HookCode, sizeof(HookCode), NULL);

    // DWORD NumberOfByte;
    // WriteProcessMemory(hProcess, mFuncAddr, hookFn, 128, &NumberOfByte);
    // HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mFuncAddr, 0, 0, &NumberOfByte);
    HANDLE hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ThreadFunc, 0, 0, NULL);  // �����߳�
    WaitForSingleObject(hThread, INFINITE);

    system("pause");
    /*
    1. hook��ַjmp���Լ��Ĵ���ִ�� ebp==00197D00 �� [ebp+8] ��ֵ��ĳ������
    2. �����߳� ������������Ƿ񱻸�ֵ  ����ֵ�˾�ȡ�������򿪹رյĻ���ַ
    */
    return 0;
}