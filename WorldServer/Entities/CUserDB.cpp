#include "pch.h"

#include "CUserDB.h"

#include <cstring>
#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <mmsystem.h>

#include "CMapOperation.h"
#include "CNationSettingManager.h"
#include "CRecordData.h"
#include "class_fld.h"
#include "FireGuard.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "CLogFile.h"
#include "CNetProcess.h"
#include "CNetworkEX.h"
#include "add_char_result_zone.h"
#include "alive_char_result_zocl.h"
#include "Packet/ClientZonePacket.h"
#include "enter_world_result_zone.h"
#include "chat_lock_inform_zocl.h"
#include "del_char_result_zone.h"
#include "moveout_user_result_zone.h"
#include "not_arranged_char_inform_zocl.h"
#include "qry_case_alive_char.h"
#include "qry_case_contsave.h"
#include "qry_sheet_delete.h"
#include "qry_sheet_insert.h"
#include "qry_sheet_load.h"
#include "qry_sheet_lobby.h"
#include "qry_sheet_reged.h"
#include "reged_char_result_zone.h"
#include "sel_char_result_zone.h"
#include "server_notify_inform_zone.h"
#include "log_change_class_after_init_class.h"
#include "log_case_charselect.h"
#include "qry_logout.h"

bool CUserDB::IsValidBaseShape(unsigned int baseShape)
{
  for (int i = 2; i < 8; ++i)
  {
    const unsigned int nibble = (baseShape << (i * 4)) >> 28;
    if (nibble == 0xF)
    {
      return false;
    }
  }
  return true;
}
#include "exit_alter_param.h"
#include "RFEvent_ClassRefine.h"
#include "CNetIndexList.h"
#include "Packet/AccountZonePacket.h"
#include "Packet/ZoneAccountPacket.h"
#include "Packet/ZoneClientPacket.h"

int CUserDB::s_nLoginNum;
CLogFile CUserDB::s_logAvatorDB;
CMgrAccountLobbyHistory CUserDB::s_MgrLobbyHistory;
_MOVE_LOBBY_DELAY CUserDB::s_MoveLobbyDelay;

namespace
{
// Non-IDA parity: track close-time logout completions separately from generic DB wait state.
volatile LONG g_shutdownLogoutPending[MAX_PLAYER]{};

#ifdef CLIENTDEBUGCODE
constexpr unsigned int kClientDebugAccountSerial = 2000000000u;

void InitializeClientDebugWaitAccount(_WAIT_ENTER_ACCOUNT &waitData, unsigned int *masterKey)
{
  std::memset(&waitData, 0, sizeof(waitData));

  char accountId[13] = "!teste";
  _GLBID globalId{};
  globalId.dwIndex = 1;
  globalId.dwSerial = 1;

  unsigned int acceptedKey[4]{};
  if (masterKey)
  {
    std::memcpy(acceptedKey, masterKey, sizeof(acceptedKey));
  }

  waitData.SetData(kClientDebugAccountSerial, accountId, 4, 4, &globalId, acceptedKey, false);

  char cms[7]{};
  _SYSTEMTIME endDate{};
  endDate.wYear = 2099;
  endDate.wMonth = 12;
  endDate.wDay = 31;
  waitData.SetBillingInfo(8, cms, 255, &endDate);
  waitData.SetTransFlag(0);
  waitData.SetAgeLimitFlag(false);
  waitData.SetPcBangFlag(true);
}
#endif

void BeginShutdownLogoutTracking(const CUserDB &user)
{
  if (!g_Main.IsServerClosing())
  {
    return;
  }

  const unsigned int userIndex = user.m_idWorld.wIndex;
  if (userIndex >= MAX_PLAYER)
  {
    return;
  }

  if (InterlockedCompareExchange(&g_shutdownLogoutPending[userIndex], 1, 0) == 0)
  {
    InterlockedIncrement(&g_Main.m_lShutdownPendingLogoutCount);
  }
}

void CompleteShutdownLogoutTracking(const CUserDB &user)
{
  const unsigned int userIndex = user.m_idWorld.wIndex;
  if (userIndex >= MAX_PLAYER)
  {
    return;
  }

  if (InterlockedCompareExchange(&g_shutdownLogoutPending[userIndex], 0, 1) == 1)
  {
    InterlockedDecrement(&g_Main.m_lShutdownPendingLogoutCount);
  }
}
}

const char wszNonMakeName_0[3][17] = {"GM", "ADMIN", "OPERATOR"};
const char wszNonMakeName_1[3][17] = {"GM", "ADMIN", "OPERATOR"};
const char wszNonMakeName[3][17] = {"GM", "ADMIN", "OPERATOR"};
const char wszGMCmp[] = "[GameMaster]";
size_t nGMCmpLen = std::strlen(wszGMCmp);

CUserDB::CUserDB()
{
  m_dwAccountSerial = static_cast<unsigned int>(-1);
  m_AvatorData.InitData();
  std::memset(m_dwCanonicalRegedLastConnTime, 0, sizeof(m_dwCanonicalRegedLastConnTime));
  m_dwCanonicalLastConnTime = 0;
  m_dwCanonicalBackupLastConnTime = 0;
  std::memset(m_dwCanonicalUnitCutTime, 0, sizeof(m_dwCanonicalUnitCutTime));
  std::memset(m_dwCanonicalBackupUnitCutTime, 0, sizeof(m_dwCanonicalBackupUnitCutTime));
  m_bActive = false;
  m_bField = false;
  m_bChatLock = false;
  m_bNoneUpdateData = false;
  m_dwSerial = static_cast<unsigned int>(-1);
  m_idWorld.dwSerial = static_cast<unsigned int>(-1);
  m_gidGlobal.dwSerial = static_cast<unsigned int>(-1);
  m_bCreateTrunkFree = false;
  if (!CUserDB::s_logAvatorDB.m_bInit)
  {
    const unsigned int localTime = static_cast<unsigned int>(GetKorLocalTime());
    char buffer[80]{};
    sprintf_s(buffer, "..\\ZoneServerLog\\Systemlog\\AvatorDBError%d.log", localTime);
    CUserDB::s_logAvatorDB.SetWriteLogFile(buffer, 1, false, true, true);
  }
  m_AvatorData_bk.InitData();
}

CUserDB::~CUserDB()
{
}

void CUserDB::SetRegedCanonicalLastConnTime(unsigned __int8 bySlotIndex, unsigned __int64 dwLastConnTime)
{
  if (bySlotIndex >= 3u)
  {
    return;
  }

  m_dwCanonicalRegedLastConnTime[bySlotIndex] = dwLastConnTime;
  m_RegedList[bySlotIndex].m_dwLastConnTime = ClampLegacyLastConnTime(dwLastConnTime);
}

unsigned __int64 CUserDB::GetRegedCanonicalLastConnTime(unsigned __int8 bySlotIndex) const
{
  if (bySlotIndex >= 3u)
  {
    return 0;
  }

  return m_dwCanonicalRegedLastConnTime[bySlotIndex];
}

void CUserDB::SetCanonicalLastConnTime(unsigned __int64 dwLastConnTime)
{
  m_dwCanonicalLastConnTime = dwLastConnTime;
  m_AvatorData.dbAvator.m_dwLastConnTime = ClampLegacyLastConnTime(dwLastConnTime);
}

unsigned __int64 CUserDB::GetCanonicalLastConnTime() const
{
  return m_dwCanonicalLastConnTime;
}

void CUserDB::SetCanonicalBackupLastConnTime(unsigned __int64 dwLastConnTime)
{
  m_dwCanonicalBackupLastConnTime = dwLastConnTime;
  m_AvatorData_bk.dbAvator.m_dwLastConnTime = ClampLegacyLastConnTime(dwLastConnTime);
}

unsigned __int64 CUserDB::GetCanonicalBackupLastConnTime() const
{
  return m_dwCanonicalBackupLastConnTime;
}

void CUserDB::SyncCanonicalBackupLastConnTime()
{
  SetCanonicalBackupLastConnTime(m_dwCanonicalLastConnTime);
}

void CUserDB::SetCanonicalUnitCutTime(unsigned __int8 bySlotIndex, unsigned __int64 dwCutTime)
{
  if (bySlotIndex >= 4u)
  {
    return;
  }

  m_dwCanonicalUnitCutTime[bySlotIndex] = dwCutTime;
  m_AvatorData.dbUnit.m_List[bySlotIndex].dwCutTime = ClampLegacyKorLocalTime(dwCutTime);
}

unsigned __int64 CUserDB::GetCanonicalUnitCutTime(unsigned __int8 bySlotIndex) const
{
  if (bySlotIndex >= 4u)
  {
    return 0;
  }

  return m_dwCanonicalUnitCutTime[bySlotIndex];
}

void CUserDB::SetCanonicalBackupUnitCutTime(unsigned __int8 bySlotIndex, unsigned __int64 dwCutTime)
{
  if (bySlotIndex >= 4u)
  {
    return;
  }

  m_dwCanonicalBackupUnitCutTime[bySlotIndex] = dwCutTime;
  m_AvatorData_bk.dbUnit.m_List[bySlotIndex].dwCutTime = ClampLegacyKorLocalTime(dwCutTime);
}

unsigned __int64 CUserDB::GetCanonicalBackupUnitCutTime(unsigned __int8 bySlotIndex) const
{
  if (bySlotIndex >= 4u)
  {
    return 0;
  }

  return m_dwCanonicalBackupUnitCutTime[bySlotIndex];
}

void CUserDB::SyncCanonicalBackupUnitCutTime()
{
  for (unsigned __int8 slotIndex = 0; slotIndex < 4u; ++slotIndex)
  {
    SetCanonicalBackupUnitCutTime(slotIndex, m_dwCanonicalUnitCutTime[slotIndex]);
  }
}

CUserDB *SearchAvatorWithName(CUserDB *pList, int nMax, char *pwszName)
{
  const unsigned __int8 nameLen = static_cast<unsigned __int8>(std::strlen(pwszName));
  for (int index = 0; index < nMax; ++index)
  {
    CUserDB &entry = pList[index];
    if (entry.m_bActive
        && entry.m_bField
        && entry.m_byNameLen == nameLen
        && !std::strncmp(entry.m_wszAvatorName, pwszName, nameLen))
    {
      return &entry;
    }
  }
  return nullptr;
}

void CUserDB::Init(unsigned __int16 index)
{
  m_idWorld.wIndex = index;
  ParamInit();
  m_tmrCheckPlayMin.BeginTimer(60000);
}

void CUserDB::DummyCreate(unsigned int dwSerial)
{
  Init(0);
  m_bActive = true;
  m_dwSerial = dwSerial;
}

void CUserDB::ParamInit()
{
  m_idWorld.dwSerial = static_cast<unsigned int>(-1);
  m_gidGlobal.dwSerial = static_cast<unsigned int>(-1);
  m_AvatorData.InitData();
  m_AvatorData_bk.InitData();
  std::memset(m_dwCanonicalRegedLastConnTime, 0, sizeof(m_dwCanonicalRegedLastConnTime));
  m_dwCanonicalLastConnTime = 0;
  m_dwCanonicalBackupLastConnTime = 0;
  std::memset(m_dwCanonicalUnitCutTime, 0, sizeof(m_dwCanonicalUnitCutTime));
  std::memset(m_dwCanonicalBackupUnitCutTime, 0, sizeof(m_dwCanonicalBackupUnitCutTime));
  for (int j = 0; j < 3; ++j)
  {
    m_RegedList[j].init();
  }
  m_bActive = false;
  m_dwSerial = static_cast<unsigned int>(-1);
  m_bDBWaitState = false;
  m_pDBPushData = nullptr;
  m_bChatLock = false;
  m_bNoneUpdateData = false;
  m_byUserDgr = 0;
  m_bySubDgr = 0;
  m_ss.Init();
  m_dwTotalPlayMin = 0;
  m_bDataUpdate = false;
  m_dwLastContSaveTime = 0;
  m_RadarItemMgr.Init();
  m_byUILock = 255;
  m_byUILock_InitFailCnt = 255;
  m_byUILock_FailCnt = 255;
  m_szUILock_PW[0] = '\0';
  m_szAccount_PW[0] = '\0';
  m_byUILock_HintIndex = 255;
  m_uszUILock_HintAnswer[0] = '\0';
  m_byUILock_InitFindPassFailCount = 255;
  m_byUILockFindPassFailCount = 0;
  std::memset(m_dwRequestMoveCharacterSerialList, 0, sizeof(m_dwRequestMoveCharacterSerialList));
  std::memset(m_dwTournamentCharacterSerialList, 0, sizeof(m_dwTournamentCharacterSerialList));
}

bool CUserDB::Update_Map(unsigned __int8 map, float *pos)
{
  if (!g_MapOper.IsExistStdMapID(map))
  {
    return false;
  }

  this->m_AvatorData.dbAvator.m_byMapCode = map;
  this->m_AvatorData.dbAvator.m_fStartPos[0] = pos[0];
  this->m_AvatorData.dbAvator.m_fStartPos[1] = pos[1];
  this->m_AvatorData.dbAvator.m_fStartPos[2] = pos[2];
  return true;
}

bool CUserDB::Update_Bind(char *pszMapCode, char *pDummyCode, bool bUpdate)
{
  if (std::strlen(pszMapCode) > 11)
  {
    return false;
  }
  if (std::strlen(pDummyCode) > 11)
  {
    return false;
  }
  std::strcpy(m_AvatorData.dbAvator.m_szBindMapCode, pszMapCode);
  std::strcpy(m_AvatorData.dbAvator.m_szBindDummy, pDummyCode);
return true;
}

void CUserDB::Update_PvpPointLeak(long double dValue)
{
  m_AvatorData.dbSupplement.dPvpPointLeak = dValue;
}

void CUserDB::Update_LastAttBuff(bool bSet)
{
  m_AvatorData.dbSupplement.bLastAttBuff = bSet;
}

void CUserDB::Update_GuildEntryDelay(unsigned int dwAvartorSerial)
{
  (void)dwAvartorSerial;

  const std::time_t currentTime = std::time(nullptr);
  const unsigned __int64 currentUnixTime = currentTime > 0 ? static_cast<unsigned __int64>(currentTime) : 0;
  m_AvatorData.dbSupplement.dwGuildEntryDelay = g_Main.m_dwGuildEntryDelay + currentUnixTime;
}

void CUserDB::Update_PlayerInteg(unsigned __int8 byPlayerInteg)
{
  m_AvatorData.dbSupplement.byPlayerInteg = byPlayerInteg;
}

void CUserDB::Update_TrunkInteg(unsigned __int8 byTrunkInteg)
{
  m_AvatorData.dbTrunk.byTrunkInteg = byTrunkInteg;
  m_bDataUpdate = true;
}

bool CUserDB::Update_Stat(unsigned __int8 byStatIndex, unsigned int dwNewCum, bool bUpdate)
{
if (byStatIndex < 80)
  {
    if (m_AvatorData.dbStat.m_dwDamWpCnt[byStatIndex] <= dwNewCum || m_byUserDgr)
    {
      m_AvatorData.dbStat.m_dwDamWpCnt[byStatIndex] = dwNewCum;
      m_bDataUpdate = true;
      return true;
    }

    g_Main.m_logSystemError.Write(
      "%s:Update_Stat(Idx:%d)..%d >= %d",
      m_aszAvatorName,
      byStatIndex,
      m_AvatorData.dbStat.m_dwDamWpCnt[byStatIndex],
      dwNewCum);
    return false;
  }

  g_Main.m_logSystemError.Write("%s:Update_Stat(Idx:%d)", m_aszAvatorName, byStatIndex);
  return false;
}

bool CUserDB::Update_QuestInsert(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData)
{
  if (bySlotIndex < 30)
  {
    if (m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 255)
    {
      std::memcpy(
        &m_AvatorData.dbQuest.m_List[bySlotIndex],
        pSlotData,
        sizeof(m_AvatorData.dbQuest.m_List[bySlotIndex]));
      m_bDataUpdate = true;
      return true;
    }

    g_Main.m_logSystemError.Write(
      "%s : Update_QuestInsert(EXIST) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_QuestInsert(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_QuestDelete(unsigned __int8 bySlotIndex)
{
  if (bySlotIndex < 30)
  {
    if (m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 255)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_QuestDelete(EXIST) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return false;
    }

    m_AvatorData.dbQuest.m_List[bySlotIndex].Init();
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_QuestDelete(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_QuestUpdate(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData, bool bUpdate)
{
  if (bySlotIndex < 30)
  {
    if (m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 255)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_QuestUpdate(NOTHING) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return false;
    }

    std::memcpy(
      &m_AvatorData.dbQuest.m_List[bySlotIndex],
      pSlotData,
      sizeof(m_AvatorData.dbQuest.m_List[bySlotIndex]));
    if (bUpdate)
    {
      m_bDataUpdate = true;
    }
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_QuestUpdate(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_CuttingEmpty()
{
  m_AvatorData.dbCutting.Init();
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_CuttingPush(unsigned __int8 resnum, _CUTTING_DB_BASE::_LIST *plist)
{
  if (resnum > 20u)
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_CuttingPush(CODE) byResNum (%d) => failed ",
      m_aszAvatorName,
      resnum);
    return false;
  }

  for (unsigned __int8 index = 0; index < resnum; ++index)
  {
    if (plist[index].Key.wItemIndex >= GetMaxResKind())
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_CuttingPush(CODE) List[%d].byResIndex (%d) => failed ",
        m_aszAvatorName,
        index,
        plist[index].Key.wItemIndex);
      return false;
    }
  }

  m_AvatorData.dbCutting.Init();
  m_AvatorData.dbCutting.m_byLeftNum = resnum;
  std::memcpy(m_AvatorData.dbCutting.m_List, plist, sizeof(_CUTTING_DB_BASE::_LIST) * resnum);
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_CuttingTrans(unsigned __int16 wResItemIndex, unsigned __int16 wLeftAmt)
{
  if (wResItemIndex >= GetMaxResKind())
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_CuttingTrans(CODE) wResItemIndex (%d) => failed ",
      m_aszAvatorName,
      wResItemIndex);
    return false;
  }

  bool found = false;
  for (int index = 0; index < 20; ++index)
  {
    if (m_AvatorData.dbCutting.m_List[index].Key.wItemIndex == wResItemIndex)
    {
      m_AvatorData.dbCutting.m_List[index].dwDur = wLeftAmt;
      if (!wLeftAmt)
      {
        m_AvatorData.dbCutting.m_List[index].Init();
        --m_AvatorData.dbCutting.m_byLeftNum;
      }
      found = true;
      break;
    }
  }

  if (!found)
  {
    g_Main.m_logSystemError.Write("%s:Update_TransRes(Idx:%d)", m_aszAvatorName, wResItemIndex);
    return false;
  }

  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_NPCQuestHistory(unsigned __int8 byIndex, _QUEST_DB_BASE::_NPC_QUEST_HISTORY *pHisData)
{
  if (byIndex < 70)
  {
    std::memcpy(
      &m_AvatorData.dbQuest.m_History[byIndex],
      pHisData,
      sizeof(m_AvatorData.dbQuest.m_History[byIndex]));
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_NPCQuestHistory(Index OVER) : %d",
    m_aszAvatorName,
    byIndex);
  return false;
}

bool CUserDB::Update_StartNPCQuestHistory(unsigned __int8 byIndex, _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY *pHisData)
{
  if (byIndex >= g_Main.m_dwStartNPCQuestCnt[m_AvatorData.dbAvator.m_byRaceSexCode / 2])
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_NPCQuestHistory(Index OVER) : %d",
      m_aszAvatorName,
      byIndex);
    return false;
  }

  std::strcpy(m_AvatorData.dbQuest.m_StartHistory[byIndex].szQuestCode, pHisData->szQuestCode);
  m_AvatorData.dbQuest.m_StartHistory[byIndex].byLevel = pHisData->byLevel;
  GetLocalTime(&m_AvatorData.dbQuest.m_StartHistory[byIndex].tmStartTime);
  m_AvatorData.dbQuest.m_StartHistory[byIndex].nEndTime = pHisData->nEndTime;
  ++m_AvatorData.dbQuest.dwListCnt;
  return true;
}

void CUserDB::Update_MaxLevel(unsigned __int8 byMaxLevel)
{
  m_AvatorData.dbAvator.m_byMaxLevel = byMaxLevel;
}

void CUserDB::ForceCloseCommand(unsigned __int8 byKickType, unsigned int dwPushIP, bool bSlow, const char *pszCause)
{
  _server_notify_inform_zone msg{};
  msg.wMsgCode = byKickType;
  msg.dwPushIP = dwPushIP;

  unsigned __int8 type[2] = {1, 16};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&msg), len);

  char buffer[144] = {};
  sprintf_s(buffer, "CLOSE>> ForceCloseCommand Type: %d, ID: %s Reason : ", byKickType, m_szAccountID);
  if (pszCause)
  {
    std::strcat(buffer, pszCause);
  }
  g_Network.Close(0, m_idWorld.wIndex, bSlow, buffer);
}

void CUserDB::Inform_For_Exit_By_FireguardBlock()
{

  _fireguard_block_request_wrac request{};
  strcpy_s(request.szAccountID, sizeof(request.szAccountID), m_szAccountID);
  request.dwAccountSerial = m_dwAccountSerial;
  request.dwIP = m_dwIP;

  unsigned __int8 pbyType[2]{1, 22};
  g_Network.m_pProcess[1]->LoadSendMsg(
    0,
    pbyType,
    reinterpret_cast<char *>(&request),
    static_cast<unsigned __int16>(sizeof(request)));
}

void CUserDB::ClearBillingData()
{
  std::memset(&m_BillingInfo, 0, sizeof(m_BillingInfo));
}

void CUserDB::SetBillingData(_BILLING_INFO *pBillingInfo)
{
  if (pBillingInfo)
  {
    std::memcpy(&m_BillingInfo, pBillingInfo, sizeof(m_BillingInfo));
  }
  m_BillingInfo.bPCCheat = 0;
}

void CUserDB::SetBillingData(char *szCMSCode, __int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate)
{
  m_BillingInfo.iType = iType;
  m_BillingInfo.lRemainTime = lRemainTime;
  if (iType == 6 || iType == 7)
  {
    std::memcpy(m_BillingInfo.szCMS, szCMSCode, sizeof(m_BillingInfo.szCMS));
  }
  else
  {
    std::memset(m_BillingInfo.szCMS, 0, sizeof(m_BillingInfo.szCMS));
  }
  if (pstEndDate)
  {
    std::memcpy(&m_BillingInfo.stEndDate, pstEndDate, sizeof(m_BillingInfo.stEndDate));
  }
  if (!g_Main.IsReleaseServiceMode() && m_BillingInfo.bPCCheat)
  {
    m_BillingInfo.bIsPcBang = 1;
    m_BillingInfo.iType = 7;
  }
}

int CUserDB::GetBillingType()
{
  return m_BillingInfo.iType;
}

void CUserDB::SetBillingNoLogout(bool bNoLogout)
{
  m_bBillingNoLogout = bNoLogout;
}

void CUserDB::SendMsg_BillingInfo()
{

  _current_billing_type_inform_zocl msg{};
  msg.iType = m_BillingInfo.iType;
  msg.byPaymentType = 0;
  msg.lRemainMin = m_BillingInfo.lRemainTime;
  std::memcpy(&msg.stEndDate, &m_BillingInfo.stEndDate, sizeof(msg.stEndDate));

  unsigned __int8 pbyType[2]{29, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_idWorld.wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CUserDB::SetRemainTime(int lRemainTime)
{
  m_BillingInfo.lRemainTime = lRemainTime;
}

void CUserDB::UILockInfo_Init(const _uilock_init_result_acwr *request)
{
  if (!request)
  {
    return;
  }

  _uilock_init_result_zocl msg{};
  if (m_byUILock)
  {
    msg.byRet = 11;
  }
  else if (request->byRet)
  {
    if (request->byRet == 1)
    {
      msg.byRet = 11;
    }
    else if (request->byRet == 2)
    {
      msg.byRet = 12;
    }
    else
    {
      msg.byRet = 15;
    }
  }
  else
  {
    msg.byRet = 0;
    msg.byUILock_HintIndex = request->byHintIndex;
    m_byUILock = 1;
    std::strcpy(m_szUILock_PW, request->uszUILockPW);
    m_byUILock_HintIndex = request->byHintIndex;
    std::strcpy(m_uszUILock_HintAnswer, request->uszHintAnswer);
  }

  unsigned __int8 pbyType[2]{13, 128};
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CUserDB::UILockInfo_Update(const _uilock_update_result_acwr *request)
{
  if (!request)
  {
    return;
  }

  _uilock_update_info_result_zocl msg{};
  if (m_byUILock >= 2u)
  {
    if (request->byRet)
    {
      if (request->byRet == 1)
      {
        msg.byRet = 10;
      }
      else if (request->byRet == 2)
      {
        msg.byRet = 12;
      }
      else
      {
        msg.byRet = 8;
      }
    }
    else
    {
      msg.byRet = 0;
      m_byUILock = 2;
      std::strcpy(m_szUILock_PW, request->uszUILockPW);
      m_byUILock_HintIndex = request->byHintIndex;
      std::strcpy(m_uszUILock_HintAnswer, request->uszHintAnswer);
    }
  }
  else
  {
    msg.byRet = 7;
  }

  unsigned __int8 pbyType[2]{13, static_cast<unsigned __int8>(-124)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, pbyType, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CUserDB::SetChatLock(bool bLock)
{
  m_bChatLock = bLock;
  if (bLock)
  {
    _chat_lock_inform_zocl msg{};
    msg.bLock = 1;
    unsigned __int8 pbyType[2]{2, 12};
    const unsigned __int16 len = msg.size();
    g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, pbyType, reinterpret_cast<char *>(&msg), len);
  }
}

bool CUserDB::Update_AlterPvPPoint(long double dNewPoint)
{
  m_AvatorData.dbAvator.m_dPvPPoint = dNewPoint;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_AlterPvPCashBag(long double dNewPoint)
{
  m_AvatorData.dbAvator.m_dPvPCashBag = dNewPoint;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_ItemAdd(
  unsigned __int8 storage,
  unsigned __int8 slot,
  const _STORAGE_LIST::_db_con *pItem,
  bool /*bUpdate*/)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_ItemAdd(CODE) : storage : %d, slot : %d",
      m_aszAvatorName,
      storage,
      slot);
    return false;
  }

  unsigned __int8 bySlotIndex = static_cast<unsigned __int8>(-1);
  unsigned __int8 byTableCode = static_cast<unsigned __int8>(-1);
  unsigned __int16 wItemIndex = static_cast<unsigned __int16>(-1);

  switch (storage)
  {
    case 0u:
    {
      auto *entry = &m_AvatorData.dbInven.m_List[slot];
      if (!entry->Set(pItem))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 1u:
      if (!m_AvatorData.dbAvator.Set(slot, pItem))
      {
        byTableCode = slot;
        wItemIndex = m_AvatorData.dbAvator.m_EquipKey[slot].zItemIndex;
      }
      break;
    case 2u:
    {
      auto *entry = &m_AvatorData.dbEquip.m_EmbellishList[slot];
      if (!entry->Set(pItem))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 3u:
    {
      auto *entry = &m_AvatorData.dbForce.m_List[slot];
      if (!entry->Set(pItem))
      {
        byTableCode = 15;
        wItemIndex = entry->Key.GetIndex();
      }
      break;
    }
    case 4u:
    {
      auto *entry = &m_AvatorData.dbAnimus.m_List[slot];
      if (!entry->Set(pItem))
      {
        byTableCode = 24;
        wItemIndex = entry->Key.byItemIndex;
      }
      break;
    }
    case 5u:
    {
      auto *entry = &m_AvatorData.dbTrunk.m_List[slot];
      if (!entry->Set(pItem, m_AvatorData.dbAvator.m_byRaceSexCode / 2))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 6u:
    {
      auto *entry = &m_AvatorData.dbPersonalAmineInven.m_List[slot];
      if (!entry->Set(pItem))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    case 7u:
    {
      auto *entry = &m_AvatorData.dbTrunk.m_ExtList[slot];
      if (!entry->Set(pItem, m_AvatorData.dbAvator.m_byRaceSexCode / 2))
      {
        bySlotIndex = entry->Key.bySlotIndex;
        byTableCode = entry->Key.byTableCode;
        wItemIndex = entry->Key.wItemIndex;
      }
      break;
    }
    default:
      break;
  }

  if (byTableCode == static_cast<unsigned __int8>(-1))
  {
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s:Update_Add(storage:%d, Idx:%d) CUR..(item:%d,%d,%d) ADD..(item:%d,%d)",
    m_aszAvatorName,
    storage,
    slot,
    bySlotIndex,
    byTableCode,
    wItemIndex,
    pItem->m_byTableCode,
    pItem->m_wItemIndex);
  return false;
}

bool CUserDB::Update_ItemDelete(unsigned __int8 storage, unsigned __int8 slot, bool /*bUpdate*/)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : UpdateItemDelete(CODE) : scode : %d, icode : %d",
      m_aszAvatorName,
      storage,
      slot);
    return false;
  }

  bool released = false;
  switch (storage)
  {
    case 0u:
      released = m_AvatorData.dbInven.m_List[slot].Release();
      break;
    case 1u:
      released = m_AvatorData.dbAvator.Release(slot);
      break;
    case 2u:
      released = m_AvatorData.dbEquip.m_EmbellishList[slot].Release();
      break;
    case 3u:
      released = m_AvatorData.dbForce.m_List[slot].Release();
      break;
    case 4u:
      released = m_AvatorData.dbAnimus.m_List[slot].Release();
      break;
    case 5u:
      released = m_AvatorData.dbTrunk.m_List[slot].Release();
      break;
    case 6u:
      released = m_AvatorData.dbPersonalAmineInven.m_List[slot].Release();
      break;
    case 7u:
      released = m_AvatorData.dbTrunk.m_ExtList[slot].Release();
      break;
    default:
      break;
  }

  if (released)
  {
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s:UpdateItemDelete(storage:%d, Idx:%d)",
    m_aszAvatorName,
    storage,
    slot);
  return false;
}

bool CUserDB::Update_ItemDur(unsigned __int8 storage, unsigned __int8 slot, unsigned __int64 amount, bool /*bUpdate*/)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_ItemDur(CODE) : scode : %d, icode : %d  ",
      m_aszAvatorName,
      storage,
      slot);
    return false;
  }

  if (storage)
  {
    switch (storage)
    {
      case 2u:
      {
        _EMBELLKEY *key = &m_AvatorData.dbEquip.m_EmbellishList[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_ItemDur(EMBELL, Idx:%d)", m_aszAvatorName, slot);
          return false;
        }
        m_AvatorData.dbEquip.m_EmbellishList[slot].wAmount = static_cast<unsigned __int16>(amount);
        break;
      }
      case 3u:
      {
        _FORCEKEY *key = &m_AvatorData.dbForce.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_ItemDur(FORCE, Idx:%d)", m_aszAvatorName, slot);
          return false;
        }
        key->SetStat(static_cast<unsigned int>(amount));
        break;
      }
      case 4u:
      {
        _ANIMUSKEY *key = &m_AvatorData.dbAnimus.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_ItemDur(ANIMUS, Idx:%d)", m_aszAvatorName, slot);
          return false;
        }
        m_AvatorData.dbAnimus.m_List[slot].dwExp = amount;
        break;
      }
      case 5u:
      {
        _INVENKEY *key = &m_AvatorData.dbTrunk.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_ItemDur(TRUNK, Idx:%d)", m_aszAvatorName, slot);
          return false;
        }
        m_AvatorData.dbTrunk.m_List[slot].dwDur = amount;
        break;
      }
      case 6u:
      {
        _INVENKEY *key = &m_AvatorData.dbPersonalAmineInven.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_ItemDur(TRUNK, Idx:%d)", m_aszAvatorName, slot);
          return false;
        }
        m_AvatorData.dbPersonalAmineInven.m_List[slot].dwDur = static_cast<unsigned int>(amount);
        break;
      }
      case 7u:
      {
        _INVENKEY *key = &m_AvatorData.dbTrunk.m_ExtList[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_ItemDur(EXT_TRUNK, Idx:%d)", m_aszAvatorName, slot);
          return false;
        }
        m_AvatorData.dbTrunk.m_ExtList[slot].dwDur = amount;
        break;
      }
      default:
        break;
    }
  }
  else
  {
    _INVENKEY *key = &m_AvatorData.dbInven.m_List[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_ItemDur(INVEN, Idx:%d)", m_aszAvatorName, slot);
      return false;
    }
    m_AvatorData.dbInven.m_List[slot].dwDur = amount;
  }

  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_Money(unsigned int dalant, unsigned int gold)
{
  m_AvatorData.dbAvator.m_dwDalant = dalant;
  m_AvatorData.dbAvator.m_dwGold = gold;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_SFContDelete(unsigned __int8 byContCode, unsigned __int8 bySlotIndex)
{
  if (byContCode < 2)
  {
    if (bySlotIndex < 8)
    {
      if (m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].IsFilled())
      {
        m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].Init();
        return true;
      }
      g_Main.m_logSystemError.Write(
        "%s : Update_SFContDelete(NOTHING) : code : %d, slot : %d",
        m_aszAvatorName,
        byContCode,
        bySlotIndex);
      return false;
    }
    g_Main.m_logSystemError.Write(
      "%s : Update_SFContDelete(SlotIndex OVER) : code : %dslot : %d",
      m_aszAvatorName,
      byContCode,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_SFContDelete(byContCode OVER) : code : %d, slot : %d",
    m_aszAvatorName,
    byContCode,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_SFContInsert(
  unsigned __int8 byContCode,
  unsigned __int8 bySlotIndex,
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  unsigned __int16 wDurSec)
{
  if (byContCode < 2)
  {
    if (bySlotIndex < 8)
    {
      if (m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].IsFilled())
      {
        g_Main.m_logSystemError.Write(
          "%s : Update_SFContInsert(EXIST) : code : %d, slot : %d",
          m_aszAvatorName,
          byContCode,
          bySlotIndex);
        return false;
      }
      m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].SetKey(
        0,
        byEffectCode,
        wEffectIndex,
        byLv,
        wDurSec);
      return true;
    }
    g_Main.m_logSystemError.Write(
      "%s : Update_SFContInsert(SlotIndex OVER) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_SFContInsert(byContCode OVER) : code : %d",
    m_aszAvatorName,
    byContCode);
  return false;
}

bool CUserDB::Update_SFContUpdate(unsigned __int8 byContCode, unsigned __int8 bySlotIndex, unsigned __int16 wTime)
{
  if (byContCode < 2)
  {
    if (bySlotIndex < 8)
    {
      if (m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].IsFilled())
      {
        m_AvatorData.dbSfcont.m_List[byContCode][bySlotIndex].SetLeftTime(wTime);
        return true;
      }
      g_Main.m_logSystemError.Write(
        "%s : Update_SFContUpdate(NOTHING) : code : %d, slot : %d",
        m_aszAvatorName,
        byContCode,
        bySlotIndex);
      return false;
    }
    g_Main.m_logSystemError.Write(
      "%s : Update_SFContUpdate(SlotIndex OVER) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_SFContUpdate(byContCode OVER) : code : %d",
    m_aszAvatorName,
    byContCode);
  return false;
}

bool CUserDB::Update_UnitInsert(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData)
{
  if (bySlotIndex < 4)
  {
    if (m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 255)
    {
      std::memcpy(&m_AvatorData.dbUnit.m_List[bySlotIndex], pData, sizeof(m_AvatorData.dbUnit.m_List[bySlotIndex]));
      m_dwCanonicalUnitCutTime[bySlotIndex] = pData->dwCutTime;
      m_bDataUpdate = true;
      return true;
    }

    g_Main.m_logSystemError.Write(
      "%s : Update_UnitInsert(EXIST) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return false;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_UnitInsert(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_UnitDelete(unsigned __int8 bySlotIndex)
{
  if (bySlotIndex < 4)
  {
    if (m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 255)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_UnitDelete(EXIST) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return false;
    }

    m_AvatorData.dbUnit.m_List[bySlotIndex].DelUnit();
    m_dwCanonicalUnitCutTime[bySlotIndex] = 0;
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_UnitDelete(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

bool CUserDB::Update_UnitData(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData)
{
  if (bySlotIndex < 4)
  {
    if (m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 255)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_UnitData(EXIST) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return false;
    }
    std::memcpy(&m_AvatorData.dbUnit.m_List[bySlotIndex], pData, sizeof(m_AvatorData.dbUnit.m_List[bySlotIndex]));
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_UnitData(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return false;
}

void _EQUIPKEY::SetRelease()
{
  zItemIndex = -1;
}

bool _EQUIPKEY::IsFilled()
{
  return zItemIndex != -1;
}

void _EQUIPKEY::LoadDBKey(__int16 key)
{
  zItemIndex = key;
}

__int16 _EQUIPKEY::CovDBKey()
{
  return zItemIndex;
}

_INVENKEY::_INVENKEY() = default;

_INVENKEY::_INVENKEY(unsigned __int8 byInSlotIndex, unsigned __int8 byInTableCode, unsigned __int16 wInItemIndex)
{
  bySlotIndex = byInSlotIndex;
  byTableCode = byInTableCode;
  wItemIndex = wInItemIndex;
}

void _INVENKEY::LoadDBKey(int pl_nKey)
{
  *reinterpret_cast<int *>(this) = pl_nKey;
}

int _INVENKEY::CovDBKey() const
{
  return *reinterpret_cast<const int *>(this);
}

bool _INVENKEY::IsFilled() const
{
  return bySlotIndex != 255 || byTableCode != 255 || wItemIndex != 65535;
}

void _INVENKEY::SetRelease()
{
  bySlotIndex = 255;
  byTableCode = 255;
  wItemIndex = 65535;
}

bool _EMBELLKEY::IsFilled()
{
  return bySlotIndex != 255 || byTableCode != 255 || wItemIndex != 65535;
}

void _EMBELLKEY::SetRelease()
{
  bySlotIndex = 255;
  byTableCode = 255;
  wItemIndex = 65535;
}

void _EMBELLKEY::LoadDBKey(_EMBELLKEY key)
{
  *this = key;
}

int _EMBELLKEY::CovDBKey()
{
  const unsigned int packed =
    static_cast<unsigned int>(bySlotIndex)
    | (static_cast<unsigned int>(byTableCode) << 8)
    | (static_cast<unsigned int>(wItemIndex) << 16);
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<__int64>(packed));
}

bool _FORCEKEY::IsFilled()
{
  return dwKey != static_cast<unsigned int>(-1);
}

void _FORCEKEY::SetRelease()
{
  dwKey = static_cast<unsigned int>(-1);
}

void _FORCEKEY::SetKey(unsigned __int8 byItemIndex, unsigned int dwStat)
{
  dwKey = (dwStat & 0xFFFFFFu) | (static_cast<unsigned int>(byItemIndex) << 24);
}

void _FORCEKEY::SetStat(unsigned int pl_dwStat)
{
  dwKey &= 0xFF000000u;
  dwKey |= pl_dwStat & 0xFFFFFFu;
}

unsigned __int8 _FORCEKEY::GetIndex()
{
  return static_cast<unsigned __int8>(dwKey >> 24);
}

unsigned int _FORCEKEY::GetStat()
{
  return dwKey & 0xFFFFFFu;
}

int _FORCEKEY::CovDBKey()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<__int64>(dwKey));
}

void _EQUIP_DB_BASE::_EMBELLISH_LIST::Init()
{
  Key.SetRelease();
  wAmount = 0;
  dwItemETSerial = 0;
  lnUID = 0;
  byCsMethod = 0;
  dwT = static_cast<unsigned int>(-1);
  dwLendRegdTime = static_cast<unsigned int>(-1);
}

void _EQUIP_DB_BASE::Init()
{
  for (int j = 0; j < 7; ++j)
  {
    m_EmbellishList[j].Init();
  }
}

void _FORCE_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwItemETSerial = 0;
  lnUID = 0;
  byCsMethod = 0;
  dwT = static_cast<unsigned int>(-1);
  m_dwLendRegdTime = static_cast<unsigned int>(-1);
}

void _FORCE_DB_BASE::Init()
{
  for (int j = 0; j < 88; ++j)
  {
    m_List[j].Init();
  }
}

bool _LINKKEY::IsFilled()
{
  return wEffectCode != 65535;
}

void _LINKKEY::SetRelease()
{
  wEffectCode = 65535;
}

void _LINKKEY::LoadDBKey(unsigned __int16 pl_zKey)
{
  wEffectCode = pl_zKey;
}

unsigned __int16 _LINKKEY::GetCode()
{
  return static_cast<unsigned __int16>(wEffectCode >> 12);
}

unsigned __int16 _LINKKEY::GetIndex()
{
  return static_cast<unsigned __int16>(wEffectCode & 0x0FFF);
}

void _LINKKEY::SetData(unsigned __int16 wCode, unsigned __int16 wIndex)
{
  wEffectCode = static_cast<unsigned __int16>(wIndex | (wCode << 12));
}

__int16 _LINKKEY::CovDBKey()
{
  return wEffectCode;
}

_LINK_DB_BASE::_LIST::_LIST()
{
  Init();
}

void _LINK_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
}

_LINK_DB_BASE::_LINK_DB_BASE()
{
  Init();
}

void _LINK_DB_BASE::Init()
{
  for (int j = 0; j < 50; ++j)
  {
    m_LinkList[j].Init();
  }
}

bool _ANIMUSKEY::IsFilled()
{
  return byItemIndex != 255;
}

void _ANIMUSKEY::SetRelease()
{
  byItemIndex = 255;
}

void _ANIMUSKEY::LoadDBKey(unsigned __int8 key)
{
  byItemIndex = key;
}

unsigned __int8 _ANIMUSKEY::CovDBKey()
{
  return byItemIndex;
}

_ANIMUS_DB_BASE::_LIST::_LIST()
{
  Init();
}

void _ANIMUS_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwExp = 0;
  dwParam = 0;
  dwItemETSerial = 0;
  lnUID = -1;
  byCsMethod = 0;
  dwT = static_cast<unsigned int>(-1);
  dwLendRegdTime = 0;
}

_ANIMUS_DB_BASE::_ANIMUS_DB_BASE()
{
  Init();
}

void _INVEN_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwDur = 0;
  dwUpt = 268435455;
  dwItemETSerial = 0;
  lnUID = 0;
  byCsMethod = 0;
  dwT = static_cast<unsigned int>(-1);
  dwLendRegdTime = static_cast<unsigned int>(-1);
}

void _INVEN_DB_BASE::Init()
{
  for (int j = 0; j < 100; ++j)
  {
    m_List[j].Init();
  }
}

void _ANIMUS_DB_BASE::Init()
{
  for (int j = 0; j < 4; ++j)
  {
    m_List[j].Init();
  }
}

void CUserDB::SetDBPostData(
  unsigned int n,
  unsigned int dwSerial,
  int nNumber,
  unsigned __int8 byState,
  int nKey,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  bool bUpdateIndex,
  unsigned __int64 lnUID)
{
  if (n < 50)
  {
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwPSSerial = dwSerial;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].nNumber = nNumber;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].byState = byState;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].nKey = nKey;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwDur = dwDur;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwUpt = dwUpt;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwGold = dwGold;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].bNew = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].lnUID = lnUID;
    if (bUpdateIndex)
    {
      m_AvatorData.dbPostData.dbPost.m_PostList[n].bUpdate = 1;
      m_AvatorData.dbPostData.dbPost.m_PostList[n].bUpdateIndex = 1;
      m_AvatorData.dbPostData.dbPost.m_bUpdate = 1;
      m_bDataUpdate = 1;
    }
    memcpy_s(
      &m_AvatorData_bk.dbPostData.dbPost.m_PostList[n],
      297,
      &m_AvatorData.dbPostData.dbPost.m_PostList[n],
      297);
  }
}

void CUserDB::SetNewDBPostData(
  unsigned int n,
  unsigned int dwSerial,
  int nNumber,
  unsigned __int8 byState,
  char *wszSendName,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  int nKey,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned __int64 lnUID)
{
  if (n < 50)
  {
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwPSSerial = dwSerial;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].nNumber = nNumber;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].byState = byState;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].nKey = nKey;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwDur = dwDur;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwUpt = dwUpt;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwGold = dwGold;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].lnUID = lnUID;
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszSendName, 17, wszSendName);
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszRecvName, 17, wszRecvName);
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszTitle, 21, wszTitle);
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszContent, 201, wszContent);
    m_AvatorData.dbPostData.dbPost.m_PostList[n].bNew = 1;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].bUpdate = 1;
    m_AvatorData.dbPostData.dbPost.m_bUpdate = 1;
    m_bDataUpdate = 1;
  }
}

bool CUserDB::Update_DelPost(unsigned int dwSerial, unsigned int nIndex)
{
  if (nIndex >= 50)
  {
    return false;
  }

  if (m_AvatorData.dbPostData.dbPost.m_PostList[nIndex].bNew)
  {
    return true;
  }

  _DELPOST_DB_BASE *delPost = &m_AvatorData.dbPostData.dbDelPost;
  if (delPost->m_nCum >= delPost->m_nMax)
  {
    return false;
  }

  delPost->m_List[delPost->m_nCum].dwDelSerial = dwSerial;
  delPost->m_List[delPost->m_nCum].nStorageIndex = static_cast<int>(nIndex);
  ++delPost->m_nCum;
  delPost->m_bUpdate = 1;
  m_bDataUpdate = 1;
  return true;
}

bool CUserDB::IsReturnPostUpdate()
{
  return m_AvatorData.dbPostData.dbRetPost.m_nCum < m_AvatorData.dbPostData.dbRetPost.m_nMax;
}

void CUserDB::Update_ReturnPost(unsigned int dwSerial)
{
  _RETURNPOST_DB_BASE *retPost = &m_AvatorData.dbPostData.dbRetPost;
  retPost->m_RetSerials[retPost->m_nCum] = dwSerial;
  ++retPost->m_nCum;
  retPost->m_bUpdate = 1;
  m_bDataUpdate = 1;
}

void CUserDB::DelPostData(unsigned int dwIndex)
{
  if (dwIndex < 50)
  {
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].dwPSSerial = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].nNumber = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].byState = static_cast<unsigned __int8>(-1);
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].nKey = -1;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].dwDur = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].dwUpt = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].dwGold = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].lnUID = 0;
    std::memset(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszSendName,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszSendName));
    std::memset(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszRecvName,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszRecvName));
    std::memset(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszTitle,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszTitle));
    std::memset(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszContent,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszContent));
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].bNew = 0;
    m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].bUpdate = 0;
  }
}

void _PCBANG_FAVOR_ITEM_DB_BASE::Init()
{
  for (int index = 0; index < 50; ++index)
  {
    lnUID[index] = static_cast<unsigned __int64>(-1);
  }
}

bool _PCBANG_FAVOR_ITEM_DB_BASE::InsertItem(_STORAGE_LIST::_db_con *Item)
{
  for (int index = 0; index < 50; ++index)
  {
    if (lnUID[index] == static_cast<unsigned __int64>(-1))
    {
      lnUID[index] = Item->m_lnUID;
      return true;
    }
  }
  return false;
}

bool _PCBANG_FAVOR_ITEM_DB_BASE::IsDeleteItem(_STORAGE_LIST::_db_con *Item)
{
  for (int index = 0; index < 50; ++index)
  {
    if (lnUID[index] != static_cast<unsigned __int64>(-1) && lnUID[index] == Item->m_lnUID)
    {
      return true;
    }
  }
  return false;
}

bool _INVEN_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  dwDur = pItem->m_dwDur;
  dwUpt = pItem->m_dwLv;
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _INVEN_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _EQUIP_DB_BASE::_EMBELLISH_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  wAmount = static_cast<unsigned __int16>(pItem->m_dwDur);
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _EQUIP_DB_BASE::_EMBELLISH_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _FORCE_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  if (pItem->m_wItemIndex > 255)
  {
    return false;
  }
  Key.SetKey(static_cast<unsigned __int8>(pItem->m_wItemIndex), static_cast<unsigned int>(pItem->m_dwDur));
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  m_dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _FORCE_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _ANIMUS_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.LoadDBKey(static_cast<unsigned __int8>(pItem->m_wItemIndex));
  dwExp = pItem->m_dwDur;
  dwParam = pItem->m_dwLv;
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _ANIMUS_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

bool _TRUNK_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem, unsigned __int8 byRaceCode)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  dwDur = pItem->m_dwDur;
  dwUpt = pItem->m_dwLv;
  byRace = byRaceCode;
  lnUID = pItem->m_lnUID;
  dwT = pItem->m_dwT;
  byCsMethod = pItem->m_byCsMethod;
  dwLendRegdTime = pItem->m_dwLendRegdTime;
  return true;
}

bool _TRUNK_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  lnUID = 0;
  dwT = static_cast<unsigned int>(-1);
  byCsMethod = 0;
  return true;
}

void _TRUNK_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwDur = 0;
  dwUpt = 268435455;
  byRace = 0;
  dwItemETSerial = 0;
  lnUID = 0;
  byCsMethod = 0;
  dwT = static_cast<unsigned int>(-1);
  dwLendRegdTime = static_cast<unsigned int>(-1);
}

void _TRUNK_DB_BASE::Init()
{
  bySlotNum = 0;
  for (int j = 0; j < 100; ++j)
  {
    m_List[j].Init();
  }

  byExtSlotNum = 0;
  for (int k = 0; k < 40; ++k)
  {
    m_ExtList[k].Init();
  }
}

void _TRADE_DB_BASE::_LIST::Clear()
{
  byState = 0;
  dwRegistSerial = 0;
  byInvenIndex = static_cast<unsigned __int8>(-1);
  dwPrice = 0;
  tStartTime = 0;
  bySellTurm = 0;
  dwBuyerSerial = static_cast<unsigned int>(-1);
  dwTax = 0;
  tResultTime = 0;
  wszBuyerName[0] = '\0';
  szBuyerAccount[0] = '\0';
}

void _TRADE_DB_BASE::Clear()
{
  for (int j = 0; j < 20; ++j)
  {
    m_List[j].Clear();
  }
}

void _TRADE_DB_BASE::Init()
{
  Clear();
}

_PERSONALAMINE_INVEN_DB_BASE::_LIST::_LIST()
{
  Init();
}

void _PERSONALAMINE_INVEN_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwDur = 0;
}

_PERSONALAMINE_INVEN_DB_BASE::_PERSONALAMINE_INVEN_DB_BASE()
{
  Init();
}

void _PERSONALAMINE_INVEN_DB_BASE::Init()
{
  bUsable = false;
  for (int j = 0; j < 40; ++j)
  {
    m_List[j].Init();
  }
}

void _CRYMSG_DB_BASE::_LIST::Init()
{
  wszCryMsg[0] = '\0';
}

void _CRYMSG_DB_BASE::Init()
{
  for (int j = 0; j < 10; ++j)
  {
    m_List[j].Init();
  }
}

void _PVPPOINT_LIMIT_DB_BASE::Init()
{
  tUpdatedate = 0;
  bUseUp = false;
  byLimitRate = 0;
  dOriginalPoint = 0.0;
  dLimitPoint = 0.0;
  dUsePoint = 0.0;
}

void _PVP_ORDER_VIEW_DB_BASE::Init()
{
  std::memset(this, 0, sizeof(_PVP_ORDER_VIEW_DB_BASE));
}

void _SUPPLEMENT_DB_BASE::Init()
{
  std::memset(this, 0, sizeof(_SUPPLEMENT_DB_BASE));
}

void _PCBANG_PLAY_TIME::Init()
{
  std::memset(this, 0, sizeof(_PCBANG_PLAY_TIME));
}

void _POTION_NEXT_USE_TIME_DB_BASE::Init()
{
  std::memset(this, 0, sizeof(_POTION_NEXT_USE_TIME_DB_BASE));
}

void _AVATOR_DB_BASE::Init()
{
  _REGED_AVATOR_DB::Init();

  for (int j = 0; j < 3; ++j)
  {
    m_zClassHistory[j] = static_cast<__int16>(-1);
  }

  m_dwPvpRank = 0;
  m_wRankRate = static_cast<unsigned __int16>(-1);
  m_dwGuildSerial = static_cast<unsigned int>(-1);
  m_dwGuildExplusDate = static_cast<unsigned int>(-1);
  m_bOverlapVote = true;
  m_dwGivebackCount = 0;
  m_dwRadarDelayTime = 0;
  m_dwTakeLastMentalTicket = 0;
  m_dwTakeLastCriTicket = 0;
  m_byMaxLevel = 0;

  for (int k = 0; k < 3; ++k)
  {
    m_dwPunishment[k] = static_cast<unsigned int>(-1);
    m_dwElectSerial[k] = 0;
  }

  for (int m = 0; m < 3; ++m)
  {
    m_dwRaceBattleRecord[m] = 0;
  }
}

bool _PERSONALAMINE_INVEN_DB_BASE::_LIST::Set(const _STORAGE_LIST::_db_con *pItem)
{
  if (Key.IsFilled())
  {
    return false;
  }
  Key.bySlotIndex = pItem->m_byStorageIndex;
  Key.byTableCode = pItem->m_byTableCode;
  Key.wItemIndex = pItem->m_wItemIndex;
  dwDur = static_cast<unsigned int>(pItem->m_dwDur);
  return true;
}

bool _PERSONALAMINE_INVEN_DB_BASE::_LIST::Release()
{
  if (!Key.IsFilled())
  {
    return false;
  }
  Key.SetRelease();
  return true;
}

bool _REGED::Set(unsigned __int8 bySlot, const _STORAGE_LIST::_db_con *pItem)
{
  if (m_EquipKey[bySlot].IsFilled())
  {
    return false;
  }
  m_EquipKey[bySlot].zItemIndex = pItem->m_wItemIndex;
  m_dwFixEquipLv[bySlot] = pItem->m_dwLv;
  m_lnUID[bySlot] = pItem->m_lnUID;
  m_dwET[bySlot] = pItem->m_dwT;
  m_byCsMethod[bySlot] = pItem->m_byCsMethod;
  m_dwLendRegdTime[bySlot] = pItem->m_dwLendRegdTime;
  return true;
}

bool _REGED::Release(unsigned __int8 bySlot)
{
  if (!m_EquipKey[bySlot].IsFilled())
  {
    return false;
  }
  m_EquipKey[bySlot].SetRelease();
  m_lnUID[bySlot] = 0;
  m_dwET[bySlot] = static_cast<unsigned int>(-1);
  m_byCsMethod[bySlot] = 0;
  return true;
}

_REGED_AVATOR_DB::_REGED_AVATOR_DB()
{
  Init();
}

void _REGED_AVATOR_DB::Init()
{
  std::memset(this, 0, sizeof(_REGED_AVATOR_DB));
  m_bySlotIndex = static_cast<unsigned __int8>(-1);
  m_dwRecordNum = static_cast<unsigned int>(-1);
  for (int j = 0; j < 8; ++j)
  {
    m_EquipKey[j].SetRelease();
  }
}

void _REGED::init()
{
  Init();
  for (int j = 0; j < 8; ++j)
  {
    m_dwFixEquipLv[j] = 268435455;
    m_dwItemETSerial[j] = 0;
    m_lnUID[j] = 0;
    m_dwET[j] = static_cast<unsigned int>(-1);
    m_byCsMethod[j] = 0;
    m_dwLendRegdTime[j] = static_cast<unsigned int>(-1);
  }
}

void _REGED::UpdateEquipLv()
{
  for (int j = 0; j < 8; ++j)
  {
    m_byEquipLv[j] = GetItemUpgedLv(m_dwFixEquipLv[j]);
  }
}

void _SYNC_STATE::Init()
{
  std::memset(this, 0, sizeof(_SYNC_STATE));
}

void _SYNC_STATE::re_lobby()
{
  bSelect = false;
  bReged = false;
}

bool _SYNC_STATE::chk_enter()
{
  if (bEnter)
  {
    return false;
  }
  bEnter = true;
  return true;
}

bool _SYNC_STATE::chk_reged()
{
  if (bReged)
  {
    return false;
  }
  bReged = true;
  return true;
}

bool _SYNC_STATE::chk_select()
{
  if (bSelect)
  {
    return false;
  }
  bSelect = true;
  return true;
}

_worlddb_sf_delay_info::_eff_list::_eff_list()
{
  byEffectCode = static_cast<unsigned __int8>(-1);
  wEffectIndex = 0;
  dwNextTime = 0;
}

_worlddb_sf_delay_info::_mas_list::_mas_list()
{
  byEffectCode = static_cast<unsigned __int8>(-1);
  byMastery = 0;
  dwNextTime = 0;
}

_worlddb_sf_delay_info::_worlddb_sf_delay_info()
{
  for (int index = 0; index < 10; ++index)
  {
    EFF[index].byEffectCode = static_cast<unsigned __int8>(-1);
    EFF[index].wEffectIndex = 0;
    EFF[index].dwNextTime = 0;
    MAS[index].byEffectCode = static_cast<unsigned __int8>(-1);
    MAS[index].byMastery = 0;
    MAS[index].dwNextTime = 0;
  }
}

_AVATOR_DATA::_AVATOR_DATA()
{
  InitData();
}

void _AVATOR_DATA::InitData()
{
  dbAvator.Init();
  dbLink.Init();
  dbEquip.Init();
  dbForce.Init();
  dbAnimus.Init();
  dbStat.Init();
  dbInven.Init();
  dbCutting.Init();
  dbCutting.ReSetOldDataLoad();
  dbQuest.Init();
  dbSfcont.Init();
  dbTrade.Init();
  dbBuddy.Init();
  dbTrunk.Init();
  dbItemCombineEx.Init();
  dbPersonalAmineInven.Init();
  dbPvpPointLimit.Init();
  dbPostData.Init();
  dbBossCry.Init();
  dbPvpOrderView.Init();
  dbSupplement.Init();
  dbPlayTimeInPcbang.Init();
  dbPotionNextUseTime.Init();
  dbPcBangFavorItem.Init();
  m_bCristalBattleDateUpdate = 1;
}

void CUserDB::SetWorldCLID(unsigned int dwSerial, unsigned int *pipAddr)
{
  m_idWorld.dwSerial = dwSerial;
  m_ipAddress = *pipAddr;
  m_ss.Init();
  const int index = m_idWorld.wIndex;
  CNationSettingManager::Instance()->OnConnectSession(index);
}

void CUserDB::SendMsgAccount_UILockRefresh_Update()
{
  if (m_byUILock != 255
      && !m_byUserDgr
      && (m_byUILock_InitFailCnt != m_byUILock_FailCnt
          || m_byUILock_InitFindPassFailCount != m_byUILockFindPassFailCount))
  {
    _uilock_user_refresh_info_request_wrac msg{};
    msg.dwAccountSerial = m_dwAccountSerial;
    msg.byFailCnt = m_byUILock_FailCnt;
    msg.byFindPassFailCount = m_byUILockFindPassFailCount;
    msg.gidGlobal = m_gidGlobal;

    unsigned __int8 type[2] = {1, 19};
    const unsigned __int16 len = msg.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&msg), len);
  }
}

bool CUserDB::Enter_Account(
  unsigned int dwAccountSerial,
  unsigned int dwIP,
  unsigned int dwProtocolVer,
  unsigned int *pdwMasterKey)
{
  if (m_bActive)
  {
    g_Main.m_logSystemError.Write( "%d.. Enter_Account : m_bActive == true", dwAccountSerial);
    return false;
  }
  if (!m_ss.chk_enter())
  {
    g_Main.m_logSystemError.Write( "%d.. Enter_Account : if(!m_ss.chk_enter())", dwAccountSerial);
    return false;
  }

#ifdef CLIENTDEBUGCODE
  _WAIT_ENTER_ACCOUNT clientDebugWaitData;
#endif
  _WAIT_ENTER_ACCOUNT *waitData = nullptr;
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    _WAIT_ENTER_ACCOUNT *entry = &g_Main.m_WaitEnterAccount[j];
    if (entry->m_bLoad && entry->m_dwAccountSerial == dwAccountSerial)
    {
      bool keyMatch = true;
      for (int k = 0; k < 4; ++k)
      {
        if (entry->m_dwKey[k] != pdwMasterKey[k])
        {
          keyMatch = false;
          break;
        }
      }
      if (keyMatch)
      {
        entry->Release();
        waitData = entry;
        break;
      }
    }
  }

#ifdef CLIENTDEBUGCODE
  if (!waitData && dwAccountSerial == kClientDebugAccountSerial)
  {
    InitializeClientDebugWaitAccount(clientDebugWaitData, pdwMasterKey);
    waitData = &clientDebugWaitData;
    g_Main.m_logSystemError.Write(
      "CLIENTDEBUGCODE Enter_Account: synthesized wait account for accountSerial=%u",
      dwAccountSerial);
  }
#endif

  if (!waitData)
  {
    g_Main.m_logSystemError.Write( "%d.. Enter_Account : pWaitData == NULL", dwAccountSerial);
    return false;
  }

  bool valid = true;
  if (!waitData->m_byUserDgr && dwProtocolVer != 126455)
  {
    g_Main.m_logSystemError.Write(
      "%s.. Enter_Account : if(dwProtocolVer != __PROTOCOL_VER)",
      waitData->m_szAccountID);
    valid = false;
  }
  if (g_Player[m_idWorld.wIndex].m_bLoad)
  {
    g_Main.m_logSystemError.Write(
      "%s.. Enter_Account : if(g_Player[m_idWorld.wIndex].m_bLoad)",
      waitData->m_szAccountID);
    valid = false;
  }
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CUserDB *user = &g_UserDB[j];
    if (user->m_bActive && dwAccountSerial == user->m_dwAccountSerial)
    {
      g_Main.m_logSystemError.Write(
        "%s.. Enter_Account : if(dwAccountSerial == p->m_dwAccountSerial)",
        waitData->m_szAccountID);
      valid = false;
    }
  }

  if (!valid)
  {
    _logout_account_request_wrac msg{};
    std::memcpy(&msg, &m_gidGlobal, sizeof(msg));
    unsigned __int8 type[2]{1, 5};
    const unsigned __int16 len = msg.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&msg), len);
    return false;
  }

  m_bActive = true;
  m_dwOperLobbyTime = timeGetTime();
  m_dwAccountSerial = dwAccountSerial;
  std::strcpy(m_szAccountID, waitData->m_szAccountID);
  m_byUserDgr = waitData->m_byUserDgr;
  m_bySubDgr = waitData->m_bySubDgr;
  m_dwIP = dwIP;
  std::memcpy(&m_gidGlobal, &waitData->m_gidGlobal, sizeof(m_gidGlobal));
  m_dwTotalPlayMin = 0;
  m_dwSerial = static_cast<unsigned int>(-1);
  m_bDBWaitState = false;
  m_pDBPushData = nullptr;
  m_bChatLock = waitData->m_bChatLock;
  m_bWndFullMode = false;
  m_bDataUpdate = false;
  m_nTrans = waitData->m_nTrans;
  for (int j = 0; j < 50; ++j)
  {
    m_NotArrangedChar[j].dwSerial = static_cast<unsigned int>(-1);
    m_dwArrangePassCase0[j] = static_cast<unsigned int>(-1);
  }

  unsigned __int8 retCode = 0;
  ClearBillingData();
  SetBillingData(&waitData->m_BillingInfo);
  if (m_byUserDgr)
  {
    m_byUILock = 2;
  }
  else
  {
    if (!m_BillingInfo.iType)
    {
      g_Main.m_logSystemError.Write( "%s.. Enter_Account Billing Type = 0", waitData->m_szAccountID);
      return false;
    }
    if (!m_BillingInfo.lRemainTime && (m_BillingInfo.iType == 5 || m_BillingInfo.iType == 2))
    {
      g_Main.m_logSystemError.Write(
        "%s.. Enter_Account Billing lRemainTime = 0",
        waitData->m_szAccountID);
      return false;
    }

    m_byUILock = waitData->m_byUILock;
    m_byUILock_InitFailCnt = waitData->m_byUILock_FailCnt;
    m_byUILock_FailCnt = m_byUILock_InitFailCnt;
    m_byUILock_HintIndex = waitData->m_byUILock_HintIndex;
    strcpy_s(m_szUILock_PW, 13, waitData->m_szUILock_PW);
    strcpy_s(m_szAccount_PW, 13, waitData->m_szAccount_PW);
    strcpy_s(m_uszUILock_HintAnswer, 17, waitData->m_uszUILock_HintAnswer);
    m_byUILock_InitFindPassFailCount = waitData->m_byUILockFindPassFailCount;
    m_byUILockFindPassFailCount = m_byUILock_InitFindPassFailCount;
    for (int m = 0; m < 3; ++m)
    {
      m_dwRequestMoveCharacterSerialList[m] = waitData->m_dwRequestMoveCharacterSerialList[m];
      m_dwTournamentCharacterSerialList[m] = waitData->m_dwTournamentCharacterSerialList[m];
    }
  }

  if (!retCode)
  {
    _enter_world_request_wrac request{};
    std::memcpy(&request, &m_gidGlobal, 8u);
    std::memcpy(&request.idLocal, &m_idWorld, sizeof(request.idLocal));
    request.ulConnectIP = m_ipAddress;
    unsigned __int8 type[2]{1, 10};
    const unsigned __int16 len = static_cast<unsigned __int16>(request.size());
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&request), len);
  }

  _enter_world_result_zone result{};
  result.byResult = retCode;
  result.byUserGrade = m_byUserDgr;
  result.bySvrType = g_Main.m_byWorldType;
  unsigned __int8 type[2]{1, 2};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&result), len);
  return true;
}

bool CUserDB::Lobby_Char_Request()
{
  if (!m_bActive)
  {
    return false;
  }
  if (!m_bField || m_dwSerial == static_cast<unsigned int>(-1))
  {
    return false;
  }

  g_Player[m_idWorld.wIndex].NetClose(true);
  _qry_sheet_lobby qry;
  qry.dwAvatorSerial = m_dwSerial;
  std::memcpy(&qry.NewData, &m_AvatorData, sizeof(qry.NewData));
  _AVATOR_DATA *contData = IsContPushBefore();
  if (contData)
  {
    std::memcpy(&qry.OldData, contData, sizeof(qry.OldData));
  }
  else
  {
    std::memcpy(&qry.OldData, &m_AvatorData_bk, sizeof(qry.OldData));
  }

  qry.bUpdateRefineCnt = false;
  if (g_Main.m_pRFEvent_ClassRefine->IsDbUpdate(m_idWorld.wIndex))
  {
    _event_participant_classrefine *state =
      g_Main.m_pRFEvent_ClassRefine->GetPlayerState(m_idWorld.wIndex, m_idWorld.dwSerial);
    if (state)
    {
      qry.bUpdateRefineCnt = true;
      qry.byRefinedCnt = state->nCurRefineCnt;
      qry.dwRefineDate = state->dwRefineDate;
    }
  }

  const int size = static_cast<int>(qry.size());
  if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 6u, reinterpret_cast<char *>(&qry), size))
  {
    m_dwOperLobbyTime = GetTickCount();
    m_bDBWaitState = true;
    return true;
  }

  Exit_Account_Complete(100);
  return false;
}

bool CUserDB::Reged_Char_Request()
{
  if (!m_bActive)
  {
    return false;
  }
  if (!m_ss.chk_reged())
  {
    return false;
  }
  if (m_bDBWaitState)
  {
    return false;
  }

  for (int j = 0; j < 50; ++j)
  {
    m_NotArrangedChar[j].dwSerial = static_cast<unsigned int>(-1);
    m_dwArrangePassCase0[j] = static_cast<unsigned int>(-1);
  }

  s_MgrLobbyHistory.reged_char_request(m_szLobbyHistoryFileName);

  _qry_sheet_reged qry;
  qry.dwAccountSerial = m_dwAccountSerial;
  in_addr addr{};
  addr.S_un.S_addr = m_ipAddress;
  char *ipText = inet_ntoa(addr);
  std::strcpy(qry.in_szIP, ipText);

  const int size = static_cast<int>(qry.size());
  if (!g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 0u, reinterpret_cast<char *>(&qry), size))
  {
    return false;
  }
  m_bDBWaitState = true;
  return true;
}

bool CUserDB::Insert_Char_Request(
  char *pwszCharName,
  unsigned __int8 bySlotIndex,
  unsigned __int8 byRaceSexCode,
  char *pszClassCode,
  unsigned int dwBaseShape)
{
  if (!m_bActive)
  {
    return false;
  }
  if (!m_ss.bReged || m_ss.bSelect)
  {
    return false;
  }
  if (m_bDBWaitState)
  {
    return false;
  }
  if (m_RegedList[bySlotIndex].m_bySlotIndex != 255)
  {
    return false;
  }

  bool invalidName = false;

  do
  {
    // Yorozuya fix (non-IDA parity): reject empty character names.
    if (!pwszCharName || !*pwszCharName)
    {
      invalidName = true;
      break;
    }
    // Yorozuya fix (non-IDA parity): reject invalid base shape (0xF nibble).
    if (!IsValidBaseShape(dwBaseShape))
    {
      invalidName = true;
      break;
    }

    for (char *cursor = pwszCharName; ; ++cursor)
    {
      if (*cursor == 32 || *cursor == 39)
      {
        invalidName = true;
        break;
      }
      if (!*cursor)
      {
        break;
      }
    }
    if (invalidName)
    {
      break;
    }
    if (*pwszCharName == 42)
    {
      invalidName = true;
      break;
    }
    for (int k = 0; k < 3; ++k)
    {
      if (!std::strcmp(pwszCharName, wszNonMakeName_0[k]))
      {
        invalidName = true;
        break;
      }
    }
    if (invalidName)
    {
      break;
    }

    CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
    if ((m_byUserDgr != 2 && std::strlen(pwszCharName) >= nGMCmpLen
         && !strncmp(pwszCharName, wszGMCmp, nGMCmpLen))
        || !manager->IsNormalString(pwszCharName))
    {
      invalidName = true;
    }
  }
  while (false);
  if (invalidName)
  {
    Insert_Char_Complete(47, nullptr);
    return true;
  }

  if (IsSQLValidString(pwszCharName))
  {
    _class_fld *record = static_cast<_class_fld *>(g_Main.m_tblClass.GetRecord(pszClassCode));
    if (!record)
    {
      return false;
    }
    if (record->m_nGrade)
    {
      return false;
    }
    if (record->m_nRaceCode == static_cast<int>(byRaceSexCode) >> 1)
    {
      s_MgrLobbyHistory.add_char_request(m_szLobbyHistoryFileName);
      _qry_sheet_insert qry;
      qry.dwAccountSerial = m_dwAccountSerial;
      std::strcpy(qry.szAccountID, m_szAccountID);
      std::strcpy(qry.InsertData.m_wszAvatorName, pwszCharName);
      qry.InsertData.m_byRaceSexCode = byRaceSexCode;
      qry.InsertData.m_bySlotIndex = bySlotIndex;
      qry.InsertData.m_dwBaseShape = dwBaseShape;
      std::strcpy(qry.InsertData.m_szClassCode, record->m_strCode);
      const int size = static_cast<int>(qry.size());
      if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 1u, reinterpret_cast<char *>(&qry), size))
      {
        m_bDBWaitState = true;
        return true;
      }
      return false;
    }
    return false;
  }

  g_Main.m_logSystemError.Write(
    "CUserDB::Insert_Char_Request() : Account : %s(%u) ::IsSQLValidString(pwszCharName(%s)) Invalid!",
    m_szAccountID,
    m_dwAccountSerial,
    pwszCharName);
  Insert_Char_Complete(47, nullptr);
  return true;
}

bool CUserDB::Delete_Char_Request(unsigned __int8 bySlotIndex)
{
  if (!m_bActive)
  {
    return false;
  }
  if (!m_ss.bReged || m_ss.bSelect)
  {
    return false;
  }
  if (m_bDBWaitState)
  {
    return false;
  }
  if (m_RegedList[bySlotIndex].m_bySlotIndex == 255)
  {
    return false;
  }

  unsigned __int8 exist = IsExistRequestMoveCharacterList(m_RegedList[bySlotIndex].m_dwRecordNum);
  if (exist)
  {
    _del_char_result_zone result{};
    std::memset(&result, 0, sizeof(result));
    if (exist == 1)
    {
      result.byRetCode = 74;
    }
    else
    {
      result.byRetCode = 76;
    }
    result.bySlotIndex = bySlotIndex;
    unsigned __int8 type[2]{1, 13};
    const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&result), len);
    return true;
  }

  s_MgrLobbyHistory.del_char_request(
    bySlotIndex,
    m_RegedList[bySlotIndex].m_dwRecordNum,
    m_szLobbyHistoryFileName);
  _qry_sheet_delete qry;
  qry.bySlotIndex = bySlotIndex;
  qry.byRaceCode = static_cast<unsigned __int8>(m_RegedList[bySlotIndex].m_byRaceSexCode >> 1);
  qry.dwAvatorSerial = m_RegedList[bySlotIndex].m_dwRecordNum;
  const int size = static_cast<int>(qry.size());
  if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 2u, reinterpret_cast<char *>(&qry), size))
  {
    m_bDBWaitState = true;
    return true;
  }
  return false;
}

bool CUserDB::Select_Char_Request(unsigned __int8 bySlotIndex)
{
  if (!m_bActive)
  {
    return false;
  }
  if (!m_ss.bReged)
  {
    return false;
  }
  if (!m_ss.chk_select())
  {
    return false;
  }
  if (m_bDBWaitState)
  {
    return false;
  }
  if (m_RegedList[bySlotIndex].m_bySlotIndex == 255)
  {
    return false;
  }

  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CUserDB *user = &g_UserDB[j];
    if (user->m_bActive && m_RegedList[bySlotIndex].m_dwRecordNum == user->m_dwSerial)
    {
      return false;
    }
  }

  unsigned __int8 exist = IsExistRequestMoveCharacterList(m_RegedList[bySlotIndex].m_dwRecordNum);
  if (exist)
  {
    m_ss.bSelect = false;
    _sel_char_result_zone result{};
    std::memset(&result, 0, sizeof(result));
    if (exist == 1)
    {
      result.byRetCode = 74;
    }
    else
    {
      result.byRetCode = 76;
    }
    unsigned __int8 type[2]{1, 15};
    const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&result), len);
    return true;
  }

  s_MgrLobbyHistory.sel_char_request(
    bySlotIndex,
    m_RegedList[bySlotIndex].m_dwRecordNum,
    m_szLobbyHistoryFileName);
  _qry_sheet_load qry{};
  qry.dwAvatorSerial = m_RegedList[bySlotIndex].m_dwRecordNum;
  qry.dwCheckSum = 0;
  qry.dwCanonicalLastConnTime = GetRegedCanonicalLastConnTime(bySlotIndex);
  std::memcpy(&qry.LoadData, &m_RegedList[bySlotIndex], 269);

  const int size = static_cast<int>(qry.size());
  if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 3u, reinterpret_cast<char *>(&qry), size))
  {
    m_bDBWaitState = true;
    return true;
  }
  return false;
}

unsigned __int8 CUserDB::IsExistRequestMoveCharacterList(unsigned int dwCharSerial)
{
  for (int j = 0; j < 3; ++j)
  {
    if (m_dwRequestMoveCharacterSerialList[j] || m_dwTournamentCharacterSerialList[j])
    {
      if (m_dwRequestMoveCharacterSerialList[j] == dwCharSerial)
      {
        return 1;
      }
      if (m_dwTournamentCharacterSerialList[j] == dwCharSerial)
      {
        return 2;
      }
    }
  }
  return 0;
}

bool CUserDB::Alive_Char_Request(
  unsigned __int8 byCase,
  unsigned int dwSerial,
  char *pwszName,
  unsigned __int8 bySlotIndex)
{
  if (m_RegedList[bySlotIndex].m_bySlotIndex != 255)
  {
    Alive_Char_Complete(50, byCase, dwSerial, nullptr, 0);
    return false;
  }

  if (byCase)
  {
    bool hasArranged = false;
    for (int j = 0; j < 50; ++j)
    {
      if (m_dwArrangePassCase0[j] == dwSerial)
      {
        hasArranged = true;
        break;
      }
    }
    if (!hasArranged)
    {
      Alive_Char_Complete(50, byCase, dwSerial, nullptr, 0);
      return false;
    }
  }

  {
    bool found = false;
    for (int k = 0; k < 50; ++k)
    {
      if (m_NotArrangedChar[k].dwSerial != static_cast<unsigned int>(-1)
          && m_NotArrangedChar[k].dwSerial == dwSerial)
      {
        if (byCase)
        {
          if (!std::strcmp(m_NotArrangedChar[k].wszName, pwszName))
          {
            break;
          }
        }
        else if (std::strcmp(m_NotArrangedChar[k].wszName, pwszName))
        {
          break;
        }
        found = true;
        break;
      }
    }
    if (!found)
    {
      Alive_Char_Complete(57, byCase, dwSerial, nullptr, 0);
      return false;
    }
  }

  for (char *cursor = pwszName; ; ++cursor)
  {
    if (*cursor == 32 || *cursor == 39)
    {
      Alive_Char_Complete(47, byCase, dwSerial, nullptr, 0);
      return true;
    }
    if (!*cursor)
    {
      break;
    }
  }

  CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
  if (manager->IsNormalString(pwszName))
  {
    _qry_case_alive_char qry;
    qry.in_byCase = byCase;
    qry.in_dwSerial = dwSerial;
    std::strcpy(qry.in_w_szName, pwszName);
    qry.in_bySlot = bySlotIndex;
    const int size = static_cast<int>(qry.size());
    if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 23, reinterpret_cast<char *>(&qry), size))
    {
      m_bDBWaitState = true;
      return true;
    }
    return false;
  }

  Insert_Char_Complete(47, nullptr);
  return true;
}

void CUserDB::Insert_Char_Complete(unsigned __int8 byRetCode, _REGED_AVATOR_DB *pInsertData)
{
  m_bDBWaitState = false;
  m_dwOperLobbyTime = GetTickCount();

  unsigned __int8 slotIndex = static_cast<unsigned __int8>(-1);
  if (!byRetCode && pInsertData)
  {
    slotIndex = pInsertData->m_bySlotIndex;
    std::memcpy(&m_RegedList[slotIndex], pInsertData, 69);
    SetRegedCanonicalLastConnTime(slotIndex, pInsertData->m_dwLastConnTime);
  }

  s_MgrLobbyHistory.add_char_complete(byRetCode, pInsertData, m_szLobbyHistoryFileName);

  _add_char_result_zone result{};
  result.byRetCode = byRetCode;
  result.byAddSlotIndex = slotIndex;
  unsigned __int8 type[2]{1, 11};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&result), len);
}

_AVATOR_DATA *CUserDB::IsContPushBefore()
{
  if (!m_bDBWaitState)
  {
    return nullptr;
  }
  if (!m_pDBPushData)
  {
    return nullptr;
  }
  if (!m_pDBPushData->m_bUse)
  {
    return nullptr;
  }
  if (m_pDBPushData->m_byQryCase != 12)
  {
    return nullptr;
  }

  if (!std::memcmp(&m_idWorld, &m_pDBPushData->m_idWorld, sizeof(_CLID)))
  {
    auto *query = reinterpret_cast<_qry_case_contsave *>(m_pDBPushData->m_sData);
    if (query->dwAvatorSerial == m_dwSerial)
    {
      return &query->NewData;
    }

    g_Main.m_logSave.Write(
      "Before Push Cont Save FAIL >> id: %s, name: %s (%d) : LoadSheet->dwAvatorSerial != m_dwSerial",
      m_szAccountID,
      m_aszAvatorName,
      m_dwSerial);
    return nullptr;
  }

  g_Main.m_logSave.Write(
    "Before Push Cont Save FAIL >> id: %s, name: %s (%d) : if(memcmp(&m_idWorld, &pLoadSheet->m_idWorld, sizeof(_CLID)))",
    m_szAccountID,
    m_aszAvatorName,
    m_dwSerial);
  return nullptr;
}

void CUserDB::Exit_Account_Request()
{
  CNationSettingManager::Instance()->OnDisConnectSession(m_idWorld.wIndex);

  if (m_bActive)
  {
    BeginShutdownLogoutTracking(*this);

    if (m_dwSerial == static_cast<unsigned int>(-1))
    {
      Exit_Account_Complete(0);
    }
    else
    {
      _qry_sheet_logout qry{};
      qry.dwAvatorSerial = m_dwSerial;
      std::memcpy(&qry.NewData, &m_AvatorData, sizeof(qry.NewData));
      _AVATOR_DATA *contData = IsContPushBefore();
      if (contData)
      {
        std::memcpy(&qry.OldData, contData, sizeof(qry.OldData));
      }
      else
      {
        std::memcpy(&qry.OldData, &m_AvatorData_bk, sizeof(qry.OldData));
      }

      qry.bCheckLowHigh = !m_bNoneUpdateData;
      qry.bUpdateRefineCnt = false;

      if (g_Main.m_pRFEvent_ClassRefine->IsDbUpdate(m_idWorld.wIndex))
      {
        _event_participant_classrefine *state =
          g_Main.m_pRFEvent_ClassRefine->GetPlayerState(m_idWorld.wIndex, m_idWorld.dwSerial);
        if (state)
        {
          qry.bUpdateRefineCnt = true;
          qry.byRefinedCnt = state->nCurRefineCnt;
          qry.dwRefineDate = state->dwRefineDate;
        }
      }

      const int size = static_cast<int>(qry.size());
      if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 5u, reinterpret_cast<char *>(&qry), size))
      {
        m_bDBWaitState = true;
      }
      else
      {
        Exit_Account_Complete(100);
      }
    }
  }
  else
  {
    ParamInit();
  }
}

void CUserDB::Exit_Account_Complete(unsigned __int8 byRetCode)
{
  CompleteShutdownLogoutTracking(*this);
  m_bDBWaitState = false;

  _logout_account_request_wrac msg{};
  std::memcpy(&msg, &m_gidGlobal, sizeof(msg));
  unsigned __int8 type[2] = {1, 5};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&msg), len);

  ParamInit();
}

void CUserDB::Lobby_Char_Complete(unsigned __int8 byRetCode)
{
  m_bDBWaitState = false;
  m_bField = false;
  m_dwSerial = static_cast<unsigned int>(-1);
  m_ss.re_lobby();
  m_bDataUpdate = false;
  m_AvatorData.InitData();
  m_AvatorData_bk.InitData();
  for (int j = 0; j < 3; ++j)
  {
    m_RegedList[j].init();
  }

  if (m_byUserDgr)
  {
    m_byUILock = 2;
    _moveout_user_result_zone msg{};
    msg.byRetCode = byRetCode;
    unsigned __int8 type[2] = {1, 7};
    const unsigned __int16 len = msg.size();
    g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
  else
  {
    s_MoveLobbyDelay.Push(m_idWorld.wIndex, m_idWorld.dwSerial);
    m_byUILock = 1;
  }

  _enter_lobby_report_wrac report{};
  std::memcpy(&report, &m_gidGlobal, sizeof(report));
  unsigned __int8 type[2] = {1, 13};
  const unsigned __int16 len = report.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&report), len);
}

void CUserDB::Cont_UserSave_Complete(
  unsigned __int8 byResult,
  _AVATOR_DATA *pAvatorData,
  const unsigned __int64 *pdwCanonicalUnitCutTime)
{
  if (byResult)
  {
    g_Network.Close(0, m_idWorld.wIndex, true, "Cont Save Error");
    return;
  }

  _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY *startHistory = m_AvatorData_bk.dbQuest.m_StartHistory;
  unsigned int listCount = m_AvatorData_bk.dbQuest.dwListCnt;
  std::memcpy(&m_AvatorData_bk, pAvatorData, sizeof(m_AvatorData_bk));
  m_AvatorData_bk.dbQuest.m_StartHistory = startHistory;
  m_AvatorData_bk.dbQuest.dwListCnt = listCount;
  SyncCanonicalBackupLastConnTime();
  if (pdwCanonicalUnitCutTime)
  {
    for (unsigned __int8 slotIndex = 0; slotIndex < 4u; ++slotIndex)
    {
      SetCanonicalBackupUnitCutTime(slotIndex, pdwCanonicalUnitCutTime[slotIndex]);
    }
  }
  else
  {
    SyncCanonicalBackupUnitCutTime();
  }

  const unsigned int addCount = m_AvatorData.dbQuest.dwListCnt - m_AvatorData_bk.dbQuest.dwListCnt;
  for (unsigned int j = 0; j < addCount; ++j)
  {
    std::memcpy(
      &m_AvatorData_bk.dbQuest.m_StartHistory[m_AvatorData_bk.dbQuest.dwListCnt],
      &m_AvatorData.dbQuest.m_StartHistory[m_AvatorData_bk.dbQuest.dwListCnt],
      sizeof(m_AvatorData_bk.dbQuest.m_StartHistory[m_AvatorData_bk.dbQuest.dwListCnt]));
    ++m_AvatorData_bk.dbQuest.dwListCnt;
  }

  m_bDBWaitState = false;
  m_pDBPushData = nullptr;
}

void CUserDB::Alive_Char_Complete(
  unsigned __int8 byRetCode,
  unsigned __int8 byCase,
  unsigned int dwSerial,
  _REGED *pAliveAvator,
  unsigned __int64 dwCanonicalLastConnTime)
{
  m_bDBWaitState = false;

  unsigned __int8 resultCode = byRetCode;
  unsigned __int8 slotIndex = 255;
  if (!byRetCode)
  {
    slotIndex = pAliveAvator->m_bySlotIndex;
    if (slotIndex < 3u)
    {
      if (m_RegedList[slotIndex].m_bySlotIndex == 255)
      {
        std::memcpy(&m_RegedList[slotIndex], pAliveAvator, sizeof(m_RegedList[slotIndex]));
        SetRegedCanonicalLastConnTime(slotIndex, dwCanonicalLastConnTime);
        m_RegedList[slotIndex].UpdateEquipLv();
        for (int j = 0; j < 50; ++j)
        {
          if (m_NotArrangedChar[j].dwSerial == dwSerial)
          {
            m_NotArrangedChar[j].dwSerial = static_cast<unsigned int>(-1);
            break;
          }
        }
      }
      else
      {
        resultCode = 50;
      }
    }
    else
    {
      resultCode = 50;
    }
  }

  if (!byCase)
  {
    for (int k = 0; k < 50; ++k)
    {
      if (m_dwArrangePassCase0[k] == static_cast<unsigned int>(-1))
      {
        m_dwArrangePassCase0[k] = dwSerial;
        break;
      }
    }
  }

  s_MgrLobbyHistory.recovery_char_complete(resultCode, pAliveAvator, m_szLobbyHistoryFileName);

  _alive_char_result_zocl msg{};
  msg.byRetCode = resultCode;
  if (!resultCode)
  {
    msg.dwSerial = dwSerial;
    std::memcpy(&msg.AliveChar, &m_RegedList[slotIndex], sizeof(msg.AliveChar));
  }

  unsigned __int8 type[2] = {1, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&msg), 74);
}

bool CUserDB::Update_Param(_EXIT_ALTER_PARAM *pCon)
{
  m_AvatorData.dbAvator.m_dwHP = pCon->dwHP;
  m_AvatorData.dbAvator.m_dwFP = pCon->dwFP;
  m_AvatorData.dbAvator.m_dwSP = pCon->dwSP;
  m_AvatorData.dbAvator.m_dwDP = pCon->dwDP;
  m_AvatorData.dbAvator.m_dExp = pCon->dExp;
  m_AvatorData.dbAvator.m_dwDalant = pCon->dwDalant;
  m_AvatorData.dbAvator.m_dwGold = pCon->dwGold;

  if (g_MapOper.IsExistStdMapID(pCon->byMapCode))
  {
    m_AvatorData.dbAvator.m_byMapCode = pCon->byMapCode;
    m_AvatorData.dbAvator.m_fStartPos[0] = pCon->fStartPos[0];
    m_AvatorData.dbAvator.m_fStartPos[1] = pCon->fStartPos[1];
    m_AvatorData.dbAvator.m_fStartPos[2] = pCon->fStartPos[2];
  }

  return true;
}

bool CUserDB::CheckDQSLoadCharacterData(_AVATOR_DATA *pData)
{
  int maxLevel = pData->dbAvator.m_byMaxLevel ? pData->dbAvator.m_byMaxLevel : 50;
  if (pData->dbAvator.m_byLevel <= maxLevel)
  {
    if (pData->dbAvator.m_byRaceSexCode < 5u)
    {
      return true;
    }

    static const char kDataValidCheckRace[] = "D_DataValidCheck (Serial:%d) Race:%d";
    CUserDB::s_logAvatorDB.Write(
      kDataValidCheckRace,
      pData->dbAvator.m_dwRecordNum,
      pData->dbAvator.m_byRaceSexCode);
    return false;
  }

  static const char kDataValidCheckLevel[] = "D_DataValidCheck (Serial:%d) Level:%d Max:%d";
  CUserDB::s_logAvatorDB.Write(
    kDataValidCheckLevel,
    pData->dbAvator.m_dwRecordNum,
    pData->dbAvator.m_byLevel,
    maxLevel);
  return false;
}

void CUserDB::ReRangeClientIndex(_AVATOR_DATA *pData)
{
  CNetIndexList list;
  list.SetList(100);

  unsigned char slotUsed[100]{};
  unsigned int indexList[101]{};

  int maxSlots = 20 * pData->dbAvator.m_byBagNum;
  list.ResetList();
  std::memset(slotUsed, 0, sizeof(slotUsed));

  for (unsigned int i = 0; static_cast<int>(i) < maxSlots; ++i)
  {
    if (pData->dbInven.m_List[i].Key.IsFilled())
    {
      unsigned __int8 slotIndex = pData->dbInven.m_List[i].Key.bySlotIndex;
      if (slotIndex >= maxSlots || slotUsed[slotIndex])
      {
        list.PushNode_Back(i);
      }
      else
      {
        slotUsed[slotIndex] = 1;
      }
    }
  }

  int startIndex = 0;
  int listCount = list.CopyIndexList(indexList, maxSlots);
  for (int j = 0; j < listCount; ++j)
  {
    for (int k = startIndex; k < maxSlots; ++k)
    {
      if (!slotUsed[k])
      {
        pData->dbInven.m_List[indexList[j]].Key.bySlotIndex = static_cast<unsigned __int8>(k);
        slotUsed[k] = 1;
        startIndex = k;
        break;
      }
    }
  }

  maxSlots = 7;
  list.ResetList();
  std::memset(slotUsed, 0, sizeof(slotUsed));
  startIndex = 0;

  for (unsigned int i = 0; static_cast<int>(i) < maxSlots; ++i)
  {
    if (pData->dbEquip.m_EmbellishList[i].Key.IsFilled())
    {
      unsigned __int8 slotIndex = pData->dbEquip.m_EmbellishList[i].Key.bySlotIndex;
      if (slotIndex >= maxSlots || slotUsed[slotIndex])
      {
        list.PushNode_Back(i);
      }
      else
      {
        slotUsed[slotIndex] = 1;
      }
    }
  }

  listCount = list.CopyIndexList(indexList, maxSlots);
  for (int j = 0; j < listCount; ++j)
  {
    for (int k = startIndex; k < maxSlots; ++k)
    {
      if (!slotUsed[k])
      {
        pData->dbEquip.m_EmbellishList[indexList[j]].Key.bySlotIndex = static_cast<unsigned __int8>(k);
        slotUsed[k] = 1;
        startIndex = k;
        break;
      }
    }
  }

  maxSlots = pData->dbTrunk.bySlotNum;
  list.ResetList();
  std::memset(slotUsed, 0, sizeof(slotUsed));
  startIndex = 0;

  for (unsigned int i = 0; static_cast<int>(i) < maxSlots; ++i)
  {
    if (pData->dbTrunk.m_List[i].Key.IsFilled())
    {
      unsigned __int8 slotIndex = pData->dbTrunk.m_List[i].Key.bySlotIndex;
      if (slotIndex >= maxSlots || slotUsed[slotIndex])
      {
        list.PushNode_Back(i);
      }
      else
      {
        slotUsed[slotIndex] = 1;
      }
    }
  }

  listCount = list.CopyIndexList(indexList, maxSlots);
  for (int j = 0; j < listCount; ++j)
  {
    for (int k = startIndex; k < maxSlots; ++k)
    {
      if (!slotUsed[k])
      {
        pData->dbTrunk.m_List[indexList[j]].Key.bySlotIndex = static_cast<unsigned __int8>(k);
        slotUsed[k] = 1;
        startIndex = k;
        break;
      }
    }
  }
}

_BUDDY_DB_BASE::_LIST::_LIST()
{
  Init();
}


void _BUDDY_DB_BASE::_LIST::Init()
{
  dwSerial = static_cast<unsigned int>(-1);
  wszName[0] = '\0';
}


bool _BUDDY_DB_BASE::_LIST::IsFilled()
{
  return dwSerial != static_cast<unsigned int>(-1);
}


_BUDDY_DB_BASE::_BUDDY_DB_BASE()
{
  Init();
}


void _BUDDY_DB_BASE::Init()
{
  for (int j = 0; j < 50; ++j)
  {
    m_List[j].Init();
  }
}


_INVENKEY &_INVENKEY::operator=(const _INVENKEY &rhs)
{
  std::memcpy(this, &rhs, sizeof(_INVENKEY));
  return *this;
}


bool _INVENKEY::operator==(const _INVENKEY &rhs) const
{
  return byTableCode == rhs.byTableCode && wItemIndex == rhs.wItemIndex;
}


int _INVENKEY::IsOverlapItem() const
{
  return IsOverLapItem(byTableCode) != 0;
}


_INVEN_DB_BASE::_LIST &_INVEN_DB_BASE::_LIST::operator=(const _LIST &rhs)
{
  Key = rhs.Key;
  dwDur = rhs.dwDur;
  dwUpt = rhs.dwUpt;
  dwItemETSerial = rhs.dwItemETSerial;
  lnUID = rhs.lnUID;
  byCsMethod = rhs.byCsMethod;
  dwT = rhs.dwT;
  dwLendRegdTime = rhs.dwLendRegdTime;
  return *this;
}


_INVEN_DB_BASE & _INVEN_DB_BASE::operator=(const _INVEN_DB_BASE &rhs)
{
  for (unsigned int j = 0; j < 100; ++j)
  {
    m_List[j] = rhs.m_List[j];
  }
  return *this;
}


void _CUTTING_DB_BASE::_LIST::Init()
{
  Key.SetRelease();
  dwDur = 0;
}


void _CUTTING_DB_BASE::Init()
{
  m_byLeftNum = 0;
  for (int j = 0; j < 20; ++j)
  {
    m_List[j].Init();
  }
}


void _CUTTING_DB_BASE::ReSetOldDataLoad()
{
  m_bOldDataLoad = false;
}


_NOT_ARRANGED_AVATOR_DB::_NOT_ARRANGED_AVATOR_DB()
{
  Init();
}


void _NOT_ARRANGED_AVATOR_DB::Init()
{
  std::memset(this, 0, sizeof(_NOT_ARRANGED_AVATOR_DB));
  dwSerial = static_cast<unsigned int>(-1);
}

bool CUserDB::Update_AddBuddy(unsigned __int8 bySlotIndex, unsigned int dwAdderSerial, char *pwszAdderName)
{
  m_AvatorData.dbBuddy.m_List[bySlotIndex].dwSerial = dwAdderSerial;
  std::strcpy(m_AvatorData.dbBuddy.m_List[bySlotIndex].wszName, pwszAdderName);
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_DelBuddy(unsigned __int8 bySlotIndex)
{
  m_AvatorData.dbBuddy.m_List[bySlotIndex].Init();
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_UserGetScaner(unsigned __int16 wScanerCnt, unsigned __int16 wBattleTime)
{
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  char buffer[56]{};
  std::memset(buffer, 0, 30);
  m_AvatorData.dbSupplement.wScanerCnt += wScanerCnt;
  sprintf_s(
    buffer,
    30,
    "%04d%02d%02d%d",
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    wBattleTime);
  m_AvatorData.dbSupplement.dwScanerGetDate = static_cast<unsigned int>(atoi(buffer));
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_UserPlayTime(unsigned int dwAccPlayTime)
{
  m_AvatorData.dbSupplement.dwAccumPlayTime += dwAccPlayTime;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_RaceVoteInfoInit()
{
  m_AvatorData.dbSupplement.VoteEnable = 1;
  m_AvatorData.dbSupplement.dwAccumPlayTime = 0;
  m_AvatorData.dbSupplement.byVoted = 0;
  m_AvatorData.dbSupplement.wScanerCnt = 0;
  m_AvatorData.dbSupplement.dwScanerGetDate = 0;
  m_AvatorData.dbSupplement.dwLastResetDate = g_Main.m_dwServerResetToken;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_UserVoteData()
{
  m_AvatorData.dbSupplement.VoteEnable = 0;
  m_AvatorData.dbSupplement.byVoted = 1;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_AutoTradeAllClear()
{
  m_AvatorData.dbTrade.Clear();
  return true;
}

bool CUserDB::Update_LinkBoardSlot(unsigned __int8 bySlot, unsigned __int8 byLinkCode, unsigned __int16 wIndex)
{
  if (bySlot >= 50)
  {
    return false;
  }

  if (byLinkCode == 255)
  {
    m_AvatorData.dbLink.m_LinkList[bySlot].Key.SetRelease();
  }
  else if (byLinkCode < 7u)
  {
    m_AvatorData.dbLink.m_LinkList[bySlot].Key.SetData(byLinkCode, wIndex);
  }
  return true;
}

bool CUserDB::Update_LinkBoardLock(unsigned __int8 byLBLock)
{
  m_AvatorData.dbLink.m_byLinkBoardLock = byLBLock;
  return true;
}

bool CUserDB::Update_ItemSlot(unsigned __int8 storage, unsigned __int8 slot, unsigned __int8 clientpos)
{
  bool outOfRange = false;
  if (!storage)
  {
    outOfRange = slot >= 100;
  }
  else if (storage == 2)
  {
    outOfRange = slot >= 7u;
  }
  else if (storage == 5)
  {
    outOfRange = slot >= 100;
  }
  else if (storage == 6 || storage == 7)
  {
    outOfRange = slot >= 40;
  }
  else
  {
    return false;
  }

  if (outOfRange)
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_ItemSlot(CODE) : scode : %d, icode : %d  ",
      m_aszAvatorName,
      storage,
      slot);
    return false;
  }

  if (!storage)
  {
    _INVENKEY *key = &m_AvatorData.dbInven.m_List[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_ItemSlot(INVEN, Idx:%d)", m_aszAvatorName, slot);
      return false;
    }
    key->bySlotIndex = clientpos;
    return true;
  }

  if (storage == 2)
  {
    _EMBELLKEY *key = &m_AvatorData.dbEquip.m_EmbellishList[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_ItemSlot(EMBELL, Idx:%d)", m_aszAvatorName, slot);
      return false;
    }
    key->bySlotIndex = clientpos;
    return true;
  }

  if (storage == 5)
  {
    _INVENKEY *key = &m_AvatorData.dbTrunk.m_List[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_ItemSlot(TRUNK, Idx:%d)", m_aszAvatorName, slot);
      return false;
    }
    key->bySlotIndex = clientpos;
    return true;
  }

  if (storage == 6)
  {
    _INVENKEY *key = &m_AvatorData.dbPersonalAmineInven.m_List[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_ItemSlot(TRUNK, Idx:%d)", m_aszAvatorName, slot);
      return false;
    }
    key->bySlotIndex = clientpos;
    return true;
  }

  _INVENKEY *key = &m_AvatorData.dbTrunk.m_ExtList[slot].Key;
  if (!key->IsFilled())
  {
    g_Main.m_logSystemError.Write("%s:Update_ItemSlot(EXT_TRUNK, Idx:%d)", m_aszAvatorName, slot);
    return false;
  }
  key->bySlotIndex = clientpos;
  return true;
}

bool CUserDB::Update_WindowInfo(
  unsigned int *pdwSkill,
  unsigned int *pdwForce,
  unsigned int *pdwChar,
  unsigned int *pdwAnimus,
  unsigned int dwInven,
  unsigned int *pdwInvenBag)
{
  std::memcpy(m_AvatorData.dbLink.m_dwSkill, pdwSkill, sizeof(m_AvatorData.dbLink.m_dwSkill));
  std::memcpy(m_AvatorData.dbLink.m_dwForce, pdwForce, sizeof(m_AvatorData.dbLink.m_dwForce));
  std::memcpy(m_AvatorData.dbLink.m_dwCharacter, pdwChar, sizeof(m_AvatorData.dbLink.m_dwCharacter));
  std::memcpy(m_AvatorData.dbLink.m_dwAnimus, pdwAnimus, sizeof(m_AvatorData.dbLink.m_dwAnimus));
  m_AvatorData.dbLink.m_dwInven = dwInven;
  std::memcpy(m_AvatorData.dbLink.m_dwInvenBag, pdwInvenBag, sizeof(m_AvatorData.dbLink.m_dwInvenBag));
  return true;
}

bool CUserDB::Update_Macro(char *pBuf)
{
  const auto *request = reinterpret_cast<const _player_macro_update_request_clzo *>(pBuf);
  _AIOC_A_MACRODATA *macro = &m_AvatorData.dbMacro;

  for (int belt = 0; belt < 1; ++belt)
  {
    for (int slot = 0; slot < 3; ++slot)
    {
      macro->mcr_Potion[belt].Potion[slot] = request->potion[slot];
      macro->mcr_Potion[belt].PotionValue[slot] = request->potionvalue[slot];
    }
  }

  for (int belt = 0; belt < 3; ++belt)
  {
    for (int slot = 0; slot < 10; ++slot)
    {
      macro->mcr_Action[belt].Action[slot] = request->behavior[10 * belt + slot];
    }
  }

  for (int belt = 0; belt < 2; ++belt)
  {
    for (int slot = 0; slot < 5; ++slot)
    {
      char *chat = const_cast<char *>(request->chatting[belt][slot]);
      if (!IsSQLValidString(chat))
      {
        g_Main.m_logSystemError.Write(
          "%u(%s) CUserDB::Update_Macro() : ::IsSQLValidString( pMacro->chatting[iBelt][iSock](%s) ) Invalid!",
          m_dwSerial,
          m_aszAvatorName,
          chat);
        return true;
      }

      std::memset(macro->mcr_Chat[belt].Chat[slot], 0, 81);
      strncpy(macro->mcr_Chat[belt].Chat[slot], chat, 81);
    }
  }

  m_bDataUpdate = 1;
  return true;
}

void CUserDB::Update_BossCryMsg(unsigned __int8 bySlot, char *pwszCryMsg)
{
  std::strcpy(m_AvatorData.dbBossCry.m_List[bySlot].wszCryMsg, pwszCryMsg);
}

bool CUserDB::Update_BagNum(unsigned __int8 bagnum)
{
  if (bagnum > 5u)
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_BagNum(CODE) byBagNum (%d) => failed ",
      m_aszAvatorName,
      bagnum);
    return false;
  }

  if (m_AvatorData.dbAvator.m_byBagNum < bagnum)
  {
    m_AvatorData.dbAvator.m_byBagNum = bagnum;
    m_bDataUpdate = true;
    return true;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_BagNum(CODE) m_AvatorData.dbAvator.m_byBagNum(%d) > pCon->byBagNum(%d) => failed ",
    m_aszAvatorName,
    m_AvatorData.dbAvator.m_byBagNum,
    bagnum);
  return false;
}

void CUserDB::CalcRadarDelay()
{
  if (m_RadarItemMgr.IsUse())
  {
    const unsigned int delayMs = m_RadarItemMgr.CalcDelay();
    unsigned int delaySec = 0;
    if (delayMs)
    {
      delaySec = delayMs / 1000;
      if (delayMs % 1000 >= 500)
      {
        ++delaySec;
      }
    }
    SetRadarDelay(delaySec);
  }
}

void CUserDB::SetRadarDelay(unsigned int dwDelay)
{
  m_AvatorData.dbAvator.m_dwRadarDelayTime = dwDelay;
  m_bDataUpdate = true;
}


bool CUserDB::Update_LossExp(long double dLossExp)
{
  m_AvatorData.dbAvator.m_dLossExp = dLossExp;
  m_bDataUpdate = true;
  return true;
}


bool CUserDB::Update_Exp(long double exp)
{
  m_AvatorData.dbAvator.m_dExp = exp;
  m_bDataUpdate = true;
  return true;
}


bool CUserDB::Update_UserFatigue(unsigned int dwFatigue)
{
  m_AvatorData.dbTimeLimitInfo.dwFatigue = dwFatigue;
  m_bDataUpdate = true;
  return true;
}


bool CUserDB::Update_UserTLStatus(unsigned __int8 byStatus)
{
  m_AvatorData.dbTimeLimitInfo.byTLStatus = byStatus;
  m_bDataUpdate = true;
  return true;
}

unsigned int CUserDB::GetActPoint(unsigned __int8 byCode)
{
  return m_AvatorData.dbSupplement.dwActionPoint[byCode];
}

unsigned int *CUserDB::GetPtrActPoint()
{
  return m_AvatorData.dbSupplement.dwActionPoint;
}

void CUserDB::SetActPoint(unsigned __int8 byCode, unsigned int dwLeftPoint)
{
  unsigned int dwPoint = dwLeftPoint;
  if (byCode == 2)
  {
    if (dwLeftPoint > 999999)
    {
      dwPoint = 999999;
    }
  }
  else if (dwLeftPoint > 9999999)
  {
    dwPoint = 9999999;
  }
  Update_User_Action_Point(byCode, dwPoint);
}

bool CUserDB::Update_User_Action_Point(unsigned __int8 byActionCode, unsigned int dwPoint)
{
  unsigned int clampedPoint = dwPoint;
  if (byActionCode == 2)
  {
    if (dwPoint > 999999)
    {
      clampedPoint = 999999;
    }
  }
  else if (dwPoint > 9999999)
  {
    clampedPoint = 9999999;
  }

  m_AvatorData.dbSupplement.dwActionPoint[byActionCode] = clampedPoint;
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::InitClass(char *pszClassCode)
{
  unsigned __int8 nextHistory = 0;
  int j = 2;
  for (; j >= 0 && m_AvatorData.dbAvator.m_zClassHistory[j] == -1; --j)
  {
  }

  nextHistory = static_cast<unsigned __int8>(j + 1);
  if (nextHistory && nextHistory > m_AvatorData.dbAvator.m_byLastClassGrade)
  {
    m_AvatorData.dbAvator.m_byLastClassGrade = nextHistory;
  }

  std::strcpy(m_AvatorData.dbAvator.m_szClassCode, pszClassCode);
  for (j = 0; j < 3; ++j)
  {
    m_AvatorData.dbAvator.m_zClassHistory[j] = -1;
  }
  m_bDataUpdate = true;
  return true;
}

bool CUserDB::Update_Class(char *pszClassCode, unsigned __int8 byHistoryRecordNum, unsigned __int16 wHistoryClassIndex)
{
  if (byHistoryRecordNum >= 3u)
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_Class(): byHistoryRecordNum (%d) => failed ",
      m_aszAvatorName,
      byHistoryRecordNum);
    return false;
  }

  std::strcpy(m_AvatorData.dbAvator.m_szClassCode, pszClassCode);
  m_AvatorData.dbAvator.m_zClassHistory[byHistoryRecordNum] = static_cast<signed __int16>(wHistoryClassIndex);
  if (byHistoryRecordNum + 1 > m_AvatorData.dbAvator.m_byLastClassGrade)
  {
    m_AvatorData.dbAvator.m_byLastClassGrade = byHistoryRecordNum + 1;
  }
  m_bDataUpdate = true;
  return true;
}

void CUserDB::WriteLog_ChangeClassAfterInitClass(unsigned __int8 byType, char *szPrevClass)
{
  if (byType == 1 && !m_AvatorData.dbAvator.m_dwClassInitCnt)
  {
    return;
  }

  const std::time_t now = std::time(nullptr);
  if (now == static_cast<std::time_t>(-1))
  {
    return;
  }

  std::tm localTime{};
  if (localtime_s(&localTime, &now) != 0)
  {
    return;
  }

  _log_change_class_after_init_class query{};
  query.dwCharacSerial = m_dwSerial;
  query.byType = byType;
  std::strcpy(query.szPrevClassCode, szPrevClass);
  std::strcpy(query.szNextClassCode, m_AvatorData.dbAvator.m_szClassCode);
  query.nClassInitCnt = static_cast<int>(m_AvatorData.dbAvator.m_dwClassInitCnt);
  query.byLastClassGrade = m_AvatorData.dbAvator.m_byLastClassGrade;
  query.dwYear = static_cast<unsigned __int16>(localTime.tm_year + 1900);
  query.byMonth = static_cast<unsigned __int8>(localTime.tm_mon + 1);
  query.byDay = static_cast<unsigned __int8>(localTime.tm_mday);
  query.byHour = static_cast<unsigned __int8>(localTime.tm_hour);
  query.byMin = static_cast<unsigned __int8>(localTime.tm_min);
  query.bySec = static_cast<unsigned __int8>(localTime.tm_sec);
  g_Main.PushDQSData(
    m_dwAccountSerial,
    &m_idWorld,
    24,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));
}

void CUserDB::StartFieldMode()
{
  m_bField = true;
}


void CUserDB::Update_PotionNextUseTime(unsigned __int8 byPotionClass, unsigned int dwNextUseTime)
{
  if (byPotionClass < 38 && dwNextUseTime != static_cast<unsigned int>(-1))
  {
    m_AvatorData.dbPotionNextUseTime.dwPotionNextUseTime[byPotionClass] = dwNextUseTime;
  }
}


void CUserDB::Update_Post(
  unsigned int n,
  unsigned int dwSerial,
  int nNumber,
  unsigned __int8 byState,
  int nKey,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned __int64 lnUID)
{
  if (n < 50)
  {
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwPSSerial = dwSerial;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].nNumber = nNumber;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].byState = byState;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].nKey = nKey;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwDur = dwDur;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwUpt = dwUpt;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].dwGold = dwGold;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].lnUID = lnUID;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].bUpdate = 1;
    m_AvatorData.dbPostData.dbPost.m_bUpdate = 1;
    m_bDataUpdate = 1;
  }
}


void CUserDB::SendMsg_Inform_UILock()
{
  _uilock_inform_request_zocl payload{};
  payload.byInformType = static_cast<char>(m_byUILock);
  payload.byFailCount = static_cast<char>(m_byUILock_FailCnt);
  payload.byHintIndex = static_cast<char>(m_byUILock_HintIndex);
  payload.byFindPassFailCount = static_cast<char>(m_byUILockFindPassFailCount);

  unsigned __int8 type[2] = {13, static_cast<unsigned __int8>(-123)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}


bool CUserDB::Update_CopyAll(_AVATOR_DATA *pSrc)
{
  const unsigned __int64 currentCanonicalLastConnTime = m_dwCanonicalLastConnTime;
  _AVATOR_DATA snapshot{};
  snapshot = _AVATOR_DATA();
  std::memcpy(&snapshot, &this->m_AvatorData, sizeof(snapshot));
  std::memcpy(&this->m_AvatorData, pSrc, sizeof(this->m_AvatorData));
  std::strcpy(this->m_AvatorData.dbAvator.m_wszAvatorName, snapshot.dbAvator.m_wszAvatorName);
  this->m_AvatorData.dbAvator.m_dwRecordNum = snapshot.dbAvator.m_dwRecordNum;
  this->m_AvatorData.dbAvator.m_byRaceSexCode = snapshot.dbAvator.m_byRaceSexCode;
  this->m_AvatorData.dbAvator.m_bySlotIndex = snapshot.dbAvator.m_bySlotIndex;
  this->m_AvatorData.dbAvator.m_dwBaseShape = snapshot.dbAvator.m_dwBaseShape;
  this->m_AvatorData.dbAvator.m_dwLastConnTime = snapshot.dbAvator.m_dwLastConnTime;
  this->m_AvatorData.dbAvator.m_byMapCode = snapshot.dbAvator.m_byMapCode;
  std::memcpy(
    this->m_AvatorData.dbAvator.m_fStartPos,
    snapshot.dbAvator.m_fStartPos,
    sizeof(this->m_AvatorData.dbAvator.m_fStartPos));
  this->m_AvatorData.dbAvator.m_dwTotalPlayMin = snapshot.dbAvator.m_dwTotalPlayMin;
  this->m_AvatorData.dbAvator.m_dwStartPlayTime = snapshot.dbAvator.m_dwStartPlayTime;
  SetCanonicalLastConnTime(currentCanonicalLastConnTime);
  this->m_bNoneUpdateData = true;
  this->m_bDataUpdate = true;
  return true;
}


void CUserDB::Reged_Char_Complete(
  unsigned __int8 byRetCode,
  _REGED *pRegedList,
  _NOT_ARRANGED_AVATOR_DB *pArrangedList,
  const unsigned __int64 *pdwCanonicalLastConnTime)
{
  m_bDBWaitState = false;

  if (!byRetCode)
  {
    std::memcpy(m_RegedList, pRegedList, sizeof(m_RegedList));
    std::memset(m_dwCanonicalRegedLastConnTime, 0, sizeof(m_dwCanonicalRegedLastConnTime));
    for (int j = 0; j < 3; ++j)
    {
      _REGED *reged = &m_RegedList[j];
      if (m_RegedList[j].m_bySlotIndex != 255)
      {
        const unsigned __int64 canonicalLastConnTime =
          pdwCanonicalLastConnTime ? pdwCanonicalLastConnTime[j] : reged->m_dwLastConnTime;
        SetRegedCanonicalLastConnTime(static_cast<unsigned __int8>(j), canonicalLastConnTime);
        reged->UpdateEquipLv();
      }
    }
  }

  _reged_char_result_zone result;
  result.byRetCode = byRetCode;
  int charNum = 0;
  if (!byRetCode)
  {
    for (int k = 0; k < 3; ++k)
    {
      if (m_RegedList[k].m_bySlotIndex != 255)
      {
        std::memcpy(&result.RegedList[charNum], &m_RegedList[k], sizeof(result.RegedList[charNum]));
        if (result.RegedList[charNum].m_dwDalant > MAX_DALANT)
        {
          result.RegedList[charNum].m_dwDalant = MAX_DALANT;
        }
        if (result.RegedList[charNum].m_dwGold > MAX_GOLD)
        {
          result.RegedList[charNum].m_dwGold = MAX_GOLD;
        }
        if (!m_RegedList[k].m_byLevel)
        {
          result.RegedList[charNum].m_dwDalant = 0;
          result.RegedList[charNum].m_dwGold = 0;
        }

        if (IsExistRequestMoveCharacterList(pRegedList[k].m_dwRecordNum) == 1)
        {
          result.iLockType[k] = 1;
        }
        else
        {
          result.iLockType[k] = 2;
        }
        ++charNum;
      }
    }
    result.byCharNum = static_cast<unsigned __int8>(charNum);
  }

  unsigned __int8 type[2] = {1, 9};
  const unsigned __int16 len = result.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&result), len);

  std::memcpy(m_NotArrangedChar, pArrangedList, sizeof(m_NotArrangedChar));
  s_MgrLobbyHistory.reged_char_complete(byRetCode, charNum, pRegedList, m_szLobbyHistoryFileName);
  if (!byRetCode && m_bChatLock)
  {
    SetChatLock(true);
  }

  if (!byRetCode)
  {
    _not_arranged_char_inform_zocl info;
    charNum = 0;
    for (int m = 0; m < 50; ++m)
    {
      if (pArrangedList[m].dwSerial != static_cast<unsigned int>(-1))
      {
        std::memcpy(&info.CharList[charNum++], &pArrangedList[m], sizeof(info.CharList[0]));
      }
    }
    info.byCharNum = static_cast<unsigned __int8>(charNum);
    unsigned __int8 msgType[2] = {1, 21};
    const unsigned __int16 msgLen = info.size();
    g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, msgType, reinterpret_cast<char *>(&info), msgLen);
  }

  if (!m_byUserDgr)
  {
    SendMsg_Inform_UILock();
  }
}


void CUserDB::Delete_Char_Complete(unsigned __int8 byRetCode, unsigned __int8 bySlotIndex)
{
  m_bDBWaitState = false;
  m_dwOperLobbyTime = GetTickCount();

  if (!byRetCode)
  {
    m_RegedList[bySlotIndex].m_bySlotIndex = 255;
    if (bySlotIndex < 3u)
    {
      m_dwCanonicalRegedLastConnTime[bySlotIndex] = 0;
    }
  }

  s_MgrLobbyHistory.del_char_complete(byRetCode, m_szLobbyHistoryFileName);

  _del_char_result_zone result{};
  result.byRetCode = byRetCode;
  result.bySlotIndex = bySlotIndex;
  unsigned __int8 type[2] = {1, 13};
  const unsigned __int16 len = result.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&result), len);
}


void CUserDB::Select_Char_Complete(
  unsigned __int8 byRetCode,
  _AVATOR_DATA *pLoadData,
  bool *pbAddItem,
  unsigned int dwAddDalant,
  unsigned int dwAddGold,
  unsigned int /*dwCheckSum*/,
  bool *pbTrunkAddItem,
  unsigned __int8 byTrunkOldSlot,
  long double dTrunkOldDalant,
  long double dTrunkOldGold,
  bool bCreateTrunkFree,
  bool *pbExtTrunkAddItem,
  unsigned __int8 byExtTrunkOldSlot,
  unsigned __int64 dwCanonicalLastConnTime,
  const unsigned __int64 *pdwCanonicalUnitCutTime)
{
  m_bDBWaitState = false;
  const DWORD now = GetTickCount();
  m_dwOperLobbyTime = now;
  m_bCreateTrunkFree = bCreateTrunkFree;
  if (m_nTrans == 1)
  {
    m_bCreateTrunkFree = true;
  }

  unsigned __int8 slotIndex = 255;
  bool dataUpdated = false;
  if (!byRetCode)
  {
    std::memcpy(&m_AvatorData, pLoadData, sizeof(m_AvatorData));
    std::memcpy(&m_AvatorData_bk, pLoadData, sizeof(m_AvatorData_bk));
    const unsigned __int64 canonicalLastConnTime =
      dwCanonicalLastConnTime ? dwCanonicalLastConnTime : pLoadData->dbAvator.m_dwLastConnTime;
    SetCanonicalLastConnTime(canonicalLastConnTime);
    SetCanonicalBackupLastConnTime(canonicalLastConnTime);
    for (unsigned __int8 slotIndex = 0; slotIndex < 4u; ++slotIndex)
    {
      const unsigned __int64 canonicalUnitCutTime =
        pdwCanonicalUnitCutTime ? pdwCanonicalUnitCutTime[slotIndex] : pLoadData->dbUnit.m_List[slotIndex].dwCutTime;
      SetCanonicalUnitCutTime(slotIndex, canonicalUnitCutTime);
      SetCanonicalBackupUnitCutTime(slotIndex, canonicalUnitCutTime);
    }

    const unsigned int startCount = g_Main.m_dwStartNPCQuestCnt[m_AvatorData_bk.dbAvator.m_byRaceSexCode / 2];
    auto *startHistory = new (std::nothrow) _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY[startCount];
    m_AvatorData_bk.dbQuest.m_StartHistory = startHistory;
    std::memcpy(
      m_AvatorData_bk.dbQuest.m_StartHistory,
      m_AvatorData.dbQuest.m_StartHistory,
      89 * startCount);
    m_AvatorData_bk.dbQuest.dwListCnt = m_AvatorData.dbQuest.dwListCnt;

    if (pLoadData->dbCutting.m_bOldDataLoad)
    {
      m_AvatorData_bk.dbCutting.Init();
    }

    DataValidCheckRevise(&m_AvatorData, &dataUpdated);

    bool changed = false;
    if (pbAddItem)
    {
      const int maxSlots = 20 * pLoadData->dbAvator.m_byBagNum;
      for (int j = 0; j < maxSlots; ++j)
      {
        if (pbAddItem[j])
        {
          m_AvatorData_bk.dbInven.m_List[j].Key.SetRelease();
          changed = true;
        }
      }
    }

    if (dwAddDalant)
    {
      if (CanAddMoneyForMaxLimMoney(dwAddDalant, m_AvatorData.dbAvator.m_dwDalant))
      {
        m_AvatorData.dbAvator.m_dwDalant += dwAddDalant;
        changed = true;
      }
      else
      {
        g_Main.m_logSave.Write(
          "Push Dalant Fail >> Over Max : charserial( %d ), has( %d ), push( %d )",
          m_AvatorData.dbAvator.m_dwRecordNum,
          m_AvatorData.dbAvator.m_dwDalant,
          dwAddDalant);
      }
    }

    if (dwAddGold)
    {
      if (CanAddMoneyForMaxLimGold(dwAddGold, m_AvatorData.dbAvator.m_dwGold))
      {
        m_AvatorData.dbAvator.m_dwGold += dwAddGold;
        changed = true;
      }
      else
      {
        g_Main.m_logSave.Write(
          "Push Gold Fail >> Over max : charserial( %d ), has( %d\t, push( %d\t)",
          m_AvatorData.dbAvator.m_dwRecordNum,
          m_AvatorData.dbAvator.m_dwGold,
          dwAddGold);
      }
    }

    if (pbTrunkAddItem)
    {
      for (unsigned __int8 k = 0; k < pLoadData->dbTrunk.bySlotNum; ++k)
      {
        if (pbTrunkAddItem[k])
        {
          m_AvatorData_bk.dbTrunk.m_List[k].Key.SetRelease();
          changed = true;
        }
      }
    }

    if (pbExtTrunkAddItem)
    {
      for (unsigned __int8 m = 0; m < pLoadData->dbTrunk.byExtSlotNum; ++m)
      {
        if (pbExtTrunkAddItem[m])
        {
          m_AvatorData_bk.dbTrunk.m_ExtList[m].Key.SetRelease();
          changed = true;
        }
      }
    }

    if (byTrunkOldSlot != m_AvatorData.dbTrunk.bySlotNum)
    {
      changed = true;
    }
    if (dTrunkOldDalant != m_AvatorData.dbTrunk.dDalant)
    {
      changed = true;
    }
    if (dTrunkOldGold != m_AvatorData.dbTrunk.dGold)
    {
      changed = true;
    }
    if (byExtTrunkOldSlot != m_AvatorData.dbTrunk.byExtSlotNum)
    {
      changed = true;
    }

    m_AvatorData_bk.dbTrunk.bySlotNum = byTrunkOldSlot;
    m_AvatorData_bk.dbTrunk.dDalant = dTrunkOldDalant;
    m_AvatorData_bk.dbTrunk.dGold = dTrunkOldGold;
    m_AvatorData_bk.dbTrunk.byExtSlotNum = byExtTrunkOldSlot;

    std::strcpy(m_wszAvatorName, pLoadData->dbAvator.m_wszAvatorName);
    W2M(pLoadData->dbAvator.m_wszAvatorName, m_aszAvatorName, 17);
    m_byNameLen = static_cast<unsigned char>(std::strlen(m_wszAvatorName));
    m_tmrCheckPlayMin.TermTimeRun();
    m_dwSerial = pLoadData->dbAvator.m_dwRecordNum;
    slotIndex = pLoadData->dbAvator.m_bySlotIndex;

    bool firstStart = false;
    if (!m_AvatorData.dbAvator.m_byLevel)
    {
      firstStart = true;
      if (!FirstSettingData())
      {
        g_Main.m_logSystemError.Write(
          "FirstSettingData() == false : char: %s, class: %s",
          m_aszAvatorName,
          m_AvatorData.dbAvator.m_szClassCode);
      }
    }

    if (changed || dataUpdated)
    {
      UpdateContUserSave(true);
    }

    m_AvatorData.dbAvator.m_dwPvpRank = pLoadData->dbAvator.m_dwPvpRank;
    m_AvatorData.dbAvator.m_bOverlapVote = pLoadData->dbAvator.m_bOverlapVote;

    if (!g_Player[m_idWorld.wIndex].Load(this, firstStart))
    {
      char buffer[152]{};
      sprintf_s(buffer, "Close_Ae : %s, %s", m_szAccountID, m_aszAvatorName);
      g_Network.Close(0, m_idWorld.wIndex, false, buffer);
      return;
    }

    _select_avator_report_wrac report{};
    std::memcpy(&report, &m_gidGlobal, sizeof(report.gidGlobal));
    std::strcpy(report.wszCharName, m_wszAvatorName);
    report.wszCharName[16] = 0;
    report.dwAvatorSerial = m_dwSerial;
    report.byLevel = pLoadData->dbAvator.m_byLevel;
    unsigned __int8 type[2] = {1, 12};
    const unsigned __int16 len = report.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&report), len);

    m_dwTermContSaveTime = now;
    m_dwLastContSaveTime = now;

    if (g_Network.m_bUseFG)
    {
      const size_t nameLen = std::strlen(m_wszAvatorName);
      const unsigned int accLen = static_cast<unsigned int>(std::strlen(m_szAccountID));
      void *context = _CcrFG_rs_CreateUserContext(
        m_dwAccountSerial,
        reinterpret_cast<unsigned __int8 *>(m_szAccountID),
        accLen,
        reinterpret_cast<unsigned __int8 *>(m_wszAvatorName),
        static_cast<unsigned int>(nameLen),
        m_dwIP,
        &m_idWorld);
      if (context)
      {
        _socket *sock = g_Network.GetSocket(0, m_idWorld.wIndex);
        sock->m_hFGContext = context;
      }
    }
  }

  s_MgrLobbyHistory.sel_char_complete(byRetCode, &m_AvatorData, dwAddDalant, dwAddGold, m_szLobbyHistoryFileName);

  _sel_char_result_zone result{};
  result.byRetCode = byRetCode;
  result.bySlotIndex = slotIndex;
  result.dwWorldSerial = m_AvatorData.dbAvator.m_dwRecordNum;
  result.dwDalant = g_Player[m_idWorld.wIndex].m_Param.GetDalant();
  result.dwGold = g_Player[m_idWorld.wIndex].m_Param.GetGold();
  unsigned __int8 outType[2] = {1, 15};
  const unsigned __int16 outLen = result.size();
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, outType, reinterpret_cast<char *>(&result), outLen);
}

void CUserDB::WriteLog_CharSelect()
{
  _log_case_charselect logData{};
  std::strcpy(logData.szID, m_szAccountID);
  logData.dwIDSerial = m_dwAccountSerial;
  std::strcpy(logData.wszName, m_wszAvatorName);
  logData.dwNameSerial = m_dwSerial;

  __time64_t now = 0;
  _time64(&now);
  tm *localTime = _localtime64(&now);
  logData.dwYear = static_cast<unsigned __int16>(localTime->tm_year + 1900);
  logData.byMonth = static_cast<unsigned __int8>(localTime->tm_mon + 1);
  logData.byDay = static_cast<unsigned __int8>(localTime->tm_mday);
  logData.byHour = static_cast<unsigned __int8>(localTime->tm_hour);
  logData.byMin = static_cast<unsigned __int8>(localTime->tm_min);
  logData.bySec = static_cast<unsigned __int8>(localTime->tm_sec);

  const int size = static_cast<int>(logData.size());
  g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 10, logData.szID, size);
}

bool CUserDB::Update_CombineExResult_Push(_ITEMCOMBINE_DB_BASE *pItemCombineDB_IN)
{
  std::memcpy(&m_AvatorData.dbItemCombineEx, pItemCombineDB_IN, sizeof(m_AvatorData.dbItemCombineEx));
  m_bDataUpdate = 1;
  return true;
}

bool CUserDB::Update_CombineExResult_Pop()
{
  m_AvatorData.dbItemCombineEx.Init();
  m_bDataUpdate = 1;
  return true;
}

bool CUserDB::Update_TakeLastCriTicket(unsigned int dwCriTicket)
{
  m_AvatorData.dbAvator.m_dwTakeLastCriTicket = dwCriTicket;
  m_bDataUpdate = 1;
  return true;
}

