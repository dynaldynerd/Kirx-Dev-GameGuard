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
  if (!m_bWriteAble)
  {
    return;
  }
  char buffer[1024]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
}
