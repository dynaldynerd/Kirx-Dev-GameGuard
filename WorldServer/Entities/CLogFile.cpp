#include "pch.h"

#include "CLogFile.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

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
