#include "pch.h"

#include "WorldServerUtil.h"

#include <cstdarg>
#include <cstdio>

CWnd *g_pFrame = nullptr;

unsigned int GetLoopTime()
{
  return GetTickCount();
}

void WriteServerStartHistory(const char *format, ...)
{
  char buffer[512]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
}

void clear_file(const char *directory, int keepCount)
{
  (void)directory;
  (void)keepCount;
}

int MyMessageBox(const char *title, const char *format, ...)
{
  char buffer[1024]{};
  va_list args;
  va_start(args, format);
  vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args);
  va_end(args);
  OutputDebugStringA(buffer);
  OutputDebugStringA("\n");
  MessageBoxA(nullptr, buffer, title, MB_OK | MB_ICONERROR);
  return 0;
}

int MyCrtDebugReportHook(int reportType, char *message, int *returnValue)
{
  (void)reportType;
  (void)message;
  if (returnValue != nullptr)
  {
    *returnValue = 0;
  }
  return 0;
}
