#include <Windows.h>
#include <string>
#include <iostream>
#include <tlhelp32.h>
#include <processsnapshot.h>
#include <fstream>
#include <sstream>
#include "json/json.h"
#include "log.hpp"
#include "kmp.h"

DWORD GetPidByName(const WCHAR* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    PROCESSENTRY32 pe32 = {sizeof(PROCESSENTRY32)};
    DWORD pid = 0;

    if (Process32First(snapshot, &pe32)) {
        do {
            WCHAR* szName = pe32.szExeFile;
            // printf("szName = %ls\n", szName);
            if (wcscmp(szName, name) == 0) {
                pid = pe32.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &pe32));
    }
    CloseHandle(snapshot);
    return pid;
}

std::string removeSpaces(std::string& input) {
    input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
    return input;
}


BOOL ByteToChar(BYTE* byteCode, char* strCode, int codeLen) {
    for (int i = 0; i < codeLen; i++) {
        wsprintfA(&strCode[i * 2], "%02X", byteCode[i]);
    }
    return TRUE;
}

BOOL CmpStrCode(char* code, char* readStr, int len) {
    for (int i = 0; i < len; i++) {
        if (code[i] == '?') {
            continue;
        }
        if (code[i] != readStr[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

BOOL ScanGameCode(
    HANDLE hProcess,
    DWORD beginAddr,
    DWORD endAddr,
    char* code,
    DWORD codeLen,
    DWORD& retAddr
) {
    const size_t size = 0x1000;
    BYTE* readCode = new BYTE[size];
    for (DWORD readAddr = beginAddr; readAddr <= endAddr - size; readAddr += (size - codeLen)) {
        memset(readCode, 0, size);
        char strCode[size * 2 + 1] = {0};
        BOOL retRead = ReadProcessMemory(hProcess, (LPVOID)readAddr, readCode, size, NULL);
        if (retRead == 0) {
            continue;
        }
        ByteToChar(readCode, strCode, size);
        for (size_t i = 0; i < size * 2 + 1 - codeLen; i++) {
            // BOOL retCmp = CmpStrCode(code, &strCode[i], codeLen);
            BOOL retCmp = kmpSearch(&strCode[i], code);
            if (retCmp == TRUE) {
                retAddr = readAddr + i / 2;
                printf("起始地址为:0x%X\n", retAddr);
            }
        }
    }
    return TRUE;
}

Json::Value readJsonFile(const String& filename) {
    std::ifstream file;
    file.open(filename);

    Json::CharReaderBuilder ReaderBuilder;
    ReaderBuilder["emitUTF8"] = true;

    Json::Value root;

    String strerr;
    bool ok = Json::parseFromStream(ReaderBuilder, file, &root, &strerr);
    if (!ok) {
        std::cerr << "json解析错误";
    }
    return root;
}

int update() {
    DWORD pid = GetPidByName(L"xajh.exe");
    printf("pid -> %d\n", pid);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, (DWORD)pid);
    DWORD addr = 0;

    Json::Value root = readJsonFile("./code.json");
    for (const auto& item : root) {
        String code = item["code"].asString();
        String comment = item["comment"].asString();
        int offset = item["offset"].asInt();
        removeSpaces(code);
        ScanGameCode(hProcess, 0x00AA94FF, 0x00BA9600, (char*)code.c_str(), code.size(), addr);
        log(comment, hex(addr), "offset = ", offset);
    }
    return 0;
}

int main() {
    update();
    system("pause");
    return 0;
}