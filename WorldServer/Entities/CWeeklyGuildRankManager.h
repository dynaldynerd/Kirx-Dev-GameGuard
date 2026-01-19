#pragma once

#include "IdaCompat.h"
#include "CWeeklyGuildRankInfo.h"

class __cppobj CWeeklyGuildRankManager
{
public:
  long long m_tNextUpdateTime;
  long long m_tNextSetOwnerTime;
  CWeeklyGuildRankInfo m_kInfo;
};
