#include "pch.h"

bool validateString(const std::string& s)
{
    for (const char c : s)
    {
        // get past assert
        if (c >= -1 && c <= 255)
        {
            if (!isalpha(c) && !isspace(c))
                return false;
        }
    }

    return true;
}

uint64_t aobSearch(const char* AOB, std::string funcName)
{
    uint64_t foundAddr = PLH::findPattern((uint64_t)MW3Module.lpBaseOfDll, MW3Module.SizeOfImage, AOB);

    if (!foundAddr)
    {
        std::cout << funcName + " AOB search failed." << std::endl;
    }

    return foundAddr;
}

void depsTreePrintToFile(const std::unordered_map<char*, std::vector<char*>>& depsTree, const std::string& filename, int indentLevel)
{
    std::ofstream outfile(filename, std::ios::app);

    if (!outfile.is_open())
    {
        std::cerr << "Error: Could not open file " << filename << " for writing." << std::endl;
        return;
    }

    const std::string indent(indentLevel * 4, ' ');

    for (const auto& entry : depsTree)
    {
        const char* parent = entry.first;
        const std::vector<char*>& children = entry.second;

        outfile << indent << parent << std::endl;
        for (const char* child : children)
        {
            outfile << indent << "    " << child << std::endl;
        }
    }

    std::cout << "Printed dependencies to: " << filename << std::endl;

    outfile.close();
}

void depsTreePrint(const std::unordered_map<char*, std::vector<char*>>& depsTree, int indentLevel)
{
    const std::string indent(indentLevel * 4, ' '); // Indentation (4 spaces per level)

    for (const auto& entry : depsTree)
    {
        const char* parent = entry.first;
        const std::vector<char*>& children = entry.second;

        std::cout << indent << parent << std::endl; // Print parent
        for (const char* child : children)
        {
            std::cout << indent << "    " << child << std::endl; // Print child indented
        }
    }
}

std::vector<uint8_t> charArrayToHexVector(const char* data, std::size_t size)
{
    std::vector<uint8_t> result;

    for (std::size_t i = 0; i < size; ++i)
    {
        result.push_back(static_cast<uint8_t>( data[i] ));
    }

    return result;
}

std::vector<uint8_t> stringToHexVector(const char* str)
{
    std::vector<uint8_t> result;

    while (*str != '\0')
    {
        result.push_back(static_cast<uint8_t>( *str ));
        ++str;
    }

    return result;
}

std::vector<uint8_t> toLittleEndianArray(uint32_t value)
{
    std::vector<uint8_t> byteArray = {
        static_cast<uint8_t>( value & 0xFF ),
        static_cast<uint8_t>( ( value >> 8 ) & 0xFF ),
        static_cast<uint8_t>( ( value >> 16 ) & 0xFF ),
        static_cast<uint8_t>( ( value >> 24 ) & 0xFF )
    };
    return byteArray;
}

// this can be used for debugging; see what eip is and use it for attching, logging, etc.
void reportExecutionPoint(const char* caption)
{
    uint32_t _EIP;
    __asm {
        call get_eip
        jmp exec_finish

        get_eip :
        push eax
        mov eax, [esp + 4]
        mov DWORD ptr[_EIP], eax
        add esp, 4
        ret

            exec_finish :
    }

    const char* baseStr = "Calling from: ";
    char        addressStr[20];
    std::sprintf(addressStr, "0x%X", _EIP);
    size_t totalLength = std::strlen(baseStr) + std::strlen(addressStr) + 1;
    char* concatenatedStr = new char[totalLength];
    std::strcpy(concatenatedStr, baseStr);
    std::strcat(concatenatedStr, addressStr);

    MessageBoxA(0, concatenatedStr, caption, MB_OK | MB_ICONQUESTION);
}

// Function to find the main thread of the current process; the game also has global "threadId" variable.
DWORD GetMainThreadId()
{
    DWORD processId = GetCurrentProcessId();
    THREADENTRY32 te32;
    te32.dwSize = sizeof(THREADENTRY32);

    HANDLE hThreadSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (hThreadSnap == INVALID_HANDLE_VALUE)
    {
        std::cerr << "CreateToolhelp32Snapshot failed!" << std::endl;
        return 0;
    }

    DWORD mainThreadId = 0;
    FILETIME createTime, exitTime, kernelTime, userTime;
    FILETIME earliestTime = {0xFFFFFFFF, 0xFFFFFFFF};

    if (Thread32First(hThreadSnap, &te32))
    {
        do
        {
            if (te32.th32OwnerProcessID == processId)
            {
                HANDLE hThread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, te32.th32ThreadID);
                if (hThread)
                {
                    if (GetThreadTimes(hThread, &createTime, &exitTime, &kernelTime, &userTime))
                    {
                        if (CompareFileTime(&createTime, &earliestTime) < 0)
                        {
                            earliestTime = createTime;
                            mainThreadId = te32.th32ThreadID;
                        }
                    }
                    CloseHandle(hThread);
                }
            }
        }
        while (Thread32Next(hThreadSnap, &te32));
    }
    CloseHandle(hThreadSnap);

    if (mainThreadId == 0)
    {
        std::cerr << "Failed to find the main thread!" << std::endl;
        return 0;
    }

    return mainThreadId;
}

std::vector<std::string> extractStrings(const std::vector<uint8_t>& data)
{
    std::vector<std::string> strings;
    size_t pos = 0;

    while (pos < data.size())
    {
// Look for the next null terminator
        auto nullPos = std::find(data.begin() + pos, data.end(), '\0');

        // If a null terminator is found, create a string from current position to null terminator
        if (nullPos != data.end())
        {
            strings.emplace_back(data.begin() + pos, nullPos);
            pos = std::distance(data.begin(), nullPos) + 1;  // Move past the null terminator
        }
        else
        {
            break;  // No more null terminators
        }
    }
    return strings;
}


std::unordered_map<std::string, Register> registerMap = {
    {"eax", REG_EAX}, {"ebx", REG_EBX}, {"ecx", REG_ECX}, {"edx", REG_EDX},
    {"esi", REG_ESI}, {"edi", REG_EDI}, {"esp", REG_ESP}, {"ebp", REG_EBP}
};


// A wrapper to call the user-defined functions.
// regArgs refers to registers that the function is expecting.
// returnRegs refers to the registers whose values you want to retrieve.
// numArgs refers to number of variables that are to be pushed on stack.
// then pass any value the the custom function is expecting to be on stack.
void CustomCall(
    void* functionAddr,
    const std::unordered_map<std::string, uint32_t>& regArgs,
    std::unordered_map<std::string, uint32_t>& returnRegs,
    size_t numArgs,
    ...)
{

    __asm
    {
        pushad
        pushfd
    }

    PseuReg pseuReg {};

    // Assign register values from the map
    for (const auto& [regName, value] : regArgs)
    {
        if (registerMap.find(regName) != registerMap.end())
        {
            switch (registerMap[regName])
            {
            case REG_EAX: pseuReg._eax = value; break;
            case REG_EBX: pseuReg._ebx = value; break;
            case REG_ECX: pseuReg._ecx = value; break;
            case REG_EDX: pseuReg._edx = value; break;
            case REG_ESI: pseuReg._esi = value; break;
            case REG_EDI: pseuReg._edi = value; break;
            default: break; // Ignore ESP/EBP unless necessary
            }
        }
    }

    // Prepare the stack with variadic arguments
    va_list args;
    va_start(args, numArgs);

    // Use an array to store variadic arguments for pushing later
    uint32_t* stackArgs = new uint32_t[numArgs];
    for (size_t i = 0; i < numArgs; ++i)
    {
        stackArgs[i] = va_arg(args, uint32_t);
    }
    va_end(args);

    size_t idx;
    for (size_t i = 0; i < numArgs; ++i)
    {
        idx = i * 4;
        __asm
        {
            mov eax, stackArgs
            mov eax, [eax]
            add eax, idx; lea eax, [eax + i * 4] not working
            push eax
        }
    }

    // Assign values to registers
    __asm
    {
        cmp[pseuReg._eax], 0
        je next
        mov eax, [pseuReg._eax]

        next:
        cmp[pseuReg._ebx], 0
            je next2
            mov ebx, [pseuReg._ebx]

            next2 :
            cmp[pseuReg._ecx], 0
            je next3
            mov ecx, [pseuReg._ecx]

            next3 :
            cmp[pseuReg._edx], 0
            je next4
            mov edx, [pseuReg._edx]

            next4 :
            cmp[pseuReg._esi], 0
            je next5
            mov esi, [pseuReg._esi]

            next5 :
            cmp[pseuReg._edi], 0
            je next6
            mov edi, [pseuReg._edi]

            next6 :
            cmp[pseuReg._ebp], 0
            je next7
            mov ebp, [pseuReg._ebp]

            next7 :


            call functionAddr

            mov pseuReg._eax, eax
            mov pseuReg._ebx, ebx
            mov pseuReg._ecx, ecx
            mov pseuReg._edx, edx
            mov pseuReg._esi, esi
            mov pseuReg._edi, edi
    }


    // Create a map to store and return requested register values
    for (const auto reReg : returnRegs)
    {
        const std::string& regName = reReg.first;
        if (registerMap.find(regName) != registerMap.end())
        {
            switch (registerMap[regName])
            {
            case REG_EAX: returnRegs[regName] = pseuReg._eax; break;
            case REG_EBX: returnRegs[regName] = pseuReg._ebx; break;
            case REG_ECX: returnRegs[regName] = pseuReg._ecx; break;
            case REG_EDX: returnRegs[regName] = pseuReg._edx; break;
            case REG_ESI: returnRegs[regName] = pseuReg._esi; break;
            case REG_EDI: returnRegs[regName] = pseuReg._edi; break;
            default: break; // Ignore ESP/EBP unless necessary
            }
        }
    }


    __asm
    {
        // Clean up pushed args
        mov ecx, numArgs
        shl ecx, 2
        add esp, ecx

        popfd
        popad
    }

    // Clean up dynamically allocated array
    delete[] stackArgs;
}