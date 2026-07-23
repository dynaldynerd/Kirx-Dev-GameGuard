#pragma once
// ============================================================================
// ProtectConfig.h - INI Configuration Manager for Protection System
// Recreated from 6.23_Guard 2 behavior for RF-Online WorldServer
// ============================================================================

#include <string>
#include <map>
#include <vector>
#include <mutex>
#include <cstdio>
#include <cstring>

class ProtectConfig
{
public:
    static ProtectConfig& Instance()
    {
        static ProtectConfig inst;
        return inst;
    }

    // Load all config files from a directory
    bool LoadAll(const char* configDir);

    // Load a single .ini file
    bool LoadFile(const char* filePath);

    // Reload all configs (hot-reload support)
    bool ReloadAll();

    // Getters
    std::string GetString(const char* file, const char* section, const char* key, const char* defaultVal = "");
    int         GetInt(const char* file, const char* section, const char* key, int defaultVal = 0);
    float       GetFloat(const char* file, const char* section, const char* key, float defaultVal = 0.0f);
    bool        GetBool(const char* file, const char* section, const char* key, bool defaultVal = false);

    // Parse comma-separated int list (e.g., "10,11,12,13")
    std::vector<int> GetIntList(const char* file, const char* section, const char* key);

    // Parse colon-separated float list (e.g., "1000:750:500:250")
    std::vector<float> GetFloatListColon(const char* file, const char* section, const char* key);

    const char* GetConfigDir() const { return m_configDir; }

private:
    ProtectConfig() { m_configDir[0] = 0; }
    ~ProtectConfig() {}
    ProtectConfig(const ProtectConfig&) = delete;
    ProtectConfig& operator=(const ProtectConfig&) = delete;

    // file -> section -> key -> value
    struct IniData
    {
        std::map<std::string, std::map<std::string, std::string>> sections;
    };

    bool ParseIniFile(const char* filePath, IniData& data);
    std::string ExtractFilename(const char* filePath);
    static std::string Trim(const std::string& s);
    static std::string ToLower(const std::string& s);

    std::map<std::string, IniData> m_files;  // filename -> data
    std::mutex m_mutex;
    char m_configDir[260];
};
