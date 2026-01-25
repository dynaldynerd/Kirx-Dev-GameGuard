#include "pch.h"

#include "CAsyncLogger.h"

CAsyncLogger *CAsyncLogger::Instance()
{
  static CAsyncLogger s_instance;
  return &s_instance;
}

void CAsyncLogger::Regist(int logType, const char *directory, const char *filePrefix, int isDaily, unsigned int maxSize)
{
  (void)logType;
  (void)directory;
  (void)filePrefix;
  (void)isDaily;
  (void)maxSize;
}

int CAsyncLogger::Init()
{
  return 0;
}
