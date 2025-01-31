#pragma once

struct ScriptFile
{
    const char* filename;
    int compressedLen;
    int len;
    int bytecodeLen;
    const char* buffer;
    const char* bytecode;
};

struct RawFile
{
    const char* filename;
    int compressedLen;
    int len;
    const char* buffer;
};
 

struct XZoneInfo
{
    const char* name;
    int allocFlags;
    int freeFlags;
};

struct ScriptFunctions
{
    int maxSize; // = 512;
    int count;
    uint32_t* address;
};

struct XAssetHeader
{
    // there are much more
    RawFile* rawfile;
};

struct XAssetScriptEntry
{
    int type;            //XAssetType type
    ScriptFile* scriptFile;    //XAssetHeader header;
};

struct XAssetRawEntry
{
    int type;            
    RawFile* rawfile;    
};

struct XAssetEntry
{
    int type;            
    uint32_t header;    // need casting
};

enum XAssetType
{   // those i could come up with
    ASSET_TYPE_PIXELSHADER = 0x6,
    ASSET_TYPE_SOUND = 0xB, // .wav
    ASSET_TYPE_SOUND_2 = 0xD, // .wav
    ASSET_TYPE_MENU = 0x19,
    ASSET_TYPE_STRING = 0x1B, // script strings?
    ASSET_TYPE_FX = 0x1F,
    ASSET_TYPE_RAWFILE = 0x26,
    ASSET_TYPE_SCRIPTFILE = 0x27,
    ASSET_TYPE_STRINGTABLE = 0x28,  // .csv
    ASSET_TYPE_ATTACHMENT = 0x1C,
}; 

enum ScriptFuncName
{
    main = 1650,
    init = 1664,
    end_script = 1663,
    createstruct = 1662,
    initstructs = 1661,
    precache = 1665,
    spawner = 1666,
    origin = 1669,

};

enum GSCColor
{
    WHITE = 0x15,
    RED = 0x16,
    GREEN = 0x17,
    BLUE = 0x18,
    YELLOW = 0x19,
    CYAN = 0x1A,
    MAGENTA = 0x1B,
    ORANGE = 0x1C,
    BLACK = 0x1D,
    GRAY = 0x1E
};