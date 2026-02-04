#include "pch.h"

#include "CPlayer.h"
#include "CRecordData.h"
#include "CMapData.h"
#include "CLevel.h"
#include "CTransportShip.h"
#include "CUserDB.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "CNuclearBombMgr.h"
#include "CParkingUnit.h"
#include "LendItemMng.h"
#include "TimeLimitJadeMng.h"
#include "ResourceItem_fld.h"
#include "WorldServerUtil.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"

#include <ctime>
#include <cstdlib>

_skill_fld *CPlayer::ms_pXmas_Snow_Effect = nullptr;
_skill_fld *CPlayer::ms_pXmas_Snow_Bullet_Effect = nullptr;
CMgrAvatorItemHistory CPlayer::s_MgrItemHistory{};
CMgrAvatorLvHistory CPlayer::s_MgrLvHistory{};
int CPlayer::s_nLiveNum = 0;
int _ATTACK_DELAY_CHECKER::s_nSpareTime = 0;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff;
CRecordData ItemCombineMgr::ms_tbl_ItemCombine_Link_Result;

namespace
{
  const char *sType[10] = {
    "kill_s_inc",
    "kill_p_inc",
    "die_dec",
    "quest_inc",
    "logoff_inc",
    "logoff_dec",
    "holy_award",
    "holy_dec",
    "cheat",
    "guildbattle",
  };
}

bool ItemCombineMgr::LoadData()
{
  char szMessage[144] = {};
  if (ItemCombineMgr::ms_tbl_ItemCombine.ReadRecord_Ex(
        ".\\script\\CombineTable.dat",
        ".\\script\\CombineTable2.dat",
        0x3C8u,
        szMessage))
  {
    if (ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff.ReadRecord(
          ".\\script\\LinkedStuff.dat",
          0x1944u,
          szMessage))
    {
      if (ItemCombineMgr::ms_tbl_ItemCombine_Link_Result.ReadRecord(
            ".\\script\\LinkedResult.dat",
            0x1944u,
            szMessage))
      {
        return true;
      }
    }
  }

  MyMessageBox("DatafileInit", szMessage);
  return false;
}

bool ItemCombineMgr::CheckLoadData()
{
  for (int n = 0; ; ++n)
  {
    const int recordNum = static_cast<int>(ItemCombineMgr::ms_tbl_ItemCombine.GetRecordNum());
    if (n >= recordNum)
    {
      break;
    }
    (void)ItemCombineMgr::ms_tbl_ItemCombine.GetRecord(n);
  }
  return true;
}

CPlayer::CPlayer() = default;

void CPlayer::SetStaticMember()
{
}

void CPlayer::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_EP.AllocEffParam();
  m_tmrIntervalSec.BeginTimer(0x3E8);
  m_tmrBilling.BeginTimer(0x3A980);
  m_pPartyMgr = &g_PartyPlayer[static_cast<unsigned __int64>(pID->m_wIndex)];
  m_bLoad = false;
  m_bOper = false;
  m_bPostLoad = false;
  m_bFullMode = false;
  m_byPosRaceTown = static_cast<unsigned __int8>(-1);
  m_bCheat_100SuccMake = false;
  m_bCheat_makeitem_no_use_matrial = false;
  m_bCheat_Matchless = false;
  m_bFreeRecallWaitTime = false;
  m_pUserDB = nullptr;
  m_pUsingUnit = nullptr;
  m_pParkingUnit = nullptr;
  m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
  m_pRecalledAnimusItem = nullptr;
  m_pRecalledAnimusChar = nullptr;
  m_dwLastRecallTime = 0;
  m_byNextRecallReturn = static_cast<unsigned __int8>(-1);
  m_id.dwSerial = static_cast<unsigned int>(-1);
  m_id.wIndex = pID->m_wIndex;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_byUserDgr = 0;
  m_pBindMapData = nullptr;
  m_pBindDummyData = nullptr;
  m_dwNextTimeDungeonDie = 0;
  m_bNeverDie = false;
  m_bRecvMapChat = false;
  PastWhisperInit();
  m_clsSetItem.Init_Info();
  m_nChaosMode = 0;
  m_dwChaosModeTime10Per = 0;
  m_dwChaosModeEndTime = 0;
  m_MinigTicket.Init();
  m_bSnowMan = false;
  m_tmrGroupTargeting.BeginTimer(0x2710);
  m_bAfterEffect = false;
  m_bSFDelayNotCheck = false;
  m_tmrEffectStartTime.BeginTimer(0x36EE80);
  m_tmrEffectEndTime.BeginTimer(0xEA60);
  m_kMoveDelayChecker.Init(0x0A);
  m_NameChangeBuddyInfo.Init();
  m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);
  m_tmrAccumPlayingTime.BeginTimer(0x493E0);
  m_tmrPremiumPVPInform.BeginTimer(0x1B7740);
}

CPlayer::~CPlayer() = default;

void CPlayer::PastWhisperInit()
{
  for (int j = 0; j < 10; ++j)
  {
    m_PastWhiper[j].bMemory = false;
  }
}

void _WEAPON_PARAM::SetStaticMember(CRecordData *itemTable)
{
  (void)itemTable;
}

void _MASTERY_PARAM::SetStaticMember(CRecordData *effectTable, CRecordData *forceTable)
{
  (void)effectTable;
  (void)forceTable;
}

void MiningTicket::_AuthKeyTicket::Init()
{
  ___u0.uiData = 0;
}

void MiningTicket::_AuthKeyTicket::Set(unsigned int uiSrc)
{
  ___u0.uiData = uiSrc;
}

void MiningTicket::_AuthKeyTicket::Set(
  unsigned __int16 byYear,
  unsigned __int8 byMonth,
  unsigned __int8 byDay,
  unsigned __int8 byHour,
  unsigned __int8 byNumofTime)
{
  ___u0.uiData = ((byYear & 0x3FFFu) << 18) | (___u0.uiData & 0x3FFFFu);
  ___u0.uiData = ((byMonth & 0xFu) << 14) | (___u0.uiData & 0xFFFC3FFFu);
  ___u0.uiData = ((byDay & 0x1Fu) << 9) | (___u0.uiData & 0xFFFFC1FFu);
  ___u0.uiData = ((byHour & 0x1Fu) << 4) | (___u0.uiData & 0xFFFFFE0Fu);
  ___u0.uiData = (byNumofTime & 0xFu) | (___u0.uiData & 0xFFFFFFF0u);
}

bool MiningTicket::_AuthKeyTicket::operator==(const _AuthKeyTicket &src) const
{
  return ___u0.uiData == src.___u0.uiData;
}

bool MiningTicket::_AuthKeyTicket::operator!=(const _AuthKeyTicket &src) const
{
  return ___u0.uiData != src.___u0.uiData;
}

void MiningTicket::Init()
{
  m_dwTakeLastMentalTicket.Init();
  m_dwTakeLastCriTicket.Init();
}

bool MiningTicket::AuthLastMentalTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  if (!m_dwTakeLastMentalTicket.___u0.uiData)
  {
    return false;
  }

  _AuthKeyTicket current;
  current.Set(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
  return !(current != m_dwTakeLastMentalTicket);
}

bool MiningTicket::AuthLastCriTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  if (!m_dwTakeLastCriTicket.___u0.uiData)
  {
    return false;
  }

  _AuthKeyTicket current;
  current.Set(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
  return !(current != m_dwTakeLastCriTicket);
}

unsigned int MiningTicket::GetLastMentalTicket()
{
  return m_dwTakeLastMentalTicket.___u0.uiData;
}

unsigned int MiningTicket::GetLastCriTicket()
{
  return m_dwTakeLastCriTicket.___u0.uiData;
}

void MiningTicket::SetLastMentalTicket(unsigned int uiMentalTicket)
{
  m_dwTakeLastMentalTicket.___u0.uiData = uiMentalTicket;
}

void MiningTicket::SetLastMentalTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentYear & 0x3FFFu) << 18) | (m_dwTakeLastMentalTicket.___u0.uiData & 0x3FFFFu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentMonth & 0xFu) << 14) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFC3FFFu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentDay & 0x1Fu) << 9) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFFC1FFu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    ((byCurrentHour & 0x1Fu) << 4) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFFFE0Fu);
  m_dwTakeLastMentalTicket.___u0.uiData =
    (byNumOfTime & 0xFu) | (m_dwTakeLastMentalTicket.___u0.uiData & 0xFFFFFFF0u);
}

void MiningTicket::SetLastCriTicket(unsigned int uiCriTicket)
{
  m_dwTakeLastCriTicket.___u0.uiData = uiCriTicket;
}

void MiningTicket::SetLastCriTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentYear & 0x3FFFu) << 18) | (m_dwTakeLastCriTicket.___u0.uiData & 0x3FFFFu);
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentMonth & 0xFu) << 14) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFC3FFFu);
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentDay & 0x1Fu) << 9) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFFC1FFu);
  m_dwTakeLastCriTicket.___u0.uiData =
    ((byCurrentHour & 0x1Fu) << 4) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFFFE0Fu);
  m_dwTakeLastCriTicket.___u0.uiData =
    (byNumOfTime & 0xFu) | (m_dwTakeLastCriTicket.___u0.uiData & 0xFFFFFFF0u);
}

void _NameChangeBuddyInfo::Init()
{
  bNameChange = false;
  nSendNum = 0;
}

void CPlayer::SendMsg_GotoRecallResult(
  char byErrCode,
  char byMapCode,
  float *pfStartPos,
  unsigned __int8 byMapInType)
{
  struct
  {
    char errCode;
    char mapCode;
    float startPos[3];
    unsigned __int8 mapInType;
  } msg{};

  msg.errCode = byErrCode;
  msg.mapCode = byMapCode;
  memcpy_0(msg.startPos, pfStartPos, sizeof(msg.startPos));
  msg.mapInType = byMapInType;

  unsigned __int8 type[2]{};
  type[0] = 4;
  type[1] = 29;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 15);
}

void CPlayer::SendMsg_GotoBasePortalResult(char byErrCode)
{
  char msg[2]{};
  short pos[3]{};

  msg[0] = byErrCode;
  msg[1] = static_cast<char>(m_pCurMap->m_pMapSet->m_dwIndex);
  FloatToShort(m_fCurPos, pos, 3);

  unsigned __int8 type[2]{};
  type[0] = 4;
  type[1] = 27;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, msg, 8u);
}

void CPlayer::SendMsg_MapOut(unsigned __int8 byMapOutCode, unsigned __int8 byNextMapCode)
{
  struct
  {
    unsigned __int8 mapOutCode;
    unsigned int objSerial;
    unsigned __int8 nextMapCode;
  } msg{};

  msg.mapOutCode = byMapOutCode;
  msg.objSerial = m_dwObjSerial;
  msg.nextMapCode = byNextMapCode;

  unsigned __int8 type[2]{};
  type[0] = 3;
  type[1] = 39;
  CircleReport(type, reinterpret_cast<char *>(&msg), 6, false);
}

bool CPlayer::IsHaveMentalTicket()
{
  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byCurrentHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  return m_MinigTicket.AuthLastCriTicket(startYear, startMonth, startDay, byCurrentHour, byNumOfTime);
}

bool CPlayer::IsUseReleaseRaceBuffPotion()
{
  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  MiningTicket::_AuthKeyTicket key;
  key.Set(startYear, startMonth, startDay, byHour, byNumOfTime);
  return m_dwRaceBuffClearKey == key;
}

void CPlayer::SetUseReleaseRaceBuffPotion()
{
  const unsigned __int8 byNumOfTime = g_HolySys.GetNumOfTime();
  const unsigned __int8 byHour = g_HolySys.GetStartHour();
  const unsigned __int8 startDay = g_HolySys.GetStartDay();
  const unsigned __int8 startMonth = g_HolySys.GetStartMonth();
  const unsigned __int16 startYear = g_HolySys.GetStartYear();

  m_dwRaceBuffClearKey.Set(startYear, startMonth, startDay, byHour, byNumOfTime);
  m_pUserDB->m_AvatorData.dbSupplement.dwRaceBuffClear = m_dwRaceBuffClearKey.___u0.uiData;
  m_pUserDB->m_bDataUpdate = true;
}

void CPlayer::SendMsg_MapEnvInform(char byMapCode, unsigned int dwMapEnvCode)
{
  struct
  {
    char mapCode;
    unsigned int mapEnvCode;
    int mapTime;
  } msg{};

  msg.mapCode = byMapCode;
  msg.mapEnvCode = dwMapEnvCode;

  std::time_t now{};
  std::time(&now);
  std::tm localTime{};
  if (auto *tmPtr = std::localtime(&now))
  {
    localTime = *tmPtr;
  }
  localTime.tm_hour = g_WorldSch.m_nCurHour;
  localTime.tm_min = g_WorldSch.m_nCurMin;
  localTime.tm_sec = g_WorldSch.m_nCurMilSec / 1000;
  msg.mapTime = static_cast<int>(std::mktime(&localTime));

  unsigned __int8 type[2]{};
  type[0] = 8;
  type[1] = 5;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 9);
}

void CPlayer::SendMsg_MineCancle()
{
  unsigned __int8 type[2]{};
  type[0] = 14;
  type[1] = 4;

  char msg{};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, &msg, 1);
}

void CPlayer::SendMsg_DeleteStorageInform(char byStorageCode, unsigned __int16 wSerial)
{
  struct
  {
    char storageCode;
    unsigned __int16 serial;
  } msg{};

  msg.storageCode = byStorageCode;
  msg.serial = wSerial;

  unsigned __int8 type[2]{};
  type[0] = 7;
  type[1] = 23;
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 3);
}

void _STORAGE_LIST::_storage_con::empty()
{
  m_bLoad = 0;
  m_bLock = 0;
  m_dwETSerialNumber = 0;
  m_byCsMethod = 0;
}

bool _STORAGE_LIST::EmptyCon(int n)
{
  if (m_nListCode == 1 && n >= 8)
  {
    return false;
  }
  if (!m_pStorageList[n].m_bLoad)
  {
    return false;
  }
  m_pStorageList[n].empty();
  return true;
}

int _STORAGE_LIST::GetIndexEmptyCon()
{
  for (int j = 0; j < m_nUsedNum; ++j)
  {
    if (!m_pStorageList[j].m_bLoad)
    {
      return j;
    }
  }
  return 255;
}

int _STORAGE_LIST::GetNumEmptyCon()
{
  int count = 0;
  for (int j = 0; j < m_nUsedNum; ++j)
  {
    if (!m_pStorageList[j].m_bLoad)
    {
      ++count;
    }
  }
  return count;
}

bool CPlayer::OutOfMap(CMapData *pIntoMap, unsigned __int16 wLayerIndex, unsigned __int8 byMapOutType, float *pfStartPos)
{
  if (!m_bInGuildBattle
      && m_byUserDgr >= 2
      && !m_bObserver
      && pIntoMap != m_pCurMap
      && pIntoMap->m_pMapSet->m_nRaceVillageCode < 3
      && static_cast<unsigned int>(CPlayerDB::GetRaceCode(&m_Param)) != static_cast<unsigned int>(pIntoMap->m_pMapSet->m_nRaceVillageCode))
  {
    mgr_tracing(1);
  }

  if (m_pCurMap == g_TransportShip[CPlayerDB::GetRaceCode(&m_Param)].m_pLinkShipMap)
  {
    const int raceCode = CPlayerDB::GetRaceCode(&m_Param);
    g_TransportShip[raceCode].ExitMember(this, false);
  }

  m_bMapLoading = true;
  _AnimusReturn(3);
  DTradeInit();
  m_byMapInModeBuffer = byMapOutType;

  const unsigned int dwMapEnvCode = CLevel::GetEnvironment(&pIntoMap->m_Level);
  SendMsg_MapEnvInform(static_cast<char>(pIntoMap->m_pMapSet->m_dwIndex), dwMapEnvCode);
  SendMsg_MapOut(byMapOutType, static_cast<unsigned __int8>(pIntoMap->m_pMapSet->m_dwIndex));

  const unsigned int curSecNum = GetCurSecNum();
  m_pCurMap->ExitMap(this, curSecNum);
  SetCurSecNum(0xFFFFFFFF);

  if (pIntoMap->m_pMapSet->m_nMapType)
  {
    if (m_pUserDB)
    {
      CUserDB::Update_Map(m_pUserDB, m_pCurMap->m_pMapSet->m_dwIndex, m_fCurPos);
    }
  }
  else
  {
    if (m_pUserDB)
    {
      CUserDB::Update_Map(m_pUserDB, pIntoMap->m_pMapSet->m_dwIndex, pfStartPos);
    }
    memcpy_0(m_fCurPos, pfStartPos, sizeof(m_fCurPos));
  }

  if (CGameObject::s_pSelectObject == this)
  {
    CGameObject::s_pSelectObject = nullptr;
  }

  if (m_EP.GetEff_State(14))
  {
    RemoveSFContHelpByEffect(2, 14);
  }

  if (m_bMineMode)
  {
    m_bMineMode = false;
    m_dwMineNextTime = static_cast<unsigned int>(-1);
    SendMsg_MineCancle();
  }

  m_pCurMap = pIntoMap;
  m_wMapLayerIndex = wLayerIndex;
  CPlayerDB::SetMapCode(&m_Param, pIntoMap->m_pMapSet->m_dwIndex);
  SetCurPos(pfStartPos);
  memcpy_0(m_fTarPos, m_fCurPos, sizeof(m_fTarPos));
  m_bMove = false;
  memcpy_0(m_fLastRecvPos, m_fCurPos, sizeof(m_fLastRecvPos));
  m_byLastRecvMapIndex = m_pCurMap->m_pMapSet->m_dwIndex;
  return true;
}
