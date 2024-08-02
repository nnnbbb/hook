#include <ntifs.h>
#include <ntddstor.h>
#include <mountdev.h>
#include <ntddvol.h>
#include <ntstrsafe.h>
#include <ntimage.h>

#define print(...) DbgPrint(__VA_ARGS__)

static UNICODE_STRING DeviceName;
static UNICODE_STRING Win32Device;

// 定义字符串
void DefineStringTest() {
    WCHAR wstr1[100] = L"51hook";
    UNICODE_STRING s1 = {0};
    s1.Buffer = wstr1;
    s1.Length = wcslen(wstr1) * sizeof(WCHAR);
    s1.MaximumLength = 100;
    print("%ws", s1.Buffer);

    DECLARE_CONST_UNICODE_STRING(s2, L"DECLARE_CONST_UNICODE_STRING");
    print("%ws", s2.Buffer);

    UNICODE_STRING s3 = RTL_CONSTANT_STRING(L"RTL_CONSTANT_STRING");
    print("%ws", s3.Buffer);

    UNICODE_STRING s4 = {0};
    RtlInitUnicodeString(&s4, L"RtlInitUnicodeString");
    print("%ws", s4.Buffer);
}

// 申请内存
void DefineMallocTest() {
    PWCHAR pwstr2 = (PWCHAR)ExAllocatePoolWithTag(NonPagedPool, 0x100, 'abcr');

    if (pwstr2) {
        RtlZeroMemory(pwstr2, 0x100);
        RtlCopyMemory(pwstr2, L"hahaha", 12);
        UNICODE_STRING ustr1 = {0};
        RtlInitUnicodeString(&ustr1, pwstr2);
        print("%wZ", &ustr1);
        ExFreePoolWithTag(pwstr2, 'abcr');
    }
}

// list_entry
typedef struct _TEST {
    ULONG m_dataA;
    ULONG m_dataB;
    LIST_ENTRY m_listEntry;
    ULONG m_dataC;
} TEST, *PTEST;

void ListEntryTest() {
    PLIST_ENTRY header = (PLIST_ENTRY)ExAllocatePool(NonPagedPool, sizeof(LIST_ENTRY));
    TEST test1 = {0};
    TEST test2 = {0};
    TEST test3 = {0};
    TEST test4 = {0};
    test1.m_dataA = 0x123;
    test2.m_dataA = 0x456;
    test3.m_dataA = 0x789;
    test4.m_dataA = 0x321;
    if (header) {
        RtlZeroMemory(header, sizeof(LIST_ENTRY));
        InitializeListHead(header);
        InsertTailList(header, &test1.m_listEntry);
        InsertTailList(header, &test2.m_listEntry);
        InsertTailList(header, &test3.m_listEntry);
        InsertTailList(header, &test4.m_listEntry);
        PLIST_ENTRY listEntry = header->Flink;
        while (listEntry != header) {
            PTEST test = CONTAINING_RECORD(listEntry, TEST, m_listEntry);
            print("%x\n", test->m_dataA);
            listEntry = listEntry->Flink;
        }
        RemoveHeadList(header);
        RemoveTailList(header);
        ExFreePool(header);
    }
}

// 写文件
void Log(const char* format, ...) {
    char message[1024] = "";
    va_list vl;
    va_start(vl, format);
    const int n = _vsnprintf(message, sizeof(message) / sizeof(char), format, vl);
    message[n] = '\0';
    va_end(vl);

    va_end(format);
    UNICODE_STRING FileName;
    OBJECT_ATTRIBUTES objAttr;
    // RtlInitUnicodeString(&FileName, L"\\DosDevices\\C:\\Test.log");
    RtlInitUnicodeString(&FileName, L"\\??\\C:\\Test.log");
    InitializeObjectAttributes(&objAttr, &FileName, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

    HANDLE handle;
    IO_STATUS_BLOCK ioStatusBlock;
    NTSTATUS ntstatus =
        ZwCreateFile(&handle, FILE_APPEND_DATA, &objAttr, &ioStatusBlock, NULL, FILE_ATTRIBUTE_NORMAL,
                     FILE_SHARE_WRITE | FILE_SHARE_READ, FILE_OPEN_IF, FILE_SYNCHRONOUS_IO_NONALERT, NULL, 0);
    if (NT_SUCCESS(ntstatus)) {
        size_t cb;
        ntstatus = RtlStringCbLengthA(message, sizeof(message), &cb);
        if (NT_SUCCESS(ntstatus)) {
            ZwWriteFile(handle, NULL, NULL, NULL, &ioStatusBlock, message, (ULONG)cb, NULL, NULL);
            print(message);
        }
        ZwClose(handle);
    }
}

static NTSTATUS DriverCreateClose(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    print("Driver Create Close");

    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_SUCCESS;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_SUCCESS;
}

static NTSTATUS DriverWrite(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS RetStatus = STATUS_SUCCESS;

    PIO_STACK_LOCATION pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    if (pIoStackIrp) {
        PVOID pInBuffer = (PVOID)Irp->AssociatedIrp.SystemBuffer;
        if (pInBuffer) {
            print("Receive r3 data %s\r\n", pInBuffer);

            // if (Hider::ProcessData(pInBuffer, pIoStackIrp->Parameters.Write.Length))
            //	Log("[TITANHIDE] HiderProcessData OK!\r\n");
            // else
            //{
            //	Log("[TITANHIDE] HiderProcessData failed...\r\n");
            //	RetStatus = STATUS_UNSUCCESSFUL;
            //}
        }
    } else {
        Log("[TITANHIDE] Invalid IRP stack pointer...\r\n");
        RetStatus = STATUS_UNSUCCESSFUL;
    }

    Irp->IoStatus.Status = RetStatus;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return RetStatus;
}

static NTSTATUS DriverRead(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    UNREFERENCED_PARAMETER(DeviceObject);
    NTSTATUS RetStatus = STATUS_SUCCESS;

    PIO_STACK_LOCATION pIoStackIrp = IoGetCurrentIrpStackLocation(Irp);
    const char* s = "hello r3!";

    if (pIoStackIrp) {
        PVOID Buffer = (PVOID)Irp->AssociatedIrp.SystemBuffer;
        if (Buffer) {
            size_t size = 0;
            memcpy(Buffer, s, strlen(s) + 1);
        }
    }

    Irp->IoStatus.Status = RetStatus;
    Irp->IoStatus.Information = strlen(s) + 1;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return RetStatus;
}

static NTSTATUS DriverDefaultHandler(IN PDEVICE_OBJECT DeviceObject, IN PIRP Irp) {
    PIO_STACK_LOCATION ioStack = IoGetCurrentIrpStackLocation(Irp);
    if (ioStack->MajorFunction == IRP_MJ_DEVICE_CONTROL) {
    }
    UNREFERENCED_PARAMETER(DeviceObject);
    Irp->IoStatus.Status = STATUS_NOT_SUPPORTED;
    Irp->IoStatus.Information = 0;
    IoCompleteRequest(Irp, IO_NO_INCREMENT);
    return STATUS_NOT_SUPPORTED;
}

void DriverUnload(IN PDRIVER_OBJECT DriverObject) {
    IoDeleteSymbolicLink(&Win32Device);
    IoDeleteDevice(DriverObject->DeviceObject);

    print("debug驱动卸载2");
}
extern "C" NTSTATUS DriverEntry(IN PDRIVER_OBJECT DriverObject, IN PUNICODE_STRING path) {
    print("debug驱动加载1");
    DriverObject->DriverUnload = DriverUnload;
    // DefineStringTest();
    // DefineMallocTest();
    // ListEntryTest();

    // set callback functions
    // 将其他函数设置为未支持
    for (unsigned int i = 0; i <= IRP_MJ_MAXIMUM_FUNCTION; i++) {
        DriverObject->MajorFunction[i] = DriverDefaultHandler;
    }

    DriverObject->MajorFunction[IRP_MJ_CREATE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_CLOSE] = DriverCreateClose;
    DriverObject->MajorFunction[IRP_MJ_WRITE] = DriverWrite;
    DriverObject->MajorFunction[IRP_MJ_READ] = DriverRead;

    // create io device
    PDEVICE_OBJECT DeviceObject = NULL;

    RtlInitUnicodeString(&DeviceName, L"\\Device\\MyDriver");
    RtlInitUnicodeString(&Win32Device, L"\\DosDevices\\MyDriver");

    NTSTATUS status;

    status = IoCreateDevice(DriverObject, 0, &DeviceName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE,
                            &DeviceObject);
    if (!NT_SUCCESS(status)) {
        Log("IoCreateDevice Error...\r\n");
        return status;
    }
    if (!DeviceObject) {
        Log("Unexpected I/O Error...\r\n");
        return STATUS_UNEXPECTED_IO_ERROR;
    }
    Log("Device %.*ws created successfully!\r\n", DeviceName.Length / sizeof(WCHAR), DeviceName.Buffer);

    // create symbolic link
    DeviceObject->Flags |= DO_BUFFERED_IO;
    DeviceObject->Flags &= (~DO_DEVICE_INITIALIZING);
    status = IoCreateSymbolicLink(&Win32Device, &DeviceName);
    if (!NT_SUCCESS(status)) {
        IoDeleteDevice(DeviceObject);
        Log("IoCreateSymbolicLink Error...\r\n");
        return status;
    }
    Log("Symbolic link %.*ws->%.*ws created!\r\n", Win32Device.Length / sizeof(WCHAR), Win32Device.Buffer,
        DeviceName.Length / sizeof(WCHAR), DeviceName.Buffer);

    // initialize hooking
    // Log("Hooks::Initialize() hooked %d functions\r\n");

    return STATUS_SUCCESS;
}
