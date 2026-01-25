#pragma once

#include "IdaCompat.h"
#include "CWeeklyGuildRankInfo.h"

class __cppobj CWeeklyGuildRankManager
{
public:
  static CWeeklyGuildRankManager *Instance();
  bool Init();

  long long m_tNextUpdateTime;
  long long m_tNextSetOwnerTime;
  CWeeklyGuildRankInfo m_kInfo;
};
