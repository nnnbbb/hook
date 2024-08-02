   +0x000 Pcb              : _KPROCESS
   +0x0e0 ProcessLock      : _EX_PUSH_LOCK
   +0x0e4 UniqueProcessId  : Ptr32 Void
   +0x0e8 ActiveProcessLinks : _LIST_ENTRY
   +0x0f0 RundownProtect   : _EX_RUNDOWN_REF
   +0x0f4 VdmObjects       : Ptr32 Void
   +0x0f8 Flags2           : Uint4B
   +0x0f8 JobNotReallyActive : Pos 0, 1 Bit
   +0x0f8 AccountingFolded : Pos 1, 1 Bit
   +0x0f8 NewProcessReported : Pos 2, 1 Bit
   +0x0f8 ExitProcessReported : Pos 3, 1 Bit
   +0x0f8 ReportCommitChanges : Pos 4, 1 Bit
   +0x0f8 LastReportMemory : Pos 5, 1 Bit
   +0x0f8 ForceWakeCharge  : Pos 6, 1 Bit
   +0x0f8 CrossSessionCreate : Pos 7, 1 Bit
   +0x0f8 NeedsHandleRundown : Pos 8, 1 Bit
   +0x0f8 RefTraceEnabled  : Pos 9, 1 Bit
   +0x0f8 PicoCreated      : Pos 10, 1 Bit
   +0x0f8 EmptyJobEvaluated : Pos 11, 1 Bit
   +0x0f8 DefaultPagePriority : Pos 12, 3 Bits
   +0x0f8 PrimaryTokenFrozen : Pos 15, 1 Bit
   +0x0f8 ProcessVerifierTarget : Pos 16, 1 Bit
   +0x0f8 RestrictSetThreadContext : Pos 17, 1 Bit
   +0x0f8 AffinityPermanent : Pos 18, 1 Bit
   +0x0f8 AffinityUpdateEnable : Pos 19, 1 Bit
   +0x0f8 PropagateNode    : Pos 20, 1 Bit
   +0x0f8 ExplicitAffinity : Pos 21, 1 Bit
   +0x0f8 ProcessExecutionState : Pos 22, 2 Bits
   +0x0f8 EnableReadVmLogging : Pos 24, 1 Bit
   +0x0f8 EnableWriteVmLogging : Pos 25, 1 Bit
   +0x0f8 FatalAccessTerminationRequested : Pos 26, 1 Bit
   +0x0f8 DisableSystemAllowedCpuSet : Pos 27, 1 Bit
   +0x0f8 ProcessStateChangeRequest : Pos 28, 2 Bits
   +0x0f8 ProcessStateChangeInProgress : Pos 30, 1 Bit
   +0x0f8 InPrivate        : Pos 31, 1 Bit
   +0x0fc Flags            : Uint4B
   +0x0fc CreateReported   : Pos 0, 1 Bit
   +0x0fc NoDebugInherit   : Pos 1, 1 Bit
   +0x0fc ProcessExiting   : Pos 2, 1 Bit
   +0x0fc ProcessDelete    : Pos 3, 1 Bit
   +0x0fc ManageExecutableMemoryWrites : Pos 4, 1 Bit
   +0x0fc VmDeleted        : Pos 5, 1 Bit
   +0x0fc OutswapEnabled   : Pos 6, 1 Bit
   +0x0fc Outswapped       : Pos 7, 1 Bit
   +0x0fc FailFastOnCommitFail : Pos 8, 1 Bit
   +0x0fc Wow64VaSpace4Gb  : Pos 9, 1 Bit
   +0x0fc AddressSpaceInitialized : Pos 10, 2 Bits
   +0x0fc SetTimerResolution : Pos 12, 1 Bit
   +0x0fc BreakOnTermination : Pos 13, 1 Bit
   +0x0fc DeprioritizeViews : Pos 14, 1 Bit
   +0x0fc WriteWatch       : Pos 15, 1 Bit
   +0x0fc ProcessInSession : Pos 16, 1 Bit
   +0x0fc OverrideAddressSpace : Pos 17, 1 Bit
   +0x0fc HasAddressSpace  : Pos 18, 1 Bit
   +0x0fc LaunchPrefetched : Pos 19, 1 Bit
   +0x0fc Background       : Pos 20, 1 Bit
   +0x0fc VmTopDown        : Pos 21, 1 Bit
   +0x0fc ImageNotifyDone  : Pos 22, 1 Bit
   +0x0fc PdeUpdateNeeded  : Pos 23, 1 Bit
   +0x0fc VdmAllowed       : Pos 24, 1 Bit
   +0x0fc ProcessRundown   : Pos 25, 1 Bit
   +0x0fc ProcessInserted  : Pos 26, 1 Bit
   +0x0fc DefaultIoPriority : Pos 27, 3 Bits
   +0x0fc ProcessSelfDelete : Pos 30, 1 Bit
   +0x0fc SetTimerResolutionLink : Pos 31, 1 Bit
   +0x100 CreateTime       : _LARGE_INTEGER
   +0x108 ProcessQuotaUsage : [2] Uint4B
   +0x110 ProcessQuotaPeak : [2] Uint4B
   +0x118 PeakVirtualSize  : Uint4B
   +0x11c VirtualSize      : Uint4B
   +0x120 SessionProcessLinks : _LIST_ENTRY
   +0x128 ExceptionPortData : Ptr32 Void
   +0x128 ExceptionPortValue : Uint4B
   +0x128 ExceptionPortState : Pos 0, 3 Bits
   +0x12c Token            : _EX_FAST_REF
   +0x130 MmReserved       : Uint4B
   +0x134 AddressCreationLock : _EX_PUSH_LOCK
   +0x138 PageTableCommitmentLock : _EX_PUSH_LOCK
   +0x13c RotateInProgress : Ptr32 _ETHREAD
   +0x140 ForkInProgress   : Ptr32 _ETHREAD
   +0x144 CommitChargeJob  : Ptr32 _EJOB
   +0x148 CloneRoot        : _RTL_AVL_TREE
   +0x14c NumberOfPrivatePages : Uint4B
   +0x150 NumberOfLockedPages : Uint4B
   +0x154 Win32Process     : Ptr32 Void
   +0x158 Job              : Ptr32 _EJOB
   +0x15c SectionObject    : Ptr32 Void
   +0x160 SectionBaseAddress : Ptr32 Void
   +0x164 Cookie           : Uint4B
   +0x168 WorkingSetWatch  : Ptr32 _PAGEFAULT_HISTORY
   +0x16c Win32WindowStation : Ptr32 Void
   +0x170 InheritedFromUniqueProcessId : Ptr32 Void
   +0x174 LdtInformation   : Ptr32 Void
   +0x178 OwnerProcessId   : Uint4B
   +0x17c Peb              : Ptr32 _PEB
   +0x180 Session          : Ptr32 _MM_SESSION_SPACE
   +0x184 Spare1           : Ptr32 Void
   +0x188 QuotaBlock       : Ptr32 _EPROCESS_QUOTA_BLOCK
   +0x18c ObjectTable      : Ptr32 _HANDLE_TABLE
   +0x190 DebugPort        : Ptr32 Void
   +0x194 PaeTop           : Ptr32 Void
   +0x198 DeviceMap        : Ptr32 Void
   +0x19c EtwDataSource    : Ptr32 Void
   +0x1a0 PageDirectoryPte : Uint8B
   +0x1a8 ImageFilePointer : Ptr32 _FILE_OBJECT
   +0x1ac ImageFileName    : [15] UChar
   +0x1bb PriorityClass    : UChar
   +0x1bc SecurityPort     : Ptr32 Void
   +0x1c0 SeAuditProcessCreationInfo : _SE_AUDIT_PROCESS_CREATION_INFO
   +0x1c4 JobLinks         : _LIST_ENTRY
   +0x1cc HighestUserAddress : Ptr32 Void
   +0x1d0 ThreadListHead   : _LIST_ENTRY
   +0x1d8 ActiveThreads    : Uint4B
   +0x1dc ImagePathHash    : Uint4B
   +0x1e0 DefaultHardErrorProcessing : Uint4B
   +0x1e4 LastThreadExitStatus : Int4B
   +0x1e8 PrefetchTrace    : _EX_FAST_REF
   +0x1ec LockedPagesList  : Ptr32 Void
   +0x1f0 ReadOperationCount : _LARGE_INTEGER
   +0x1f8 WriteOperationCount : _LARGE_INTEGER
   +0x200 OtherOperationCount : _LARGE_INTEGER
   +0x208 ReadTransferCount : _LARGE_INTEGER
   +0x210 WriteTransferCount : _LARGE_INTEGER
   +0x218 OtherTransferCount : _LARGE_INTEGER
   +0x220 CommitChargeLimit : Uint4B
   +0x224 CommitCharge     : Uint4B
   +0x228 CommitChargePeak : Uint4B
   +0x240 Vm               : _MMSUPPORT_FULL
   +0x340 MmProcessLinks   : _LIST_ENTRY
   +0x348 ModifiedPageCount : Uint4B
   +0x34c ExitStatus       : Int4B
   +0x350 VadRoot          : _RTL_AVL_TREE
   +0x354 VadHint          : Ptr32 Void
   +0x358 VadCount         : Uint4B
   +0x35c VadPhysicalPages : Uint4B
   +0x360 VadPhysicalPagesLimit : Uint4B
   +0x364 AlpcContext      : _ALPC_PROCESS_CONTEXT
   +0x374 TimerResolutionLink : _LIST_ENTRY
   +0x37c TimerResolutionStackRecord : Ptr32 _PO_DIAG_STACK_RECORD
   +0x380 RequestedTimerResolution : Uint4B
   +0x384 SmallestTimerResolution : Uint4B
   +0x388 ExitTime         : _LARGE_INTEGER
   +0x390 ActiveThreadsHighWatermark : Uint4B
   +0x394 LargePrivateVadCount : Uint4B
   +0x398 ThreadListLock   : _EX_PUSH_LOCK
   +0x39c WnfContext       : Ptr32 Void
   +0x3a0 ServerSilo       : Ptr32 _EJOB
   +0x3a4 SignatureLevel   : UChar
   +0x3a5 SectionSignatureLevel : UChar
   +0x3a6 Protection       : _PS_PROTECTION
   +0x3a7 HangCount        : Pos 0, 3 Bits
   +0x3a7 GhostCount       : Pos 3, 3 Bits
   +0x3a7 PrefilterException : Pos 6, 1 Bit
   +0x3a8 Flags3           : Uint4B
   +0x3a8 Minimal          : Pos 0, 1 Bit
   +0x3a8 ReplacingPageRoot : Pos 1, 1 Bit
   +0x3a8 Crashed          : Pos 2, 1 Bit
   +0x3a8 JobVadsAreTracked : Pos 3, 1 Bit
   +0x3a8 VadTrackingDisabled : Pos 4, 1 Bit
   +0x3a8 AuxiliaryProcess : Pos 5, 1 Bit
   +0x3a8 SubsystemProcess : Pos 6, 1 Bit
   +0x3a8 IndirectCpuSets  : Pos 7, 1 Bit
   +0x3a8 RelinquishedCommit : Pos 8, 1 Bit
   +0x3a8 HighGraphicsPriority : Pos 9, 1 Bit
   +0x3a8 CommitFailLogged : Pos 10, 1 Bit
   +0x3a8 ReserveFailLogged : Pos 11, 1 Bit
   +0x3a8 SystemProcess    : Pos 12, 1 Bit
   +0x3a8 HideImageBaseAddresses : Pos 13, 1 Bit
   +0x3a8 AddressPolicyFrozen : Pos 14, 1 Bit
   +0x3a8 ProcessFirstResume : Pos 15, 1 Bit
   +0x3a8 ForegroundExternal : Pos 16, 1 Bit
   +0x3a8 ForegroundSystem : Pos 17, 1 Bit
   +0x3a8 HighMemoryPriority : Pos 18, 1 Bit
   +0x3a8 EnableProcessSuspendResumeLogging : Pos 19, 1 Bit
   +0x3a8 EnableThreadSuspendResumeLogging : Pos 20, 1 Bit
   +0x3a8 SecurityDomainChanged : Pos 21, 1 Bit
   +0x3a8 SecurityFreezeComplete : Pos 22, 1 Bit
   +0x3a8 VmProcessorHost  : Pos 23, 1 Bit
   +0x3a8 VmProcessorHostTransition : Pos 24, 1 Bit
   +0x3a8 AltSyscall       : Pos 25, 1 Bit
   +0x3a8 TimerResolutionIgnore : Pos 26, 1 Bit
   +0x3a8 DisallowUserTerminate : Pos 27, 1 Bit
   +0x3ac DeviceAsid       : Int4B
   +0x3b0 SvmData          : Ptr32 Void
   +0x3b4 SvmProcessLock   : _EX_PUSH_LOCK
   +0x3b8 SvmLock          : Uint4B
   +0x3bc SvmProcessDeviceListHead : _LIST_ENTRY
   +0x3c8 LastFreezeInterruptTime : Uint8B
   +0x3d0 DiskCounters     : Ptr32 _PROCESS_DISK_COUNTERS
   +0x3d4 PicoContext      : Ptr32 Void
   +0x3d8 HighPriorityFaultsAllowed : Uint4B
   +0x3dc InstrumentationCallback : Ptr32 Void
   +0x3e0 EnergyContext    : Ptr32 _PO_PROCESS_ENERGY_CONTEXT
   +0x3e4 VmContext        : Ptr32 Void
   +0x3e8 SequenceNumber   : Uint8B
   +0x3f0 CreateInterruptTime : Uint8B
   +0x3f8 CreateUnbiasedInterruptTime : Uint8B
   +0x400 TotalUnbiasedFrozenTime : Uint8B
   +0x408 LastAppStateUpdateTime : Uint8B
   +0x410 LastAppStateUptime : Pos 0, 61 Bits
   +0x410 LastAppState     : Pos 61, 3 Bits
   +0x418 SharedCommitCharge : Uint4B
   +0x41c SharedCommitLock : _EX_PUSH_LOCK
   +0x420 SharedCommitLinks : _LIST_ENTRY
   +0x428 AllowedCpuSets   : Uint4B
   +0x42c DefaultCpuSets   : Uint4B
   +0x428 AllowedCpuSetsIndirect : Ptr32 Uint4B
   +0x42c DefaultCpuSetsIndirect : Ptr32 Uint4B
   +0x430 DiskIoAttribution : Ptr32 Void
   +0x434 DxgProcess       : Ptr32 Void
   +0x438 Win32KFilterSet  : Uint4B
   +0x440 ProcessTimerDelay : _PS_INTERLOCKED_TIMER_DELAY_VALUES
   +0x448 KTimerSets       : Uint4B
   +0x44c KTimer2Sets      : Uint4B
   +0x450 ThreadTimerSets  : Uint4B
   +0x454 VirtualTimerListLock : Uint4B
   +0x458 VirtualTimerListHead : _LIST_ENTRY
   +0x460 WakeChannel      : _WNF_STATE_NAME
   +0x460 WakeInfo         : _PS_PROCESS_WAKE_INFORMATION
   +0x490 MitigationFlags  : Uint4B
   +0x490 MitigationFlagsValues : <anonymous-tag>
   +0x494 MitigationFlags2 : Uint4B
   +0x494 MitigationFlags2Values : <anonymous-tag>
   +0x498 PartitionObject  : Ptr32 Void
   +0x4a0 SecurityDomain   : Uint8B
   +0x4a8 ParentSecurityDomain : Uint8B
   +0x4b0 CoverageSamplerContext : Ptr32 Void
   +0x4b4 MmHotPatchContext : Ptr32 Void
   +0x4b8 DynamicEHContinuationTargetsTree : _RTL_AVL_TREE
   +0x4bc DynamicEHContinuationTargetsLock : _EX_PUSH_LOCK
   +0x4c0 DynamicEnforcedCetCompatibleRanges : _PS_DYNAMIC_ENFORCED_ADDRESS_RANGES
   +0x4c8 DisabledComponentFlags : Uint4B
   +0x4cc PathRedirectionHashes : Ptr32 Uint4B
   +0x4d0 MitigationFlags3 : Uint4B
   +0x4d0 MitigationFlags3Values : <anonymous-tag>
