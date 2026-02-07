#include "pch.h"

#include "CLogFile.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <cwchar>

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

void CLogFile::SetWriteLogFile(const char *fileName, int addCount, int date, int trace, int init)
{
  if (fileName != nullptr)
  {
    strncpy_s(m_szFileName, sizeof(m_szFileName), fileName, _TRUNCATE);
  }
  m_bAddCount = addCount != 0;
  m_bDate = date != 0;
  m_bTrace = trace != 0;
  m_bInit = init != 0;
  m_bWriteAble = 1;
}

void CLogFile::Write(const char *format, ...)
{
  va_list args;
  va_start(args, format);
  WriteFromArg(format, args);
  va_end(args);
}

void CLogFile::WriteFromArg(const char *format, va_list arg)
{
  if (!m_bInit || !m_bWriteAble)
  {
    return;
  }

  char buffer[0x2800]{};
  char line[11296]{};
  char date[0x80]{};
  char time[0x80]{};

  vsprintf_s(buffer, sizeof(buffer), format, arg);

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
      wsprintfA(line, "%d %s %s : %s\r\n", count, date, time, buffer);
    }
    else
    {
      const unsigned int count = m_dwLogCount++;
      wsprintfA(line, "%d : %s\r\n", count, buffer);
    }
  }
  else if (m_bDate)
  {
    wsprintfA(line, "%s %s %s\r\n", date, time, buffer);
  }
  else
  {
    wsprintfA(line, "%s\r\n", buffer);
  }

  HANDLE hFile = CreateFileA(m_szFileName, GENERIC_WRITE, FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  if (hFile != INVALID_HANDLE_VALUE)
  {
    SetFilePointer(hFile, 0, nullptr, FILE_END);
    DWORD written = 0;
    const DWORD len = strlen_0(line);
    WriteFile(hFile, line, len, &written, nullptr);
    CloseHandle(hFile);
  }
}

void CLogFile::WriteFromArg(const wchar_t *format, va_list arg)
{
  if (!m_bInit || !m_bWriteAble)
  {
    return;
  }

  wchar_t buffer[10256]{};
  wchar_t line[11280]{};
  wchar_t date[144]{};
  wchar_t time[140]{};

  vswprintf_s(buffer, 0x2800u, format, arg);

  if (m_bDate)
  {
    _wstrdate_s(date, 0x80u);
    _wstrtime_s(time, 0x80u);
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
      swprintf_s(line, 0x2C00u, L"%d %s %s : %s\r\n", count, date, time, buffer);
    }
    else
    {
      const unsigned int count = m_dwLogCount++;
      swprintf_s(line, 0x2C00u, L"%d : %s\r\n", count, buffer);
    }
  }
  else if (m_bDate)
  {
    swprintf_s(line, 0x2C00u, L"%s %s %s\r\n", date, time, buffer);
  }
  else
  {
    swprintf_s(line, 0x2C00u, L"%s\r\n", buffer);
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

    const size_t len = wcslen(line);
    fwrite(line, 2 * len, 1, stream);
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
  char date[0x80]{};
  char time[0x80]{};

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
    const DWORD len = strlen_0(line);
    WriteFile(hFile, line, len, &written, nullptr);
    CloseHandle(hFile);
  }
}
