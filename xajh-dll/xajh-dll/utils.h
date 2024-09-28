#include <string>
#include <vector>
#include <Windows.h>

#ifndef _UTILS
#define _UTILS
typedef unsigned long u32;
typedef unsigned long long u64;
typedef unsigned char u8;
typedef unsigned short u16;
typedef std::string String;
typedef std::wstring WString;


String Utf16ToUtf8(const WString& wstr);
String Utf16ToUtf8(const wchar_t* wstr);
WString Utf8ToUtf16(const String& str);
WString Utf8ToUtf16(const char* str);

#define FN_ADDRESS(func) \
    std::cout << "Function address of " #func ": " << "0x" << (void*)func << std::endl;

#define NOT_NULL(value)                                 \
    do {                                                \
        if (value == NULL) {                            \
            throw std::runtime_error("Handle is NULL"); \
        }                                               \
    } while (0)


#define PrintCString(cstr)   \
    {                        \
        CStringA strA(cstr); \
        LPCSTR str = strA;   \
        printf("%s", str);   \
    }

#define PrintWChar(address)                                    \
    {                                                          \
        CString cstr;                                          \
        cstr.Format(L#address " = %s\r\n", (wchar_t*)address); \
        PrintCString(cstr);                                    \
    }
#endif  // !_UTILS
