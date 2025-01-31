#include "pch.h"


char* mainMapName = new char[200];
char* mainMapFullName = new char[200];
const char* customScript = "sample";

extern IniOpt iniOpt;

std::vector<char*> mainMapDeps;
std::unordered_map<char*, std::vector<char*>> depsTree;

std::vector<std::string> filesToGetHdl;
std::vector<int> funcHandles;

extern MODULEINFO MW3Module;
// main Map Script XAsset
uint32_t mainMapScrAss;
size_t mapScriptUncompOrijSz;


// these are in test phase and for editing script's buffer
char* g_ScriptBuffer_buff = nullptr;
const uintptr_t p_g_ScriptBuffer_buff = 0x01BF2578;
uint32_t p_g_ScriptBuffer_buff_save = 0;
size_t g_ScriptBuffer_buff_idx = 0; // 0x01BEFCF8
char* g_ScriptBuffer_buff_save = nullptr;
static bool funcLinked = false;
static bool DB_LoadXAssetsLoadOnce = false;


uint64_t GetMessageA_hook_Tramp = NULL;
uint64_t DB_LinkXAssetEntry1_hook_Tramp = NULL;
uint64_t GScr_SetScripts_hook_Tramp = NULL;
uint64_t GScr_LoadScripts_hook_Tramp = NULL;
uint64_t Scr_LoadScript_hook_Tramp = NULL;
uint64_t Scr_LoadScriptInternal_hook_Tramp = NULL;
uint64_t Scr_EndLoadScripts_hook_Tramp = NULL;
uint64_t DB_LoadXAssets_hook_Tramp = NULL;
uint64_t Scr_GetFunctionHandle_hook_Tramp = NULL;
uint64_t Cmd_ExecuteSingleCommand_hook_Tramp = NULL;
uint64_t DB_TryLoadXFileInternal_hook_Tramp = NULL;
uint64_t Scr_LoadLevel_hook_Tramp = NULL;
uint64_t GScr_PostLoadScripts_hook_Tramp = NULL;
uint64_t Scr_ExecEntThread_hook_Tramp = NULL;
uint64_t Scr_ExecThread_hook_Tramp = NULL;
uint64_t EmitFunction_hook_Tramp = NULL;


DB_LoadXAssets_type* DB_LoadXAssets;
Cmd_ExecuteSingleCommand_type* Cmd_ExecuteSingleCommand;
Com_Error_type* Com_Error;
DB_GetAllXAssetOfType_type* DB_GetAllXAssetOfType;
DB_GetRawBuffer_type* DB_GetRawBuffer;
DB_InflateInit_type* DB_InflateInit;
DB_LinkXAssetEntry1_type* DB_LinkXAssetEntry1;
DB_TryLoadXFileInternal_type* DB_TryLoadXFileInternal;
Dvar_GetString_type* Dvar_GetString;
Dvar_RegisterInt_type* Dvar_RegisterInt;
Dvar_SetBoolByName_type* Dvar_SetBoolByName;
Dvar_SetFloatByName_type* Dvar_SetFloatByName;
Dvar_SetIntByName_type* Dvar_SetIntByName;
Dvar_SetStringByName_type* Dvar_SetStringByName;
EmitFunction_type* EmitFunction;
GScr_LoadScripts_type* GScr_LoadScripts;
GScr_PostLoadScripts_type* GScr_PostLoadScripts;
GScr_SetScripts_type* GScr_SetScripts;
Load_ScriptFile_type* Load_ScriptFile;
Load_XAssetHeader_type* Load_XAssetHeader;
Scr_CreateCanonicalFilename_type* Scr_CreateCanonicalFilename;
Scr_EndLoadScripts_type* Scr_EndLoadScripts;
Scr_ExecEntThread_type* Scr_ExecEntThread;
Scr_ExecThread_type* Scr_ExecThread;
Scr_FreeThread_type* Scr_FreeThread;
Scr_GetFunctionHandle_type* Scr_GetFunctionHandle;
Scr_LoadLevel_type* Scr_LoadLevel;
Scr_LoadScript_type* Scr_LoadScript;
Scr_LoadScriptInternal_type* Scr_LoadScriptInternal;
SL_ConvertToString_type* SL_ConvertToString;
SL_GetCanonicalString_type* SL_GetCanonicalString;

// these are optimized functions so we call them using wraper
void* Item_RunScript;
void* DB_FindXAssetEntry;
void* DB_SwapXAsset;


void getAllFuncAOB()
{
    const char* DB_SwapXAsset_aob = "55 8B ?? 83 ?? ?? 8B ?? 8B ?? ?? ?? ?? ?? ?? 81 ?? ?? ?? ?? ?? 53";
    DB_SwapXAsset = (void*)aobSearch(DB_SwapXAsset_aob, "DB_SwapXAsset");

    const char* Item_RunScript_aob = "81 ?? ?? ?? ?? ?? 55 8B ?? ?? ?? ?? ?? ?? 85 ?? 0F ?? ?? ?? ?? ?? 80";
    Item_RunScript = (void*)aobSearch(Item_RunScript_aob, "Item_RunScript");

    const char* DB_FindXAssetEntry_aob = "53 8B ?? ?? ?? 56 8B ?? 8B ?? E8";
    DB_FindXAssetEntry = (void*)aobSearch(DB_FindXAssetEntry_aob, "DB_FindXAssetEntry");

    const char* DB_LoadXAssets_aob = "83 ec 0c 8b ?? ?? 18 33 ?? 83 ?? 03 0F 94 ?? 33 ?? 83 ?? 02 0f 94 ?? ?? 8b ?? 89 ?? ?? 08 8b";
    DB_LoadXAssets = (DB_LoadXAssets_type*)aobSearch(DB_LoadXAssets_aob, "DB_LoadXAssets");

    const char* Cmd_ExecuteSingleCommand_aob = "8B ?? ?? ?? B8 ?? ?? ?? ?? 2B ?? ?? ?? ?? ?? 56 50 51 B9 ?? ?? ?? ?? BE ?? ?? ?? ?? E8 ?? ?? ?? ?? A1";
    Cmd_ExecuteSingleCommand = ( Cmd_ExecuteSingleCommand_type* )aobSearch(Cmd_ExecuteSingleCommand_aob, "Cmd_ExecuteSingleCommand");

    const char* Com_Error_aob = "E8 ?? ?? ?? ?? 84 ?? 74 ?? E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? A3 ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A ?? E8 ?? ?? ?? ?? 83 ?? ?? E8 ?? ?? ?? ?? 84 ?? 74 ?? E8 ?? ?? ?? ?? 83";
    Com_Error = (Com_Error_type*)aobSearch(Com_Error_aob, "Com_Error");

    const char* DB_GetAllXAssetOfType_aob = "53 56 57 68 ?? ?? ?? ?? 33 ?? FF ?? ?? ?? ?? ?? A1";
    DB_GetAllXAssetOfType = (DB_GetAllXAssetOfType_type*)aobSearch(DB_GetAllXAssetOfType_aob, "DB_GetAllXAssetOfType");

    const char* DB_GetRawBuffer_aob = "83 ?? ?? 53 56 57 68 ?? ?? ?? ?? 8D ?? ?? ?? E8 ?? ?? ?? ?? 8D ?? ?? ?? E8 ?? ?? ?? ?? 8B ?? ?? ?? 8B";
    DB_GetRawBuffer = (DB_GetRawBuffer_type*)aobSearch(DB_GetRawBuffer_aob, "DB_GetRawBuffer");

    const char* DB_InflateInit_aob = "E8 ?? ?? ?? ?? A1 ?? ?? ?? ?? 8B ?? ?? ?? 83 ?? ?? 50";
    DB_InflateInit = (DB_InflateInit_type*)aobSearch(DB_InflateInit_aob, "DB_InflateInit");

    const char* DB_LinkXAssetEntry1_aob = "83 ?? ?? 8B ?? ?? ?? 8B ?? 53 55 8b";
    DB_LinkXAssetEntry1 = (DB_LinkXAssetEntry1_type*)aobSearch(DB_LinkXAssetEntry1_aob, "DB_LinkXAssetEntry1");
    
    const char* DB_TryLoadXFileInternal_aob = "81 ?? ?? ?? ?? ?? 55 8B ?? ?? ?? ?? ?? ?? 56 57 55";
    DB_TryLoadXFileInternal = (DB_TryLoadXFileInternal_type*)aobSearch(DB_TryLoadXFileInternal_aob, "DB_TryLoadXFileInternal");
    if (!DB_TryLoadXFileInternal)
    {
        // PUSH instructions in DB_TryLoadXFileInternal are inconsistant through versions so if not found, try getting it from DB_TryLoadXFile
        const char* DB_TryLoadXFileInternal_aob = "8B ?? 40 50 56 e8 ?? ?? ?? ?? 83 ?? 08 85 ?? 74";
        auto DB_TryLoadXFile = aobSearch(DB_TryLoadXFileInternal_aob, "DB_TryLoadXFile");
        DB_TryLoadXFileInternal = (DB_TryLoadXFileInternal_type*)( ( DB_TryLoadXFile + 0x10 ) + *( (uint64_t*)( DB_TryLoadXFile + 6 ) ) );
        if (DB_TryLoadXFileInternal)
            std::cout << "Tried to retrieve DB_TryLoadXFileInternal from DB_TryLoadXFile." << std::endl;
    }

    const char* Dvar_GetString_aob = "57 8B ?? ?? ?? E8 ?? ?? ?? ?? 5F 85 ?? 75 ?? B8";
    Dvar_GetString = (Dvar_GetString_type*)aobSearch(Dvar_GetString_aob, "Dvar_GetString");

    const char* Dvar_RegisterInt_aob = "8B ?? ?? ?? 8B ?? ?? ?? 8B ?? ?? ?? 83 ?? ?? 53 8B ?? ?? ?? 52";
    Dvar_RegisterInt = (Dvar_RegisterInt_type*)aobSearch(Dvar_RegisterInt_aob, "Dvar_RegisterInt");

    const char* Dvar_SetBoolByName_aob = "83 ?? ?? 53 57 8B ?? ?? ?? E8 ?? ?? ?? ?? 85 ?? 74 ?? 8B ?? ?? ?? 6A ?? 51 50 E8 ?? ?? ?? ?? 83 ?? ?? 5F 5B 83 ?? ?? C3 80";
    Dvar_SetBoolByName = (Dvar_SetBoolByName_type*)aobSearch(Dvar_SetBoolByName_aob, "Dvar_SetBoolByName");

    const char* Dvar_SetFloatByName_aob = "83 ?? ?? 53 57 8B ?? ?? ?? E8 ?? ?? ?? ?? 85 ?? 74 ?? D9 ?? ?? ?? 6A ?? 51";
    Dvar_SetFloatByName = (Dvar_SetFloatByName_type*)aobSearch(Dvar_SetFloatByName_aob, "Dvar_SetFloatByName");

    const char* Dvar_SetIntByName_aob = "83 ?? ?? 53 57 8B ?? ?? ?? E8 ?? ?? ?? ?? 85 ?? 74 ?? 8B ?? ?? ?? 6A ?? 51 50 E8 ?? ?? ?? ?? 83 ?? ?? 5F 5B 83 ?? ?? C3 8B";
    Dvar_SetIntByName = (Dvar_SetIntByName_type*)aobSearch(Dvar_SetIntByName_aob, "Dvar_SetIntByName");

    const char* Dvar_SetStringByName_aob = "83 ?? ?? 53 56 57 8B ?? ?? ?? E8 ?? ?? ?? ?? 8B ?? 85 ?? 74";
    Dvar_SetStringByName = (Dvar_SetStringByName_type*)aobSearch(Dvar_SetStringByName_aob, "Dvar_SetStringByName");

    const char* EmitFunction_aob = "83 ?? ?? 56 8B ?? ?? ?? 57 8D ?? ?? ?? 50 8D";
    EmitFunction = (EmitFunction_type*)aobSearch(EmitFunction_aob, "EmitFunction");

    const char* GScr_LoadScripts_aob = "83 ?? ?? 56 57 E8 ?? ?? ?? ?? 68 ?? ?? ?? ?? 68 ?? ?? ?? ?? 8D";
    GScr_LoadScripts = (GScr_LoadScripts_type*)aobSearch(GScr_LoadScripts_aob, "GScr_LoadScripts");

    const char* GScr_PostLoadScripts_aob = "56 33 ?? 56 E8 ?? ?? ?? ?? 46 83 ?? ?? 83 ?? ?? 7C ?? E8";
    GScr_PostLoadScripts = (GScr_PostLoadScripts_type*)aobSearch(GScr_PostLoadScripts_aob, "GScr_PostLoadScripts");

    const char* GScr_SetScripts_aob = "56 57 E8 ?? ?? ?? ?? 8B ?? ?? ?? 84 ?? 75";
    GScr_SetScripts = (GScr_SetScripts_type*)aobSearch(GScr_SetScripts_aob, "GScr_SetScripts");

    const char* Load_ScriptFile_aob = "A1 ?? ?? ?? ?? 8B ?? ?? ?? 6A ?? 50 51 E8 ?? ?? ?? ?? 6A ?? E8 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 6A ?? 89 ?? ?? ?? ?? ?? E8 ?? ?? ?? ?? 6A";
    Load_ScriptFile = (Load_ScriptFile_type*)aobSearch(Load_ScriptFile_aob, "Load_ScriptFile");

    const char* Load_XAssetHeader_aob = "A1 ?? ?? ?? ?? 8B ?? ?? ?? 6A ?? 50 51 E8 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 83 ?? ?? 83 ?? ?? 89";
    Load_XAssetHeader = (Load_XAssetHeader_type*)aobSearch(Load_XAssetHeader_aob, "Load_XAssetHeader");

    const char* Scr_CreateCanonicalFilename_aob = "8B ?? ?? ?? 81 ?? ?? ?? ?? ?? 68 ?? ?? ?? ?? 50";
    Scr_CreateCanonicalFilename = (Scr_CreateCanonicalFilename_type*)aobSearch(Scr_CreateCanonicalFilename_aob, "Scr_CreateCanonicalFilename");

    const char* Scr_EndLoadScripts_aob = "53 E8 ?? ?? ?? ?? 6A ?? E8 ?? ?? ?? ?? A1";
    Scr_EndLoadScripts = (Scr_EndLoadScripts_type*)aobSearch(Scr_EndLoadScripts_aob, "Scr_EndLoadScripts");

    const char* Scr_ExecEntThread_aob = "8B ?? ?? ?? 8B ?? ?? ?? 8B ?? ?? ?? 50 0F";
    Scr_ExecEntThread = (Scr_ExecEntThread_type*)aobSearch(Scr_ExecEntThread_aob, "Scr_ExecEntThread");

    const char* Scr_LoadLevel_aob = "A1 ?? ?? ?? ?? 85 ?? 74 ?? 6A ?? 50 E8 ?? ?? ?? ?? 0F";
    Scr_LoadLevel = (Scr_LoadLevel_type*)aobSearch(Scr_LoadLevel_aob, "Scr_LoadLevel");

    
    // Scr_ExecThread and Scr_AddExecThread are identical
    const char* Scr_ExecThread_aob = "8B ?? ?? ?? 8B ?? ?? ?? ?? ?? 8B ?? ?? ?? 50 51 52 E8 ?? ?? ?? ?? 83 ?? ?? C3";
    Scr_ExecThread = (Scr_ExecThread_type*)aobSearch(Scr_ExecThread_aob, "Scr_ExecThread");
    // so we do relative calculation to get Scr_ExecThread from Scr_LoadLevel's body
    Scr_ExecThread = (Scr_ExecThread_type*)(( (uint32_t)Scr_LoadLevel + 0x11 ) + *( (uint32_t*)( (uint32_t)Scr_LoadLevel + 0xd )));


    const char* Scr_FreeThread_aob = "0F B7 ?? 24 04 89 ?? 24 04 e9";
    Scr_FreeThread = (Scr_FreeThread_type*)aobSearch(Scr_FreeThread_aob, "Scr_FreeThread");

    const char* Scr_GetFunctionHandle_aob = "8B ?? ?? ?? 83 ?? ?? 53 56 57 50 E8 ?? ?? ?? ?? 8B ?? ?? ?? ?? ?? 8B ?? 57";
    Scr_GetFunctionHandle = (Scr_GetFunctionHandle_type*)aobSearch(Scr_GetFunctionHandle_aob, "Scr_GetFunctionHandle");

    const char* Scr_LoadScript_aob = "83 ?? ?? 56 68 ?? ?? ?? ?? 8D ?? ?? ?? E8 ?? ?? ?? ?? 8D ?? ?? ?? E8 ?? ?? ?? ?? 6A";
    Scr_LoadScript = (Scr_LoadScript_type*)aobSearch(Scr_LoadScript_aob, "Scr_LoadScript");

    const char* Scr_LoadScriptInternal_aob = "8B ?? ?? ?? 83 ?? ?? 56 57 50 E8 ?? ?? ?? ?? 8B";
    Scr_LoadScriptInternal = (Scr_LoadScriptInternal_type*)aobSearch(Scr_LoadScriptInternal_aob, "Scr_LoadScriptInternal");

    const char* SL_ConvertToString_aob = "8B ?? ?? ?? 85 ?? 74 ?? 8B ?? ?? ?? ?? ?? C1 ?? ?? 8D ?? ?? ?? C3 33";
    SL_ConvertToString = (SL_ConvertToString_type*)aobSearch(SL_ConvertToString_aob, "SL_ConvertToString");

    const char* SL_GetCanonicalString_aob = "51 53 55 56 8B ?? ?? ?? 8D";
    SL_GetCanonicalString = (SL_GetCanonicalString_type*)aobSearch(SL_GetCanonicalString_aob, "SL_GetCanonicalString");
}

// this function is used for inserting extra bytecodes for script load
void bytecodeFindAndInsert(char* buffer, size_t& size, size_t capacity, const unsigned char* searchSequence, size_t searchSize)
{
    // Validate inputs
    if (searchSize == 0 || searchSize > size)
    {
        std::cerr << "Invalid search sequence size." << std::endl;
        return;
    }


    // Search for the byte sequence in the buffer
    for (size_t i = 0; i <= size - searchSize; ++i)
    {
        // Check if the current slice of `buffer` matches `searchSequence`
        if (std::memcmp(buffer + i, searchSequence, searchSize) == 0)
        {
            // Ensure we don't exceed the buffer capacity
            if (size + 5 <= capacity) // Only need space for the 4 bytes
            {
                // Calculate the position where we will insert the 4-byte gap
                size_t insertPos = i + searchSize + 5; // +5 to skip 93 28 XX XX XX

                // Shift the buffer to make space for the gap
                std::memmove(buffer + insertPos, buffer + i + searchSize, size - ( i + searchSize ));

                // Update the buffer size
                size += 5;

            }
            else
            {
                std::cerr << "Not enough space to insert bytes." << std::endl;
            }
            return; // Exit after processing
        }
    }

    std::cout << "Sequence not found." << std::endl;
}

void modifyBufferNum(char* start, size_t size, char* current, const uint8_t* customBytes)
{
    if (!start || !current || !customBytes) return;

    // Calculate end of the buffer
    char* end = start + size;

    // Calculate number of bytes to move (from current to end)
    size_t bytesToMove = end - current;

    // Move data 4 bytes forward from `current` to `end`
    memmove(current + 4, current, bytesToMove);

    // Write the custom 4 bytes after the created space; so parser picks it up it next iteration
    memcpy(current, customBytes, 4);
}

void concatenateToLittleEndian(uint16_t canonName, uint16_t funcID, uint8_t customBytes[4])
{
    // Reconstruct hexadecimal for the iw5 parser to read
    char buff[10];
    snprintf(buff, 10, "%x", canonName);
    uint16_t canonNameHex = strtol(buff, nullptr, 10);
    // snprintf(buff, 10, "%x", xxx);

    // Combine the two 2-byte values into a 4-byte value (little-endian)
    uint32_t combined = ( static_cast<uint32_t>( funcID ) << 16 ) | canonNameHex;

    // Write the 4 bytes of the combined value to the array in little-endian order
    customBytes[0] = static_cast<uint8_t>( combined & 0xFF );        // Least significant byte
    customBytes[1] = static_cast<uint8_t>( ( combined >> 8 ) & 0xFF );
    customBytes[2] = static_cast<uint8_t>( ( combined >> 16 ) & 0xFF );
    customBytes[3] = static_cast<uint8_t>( ( combined >> 24 ) & 0xFF );
}

void modifyBufferName(char* start, size_t& size, char* current, const uint8_t* customBytes, size_t customBytesSz)
{
    if (!start || !current || !customBytes) return;

    // Calculate end of the buffer
    char* end = start + size;

    // Calculate number of bytes to move (from current to end)
    size_t bytesToMove = end - current;

    // Move data 4 bytes forward from `current` to `end`
    memmove(current + customBytesSz, current, bytesToMove);

    // Write the custom 4 bytes after the created space; so parser picks it up it next iteration
    memcpy(current, customBytes, customBytesSz);
    size += customBytesSz;
    // memset(start + size, 0, 1);
}

void modifyBuffer(const char* scriptName)
{
    auto p_mapScriptXAss = (XAssetScriptEntry*)mainMapScrAss;
    std::vector<uint8_t> sample_unComp = uncompressData(p_mapScriptXAss->scriptFile->buffer, p_mapScriptXAss->scriptFile->compressedLen);

    if (sample_unComp.size() != mapScriptUncompOrijSz)
    {
        char* msg1 = (char*)"Main Map's bytecode polluted:\n %s\n";
        int ret1 = Com_Error(1, msg1, "This should not happen at all.", 2);
    }

    size_t size = mapScriptUncompOrijSz;  // Size of the buffer

    auto g_canonName = Scr_CreateCanonicalFilename(customScript);
    auto g_funcID = ScriptFuncName::main; // should be general so user can use any funcName

    // Custom 4-byte data to write
    uint8_t customBytes[4];

    concatenateToLittleEndian(g_canonName, g_funcID, customBytes);

    // Modify the buffer if script name is numerical
    // modifyBufferNum(start, size, current, customBytes);

    uint8_t customBytesName[20] = {0};  // Adjust size as necessary
    size_t customBytesNameSz = 0;

    //uint16_t funcID_LE = ( g_funcID >> 8 ) | ( g_funcID << 8 );
    customBytesNameSz += strlen(scriptName);
    strncpy(reinterpret_cast<char*>( customBytesName + 2 ), scriptName, customBytesNameSz);  // Ensure we don't overflow
    customBytesNameSz += 2;
    // Null-terminate the string manually
    customBytesName[customBytesNameSz] = '\0';  // Safety measure
    customBytesNameSz += 1;
    strncpy(reinterpret_cast<char*>( customBytesName + customBytesNameSz ), (char*)&g_funcID, 2);
    customBytesNameSz += 2;

    g_ScriptBuffer_buff = (char*)*(uint32_t*)p_g_ScriptBuffer_buff;

    size_t length = size + customBytesNameSz;
    g_ScriptBuffer_buff_save = new char[length];
    std::memcpy(g_ScriptBuffer_buff_save, g_ScriptBuffer_buff, length);
    *(uint32_t*)p_g_ScriptBuffer_buff = (uintptr_t)g_ScriptBuffer_buff_save;

    // trying not to pollute global ScriptBuffer's data
    g_ScriptBuffer_buff = (char*)*(uint32_t*)p_g_ScriptBuffer_buff;
    g_ScriptBuffer_buff_idx = *(uint32_t*)0x01BEFCF8; // update AOB

    char* current = g_ScriptBuffer_buff + g_ScriptBuffer_buff_idx;   // Current position (middle of the buffer)
    char* start = g_ScriptBuffer_buff;

    modifyBufferName(start, size, current, customBytesName, customBytesNameSz);

    funcLinked = true;
}




XAssetEntry* findScriptAssetEntry(const char* assetName)
{
    std::unordered_map<std::string, uint32_t> FindXAss_regArgs = {
        {"edi", 0x27}
    };
    std::unordered_map<std::string, uint32_t> FindXAss_RetRegs = {{ "eax", 0 }};
    CustomCall(DB_FindXAssetEntry,
               FindXAss_regArgs,
               FindXAss_RetRegs,
               1,
               assetName);

    if (!FindXAss_RetRegs.empty())
    {
        return (XAssetEntry*)FindXAss_RetRegs.begin()->second;
    }
    else
    {
        return 0;
    }
}

void printAllScriptAss(bool sorted = false)
{
    // get all script assets
    ScriptFile* xassets[901] {};
    uint32_t asstNum = DB_GetAllXAssetOfType(XAssetType::ASSET_TYPE_SCRIPTFILE, (uint32_t*)xassets);

    // get all script names
    std::vector<std::string> xassetsNames {};
    for (uint32_t i = 0; i < asstNum; i++)
    {
        xassetsNames.push_back(xassets[i]->filename);
    }

    if (sorted)
    {
        // sort them
        auto xassetsNamesSorted = xassetsNames;
        sort(xassetsNamesSorted.begin(), xassetsNamesSorted.end());
        // print 
        return;
    }
    
    // print unsorted
    return;

    // can also do:
    /*
    // get all script assets into a vector
    std::vector<ScriptFile*> xAssetsSorted;
    for (int i = 0; i < asstNum; i++)
    {
        xAssetsSorted.push_back(xassets[i]);
    }

    // sort them
    std::sort(xAssetsSorted.begin(), xAssetsSorted.end(), [](const ScriptFile* a, const ScriptFile* b)
              {
                  return std::strcmp(a->filename, b->filename) < 0;
              });

    // try to find a specific script
    const char* searchName = "maps/so_survival_mp_paris";
    auto it = std::find_if(xAssetsSorted.begin(), xAssetsSorted.end(), [searchName](const ScriptFile* element)
                           {
                               return std::strcmp(element->filename, searchName) == 0;
                           });

    // found?
    if (it != xAssetsSorted.end())
    {
        // do what ever with the found script
        auto foundAss = *it;
        auto foundAssNm = ( *it )->filename;
    }
    */
}

NOINLINE BOOL WINAPI GetMessageA_hook(LPMSG lpMsg, HWND hWnd, UINT wMsgFilterMin, UINT wMsgFilterMax)
{
    if (enterAlert)
    {
        if (GetCurrentThreadId() == mainThreadId)
        {
            // forcing the main thread into alertable state; so our function gets called ASAP.
            SleepEx(0, TRUE);
            enterAlert = false;
        }
    }

    return PLH::FnCast(GetMessageA_hook_Tramp, &GetMessageA)( lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax );
}

void DB_SwapXAsset_wrap(uint32_t* from, uint32_t* to)
{
    std::unordered_map<std::string, uint32_t> regArgs = {
        {"edi", (uint32_t)from},
        {"esi", (uint32_t)to}
    };
    std::unordered_map<std::string, uint32_t> retRegs = {};

    CustomCall(DB_SwapXAsset,
               regArgs,
               retRegs,
               0);
}


NOINLINE void* Scr_EndLoadScripts_hook()
{
    // script can be executed here

    return PLH::FnCast(Scr_EndLoadScripts_hook_Tramp, Scr_EndLoadScripts)( );
}


NOINLINE int __cdecl Scr_ExecThread_hook(int handle, int paramcount)
{
    // uint32_t caller {};
    // caller = ( *(uint32_t*)( caller + 4 ) ) - 5;
    //if (mainMapLoaded && myFuncHdl && caller == 0x050AF81 ) // is it being called from Scr_LoadLevel?

    return PLH::FnCast(Scr_ExecThread_hook_Tramp, Scr_ExecThread)( handle, paramcount );
}


NOINLINE int __cdecl Scr_ExecEntThread_hook(int ent, int handle, unsigned int paramcount)
{
    // Scr_GetFunctionHandle does not work here

    return PLH::FnCast(Scr_ExecEntThread_hook_Tramp, Scr_ExecEntThread)( ent, handle, paramcount );
}


NOINLINE int Scr_LoadLevel_hook()
{
    for (auto hdl : funcHandles)
    {
        auto thrHdl = Scr_ExecThread(hdl, 0);
        Scr_FreeThread(thrHdl);
    }

    return PLH::FnCast(Scr_LoadLevel_hook_Tramp, Scr_LoadLevel)( );
}


NOINLINE int __cdecl DB_TryLoadXFileInternal_hook(char* zoneName, int a2)
{
    logFastFile(zoneName);

    return PLH::FnCast(DB_TryLoadXFileInternal_hook_Tramp, DB_TryLoadXFileInternal)( zoneName, a2 );
}


NOINLINE int __cdecl Cmd_ExecuteSingleCommand_hook(int localClientNum, int controllerIndex, char* text)
{
    logExecSingleCmd(localClientNum, controllerIndex, text);

    // you can also try to execute an extra command here; not garanteed to work.

    // if you want to replace a command with another
    //if (!strcmp("seta cl_freelook \"1\"", text))
    //{
    //    return PLH::FnCast(Cmd_ExecuteSingleCommand_hook_Tramp, Cmd_ExecuteSingleCommand)( 0, 0, (char*)"seta cl_freelook \"0\"" );
    //}

    return PLH::FnCast(Cmd_ExecuteSingleCommand_hook_Tramp, Cmd_ExecuteSingleCommand)( localClientNum, controllerIndex, text );
}


NOINLINE int __cdecl Scr_GetFunctionHandle_hook(const char* filename, int hardCode)
{
    auto Hdl = PLH::FnCast(Scr_GetFunctionHandle_hook_Tramp, Scr_GetFunctionHandle)( filename, hardCode );

    logFuncHdl(filename, hardCode, Hdl);

    return Hdl;
}


NOINLINE char __cdecl DB_LoadXAssets_hook(XZoneInfo* p_zoneInfo, int zoneCount, int syncMode)
{
    // re-read the ini in case user changed it
    iniSetup();

    auto orijRet = PLH::FnCast(DB_LoadXAssets_hook_Tramp, DB_LoadXAssets)( p_zoneInfo, zoneCount, syncMode );

    bool zoneName = p_zoneInfo->name;


    // if it is not unloading
    // zoneCount can be 1 even on unloading but zoneName or first ff will be null
    // these conditions try to load custom .ff file on level load not on game load.
    // will crash if not injected once?
    if (!DB_LoadXAssetsLoadOnce && zoneName &&
        strcmp(p_zoneInfo->name, "ui") &&
        strcmp(p_zoneInfo->name, "code_pre_gfx") &&
        strcmp(p_zoneInfo->name, "code_post_gfx")
        )
    {
        std::filesystem::create_directories(iniOpt.gscBinDir);

        filesToGetHdl.clear();
        std::vector<ScriptFile> scriptFiles;

        std::error_code ec;
        std::filesystem::recursive_directory_iterator it(iniOpt.gscBinDir, std::filesystem::directory_options::skip_permission_denied, ec);
        std::filesystem::recursive_directory_iterator end;

        while (it != end)
        {
            const auto& entry = *it;
            std::ifstream file;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);

            if (entry.is_regular_file() && entry.path().extension() == ".gscbin")
            {
                try
                {
                    file.open(entry.path());
                    if (file.is_open() && file.good())
                    {
                        scriptFiles.push_back(*gscBinRead(entry));
                        filesToGetHdl.push_back(entry.path().stem().string());
                    }
                    else
                    {
                        std::cerr << "Failed to open file: " << entry.path() << '\n';
                    }

                    it.increment(ec);
                    if (ec)
                    {
                        std::cerr << "Error advancing iterator: " << ec.message() << '\n';
                        it = std::filesystem::recursive_directory_iterator();  // Reset iterator
                        break;
                    }
                }
                catch (const std::ifstream::failure& e)
                {
                    std::cerr << "Exception opening/reading file: " << e.what() << std::endl;
                }
            }

            file.close();
        }
        
        createPseudoFastFile(scriptFiles, strcat((char*)iniOpt.pseuFFName.c_str(), ".ff"));

        // big files may result in "out of memory" error
        if (!iniOpt.ffFilesToLoad.empty())
        {
            size_t ffSize = iniOpt.ffFilesToLoad.size();
            XZoneInfo* zInfArr = new XZoneInfo[ffSize];

            for (size_t idx = 0; idx < ffSize ; idx++)
            {
                zInfArr[idx].name = iniOpt.ffFilesToLoad.at(idx).c_str();
                zInfArr[idx].allocFlags = 4;
                zInfArr[idx].freeFlags = 0;
            }

            PLH::FnCast(DB_LoadXAssets_hook_Tramp, DB_LoadXAssets)( zInfArr, ffSize, 1 ); // 0 wont work
            DB_LoadXAssetsLoadOnce = true;
        }
    }

    // reset the bool that Scr_LoadLevel and Scr_ExecThread use, on unload
    if (!zoneName)
    {
        DB_LoadXAssetsLoadOnce = false;
    }

    return orijRet;
}


NOINLINE unsigned int __cdecl Scr_LoadScriptInternal_hook(char* filename, uint32_t entries, int entriesCount)
{
    uint32_t EbpBk;
    __asm
    {
        mov [EbpBk], ebp
    }

    uint32_t caller = (* (uint32_t*)(EbpBk + 4)) - 5;


    if (entriesCount)
    {
        uint32_t filePosId = *(uint32_t*)( EbpBk + 56 );
        uint32_t fileCountId = *(uint32_t*)( EbpBk + 60 );
        uint32_t scriptId = *(uint32_t*)( EbpBk + 64 );
        uint32_t _entries = *(uint32_t*)( EbpBk + 68 );
        uint32_t* entriesCount = (uint32_t*)( EbpBk + 72 );

        uint32_t compileCanonName = *(uint32_t*)( EbpBk + 76 );
        char* compileName = SL_ConvertToString(compileCanonName);

        if (!strcmp(compileName, "maps/so_survival_mp_paris"))
        {
            // every break here means "filename" is a sub-file of "maps/so_survival_mp_paris"
        }

        // list sub-scripts
        std::vector<char*> entryNames;
        for ( uint32_t i=0; i< (*entriesCount); i++)
        {
            uint16_t canonName = *(uint32_t*)( _entries + i * 2 );
            // may need clean up later
            char* compileNameStr = _strdup(SL_ConvertToString(canonName));
            entryNames.push_back(compileNameStr);
        }

        // script tree
        depsTree[_strdup(compileName)] = entryNames;

        if (!strcmp(compileName, mainMapFullName))
        {
            mainMapDeps = entryNames;
        }
    }
    else 
    {
        // script tree add single
        if (true)
        {
            // this causes crash on 4th loop
            // depsTree[_strdup(filename)] = {};

            std::vector<char*> empty {};
            depsTree[_strdup(filename)] = empty;
        }
    }

    auto hdl = PLH::FnCast(Scr_LoadScriptInternal_hook_Tramp, Scr_LoadScriptInternal)( filename, entries, entriesCount );

    // handle of specific scripts can be filtered here
    // handle is actually the offset at which script's bytecode is; scrVarPub.programBuffer + handle
    if (!strcmp(filename, "maps/_so_survival_chaos"))
    {

    }

    logScriptInternal(filename);

    return hdl;
}


NOINLINE unsigned int __cdecl Scr_LoadScript_hook(char* filename)
{
    uint32_t EbpBk;
    __asm
    {
        mov[EbpBk], ebp
    }

    uint32_t caller = *(uint32_t*)( EbpBk + 4 );    

    logScript(filename);

    return PLH::FnCast(Scr_LoadScript_hook_Tramp, Scr_LoadScript)( filename );
}


NOINLINE int __cdecl DB_LinkXAssetEntry1_hook(int type, uint32_t header)
{
    uint32_t ebpBkp;
    __asm 
    {
        mov ebpBkp,ebp
    }

    // the fastfile that contains this xasset;
    // different assets have different frame so this won't always point to a string
    char* fastFileName = (char*)*(uint32_t*)( ebpBkp + 0x34 );

    uint32_t xAssetEntry = 0;
    xAssetEntry = header; // save orij asset addr

    uint32_t xAsset = *( (uint32_t*)xAssetEntry );

    char* xAssetName = (char*)( *(uint32_t*)xAsset );


    // is it empty file or problematic file?
    if (!( *(uint32_t*)xAsset ) 
        || !strcmp((char*)( *(uint32_t*)xAsset ), "codescripts/$default") 
        || !strcmp((char*)( *(uint32_t*)xAsset ), "$default"))
    {
        return PLH::FnCast(DB_LinkXAssetEntry1_hook_Tramp, DB_LinkXAssetEntry1)( type, xAssetEntry );
    }

    // not having this check crashes the game
    if (*(uint32_t*)xAsset)
    {
        // xAsset names etc can be checked here
        
    }


    switch (type)
    {
        case XAssetType::ASSET_TYPE_RAWFILE:
            RawFile  rawFile;
            rawFile.filename = (char*)( *(uint32_t*)xAsset );
            rawFile.compressedLen = *(int*)( xAsset + 4 );
            rawFile.len = *(int*)( xAsset + 8 );
            rawFile.buffer = (char*)*(uint32_t*)( xAsset + 0xc );

            DumpRaw(rawFile);
            logAsset(type, header, fastFileName);
            break;

        case XAssetType::ASSET_TYPE_SCRIPTFILE:
            ScriptFile  scriptFile;
            scriptFile.filename = (char*)( *(uint32_t*)xAsset );
            scriptFile.compressedLen = *(int*)( xAsset + 4 );
            scriptFile.len = *(int*)( xAsset + 8 );
            scriptFile.bytecodeLen = *(int*)( xAsset + 0xc );
            scriptFile.buffer = (char*)*(uint32_t*)( xAsset + 0x10 );
            scriptFile.bytecode = (char*)*(uint32_t*)( xAsset + 0x14 );

            DumpScript(scriptFile);
            logAsset(type, header, fastFileName);
            break;

        default:
            logAsset(type, header, (char*)" "); // ff file name not guaranteed
            break;
    }

    return PLH::FnCast(DB_LinkXAssetEntry1_hook_Tramp, DB_LinkXAssetEntry1)( type, xAssetEntry );
}


NOINLINE int __cdecl GScr_SetScripts_hook(ScriptFunctions* Functions)
{
     // the game has finished loading scripts and is setting them


    return PLH::FnCast(GScr_SetScripts_hook_Tramp, GScr_SetScripts)( Functions );
}


NOINLINE int GScr_LoadScripts_hook(const char* mapname, ScriptFunctions* functions)
{
    funcLinked = false;

    mainMapName = Dvar_GetString("mapname");
    sprintf(mainMapFullName, "maps/%s", mainMapName);
    
    mainMapScrAss = (uint32_t)findScriptAssetEntry(mainMapFullName);
    if (!mainMapScrAss)
    {
        std::cout << "Couldn't main map script asset." << std::endl;
    }

    // from this point ahead, is an atempt to insert an extra dependency to load
    // our custom script without calling execThread; which works on some maps and crashes on others so currently is commented out.
    auto p_mapScriptXAss = (XAssetScriptEntry*)mainMapScrAss;
    char* buffer = (char*)p_mapScriptXAss->scriptFile->bytecode;
    size_t size = p_mapScriptXAss->scriptFile->bytecodeLen; // +p_mapScriptXAss->scriptFile->compressedLen;
    mapScriptUncompOrijSz = p_mapScriptXAss->scriptFile->len;

    // Sequence to search for
    size_t capacity = 999;
    unsigned char searchSequence[] = {0x93, 0x28}; 

    // commented out
    // bytecodeFindAndInsert(buffer, size, capacity, searchSequence, 2);

    p_mapScriptXAss->scriptFile->bytecodeLen = size;


    bool numName = false;
    bool strName = true;
    const char* scriptName = customScript;

    if (numName)
    {
        size_t size = 4; // 2byte canonName + byte funcName
        memmove((void*)(p_mapScriptXAss->scriptFile->bytecode+ size ), p_mapScriptXAss->scriptFile->bytecode, p_mapScriptXAss->scriptFile->bytecodeLen);
        memset((void*)p_mapScriptXAss->scriptFile->bytecode, 0, size);
        p_mapScriptXAss->scriptFile->bytecode += size;
    }
    else if (strName)
    {
        // size_t size = 2 + strlen(scriptName) + 1 + 2; // +2 for 00 00 byteCodes // +1 for null terminator
        // memmove((void*)( p_mapScriptXAss->scriptFile->bytecode + size ), p_mapScriptXAss->scriptFile->bytecode, p_mapScriptXAss->scriptFile->bytecodeLen);
        // memset((void*)p_mapScriptXAss->scriptFile->bytecode, 0, size);
        // p_mapScriptXAss->scriptFile->bytecode += size;
    }
    else
    {
        std::cerr << "No dependency insert method chosen." << std::endl;
    }

    return PLH::FnCast(GScr_LoadScripts_hook_Tramp, GScr_LoadScripts)( mapname, functions );
}


NOINLINE int __cdecl EmitFunction_hook(unsigned int canonName, unsigned int funcID, BYTE* byteCodeBuff)
{
    uint32_t ebpBkp;
    __asm
    {
        mov [ebpBkp], ebp
    }

    char* motherscript = (char*)*(uint32_t*)( ebpBkp + 0x17c );
    char* Script = (char*)*(uint32_t*)( ebpBkp + 0x40 );
    char* currScript = SL_ConvertToString(canonName);

    if (!funcLinked && !strcmp(mainMapFullName, Script))
    {
        // here we try to load our script as dependency (93 28 xx xx xx)
        // check if expected bytCode was executed 
        if (*( byteCodeBuff - 1 ) == 0x28 && *( byteCodeBuff - 2 ) == 0x93)
        {
            // modifyBuffer(customScript);
        }
        
    }


    // if inject is done and another script is being handled
    if (funcLinked && strcmp(mainMapFullName, Script))
    {
       
    }

    return PLH::FnCast(EmitFunction_hook_Tramp, EmitFunction)( canonName, funcID, byteCodeBuff );
}


NOINLINE int __cdecl GScr_PostLoadScripts_hook()
{
    funcHandles.clear();

    for (auto file : filesToGetHdl)
    {
        //auto hdl = Scr_GetFunctionHandle(customScript, ScriptFuncName::main);
        Scr_LoadScript(file.c_str());
        auto hdl = Scr_GetFunctionHandle(file.c_str(), ScriptFuncName::main);
        funcHandles.push_back(hdl);
    }

    return PLH::FnCast(GScr_PostLoadScripts_hook_Tramp, GScr_PostLoadScripts)( );
}


