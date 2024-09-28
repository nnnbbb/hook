msdn上面的解释：

MT：mutithread，多线程库，编译器会从运行时库里面选择多线程静态连接库来解释程序中的代码，即连接LIBCMT.lib库

MTd：mutithread+debug，多线程调试版，连接LIBMITD.lib库

MD：MT+DLL，多线程动态库，连接MSVCRT.lib库，这是个导入库，对应动态库为MSVCRT.dll

MDd： MT+DLL+debug，多线程动态调试库，连接MSVCRTD.lib库，对应动态库为MSVCRTD.dll

开发多线程程序时（单线程本文不做讨论），需要选择MT、MTd、MD、MDd其中的一个。

对于MT/MTd，由于连接运行时库是LIBCMT.lib/LIBCMTD.lib，这两个库是静态库，所以此种方式编译的程序，移到另一台机器上面也可以正常运行。

但是对于MD/MDd，连接的是动态库，所以如果另一台机器上没有MSVCRT.dll/MSVCRTD.dll时，就提示缺少动态库这样的错误。
https://www.cnblogs.com/SZxiaochun/p/7684371.html