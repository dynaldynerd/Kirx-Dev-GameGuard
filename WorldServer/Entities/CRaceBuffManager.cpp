#include "pch.h"

#include "CRaceBuffManager.h"

CRaceBuffManager *CRaceBuffManager::Instance()
{
  static CRaceBuffManager s_instance;
  return &s_instance;
}

CRaceBuffManager::CRaceBuffManager()
  : m_kBuffByHolyQuest()
{
}

CRaceBuffManager::~CRaceBuffManager() = default;

void CRaceBuffManager::Loop()
{
  m_kBuffByHolyQuest.Loop();
}

bool CRaceBuffManager::Init()
{
  return m_kBuffByHolyQuest.Init();
}

bool CRaceBuffManager::RequestHolyQuestRaceBuff(int iType)
{
  return m_kBuffByHolyQuest.Request(iType);
}

bool CRaceBuffManager::CreateComplete(CPlayer *pkPlayer)
{
  return m_kBuffByHolyQuest.CreateComplete(pkPlayer);
}
