
过TP保护与解除游戏驱动保护
https://blog.csdn.net/whl0071/article/details/131052006

```
//这是TP防止调试器在它体内创建线程   
NtOpenThread    
//这是TP防止OD等在进程列表看到游戏进程 
NtOpenProcess
//这是TP防止其他软件附加它  
KiAttachProcess
//这是TP防止别人读取它的内存  
NtReadVirtualMemory
//这是TP防止别人在它的内存里面乱写乱画 
NtWriteVirtualMemory
//这是TP这两个是COM串口的接受和发送数据 
KDCOM.dll:KdReceivePacket
//这是TP主要用来方式别人双机调试,TP使用了KdDisableDebugger来禁用双机调试.  
KDCOM.dll:KdSendPacket
```