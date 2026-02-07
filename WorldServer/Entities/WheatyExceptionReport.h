#pragma once

#include "IdaCompat.h"

struct __cppobj WheatyExceptionReport
{
  void SetLogName(const char *logName);
  void SetDescription(const char *description);

  static char m_szLogName[256];
  static char m_szDescription[256];
};

extern WheatyExceptionReport g_WheatyExceptionReport;
