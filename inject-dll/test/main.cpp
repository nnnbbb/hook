#include <iostream>
#include <windows.h>
#include <string>
#include <tlhelp32.h>

// ���ݽ������õ�����PID
DWORD GetPidByName(const WCHAR* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            WCHAR* szName = pe32.szExeFile;
            //printf("szName = %ls\n", szName);
            if (wcscmp(szName, name) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return pid;
}

// ȫ����ը����
void FullScreenAttack() {
    __asm {
        pushad
        mov ecx,dword ptr ds:[0x00755EAC]
        mov ecx,[ecx+0x868]
        push 0xFFFFFFFF
        push 0  // ֲ��id  
        mov eax,0  // y
        push 0  // x
        push ecx
        mov esi,0x00418D70
        call esi
        popad
    }
}

// ע�����
BOOL InjectCode(DWORD dwProcId, LPVOID mFunc) {
    HANDLE hProcess, hThread;
    LPVOID mFuncAddr, ParamAddr;
    DWORD NumberOfByte;

    // �򿪵�ǰ����
    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcId);
    if (hProcess == NULL) {
        return FALSE;
    }

    printf("[*] ��Ŀ����� \n");

    // �����ڴ�ռ�
    mFuncAddr = VirtualAllocEx(hProcess, NULL, 128, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (mFuncAddr == NULL) {
        return FALSE;
    }

    printf("[*] �����ڴ�ռ� \n");

    // д������
    if (!WriteProcessMemory(hProcess, mFuncAddr, mFunc, 128, &NumberOfByte)) {
        return FALSE;
    }

    // ����Զ���߳�
    hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mFuncAddr, 0, 0, &NumberOfByte);
    if (hThread == NULL) {
        return FALSE;
    }

    printf("[*] ����Զ���߳� \n");

    // �ȴ��߳�ִ�н���
    WaitForSingleObject(hThread, INFINITE);

    // �ͷ��ڴ�ռ�
    // BOOL virtual_flag = VirtualFreeEx(hProcess, mFuncAddr, 128, MEM_RELEASE);
    // if (virtual_flag == FALSE) {
    //    return FALSE;
    //}

    // �رղ�����
    CloseHandle(hThread);
    CloseHandle(hProcess);

    return TRUE;
}
int PvzHook() {
    HWND Hwnd = FindWindowA(NULL, "Plants vs. Zombies");
    DWORD Pid = 0;
    if (Hwnd == NULL) {
        MessageBoxA(0, "��Ϸδ���У�", "��ʾ", MB_OK);
        return 0;
    }
    GetWindowThreadProcessId(Hwnd, &Pid);

    BOOL ref = InjectCode(Pid, FullScreenAttack);
    if (ref == TRUE) {
        printf("[+] ����ע����� \n");
    }
}
void hookFn() {
    printf("11");
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
    printf("[*] mFuncAddr %p\n", mFuncAddr);
    if (mFuncAddr == NULL) {
        return FALSE;
    }
    DWORD NumberOfByte;
    WriteProcessMemory(hProcess, mFuncAddr, hookFn, 128, &NumberOfByte);
    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)mFuncAddr, 0, 0, &NumberOfByte);
    //WaitForSingleObject(hThread, INFINITE);

    system("pause");
    /*
    1. hook��ַjmp���Լ��Ĵ���ִ�� ebp==00197D00 �� [ebp+8] ��ֵ��ĳ������
    2. �����߳� ������������Ƿ񱻸�ֵ  ����ֵ�˾�ȡ�������򿪹رյĻ���ַ
    */
    return 0;
}