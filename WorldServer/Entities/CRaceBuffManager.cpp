#include "pch.h"

#include "CRaceBuffManager.h"

CRaceBuffManager *CRaceBuffManager::Instance()
{
  static CRaceBuffManager s_instance;
  return &s_instance;
}

bool CRaceBuffManager::Init()
{
  return m_kBuffByHolyQuest.Init();
}

bool CRaceBuffManager::RequestHolyQuestRaceBuff(int iType)
{
  return m_kBuffByHolyQuest.Request(iType);
}
