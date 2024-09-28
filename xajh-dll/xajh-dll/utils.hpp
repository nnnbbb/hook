#ifndef _UTILS
#define _UTILS
typedef unsigned long u32;
typedef unsigned long long u64;
typedef unsigned char u8;
typedef unsigned short u16;

#define NOT_NULL(value)                                 \
    do {                                                \
        if (value == NULL) {                            \
            throw std::runtime_error("Handle is NULL"); \
        }                                               \
    } while (0)

inline void PrintCString(const CString& cstr) {
    // 将 CString 转换为多字节字符集 LPCSTR
    CStringA strA(cstr);
    LPCSTR str = strA;

    // 使用 printf 输出字符串
    printf("%s", str);
}

#endif  // !_UTILS
