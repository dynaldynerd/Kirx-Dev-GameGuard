#include "InputSanitizer.h"
#include "../Entities/CLogFile.h"

#include <algorithm>
#include <cctype>
#include <cstring>
#include <regex>

// ============================================================================
// InputSanitizer.cpp - SQL Injection Prevention Implementation
// ============================================================================

void InputSanitizer::Initialize()
{
    static CLogFile s_log;
    s_log.SetWriteLogFile("Protection_InputSanitizer", 1, false, true, true);
    m_pLog = &s_log;
    m_pLog->Write("[InputSanitizer] Initialized");
}

// ---- SQL Dangerous Patterns ----

const std::vector<std::string>& InputSanitizer::GetSQLPatterns()
{
    static std::vector<std::string> patterns = {
        "select ", "insert ", "update ", "delete ", "drop ",
        "alter ", "create ", "exec ", "execute ", "union ",
        "declare ", "cast(", "convert(", "char(", "nchar(",
        "varchar(", "nvarchar(", "waitfor ", "delay ",
        "xp_", "sp_", "0x",
        "--", "/*", "*/", "@@", "@@version",
        "sysobjects", "syscolumns", "sys.", "information_schema",
        "having ", "group by", "order by",
        "shutdown", "truncate ", "bulk ", "openrowset",
        "openquery", "opendatasource"
    };
    return patterns;
}

// ---- Core Sanitization ----

std::string InputSanitizer::SanitizeForSQL(const char* input, int maxLen)
{
    if (!input) return "";

    std::string result;
    int len = static_cast<int>(strlen(input));
    if (len > maxLen) len = maxLen;

    result.reserve(len);

    for (int i = 0; i < len; i++)
    {
        char c = input[i];

        // Block null bytes
        if (c == 0) break;

        // Escape single quotes (primary SQL injection vector)
        if (c == '\'')
        {
            result += "''"; // SQL-safe escaping
            continue;
        }

        // Block dangerous characters entirely
        if (c == ';' || c == '\\' || c == '\x1a' /* EOF */)
        {
            if (m_pLog)
                m_pLog->Write("[SQLSanitize] Stripped dangerous char 0x%02X from input", (unsigned char)c);
            continue;
        }

        // Block control characters (except newline/tab for chat)
        if (c < 0x20 && c != '\n' && c != '\r' && c != '\t')
            continue;

        result += c;
    }

    // Check for SQL injection patterns in the sanitized string
    if (ContainsSQLInjection(result.c_str()))
    {
        if (m_pLog)
            m_pLog->Write("[SQLSanitize] SQL injection pattern detected in: '%.64s'", result.c_str());
        // Return empty - caller should reject this input
        return "";
    }

    return result;
}

bool InputSanitizer::ValidateName(const char* name, int nameType)
{
    if (!name || name[0] == 0) return false;

    size_t len = strlen(name);

    // Length check
    if (len < 2 || len > 16)
    {
        if (m_pLog)
            m_pLog->Write("[ValidateName] Invalid length: %zu", len);
        return false;
    }

    // Buffer overflow check
    if (IsBufferOverflow(name, 16))
        return false;

    // Character validation
    for (size_t i = 0; i < len; i++)
    {
        unsigned char c = static_cast<unsigned char>(name[i]);

        // Allow Korean/CJK characters (multi-byte)
        if (c >= 0x80) continue;

        if (nameType == 0)
        {
            // Type 0: alphanumeric only
            if (!std::isalnum(c))
            {
                if (m_pLog)
                    m_pLog->Write("[ValidateName] Invalid char '%c' (type 0) in: '%s'", name[i], name);
                return false;
            }
        }
        else if (nameType == 1)
        {
            // Type 1: alphanumeric + [ ] - ? ! +
            if (!std::isalnum(c) &&
                c != '[' && c != ']' && c != '-' && c != '?' && c != '!' && c != '+')
            {
                if (m_pLog)
                    m_pLog->Write("[ValidateName] Invalid char '%c' (type 1) in: '%s'", name[i], name);
                return false;
            }
        }

        // Always block SQL injection chars in names
        if (c == '\'' || c == '"' || c == ';' || c == '\\' || c == '%' ||
            c == '=' || c == '<' || c == '>' || c == '(' || c == ')' ||
            c == '{' || c == '}' || c == '|' || c == '`' || c == '~')
        {
            if (m_pLog)
                m_pLog->Write("[ValidateName] SQL danger char '%c' in: '%s'", name[i], name);
            return false;
        }
    }

    // Check for SQL injection patterns
    if (ContainsSQLInjection(name))
    {
        if (m_pLog)
            m_pLog->Write("[ValidateName] SQL injection pattern in name: '%s'", name);
        return false;
    }

    return true;
}

bool InputSanitizer::ValidateGuildName(const char* name)
{
    if (!name || name[0] == 0) return false;

    size_t len = strlen(name);
    if (len < 2 || len > 20) return false;

    // Guild names are more permissive but still block SQL
    for (size_t i = 0; i < len; i++)
    {
        unsigned char c = static_cast<unsigned char>(name[i]);
        if (c >= 0x80) continue; // allow multi-byte

        if (c == '\'' || c == '"' || c == ';' || c == '\\' ||
            c == '<' || c == '>' || c == '(' || c == ')' ||
            c == '{' || c == '}' || c == '`')
        {
            if (m_pLog)
                m_pLog->Write("[ValidateGuild] Dangerous char in guild name: '%s'", name);
            return false;
        }
    }

    return !ContainsSQLInjection(name);
}

std::string InputSanitizer::SanitizeChat(const char* input, int maxLen)
{
    if (!input) return "";

    std::string result;
    int len = static_cast<int>(strlen(input));
    if (len > maxLen) len = maxLen;
    result.reserve(len);

    for (int i = 0; i < len; i++)
    {
        unsigned char c = static_cast<unsigned char>(input[i]);
        if (c == 0) break;

        // Strip control chars except basic whitespace
        if (c < 0x20 && c != ' ' && c != '\t')
            continue;

        result += static_cast<char>(c);
    }

    return result;
}

bool InputSanitizer::ValidatePacketString(const char* input, int expectedMaxLen)
{
    if (!input) return false;

    // Check for buffer overflow
    if (IsBufferOverflow(input, expectedMaxLen))
    {
        if (m_pLog)
            m_pLog->Write("[PacketString] Buffer overflow attempt detected (maxLen=%d)", expectedMaxLen);
        return false;
    }

    // Check for null byte injection
    if (ContainsNullByteInjection(input, expectedMaxLen))
    {
        if (m_pLog)
            m_pLog->Write("[PacketString] Null byte injection detected");
        return false;
    }

    return true;
}

// ---- Exploit Detection ----

bool InputSanitizer::ContainsSQLInjection(const char* input)
{
    if (!input) return false;

    // Convert to lowercase for pattern matching
    std::string lower(input);
    std::transform(lower.begin(), lower.end(), lower.begin(),
        [](unsigned char c) { return std::tolower(c); });

    // Check against known SQL patterns
    for (const auto& pattern : GetSQLPatterns())
    {
        if (lower.find(pattern) != std::string::npos)
            return true;
    }

    // Check for hex-encoded injection (0x27 = ', 0x3B = ;)
    if (lower.find("0x27") != std::string::npos ||
        lower.find("0x3b") != std::string::npos)
        return true;

    // Check for Unicode-encoded injection
    if (lower.find("%27") != std::string::npos ||  // URL-encoded '
        lower.find("%22") != std::string::npos ||  // URL-encoded "
        lower.find("%3b") != std::string::npos)     // URL-encoded ;
        return true;

    return false;
}

bool InputSanitizer::IsBufferOverflow(const char* input, int maxLen)
{
    if (!input) return false;

    // Don't use strlen (could crash on unterminated string)
    // Scan up to maxLen + 1 to detect overflow
    for (int i = 0; i <= maxLen; i++)
    {
        if (input[i] == 0)
            return false; // properly terminated within bounds
    }

    return true; // string exceeds maxLen without null terminator
}

bool InputSanitizer::ContainsNullByteInjection(const char* input, int bufferLen)
{
    if (!input) return false;

    // Look for null bytes followed by more data
    // This can bypass length checks in some implementations
    bool foundNull = false;
    for (int i = 0; i < bufferLen; i++)
    {
        if (input[i] == 0)
        {
            if (foundNull) return false; // double null = end
            foundNull = true;
            continue;
        }
        if (foundNull)
        {
            // Data after null byte = injection attempt
            return true;
        }
    }
    return false;
}
