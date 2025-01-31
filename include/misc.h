#pragma once

struct PseuReg
{
    uint32_t _eax = 0, _ebx = 0, _ecx = 0, _edx = 0, _esi = 0, _edi = 0, _ebp = 0;
};

enum Register
{
    REG_EAX,
    REG_EBX,
    REG_ECX,
    REG_EDX,
    REG_ESI,
    REG_EDI,
    REG_ESP, // Avoid modifying ESP or EBP unless explicitly required
    REG_EBP
};

bool validateString(const std::string& s);

uint64_t aobSearch(const char* AOB, std::string funcName = "");

void depsTreePrint(const std::unordered_map<char*, std::vector<char*>>& depsTree, int indentLevel = 0);

void depsTreePrintToFile(const std::unordered_map<char*, std::vector<char*>>& depsTree, const std::string& filename, int indentLevel = 0);

std::vector<uint8_t> charArrayToHexVector(const char* data, std::size_t size);

std::vector<uint8_t> stringToHexVector(const char* str);

std::vector<uint8_t> toLittleEndianArray(uint32_t value);

void reportExecutionPoint(const char* caption);

DWORD GetMainThreadId();

std::vector<std::string> extractStrings(const std::vector<uint8_t>& data);

void CustomCall(
    void* functionAddr,
    const std::unordered_map<std::string, uint32_t>& regArgs,
    std::unordered_map<std::string, uint32_t>& returnRegs,
    size_t numArgs,
    ...);