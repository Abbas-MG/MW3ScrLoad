#define _CRT_SECURE_NO_WARNINGS 

#ifndef PCH_H
#define PCH_H

#include "framework.h"
#include "polyhook2/IHook.hpp"
#include "polyhook2/Detour/NatDetour.hpp"

#include <tlhelp32.h>
#include <fstream>
#include <filesystem>
#include <thread>
#include <psapi.h> // For GetModuleInformation
#include "zlib.h"
#include "ini.h"

#include "MW3_Types.h"
#include "gameFuncs.h"
#include "compression.h"
#include "misc.h" 
#include "wrFile.h"

extern DWORD mainThreadId;
extern std::atomic<bool> enterAlert;
extern MODULEINFO MW3Module;

#endif //PCH_H
