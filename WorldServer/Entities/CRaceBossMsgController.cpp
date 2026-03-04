#include "pch.h"

#include "CRaceBossMsgController.h"

#include <cstdio>
#include <ctime>
#include <cstring>
#include <new>

#include <mmsystem.h>

#include "CNetworkEX.h"
#include "CMainThread.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CPlayer.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "raceboss_msg_confirm_zowb.h"
#include "racebosssms_fromweb_send_error_result_zowb.h"

CRaceBossMsgController *CRaceBossMsgController::ms_Instance = nullptr;

CRaceBossMsgController *CRaceBossMsgController::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CRaceBossMsgController();
  }
  return ms_Instance;
}

void CRaceBossMsgController::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

CRaceBossMsgController::CRaceBossMsgController()
  : m_pkTimer(nullptr), m_kManager()
{
}

CRaceBossMsgController::~CRaceBossMsgController()
{
  CleanUp();
}

void CRaceBossMsgController::CleanUp()
{
  if (m_pkTimer)
  {
    delete m_pkTimer;
    m_pkTimer = nullptr;
  }
}

RACE_BOSS_MSG::CMsg::CMsg(unsigned __int8 /*ucRace*/, unsigned int dwID)
{
  Clear();
  m_dwID = dwID;
  m_dwWebSendDBID = 0;
}

RACE_BOSS_MSG::CMsg::~CMsg()
{
  // this is not a stub
}

void RACE_BOSS_MSG::CMsg::Clear()
{
  m_uiState = 0;
  m_dwSerial = static_cast<unsigned int>(-1);
  memset_0(m_wszName, 0, sizeof(m_wszName));
  memset_0(m_wszMsg, 0, sizeof(m_wszMsg));
  m_dwSendTime = 0;
  m_dwWebSendDBID = 0;
}

char RACE_BOSS_MSG::CMsg::Set(
  unsigned int dwSerial,
  const char *pwszName,
  const char *pwszMsg,
  unsigned int dbWebSendDBID)
{
  if (m_uiState == 3)
  {
    return 0;
  }
  m_dwSerial = dwSerial;
  strncpy(m_wszName, pwszName, 0x10u);
  m_wszName[16] = 0;
  strncpy(m_wszMsg, pwszMsg, 0x30u);
  m_wszMsg[48] = 0;
  for (int j = 0; j <= 48; ++j)
  {
    if (m_wszMsg[j] == 39)
    {
      m_wszMsg[j] = 32;
    }
  }
  m_dwSendTime = timeGetTime();
  m_dwWebSendDBID = dbWebSendDBID;
  m_uiState = 1;
  return 1;
}

unsigned int RACE_BOSS_MSG::CMsg::GetID()
{
  return m_dwID;
}

unsigned int RACE_BOSS_MSG::CMsg::GetSerial()
{
  return m_dwSerial;
}

char *RACE_BOSS_MSG::CMsg::GetBossName()
{
  return m_wszName;
}

char *RACE_BOSS_MSG::CMsg::GetMsg()
{
  return m_wszMsg;
}

unsigned int RACE_BOSS_MSG::CMsg::GetWebDBID()
{
  return m_dwWebSendDBID;
}

void RACE_BOSS_MSG::CMsg::SetDayChanged()
{
  m_uiState = 2;
}

void RACE_BOSS_MSG::CMsg::SetDone()
{
  m_uiState = 3;
}

bool RACE_BOSS_MSG::CMsg::IsWait()
{
  return m_uiState == 1;
}

bool RACE_BOSS_MSG::CMsg::IsDayChanged()
{
  return m_uiState == 2;
}

bool RACE_BOSS_MSG::CMsg::IsSendTime()
{
  if (!m_uiState || m_uiState == 3)
  {
    return false;
  }

  const DWORD now = timeGetTime();
  return m_dwSendTime <= now;
}

bool RACE_BOSS_MSG::CMsg::IsSendFromWeb()
{
  return m_dwWebSendDBID != 0;
}

RACE_BOSS_MSG::CMsgList::CMsgList(unsigned __int8 ucRace, unsigned int uiSize)
  : m_ucRace(ucRace),
    m_ppMsg(nullptr),
    m_uiSize(uiSize)
{
  Init();
}

RACE_BOSS_MSG::CMsgList::~CMsgList()
{
  CleanUp();
}

char RACE_BOSS_MSG::CMsgList::Init()
{
  if (!m_uiSize)
  {
    return 0;
  }

  m_kEmptyInxList.SetList(m_uiSize);
  m_kUseInxList.SetList(m_uiSize);
  m_kWaitInxList.SetList(m_uiSize);

  m_ppMsg = new (std::nothrow) CMsg *[m_uiSize];
  for (unsigned int id = 0; id < m_uiSize; ++id)
  {
    CMsg *msg = new (std::nothrow) CMsg(m_ucRace, id);
    m_ppMsg[id] = msg;
  }

  for (unsigned int index = 0; index < 2; ++index)
  {
    m_kEmptyInxList.PushNode_Back(index);
  }
  for (unsigned int index = 2; index < 4; ++index)
  {
    m_kWaitInxList.PushNode_Back(index);
  }
  return 1;
}

void RACE_BOSS_MSG::CMsgList::CleanUp()
{
  if (!m_uiSize)
  {
    return;
  }
  for (unsigned int j = 0; j < m_uiSize; ++j)
  {
    delete m_ppMsg[j];
  }
  delete[] m_ppMsg;
}

RACE_BOSS_MSG::CMsg *RACE_BOSS_MSG::CMsgList::GetEmpty()
{
  unsigned int outIndex = 0;
  if (!m_kEmptyInxList.PopNode_Front(&outIndex))
  {
    return nullptr;
  }
  m_ppMsg[outIndex]->Clear();
  return m_ppMsg[outIndex];
}

void RACE_BOSS_MSG::CMsgList::AddUse(CMsg *pkMsg)
{
  if (!pkMsg)
  {
    return;
  }
  const unsigned int id = pkMsg->GetID();
  if (!m_kUseInxList.IsInList(id))
  {
    m_kUseInxList.PushNode_Back(id);
  }
}

void RACE_BOSS_MSG::CMsgList::RollBack()
{
  if (m_uiSize && m_kWaitInxList.size())
  {
    unsigned int outIndex = static_cast<unsigned int>(-1);
    m_kUseInxList.PopNode_Back(&outIndex);
    if (!m_kEmptyInxList.IsInList(outIndex))
    {
      m_kEmptyInxList.PushNode_Front(outIndex);
    }
  }
}

int RACE_BOSS_MSG::CMsgList::Cancel(unsigned int dwMsgID, CMsg **pkMsg)
{
  if (!m_uiSize)
  {
    return 1;
  }
  if (!m_kUseInxList.size())
  {
    return 2;
  }
  if (!m_kUseInxList.FindNode(dwMsgID))
  {
    return 2;
  }
  *pkMsg = m_ppMsg[dwMsgID];
  return 0;
}

void RACE_BOSS_MSG::CMsgList::AddEmpty(CMsg *pkMsg)
{
  if (!pkMsg)
  {
    return;
  }
  const unsigned int id = pkMsg->GetID();
  if (!m_kEmptyInxList.IsInList(id))
  {
    m_kEmptyInxList.PushNode_Front(id);
  }
}

void RACE_BOSS_MSG::CMsgList::Release(CMsg *pkMsg)
{
  if (!pkMsg)
  {
    return;
  }
  const unsigned int dwIndex = pkMsg->GetID();
  if (!m_kWaitInxList.IsInList(dwIndex))
  {
    m_kUseInxList.FindNode(dwIndex);
    m_kWaitInxList.PushNode_Back(dwIndex);
    pkMsg->Clear();
    pkMsg->SetDone();
  }
}

RACE_BOSS_MSG::CMsg *RACE_BOSS_MSG::CMsgList::GetSendMsg()
{
  unsigned int outIndex = 0;
  if (!m_kUseInxList.CopyFront(&outIndex))
  {
    return nullptr;
  }

  CMsg *msg = m_ppMsg[outIndex];
  if (msg->IsSendTime())
  {
    return msg;
  }
  return nullptr;
}

int RACE_BOSS_MSG::CMsgList::GetRemainCnt()
{
  return static_cast<int>(m_kEmptyInxList.size());
}

void RACE_BOSS_MSG::CMsgList::Refresh()
{
  if (!m_uiSize)
  {
    return;
  }

  if (m_kUseInxList.size() > 0)
  {
    m_kUseInxList.m_csList.Lock();
    for (CNetIndexList::_index_node *node = m_kUseInxList.m_Head.m_pNext;
         node != &m_kUseInxList.m_Tail;
         node = node->m_pNext)
    {
      m_ppMsg[node->m_dwIndex]->SetDayChanged();
    }
    m_kUseInxList.m_csList.Unlock();
  }

  const int moveCount = 2 - static_cast<int>(m_kEmptyInxList.size());
  if (moveCount > 0)
  {
    unsigned int outIndex = 0;
    for (int index = 0; index < moveCount && m_kWaitInxList.PopNode_Front(&outIndex); ++index)
    {
      m_kEmptyInxList.PushNode_Back(outIndex);
    }
  }
}

RACE_BOSS_MSG::CMsgListManager::CMsgListManager()
  : m_bEmpty(true)
{
  memset_0(m_pkMsgList, 0, sizeof(m_pkMsgList));
  if (Init())
  {
    m_bEmpty = false;
  }
}

RACE_BOSS_MSG::CMsgListManager::~CMsgListManager()
{
  CleanUp();
}

char RACE_BOSS_MSG::CMsgListManager::Init()
{
  char ok = 1;
  for (int j = 0; j < 3; ++j)
  {
    CMsgList *list = new (std::nothrow) CMsgList(static_cast<unsigned __int8>(j), 4u);
    m_pkMsgList[j] = list;
    if (!m_pkMsgList[j])
    {
      ok = 0;
    }
  }
  return ok;
}

void RACE_BOSS_MSG::CMsgListManager::CleanUp()
{
  for (int j = 0; j < 3; ++j)
  {
    delete m_pkMsgList[j];
    m_pkMsgList[j] = nullptr;
  }
}

int RACE_BOSS_MSG::CMsgListManager::Send(
  unsigned __int8 ucRace,
  unsigned int dwSerial,
  const char *pwszName,
  const char *pwszMsg,
  CMsg **pkSend,
  unsigned int dbWebSendDBID)
{
  if (m_bEmpty)
  {
    return 1;
  }
  if (ucRace >= 3u || pwszName == nullptr || pwszMsg == nullptr || !strlen_0(pwszMsg))
  {
    return 1;
  }

  CMsg *msg = m_pkMsgList[ucRace]->GetEmpty();
  if (!msg)
  {
    return 4;
  }

  msg->Set(dwSerial, pwszName, pwszMsg, dbWebSendDBID);
  m_pkMsgList[ucRace]->AddUse(msg);
  if (m_pkMsgList[ucRace]->Save()
      && WritePrivateProfileStringA("RaceBossSMSSave", "Flag", "TRUE", "..\\SystemSave\\ServerState.ini"))
  {
    *pkSend = msg;
    return 0;
  }

  m_pkMsgList[ucRace]->RollBack();
  return 1;
}

int RACE_BOSS_MSG::CMsgListManager::Cancel(
  unsigned __int8 ucRace,
  unsigned int dwMsgID,
  CMsg **pkMsg)
{
  if (m_bEmpty)
  {
    return 1;
  }
  if (ucRace < 3u)
  {
    return m_pkMsgList[ucRace]->Cancel(dwMsgID, pkMsg);
  }
  return 2;
}

void RACE_BOSS_MSG::CMsgListManager::CleanUpCancel(unsigned __int8 ucRace, CMsg *pkMsg)
{
  if (!m_bEmpty && ucRace < 3u)
  {
    if (pkMsg->IsWait())
    {
      m_pkMsgList[ucRace]->AddEmpty(pkMsg);
    }
    else
    {
      if (!pkMsg->IsDayChanged())
      {
        return;
      }
      m_pkMsgList[ucRace]->Release(pkMsg);
    }
    m_pkMsgList[ucRace]->Save();
  }
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

RACE_BOSS_MSG::CMsg *RACE_BOSS_MSG::CMsgListManager::GetSendMsg(unsigned __int8 *ucRace)
{
  if (m_bEmpty)
  {
    return nullptr;
  }

  for (int race = 0; race < 3; ++race)
  {
    CMsg *msg = m_pkMsgList[race]->GetSendMsg();
    if (msg)
    {
      *ucRace = static_cast<unsigned __int8>(race);
      return msg;
    }
  }

  return nullptr;
}

unsigned __int8 RACE_BOSS_MSG::CMsgListManager::GetRemainCnt(unsigned __int8 ucRace)
{
  if (m_bEmpty || ucRace >= 3)
  {
    return 0;
  }

  return static_cast<unsigned __int8>(m_pkMsgList[ucRace]->GetRemainCnt());
}

void RACE_BOSS_MSG::CMsgListManager::Release(unsigned __int8 ucRace, CMsg *pkMsg)
{
  if (!m_bEmpty && ucRace < 3)
  {
    m_pkMsgList[ucRace]->Release(pkMsg);
  }
}

void RACE_BOSS_MSG::CMsgListManager::Refresh()
{
  if (m_bEmpty)
  {
    return;
  }

  for (int race = 0; race < 3; ++race)
  {
    m_pkMsgList[race]->Refresh();
  }
}

void RACE_BOSS_MSG::CMsgListManager::Save(unsigned __int8 ucRace)
{
  if (!m_bEmpty && ucRace < 3)
  {
    m_pkMsgList[ucRace]->Save();
    WritePrivateProfileStringA("RaceBossSMSSave", "Flag", "TRUE", "..\\SystemSave\\ServerState.ini");
  }
}

bool RACE_BOSS_MSG::CMsgList::Load(unsigned int dwCurTime)
{
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

char CRaceBossMsgController::Send(
  unsigned __int8 ucRace,
  unsigned int dwSerial,
  const char *wszName,
  const char *pwszMsg,
  unsigned int dwWebSendDBID)
{
  if (!g_Main.m_bConnectedWebAgentServer)
  {
    return 0;
  }
  if (!dwSerial)
  {
    SendWebRaceBossSMSErrorResult(2, dwWebSendDBID);
    return 0;
  }

  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  if (ranking->GetCurrentRaceBossSerial(ucRace, 0) == dwSerial)
  {
    RACE_BOSS_MSG::CMsg *msg = nullptr;
    const int ret = m_kManager.Send(ucRace, dwSerial, wszName, pwszMsg, &msg, dwWebSendDBID);
    if (ret)
    {
      SendWebRaceBossSMSErrorResult(static_cast<char>(ret), dwWebSendDBID);
      return 0;
    }

    SendComfirmWeb(ucRace, msg);
    SendConfirmCtrl(ucRace, msg);
    return 1;
  }

  SendWebRaceBossSMSErrorResult(3, dwWebSendDBID);
  return 0;
}

char CRaceBossMsgController::Send(CPlayer *pkSender, const char *pwszMsg)
{
  if (!pkSender)
  {
    return 0;
  }

  if (g_Main.m_bConnectedWebAgentServer)
  {
    const unsigned __int8 raceCode = pkSender->m_Param.GetRaceCode();
    const unsigned int serial = pkSender->m_Param.GetCharSerial();
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    if (ranking->GetCurrentRaceBossSerial(raceCode, 0) == serial)
    {
      RACE_BOSS_MSG::CMsg *msg = nullptr;
      const char *name = pkSender->m_Param.GetCharNameW();
      const int ret = m_kManager.Send(raceCode, serial, name, pwszMsg, &msg, 0);
      if (ret)
      {
        SendMsgRequestResult(pkSender->m_ObjID.m_wIndex, static_cast<char>(ret));
        return 0;
      }

      SendMsgRequestResult(pkSender->m_ObjID.m_wIndex, 0);
      SendComfirmWeb(raceCode, msg);
      SendConfirmCtrl(raceCode, msg);
      return 1;
    }

    SendMsgRequestResult(pkSender->m_ObjID.m_wIndex, 3);
    return 0;
  }

  SendMsgRequestResult(pkSender->m_ObjID.m_wIndex, 1);
  return 0;
}

void CRaceBossMsgController::SendMsgRequestResult(unsigned __int16 usInx, char ucRet)
{
  char msg[32]{};
  unsigned __int8 type[2]{52, 2};

  msg[0] = ucRet;
  g_Network.m_pProcess[0]->LoadSendMsg(usInx, type, msg, 1u);
}

void CRaceBossMsgController::SendWebRaceBossSMSErrorResult(char iRet, unsigned int dwWebDBID)
{
  _racebosssms_fromweb_send_error_result_zowb msg{};
  msg.byErrCode = static_cast<unsigned __int8>(iRet + 10);
  msg.dwWebSendDBID = dwWebDBID;

  unsigned __int8 type[2] = {51, 13};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      type,
      reinterpret_cast<char *>(&msg),
      msg.size());
  }
}

void CRaceBossMsgController::SendComfirmWeb(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg)
{
  if (!g_Main.m_bWorldOpen || !g_Main.m_bWorldService)
  {
    return;
  }

  _raceboss_msg_confirm_zowb msg{};
  msg.nCountIndex = static_cast<int>(pkMsg->GetID());
  msg.nWorldCode = static_cast<int>(g_Main.m_byWorldCode);
  msg.byRaceCode = ucRace;
  strncpy(msg.wszMasterName, pkMsg->GetBossName(), 0x10u);
  msg.wszMasterName[16] = 0;
  strncpy(msg.wszMsg, pkMsg->GetMsg(), 0x30u);
  msg.wszMsg[48] = 0;

  unsigned __int8 type[2] = {51, 9};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byWebAgentServerNetInx,
      type,
      reinterpret_cast<char *>(&msg),
      msg.size());
  }
}

void CRaceBossMsgController::SendConfirmCtrl(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg)
{
  if (!g_Main.m_bWorldOpen || !g_Main.m_bWorldService)
  {
    return;
  }

  _raceboss_msg_confirm_zowb msg{};
  msg.nCountIndex = static_cast<int>(pkMsg->GetID());
  msg.nWorldCode = static_cast<int>(g_Main.m_byWorldCode);
  msg.byRaceCode = ucRace;
  strncpy(msg.wszMasterName, pkMsg->GetBossName(), 0x10u);
  msg.wszMasterName[16] = 0;
  strncpy(msg.wszMsg, pkMsg->GetMsg(), 0x30u);
  msg.wszMsg[48] = 0;

  unsigned __int8 type[2] = {54, 2};
  if (g_Main.m_bConnectedControllServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(
      g_Main.m_byControllServerNetInx,
      type,
      reinterpret_cast<char *>(&msg),
      msg.size());
  }
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

void CRaceBossMsgController::OnLoop()
{
  if (!m_pkTimer)
  {
    return;
  }

  if (g_Main.m_bConnectedWebAgentServer)
  {
    UpdateSend();
  }

  if (m_pkTimer->CountingTimer())
  {
    SaveCurTime();
    if (IsDayChanged())
    {
      m_kManager.Refresh();
      m_kManager.Save();
    }
  }
}

void CRaceBossMsgController::UpdateSend()
{
  unsigned __int8 raceCode = 0;
  RACE_BOSS_MSG::CMsg *msg = m_kManager.GetSendMsg(&raceCode);
  if (!msg)
  {
    return;
  }

  if (!msg->IsSendFromWeb())
  {
    const unsigned __int8 remainCount = m_kManager.GetRemainCnt(raceCode);
    SendInfomSender(msg->GetSerial(), remainCount);
  }

  SendRequestWeb(raceCode, msg);
  m_kManager.Release(raceCode, msg);
  m_kManager.Save(raceCode);
  SaveCurTime();
}

void CRaceBossMsgController::SendInfomSender(unsigned int dwSerial, unsigned __int8 ucRemainCnt)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, dwSerial);
  if (!player || !player->m_bLive)
  {
    return;
  }

  char payload[1]{};
  payload[0] = static_cast<char>(ucRemainCnt);
  unsigned __int8 type[2]{52, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, payload, 1u);
}

void CRaceBossMsgController::SendRequestWeb(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg)
{
  if (!g_Main.m_bWorldOpen || !g_Main.m_bWorldService)
  {
    return;
  }

  if (pkMsg->IsSendFromWeb())
  {

    _send_fromweb_raceboss_msg_request_zowb payload{};
    payload.dwWebSendDBID = pkMsg->GetWebDBID();
    payload.nCountIndex = pkMsg->GetID();
    payload.nWorldCode = static_cast<int>(g_Main.m_byWorldCode);
    payload.byRaceCode = ucRace;

    unsigned __int8 type[2]{51, 14};
    if (g_Main.m_bConnectedWebAgentServer)
    {
      g_Network.m_pProcess[2]->LoadSendMsg(
        g_Main.m_byWebAgentServerNetInx,
        type,
        reinterpret_cast<char *>(&payload),
        sizeof(payload));
    }
    return;
  }

  unsigned __int8 type[2]{51, 10};
  char payload[9]{};
  *reinterpret_cast<unsigned int *>(payload) = pkMsg->GetID();
  *reinterpret_cast<int *>(payload + 4) = static_cast<int>(g_Main.m_byWorldCode);
  payload[8] = static_cast<char>(ucRace);

  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(g_Main.m_byWebAgentServerNetInx, type, payload, 9u);
  }
}

bool CRaceBossMsgController::IsDayChanged()
{
  const int curDay = GetCurDay();
  if (curDay < 0)
  {
    return false;
  }
  if (curDay == m_iOldDay)
  {
    return false;
  }

  m_iOldDay = curDay;
  return true;
}

void CRaceBossMsgController::SaveCurTime()
{
  char buffer[272]{};
  const DWORD now = timeGetTime();
  sprintf(buffer, "%d", now);
  if (!WritePrivateProfileStringA("RaceBossSMSCurTime", "Time", buffer, "..\\SystemSave\\ServerState.ini"))
  {
    WritePrivateProfileStringA("RaceBossSMSCurTime", "Time", buffer, "..\\SystemSave\\ServerState.ini");
  }
}

char CRaceBossMsgController::Cancel(unsigned __int8 ucRace, unsigned int dwMsgID)
{
  RACE_BOSS_MSG::CMsg *pkMsg = nullptr;
  if (m_kManager.Cancel(ucRace, dwMsgID, &pkMsg))
  {
    return 0;
  }

  const unsigned int serial = pkMsg->GetSerial();
  SendCancleInfomSender(serial);
  SendCancelWeb(ucRace, pkMsg);
  m_kManager.CleanUpCancel(ucRace, pkMsg);
  return 1;
}

char CRaceBossMsgController::Cancel(unsigned __int8 ucRace, unsigned int dwMsgID, CPlayer *pkManager)
{
  if (pkManager->m_byUserDgr != 2 || pkManager->m_bySubDgr == 3)
  {
    return 0;
  }

  RACE_BOSS_MSG::CMsg *pkMsg = nullptr;
  const int result = m_kManager.Cancel(ucRace, dwMsgID, &pkMsg);
  if (result)
  {
    SendCancleInfomManager(pkManager->m_ObjID.m_wIndex, result, 0xFFFFFFFFu, nullptr);
    return 0;
  }

  const unsigned int serial = pkMsg->GetSerial();
  SendCancleInfomSender(serial);
  const char *bossName = pkMsg->GetBossName();
  SendCancleInfomManager(pkManager->m_ObjID.m_wIndex, 0, dwMsgID, bossName);
  SendCancelWeb(ucRace, pkMsg);
  m_kManager.CleanUpCancel(ucRace, pkMsg);
  return 1;
}

void CRaceBossMsgController::SendCancleInfomSender(unsigned int dwSerial)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, dwSerial);
  if (player && player->m_bLive)
  {
    unsigned __int8 pbyType[2] = {52, 3};
    char szMsg[1]{};
    g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
  }
}

void CRaceBossMsgController::SendCancleInfomManager(
  unsigned __int16 usInx,
  char ucRet,
  unsigned int dwMsgID,
  const char *pwszName)
{
char szMsg[0x16]{};
  szMsg[0] = ucRet;
  if (pwszName)
  {
    strncpy_s(szMsg + 1, 0x10u, pwszName, 0x10u);
    szMsg[17] = '\0';
  }

  unsigned __int8 pbyType[2] = {52, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(usInx, pbyType, szMsg, 0x16u);
}

void CRaceBossMsgController::SendCancelWeb(unsigned __int8 ucRace, RACE_BOSS_MSG::CMsg *pkMsg)
{
  if (!g_Main.m_bWorldOpen || !g_Main.m_bWorldService)
  {
    return;
  }

  char szMsg[9]{};
  *reinterpret_cast<unsigned int *>(szMsg) = pkMsg->GetID();
  *reinterpret_cast<int *>(szMsg + 4) = g_Main.m_byWorldCode;
  szMsg[8] = static_cast<char>(ucRace);

  unsigned __int8 pbyType[2] = {51, 11};
  if (g_Main.m_bConnectedWebAgentServer)
  {
    g_Network.m_pProcess[2]->LoadSendMsg(g_Main.m_byWebAgentServerNetInx, pbyType, szMsg, 9u);
  }
}
