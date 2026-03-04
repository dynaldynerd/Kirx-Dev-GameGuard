#include "pch.h"

#include "CRaceBuffManager.h"

CRaceBuffManager *CRaceBuffManager::ms_Instance = nullptr;

CRaceBuffManager *CRaceBuffManager::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CRaceBuffManager();
  }
  return ms_Instance;
}

void CRaceBuffManager::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
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

int CRaceBuffManager::GetRaceBuffLevel(CPlayer *pOne)
{
  return m_kBuffByHolyQuest.GetRaceBuffLevel(pOne);
}

int CRaceBuffManager::CancelPlayerRaceBuff(
  CPlayer *pkPlayer,
  CRaceBuffInfoByHolyQuestfGroup::RESULT_TYPE eReleaseType,
  unsigned int uiContinueCnt)
{
  return m_kBuffByHolyQuest.CancelPlayerRaceBuff(pkPlayer, eReleaseType, uiContinueCnt);
}
