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

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#ifndef _LOG
#define _LOG

template <class T>
void _print(T arg) {
    std::cout << arg << " ";
}

template <typename... Args>
void log(Args... args) {
    time_t t;
    struct tm tmp;
    char dt[50];
    time(&t);
    localtime_s(&tmp, &t);
    strftime(dt, sizeof(dt), "[%Y-%m-%d %H:%M:%S]", &tmp);

    std::cout << dt << " ";  // Print timestamp to console

    std::string path = fs::current_path().string() + "/log.txt";
    // Write to log file
    std::ofstream file(path, std::ios_base::app | std::ios::out);  // ��utf-8�����ʽ���ļ�
    if (file.is_open()) {
        file << dt << " ";
        int _[] = {0, ((_print(args), file << args << " "), 0)...};
        std::cout << std::endl;
        file << std::endl;
        file.close();
    }
}

#endif  // !_LOG
