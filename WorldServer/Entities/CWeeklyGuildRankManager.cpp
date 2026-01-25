#include "pch.h"

#include "CWeeklyGuildRankManager.h"

CWeeklyGuildRankManager *CWeeklyGuildRankManager::Instance()
{
  static CWeeklyGuildRankManager s_instance;
  return &s_instance;
}

bool CWeeklyGuildRankManager::Init()
{
  return true;
}
