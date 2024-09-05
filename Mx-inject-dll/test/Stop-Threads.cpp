#include "Stop-Threads.h"
#include "utils.hpp"

using namespace std;

#define CHECK_NOT_NULL(address)                          \
    do {                                                 \
        if (address == 0) {                              \
            throw std::runtime_error("address is NULL"); \
        }                                                \
    } while (0)


#pragma region ����
typedef enum _THREADINFOCLASS {
    ThreadBasicInformation,
    ThreadTimes,
    ThreadPriority,
    ThreadBasePriority,
    ThreadAffinityMask,
    ThreadImpersonationToken,
    ThreadDescriptorTableEntry,
    ThreadEnableAlignmentFaultFixup,
    ThreadEventPair_Reusable,
    ThreadQuerySetWin32StartAddress,
    ThreadZeroTlsCell,
    ThreadPerformanceCount,
    ThreadAmILastThread,
    ThreadIdealProcessor,
    ThreadPriorityBoost,
    ThreadSetTlsArrayAddress,
    ThreadIsIoPending,
    ThreadHideFromDebugger,
    ThreadBreakOnTermination,
    MaxThreadInfoClass
} THREADINFOCLASS;
typedef struct _CLIENT_ID {
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID;
typedef struct _THREAD_BASIC_INFORMATION {
    LONG ExitStatus;
    PVOID TebBaseAddress;
    CLIENT_ID ClientId;
    LONG AffinityMask;
    LONG Priority;
    LONG BasePriority;
} THREAD_BASIC_INFORMATION, *PTHREAD_BASIC_INFORMATION;
extern "C" LONG(__stdcall* ZwQueryInformationThread)(
    IN HANDLE ThreadHandle,
    IN THREADINFOCLASS ThreadInformationClass,
    OUT PVOID ThreadInformation,
    IN ULONG ThreadInformationLength,
    OUT PULONG ReturnLength OPTIONAL
) = NULL;
#pragma endregion


std::basic_string<WCHAR> extractFileName(const TCHAR* fullPath) {
    std::size_t len = _tcslen(fullPath);
    std::size_t found = std::basic_string<TCHAR>(fullPath, len).find_last_of(_T('\\'));

    if (found != std::basic_string<TCHAR>::npos) {
        return std::basic_string<TCHAR>(fullPath + found + 1);
    } else {
        return _T("File name not found in the path.");
    }
}
PROCESSENTRY32* FindByProcessName(const WCHAR* processName) {
    HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE) {
        std::cerr << "CreateToolhelp32Snapshot failed." << std::endl;
        return nullptr;
    }

    PROCESSENTRY32* process = new PROCESSENTRY32;
    process->dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, process)) {
        std::cerr << "Process32First failed." << std::endl;
        CloseHandle(hProcessSnap);
        delete process;
        return nullptr;
    }

    do {
        // std::wcout << L"szExeFile: " << process->szExeFile << '\n';

        if (wcscmp(process->szExeFile, processName) == 0) {
            CloseHandle(hProcessSnap);
            return process;
        }
    } while (Process32Next(hProcessSnap, process));

    CloseHandle(hProcessSnap);
    delete process;
    return nullptr;
}

BOOL MatchByThreadByNameAndStop(DWORD& th32ThreadID, const WCHAR* ModuleName) {
    HANDLE hThread = ::OpenThread(
        THREAD_ALL_ACCESS,  // ����Ȩ�ޣ�THREAD_ALL_ACCESS ������Ȩ��
        FALSE,              // �ɴ��̴߳����Ľ��̲��̳��̵߳ľ��
        th32ThreadID        // �߳� ID
    );
    if (hThread == NULL) {
        cout << "�̴߳�ʧ��" << endl;
    }


    // ��ȡ ntdll.dll ��ģ����
    HINSTANCE hNTDLL = ::GetModuleHandle(L"ntdll.dll");
    CHECK_NOT_NULL(hNTDLL);

    // �� ntdll.dll ��ȡ�� ZwQueryInformationThread
    (FARPROC&)ZwQueryInformationThread = ::GetProcAddress(hNTDLL, "ZwQueryInformationThread");

    // ��ȡ�߳���ڵ�ַ
    PVOID startaddr;  // ���������߳���ڵ�ַ
    ZwQueryInformationThread(
        hThread,                          // �߳̾��
        ThreadQuerySetWin32StartAddress,  // �߳���Ϣ���ͣ�ThreadQuerySetWin32StartAddress ���߳���ڵ�ַ
        &startaddr,                       // ָ�򻺳�����ָ��
        sizeof(startaddr),                // �������Ĵ�С
        NULL
    );

    // ��ȡ�߳�����ģ��
    THREAD_BASIC_INFORMATION tbi;  // _THREAD_BASIC_INFORMATION �ṹ�����
    TCHAR modname[MAX_PATH];       // ��������ģ��ȫ·��
    ZwQueryInformationThread(
        hThread,                 // �߳̾��
        ThreadBasicInformation,  // �߳���Ϣ���ͣ�ThreadBasicInformation ���̻߳�����Ϣ
        &tbi,                    // ָ�򻺳�����ָ��
        sizeof(tbi),             // �������Ĵ�С
        NULL
    );

    // �����ڵ�ַ�Ƿ�λ��ĳģ����
    K32GetMappedFileNameW(
        ::OpenProcess(                         // ���̾��
            PROCESS_ALL_ACCESS,                // ����Ȩ�ޣ�THREAD_ALL_ACCESS ������Ȩ��
            FALSE,                             // �ɴ��̴߳����Ľ��̲��̳��̵߳ľ��
            (DWORD)tbi.ClientId.UniqueProcess  // Ψһ���� ID
        ),
        startaddr,  // Ҫ���ĵ�ַ
        modname,    // ��������ģ������ָ��
        MAX_PATH    // ��������С
    );
    std::basic_string<WCHAR> baseName = extractFileName(modname);
    // std::wcout << L"lpBaseName: " << lpBaseName << '\n';
    CHECK_NOT_NULL(hThread);
    if (wcscmp(baseName.c_str(), ModuleName) == 0) {
        // std::wcout << L"th32ThreadID: " << th32ThreadID << L" hThread: " << hThread << L" baseName: " << baseName << '\n';

        SuspendThread(hThread);
        SuspendThread((HANDLE)th32ThreadID);
        return TRUE;

    } else {
        // ResumeThread(hThread);
        // ResumeThread((HANDLE)th32ThreadID);
    }
    return FALSE;
}
VOID StopThreadByDllName(PROCESSENTRY32W& process, const WCHAR* DllName) {


    HANDLE hThreadSnap = INVALID_HANDLE_VALUE;  // �߳̿��վ��
    THREADENTRY32 ThreadEntry32;                // �߳̿�����Ϣ

    // �����߳̿���
    hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE) {
        cout << "�����߳̿���ʧ��" << endl;
    }

    // Ϊ���շ����ڴ�ռ�
    ThreadEntry32.dwSize = sizeof(THREADENTRY32);

    // ��ȡ��һ���̵߳���Ϣ
    if (!Thread32First(hThreadSnap, &ThreadEntry32)) {
        cout << "�߳���Ϣ��ȡʧ��" << endl;
    }
    int i = 1;
    // �����߳�
    while (Thread32Next(hThreadSnap, &ThreadEntry32)) {
        if (ThreadEntry32.th32OwnerProcessID == process.th32ProcessID) {

            BOOL Match = MatchByThreadByNameAndStop(ThreadEntry32.th32ThreadID, DllName);
            HANDLE ThreadID = (HANDLE)ThreadEntry32.th32ThreadID;
            if (Match) {
                DWORD r = SuspendThread(ThreadID);
                std::wcout << DllName;
                std::cout << ThreadID << "�̱߳�����: " << r << " i = " << i << '\n';

                CONTEXT context;
                context.ContextFlags = CONTEXT_CONTROL;
                GetThreadContext(ThreadID, &context);
                BOOL IsSuspend = context.ContextFlags & CONTEXT_CONTROL;
                if (IsSuspend) {
                    // �߳�δ������
                    // std::wcout << L"�߳�δ������: " << r << '\n';
                }
                i++;
            }
        }
    }
}
VOID StopThreads() {
    PROCESSENTRY32* GenDumpProcess = FindByProcessName(L"GenDump.dll");
    PROCESSENTRY32 process = *(FindByProcessName(L"mxmain.dll"));
    log("Pid = ", process.th32ProcessID);

    if (GenDumpProcess) {
        system("taskkill /f /im GenDump.dll");
        printf("Close GenDump ProcessID = %d\n", GenDumpProcess->th32ProcessID);
        log("Close GenDump ProcessID = ", GenDumpProcess->th32ProcessID);
    }
    StopThreadByDllName(process, L"mx-inject-dll.dll");
    StopThreadByDllName(process, L"EPStableCommon.dll");
}