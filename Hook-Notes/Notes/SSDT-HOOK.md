51hook windows内核开发4 ssdt

1. r3 传eax就是系调用号
2. 进入KiFastCallEntry
3. 查SSDT函数地址表 
4. eax and 0x0FFF 获取系统调用号


SSDT HOOK
获取系统路径加载ntdll，使用MmCreateSection和mMapViewInSystemSpace加载到内存，查找导出表找到目标函数，然后修改SSDT表实现函数钩子
https://blog.csdn.net/qq_45844442/article/details/131344633

获取未导出的函数
比如 NtDebugActiveProcess