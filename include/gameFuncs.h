#pragma once

extern std::vector<char*> mainMapDeps;
extern std::unordered_map<char*, std::vector<char*>> depsTree;

extern uint64_t GetMessageA_hook_Tramp;
extern uint64_t DB_LinkXAssetEntry1_hook_Tramp;
extern uint64_t GScr_SetScripts_hook_Tramp;
extern uint64_t GScr_LoadScripts_hook_Tramp;
extern uint64_t Scr_LoadScript_hook_Tramp;
extern uint64_t Scr_LoadScriptInternal_hook_Tramp;
extern uint64_t Scr_EndLoadScripts_hook_Tramp;
extern uint64_t DB_LoadXAssets_hook_Tramp;
extern uint64_t Scr_GetFunctionHandle_hook_Tramp;
extern uint64_t Cmd_ExecuteSingleCommand_hook_Tramp;
extern uint64_t DB_TryLoadXFileInternal_hook_Tramp;
extern uint64_t Scr_LoadLevel_hook_Tramp;
extern uint64_t GScr_PostLoadScripts_hook_Tramp;
extern uint64_t Scr_ExecEntThread_hook_Tramp;
extern uint64_t Scr_ExecThread_hook_Tramp;
extern uint64_t EmitFunction_hook_Tramp;


typedef char __cdecl DB_LoadXAssets_type(XZoneInfo* a1, int zoneCount, int syncMode);
typedef int __cdecl Cmd_ExecuteSingleCommand_type(int localClientNum, int controllerIndex, const char* text);
typedef BOOL __cdecl DB_IsXAssetDefault(unsigned int type, char* name);
typedef int __cdecl DB_CloneXAsset_type(DWORD* from, DWORD* to, DWORD* cloneType);
typedef int __cdecl DB_FindXAssetHeader_type(unsigned int type, char* nameStr, int crtDef);
typedef int __cdecl DB_GetAllXAssetOfType_type(int type, uint32_t* p_XAssetHeader);
typedef void __cdecl DB_GetRawBuffer_type(const RawFile* rawfile, char* buf, int size);
typedef int __cdecl DB_GetXAssetName_type(DWORD* XAsset);
typedef int __cdecl DB_InflateInit_type(bool isSecure);
typedef int* __cdecl DB_SetXAssetName(int* a1, int a2);
typedef int __cdecl DB_TryLoadXFileInternal_type(char* zoneName, int a2);
typedef uint32_t __cdecl DB_LinkXAssetEntry1_type(int type, uint32_t header);
typedef char* __cdecl Dvar_GetString_type(const char* dvarName);
typedef int __cdecl Dvar_RegisterInt_type(const char* dvarName, int value, int min, int max, int flags);
typedef int __cdecl Dvar_SetBoolByName_type(const char* dvarName, bool value);
typedef int __cdecl Dvar_SetFloatByName_type(const char* dvarName, float value);
typedef int __cdecl Dvar_SetIntByName_type(const char* dvarName, int value);
typedef char __cdecl Dvar_SetStringByName_type(const char* dvarName, const char* value);
typedef int __cdecl EmitFunction_type(unsigned int canonName, unsigned int funcID, BYTE* byteCodeBuff);
typedef int __cdecl Com_Error_type(int typ, char* Format, ...);
typedef int __cdecl GScr_LoadScripts_type(const char* mapname, ScriptFunctions* functions);
typedef int __cdecl GScr_PostLoadScripts_type();
typedef int __cdecl GScr_SetScripts_type(ScriptFunctions* p_Functions);
typedef int __cdecl Load_ScriptFile_type(bool a1);
typedef int __cdecl Load_XAssetHeader_type(bool atStreamStart);
typedef int __cdecl Scr_CreateCanonicalFilename_type(const char* filename);
typedef void* Scr_EndLoadScripts_type();
typedef int __cdecl Scr_ExecEntThread_type(int ent, int handle, unsigned int paramcount);
typedef int __cdecl Scr_ExecThread_type(int handle, int paramcount);
typedef int __cdecl Scr_FreeThread_type(unsigned int a1);
typedef int __cdecl Scr_GetFunctionHandle_type(const char* filename, int hardCode);
typedef unsigned int __cdecl Scr_LoadScript_type(const char* filename);
typedef unsigned int __cdecl Scr_LoadScriptInternal_type(const char* filename, uint32_t entries, int entriesCount);
typedef int Scr_LoadLevel_type();
typedef char* __cdecl SL_ConvertToString_type(int stringValue);
typedef unsigned int __cdecl SL_GetCanonicalString_type(char* str);



extern DB_GetAllXAssetOfType_type* DB_GetAllXAssetOfType;
extern Scr_CreateCanonicalFilename_type* Scr_CreateCanonicalFilename;
extern EmitFunction_type* EmitFunction;
extern DB_InflateInit_type* DB_InflateInit;
extern Load_XAssetHeader_type* Load_XAssetHeader;
extern Load_ScriptFile_type* Load_ScriptFile;
extern Com_Error_type* Com_Error;
extern Dvar_GetString_type* Dvar_GetString;
extern Dvar_SetStringByName_type* Dvar_SetStringByName;
extern Dvar_SetFloatByName_type* Dvar_SetFloatByName;
extern Dvar_SetIntByName_type* Dvar_SetIntByName;
extern Dvar_SetBoolByName_type* Dvar_SetBoolByName;
extern Dvar_RegisterInt_type* Dvar_RegisterInt;
extern DB_GetRawBuffer_type* DB_GetRawBuffer;
extern DB_LinkXAssetEntry1_type* DB_LinkXAssetEntry1;
extern Scr_ExecEntThread_type* Scr_ExecEntThread;
extern GScr_SetScripts_type* GScr_SetScripts;
extern Scr_LoadLevel_type* Scr_LoadLevel;
extern Scr_EndLoadScripts_type* Scr_EndLoadScripts;
extern Scr_GetFunctionHandle_type* Scr_GetFunctionHandle;
extern Scr_LoadScript_type* Scr_LoadScript;
extern Scr_LoadScriptInternal_type* Scr_LoadScriptInternal;
extern GScr_LoadScripts_type* GScr_LoadScripts;
extern Cmd_ExecuteSingleCommand_type* Cmd_ExecuteSingleCommand;
extern DB_TryLoadXFileInternal_type* DB_TryLoadXFileInternal;
extern DB_LoadXAssets_type* DB_LoadXAssets;
extern Scr_ExecThread_type* Scr_ExecThread;
extern Scr_FreeThread_type* Scr_FreeThread;
extern GScr_PostLoadScripts_type* GScr_PostLoadScripts;
extern SL_ConvertToString_type* SL_ConvertToString;
extern SL_GetCanonicalString_type* SL_GetCanonicalString;
extern void* Item_RunScript;
extern void* DB_FindXAssetEntry;
extern void* DB_SwapXAsset;



NOINLINE BOOL WINAPI GetMessageA_hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax);
NOINLINE int __cdecl Scr_ExecThread_hook(int handle, int paramcount);
NOINLINE int __cdecl Scr_ExecEntThread_hook(int ent, int handle, unsigned int paramcount);
NOINLINE int __cdecl GScr_PostLoadScripts_hook();
NOINLINE int Scr_LoadLevel_hook();
NOINLINE int __cdecl DB_TryLoadXFileInternal_hook(char* zoneName, int a2);
NOINLINE int __cdecl Cmd_ExecuteSingleCommand_hook(int localClientNum, int controllerIndex, char* text);
NOINLINE int __cdecl Scr_GetFunctionHandle_hook(const char* filename, int hardCode);
NOINLINE char __cdecl DB_LoadXAssets_hook(XZoneInfo* p_zoneInfo, int zoneCount, int syncMode);
NOINLINE void* Scr_EndLoadScripts_hook();
NOINLINE unsigned int __cdecl Scr_LoadScript_hook(char* filename);
NOINLINE unsigned int __cdecl Scr_LoadScriptInternal_hook(char* filename, uint32_t entries, int entriesCount);
NOINLINE int GScr_LoadScripts_hook(const char* mapname, ScriptFunctions* functions); 
NOINLINE int __cdecl GScr_SetScripts_hook(ScriptFunctions* Functions);
NOINLINE int __cdecl DB_LinkXAssetEntry1_hook(int type, uint32_t header);
NOINLINE  int __cdecl EmitFunction_hook(unsigned int canonName, unsigned int funcID, BYTE* byteCodeBuff);
void DB_SwapXAsset_wrap(uint32_t* from, uint32_t* to);
XAssetEntry* findScriptAssetEntry(const char* assetName);


void getAllFuncAOB();
void setupHooks();
