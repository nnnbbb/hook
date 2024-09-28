地址里面是一个地址
```
utf-16 就是 wchar_t 类型
一个字占两个字节 就是4个十六进制数字

LE（Little Endian）：小端序，低位字节在前。
BE（Big Endian）：大端序，高位字节在前。

UTF-16 Little Endian (LE)：
"你"：0x60 0x4F
"好"：0x7D 0x59

UTF-16 Big Endian (BE)：
"你"：0x4F 0x60
"好"：0x59 0x7D
```

用 CString 打印

```cpp
void test_cstring(void) {
    // 设置区域以支持宽字符
    std::setlocale(LC_ALL, "");  
    // "你好"的UTF-16编码
    wchar_t wc[] = {0x4F60, 0x597D, 0x0000};  

    // 使用 MessageBoxW 输出宽字符数组
    // MessageBoxW(NULL, wc, L"Title", MB_OK);
    wprintf(L"%ls\n", wc);
}
int main(int, char**) {
    test_csting();
    return 0;
}
```
|   地址      |     值      |     ASCII  | UTF-16    |
| :---------: | :---------: | :--------: | :-------: |
| 3C910000    | 597D4F60    |  `O}Y      |   你好    |
  

