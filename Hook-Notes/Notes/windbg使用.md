
### sx* 
命令用来控制被调试的程序发生某个异常或特定事件时，调试器要采取的动作


- #### sxe
当发生该异常时，在任何错误处理器被激活之前目标立即中断到调试器中。这种处理类型称为第一次处理机会。

- #### sxd
发生该类异常时，调试器不会在第一次处理机会时中断(虽然会显示信息)。如果其他错误处理器没有处理掉该异常，执行会停止下来并中断到调试器。这种处理类型称为第二次处理机会。

- #### sxn
当该异常发生时，目标程序不中断到调试器中。但是，会通过一条消息提示发生了异常。

- #### sxi
异常发生时，目标程序不中断到调试器，并且不会显示信息。



- #### sxi wos 
忽略 WOW64 single step exception 异常

[windbg-sx、ld(设置异常处理、设置模块加载时断下)](https://blog.csdn.net/hgy413/article/details/7598236)


- ##### 下载符号文件
https://bbs.kanxue.com/thread-260857.htm

``` cmd
C:/Users/nnn/AppData/Roaming/OpenArk/files/Windbg/x64/symchk.exe  /r  .\ntoskrnl.exe   /s  SRV*E:/BaiduNetdiskDownload/hack/Mirage*http://msdl.microsoft.com/download/symbols
```


- ##### 测试驱动签名
```
如果您想用微软的官方签名工具进行测试驱动的签名，您可以使用Windows SDK中的makecert.exe和signtool.exe命令行工具。
添加测试签名（含时间戳）的命令如下：
使用 makecert.exe 生成证书：
makecert.exe -r -pe -ss PrivateCertStore -n "CN=TestCert" testcert.cer
其中“TestCert”是证书名称，“testcert.cer”是生成的证书文件存放地址。
使用 signtool.exe 对驱动程序进行签名：
signtool.exe sign /v /s PrivateCertStore /n "TestCert" /t http://timestamp.digicert.com Driver.sys 其中“Driver.sys”是您要签名的驱动文件名称。
该命令会使用指定的测试证书（TestCert）和时间戳，对指定的驱动程序进行签名。
```

```CMD
用 CMD
C:\Fiddler\makecert.exe -r -pe -ss PrivateCertStore -n "CN=TestCert" testcert.cer 
"C:\Program Files (x86)\Windows Kits\10\bin\10.0.17763.0\x64\signtool.exe" sign /v /s PrivateCertStore /n "TestCert" /t http://timestamp.digicert.com  release64\Mirage.sys
```
https://www.52pojie.cn/thread-1876898-1-1.html


- ##### 去掉SXS.DLL的输出日志
```
ed nt!Kd_SXS_Mask 0
ed nt!Kd_FUSION_Mask 0
```

- ##### 查看函数汇编 u KdpTrap
```
2: kd> u KdpTrap
nt!KdpTrap:
fffff800`0d957fa8 48895c2408      mov     qword ptr [rsp+8],rbx
fffff800`0d957fad 4889542410      mov     qword ptr [rsp+10h],rdx
fffff800`0d957fb2 57              push    rdi
fffff800`0d957fb3 4883ec40        sub     rsp,40h
fffff800`0d957fb7 33d2            xor     edx,edx
fffff800`0d957fb9 498bd9          mov     rbx,r9
fffff800`0d957fbc 41813803000080  cmp     dword ptr [r8],80000003h
fffff800`0d957fc3 88542458        mov     byte ptr [rsp+58h],dl
```
