#include "_global.h"

#define RANDOM_SEED_INIT 0x3AF84E05
static ULONG RandomSeed = RANDOM_SEED_INIT;

#define print(...) DbgPrint(__VA_ARGS__)

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

ULONG RtlNextRandom(ULONG Min, ULONG Max) // [Min,Max)
{
    if(RandomSeed == RANDOM_SEED_INIT)  // One-time seed initialisation. It doesn't have to be good, just not the same every time
        RandomSeed = static_cast<ULONG>(__rdtsc());

    // NB: In user mode, the correct scale for RtlUniform/RtlRandom/RtlRandomEx is different on Win 10+:
    // Scale = (RtlNtMajorVersion() >= 10 ? MAXUINT32 : MAXINT32) / (Max - Min);
    // The KM versions seem to have been unaffected by this change, at least up until RS3.
    // If this ever starts returning values >= Max, try the above scale instead
    const ULONG Scale = static_cast<ULONG>(MAXINT32) / (Max - Min);
    return RtlRandomEx(&RandomSeed) / Scale + Min;
}

ULONG GetPoolTag()
{
    constexpr ULONG PoolTags[] =
    {
        ' prI', // Allocated IRP packets
        '+prI', // I/O verifier allocated IRP packets
        'eliF', // File objects
        'atuM', // Mutant objects
        'sFtN', // ntfs.sys!StrucSup.c
        'ameS', // Semaphore objects
        'RwtE', // Etw KM RegEntry
        'nevE', // Event objects
        ' daV', // Mm virtual address descriptors
        'sdaV', // Mm virtual address descriptors (short)
        'aCmM', // Mm control areas for mapped files
        '  oI', // I/O manager
        'tiaW', // WaitCompletion Packets
        'eSmM', // Mm secured VAD allocation
        'CPLA', // ALPC port objects
        'GwtE', // ETW GUID
        ' ldM', // Memory Descriptor Lists
        'erhT', // Thread objects
        'cScC', // Cache Manager Shared Cache Map
        'KgxD', // Vista display driver support
    };

    constexpr ULONG NumPoolTags = ARRAYSIZE(PoolTags);
    const ULONG Index = RtlNextRandom(0, NumPoolTags);
    NT_ASSERT(Index <= NumPoolTags - 1);
    return PoolTags[Index];
}

void* RtlAllocateMemory(bool InZeroMemory, SIZE_T InSize)
{
    void* Result = ExAllocatePoolWithTag(NonPagedPool, InSize, GetPoolTag());
    if(InZeroMemory && (Result != NULL))
        RtlZeroMemory(Result, InSize);
    return Result;
}

void RtlFreeMemory(void* InPointer)
{
    ExFreePool(InPointer);
}

NTSTATUS RtlSuperCopyMemory(IN VOID UNALIGNED* Destination, IN CONST VOID UNALIGNED* Source, IN ULONG Length)
{
    const KIRQL Irql = KeRaiseIrqlToDpcLevel();

    PMDL Mdl = IoAllocateMdl(Destination, Length, 0, 0, nullptr);
    if(Mdl == nullptr)
    {
        KeLowerIrql(Irql);
        return STATUS_NO_MEMORY;
    }

    MmBuildMdlForNonPagedPool(Mdl);

    // Hack: prevent bugcheck from Driver Verifier and possible future versions of Windows
#pragma prefast(push)
#pragma prefast(disable:__WARNING_MODIFYING_MDL, "Trust me I'm a scientist")
    const CSHORT OriginalMdlFlags = Mdl->MdlFlags;
    Mdl->MdlFlags |= MDL_PAGES_LOCKED;
    Mdl->MdlFlags &= ~MDL_SOURCE_IS_NONPAGED_POOL;

    // Map pages and do the copy
    const PVOID Mapped = MmMapLockedPagesSpecifyCache(Mdl, KernelMode, MmCached, nullptr, FALSE, HighPagePriority);
    if(Mapped == nullptr)
    {
        Mdl->MdlFlags = OriginalMdlFlags;
        IoFreeMdl(Mdl);
        KeLowerIrql(Irql);
        return STATUS_NONE_MAPPED;
    }

    RtlCopyMemory(Mapped, Source, Length);

    MmUnmapLockedPages(Mapped, Mdl);
    Mdl->MdlFlags = OriginalMdlFlags;
#pragma prefast(pop)
    IoFreeMdl(Mdl);
    KeLowerIrql(Irql);

    return STATUS_SUCCESS;
}
