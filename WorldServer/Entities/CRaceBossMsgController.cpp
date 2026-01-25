#include "pch.h"

#include "CRaceBossMsgController.h"

#include "WorldServerUtil.h"

#include <ctime>
#include <cstring>

CRaceBossMsgController *CRaceBossMsgController::Instance()
{
  static CRaceBossMsgController s_instance;
  return &s_instance;
}

bool RACE_BOSS_MSG::CMsgListManager::IsHaveBeenSave()
{
  char flag[10]{};
  GetPrivateProfileStringA("RaceBossSMSSave", "Flag", "FALSE", flag, sizeof(flag), "..\\SystemSave\\ServerState.ini");
  return std::strncmp("TRUE", flag, 5) == 0;
}

bool RACE_BOSS_MSG::CMsgListManager::Load(unsigned int dwCurTime)
{
  if (m_bEmpty)
  {
    return false;
  }
  bool ok = true;
  for (int j = 0; j < 3; ++j)
  {
    if (m_pkMsgList[j] != nullptr && !m_pkMsgList[j]->Load(dwCurTime))
    {
      ok = false;
    }
  }
  return ok;
}

void RACE_BOSS_MSG::CMsgListManager::Save()
{
  if (m_bEmpty)
  {
    return;
  }
  for (int j = 0; j < 3; ++j)
  {
    if (m_pkMsgList[j] != nullptr)
    {
      m_pkMsgList[j]->Save();
    }
  }
  WritePrivateProfileStringA("RaceBossSMSSave", "Flag", "TRUE", "..\\SystemSave\\ServerState.ini");
}

bool RACE_BOSS_MSG::CMsgList::Load(unsigned int dwCurTime)
{
  (void)dwCurTime;
  return m_ppMsg != nullptr;
}

bool RACE_BOSS_MSG::CMsgList::Save()
{
  return true;
}

int CRaceBossMsgController::GetCurDay()
{
  std::time_t now = std::time(nullptr);
  std::tm local{};
  if (localtime_s(&local, &now) != 0)
  {
    return -1;
  }
  return local.tm_mday;
}

bool CRaceBossMsgController::LoadCurTime(unsigned int *dwCurTime)
{
  UINT value = GetPrivateProfileIntA("RaceBossSMSCurTime", "Time", -1, "..\\SystemSave\\ServerState.ini");
  if (value == static_cast<UINT>(-1))
  {
    return false;
  }
  *dwCurTime = value;
  return true;
}

bool CRaceBossMsgController::Init()
{
  m_pkTimer = new CMyTimer();
  if (m_pkTimer == nullptr)
  {
    return false;
  }
  m_iOldDay = GetCurDay();
  if (m_iOldDay < 0)
  {
    return false;
  }
  m_pkTimer->BeginTimer(30000);

  unsigned int curTime = 0;
  if (m_kManager.IsHaveBeenSave() && LoadCurTime(&curTime))
  {
    return m_kManager.Load(curTime);
  }

  m_kManager.Save();
  return true;
}
