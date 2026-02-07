#pragma once

#include "IdaCompat.h"
#include "CWeeklyGuildRankInfo.h"

class CRFWorldDatabase;

class __cppobj CWeeklyGuildRankManager
{
public:
  static CWeeklyGuildRankManager *Instance();
  bool Init();
  bool InitNextSetOwnerDate();
  void SetNextRankDate();
  bool InsertSettlementOwner(CRFWorldDatabase *pkWorldDB, char *pData);

  long long m_tNextUpdateTime;
  long long m_tNextSetOwnerTime;
  CWeeklyGuildRankInfo m_kInfo;
};
