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

static KEVENT kEvent;
static PETHREAD pThread;
VOID MyThread(PVOID pParam) {
    pThread = PsGetCurrentThread();
    for (size_t i = 0; i < 10; i++) {
        LARGE_INTEGER timeOut = RtlConvertLongToLargeInteger(-10 * 1000 * 1000);
        NTSTATUS status = KeWaitForSingleObject(&kEvent, Executive, KernelMode, FALSE, &timeOut);

        if (STATUS_TIMEOUT == status) {
            print("�ȴ���ʱ������ִ�У�%d\r\n", i);
            continue;
        } else {
            print("���ܵ��źţ��߳�ִ����ֹ������\r\n");
            PsTerminateSystemThread(STATUS_SUCCESS);
        }
    }
}

void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    UNREFERENCED_PARAMETER(DriverObject);
    KeSetEvent(&kEvent, 0, FALSE);
    NTSTATUS status = KeWaitForSingleObject(pThread, Executive, KernelMode, FALSE, NULL);
    if (NT_SUCCESS(status)) {
        print("�߳��Ѿ���ֹ��ִ��unload\r\n");
    }
    print("debug����ж��\r\n");
}

extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    UNREFERENCED_PARAMETER(path);
    DriverObject->DriverUnload = DriverUnload;

    KeInitializeEvent(&kEvent, SynchronizationEvent, FALSE);
    HANDLE hThread = 0;
    NTSTATUS status = PsCreateSystemThread(&hThread, GENERIC_ALL, NULL, NULL, NULL, MyThread, NULL);

    if (NT_SUCCESS(status)) {
        print("�̴߳����ɹ���\r\n");
    }
    NtClose(hThread);
    return STATUS_SUCCESS;
}