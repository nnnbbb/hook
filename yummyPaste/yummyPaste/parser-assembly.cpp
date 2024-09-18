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
        dprintf("Failed to assemble given asm_str = %s\n", asm_str);
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

// 计算 call指令或者jmp指令后面的偏移
std::variant<std::string, int> ProcessJmp(std::string& s, const SELECTIONDATA& sel) {
    if (s.empty() || !ValidStartChar(s[0])) {
        return s;  // 如果不是以有效字符开头，返回原字符串
    }

    std::istringstream iss(s);
    std::string segment;
    std::vector<std::string> tokens;

    // 以空格分割字符串
    while (iss >> segment) {
        tokens.push_back(segment);
    }

    if (tokens.size() < 2) {
        return s;  // 如果没有第二个元素，返回原字符串
    }

    try {
        std::string& secondToken = tokens[1];

        // 检查第二个元素是否包含 '.'
        size_t pos = secondToken.find('.');
        if (pos != std::string::npos) {
            // 去掉 '.' 之前的部分
            secondToken = secondToken.substr(pos + 1);
            // 给数字前加上 0x
            secondToken = "0x" + secondToken;
        }

        // 将第二个元素转换为数字
        int originalNumber = std::stoi(secondToken, nullptr, 0);  // 支持十六进制转换
        int offsetNumber = originalNumber - sel.start;

        // 替换数组中的第二个元素
        std::stringstream ss;
        ss << std::hex << std::showbase << offsetNumber;
        tokens[1] = ss.str();

        // 生成新的字符串
        std::string newString = tokens[0] + " " + tokens[1];
        return newString;

    } catch (const std::invalid_argument& e) {
        dprintf("Invalid argument exception: %s\n", e.what());  // 输出异常信息到标准错误

        return s;  // 如果转换失败，返回原字符串
    } catch (const std::out_of_range& e) {
        dprintf("Out of range exception: %s\n", e.what());  // 输出异常信息到标准错误
        return s;                                           // 如果数字超出范围，返回原字符串
    }
}

std::string AddHexPrefixToHexNumber(const std::string& s) {
    std::regex hex_number_regex(R"(\b([0-9a-fA-F]+)\b)");
    return std::regex_replace(s, hex_number_regex, "0x$1");
}

// 处理输入的函数
std::vector<std::string> ProcessInput(const std::string& input, SELECTIONDATA& sel) {
    std::string temp = input;
    size_t pos = 0;
    std::vector<std::string> str;

    while ((pos = temp.find("\r\n")) != std::string::npos) {
        std::string s = temp.substr(0, pos);
        s = RemoveSegmentRegisters(s);
        // 给数字添加 0x 前缀
        s = AddHexPrefixToHexNumber(s);

        // 处理以 'j' 等需要计算偏移的指令
        auto result = ProcessJmp(s, sel);
        if (std::holds_alternative<std::string>(result)) {
            s = std::get<std::string>(result);
        }

        str.push_back(s);
        temp.erase(0, pos + 2);  // 移动到 "\r\n" 后面
    }

    // 检查是否还有剩余的字符串
    if (!temp.empty()) {
        temp = RemoveSegmentRegisters(temp);
        // 给数字添加 0x 前缀
        temp = AddHexPrefixToHexNumber(temp);

        auto result = ProcessJmp(temp, sel);
        if (std::holds_alternative<std::string>(result)) {
            temp = std::get<std::string>(result);
        }

        str.push_back(temp);
    }
    return str;
}
