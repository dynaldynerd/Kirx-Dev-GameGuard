#include "pch.h"

#include "CLogFile.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <vector>

CLogFile::CLogFile()
  : m_dwLogCount(0),
    m_bWriteAble(0),
    m_bAddCount(true),
    m_bDate(false),
    m_bTrace(false),
    m_bInit(false)
{
  m_szFileName[0] = 0;
}

CLogFile::~CLogFile()
{
}

void CLogFile::SetWriteLogFile(const char *szFileName, int bWriteAble, bool bTrace, bool bDate, bool bAddCount)
{
  char normalizedPath[128]{};
  if (szFileName != nullptr)
  {
    strncpy_s(normalizedPath, sizeof(normalizedPath), szFileName, _TRUNCATE);
    for (char *cursor = normalizedPath; *cursor != '\0'; ++cursor)
    {
      if (*cursor == '/' || *cursor == ':')
      {
        *cursor = '_';
      }
    }
  }

  m_szFileName[0] = 0;
  wsprintfA(m_szFileName, "%s", normalizedPath);
  DeleteFileA(m_szFileName);

  m_dwLogCount = 0;
  m_bInit = true;
  m_bWriteAble = bWriteAble;
  m_bAddCount = bAddCount;
  m_bDate = bDate;
  m_bTrace = bTrace;
}

void CLogFile::SetWriteAble(bool bAble)
{
  m_bWriteAble = bAble ? 1 : 0;
}

void CLogFile::Write(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  if (m_bInit && m_bWriteAble)
  {
    char line[11296]{};
    char message[10272]{};
    char date[160]{};
    char time[152]{};

    vsprintf_s(message, sizeof(message), format, args);
    if (m_bDate)
    {
      _strdate_s(date, 128);
      _strtime_s(time, 128);
    }
    else
    {
      date[0] = 0;
      time[0] = 0;
    }

    if (m_bAddCount)
    {
      if (m_bDate)
      {
        const unsigned int count = m_dwLogCount++;
        wsprintfA(line, "%d %s %s : %s\r\n", count, date, time, message);
      }
      else
      {
        const unsigned int count = m_dwLogCount++;
        wsprintfA(line, "%d : %s\r\n", count, message);
      }
    }
    else if (m_bDate)
    {
      wsprintfA(line, "%s %s %s\r\n", date, time, message);
    }
    else
    {
      wsprintfA(line, "%s\r\n", message);
    }

    HANDLE file = CreateFileA(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file != INVALID_HANDLE_VALUE)
    {
      SetFilePointer(file, 0, nullptr, FILE_END);
      const DWORD length = static_cast<DWORD>(std::strlen(line));
      DWORD written = 0;
      WriteFile(file, line, length, &written, nullptr);
      CloseHandle(file);
    }
  }
  va_end(args);
}

void CLogFile::WriteFromArg(const char *format, va_list arg)
{
  if (!m_bInit || !m_bWriteAble)
  {
    return;
  }

  char date[128]{};
  char time[128]{};

  va_list countArg;
  va_copy(countArg, arg);
  const int messageLen = _vscprintf(format, countArg);
  va_end(countArg);
  if (messageLen < 0)
  {
    return;
  }

  std::vector<char> buffer(static_cast<size_t>(messageLen) + 1);
  va_list writeArg;
  va_copy(writeArg, arg);
  if (vsprintf_s(buffer.data(), buffer.size(), format, writeArg) < 0)
  {
    va_end(writeArg);
    return;
  }
  va_end(writeArg);

  if (m_bDate)
  {
    _strdate_s(date, sizeof(date));
    _strtime_s(time, sizeof(time));
  }
  else
  {
    date[0] = 0;
    time[0] = 0;
  }

  unsigned int count = 0;
  const char *lineFormat = nullptr;
  if (m_bAddCount)
  {
    count = m_dwLogCount++;
    if (m_bDate)
    {
      lineFormat = "%d %s %s : %s\r\n";
    }
    else
    {
      lineFormat = "%d : %s\r\n";
    }
  }
  else if (m_bDate)
  {
    lineFormat = "%s %s %s\r\n";
  }
  else
  {
    lineFormat = "%s\r\n";
  }

  int lineLen = 0;
  if (m_bAddCount)
  {
    if (m_bDate)
    {
      lineLen = _scprintf(lineFormat, count, date, time, buffer.data());
    }
    else
    {
      lineLen = _scprintf(lineFormat, count, buffer.data());
    }
  }
  else if (m_bDate)
  {
    lineLen = _scprintf(lineFormat, date, time, buffer.data());
  }
  else
  {
    lineLen = _scprintf(lineFormat, buffer.data());
  }

  if (lineLen < 0)
  {
    return;
  }

  std::vector<char> line(static_cast<size_t>(lineLen) + 1);
  if (m_bAddCount)
  {
    if (m_bDate)
    {
      sprintf_s(line.data(), line.size(), lineFormat, count, date, time, buffer.data());
    }
    else
    {
      sprintf_s(line.data(), line.size(), lineFormat, count, buffer.data());
    }
  }
  else if (m_bDate)
  {
    sprintf_s(line.data(), line.size(), lineFormat, date, time, buffer.data());
  }
  else
  {
    sprintf_s(line.data(), line.size(), lineFormat, buffer.data());
  }

  HANDLE hFile = CreateFileA(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, FILE_END);
    DWORD written = 0;
    const DWORD len = static_cast<DWORD>(std::strlen(line.data()));
    WriteFile(hFile, line.data(), len, &written, nullptr);
    CloseHandle(hFile);
  }
}

void CLogFile::WriteFromArg(const wchar_t *format, va_list arg)
{
  if (!m_bInit || !m_bWriteAble)
  {
    return;
  }

  wchar_t date[144]{};
  wchar_t time[140]{};

  va_list countArg;
  va_copy(countArg, arg);
  const int messageLen = _vscwprintf(format, countArg);
  va_end(countArg);
  if (messageLen < 0)
  {
    return;
  }

  std::vector<wchar_t> buffer(static_cast<size_t>(messageLen) + 1);
  va_list writeArg;
  va_copy(writeArg, arg);
  if (vswprintf_s(buffer.data(), buffer.size(), format, writeArg) < 0)
  {
    va_end(writeArg);
    return;
  }
  va_end(writeArg);

  if (m_bDate)
  {
    _wstrdate_s(date, 128);
    _wstrtime_s(time, 128);
  }
  else
  {
    date[0] = 0;
    time[0] = 0;
  }

  unsigned int count = 0;
  const wchar_t *lineFormat = nullptr;
  if (m_bAddCount)
  {
    count = m_dwLogCount++;
    if (m_bDate)
    {
      lineFormat = L"%d %s %s : %s\r\n";
    }
    else
    {
      lineFormat = L"%d : %s\r\n";
    }
  }
  else if (m_bDate)
  {
    lineFormat = L"%s %s %s\r\n";
  }
  else
  {
    lineFormat = L"%s\r\n";
  }

  int lineLen = 0;
  if (m_bAddCount)
  {
    if (m_bDate)
    {
      lineLen = _scwprintf(lineFormat, count, date, time, buffer.data());
    }
    else
    {
      lineLen = _scwprintf(lineFormat, count, buffer.data());
    }
  }
  else if (m_bDate)
  {
    lineLen = _scwprintf(lineFormat, date, time, buffer.data());
  }
  else
  {
    lineLen = _scwprintf(lineFormat, buffer.data());
  }

  if (lineLen < 0)
  {
    return;
  }

  std::vector<wchar_t> line(static_cast<size_t>(lineLen) + 1);
  if (m_bAddCount)
  {
    if (m_bDate)
    {
      swprintf_s(line.data(), line.size(), lineFormat, count, date, time, buffer.data());
    }
    else
    {
      swprintf_s(line.data(), line.size(), lineFormat, count, buffer.data());
    }
  }
  else if (m_bDate)
  {
    swprintf_s(line.data(), line.size(), lineFormat, date, time, buffer.data());
  }
  else
  {
    swprintf_s(line.data(), line.size(), lineFormat, buffer.data());
  }

  FILE *stream = nullptr;
  fopen_s(&stream, m_szFileName, "ab");
  if (stream)
  {
    if (m_dwLogCount == 1)
    {
      unsigned __int16 bom = static_cast<unsigned __int16>(-257);
      fwrite(&bom, sizeof(bom), 1, stream);
    }

    const size_t len = wcslen(line.data());
    fwrite(line.data(), 2 * len, 1, stream);
    fclose(stream);
  }
}

void CLogFile::WriteString(const char *fmt)
{
  if (!m_bInit || !m_bWriteAble)
  {
    return;
  }

  char line[10272]{};
  char date[128]{};
  char time[128]{};

  if (m_bDate)
  {
    _strdate_s(date, sizeof(date));
    _strtime_s(time, sizeof(time));
  }
  else
  {
    date[0] = 0;
    time[0] = 0;
  }

  if (m_bAddCount)
  {
    if (m_bDate)
    {
      const unsigned int count = m_dwLogCount++;
      wsprintfA(line, "%d %s %s : %s\r\n", count, date, time, fmt);
    }
    else
    {
      const unsigned int count = m_dwLogCount++;
      wsprintfA(line, "%d : %s\r\n", count, fmt);
    }
  }
  else if (m_bDate)
  {
    wsprintfA(line, "%s %s %s\r\n", date, time, fmt);
  }
  else
  {
    wsprintfA(line, "%s\r\n", fmt);
  }

  HANDLE hFile = CreateFileA(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, FILE_END);
    DWORD written = 0;
    const DWORD len = static_cast<DWORD>(std::strlen(line));
    WriteFile(hFile, line, len, &written, nullptr);
    CloseHandle(hFile);
  }
}
