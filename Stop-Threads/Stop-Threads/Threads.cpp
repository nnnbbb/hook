#include <Windows.h>
#include <processsnapshot.h>
#include <stdio.h>
#include <string>
#include <tlhelp32.h>
#include <iostream>
#include <psapi.h>
#include <tchar.h>

using namespace std;

#define CHECK_NOT_NULL(address)                          \
    do {                                                 \
        if (address == 0) {                              \
            throw std::runtime_error("address is NULL"); \
        }                                                \
    } while (0)


#pragma region 依赖
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
}THREADINFOCLASS;
typedef struct _CLIENT_ID {
	HANDLE UniqueProcess;
	HANDLE UniqueThread;
}CLIENT_ID;
typedef struct _THREAD_BASIC_INFORMATION {
	LONG ExitStatus;
	PVOID TebBaseAddress;
	CLIENT_ID ClientId;
	LONG AffinityMask;
	LONG Priority;
	LONG BasePriority;
}THREAD_BASIC_INFORMATION, * PTHREAD_BASIC_INFORMATION;
extern "C" LONG(__stdcall * ZwQueryInformationThread)(
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
	}
	else {
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
		if (wcscmp(process->szExeFile, processName) == 0) {
			CloseHandle(hProcessSnap);
			return process;
		}
	} while (Process32Next(hProcessSnap, process));

	CloseHandle(hProcessSnap);
	delete process;
	return nullptr;
}
VOID MyThread() {

}
BOOL MatchByThreadByName(DWORD& th32ThreadID, const WCHAR* ModuleName) {
	HANDLE hThread = ::OpenThread(
		THREAD_ALL_ACCESS,		// 访问权限，THREAD_ALL_ACCESS ：所有权限
		FALSE,					// 由此线程创建的进程不继承线程的句柄
		th32ThreadID		    // 线程 ID
	);
	if (hThread == NULL) { cout << "线程打开失败" << endl; }


	// 获取 ntdll.dll 的模块句柄
	HINSTANCE hNTDLL = ::GetModuleHandle(L"ntdll.dll");
	CHECK_NOT_NULL(hNTDLL);

	// 从 ntdll.dll 中取出 ZwQueryInformationThread
	(FARPROC&)ZwQueryInformationThread = ::GetProcAddress(hNTDLL, "ZwQueryInformationThread");

	// 获取线程入口地址
	PVOID startaddr;						// 用来接收线程入口地址
	ZwQueryInformationThread(
		hThread,							// 线程句柄
		ThreadQuerySetWin32StartAddress,	// 线程信息类型，ThreadQuerySetWin32StartAddress ：线程入口地址
		&startaddr,							// 指向缓冲区的指针
		sizeof(startaddr),					// 缓冲区的大小
		NULL
	);

	// 获取线程所在模块
	THREAD_BASIC_INFORMATION tbi;			// _THREAD_BASIC_INFORMATION 结构体对象
	TCHAR modname[MAX_PATH];				// 用来接收模块全路径
	ZwQueryInformationThread(
		hThread,							// 线程句柄
		ThreadBasicInformation,				// 线程信息类型，ThreadBasicInformation ：线程基本信息
		&tbi,								// 指向缓冲区的指针
		sizeof(tbi),						// 缓冲区的大小
		NULL
	);

	// 检查入口地址是否位于某模块中
	K32GetMappedFileNameW(
		::OpenProcess(						// 进程句柄
			PROCESS_ALL_ACCESS,									// 访问权限，THREAD_ALL_ACCESS ：所有权限
			FALSE,												// 由此线程创建的进程不继承线程的句柄
			(DWORD)tbi.ClientId.UniqueProcess					// 唯一进程 ID
		),
		startaddr,							// 要检查的地址
		modname,							// 用来接收模块名的指针
		MAX_PATH							// 缓冲区大小
	);
	std::basic_string<WCHAR> baseName = extractFileName(modname);
	//std::wcout << L"lpBaseName: " << lpBaseName << '\n';
	CHECK_NOT_NULL(hThread);
	if (wcscmp(baseName.c_str(), ModuleName) == 0) {
		//std::wcout << L"th32ThreadID: " << th32ThreadID << L" hThread: " << hThread << L" baseName: " << baseName << '\n';

		SuspendThread(hThread);

	}
	else {
		ResumeThread(hThread);

	}
	return FALSE;

}
VOID CheckLoop(PROCESSENTRY32W& process) {


	HANDLE hThreadSnap = INVALID_HANDLE_VALUE;			// 线程快照句柄 
	THREADENTRY32 ThreadEntry32;				// 线程快照信息

	// 创建线程快照
	hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (hThreadSnap == INVALID_HANDLE_VALUE) { cout << "创建线程快照失败" << endl; }

	// 为快照分派内存空间
	ThreadEntry32.dwSize = sizeof(THREADENTRY32);

	// 获取第一个线程的信息
	if (!Thread32First(hThreadSnap, &ThreadEntry32)) { cout << "线程信息获取失败" << endl; }

	// 遍历线程
	while (Thread32Next(hThreadSnap, &ThreadEntry32)) {
		if (ThreadEntry32.th32OwnerProcessID == process.th32ProcessID) {
			// 打开线程
			//HANDLE hThread = ::OpenThread(
			//	THREAD_ALL_ACCESS,		// 访问权限，THREAD_ALL_ACCESS ：所有权限
			//	FALSE,					// 由此线程创建的进程不继承线程的句柄
			//	ThreadEntry32.th32ThreadID		// 线程 ID
			//);
			//if (hThread == NULL) { cout << "线程打开失败" << endl; }

			// 将区域设置设置为从操作系统获取的ANSI代码页
			setlocale(LC_ALL, ".ACP");
			BOOL Match = MatchByThreadByName(ThreadEntry32.th32ThreadID, L"EPStableCommon.dll");
			if (Match) {
				HANDLE ThreadID = (HANDLE)ThreadEntry32.th32ThreadID;
				DWORD r = SuspendThread(ThreadID);

				CONTEXT context;
				context.ContextFlags = CONTEXT_CONTROL;
				GetThreadContext(ThreadID, &context);
				if (context.ContextFlags & CONTEXT_CONTROL) {
					// 线程未被挂起
					//std::wcout << L"线程未被挂起: " << r << '\n';

				}
				//ResumeThread(hThread);
			}

		}

	}

}

int main(int argc, const char* argv[]) {
	PROCESSENTRY32 process = *(FindByProcessName(L"mxmain.dll"));						// 进程快照信息

	while (true) {
		CheckLoop(process);
		Sleep(1000);
		//cout << "dd" << endl;
	}


}
