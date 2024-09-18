#include <sstream>
#include <variant>
#include <regex>
#include "plugin.h"
#include <keystone/keystone.h>


void PrintAscii(const char* asm_str);

void PrintMachineCode(std::string s, unsigned char* encode, size_t& size);

int GetAssemblyHex(const char* asm_str, unsigned char** encode_ptr, size_t* size_ptr);

std::string AddHexPrefixToHexNumber(const std::string& s);

std::vector<std::string> ProcessInput(const std::string& input, SELECTIONDATA& sel);