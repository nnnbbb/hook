#ifndef _UTILS
#define _UTILS

#define NOT_NULL(value)                                 \
    do {                                                \
        if (value == NULL) {                            \
            throw std::runtime_error("Handle is NULL"); \
        }                                               \
    } while (0)

#endif  // !_UTILS
