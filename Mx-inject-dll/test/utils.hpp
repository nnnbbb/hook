#include <ctime>
#include <iostream>

#ifndef _LOG
#define _LOG

template <class T>
void _print(T arg) {
    std::cout << arg << " ";
}

inline void print_dt() {
    time_t t;
    struct tm tmp;
    char dt[50];
    time(&t);
    localtime_s(&tmp, &t);
    // https://www.geeksforgeeks.org/strftime-function-in-c/
    strftime(dt, sizeof(dt), "[%Y-%m-%d %H:%M:%S]", &tmp);
    std::cout << dt << " ";
}

template <class... Args>
void log(Args... args) {
    int _[] = {(print_dt(), 0), (_print(args), 0)...};
    std::cout << std::endl;
    (void)_;
}
#endif  // !_LOG
