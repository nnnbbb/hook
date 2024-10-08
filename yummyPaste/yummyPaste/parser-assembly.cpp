#include "parser-assembly.h"

void PrintAscii(const char* asm_str) {
    dprintf("s = %s\n", asm_str);
    dprintf("Print ascii: \n");
    for (const char* ptr = asm_str; *ptr != '\0'; ++ptr) {
        dprintf("'%c-%d' ", *ptr, static_cast<int>(*ptr));
    }
    dprintf("\n");
}

void PrintMachineCode(std::string s, unsigned char* encode, size_t& size) {
    dprintf("Print machine code: \n%s = ", s.c_str());
    for (size_t i = 0; i < size; i++) {
        dprintf("%02X ", encode[i]);
    }
    dprintf("\n");
}

int GetAssemblyHex(const char* asm_str, unsigned char** encode_ptr, size_t* size_ptr) {
    ks_engine* ks;
    ks_err err = ks_err::KS_ERR_OK;
    size_t count;

    if ((err = ks_open(KS_ARCH_X86, KS_MODE_32, &ks)) != KS_ERR_OK) {
        dprintf("Failed to initialize Keystone Engine");
        return 1;
    }

    if ((err = (ks_err)ks_asm(ks, asm_str, 0, encode_ptr, size_ptr, &count)) != KS_ERR_OK) {
        dprintf("Failed to assemble given code = %s\n", ks_strerror((ks_err)err));

        std::string message = "Failed to assemble given asm_str = " + std::string(asm_str);
        dprintf("%s\n", message.c_str());
        MessageBoxA(hwndDlg, message.c_str(), "Error", MB_ICONINFORMATION);
        return 1;
    }

    ks_close(ks);

    return 0;
}

std::string ReplaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();  // Move past the to string
    }
    return str;
}

// 去除字符串前后的空格
std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    if (start == std::string::npos || end == std::string::npos) {
        return "";
    }

    return str.substr(start, end - start + 1);
}

std::string AddHexPrefixToHexNumber(const std::string& s) {
    std::regex hex_number_regex(R"(\b([0-9a-fA-F]+)\b)");
    return std::regex_replace(s, hex_number_regex, "0x$1");
}

std::string RemoveSegmentRegisters(const std::string& input) {
    std::string result = input;
    result = ReplaceAll(result, "ss:", "");
    result = ReplaceAll(result, "ds:", "");
    result = ReplaceAll(result, "es:", "");
    result = ReplaceAll(result, "fs:", "");
    result = ReplaceAll(result, "gs:", "");
    return result;
}

constexpr char validStartChars[] = {'j', 'c'};
bool ValidStartChar(char c) {
    return std::find(std::begin(validStartChars), std::end(validStartChars), c) != std::end(validStartChars);
}

std::string ProcessHexNumber(const std::string& token) {
    size_t pos = token.find('.');

    std::string modifiedToken = token;
    if (pos != std::string::npos) {
        // 去掉 '.' 之前的部分
        modifiedToken = token.substr(pos + 1);
    }
    return AddHexPrefixToHexNumber(modifiedToken);
}

std::vector<std::string> ProcessToken(std::vector<std::string>& tokens, int start) {
    try {
        std::string firstToken = tokens[0];
        auto v = ValidStartChar(firstToken.c_str()[0]);
        // jmp call 指令需要计算偏移
        std::string modifiedToken = ProcessHexNumber(tokens[1]);
        printf("modifiedToken -> %s\n", modifiedToken.c_str());
        if (v == true) {
            // 将元素转换为数字
            int originalNumber = std::stoi(modifiedToken, nullptr, 0);  // 支持十六进制转换
            int offsetNumber = originalNumber - start;
            std::stringstream ss;
            ss << std::hex << std::showbase << offsetNumber;
            tokens[1] = ss.str();
        }
        return tokens;

    } catch (const std::invalid_argument& e) {
        dprintf("Invalid argument exception: %s\n", e.what());  // 输出异常信息到标准错误
        return tokens;                                          // 如果转换失败，返回原字符串
    } catch (const std::out_of_range& e) {
        dprintf("Out of range exception: %s\n", e.what());  // 输出异常信息到标准错误
        return tokens;                                      // 如果数字超出范围，返回原字符串
    }
}

std::string ProcessJmp(std::string& s, const SELECTIONDATA& sel) {
    std::vector<std::string> tokens;
    size_t startPos = 0;

    // 找到第一个空格
    size_t firstSpace = s.find_first_of(' ', startPos);
    if (firstSpace != std::string::npos) {
        // 提取指令
        tokens.push_back(s.substr(startPos, firstSpace - startPos));
        startPos = firstSpace + 1;  // 跳过空格

        // 提取后续内容，允许空格
        tokens.push_back(s.substr(startPos));
    } else {
        // 如果没有空格，整个字符串就是指令
        tokens.push_back(s);
    }

    if (tokens.size() < 2) {
        return s;  // 如果没有第二个元素，返回原字符串
    }

    std::string& secondToken = tokens[1];

    std::string newString;
    if (secondToken.find(',') == std::string::npos) {
        auto result = ProcessToken(tokens, sel.start);
        newString = tokens[0] + " " + ProcessHexNumber(tokens[1]);
    } else {
        size_t pos = 0;
        size_t comma = tokens[1].find_first_of(',', pos);
        std::vector<std::string> secondTokens;
        secondTokens.push_back(tokens[0]);


        if (comma != std::string::npos) {
            // 提取指令
            secondTokens.push_back(tokens[1].substr(pos, comma));
            pos = comma + 1;  // 跳过空格

            // 提取后续内容，允许空格
            secondTokens.push_back(tokens[1].substr(pos));
        }

        if (secondTokens.size() < 3) {
            return s;
        }

        std::string& lastToken = secondTokens[2];
        newString = secondTokens[0] + " " + ProcessHexNumber(secondTokens[1]) + ", " + ProcessHexNumber(lastToken);
    }
    return newString;
}


// 提取并处理每一行的函数
std::string ProcessLine(const std::string& line, SELECTIONDATA& sel) {
    std::string s = line;
    s = trim(s);
    s = RemoveSegmentRegisters(s);

    // 处理以 'j' 等需要计算偏移的指令
    return ProcessJmp(s, sel);
}

// 处理输入的函数
std::vector<std::string> ProcessInput(const std::string& input, SELECTIONDATA& sel) {
    std::string temp = input;
    size_t pos = 0;
    std::vector<std::string> str;

    while ((pos = temp.find("\r\n")) != std::string::npos) {
        std::string line = temp.substr(0, pos);
        str.push_back(ProcessLine(line, sel));
        temp.erase(0, pos + 2);  // 移动到 "\r\n" 后面
    }

    // 检查是否还有剩余的字符串
    if (!temp.empty()) {
        str.push_back(ProcessLine(temp, sel));
    }
    return str;
}

void TestProcessInput() {
    SELECTIONDATA sel;
    sel.start = 0x5DB1CCD5;

    std::vector<std::string> str = {
        "mov edx, ntdll.77238FC0 \r\n mov edx, [esi+77238FC0]",
        "mov dword ptr ds:[esi+0x77238FC0], edx",
        "call libcef.5DA78FA0",
        "mov eax ,  dword ptr ss:[esp]",
        "nop ",
        "add esp , 8",
        "jmp libcef.5DB1CE3A",
        "mov ecx,dword ptr ds:[esi+112]",
        "or ecx,dword ptr ds:[esi+1C]",
        "mov ecx,1",
        "fld dword ptr ss:[ebp+7D5]",
        "mov dword ptr ss:[esp+14],edx",
        "fstp dword ptr ss:[esp+24]",
        "fld1 ",
        "fcom dword ptr ss:[ebp+829]",
        "fnstsw ax",
        "test ah,5",
        "jp xajh.A208B3",
        "fld dword ptr ss:[ebp+829]",
        "fstp dword ptr ss:[esp+18]",
        "push 0",
        "jmp A208B7"
    };

    for (const auto& s : str) {
        std::vector<std::string> r = ProcessInput(s, sel);
        for (const auto& e : r) {
            dprintf("%s\n", e.c_str());
        }
    }
}