#pragma once

#include "IdaCompat.h"

class __cppobj CAsyncLogger
{
public:
  enum AsyncLogType
  {
    ALT_HACKSHIELD_SYSTEM_LOG = 0,
    ALT_APEX_SYSTEM_LOG = 1,
    ALT_FIREGUARD_DETECT_LOG = 2,
    ALT_HONOR_GUILD_LOG = 3,
    ALT_BUY_CASH_ITEM_LOG = 4,
  };

  static CAsyncLogger *Instance();
  void Regist(int logType, const char *directory, const char *filePrefix, int isDaily, unsigned int maxSize);
  int Init();
};

