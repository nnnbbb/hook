#include <ntifs.h>
#include <ntstrsafe.h>

void print(const char* format, ...) {
    char message[1024] = "";
    va_list vl;
    va_start(vl, format);
    const int n = _vsnprintf(message, sizeof(message) / sizeof(char), format, vl);
    message[n] = '\0';
    va_end(vl);

    va_end(format);
    DbgPrint(message);
}

// 上一个节点
static PLIST_ENTRY g_processBlink;
//下一个节点
// static PLIST_ENTRY g_processFlink;
static ULONG g_eprocess;

void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    PLIST_ENTRY processLinks = (PLIST_ENTRY)(g_eprocess + 0xe8);
    // g_processBlink->Flink = processLinks;
    // g_processFlink->Blink = processLinks;
    InsertHeadList(g_processBlink, processLinks);
    // processLinks->Blink = g_processBlink;
    // processLinks->Flink = g_processFlink;
    
    *(UCHAR*)(g_eprocess + 0x3a6) = 0;
    print("debug驱动卸载");
}

// 进程保护
void ProtectProcessByName(const char* processName) {
    ULONG currentProcessName = 0;
    ULONG currentProcess = (ULONG)PsGetCurrentProcess();

    PLIST_ENTRY processList = (PLIST_ENTRY)(currentProcess + 0xe8);  // ActiveProcessLinks
    PLIST_ENTRY nextList = processList->Flink;

    while (processList != nextList) {
        ULONG eprocess = (ULONG)nextList - 0xe8;
        currentProcessName = eprocess + 0x1ac;  // ImageFileName
        if (strcmp((char*)currentProcessName, processName) == 0) {
            print("Found process %s \r\n", currentProcess);
            // +0x3a6 Protection
            *(UCHAR*)(eprocess + 0x3a6) = 0x72;
            break;
        }
        print("%s\r\n", processName);
        nextList = nextList->Flink;
    }
}

ULONG FindByEprocessByName(const char* processName) {
    ULONG result = 0;
    ULONG currentProcess = (ULONG)PsGetCurrentProcess();

    PLIST_ENTRY processList = (PLIST_ENTRY)(currentProcess + 0xe8);  // ActiveProcessLinks +0xe8
    PLIST_ENTRY nextList = processList->Flink;

    while (processList != nextList) {
        ULONG eprocess = (ULONG)nextList - 0xe8;
        PUCHAR currentProcessName = (PUCHAR)(eprocess + 0x1ac);  // ImageFileName +0x1ac
        if (strcmp((char*)currentProcessName, processName) == 0) {
            print("Found process %s \r\n", currentProcessName);
            // +0x3a6 Protection
            // *(UCHAR*)(eprocess + 0x3a6) = 0x72;
            return eprocess;
        }
        nextList = nextList->Flink;
    }
    return result;
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    UNREFERENCED_PARAMETER(path);
    DriverObject->DriverUnload = DriverUnload;

    // ULONG processName = 0;
    // ULONG currentProcess = (ULONG)PsGetCurrentProcess();

    // PLIST_ENTRY processList = (PLIST_ENTRY)(currentProcess + 0xe8);  // ActiveProcessLinks
    // PLIST_ENTRY nextList = processList->Flink;

    // while (processList != nextList) {
    //    ULONG eprocess = (ULONG)nextList - 0xe8;
    //    processName = eprocess + 0x1ac;  // ImageFileName
    //    if (strcmp((char*)processName, "dbgview.exe") == 0) {
    //        print("Found process ");
    //        print("%s \r\n", processName);
    //        // +0x3a6 Protection
    //        *(UCHAR*)(eprocess + 0x3a6) = 0x72;
    //        break;
    //    }
    //    print("%s\r\n", processName);
    //    nextList = nextList->Flink;
    // }
    ProtectProcessByName("dbgview.exe");
    ULONG eprocess = FindByEprocessByName("dbgview.exe");
    if (eprocess) {
        PLIST_ENTRY processEntryList = (PLIST_ENTRY)(eprocess + 0xe8);

        // save
        g_processBlink = processEntryList->Blink;
        // g_processFlink = processLinks->Flink;
        g_eprocess = eprocess;

        // 上一个节点的下一个节点指向 ProcessLinks 的下一个节点
        // processLinks->Blink->Flink = processLinks->Flink;
        // 下一个节点的上一个节点指向 ProcessLinks 的上一个节点
        // processLinks->Flink->Blink = processLinks->Blink;
        // 断链隐藏
        RemoveEntryList(processEntryList);
    }
    return STATUS_SUCCESS;
}