#include "pch.h"
#include <string>
#include <iostream>
#include <fstream>
#include <cstdarg>
#include <chrono>
#include <ctime>
#include <locale>
#include <codecvt>
#include <iomanip>
#include <sstream>
#include <vector>
#include <thread>
#include <filesystem>
#include <algorithm>
#include "utils.h"

#ifndef _LOG
#define _LOG

namespace fs = std::filesystem;


template <class T, class F>
decltype(auto) map(const Vector<T> a, const F fn) {
    Vector<decltype(fn(a.front()))> result = {};
    std::transform(a.cbegin(), a.cend(), std::back_inserter(result), fn);
    return result;
}

template <typename T>
inline String join(const Vector<T>& vec, const T& delimiter) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i != vec.size() - 1) {
            oss << delimiter;
        }
    }
    return oss.str();
}

template <typename T>
inline String hex(T number, size_t prefix = 4) {
    std::stringstream ss;
    ss << "0x" << std::uppercase << std::hex << std::setw(prefix) << std::setfill('0') << number;
    return ss.str();
}


template <class T>
inline String _print(T arg) {
    std::cout << arg << " ";
    std::ostringstream oss;
    oss << arg;
    return LocalCPToUtf8(oss.str()) + " ";
}

template <>
inline String _print(wchar_t* arg) {
    std::cout << Utf16ToLocalCP(arg) << " ";
    return Utf16ToUtf8(arg);
}

template <>
inline String _print(const wchar_t* arg) {
    std::cout << Utf16ToLocalCP(arg) << " ";
    return Utf16ToUtf8(arg);
}


inline void writeToFile(const String& path, const Vector<String>& logs) {
    std::ofstream f(path, std::ios_base::app | std::ios::out);
    map(logs, [&f](const String& x) {
        return (f << x, x);
    });

    f << std::endl;
    f.close();
}

template <typename... Args>
inline void log(Args... args) {
    time_t t;
    struct tm tmp;
    char dt[50];
    time(&t);
    localtime_s(&tmp, &t);
    strftime(dt, sizeof(dt), "[%Y-%m-%d %H:%M:%S] ", &tmp);
    std::cout << dt;

    String path = fs::current_path().string() + "/log.txt";
    Vector<String> logs = {String(dt), _print(args)...};
    writeToFile(path, logs);
    std::cout << std::endl;
}

#endif  // !_LOG
