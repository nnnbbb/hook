#include "kmp.h"

int *pre_kmp(String pattern) {
    int size = pattern.size();
    int *pie = new int[size];
    pie[0] = 0;
    int k = 0;
    for (int i = 1; i < size; i++) {
        while (k > 0 && pattern[k] != pattern[i]) {
            k = pie[k - 1];
        }
        if (pattern[k] == pattern[i]) {
            k = k + 1;
        }
        pie[i] = k;
    }

    return pie;
}

bool kmpSearch(String text, String pattern) {
    int *prefixTable = pre_kmp(pattern);
    int matchedPos = 0;
    for (size_t i = 0; i < text.length(); i++) {
        while (matchedPos > 0 && pattern[matchedPos] != text[i] && pattern[matchedPos] != '?') {
            matchedPos = prefixTable[matchedPos - 1];
        }
        if (pattern[matchedPos] == text[i] || pattern[matchedPos] == '?') {
            matchedPos = matchedPos + 1;
        }

        if (matchedPos == pattern.length()) {
            std::cout << "Pattern occurs with shift " << i - (pattern.length() - 1) << std::endl;
            matchedPos = prefixTable[matchedPos - 1];
            return true;
        }
    }
    return false;
}

bool kmpSearch(char *text, char *pattern) {
    return kmpSearch(String(text), String(pattern));
}