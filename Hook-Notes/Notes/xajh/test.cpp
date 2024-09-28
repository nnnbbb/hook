#include <Windows.h>
#include <atlstr.h>
#include <stdio.h>

//! mode: exe
//! flag: -Wall, -O3, -g
//! int: objs
//! inc: C:/Program Files/Microsoft Visual Studio/2022/Community/VC/Tools/MSVC/14.41.34120/atlmfc/include

int main(void) {
    CString str;
    wchar_t wc[20] = {0x5e, 0x00, 0x46, 0x00};
    str.Format(L"g name = %S \r\n", (wchar_t*)wc);

    CStringA strA(str);
    LPCSTR lstr = strA;

    // 使用 printf 输出字符串
    printf("%s", lstr);

    printf("2221\n");
    return 0;
}
