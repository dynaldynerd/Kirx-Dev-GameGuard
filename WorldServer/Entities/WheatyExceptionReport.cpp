#include "pch.h"

#include "WheatyExceptionReport.h"

WheatyExceptionReport g_WheatyExceptionReport;

char WheatyExceptionReport::m_szLogName[256]{};
char WheatyExceptionReport::m_szDescription[256]{};

void WheatyExceptionReport::SetLogName(const char *logName)
{
  strcpy_0(WheatyExceptionReport::m_szLogName, logName);
}

void WheatyExceptionReport::SetDescription(const char *description)
{
  strcpy_0(WheatyExceptionReport::m_szDescription, description);
}
