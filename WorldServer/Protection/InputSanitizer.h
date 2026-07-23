#pragma once
// ============================================================================
// InputSanitizer.h - SQL Injection Prevention & Input Validation
// Covers: Rename Potion, Character Name, Guild Name, Chat, all DB strings
// ============================================================================

#include <string>
#include <vector>
#include <set>

class CLogFile;

class InputSanitizer
{
public:
    static InputSanitizer& Instance()
    {
        static InputSanitizer inst;
        return inst;
    }

    void Initialize();

    // ---- Core sanitization ----

    // Sanitize any string before SQL query. Removes/escapes dangerous chars.
    // Returns sanitized string. Original is NOT modified.
    std::string SanitizeForSQL(const char* input, int maxLen = 64);

    // Validate and sanitize character name (rename potion, creation)
    // Returns false if name is invalid (hack attempt)
    bool ValidateName(const char* name, int nameType = 0);

    // Validate guild name
    bool ValidateGuildName(const char* name);

    // Validate chat message (strip control chars, prevent injection)
    std::string SanitizeChat(const char* input, int maxLen = 256);

    // Validate any packet string field (generic)
    bool ValidatePacketString(const char* input, int expectedMaxLen);

    // ---- Specific exploit checks ----

    // Check for SQL injection patterns
    bool ContainsSQLInjection(const char* input);

    // Check for buffer overflow attempt (string longer than expected)
    bool IsBufferOverflow(const char* input, int maxLen);

    // Check for null byte injection
    bool ContainsNullByteInjection(const char* input, int bufferLen);

private:
    InputSanitizer() : m_pLog(nullptr) {}

    // Dangerous SQL keywords/patterns
    static const std::vector<std::string>& GetSQLPatterns();

    CLogFile* m_pLog;
};
