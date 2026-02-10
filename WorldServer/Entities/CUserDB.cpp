#include "pch.h"

#include "CUserDB.h"

#include <cstring>

#include "CMapOperation.h"
#include "CNationSettingManager.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "CLogFile.h"
#include "CNetProcess.h"
#include "CNetworkEX.h"
#include "add_char_result_zone.h"
#include "alive_char_result_zocl.h"
#include "enter_lobby_report_wrac.h"
#include "enter_world_request_wrac.h"
#include "enter_world_result_zone.h"
#include "chat_lock_inform_zocl.h"
#include "del_char_result_zone.h"
#include "moveout_user_result_zone.h"
#include "qry_case_alive_char.h"
#include "qry_sheet_delete.h"
#include "qry_sheet_insert.h"
#include "qry_sheet_load.h"
#include "qry_sheet_lobby.h"
#include "qry_sheet_reged.h"
#include "sel_char_result_zone.h"
#include "server_notify_inform_zone.h"
#include "wrac_packets.h"
#include "qry_logout.h"
#include "exit_alter_param.h"
#include "RFEvent_ClassRefine.h"
#include "CNetIndexList.h"

int CUserDB::s_nLoginNum = 0;
CLogFile CUserDB::s_logAvatorDB;
CMgrAccountLobbyHistory CUserDB::s_MgrLobbyHistory{};
_MOVE_LOBBY_DELAY CUserDB::s_MoveLobbyDelay{};

const char wszNonMakeName_0[3][17] = {"GM", "ADMIN", "OPERATOR"};
const char wszNonMakeName_1[3][17] = {"GM", "ADMIN", "OPERATOR"};
const char wszNonMakeName[3][17] = {"GM", "ADMIN", "OPERATOR"};
const char wszGMCmp[] = "GM";
size_t nGMCmpLen = strlen_0(wszGMCmp);

CUserDB *SearchAvatorWithName(CUserDB *pList, int nMax, char *pwszName)
{
  const unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(pwszName));
  for (int index = 0; index < nMax; ++index)
  {
    CUserDB &entry = pList[index];
    if (entry.m_bActive
        && entry.m_bField
        && entry.m_byNameLen == nameLen
        && !strncmp_0(entry.m_wszAvatorName, pwszName, nameLen))
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
  m_tmrCheckPlayMin.BeginTimer(0xEA60);
}

void CUserDB::ParamInit()
{
  m_idWorld.dwSerial = static_cast<unsigned int>(-1);
  m_gidGlobal.dwSerial = static_cast<unsigned int>(-1);
  m_AvatorData.InitData();
  m_AvatorData_bk.InitData();
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
  m_byUILock = 0xFF;
  m_byUILock_InitFailCnt = 0xFF;
  m_byUILock_FailCnt = 0xFF;
  m_szUILock_PW[0] = '\0';
  m_szAccount_PW[0] = '\0';
  m_byUILock_HintIndex = 0xFF;
  m_uszUILock_HintAnswer[0] = '\0';
  m_byUILock_InitFindPassFailCount = 0xFF;
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

char CUserDB::Update_Bind(char *pszMapCode, char *pDummyCode, bool bUpdate)
{
  if (strlen_0(pszMapCode) > 0xBu)
  {
    return 0;
  }
  if (strlen_0(pDummyCode) > 0xBu)
  {
    return 0;
  }
  strcpy_0(m_AvatorData.dbAvator.m_szBindMapCode, pszMapCode);
  strcpy_0(m_AvatorData.dbAvator.m_szBindDummy, pDummyCode);
  (void)bUpdate;
  return 1;
}

void CUserDB::Update_PvpPointLeak(long double dValue)
{
  m_AvatorData.dbSupplement.dPvpPointLeak = dValue;
}

void CUserDB::Update_LastAttBuff(bool bSet)
{
  m_AvatorData.dbSupplement.bLastAttBuff = bSet;
}

bool CUserDB::Update_Stat(unsigned __int8 byStatIndex, unsigned int dwNewCum, bool bUpdate)
{
  (void)bUpdate;
  if (byStatIndex < 0x50u)
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

char CUserDB::Update_QuestInsert(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData)
{
  if (bySlotIndex < 0x1Eu)
  {
    if (m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
    {
      memcpy_0(
        &m_AvatorData.dbQuest.m_List[bySlotIndex],
        pSlotData,
        sizeof(m_AvatorData.dbQuest.m_List[bySlotIndex]));
      m_bDataUpdate = true;
      return 1;
    }

    CLogFile::Write(
      &g_Main.m_logSystemError,
      "%s : Update_QuestInsert(EXIST) : slot : %d",
      m_aszAvatorName,
      bySlotIndex);
    return 0;
  }

  CLogFile::Write(
    &g_Main.m_logSystemError,
    "%s : Update_QuestInsert(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return 0;
}

char CUserDB::Update_QuestDelete(unsigned __int8 bySlotIndex)
{
  if (bySlotIndex < 0x1Eu)
  {
    if (m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "%s : Update_QuestDelete(EXIST) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return 0;
    }

    m_AvatorData.dbQuest.m_List[bySlotIndex].Init();
    m_bDataUpdate = true;
    return 1;
  }

  CLogFile::Write(
    &g_Main.m_logSystemError,
    "%s : Update_QuestDelete(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return 0;
}

char CUserDB::Update_QuestUpdate(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData, bool bUpdate)
{
  if (bySlotIndex < 0x1Eu)
  {
    if (m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "%s : Update_QuestUpdate(NOTHING) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return 0;
    }

    memcpy_0(
      &m_AvatorData.dbQuest.m_List[bySlotIndex],
      pSlotData,
      sizeof(m_AvatorData.dbQuest.m_List[bySlotIndex]));
    if (bUpdate)
    {
      m_bDataUpdate = true;
    }
    return 1;
  }

  CLogFile::Write(
    &g_Main.m_logSystemError,
    "%s : Update_QuestUpdate(SlotIndex OVER) : slot : %d",
    m_aszAvatorName,
    bySlotIndex);
  return 0;
}

char CUserDB::Update_NPCQuestHistory(unsigned __int8 byIndex, _QUEST_DB_BASE::_NPC_QUEST_HISTORY *pHisData)
{
  if (byIndex < 0x46u)
  {
    memcpy_0(
      &m_AvatorData.dbQuest.m_History[byIndex],
      pHisData,
      sizeof(m_AvatorData.dbQuest.m_History[byIndex]));
    return 1;
  }

  CLogFile::Write(
    &g_Main.m_logSystemError,
    "%s : Update_NPCQuestHistory(Index OVER) : %d",
    m_aszAvatorName,
    byIndex);
  return 0;
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
  sprintf(buffer, "CLOSE>> ForceCloseCommand Type: %d, ID: %s Reason : ", byKickType, m_szAccountID);
  if (pszCause)
  {
    strcat_0(buffer, pszCause);
  }
  g_Network.Close(0, m_idWorld.wIndex, bSlow, buffer);
}

void CUserDB::ClearBillingData()
{
  memset_0(&m_BillingInfo, 0, sizeof(m_BillingInfo));
}

void CUserDB::SetBillingData(_BILLING_INFO *pBillingInfo)
{
  if (pBillingInfo)
  {
    memcpy_0(&m_BillingInfo, pBillingInfo, sizeof(m_BillingInfo));
  }
  m_BillingInfo.bPCCheat = 0;
}

void CUserDB::SetBillingData(char *szCMSCode, __int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate)
{
  m_BillingInfo.iType = iType;
  m_BillingInfo.lRemainTime = lRemainTime;
  if (iType == 6 || iType == 7)
  {
    memcpy_0(m_BillingInfo.szCMS, szCMSCode, sizeof(m_BillingInfo.szCMS));
  }
  else
  {
    memset_0(m_BillingInfo.szCMS, 0, sizeof(m_BillingInfo.szCMS));
  }
  if (pstEndDate)
  {
    memcpy_0(&m_BillingInfo.stEndDate, pstEndDate, sizeof(m_BillingInfo.stEndDate));
  }
  if (!CMainThread::IsReleaseServiceMode(&g_Main) && m_BillingInfo.bPCCheat)
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
  struct _billing_msg
  {
    __int16 iType;
    int lRemainTime;
    _SYSTEMTIME stEndDate;
  };

  _billing_msg msg{};
  msg.iType = m_BillingInfo.iType;
  msg.lRemainTime = m_BillingInfo.lRemainTime;
  memcpy_0(&msg.stEndDate, &m_BillingInfo.stEndDate, sizeof(msg.stEndDate));

  unsigned __int8 pbyType[2]{29, 2};
  CNetProcess::LoadSendMsg(
    g_Network.m_pProcess[0],
    m_idWorld.wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0x16u);
}

void CUserDB::SetRemainTime(int lRemainTime)
{
  m_BillingInfo.lRemainTime = lRemainTime;
}

void CUserDB::UILockInfo_Init(char *pMsg)
{
  if (!pMsg)
  {
    return;
  }

  char msg[2]{};
  if (m_byUILock)
  {
    msg[0] = 11;
  }
  else if (*pMsg)
  {
    if (*pMsg == 1)
    {
      msg[0] = 11;
    }
    else if (*pMsg == 2)
    {
      msg[0] = 12;
    }
    else
    {
      msg[0] = 15;
    }
  }
  else
  {
    msg[0] = 0;
    msg[1] = pMsg[16];
    m_byUILock = 1;
    strcpy_0(m_szUILock_PW, pMsg + 3);
    m_byUILock_HintIndex = pMsg[16];
    strcpy_0(m_uszUILock_HintAnswer, pMsg + 17);
  }

  unsigned __int8 pbyType[2]{13, 0x80};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_idWorld.wIndex, pbyType, msg, 2u);
}

void CUserDB::UILockInfo_Update(char *pMsg)
{
  if (!pMsg)
  {
    return;
  }

  char msg[1]{};
  if (m_byUILock >= 2u)
  {
    if (*pMsg)
    {
      if (*pMsg == 1)
      {
        msg[0] = 10;
      }
      else if (*pMsg == 2)
      {
        msg[0] = 12;
      }
      else
      {
        msg[0] = 8;
      }
    }
    else
    {
      msg[0] = 0;
      m_byUILock = 2;
      strcpy_0(m_szUILock_PW, pMsg + 3);
      m_byUILock_HintIndex = pMsg[16];
      strcpy_0(m_uszUILock_HintAnswer, pMsg + 17);
    }
  }
  else
  {
    msg[0] = 7;
  }

  unsigned __int8 pbyType[2]{13, static_cast<unsigned __int8>(-124)};
  CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_idWorld.wIndex, pbyType, msg, 1u);
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
    CNetProcess::LoadSendMsg(
      g_Network.m_pProcess[0],
      m_idWorld.wIndex,
      pbyType,
      reinterpret_cast<char *>(&msg.bLock),
      len);
  }
}

bool CUserDB::Update_AlterPvPPoint(long double dNewPoint)
{
  m_AvatorData.dbAvator.m_dPvPPoint = dNewPoint;
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

char CUserDB::Update_ItemDur(unsigned __int8 storage, unsigned __int8 slot, unsigned __int64 amount, bool /*bUpdate*/)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_ItemDur(CODE) : scode : %d, icode : %d  ",
      m_aszAvatorName,
      storage,
      slot);
    return 0;
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
          return 0;
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
          return 0;
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
          return 0;
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
          return 0;
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
          return 0;
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
          return 0;
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
      return 0;
    }
    m_AvatorData.dbInven.m_List[slot].dwDur = amount;
  }

  m_bDataUpdate = true;
  return 1;
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

bool CUserDB::Update_UnitData(unsigned __int8 bySlotIndex, _UNIT_DB_BASE::_LIST *pData)
{
  if (bySlotIndex < 4)
  {
    if (m_AvatorData.dbUnit.m_List[bySlotIndex].byFrame == 0xFF)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_UnitData(EXIST) : slot : %d",
        m_aszAvatorName,
        bySlotIndex);
      return false;
    }
    memcpy_0(&m_AvatorData.dbUnit.m_List[bySlotIndex], pData, sizeof(m_AvatorData.dbUnit.m_List[bySlotIndex]));
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

__int64 _INVENKEY::CovDBKey()
{
  return static_cast<__int64>(*reinterpret_cast<unsigned int *>(this));
}

bool _INVENKEY::IsFilled()
{
  return bySlotIndex != 0xFF || byTableCode != 0xFF || wItemIndex != 0xFFFF;
}

void _INVENKEY::SetRelease()
{
  bySlotIndex = 0xFF;
  byTableCode = 0xFF;
  wItemIndex = 0xFFFF;
}

bool _EMBELLKEY::IsFilled()
{
  return bySlotIndex != 0xFF || byTableCode != 0xFF || wItemIndex != 0xFFFF;
}

void _EMBELLKEY::SetRelease()
{
  bySlotIndex = 0xFF;
  byTableCode = 0xFF;
  wItemIndex = 0xFFFF;
}

void _EMBELLKEY::LoadDBKey(_EMBELLKEY key)
{
  *this = key;
}

__int64 _EMBELLKEY::CovDBKey()
{
  const unsigned int packed =
    static_cast<unsigned int>(bySlotIndex)
    | (static_cast<unsigned int>(byTableCode) << 8)
    | (static_cast<unsigned int>(wItemIndex) << 16);
  return static_cast<__int64>(packed);
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

__int64 _FORCEKEY::CovDBKey()
{
  return static_cast<__int64>(dwKey);
}

bool _LINKKEY::IsFilled()
{
  return wEffectCode != 0xFFFF;
}

void _LINKKEY::SetRelease()
{
  wEffectCode = 0xFFFF;
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

unsigned __int16 _LINKKEY::CovDBKey()
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
  return byItemIndex != 0xFF;
}

void _ANIMUSKEY::SetRelease()
{
  byItemIndex = 0xFF;
}

void _ANIMUSKEY::LoadDBKey(unsigned __int8 key)
{
  byItemIndex = key;
}

unsigned __int8 _ANIMUSKEY::CovDBKey()
{
  return byItemIndex;
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
      0x129u,
      &m_AvatorData.dbPostData.dbPost.m_PostList[n],
      0x129u);
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
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszSendName, 0x11u, wszSendName);
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszRecvName, 0x11u, wszRecvName);
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszTitle, 0x15u, wszTitle);
    strcpy_s(m_AvatorData.dbPostData.dbPost.m_PostList[n].wszContent, 0xC9u, wszContent);
    m_AvatorData.dbPostData.dbPost.m_PostList[n].bNew = 1;
    m_AvatorData.dbPostData.dbPost.m_PostList[n].bUpdate = 1;
    m_AvatorData.dbPostData.dbPost.m_bUpdate = 1;
    m_bDataUpdate = 1;
  }
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
    memset_0(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszSendName,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszSendName));
    memset_0(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszRecvName,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszRecvName));
    memset_0(
      m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszTitle,
      0,
      sizeof(m_AvatorData.dbPostData.dbPost.m_PostList[dwIndex].wszTitle));
    memset_0(
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

char _PCBANG_FAVOR_ITEM_DB_BASE::InsertItem(_STORAGE_LIST::_db_con *Item)
{
  for (int index = 0; index < 50; ++index)
  {
    if (lnUID[index] == static_cast<unsigned __int64>(-1))
    {
      lnUID[index] = Item->m_lnUID;
      return 1;
    }
  }
  return 0;
}

char _PCBANG_FAVOR_ITEM_DB_BASE::IsDeleteItem(_STORAGE_LIST::_db_con *Item)
{
  for (int index = 0; index < 50; ++index)
  {
    if (lnUID[index] != static_cast<unsigned __int64>(-1) && lnUID[index] == Item->m_lnUID)
    {
      return 1;
    }
  }
  return 0;
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
  if (pItem->m_wItemIndex > 0xFFu)
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
    m_dwFixEquipLv[j] = 0x0FFFFFFF;
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

char _SYNC_STATE::chk_enter()
{
  if (bEnter)
  {
    return 0;
  }
  bEnter = true;
  return 1;
}

char _SYNC_STATE::chk_reged()
{
  if (bReged)
  {
    return 0;
  }
  bReged = true;
  return 1;
}

char _SYNC_STATE::chk_select()
{
  if (bSelect)
  {
    return 0;
  }
  bSelect = true;
  return 1;
}

void _AVATOR_DATA::InitData()
{
  std::memset(this, 0, sizeof(_AVATOR_DATA));
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
  if (m_byUILock != 0xFF
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
    CLogFile::Write(&g_Main.m_logSystemError, "%d.. Enter_Account : m_bActive == true", dwAccountSerial);
    return false;
  }
  if (!m_ss.chk_enter())
  {
    CLogFile::Write(&g_Main.m_logSystemError, "%d.. Enter_Account : if(!m_ss.chk_enter())", dwAccountSerial);
    return false;
  }

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

  if (!waitData)
  {
    CLogFile::Write(&g_Main.m_logSystemError, "%d.. Enter_Account : pWaitData == NULL", dwAccountSerial);
    return false;
  }

  bool valid = true;
  if (!waitData->m_byUserDgr && dwProtocolVer != 126455)
  {
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "%s.. Enter_Account : if(dwProtocolVer != __PROTOCOL_VER)",
      waitData->m_szAccountID);
    valid = false;
  }
  if (g_Player[m_idWorld.wIndex].m_bLoad)
  {
    CLogFile::Write(
      &g_Main.m_logSystemError,
      "%s.. Enter_Account : if(g_Player[m_idWorld.wIndex].m_bLoad)",
      waitData->m_szAccountID);
    valid = false;
  }
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CUserDB *user = &g_UserDB[j];
    if (user->m_bActive && dwAccountSerial == user->m_dwAccountSerial)
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "%s.. Enter_Account : if(dwAccountSerial == p->m_dwAccountSerial)",
        waitData->m_szAccountID);
      valid = false;
    }
  }

  if (!valid)
  {
    _logout_account_request_wrac msg{};
    memcpy_0(&msg, &m_gidGlobal, sizeof(msg));
    unsigned __int8 type[2]{1, 5};
    const unsigned __int16 len = msg.size();
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&msg), len);
    return false;
  }

  m_bActive = true;
  m_dwOperLobbyTime = timeGetTime();
  m_dwAccountSerial = dwAccountSerial;
  strcpy_0(m_szAccountID, waitData->m_szAccountID);
  m_byUserDgr = waitData->m_byUserDgr;
  m_bySubDgr = waitData->m_bySubDgr;
  m_dwIP = dwIP;
  memcpy_0(&m_gidGlobal, &waitData->m_gidGlobal, sizeof(m_gidGlobal));
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
      CLogFile::Write(&g_Main.m_logSystemError, "%s.. Enter_Account Billing Type = 0", waitData->m_szAccountID);
      return false;
    }
    if (!m_BillingInfo.lRemainTime && (m_BillingInfo.iType == 5 || m_BillingInfo.iType == 2))
    {
      CLogFile::Write(
        &g_Main.m_logSystemError,
        "%s.. Enter_Account Billing lRemainTime = 0",
        waitData->m_szAccountID);
      return false;
    }

    m_byUILock = waitData->m_byUILock;
    m_byUILock_InitFailCnt = waitData->m_byUILock_FailCnt;
    m_byUILock_FailCnt = m_byUILock_InitFailCnt;
    m_byUILock_HintIndex = waitData->m_byUILock_HintIndex;
    strcpy_s(m_szUILock_PW, 0xDuLL, waitData->m_szUILock_PW);
    strcpy_s(m_szAccount_PW, 0xDuLL, waitData->m_szAccount_PW);
    strcpy_s(m_uszUILock_HintAnswer, 0x11uLL, waitData->m_uszUILock_HintAnswer);
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
    memcpy_0(&request, &m_gidGlobal, 8u);
    memcpy_0(&request.idLocal, &m_idWorld, sizeof(request.idLocal));
    request.ulConnectIP = m_ipAddress;
    unsigned __int8 type[2]{1, 10};
    const unsigned __int16 len = static_cast<unsigned __int16>(request.size());
    g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&request), len);
  }

  s_MgrLobbyHistory.GetNewFileName(m_dwAccountSerial, m_szLobbyHistoryFileName);
  s_MgrLobbyHistory.enter_lobby(
    m_dwAccountSerial,
    m_szAccountID,
    m_byUserDgr,
    m_dwIP,
    true,
    m_szLobbyHistoryFileName);

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
  memcpy_0(&qry.NewData, &m_AvatorData, sizeof(qry.NewData));

  _AVATOR_DATA *contData = IsContPushBefore();
  if (contData)
  {
    memcpy_0(&qry.OldData, contData, sizeof(qry.OldData));
  }
  else
  {
    memcpy_0(&qry.OldData, &m_AvatorData_bk, sizeof(qry.OldData));
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
    m_dwOperLobbyTime = timeGetTime();
    m_bDBWaitState = true;
    return true;
  }

  Exit_Account_Complete(0x64u);
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
  strcpy_0(qry.in_szIP, ipText);

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
  if (m_RegedList[bySlotIndex].m_bySlotIndex != 0xFF)
  {
    return false;
  }

  for (char *cursor = pwszCharName; ; ++cursor)
  {
    if (*cursor == 32 || *cursor == 39)
    {
      goto invalid_name;
    }
    if (!*cursor)
    {
      break;
    }
  }
  if (*pwszCharName == 42)
  {
    goto invalid_name;
  }
  for (int k = 0; k < 3; ++k)
  {
    if (!strcmp_0(pwszCharName, wszNonMakeName_0[k]))
    {
      goto invalid_name;
    }
  }

  {
    CNationSettingManager *manager = CTSingleton<CNationSettingManager>::Instance();
    if ((m_byUserDgr != 2 && strlen_0(pwszCharName) >= nGMCmpLen
         && !strncmp(pwszCharName, wszGMCmp, nGMCmpLen))
        || !manager->IsNormalString(pwszCharName))
    {
      goto invalid_name;
    }
  }

  if (IsSQLValidString(pwszCharName))
  {
    _base_fld *record = CRecordData::GetRecord(&g_Main.m_tblClass, pszClassCode);
    if (!record)
    {
      return false;
    }
    if (*reinterpret_cast<unsigned int *>(&record[1].m_strCode[8]))
    {
      return false;
    }
    if (record[1].m_dwIndex == static_cast<int>(byRaceSexCode) >> 1)
    {
      s_MgrLobbyHistory.add_char_request(m_szLobbyHistoryFileName);
      _qry_sheet_insert qry;
      qry.dwAccountSerial = m_dwAccountSerial;
      strcpy_0(qry.szAccountID, m_szAccountID);
      strcpy_0(qry.InsertData.m_wszAvatorName, pwszCharName);
      qry.InsertData.m_byRaceSexCode = byRaceSexCode;
      qry.InsertData.m_bySlotIndex = bySlotIndex;
      qry.InsertData.m_dwBaseShape = dwBaseShape;
      strcpy_0(qry.InsertData.m_szClassCode, record->m_strCode);
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

  CLogFile::Write(
    &g_Main.m_logSystemError,
    "CUserDB::Insert_Char_Request() : Account : %s(%u) ::IsSQLValidString(pwszCharName(%s)) Invalid!",
    m_szAccountID,
    m_dwAccountSerial,
    pwszCharName);
  Insert_Char_Complete(0x2Fu, nullptr);
  return true;

invalid_name:
  Insert_Char_Complete(0x2Fu, nullptr);
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
  if (m_RegedList[bySlotIndex].m_bySlotIndex == 0xFF)
  {
    return false;
  }

  unsigned __int8 exist = IsExistRequestMoveCharacterList(m_RegedList[bySlotIndex].m_dwRecordNum);
  if (exist)
  {
    _del_char_result_zone result{};
    memset_0(&result, 0, sizeof(result));
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
  if (m_RegedList[bySlotIndex].m_bySlotIndex == 0xFF)
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
    memset_0(&result, 0, sizeof(result));
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
  _qry_sheet_load qry;
  qry.dwAvatorSerial = m_RegedList[bySlotIndex].m_dwRecordNum;
  qry.dwCheckSum = 0;
  memcpy_0(&qry.LoadData, &m_RegedList[bySlotIndex], 0x10DuLL);
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
  if (m_RegedList[bySlotIndex].m_bySlotIndex != 0xFF)
  {
    Alive_Char_Complete(0x32u, byCase, dwSerial, nullptr);
    return false;
  }

  if (!byCase)
  {
    goto check_not_arranged;
  }

  bool hasArranged = false;
  for (int j = 0; j < 50; ++j)
  {
    if (m_dwArrangePassCase0[j] == dwSerial)
    {
      hasArranged = true;
      break;
    }
  }
  if (hasArranged)
  {
    goto check_not_arranged;
  }

  Alive_Char_Complete(0x32u, byCase, dwSerial, nullptr);
  return false;

check_not_arranged:
  {
    bool found = false;
    for (int k = 0; k < 50; ++k)
    {
      if (m_NotArrangedChar[k].dwSerial != static_cast<unsigned int>(-1)
          && m_NotArrangedChar[k].dwSerial == dwSerial)
      {
        if (byCase)
        {
          if (!strcmp_0(m_NotArrangedChar[k].wszName, pwszName))
          {
            break;
          }
        }
        else if (strcmp_0(m_NotArrangedChar[k].wszName, pwszName))
        {
          break;
        }
        found = true;
        break;
      }
    }
    if (!found)
    {
      Alive_Char_Complete(0x39u, byCase, dwSerial, nullptr);
      return false;
    }
  }

  for (char *cursor = pwszName; ; ++cursor)
  {
    if (*cursor == 32 || *cursor == 39)
    {
      Alive_Char_Complete(0x2Fu, byCase, dwSerial, nullptr);
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
    strcpy_0(qry.in_w_szName, pwszName);
    qry.in_bySlot = bySlotIndex;
    const int size = static_cast<int>(qry.size());
    if (g_Main.PushDQSData(m_dwAccountSerial, &m_idWorld, 0x17u, reinterpret_cast<char *>(&qry), size))
    {
      m_bDBWaitState = true;
      return true;
    }
    return false;
  }

  Insert_Char_Complete(0x2Fu, nullptr);
  return true;
}

void CUserDB::Insert_Char_Complete(unsigned __int8 byRetCode, _REGED_AVATOR_DB *pInsertData)
{
  m_bDBWaitState = false;
  m_dwOperLobbyTime = timeGetTime();

  unsigned __int8 slotIndex = static_cast<unsigned __int8>(-1);
  if (!byRetCode && pInsertData)
  {
    slotIndex = pInsertData->m_bySlotIndex;
    memcpy_0(&m_RegedList[slotIndex], pInsertData, 0x45u);
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

  if (!memcmp_0(&m_idWorld, &m_pDBPushData->m_idWorld, sizeof(_CLID)))
  {
    char *data = m_pDBPushData->m_sData;
    if (*reinterpret_cast<unsigned int *>(data) == m_dwSerial)
    {
      return reinterpret_cast<_AVATOR_DATA *>(data + 4);
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
    if (m_dwSerial == static_cast<unsigned int>(-1))
    {
      _qry_case_lobby_logout qry{};
      qry.dwAccountSerial = m_dwAccountSerial;
      strcpy_s(qry.szLobbyHistoryFileName, sizeof(qry.szLobbyHistoryFileName), m_szLobbyHistoryFileName);
      const int size = static_cast<int>(qry.size());
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0xABu, reinterpret_cast<char *>(&qry), size);
      Exit_Account_Complete(0);
    }
    else
    {
      _qry_sheet_logout qry{};
      qry.dwAvatorSerial = m_dwSerial;
      memcpy_0(&qry.NewData, &m_AvatorData, sizeof(qry.NewData));

      _AVATOR_DATA *contData = IsContPushBefore();
      if (contData)
      {
        memcpy_0(&qry.OldData, contData, sizeof(qry.OldData));
      }
      else
      {
        memcpy_0(&qry.OldData, &m_AvatorData_bk, sizeof(qry.OldData));
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
        Exit_Account_Complete(0x64u);
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
  m_bDBWaitState = false;

  _logout_account_request_wrac msg{};
  memcpy_0(&msg, &m_gidGlobal, sizeof(msg));
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

  s_MgrLobbyHistory.enter_lobby(
    m_dwAccountSerial,
    m_szAccountID,
    m_byUserDgr,
    m_dwIP,
    0,
    m_szLobbyHistoryFileName);

  _enter_lobby_report_wrac report{};
  memcpy_0(&report, &m_gidGlobal, sizeof(report));
  unsigned __int8 type[2] = {1, 13};
  const unsigned __int16 len = report.size();
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, reinterpret_cast<char *>(&report), len);
}

void CUserDB::Cont_UserSave_Complete(unsigned __int8 byResult, _AVATOR_DATA *pAvatorData)
{
  if (byResult)
  {
    g_Network.Close(0, m_idWorld.wIndex, true, "Cont Save Error");
    return;
  }

  _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY *startHistory = m_AvatorData_bk.dbQuest.m_StartHistory;
  unsigned int listCount = m_AvatorData_bk.dbQuest.dwListCnt;
  memcpy_0(&m_AvatorData_bk, pAvatorData, sizeof(m_AvatorData_bk));
  m_AvatorData_bk.dbQuest.m_StartHistory = startHistory;
  m_AvatorData_bk.dbQuest.dwListCnt = listCount;

  const unsigned int addCount = m_AvatorData.dbQuest.dwListCnt - m_AvatorData_bk.dbQuest.dwListCnt;
  for (unsigned int j = 0; j < addCount; ++j)
  {
    memcpy_0(
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
  _REGED *pAliveAvator)
{
  m_bDBWaitState = false;

  unsigned __int8 resultCode = byRetCode;
  unsigned __int8 slotIndex = 0xFF;
  if (!byRetCode)
  {
    slotIndex = pAliveAvator->m_bySlotIndex;
    if (slotIndex < 3u)
    {
      if (m_RegedList[slotIndex].m_bySlotIndex == 0xFF)
      {
        memcpy_0(&m_RegedList[slotIndex], pAliveAvator, sizeof(m_RegedList[slotIndex]));
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
    memcpy_0(&msg.AliveChar, &m_RegedList[slotIndex], sizeof(msg.AliveChar));
  }

  unsigned __int8 type[2] = {1, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(m_idWorld.wIndex, type, reinterpret_cast<char *>(&msg), 0x4Au);
}

char CUserDB::Update_Param(_EXIT_ALTER_PARAM *pCon)
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

  return 1;
}

char CUserDB::CheckDQSLoadCharacterData(_AVATOR_DATA *pData)
{
  int maxLevel = pData->dbAvator.m_byMaxLevel ? pData->dbAvator.m_byMaxLevel : 50;
  if (pData->dbAvator.m_byLevel <= maxLevel)
  {
    if (pData->dbAvator.m_byRaceSexCode < 5u)
    {
      return 1;
    }

    static const char kDataValidCheckRace[] = "D_DataValidCheck (Serial:%d) Race:%d";
    CUserDB::s_logAvatorDB.Write(
      kDataValidCheckRace,
      pData->dbAvator.m_dwRecordNum,
      pData->dbAvator.m_byRaceSexCode);
    return 0;
  }

  static const char kDataValidCheckLevel[] = "D_DataValidCheck (Serial:%d) Level:%d Max:%d";
  CUserDB::s_logAvatorDB.Write(
    kDataValidCheckLevel,
    pData->dbAvator.m_dwRecordNum,
    pData->dbAvator.m_byLevel,
    maxLevel);
  return 0;
}

void CUserDB::ReRangeClientIndex(_AVATOR_DATA *pData)
{
  CNetIndexList list;
  list.SetList(0x64u);

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

