#include "pch.h"

#include "CTotalGuildRankManager.h"
#include "WorldServerUtil.h"

CTotalGuildRankManager *CTotalGuildRankManager::Instance()
{
  static CTotalGuildRankManager s_instance;
  return &s_instance;
}

bool CTotalGuildRankManager::Init()
{
  m_iOldDay = GetCurDay();
  return m_iOldDay != -1 && m_kInfo.Init();
}
