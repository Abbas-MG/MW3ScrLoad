#include "pch.h"


extern IniOpt iniOpt {};

const std::string& ScriptFuncNametoString(int value)
{
    static const std::map<int, std::string> scriptFuncMap = {
        {1650, "main"},
        {1664, "init"},
        {1663, "end_script"},
        {1662, "createstruct"},
        {1661, "initstructs"},
        {1665, "precache"},
        {1666, "spawner"},
        {1669, "origin"},
    };

    auto it = scriptFuncMap.find(value);
    if (it != scriptFuncMap.end())
    {
        return it->second;
    }

    static const std::string unknown = "UNKNOWN";
    return unknown;
}

std::streampos getFileSize(std::ofstream& file)
{
// Save current position
    std::streampos currentPos = file.tellp();

    // Seek to end to get file size
    file.seekp(0, std::ios::end);
    std::streampos fileSize = file.tellp();

    // Restore the write position
    file.seekp(currentPos);

    return fileSize;
}

// compress a script file
std::vector<uint8_t> scriptFileToStream(ScriptFile* scriptFile)
{
    std::vector<uint8_t> totalByteArr {};
    std::vector<uint8_t> FFbyteArr = {0xFF, 0xFF, 0xFF, 0xFF};


    totalByteArr += toLittleEndianArray(scriptFile->compressedLen) + toLittleEndianArray(scriptFile->len) + toLittleEndianArray(scriptFile->bytecodeLen);
    totalByteArr += FFbyteArr + FFbyteArr;

    // null terminator
    totalByteArr += charArrayToHexVector(scriptFile->filename, strlen(scriptFile->filename) + 1);

    totalByteArr += charArrayToHexVector(scriptFile->buffer, scriptFile->compressedLen);
    totalByteArr += charArrayToHexVector(scriptFile->bytecode, scriptFile->bytecodeLen);

    totalByteArr += FFbyteArr;

    size_t totalByteArrCprSz {};
    char* totalByteArrCprChar = compressCharData((const char*)totalByteArr.data(), totalByteArr.size(), totalByteArrCprSz);
    std::vector<uint8_t> finalByteArr = charArrayToHexVector(totalByteArrCprChar, totalByteArrCprSz);

    return finalByteArr;
}

// will create .ff file in "zone/english" so the game will be able see it
bool createPseudoFastFile(std::vector<ScriptFile> scriptFiles, const char* name)
{
    bool res = false;
    uint32_t scriptCount = scriptFiles.size();
    std::vector<uint8_t> totalByteArr {};

    std::vector<uint8_t> dummyBeforeCompress = {0xFB, 0xBD, 0x0A, 0x00,
                                                0x00, 0xC0, 0x00, 0x00,
                                                0x58, 0x01, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x36, 0xD7, 0x09, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x00, 0x00, 0x00, 0x00,
                                                0x32, 0xC7, 0x00, 0x00};


    totalByteArr += dummyBeforeCompress;


    // The target byte array with space for the insertion
    std::vector<uint8_t> zeroArr = {0x00, 0x00, 0x00, 0x00,};

    std::vector<uint8_t> FFbyteArr = {0xFF, 0xFF, 0xFF, 0xFF};

    //// Get the little-endian representation of the scriptCount
    std::vector<uint8_t> littleEndian_scriptCount = toLittleEndianArray(scriptCount);

    totalByteArr += zeroArr + zeroArr + littleEndian_scriptCount + FFbyteArr;
    //// 00 00 00 00 00 00 00 00 25 00 00 00 FF FF FF FF

    uint32_t scriptID = 39; // script file
    std::vector<uint8_t> littleEndian_scriptID = toLittleEndianArray(scriptID);

    for (uint32_t i = 1; i <= scriptCount; i++)
    {
        totalByteArr += littleEndian_scriptID + FFbyteArr;
            // [ID] 00 00 00 + FF FF FF FF
    }

    totalByteArr += FFbyteArr;
    // table end: FF FF FF FF

    for (auto it = scriptFiles.begin(); it != scriptFiles.end(); it++)
    {
        totalByteArr += toLittleEndianArray(it->compressedLen) + toLittleEndianArray(it->len) + toLittleEndianArray(it->bytecodeLen);
        totalByteArr += FFbyteArr + FFbyteArr;

        // null terminator
        totalByteArr += charArrayToHexVector(it->filename, strlen(it->filename) + 1);

        totalByteArr += charArrayToHexVector(it->buffer, it->compressedLen);
        totalByteArr += charArrayToHexVector(it->bytecode, it->bytecodeLen);

        totalByteArr += FFbyteArr;

    }

    // for debug; not compressed.
    std::string _raw = iniOpt.pseuFFName + "_raw.ff";
    std::filesystem::path rawffPath = std::filesystem::current_path() / "zone/english" / _raw;
    std::ofstream rawffFile(rawffPath.string(), std::ios::binary);
    if (rawffFile)
    {
        rawffFile.write(reinterpret_cast<const char*>( totalByteArr.data() ), totalByteArr.size());
        rawffFile.seekp(0, std::ios::beg);

        auto size = toLittleEndianArray(getFileSize(rawffFile));
        rawffFile.write(reinterpret_cast<const char*>( size.data() ), size.size());
        rawffFile.close();
        res = true;
    }


    // compress
    size_t totalByteArrCprSz {};
    char* totalByteArrCprChar = compressCharData((const char*)totalByteArr.data(), totalByteArr.size(), totalByteArrCprSz);

    std::vector<uint8_t> finalByteArr = charArrayToHexVector(totalByteArrCprChar, totalByteArrCprSz);


    // pseudo header; IWffu100 ...
    std::vector<uint8_t> stub = {0x49, 0x57, 0x66, 0x66,
                                 0x75, 0x31, 0x30, 0x30,
                                 0x01, 0x00, 0x00, 0x00,
                                 0x01, 0x3C, 0xCF, 0xCD,
                                 0x01, 0xB0, 0x97, 0xD6, 0x5B};

    std::vector<uint8_t> ffFileByteArr = stub + finalByteArr;


    std::filesystem::path ffPath = std::filesystem::current_path() / "zone/english" / name;
    std::ofstream ffFile(ffPath.string(), std::ios::binary);
    if (ffFile)
    {
        ffFile.write(reinterpret_cast<const char*>( ffFileByteArr.data() ), ffFileByteArr.size());
        ffFile.close();
        res = true;
    }
    return res;
}

ScriptFile* gscBinRead(std::filesystem::path gscBin)
{
    std::fstream gsc(gscBin, std::ios::in | std::ios::binary);
    if (gsc)
    {
        ScriptFile* p_scriptFile = new ScriptFile {};

        char magic[4];
        gsc.read(magic, 4);
        gsc.read((char*)&p_scriptFile->compressedLen, 4);
        gsc.read((char*)&p_scriptFile->len, 4);
        gsc.read((char*)&p_scriptFile->bytecodeLen, 4);


        // remove root
        auto path = gscBin.string();
        const std::string prefix = iniOpt.gscBinDir.string() + "\\";
        auto pos = path.find(prefix);
        if (pos != std::string::npos)
        {
            path.erase(pos, prefix.length());
        }

        // remove extention
        const std::string ext = R"(.gscbin)";
        pos = path.find(ext);
        if (pos != std::string::npos)
        {
            path.erase(pos, ext.length());
        }

        for (auto& ch : path)
        {
            if (ch == '\\')
            {
                ch = '/';
            }
        }

        p_scriptFile->filename = new char[path.size() + 1];
        std::strcpy((char*)p_scriptFile->filename, path.c_str());

        size_t totalLength = p_scriptFile->compressedLen + p_scriptFile->bytecodeLen;

        p_scriptFile->buffer = new char[totalLength]; //combinedBuffer;
        gsc.read((char*)p_scriptFile->buffer, totalLength);
        p_scriptFile->bytecode = p_scriptFile->buffer + p_scriptFile->compressedLen;

        return p_scriptFile;
    }
    else
    {
        std::cout << "Couldn't open the gscbin file." << std::endl;
        return nullptr;
    }
}

bool DumpScript(ScriptFile& scriptFile)
{
    bool res = false;
    if (iniOpt.do_dumpScript)
    {
        std::string binFileName = std::string(scriptFile.filename) + ".gscbin";

        std::filesystem::path filePath = iniOpt.dumpScriptPath / "ScriptFiles" / binFileName;
        const char* magic = "GSC";

        if (!iniOpt.do_dumpScript)
        {
            return res;
        }

        std::filesystem::create_directories(filePath.parent_path());
        std::fstream dump(filePath, std::ios::out | std::ios::binary);
        if (dump.is_open())
        {
            dump.write(magic, strlen(magic) + 1);
            dump.write(reinterpret_cast<const char*>( &scriptFile.compressedLen ), sizeof(scriptFile.compressedLen));
            dump.write(reinterpret_cast<const char*>( &scriptFile.len ), sizeof(scriptFile.len));
            dump.write(reinterpret_cast<const char*>( &scriptFile.bytecodeLen ), sizeof(scriptFile.bytecodeLen));
            dump.write(scriptFile.buffer, scriptFile.compressedLen);
            dump.write(scriptFile.bytecode, scriptFile.bytecodeLen);
            dump.close();

            res = true;
        }
    }
    return res;
}

bool DumpRaw(RawFile& rawFile)
{

    bool res = false;
    if (iniOpt.do_dumpRaw)
    {
        std::filesystem::path filePath = iniOpt.dumpRawPath / "RawFiles" / rawFile.filename;

        if (!iniOpt.do_dumpRaw)
        {
            return res;
        }

        char* uncopm = uncompressRawFile(rawFile);
        if (*uncopm)
        {
            std::filesystem::create_directories(filePath.parent_path());
            std::fstream dump(filePath, std::ios::out);
            if (dump.is_open())
            {
                dump.write(uncopm, rawFile.len);
                dump.close();

                res = true;
            }
        }
        delete[] uncopm;
    }
    
    return res;
}

bool logExecSingleCmd(int localClientNum, int controllerIndex, char* text)
{
    bool res = false;
    if (iniOpt.do_MiscLog)
    {
        std::filesystem::path logFilename = "Exec_singleCommands.txt";
        std::filesystem::path fPath = iniOpt.MiscLogPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());

        std::fstream log(fPath, std::ios::app);

        if (log.is_open())
        {
            std::stringstream ss;
            ss << "client: " << localClientNum << "  index: " << controllerIndex << "   cmd: " << text;
            // log.write(text, strlen(text));
            log << ss.str();
            log << '\n';
            log.close();
            res = true;
        }
    }
    
    return res;
}

bool logFuncHdl(const char* filename, int hardCode, int handle)
{
    bool res = false;
    if (iniOpt.do_MiscLog)
    {
        std::filesystem::path logFilename = "loadedFuncHandles.txt";
        std::filesystem::path fPath = iniOpt.MiscLogPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());
        std::stringstream hdlStr {};
        hdlStr <<  filename << "       Func: " << ScriptFuncNametoString(hardCode) << "     Handle: " << "0x" << std::hex << handle;

        std::fstream log(fPath, std::ios::app);
        if (log.is_open())
        {
            log.write(hdlStr.str().c_str(), hdlStr.str().size());
            log << '\n';
            log.close();
            res = true;
        }
    }

    return res;
}

bool logScriptInternal(char* filename)
{
    bool res = false;
    if (iniOpt.do_MiscLog)
    {
        std::filesystem::path logFilename = "loadedScriptsInternal.txt";
        std::filesystem::path fPath = iniOpt.MiscLogPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());

        std::fstream log(fPath, std::ios::app);
        if (log.is_open())
        {
            log.write(filename, strlen(filename));
            log << '\n';
            log.close();
            res = true;
        }
    }

    return res;
}

bool logScript(char* filename)
{
    bool res = false;
    if (iniOpt.do_LogScript)
    {
        std::filesystem::path logFilename = "loadedScripts.txt";
        std::filesystem::path fPath = iniOpt.LogScriptPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());

        std::fstream log(fPath, std::ios::app);
        if (log.is_open())
        {
            log.write(filename, strlen(filename));
            log << '\n';
            log.close();
            res = true;
        }
    }

    return res;
}

bool logRaw(char* filename)
{
    bool res = false;
    if (iniOpt.do_LogRaw)
    {
        std::filesystem::path logFilename = "loadedRawFiles.txt";
        std::filesystem::path fPath = iniOpt.LogRawPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());

        std::fstream log(fPath, std::ios::app);
        if (log.is_open())
        {
            log.write(filename, strlen(filename));
            log << '\n';
            log.close();
            res = true;
        }
    }

    return res;
}

bool logAsset(int type, uint32_t xAsset, char* ffName)
{
    bool res = false;
    if (iniOpt.do_LogAsset)
    {
        std::filesystem::path logFilename = "loadedAssetFiles.txt";
        std::filesystem::path fPath = iniOpt.LogAssetPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());

        std::fstream log(fPath, std::ios::app);
        if (log.is_open())
        {
            log.write((char*)( **(uint32_t**)xAsset ), strlen((char*)( **(uint32_t**)xAsset )));
            log << "      assetType: 0x" << std::hex << type << "     from .ff: " << ffName;
            log << '\n';
            log.close();
            res = true;
        }
    }

    return res;
}

bool logFastFile(char* zoneName)
{
    bool res = false;
    if (iniOpt.do_MiscLog)
    {
        std::filesystem::path logFilename = "loadedFastFiles.txt";
        std::filesystem::path fPath = iniOpt.MiscLogPath / logFilename;
        std::filesystem::create_directories(fPath.parent_path());

        std::fstream log(fPath, std::ios::app);
        if (log.is_open())
        {
            log.write(zoneName, strlen(zoneName));
            log << '\n';
            log.close();
            res = true;
        }
    }

    return res;
}

bool isIniTrue(std::string& value)
{
    if (value == "1" || value == "True" || value == "true")
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool iniSetup()
{
    bool res = false;
    mINI::INIFile      file("config.ini");
    mINI::INIStructure ini;

    if (file.read(ini))
    {
        //iniOpt.scrLoadAsDep = isIniTrue(ini["script"]["scrLoadAsDep"]);
        iniOpt.pseuFFName = ini["script"]["pseudoFFName"];
        std::string _ffFilesToLoad = ini["script"]["ffFilesLoad"];
        iniOpt.gscBinDir = ini["script"]["gscBinDir"];

        iniOpt.do_dumpRaw = isIniTrue(ini["Dump"]["RawFiles"]);
        iniOpt.dumpRawPath = ini["Dump"]["Dir"];

        iniOpt.do_dumpScript = isIniTrue(ini["Dump"]["ScriptFiles"]);
        iniOpt.dumpScriptPath = ini["Dump"]["Dir"];

        iniOpt.do_LogAsset = isIniTrue(ini["Log"]["AssetFiles"]);
        iniOpt.LogRawPath = ini["Log"]["Dir"];

        iniOpt.do_LogScript = isIniTrue(ini["Log"]["ScriptFiles"]);
        iniOpt.LogScriptPath = ini["Log"]["Dir"];

        iniOpt.do_MiscLog = isIniTrue(ini["Log"]["MiscLog"]);
        iniOpt.MiscLogPath = ini["Log"]["Dir"];

        iniOpt.LogAssetPath = ini["Log"]["Dir"];


        _ffFilesToLoad.erase(std::remove(_ffFilesToLoad.begin(), _ffFilesToLoad.end(), ' '), _ffFilesToLoad.end());
        std::stringstream ssFFld(_ffFilesToLoad);
        std::string part;
        iniOpt.ffFilesToLoad.clear();
        while (std::getline(ssFFld, part, ','))
        {
            iniOpt.ffFilesToLoad.push_back(part);
        }

        res = true;
    }
    else
    {
        // Custom script
        ini["script"]["pseudoFFName"] = "test";
        ini["script"]["ffFilesLoad"] = "test";
        ini["script"]["gscBinDir"] = ".\\LoadGscBin";

        // Dump settings
        ini["Dump"]["RawFiles"] = "0";
        ini["Dump"]["ScriptFiles"] = "0";
        ini["Dump"]["Dir"] = ".\\DumpedFiles";

        // Log settings
        ini["Log"]["ScriptFiles"] = "0";
        ini["Log"]["AssetFiles"] = "0";
        ini["Log"]["MiscLog"] = "0";
        ini["Log"]["Dir"] = ".\\LoggedFiles";
        

        res = file.generate(ini, true); // with pretty-print
    }

    return res;
}