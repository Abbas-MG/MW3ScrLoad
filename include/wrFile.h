#pragma once

struct IniOpt
{
    std::string pseuFFName;
    std::vector<std::string> ffFilesToLoad;
    std::filesystem::path gscBinDir;
    std::filesystem::path dumpRawPath;
    std::filesystem::path dumpScriptPath;
    std::filesystem::path LogRawPath;
    std::filesystem::path LogScriptPath;
    std::filesystem::path LogAssetPath;
    std::filesystem::path MiscLogPath;

    bool scrLoadAsDep;
    bool do_dumpRaw;
    bool do_dumpScript;
    bool do_LogScript;
    bool do_LogAsset;
    bool do_LogRaw;
    bool do_MiscLog;
};

template <typename T>
std::vector<T> operator+(std::vector<T> const& x, std::vector<T> const& y)
{
    std::vector<T> vec;
    vec.reserve(x.size() + y.size());
    vec.insert(vec.end(), x.begin(), x.end());
    vec.insert(vec.end(), y.begin(), y.end());
    return vec;
}

template <typename T>
std::vector<T>& operator+=(std::vector<T>& x, const std::vector<T>& y)
{
    x.reserve(x.size() + y.size());
    x.insert(x.end(), y.begin(), y.end());
    return x;
}

std::vector<uint8_t> scriptFileToStream(ScriptFile* scriptFile);

bool createPseudoFastFile(std::vector<ScriptFile> scriptFiles, const char* name);

ScriptFile* gscBinRead(std::filesystem::path gscBin);

bool DumpScript(ScriptFile& scriptFile);

bool DumpRaw(RawFile& rawFile);

bool logExecSingleCmd(int localClientNum, int controllerIndex, char* text);

bool logFuncHdl(const char* filename, int hardCode, int handle);

bool logScript(char* filename);

bool logScriptInternal(char* filename);

bool logRaw(char* filename);

bool logAsset(int type, uint32_t xAsset, char* ffName);

bool logFastFile(char* zoneName);

bool isIniTrue(std::string& value);

bool iniSetup();
