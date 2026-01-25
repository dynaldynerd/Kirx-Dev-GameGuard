#pragma once

#include "IdaCompat.h"

struct __cppobj WheatyExceptionReport
{
  void SetLogName(const char *logName);
  void SetDescription(const char *description);
};

extern WheatyExceptionReport g_WheatyExceptionReport;
