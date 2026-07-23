#include "ProtectConfig.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <filesystem>

// ============================================================================
// ProtectConfig.cpp - INI Configuration Manager Implementation
// ============================================================================

bool ProtectConfig::LoadAll(const char* configDir)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    strncpy(m_configDir, configDir, sizeof(m_configDir) - 1);
    m_configDir[sizeof(m_configDir) - 1] = 0;

    m_files.clear();

    namespace fs = std::filesystem;
    for (auto& entry : fs::directory_iterator(configDir))
    {
        if (entry.is_regular_file())
        {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".ini")
            {
                IniData data;
                if (ParseIniFile(entry.path().string().c_str(), data))
                {
                    std::string fname = entry.path().filename().string();
                    m_files[ToLower(fname)] = std::move(data);
                }
            }
        }
    }
    return !m_files.empty();
}

bool ProtectConfig::LoadFile(const char* filePath)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    IniData data;
    if (!ParseIniFile(filePath, data))
        return false;

    std::string fname = ExtractFilename(filePath);
    m_files[ToLower(fname)] = std::move(data);
    return true;
}

bool ProtectConfig::ReloadAll()
{
    if (m_configDir[0] == 0)
        return false;
    return LoadAll(m_configDir);
}

std::string ProtectConfig::GetString(const char* file, const char* section, const char* key, const char* defaultVal)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    auto fileIt = m_files.find(ToLower(file));
    if (fileIt == m_files.end())
        return defaultVal;

    auto secIt = fileIt->second.sections.find(ToLower(section));
    if (secIt == fileIt->second.sections.end())
        return defaultVal;

    auto keyIt = secIt->second.find(ToLower(key));
    if (keyIt == secIt->second.end())
        return defaultVal;

    return keyIt->second;
}

int ProtectConfig::GetInt(const char* file, const char* section, const char* key, int defaultVal)
{
    std::string val = GetString(file, section, key, "");
    if (val.empty()) return defaultVal;
    try { return std::stoi(val); }
    catch (...) { return defaultVal; }
}

float ProtectConfig::GetFloat(const char* file, const char* section, const char* key, float defaultVal)
{
    std::string val = GetString(file, section, key, "");
    if (val.empty()) return defaultVal;
    try { return std::stof(val); }
    catch (...) { return defaultVal; }
}

bool ProtectConfig::GetBool(const char* file, const char* section, const char* key, bool defaultVal)
{
    std::string val = GetString(file, section, key, "");
    if (val.empty()) return defaultVal;
    std::string lower = ToLower(val);
    return (lower == "true" || lower == "1" || lower == "yes");
}

std::vector<int> ProtectConfig::GetIntList(const char* file, const char* section, const char* key)
{
    std::vector<int> result;
    std::string val = GetString(file, section, key, "");
    if (val.empty()) return result;

    std::stringstream ss(val);
    std::string token;
    while (std::getline(ss, token, ','))
    {
        token = Trim(token);
        if (!token.empty())
        {
            try { result.push_back(std::stoi(token)); }
            catch (...) {}
        }
    }
    return result;
}

std::vector<float> ProtectConfig::GetFloatListColon(const char* file, const char* section, const char* key)
{
    std::vector<float> result;
    std::string val = GetString(file, section, key, "");
    if (val.empty()) return result;

    std::stringstream ss(val);
    std::string token;
    while (std::getline(ss, token, ':'))
    {
        token = Trim(token);
        if (!token.empty())
        {
            try { result.push_back(std::stof(token)); }
            catch (...) {}
        }
    }
    return result;
}

// ---- Private helpers ----

bool ProtectConfig::ParseIniFile(const char* filePath, IniData& data)
{
    std::ifstream fin(filePath);
    if (!fin.is_open())
        return false;

    std::string currentSection;
    std::string line;
    while (std::getline(fin, line))
    {
        line = Trim(line);
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        if (line[0] == '[')
        {
            size_t end = line.find(']');
            if (end != std::string::npos)
            {
                currentSection = ToLower(Trim(line.substr(1, end - 1)));
            }
            continue;
        }

        size_t eq = line.find('=');
        if (eq != std::string::npos && !currentSection.empty())
        {
            std::string key = ToLower(Trim(line.substr(0, eq)));
            std::string val = Trim(line.substr(eq + 1));
            // Remove inline comments
            size_t commentPos = val.find(';');
            if (commentPos != std::string::npos)
                val = Trim(val.substr(0, commentPos));
            commentPos = val.find('#');
            if (commentPos != std::string::npos)
                val = Trim(val.substr(0, commentPos));

            data.sections[currentSection][key] = val;
        }
    }
    return true;
}

std::string ProtectConfig::ExtractFilename(const char* filePath)
{
    std::string path(filePath);
    size_t pos = path.find_last_of("/\\");
    if (pos != std::string::npos)
        return path.substr(pos + 1);
    return path;
}

std::string ProtectConfig::Trim(const std::string& s)
{
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::string ProtectConfig::ToLower(const std::string& s)
{
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}
