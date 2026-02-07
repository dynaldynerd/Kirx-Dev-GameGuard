#include "pch.h"

#include "CRaceBossMsgController.h"

#include "WorldServerUtil.h"

#include <ctime>
#include <cstring>
#include <cstdio>

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

bool RACE_BOSS_MSG::CMsg::Save(unsigned __int8 ucRace)
{
  const char *raceName[3] = {"B", "C", "A"};
  char stringBuffer[288];
  char sectionName[288];
  char translated[1296];

  memset_0(sectionName, 0, 0xFFu);
  memset_0(stringBuffer, 0, 0xFFu);
  sprintf(sectionName, "%s_Message%d", raceName[ucRace], m_dwID);
  sprintf(stringBuffer, "%d", m_uiState);
  if (!WritePrivateProfileStringA(sectionName, "State", stringBuffer, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }
  sprintf(stringBuffer, "%d", m_dwSerial);
  if (!WritePrivateProfileStringA(sectionName, "Serial", stringBuffer, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }
  W2M(m_wszName, translated, 0x500u);
  if (!WritePrivateProfileStringA(sectionName, "Name", translated, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }
  W2M(m_wszMsg, translated, 0x500u);
  if (!WritePrivateProfileStringA(sectionName, "Msg", translated, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }
  sprintf(stringBuffer, "%d", m_dwSendTime);
  return WritePrivateProfileStringA(sectionName, "SendTime", stringBuffer, "..\\SystemSave\\ServerState.ini");
}

bool RACE_BOSS_MSG::CMsgList::Save()
{
  if (!m_ppMsg)
  {
    return false;
  }
  if (!SaveIndexList(0, &m_kEmptyInxList))
  {
    return false;
  }
  if (!SaveIndexList(1, &m_kUseInxList))
  {
    return false;
  }
  if (!SaveIndexList(2, &m_kWaitInxList))
  {
    return false;
  }
  return SaveMsgList(&m_kUseInxList);
}

bool RACE_BOSS_MSG::CMsgList::SaveIndexList(unsigned int iType, CNetIndexList *kInxList)
{
  if (iType >= 4)
  {
    return false;
  }

  const char *raceName[3] = {"B", "C", "A"};
  const char *typeName[3] = {"Empty", "Use", "Wait"};
  char sectionName[288];
  char keyName[288];
  char stringBuffer[280];

  memset_0(sectionName, 0, 0xFFu);
  memset_0(keyName, 0, 0xFFu);
  memset_0(stringBuffer, 0, 0xFFu);
  sprintf(sectionName, "%s_%s_IndexList", raceName[m_ucRace], typeName[iType]);

  const int listSize = static_cast<int>(kInxList->size());
  sprintf(stringBuffer, "%d", listSize);
  if (!WritePrivateProfileStringA(sectionName, "Count", stringBuffer, "..\\SystemSave\\ServerState.ini"))
  {
    return false;
  }

  int written = 0;
  if (kInxList->size() > 0)
  {
    kInxList->m_csList.Lock();
    for (CNetIndexList::_index_node *node = kInxList->m_Head.m_pNext;
         node != &kInxList->m_Tail;
         node = node->m_pNext)
    {
      sprintf(keyName, "%02d_Th", written);
      sprintf(stringBuffer, "%d", node->m_dwIndex);
      if (!WritePrivateProfileStringA(sectionName, keyName, stringBuffer, "..\\SystemSave\\ServerState.ini"))
      {
        kInxList->m_csList.Unlock();
        return false;
      }
      ++written;
    }
    kInxList->m_csList.Unlock();
  }

  for (int k = written; k < 2; ++k)
  {
    sprintf(keyName, "%02d_Th", k);
    WritePrivateProfileStringA(sectionName, keyName, "-1", "..\\SystemSave\\ServerState.ini");
  }
  return true;
}

bool RACE_BOSS_MSG::CMsgList::SaveMsgList(CNetIndexList *kInxList)
{
  if (!m_ppMsg)
  {
    return false;
  }
  if (!kInxList->size())
  {
    return true;
  }

  kInxList->m_csList.Lock();
  for (CNetIndexList::_index_node *node = kInxList->m_Head.m_pNext;
       node != &kInxList->m_Tail;
       node = node->m_pNext)
  {
    if (!m_ppMsg[node->m_dwIndex]->Save(m_ucRace))
    {
      kInxList->m_csList.Unlock();
      return false;
    }
  }
  kInxList->m_csList.Unlock();
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
