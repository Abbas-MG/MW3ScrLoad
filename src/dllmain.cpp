#include "pch.h"

// test
uint32_t* g_load_stream_next_in__stream_itself_too = (uint32_t*)0x00B73F40;
XAssetScriptEntry** varXAsset = (XAssetScriptEntry**)0x00BF4354;

const char* consoleName = "MW3ScrLoad Console";
std::atomic<bool> enterAlert(false);
std::atomic<bool> consoleActive(true);
DWORD mainThreadId = 0;
MODULEINFO MW3Module = {0};
void RunScriptInMainThread(const char* cmdStr);



BOOL WINAPI ConsoleCtrlHandler(DWORD dwCtrlType)
{
    if (dwCtrlType == CTRL_C_EVENT)
    {
        std::cout << "\nReceived CTRL+C. Clearing input buffer.\n";
        std::cin.clear();
        return TRUE;
    }
    return FALSE;
}

void trimCommand(std::string& cmd)
{
    // remove newlines
    cmd.erase(std::remove_if(cmd.begin(), cmd.end(), [](char c)
                  {
                      return c == '\n';
                  }), cmd.end());


    // try to eliminate last redundant ; as it crashes the engine.
    size_t lastSemicolon = cmd.find_last_of(';');
    if (lastSemicolon != std::string::npos)
    {
        bool onlyWhitespaceAfter = std::all_of(
            cmd.begin() + lastSemicolon + 1, cmd.end(),
            [](unsigned char c) { return std::isspace(c); }
        );

        if (onlyWhitespaceAfter)
        {
            cmd.erase(lastSemicolon);
            cmd.erase(cmd.find_last_not_of(" \t\n\r") + 1);
        }
    }

    // fix surrounding spaces for ;
    size_t pos {};
    while (( pos = cmd.find(';', pos) ) != std::string::npos)
    {
        if (pos && cmd.at(pos - 1) != ' ')
            cmd.insert(pos - 1, 1, ' ');
        if (pos + 1 < cmd.size() && cmd.at(pos + 1) != ' ')
            cmd.insert(pos + 1, 1, ' ');

        pos++;
    }
}

void HandleCommand(const std::string& command)
{
    if (command == "exit")
    {
        std::cout << "Exiting console input..." << std::endl;

        consoleActive = false;
    }
    else if (command == "help")
    {
        std::cout << "Available commands:\n";
        std::cout << "  exit  - Stop console input and detach DLL\n";
        std::cout << "  help  - Show this help message\n";
        std::cout << "  MainMapDeps  - Print Main Map dependencies i.e. the scripts it preloads\n";
        std::cout << "  AllDepsToFile  - Write All loaded scripts with their dependencies to file\n";
        std::cout << "  ExecCommand  - Execute the given command string\n";
        std::cout << "  ExecCommandFile  - Execute command string from file\n";
    }
    else if (command == "MainMapDeps")
    {
        for (auto dep : mainMapDeps)
        {
            std::cout << dep << "\n";
        }
    }
    else if (command.rfind("AllDepsToFile ", 0) == 0)
    {
        auto fileName = command.substr(std::string("AllDepsToFile ").length());
        depsTreePrintToFile(depsTree, fileName, 1);
    }
    else if (command.rfind("ExecCommand ", 0) == 0)
    {
        auto param = command.substr(std::string("ExecCommand ").length());
        trimCommand(param);
        RunScriptInMainThread(_strdup(param.c_str()));
    }
    else if (command.rfind("ExecCommandFile ", 0) == 0)
    {
        auto fileName = command.substr(std::string("ExecCommandFile ").length());

        std::ifstream cmdFile;
        cmdFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

        try
        {
            cmdFile.open(fileName);
            if (cmdFile.is_open() && cmdFile.good())
            {
                std::string content(( std::istreambuf_iterator<char>(cmdFile) ),
                                    std::istreambuf_iterator<char>());

                trimCommand(content);

                RunScriptInMainThread(_strdup(content.c_str()));
            }
            else 
            {
                std::cerr << "Error: Could not open file " << fileName << std::endl;
            }
            
            cmdFile.close();
        }
        catch (const std::ifstream::failure& e)
        {
            std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
        }
    }
    else
    {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type 'help' for a list of commands." << std::endl;
    }
}

void ConsoleInputThread()
{
    std::string command;

    std::cout << "Enter commands (type 'help' for a list of commands):" << std::endl;

    while (consoleActive)
    {
        SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);

        std::cout << "> ";
        std::cout.flush();

        std::getline(std::cin, command);

        if (!command.empty())
        {
            std::thread cmdHandle(HandleCommand, command);
            cmdHandle.detach();
        }
    }

    std::cout << "Console input thread terminated." << std::endl;
}

DWORD WINAPI Item_RunScript_wrap(LPVOID lpParam)
{
    const char* cmdStr = (const char*)lpParam;
    std::unordered_map<std::string, uint32_t> regArgs = {
        {"ebx", 0},
        {"edi", 0}
    };
    std::unordered_map<std::string, uint32_t> retRegs = {{ "eax", 0 }} ;
    CustomCall(Item_RunScript,
               regArgs,
               retRegs,
               1,
               cmdStr);

    std::cout << "Script string command executed." << std::endl;

    uint32_t ret = 0;
    if (!retRegs.empty())
    {
        ret = retRegs.begin()->second;
    }

    delete[] cmdStr;
    return ret;
}

// Main function to transfer execution to the main thread
// pass a solid cstr to it; not .c_str() or any scope life
void RunScriptInMainThread(const char* cmdStr)
{
    HANDLE hMainThread = OpenThread(THREAD_SET_CONTEXT | THREAD_SUSPEND_RESUME, FALSE, mainThreadId);
    if (hMainThread == NULL)
    {
        std::cerr << "Failed to open the main thread!" << std::endl;
        return;
    }

    // SuspendThread(hMainThread);

    // Queue our function to execute in the main thread
    QueueUserAPC((PAPCFUNC)Item_RunScript_wrap, hMainThread, (ULONG_PTR)cmdStr);
    enterAlert = true;

    // ResumeThread(hMainThread);

    CloseHandle(hMainThread);
}

DWORD WINAPI init_thread(LPVOID)
{
    iniSetup();

    mainThreadId = GetMainThreadId();

    getAllFuncAOB();

    // install hooks
    PLH::NatDetour Scr_GetFunctionHandle_detour =
        PLH::NatDetour((uint64_t)Scr_GetFunctionHandle, (uint64_t)Scr_GetFunctionHandle_hook, &Scr_GetFunctionHandle_hook_Tramp);
    Scr_GetFunctionHandle_detour.hook();

    PLH::NatDetour LdXAssets_detour =
        PLH::NatDetour((uint64_t)DB_LoadXAssets, (uint64_t)DB_LoadXAssets_hook, &DB_LoadXAssets_hook_Tramp);
    LdXAssets_detour.hook();


    PLH::NatDetour Scr_EndLoadScripts_detour =
        PLH::NatDetour((uint64_t)Scr_EndLoadScripts, (uint64_t)Scr_EndLoadScripts_hook, &Scr_EndLoadScripts_hook_Tramp);
    Scr_EndLoadScripts_detour.hook();


    PLH::NatDetour GScr_LoadScripts_detour =
        PLH::NatDetour((uint64_t)GScr_LoadScripts, (uint64_t)GScr_LoadScripts_hook, &GScr_LoadScripts_hook_Tramp);
    GScr_LoadScripts_detour.hook();


    PLH::NatDetour cmdExec_detour =
        PLH::NatDetour((uint64_t)Cmd_ExecuteSingleCommand, (uint64_t)Cmd_ExecuteSingleCommand_hook,
                       &Cmd_ExecuteSingleCommand_hook_Tramp);
    cmdExec_detour.hook();

    PLH::NatDetour lnkAss_detour = PLH::NatDetour((uint64_t)DB_LinkXAssetEntry1, (uint64_t)DB_LinkXAssetEntry1_hook,
                                                  &DB_LinkXAssetEntry1_hook_Tramp);
    lnkAss_detour.hook();

    PLH::NatDetour LdScr_detour =
        PLH::NatDetour((uint64_t)Scr_LoadScript, (uint64_t)Scr_LoadScript_hook, &Scr_LoadScript_hook_Tramp);
    LdScr_detour.hook();

    PLH::NatDetour LdScrInter_detour =
        PLH::NatDetour((uint64_t)Scr_LoadScriptInternal, (uint64_t)Scr_LoadScriptInternal_hook, &Scr_LoadScriptInternal_hook_Tramp);
    LdScrInter_detour.hook();


    PLH::NatDetour DB_TryLoadXFileInternal_detour =
        PLH::NatDetour((uint64_t)DB_TryLoadXFileInternal, (uint64_t)DB_TryLoadXFileInternal_hook, &DB_TryLoadXFileInternal_hook_Tramp);
    DB_TryLoadXFileInternal_detour.hook();


    PLH::NatDetour GScr_SetScripts_detour =
        PLH::NatDetour((uint64_t)GScr_SetScripts, (uint64_t)GScr_SetScripts_hook, &GScr_SetScripts_hook_Tramp);
    GScr_SetScripts_detour.hook();


    PLH::NatDetour Scr_LoadLevel_detour =
        PLH::NatDetour((uint64_t)Scr_LoadLevel, (uint64_t)Scr_LoadLevel_hook, &Scr_LoadLevel_hook_Tramp);
    Scr_LoadLevel_detour.hook();


    PLH::NatDetour Scr_ExecThread_detour =
        PLH::NatDetour((uint64_t)Scr_ExecThread, (uint64_t)Scr_ExecThread_hook, &Scr_ExecThread_hook_Tramp);
    Scr_ExecThread_detour.hook();


    PLH::NatDetour Scr_ExecEntThread_detour =
        PLH::NatDetour((uint64_t)Scr_ExecEntThread, (uint64_t)Scr_ExecEntThread_hook, &Scr_ExecEntThread_hook_Tramp);
    Scr_ExecEntThread_detour.hook();


    PLH::NatDetour GScr_PostLoadScripts_detour =
        PLH::NatDetour((uint64_t)GScr_PostLoadScripts, (uint64_t)GScr_PostLoadScripts_hook, &GScr_PostLoadScripts_hook_Tramp);
    GScr_PostLoadScripts_detour.hook();


    PLH::NatDetour EmitFunction_detour =
        PLH::NatDetour((uint64_t)EmitFunction, (uint64_t)EmitFunction_hook, &EmitFunction_hook_Tramp);
    EmitFunction_detour.hook();

    PLH::NatDetour GetMessageA_detour = PLH::NatDetour((uint64_t)&GetMessageA, (uint64_t)GetMessageA_hook, &GetMessageA_hook_Tramp);
    GetMessageA_detour.hook();


    // loop for key press
    while (consoleActive)
    {
        if (GetAsyncKeyState(VK_NUMPAD5) & GetAsyncKeyState(VK_CONTROL) & 0x8000)
        {
            // do anything on CTRL+num5 e.g. calling Item_RunScript_wrap
        }

        Sleep(300);
    }

    // restore the hook points on exit
    Scr_GetFunctionHandle_detour.unHook();
    LdXAssets_detour.unHook();
    Scr_EndLoadScripts_detour.unHook();
    GScr_LoadScripts_detour.unHook();
    cmdExec_detour.unHook();
    lnkAss_detour.unHook();
    LdScr_detour.unHook();
    LdScrInter_detour.unHook();
    DB_TryLoadXFileInternal_detour.unHook();
    GScr_SetScripts_detour.unHook();
    Scr_LoadLevel_detour.unHook();
    Scr_ExecThread_detour.unHook();
    Scr_ExecEntThread_detour.unHook();
    GScr_PostLoadScripts_detour.unHook();
    EmitFunction_detour.unHook();
    GetMessageA_detour.unHook();

    Sleep(100);

    FreeConsole();

    // FreeConsole is not closing the console wnd so:
    HWND consoleWindow = FindWindowA(NULL, consoleName);
    if (consoleWindow)
    {
        PostMessage(consoleWindow, WM_CLOSE, 0, 0);
    }

    return EXIT_SUCCESS;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    {
        if (AllocConsole())
        {
            FILE* fOut, * fIn;
            freopen_s(&fOut, "CONOUT$", "w", stdout);
            freopen_s(&fOut, "CONOUT$", "w", stderr);
            freopen_s(&fIn, "CONIN$", "r", stdin);

            if (!SetConsoleTitleA(consoleName))
            {
                std::cerr << "SetConsoleTitle failed(% d)\n" << std::endl;
            }

            std::thread(ConsoleInputThread).detach();

            std::cout << "Console allocated successfully!" << std::endl;
        }
        else
        {
            MessageBoxA(nullptr, "Failed to allocate console.", "Error", MB_OK | MB_ICONERROR);
        }


        HMODULE hModule = GetModuleHandleA(NULL);
        if (!hModule)
        {
            std::cerr << "Couldn't get MW3 module's handle." << std::endl;
        }

        if (!GetModuleInformation(GetCurrentProcess(), hModule, &MW3Module, sizeof(MW3Module)))
        {
            std::cerr << "Couldn't get MW3 module's information." << std::endl;
        }


        // patch games code to ease the debugging
        // *(BYTE*)0x006C1D0F = 0x90; // patch int3
        // *(BYTE*)0x0052A8FA = 0x90; // patch jz to jmp safemode window
#ifdef _DEBUG
        const char* _int3_aob = "89 ?? ?? 8b ?? ?? cc 64 ?? ?? ?? ?? ?? ?? 83 ?? ?? 5E 8B ?? 5D C3";
        void* _int3_loc = (void*)( aobSearch(_int3_aob, "_int3_patch") + 6 );

        const char* safemodeWnd_aob = "83 ?? ?? 80 ?? ?? ?? ?? ?? ?? 75 ?? B8 ?? ?? ?? ?? 83";
        void* safemodeWnd_loc = (void*)( aobSearch(safemodeWnd_aob, "_int3_patch") + 10 );

        DWORD oldProtection;
        if (VirtualProtect(_int3_loc, 1, PAGE_READWRITE, &oldProtection))
        {
            std::memset(_int3_loc, 0x90, 1);
            VirtualProtect(_int3_loc, 1, oldProtection, &oldProtection);
        }

        oldProtection;
        if (VirtualProtect(safemodeWnd_loc, 1, PAGE_READWRITE, &oldProtection))
        {
            std::memset(safemodeWnd_loc, 0x90, 2);
            VirtualProtect(safemodeWnd_loc, 1, oldProtection, &oldProtection);
        }
#endif


        CreateThread((LPSECURITY_ATTRIBUTES)0, 0, (LPTHREAD_START_ROUTINE)init_thread, (LPVOID)hModule, 0,
                     (LPDWORD)NULL);

        break;
    }

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}
