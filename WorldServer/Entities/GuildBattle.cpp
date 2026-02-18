
#include "pch.h"

#include "GuildBattle.h"
#include "GuildBattleTypes.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>
#include <atlcomtime.h>

#include "CLogFile.h"
#include "CMainThread.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CCircleZone.h"
#include "CGravityStoneRegener.h"
#include "CGravityStone.h"
#include "CMyTimer.h"
#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CRecordData.h"
#include "CGuild.h"
#include "CGuildBattleController.h"
#include "GlobalObjects.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "StorageList.h"
#include "WorldServerUtil.h"
#include "base_fld.h"
#include "guild_battle_get_gravity_stone_result_zocl.h"
#include "qry_case_updatereservedschedule.h"
#include "qry_case_updateclearguildbattleDayInfo.h"
#include "qry_case_addpvppoint.h"
#include "qry_case_updatewinloseguildbattlerank.h"
#include "qry_case_updatedrawguildbattlerank.h"
#include "qry_case_guild_battel_result_log.h"
#include "worlddb_guild_battle_rank_list.h"
#include "worlddb_guild_battle_schedule_list.h"
#include "worlddb_guild_battle_info.h"
#include "worlddb_guild_battle_reserved_schedule_info.h"
#include "guild_battle_goal_result_zocl.h"
#include "GuildBattleLocalStructs.h"

__int64 _qry_case_in_guildbattlecost::size()
{
  return 40;
}

__int64 _qry_case_dest_guild_out_guildbattlecost::size()
{
  return 64;
}

__int64 _qry_case_src_guild_out_guildbattlecost::size()
{
  return 64;
}

__int64 _qry_case_addguildbattleschedule::size()
{
  return 48;
}

__int64 _possible_battle_guild_list_result_zocl::size()
{
  if (byCount > 4u)
  {
    byCount = 0;
  }
  return 204 - 23LL * (4 - byCount);
}

__int64 _guild_battle_rank_list_result_zocl::size()
{
  if (byCnt <= 0xBu)
  {
    return 427;
  }
  byCnt = 0;
  return 0;
}

__int64 _guild_battle_reserved_schedule_result_zocl::size()
{
  if (byCnt <= 5u)
  {
    return 211;
  }
  byCnt = 0;
  return 0;
}

namespace GUILD_BATTLE
{
  static CGuildBattleState GUILD_BATTLE_STATE_NULL_11{};
  static CGuildBattleStateList STATE_LIST_NULL_10(2147483646, CGuildBattleStateList::GBS_ONCE, 1u);
  static CNormalGuildBattleStateList NORMAL_GUILD_BATTLE_STATE_NULL_11;
  unsigned __int8 LIMIT_SRC_GRADE = 0;
  unsigned __int8 LIMIT_DEST_GRADE = 0;
  CGuildBattleRewardItem CGuildBattleRewardItem::ms_kNullObj{};
  CGuildBattleRewardItemManager *CGuildBattleRewardItemManager::ms_Instance = nullptr;
  CGuildBattleRankManager *CGuildBattleRankManager::ms_Instance = nullptr;
  CGuildBattleScheduler *CGuildBattleScheduler::ms_Instance = nullptr;
  CGuildBattleReservedScheduleListManager *CGuildBattleReservedScheduleListManager::ms_Instance = nullptr;
  CCurrentGuildBattleInfoManager *CCurrentGuildBattleInfoManager::ms_Instance = nullptr;

  int CGuildBattle::GetObjType()
  {
    return -1;
  }

  int CGuildBattleState::Enter(CGuildBattle * /*pkBattle*/)
  {
    return 0;
  }

  int CGuildBattleState::Loop(CGuildBattle * /*pkBattle*/)
  {
    return 0;
  }

  int CGuildBattleState::Fin(CGuildBattle * /*pkBattle*/)
  {
    return 0;
  }

  ATL::CTimeSpan *CGuildBattleState::GetTerm(ATL::CTimeSpan *result)
  {
    *result = ATL::CTimeSpan(0LL);
    return result;
  }

  CGuildBattleStateList::CGuildBattleStateList(int iStateMax, GBS_LOOP_TYPE iLoopType, unsigned int uiLoopCnt)
    : m_iForceAdvance(0),
      m_iState(0),
      m_pkNextState(nullptr),
      m_pkCurState(nullptr),
      STATE_MAX(iStateMax),
      m_uiLoopCnt(uiLoopCnt),
      m_uiCurLoopCnt(0),
      m_eLoopType(GBS_ONCE),
      m_bEnter(false)
  {
    if (iLoopType <= GBS_COUNT)
    {
      m_eLoopType = iLoopType;
    }
    else
    {
      m_eLoopType = GBS_ONCE;
    }
    Clear();
  }

  void CGuildBattleStateList::SetNextState()
  {
    m_pkNextState = m_pkCurState;
  }

  void CGuildBattleStateList::Clear()
  {
    m_bEnter = true;
    m_pkCurState = &GUILD_BATTLE_STATE_NULL_11;
    m_pkNextState = m_pkCurState;
    m_iState = -3;
    m_iForceAdvance = -3;
    m_uiCurLoopCnt = 0;
  }

  void CGuildBattleStateList::SetWait()
  {
    m_iState = STATE_WAIT;
  }

  void CGuildBattleStateList::SetReady()
  {
    m_iState = STATE_READY;
  }

  bool CGuildBattleStateList::IsProc()
  {
    return m_iState != STATE_NONE && m_iState <= STATE_MAX && m_iState != STATE_WAIT && m_iState != STATE_READY;
  }

  void CGuildBattleStateList::Process(CGuildBattle *pkBattle)
  {
    if (m_bEnter && m_iState < STATE_MAX)
    {
      if (m_pkCurState != m_pkNextState)
      {
        m_pkCurState = m_pkNextState;
      }

      m_bEnter = false;
      const int advanceResult = m_pkCurState->Enter(pkBattle);
      Advance(advanceResult);
      return;
    }

    if (m_pkCurState == m_pkNextState && m_iState != STATE_MAX)
    {
      if (m_iState < STATE_MAX)
      {
        const int loopResult = m_pkCurState->Loop(pkBattle);
        Advance(loopResult);

        if (m_iForceAdvance != STATE_NONE)
        {
          Advance(m_iForceAdvance);
          m_iForceAdvance = STATE_NONE;
        }
      }
      return;
    }

    m_pkCurState->Fin(pkBattle);
    m_bEnter = true;
    if (m_iState == STATE_MAX)
    {
      Next(true);
    }
  }

  void CGuildBattleStateList::Advance(int iAdvance)
  {
    if (!iAdvance)
    {
      return;
    }

    switch (iAdvance)
    {
      case 1:
        m_iState = STATE_MAX;
        break;
      case 2:
        Next(false);
        break;
      case 3:
        Goto();
        break;
      default:
        break;
    }
  }

  int CGuildBattleStateList::Next(bool bForce)
  {
    if (!bForce && IsEmpty())
    {
      return 0;
    }

    ++m_iState;
    const int loopResult = CheckLoop();
    if (loopResult != 2)
    {
      return loopResult;
    }

    SetNextState();
    return 1;
  }

  int CGuildBattleStateList::CheckLoop()
  {
    if (m_eLoopType == GBS_ONCE)
    {
      if (m_iState == STATE_MAX)
      {
        return 1;
      }
      if (m_iState > STATE_MAX)
      {
        m_iState = STATE_MAX;
        return 0;
      }
      return 2;
    }

    if (m_eLoopType == GBS_LOOP)
    {
      if (m_iState == STATE_MAX)
      {
        return 1;
      }
      if (m_iState > STATE_MAX)
      {
        m_iState = 0;
      }
      return 2;
    }

    if (m_eLoopType == GBS_COUNT)
    {
      if (m_iState == STATE_MAX)
      {
        return 1;
      }
      if (m_iState > STATE_MAX)
      {
        m_iState = 0;
        if (m_uiLoopCnt <= ++m_uiCurLoopCnt)
        {
          return 0;
        }
      }
      return 2;
    }

    return 2;
  }

  ATL::CTimeSpan *CGuildBattleStateList::GetTerm(ATL::CTimeSpan *result)
  {
    m_pkNextState->GetTerm(result);
    return result;
  }

  bool CGuildBattleStateList::IsEmpty()
  {
    return m_iState == STATE_NONE || m_iState > STATE_MAX;
  }

  int CGuildBattleStateList::Goto()
  {
    if (IsEmpty())
    {
      return 0;
    }

    const int loopResult = CheckLoop();
    if (loopResult != 2)
    {
      return loopResult;
    }

    SetNextState();
    return 1;
  }

  void CGuildBattleStateList::ForceNext()
  {
    m_iForceAdvance = 2;
  }

  char CGuildBattleStateList::GotoState(int iState)
  {
    if (iState >= STATE_MAX)
    {
      return 0;
    }

    m_iState = iState;
    return 1;
  }

  CNormalGuildBattleState::CNormalGuildBattleState() = default;

  CNormalGuildBattleStateNotify::CNormalGuildBattleStateNotify() = default;

  CNormalGuildBattleStateReady::CNormalGuildBattleStateReady() = default;

  CNormalGuildBattleStateCountDown::CNormalGuildBattleStateCountDown() = default;

  CNormalGuildBattleStateRound::CNormalGuildBattleStateRound() = default;

  CNormalGuildBattleStateRoundStart::CNormalGuildBattleStateRoundStart() : m_pkTimer(nullptr)
  {
    m_pkTimer = new (std::nothrow) CMyTimer();
    if (!m_pkTimer)
    {
      m_pkTimer = nullptr;
    }
  }

  CNormalGuildBattleStateRoundProcess::CNormalGuildBattleStateRoundProcess() : m_pkTimer(nullptr)
  {
    m_pkTimer = new (std::nothrow) CMyTimer();
    if (!m_pkTimer)
    {
      m_pkTimer = nullptr;
    }
  }

  CNormalGuildBattleStateRoundReturnStartPos::CNormalGuildBattleStateRoundReturnStartPos() : m_pkTimer(nullptr)
  {
    m_pkTimer = new (std::nothrow) CMyTimer();
    if (!m_pkTimer)
    {
      m_pkTimer = nullptr;
    }
  }

  CNormalGuildBattleStateRoundList::CNormalGuildBattleStateRoundList()
    : CGuildBattleStateList(3, CGuildBattleStateList::GBS_LOOP, 1u)
  {
    m_pStateList[0] = &START;
    m_pStateList[1] = &PROCESS;
    m_pStateList[2] = &ROUND_END;
  }

  bool CNormalGuildBattleStateRoundList::IsInBattleRegenState()
  {
    return m_pkCurState == &PROCESS;
  }

  CNormalGuildBattleStateInBattle::CNormalGuildBattleStateInBattle()
    : m_kInBattleTime(0LL),
      m_kRountStateList()
  {
  }

  void CNormalGuildBattleStateInBattle::SetBattleTime(ATL::CTimeSpan kTime)
  {
    m_kInBattleTime = kTime;
  }

  bool CNormalGuildBattleStateInBattle::IsInBattleRegenState()
  {
    return m_kRountStateList.IsInBattleRegenState();
  }

  CNormalGuildBattleStateDivide::CNormalGuildBattleStateDivide() = default;

  CNormalGuildBattleStateReturn::CNormalGuildBattleStateReturn() = default;

  CNormalGuildBattleStateFin::CNormalGuildBattleStateFin() = default;

  CNormalGuildBattleStateList::CNormalGuildBattleStateList()
    : CGuildBattleStateList(7, CGuildBattleStateList::GBS_ONCE, 1u)
  {
    m_pStateList[0] = &NOTIFY;
    m_pStateList[1] = &READY;
    m_pStateList[2] = &COUNT;
    m_pStateList[3] = &INBATTLE;
    m_pStateList[4] = &DIVIDE;
    m_pStateList[5] = &RETURN;
    m_pStateList[6] = &FIN;
  }

  void CNormalGuildBattleStateList::SetNextState()
  {
    m_pkNextState = m_pStateList[m_iState];
  }

  bool CNormalGuildBattleStateList::IsReadyOrCountState()
  {
    return m_pkCurState == &READY || m_pkCurState == &COUNT;
  }

  bool CNormalGuildBattleStateList::IsInBattle()
  {
    return m_pkCurState == &INBATTLE;
  }

  bool CNormalGuildBattleStateList::IsInBattleRegenState()
  {
    return IsInBattle() && INBATTLE.IsInBattleRegenState();
  }

  void CNormalGuildBattleStateList::SetBattleTime(ATL::CTimeSpan kTime)
  {
    INBATTLE.SetBattleTime(kTime);
  }

  CNormalGuildBattleStateListPool *CNormalGuildBattleStateListPool::ms_Instance = nullptr;

  CNormalGuildBattleStateListPool *CNormalGuildBattleStateListPool::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CNormalGuildBattleStateListPool();
    }
    return ms_Instance;
  }

  CNormalGuildBattleStateListPool::CNormalGuildBattleStateListPool() : m_dwMaxCount(0), m_pkStateList(nullptr)
  {
  }

  bool CNormalGuildBattleStateListPool::Init()
  {
    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    const unsigned int mapCount = fieldList->GetMapCnt();
    if (!mapCount)
    {
      return false;
    }

    m_dwMaxCount = 46 * mapCount;
    m_pkStateList = new (std::nothrow) CNormalGuildBattleStateList[m_dwMaxCount];
    if (m_pkStateList)
    {
      return true;
    }

    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattleStateListPool::Init()NULL == new CNormalGuildBattleStateList[%u]",
      m_dwMaxCount);
    return false;
  }

  CNormalGuildBattleStateList *CNormalGuildBattleStateListPool::Get(unsigned int dwID)
  {
    if (m_pkStateList && m_dwMaxCount > dwID)
    {
      if (&m_pkStateList[dwID])
      {
        m_pkStateList[dwID].Clear();
        return &m_pkStateList[dwID];
      }
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleStateListPool::Get( %u ) : 0 == &m_pkStateList[%u] Invalid!",
        dwID,
        dwID);
      return nullptr;
    }
    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattleStateListPool::Get( %u ) : 0 == m_pkStateList || %u <= %u Invalid!",
      dwID,
      m_dwMaxCount,
      dwID);
    return nullptr;
  }

  CNormalGuildBattleFieldList *CNormalGuildBattleFieldList::ms_Instance = nullptr;

  CNormalGuildBattleFieldList *CNormalGuildBattleFieldList::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CNormalGuildBattleFieldList();
    }
    return ms_Instance;
  }

  CNormalGuildBattleFieldList::CNormalGuildBattleFieldList()
  {
    m_dwCnt = 0;
    m_pkField = nullptr;
    m_byUseFieldCnt[0] = 0;
    m_byUseFieldCnt[1] = 0;
    m_byUseFieldCnt[2] = 0;
    m_ppkUseFieldByRace[0] = nullptr;
    m_ppkUseFieldByRace[1] = nullptr;
    m_ppkUseFieldByRace[2] = nullptr;
  }

  bool CNormalGuildBattleFieldList::Init()
  {
    char szStrBuff[1024]{};
    unsigned int mapCount = GetPrivateProfileIntA("Field", "mapcnt", 100, "./Initialize/NormalGuildBattle.ini");
    if (!mapCount)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleFieldList::Init() GetPrivateProfileInt( Field, mapcnt, 0, %s ) = 0",
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_dwCnt = mapCount;
    m_pkField = new (std::nothrow) CNormalGuildBattleField[m_dwCnt];
    if (!m_pkField)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleFieldList::Init() NULL == new CNormalGuildBattleField[%u]",
        m_dwCnt);
      return false;
    }

    for (unsigned int uiMapInx = 0; uiMapInx < m_dwCnt; ++uiMapInx)
    {
      if (!m_pkField[uiMapInx].Init(uiMapInx))
      {
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleFieldList::Init() m_pkField[%u].Init(%u) = false",
          uiMapInx,
          uiMapInx);
        return false;
      }
    }

    char parseStorage[25600]{};
    char *szParseBuff[100]{};
    for (int j = 0; j < 100; ++j)
    {
      szParseBuff[j] = &parseStorage[256 * j];
    }
    memset_0(parseStorage, 0, sizeof(parseStorage));

    const char *szKeyName[3] = {"Bellato", "Cora", "Accratia"};
    for (int k = 0; k < 3; ++k)
    {
      if (!InitUseField(static_cast<unsigned __int8>(k), const_cast<char *>(szKeyName[k]), szStrBuff, szParseBuff))
      {
        return false;
      }
    }
    return true;
  }

  bool CNormalGuildBattleFieldList::InitUseField(
    unsigned __int8 byRace,
    char *szKeyName,
    char *szStrBuff,
    char **szParseBuff)
  {
    if (byRace >= 3u)
    {
      return false;
    }
    *szStrBuff = 0;
    GetPrivateProfileStringA(
      "UseMap",
      szKeyName,
      "X",
      szStrBuff,
      0x7FFu,
      "./Initialize/NormalGuildBattle.ini");
    if (!strcmp_0("X", szStrBuff))
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleFieldList::InitUseField( %u, %s, szUseMapInxStr, szBuff ) : GetPrivateProfileInt( UseMap, %s, X, szStr, 2047, %s ) = X",
        byRace,
        szKeyName,
        szKeyName,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    const int count = ParsingCommandA(szStrBuff, 100, szParseBuff, 255);
    if (count && count <= 100)
    {
      m_ppkUseFieldByRace[byRace] = new CNormalGuildBattleField *[count];
      if (m_pkField)
      {
        m_byUseFieldCnt[byRace] = static_cast<unsigned __int8>(count);
        for (int j = 0; j < count; ++j)
        {
          const int mapIndex = atoi(szParseBuff[j]);
          m_ppkUseFieldByRace[byRace][j] = &m_pkField[mapIndex];
        }
        return true;
      }

      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleFieldList::InitUseField( %u, %s, szUseMapInxStr, szBuff ) : m_ppkUseFieldByRace[%u] == new CNormalGuildBattleField * [%u]",
        byRace,
        szKeyName,
        byRace,
        m_dwCnt);
      return false;
    }

    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattleFieldList::InitUseField( %u, %s, szUseMapInxStr, szBuff ) : ::ParsingCommandA( szStrBuff, 100, szParseBuff, 255 ) == %u Invalid!",
      byRace,
      szKeyName,
      count);
    return false;
  }

  unsigned int CNormalGuildBattleFieldList::GetMapCnt()
  {
    return m_dwCnt;
  }

  char CNormalGuildBattleFieldList::GetMapInx(unsigned __int8 byRace, unsigned int dwMapCode, unsigned int *dwMapInx)
  {
    if (byRace >= 3u)
    {
      return 0;
    }
    if (!m_byUseFieldCnt[byRace] || !m_ppkUseFieldByRace[byRace])
    {
      return 0;
    }
    for (unsigned __int8 j = 0; j < m_byUseFieldCnt[byRace]; ++j)
    {
      if (dwMapCode == m_ppkUseFieldByRace[byRace][j]->GetMapCode())
      {
        *dwMapInx = m_ppkUseFieldByRace[byRace][j]->GetMapID();
        return 1;
      }
    }
    return 0;
  }

  __int64 CNormalGuildBattleFieldList::GetMapInxList(unsigned __int8 byRace, unsigned __int8 *pbyInxList)
  {
    if (byRace >= 3u)
    {
      return 0;
    }
    if (!m_byUseFieldCnt[byRace] || !m_ppkUseFieldByRace[byRace])
    {
      return 0;
    }

    for (unsigned int j = 0; j < m_byUseFieldCnt[byRace]; ++j)
    {
      const unsigned __int8 mapCode =
        static_cast<unsigned __int8>(m_ppkUseFieldByRace[byRace][j]->GetMapCode());
      pbyInxList[j] = mapCode;
    }

    return m_byUseFieldCnt[byRace];
  }

  CNormalGuildBattleField *CNormalGuildBattleFieldList::GetField(unsigned int dwMapID)
  {
    if (m_dwCnt > dwMapID && m_pkField)
    {
      return &m_pkField[dwMapID];
    }
    return nullptr;
  }

  CNormalGuildBattleField *CNormalGuildBattleFieldList::GetField(unsigned __int8 byRace, unsigned int dwMapCode)
  {
    if (byRace >= 3u)
    {
      return nullptr;
    }
    if (!m_byUseFieldCnt[byRace] || !m_ppkUseFieldByRace[byRace])
    {
      return nullptr;
    }
    for (unsigned __int8 j = 0; j < m_byUseFieldCnt[byRace]; ++j)
    {
      if (dwMapCode == m_ppkUseFieldByRace[byRace][j]->GetMapCode())
      {
        return m_ppkUseFieldByRace[byRace][j];
      }
    }
    return nullptr;
  }

  bool CNormalGuildBattleField::Init(unsigned int uiMapInx)
  {
    char returned[256]{};
    char buffer[256]{};
    char tmp[256]{};
sprintf(buffer, "Map%d", uiMapInx);
    returned[0] = 0;
    GetPrivateProfileStringA(buffer, "Name", "X", returned, 0xFFu, "./Initialize/NormalGuildBattle.ini");
    if (returned[0] == 'X')
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )GetPrivateProfileString( %s, Name, X, %s, 255, %s ) Fail",
        uiMapInx,
        buffer,
        returned,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_pkMap = g_MapOper.GetMap(returned);
    if (!m_pkMap)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )NULL == g_MapOper.GetMap( %s )",
        uiMapInx,
        returned);
      return false;
    }

    returned[0] = 0;
    GetPrivateProfileStringA(
      buffer,
      "1PStartPosDummyName",
      "X",
      returned,
      0xFFu,
      "./Initialize/NormalGuildBattle.ini");
    if (returned[0] == 'X')
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )GetPrivateProfileString( %s, 1PStartPosDummyName, X, %s, 255, %s ) Fail!",
        uiMapInx,
        buffer,
        returned,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_pkStartPos[0] = new (std::nothrow) _dummy_position();
    if (!m_pkStartPos[0])
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )new _dummy_position",
        uiMapInx);
      return false;
    }
    if (!m_pkMap->LoadDummy(returned, m_pkStartPos[0]))
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )m_pkMap->LoadDummy( %s, ... )",
        uiMapInx,
        returned);
      return false;
    }

    returned[0] = 0;
    GetPrivateProfileStringA(
      buffer,
      "2PStartPosDummyName",
      "X",
      returned,
      0xFFu,
      "./Initialize/NormalGuildBattle.ini");
    if (returned[0] == 'X')
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )GetPrivateProfileString( %s, 2PStartPosDummyName, X, %s, 255, %s ) Fail!",
        uiMapInx,
        buffer,
        returned,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_pkStartPos[1] = new (std::nothrow) _dummy_position();
    if (!m_pkStartPos[1])
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )new _dummy_position",
        uiMapInx);
      return false;
    }
    if (!m_pkMap->LoadDummy(returned, m_pkStartPos[1]))
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )m_pkMap->LoadDummy()",
        uiMapInx);
      return false;
    }

    m_ui1PGoalPosCnt =
      GetPrivateProfileIntA(buffer, "1PGoalPosCnt", 0, "./Initialize/NormalGuildBattle.ini");
    if (!m_ui1PGoalPosCnt)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u ) : GetPrivateProfileInt( %s, 1PGoalPosCnt, 0, %s ) is 0 !",
        uiMapInx,
        buffer,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_pk1PGoalZone = new (std::nothrow) CCircleZone[m_ui1PGoalPosCnt];
    if (!m_pk1PGoalZone)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )new CCircleZone[%u]",
        uiMapInx,
        m_ui1PGoalPosCnt);
    }
    for (int iNth = 0; iNth < static_cast<int>(m_ui1PGoalPosCnt); ++iNth)
    {
      if (!m_pk1PGoalZone[iNth].Init(uiMapInx, 1, iNth, static_cast<unsigned __int16>(iNth), m_pkMap))
      {
        const int mapCode = static_cast<int>(GetMapCode());
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleField::Init( %u )m_pk1PGoalZone[%d].Init( %u, %u, %d )",
          uiMapInx,
          iNth,
          uiMapInx,
          iNth,
          mapCode);
      }
    }

    m_ui2PGoalPosCnt =
      GetPrivateProfileIntA(buffer, "2PGoalPosCnt", 0, "./Initialize/NormalGuildBattle.ini");
    if (!m_ui2PGoalPosCnt)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u ) : GetPrivateProfileInt( %s, 2PGoalPosCnt, 0, %s ) is 0 !",
        uiMapInx,
        buffer,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_pk2PGoalZone = new (std::nothrow) CCircleZone[m_ui2PGoalPosCnt];
    if (!m_pk2PGoalZone)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )new CCircleZone[%u]",
        uiMapInx,
        m_ui2PGoalPosCnt);
    }
    for (int iNth = 0; iNth < static_cast<int>(m_ui2PGoalPosCnt); ++iNth)
    {
      if (!m_pk2PGoalZone[iNth].Init(
            uiMapInx,
            2,
            iNth,
            static_cast<unsigned __int16>(m_ui1PGoalPosCnt + iNth),
            m_pkMap))
      {
        const int mapCode = static_cast<int>(GetMapCode());
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleField::Init( %u )m_pk2PGoalZone[%d].Init( %u, %u, %d )",
          uiMapInx,
          iNth,
          uiMapInx,
          iNth,
          mapCode);
      }
    }

    m_uiRegenPosCnt =
      GetPrivateProfileIntA(buffer, "BallRegenPosCnt", 0, "./Initialize/NormalGuildBattle.ini");
    if (!m_uiRegenPosCnt)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u ) : GetPrivateProfileInt( %s, BallRegenPosCnt, 0, %s ) is 0 !",
        uiMapInx,
        buffer,
        "./Initialize/NormalGuildBattle.ini");
      return false;
    }

    m_pkRegenPos = new (std::nothrow) CGravityStoneRegener[m_uiRegenPosCnt];
    if (!m_pkRegenPos)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )new GravityStoneRegener[%u]",
        uiMapInx,
        m_uiRegenPosCnt);
    }
    for (int iNth = 0; iNth < static_cast<int>(m_uiRegenPosCnt); ++iNth)
    {
      if (!m_pkRegenPos[iNth].Init(uiMapInx, static_cast<unsigned __int16>(iNth), m_pkMap))
      {
        const int mapCode = static_cast<int>(GetMapCode());
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleField::Init( %u )m_pkRegenPos[%d].Init( %u, %d )",
          uiMapInx,
          iNth,
          iNth,
          mapCode);
      }
    }

    m_pkBall = new (std::nothrow) CGravityStone(static_cast<unsigned __int16>(uiMapInx));
    if (!m_pkBall)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleField::Init( %u )NULL == new CGravityStone",
        uiMapInx);
      return false;
    }

    m_uiMapInx = uiMapInx;
    m_bInit = true;
    return true;
  }

  bool CNormalGuildBattleField::Start(unsigned __int8 byStartPos, CPlayer *pkPlayer)
  {
    return MoveStartPos(byStartPos, 6u, pkPlayer)
        && pkPlayer->SetBindPosition(m_pkMap, m_pkStartPos[byStartPos]);
  }

  char CNormalGuildBattleField::MoveStartPos(
    unsigned __int8 byStartPos,
    unsigned __int8 byMapOutType,
    CPlayer *pkPlayer)
  {
    if (byStartPos >= 2u)
    {
      return 0;
    }
    _dummy_position *pPos = m_pkStartPos[byStartPos];
    if (!m_pkMap || !pkPlayer || !pPos)
    {
      return 0;
    }
    if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(pkPlayer->m_id.wIndex) == 99)
    {
      return 0;
    }
    pkPlayer->pc_Resurrect(0);

    float newPos[3]{};
    if (!m_pkMap->GetRandPosInDummy(pPos, newPos, true))
    {
      memcpy_0(newPos, pPos->m_fCenterPos, 0xCu);
    }
    pkPlayer->OutOfMap(m_pkMap, 0, byMapOutType, newPos);
    pkPlayer->SendMsg_GotoRecallResult(0, m_pkMap->m_pMapSet->m_dwIndex, newPos, byMapOutType);
    return 1;
  }

  unsigned __int8 CNormalGuildBattleField::DropBall(CPlayer *pkPlayer)
  {
    if (m_bInit)
    {
      return m_pkBall->Drop(pkPlayer);
    }
    return 110;
  }

  unsigned __int8 CNormalGuildBattleField::TakeBall(int iPortalInx, CPlayer *pkPlayer)
  {
    if (!m_bInit || !pkPlayer || !m_pkBall || !m_pkRegenPos)
    {
      return 110;
    }
    if (pkPlayer->IsRidingUnit())
    {
      return static_cast<unsigned __int8>(-117);
    }
    if (pkPlayer->IsRecallAnimus())
    {
      return static_cast<unsigned __int8>(-85);
    }

    int regenIndex = -1;
    for (unsigned int index = 0; index < m_uiRegenPosCnt; ++index)
    {
      if (m_pkRegenPos[index].GetPortalInx() == iPortalInx)
      {
        regenIndex = static_cast<int>(index);
        break;
      }
    }
    if (regenIndex < 0)
    {
      return static_cast<unsigned __int8>(-119);
    }

    if (!m_pkBall->m_bLive)
    {
      if (CheatRegenStone(static_cast<unsigned int>(regenIndex)) < 0)
      {
        return static_cast<unsigned __int8>(-119);
      }
    }

    if (m_pkBall->m_pkOwner && m_pkBall->m_pkOwner != pkPlayer)
    {
      m_pkBall->m_pkOwner->ClearGravityStone();
    }
    m_pkBall->m_pkOwner = pkPlayer;
    m_pkBall->m_dwTakeLimitTime = GetLoopTime();
    pkPlayer->m_bTakeGravityStone = true;
    return 0;
  }

  int CNormalGuildBattleField::CheatRegenStone(unsigned int uiPos)
  {
    if (!m_bInit || !m_pkMap || !m_pkBall || !m_pkRegenPos || uiPos >= m_uiRegenPosCnt)
    {
      return -1;
    }

    CGravityStoneRegener *regen = &m_pkRegenPos[uiPos];
    if (!regen->m_pkRegenPos)
    {
      return -1;
    }

    _object_create_setdata createData{};
    createData.m_nLayerIndex = 0;
    createData.m_pMap = m_pkMap;
    createData.m_pRecordSet = nullptr;
    createData.m_fStartPos[0] = regen->m_pkRegenPos->m_fCenterPos[0];
    createData.m_fStartPos[1] = regen->m_pkRegenPos->m_fCenterPos[1];
    createData.m_fStartPos[2] = regen->m_pkRegenPos->m_fCenterPos[2];

    m_pkBall->Clear();
    if (!m_pkBall->Regen(&createData))
    {
      return -1;
    }

    return regen->GetPortalInx();
  }

  int CNormalGuildBattleField::CheatRegenStone(CPlayer *pkPlayer)
  {
    if (!m_bInit || !pkPlayer || !m_pkBall)
    {
      return -1;
    }

    float startPos[3]{};
    if (!pkPlayer->m_pCurMap
        || !pkPlayer->m_pCurMap->GetRandPosVirtualDumExcludeStdRange(pkPlayer->m_fCurPos, 20, 10, startPos))
    {
      startPos[0] = pkPlayer->m_fCurPos[0];
      startPos[1] = pkPlayer->m_fCurPos[1];
      startPos[2] = pkPlayer->m_fCurPos[2];
    }

    _object_create_setdata createData{};
    createData.m_nLayerIndex = 0;
    createData.m_pMap = m_pkMap;
    createData.m_pRecordSet = nullptr;
    memcpy_0(createData.m_fStartPos, startPos, sizeof(createData.m_fStartPos));

    m_pkBall->Destroy();
    if (m_pkBall->Regen(&createData))
    {
      return 0;
    }
    return -3;
  }

  unsigned __int8 CNormalGuildBattleField::CheatTakeStone(int iPortalInx, CPlayer *pkPlayer)
  {
    return TakeBall(iPortalInx, pkPlayer);
  }

  unsigned __int8 CNormalGuildBattleField::CheatGetStone(CPlayer *pkPlayer)
  {
    if (!m_bInit || !pkPlayer || !m_pkBall)
    {
      return 0;
    }

    if (!m_pkBall->m_bLive)
    {
      if (CheatRegenStone(0u) < 0)
      {
        return 1;
      }
    }

    if (m_pkBall->m_pkOwner && m_pkBall->m_pkOwner != pkPlayer)
    {
      m_pkBall->m_pkOwner->ClearGravityStone();
    }
    m_pkBall->m_pkOwner = pkPlayer;
    m_pkBall->m_dwTakeLimitTime = GetLoopTime();
    pkPlayer->m_bTakeGravityStone = true;
    return 0;
  }

  void CNormalGuildBattleField::GetPortalIndexInfo(
    int *iRedPortalInx,
    int *iBluePortalInx,
    int *piRegenPortalInx)
  {
    if (m_ui1PGoalPosCnt && m_pk1PGoalZone && iRedPortalInx)
    {
      *iRedPortalInx = m_pk1PGoalZone->GetPortalInx();
    }
    if (m_ui2PGoalPosCnt && m_pk2PGoalZone && iBluePortalInx)
    {
      *iBluePortalInx = m_pk2PGoalZone->GetPortalInx();
    }

    if (piRegenPortalInx && m_uiRegenPosCnt >= 3 && m_pkRegenPos)
    {
      piRegenPortalInx[0] = m_pkRegenPos[0].GetPortalInx();
      piRegenPortalInx[1] = m_pkRegenPos[1].GetPortalInx();
      piRegenPortalInx[2] = m_pkRegenPos[2].GetPortalInx();
    }
  }

  unsigned __int8 CNormalGuildBattleField::CheatDropStone(CPlayer *pkPlayer)
  {
    if (!m_bInit)
    {
      return 1;
    }

    m_pkBall->Destroy();
    const unsigned __int8 dropResult = m_pkBall->Drop(pkPlayer);
    if (dropResult)
    {
      return dropResult;
    }

    char msg[4]{};
    *reinterpret_cast<unsigned int *>(msg) = pkPlayer->m_dwObjSerial;
    unsigned __int8 type[2] = {27, 81};
    g_Network.m_pProcess[0]->LoadSendMsg(pkPlayer->m_ObjID.m_wIndex, type, msg, 4u);
    return 0;
  }

  unsigned int CNormalGuildBattleField::GetMapCode()
  {
    if (m_pkMap)
    {
      return static_cast<unsigned int>(m_pkMap->m_nMapCode);
    }
    return 0xFFFFFFFFu;
  }

  unsigned int CNormalGuildBattleField::GetMapID()
  {
    return m_uiMapInx;
  }

  CNormalGuildBattleLogger::CNormalGuildBattleLogger() : m_pkLogger(nullptr)
  {
  }

  bool CNormalGuildBattleLogger::Init()
  {
    if (m_pkLogger)
    {
      return true;
    }
    m_pkLogger = new (std::nothrow) CLogFile();
    if (m_pkLogger)
    {
      return true;
    }
    g_Main.m_logLoadingError.Write("CGuildBattleLogger::Init() new CLogFile Fail!");
    return false;
  }

  void CNormalGuildBattleLogger::CreateLogFile(char *szLogName)
  {
    const unsigned int korLocalTime = GetKorLocalTime();
    char buffer[144]{};
    if (szLogName)
    {
      sprintf(buffer, "..\\ZoneServerLog\\Systemlog\\GuildBattle\\GuildBattle%s%d.log", szLogName, korLocalTime);
    }
    else
    {
      sprintf(buffer, "..\\ZoneServerLog\\Systemlog\\GuildBattle\\GuildBattle%d.log", korLocalTime);
    }
    m_pkLogger->SetWriteLogFile(buffer, 1, 0, 1, 1);
  }

  void CNormalGuildBattleLogger::Log(char *fmt, ...)
  {
    if (!m_pkLogger)
    {
      return;
    }
    va_list va;
    va_start(va, fmt);
    m_pkLogger->WriteFromArg(fmt, va);
    va_end(va);
  }

  CNormalGuildBattleGuildMember::CNormalGuildBattleGuildMember()
  {
    Clear();
  }

  void CNormalGuildBattleGuildMember::Clear()
  {
    m_dwSerial = 0;
    m_bRestart = false;
    m_pOldBindMapData = nullptr;
    m_pOldBindDummyData = nullptr;
    m_szOldBindMapCode[0] = 0;
    m_szOldBindDummy[0] = 0;
    m_dPvpPoint = 0.0;
    m_pkMember = nullptr;
    m_usGoalCnt = 0;
    m_usKillCnt = 0;
  }

  bool CNormalGuildBattleGuildMember::IsEmpty()
  {
    return m_dwSerial == 0;
  }

  bool CNormalGuildBattleGuildMember::IsExist()
  {
    return m_pkMember && m_pkMember->IsFill() && m_pkMember->pPlayer;
  }

  bool CNormalGuildBattleGuildMember::IsEnableStart()
  {
    return !m_pkMember->pPlayer->IsMapLoading() && !m_pkMember->pPlayer->IsRidingShip();
  }

  unsigned int CNormalGuildBattleGuildMember::GetSerial()
  {
    return m_dwSerial;
  }

  unsigned __int16 CNormalGuildBattleGuildMember::GetIndex()
  {
    if (IsEmpty())
    {
      return static_cast<unsigned __int16>(-1);
    }
    return m_pkMember->pPlayer->m_ObjID.m_wIndex;
  }

  CPlayer *CNormalGuildBattleGuildMember::GetPlayer()
  {
    if (IsEmpty())
    {
      return nullptr;
    }
    if (m_pkMember->pPlayer)
    {
      return m_pkMember->pPlayer;
    }
    return nullptr;
  }

  void CNormalGuildBattleGuildMember::StockOldInfo()
  {
    if (m_pkMember && m_pkMember->pPlayer)
    {
      m_dPvpPoint = m_pkMember->pPlayer->m_Param.GetPvPPoint();
      m_pOldBindMapData = m_pkMember->pPlayer->GetBindMapData();
      m_pOldBindDummyData = m_pkMember->pPlayer->GetBindDummy();
      const char *bindMapCode = m_pkMember->pPlayer->m_pUserDB->m_AvatorData.dbAvator.m_szBindMapCode;
      const char *bindDummy = m_pkMember->pPlayer->m_pUserDB->m_AvatorData.dbAvator.m_szBindDummy;
      strcpy_0(m_szOldBindMapCode, bindMapCode);
      strcpy_0(m_szOldBindDummy, bindDummy);
    }
  }

  void CNormalGuildBattleGuildMember::SetReStartFlag()
  {
    m_bRestart = true;
  }

  void CNormalGuildBattleGuildMember::SetBattleState(bool bFlag, unsigned __int8 byColorInx)
  {
    m_pkMember->pPlayer->pc_SetInGuildBattle(bFlag, byColorInx);
  }

  void CNormalGuildBattleGuildMember::Send(unsigned __int8 *byType, char *pSend, unsigned __int16 uiSize)
  {
    if (IsExist())
    {
      const unsigned int clientIndex = GetIndex();
      g_Network.m_pProcess[0]->LoadSendMsg(clientIndex, byType, pSend, uiSize);
    }
  }

  void CNormalGuildBattleGuildMember::CleanUpBattle()
  {
    if (IsExist())
    {
      SetBattleState(false, 0xFFu);
      Clear();
    }
  }

  void CNormalGuildBattleGuildMember::ReturnBindPos()
  {
    CPlayer *player = m_pkMember->pPlayer;
    player->SetBindMapData(m_pOldBindMapData);
    player->SetBindDummy(m_pOldBindDummyData);
    player->m_pUserDB->Update_Bind(m_szOldBindMapCode, m_szOldBindDummy, true);
  }

  void CNormalGuildBattleGuildMember::NetClose()
  {
    ReturnBindPos();
    m_dPvpPoint = m_pkMember->pPlayer->m_Param.GetPvPPoint();
  }

  void CNormalGuildBattleGuildMember::PushDQSPvpPoint(int dwPvpPoint)
  {
    _qry_case_addpvppoint query{};
    query.dwSerial = m_dwSerial;
    query.dwPoint = static_cast<unsigned int>(dwPvpPoint);
    query.dwCashBag = 0;
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x0Du, reinterpret_cast<char *>(&query), static_cast<int>(query.size()));
    g_Main.m_logPvP.Write(
      "[ %d ] type: guildbattle  >> pvp : %d  last: %.0f",
      m_dwSerial,
      dwPvpPoint,
      static_cast<double>(m_dPvpPoint) + static_cast<double>(dwPvpPoint));
  }

  CNormalGuildBattleGuild::CNormalGuildBattleGuild(unsigned __int8 byID) : m_byID(byID)
  {
    Clear();
  }

  void CNormalGuildBattleGuild::Clear()
  {
    m_byColorInx = 0;
    m_dwGoalCnt = 0;
    m_dwScore = 0;
    m_dwKillPoint = 0;
    m_pkGuild = nullptr;
    for (int j = 0; j < 150; ++j)
    {
      m_kMember[j].Clear();
    }
    m_byNotifyPositionMemberCnt = 0;
    for (int k = 0; k < 10; ++k)
    {
      m_pkNotifyPositionMember[k] = nullptr;
    }
    m_dwCurJoinMember = 0;
    m_dwKillCountSum = 0;
    m_dwMaxJoinMemberCnt = 0;
  }

  void CNormalGuildBattleGuild::SetGuild(CGuild *pkGuild)
  {
    m_pkGuild = pkGuild;
  }

  CGuild *CNormalGuildBattleGuild::GetGuild()
  {
    return m_pkGuild;
  }

  unsigned int CNormalGuildBattleGuild::GetGuildSerial()
  {
    if (m_pkGuild)
    {
      return m_pkGuild->m_dwSerial;
    }
    return 0;
  }

  char *CNormalGuildBattleGuild::GetGuildName()
  {
    if (m_pkGuild)
    {
      return m_pkGuild->m_wszName;
    }
    return nullptr;
  }

  unsigned __int8 CNormalGuildBattleGuild::GetGuildRace()
  {
    if (m_pkGuild)
    {
      return m_pkGuild->m_byRace;
    }
    return static_cast<unsigned __int8>(-1);
  }

  unsigned __int8 CNormalGuildBattleGuild::GetColorInx()
  {
    return m_byColorInx;
  }

  char *CNormalGuildBattleGuild::GetANSIGuildName()
  {
    if (m_pkGuild)
    {
      return m_pkGuild->m_aszName;
    }
    return nullptr;
  }

  char CNormalGuildBattleGuild::IsJoinMember(unsigned int dwSerial)
  {
    if (!m_pkGuild)
    {
      return 0;
    }
    for (int j = 0; j < 50; ++j)
    {
      if (!m_kMember[j].IsEmpty() && dwSerial == m_kMember[j].GetSerial())
      {
        return 1;
      }
    }
    return 0;
  }

  __int64 CNormalGuildBattleGuild::GetMember(unsigned int dwSerial)
  {
    if (!m_pkGuild)
    {
      return 0;
    }
    for (int j = 0; j < 50; ++j)
    {
      if (!m_kMember[j].IsEmpty() && dwSerial == m_kMember[j].GetSerial())
      {
        return j;
      }
    }
    return -1;
  }

  void CNormalGuildBattleGuild::AskJoin(
    int n,
    unsigned int dwSerial,
    unsigned __int8 GuildBattleNumber,
    char *wszDestGuild,
    CNormalGuildBattleLogger *kLogger)
  {
    if (!IsJoinMember(dwSerial) && GuildBattleNumber > m_dwCurJoinMember)
    {
      AskJoin(n, wszDestGuild, kLogger);
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleGuild::AskJoin( n(%d), dwSerial(%u), GuildBattleNumber(%u), %s ) : Ask Join",
        n,
        dwSerial,
        GuildBattleNumber,
        wszDestGuild);
    }
  }

  void CNormalGuildBattleGuild::AskJoin(
    unsigned int n,
    char *wszDestGuildName,
    CNormalGuildBattleLogger *kLogger)
  {
    if (m_pkGuild && wszDestGuildName)
    {
      char destination[44]{};
      strcpy_0(destination, wszDestGuildName);
      unsigned __int8 pbyType[2] = {27, 55};
      g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, destination, 0x11u);
      CGuildBattleLogger::Instance()->Log("CNormalGuildBattleGuild::AskJoin( n(%d), %s )", n, wszDestGuildName);
    }
}

  void CNormalGuildBattleGuild::LogIn(
    int n,
    unsigned int dwSerial,
    unsigned __int8 GuildBattleNumber,
    char *wszDestGuild,
    unsigned int uiID,
    CNormalGuildBattleField *pkField,
    CNormalGuildBattleLogger *kLogger)
  {
    const int iMember = static_cast<int>(GetMember(dwSerial));
    if (iMember >= 0)
    {
      CPlayer *player = m_kMember[iMember].GetPlayer();
      SendOhterNotifyCommitteeMemberPosition(player);
      CPlayer *selfPlayer = m_kMember[iMember].GetPlayer();
      SendSelfNotifyCommitteeMemberPositionList(selfPlayer);
      MoveMember(iMember, uiID, pkField, kLogger);
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleGuild::LogIn( n(%d), dwSerial(%u), GuildBattleNumber(%u), %s, uiID(%u), pkField)",
        n,
        dwSerial,
        GuildBattleNumber,
        wszDestGuild,
        uiID);
    }
    else if (GuildBattleNumber > m_dwCurJoinMember)
    {
      AskJoin(n, wszDestGuild, kLogger);
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleGuild::LogIn( n(%d), dwSerial(%u), GuildBattleNumber(%u), %s, uiID(%u), pkField) : Ask Join",
        n,
        dwSerial,
        GuildBattleNumber,
        wszDestGuild,
        uiID);
    }
  }

  void CNormalGuildBattleGuild::SendMsg(unsigned __int8 *byType, char *pMsg, unsigned __int16 uiSize)
  {
    for (int j = 0; j < 50; ++j)
    {
      if (m_kMember[j].IsExist())
      {
        const unsigned __int16 index = m_kMember[j].GetIndex();
        g_Network.m_pProcess[0]->LoadSendMsg(index, byType, pMsg, uiSize);
      }
    }
  }

  void CNormalGuildBattleGuild::SendMsg(unsigned __int8 *byType, char *pMsg, unsigned __int16 uiSize, int iExeceptMemberInx)
  {
    for (int j = 0; j < 50; ++j)
    {
      if (iExeceptMemberInx != j && m_kMember[j].IsExist())
      {
        const unsigned __int16 index = m_kMember[j].GetIndex();
        g_Network.m_pProcess[0]->LoadSendMsg(index, byType, pMsg, uiSize);
      }
    }
  }

  void CNormalGuildBattleGuild::SendMsg(
    unsigned __int8 *byType,
    char *pMsg,
    unsigned __int16 uiSize,
    unsigned int dwSerial)
  {
    for (int j = 0; j < 50; ++j)
    {
      if (dwSerial != m_kMember[j].GetSerial() && m_kMember[j].IsExist())
      {
        const unsigned __int16 index = m_kMember[j].GetIndex();
        g_Network.m_pProcess[0]->LoadSendMsg(index, byType, pMsg, uiSize);
      }
    }
  }

  void CNormalGuildBattleGuild::SendOhterNotifyCommitteeMemberPosition(CPlayer *pkPlayer)
  {
    if (!pkPlayer)
    {
      return;
    }
    char msg[9]{};
    *reinterpret_cast<unsigned int *>(msg + 3) = pkPlayer->m_dwObjSerial;
    msg[2] = static_cast<char>(m_byColorInx);
    *reinterpret_cast<unsigned __int16 *>(msg) = static_cast<unsigned __int16>(pkPlayer->m_fCurPos[0]);
    *reinterpret_cast<unsigned __int16 *>(msg + 5) = static_cast<unsigned __int16>(pkPlayer->m_fCurPos[2]);

    unsigned __int8 byType[2] = {27, 82};
    SendMsg(byType, msg, 9u, pkPlayer->m_dwObjSerial);
  }

  void CNormalGuildBattleGuild::SendSelfNotifyCommitteeMemberPositionList(CPlayer *pkPlayer)
  {
    if (!pkPlayer)
    {
      return;
    }

    unsigned __int8 byType[2] = {27, 82};
    for (int j = 0; j < 10; ++j)
    {
      if (m_pkNotifyPositionMember[j])
      {
        if (pkPlayer->m_dwObjSerial != m_pkNotifyPositionMember[j]->GetSerial())
        {
          CPlayer *player = m_pkNotifyPositionMember[j]->GetPlayer();
          char msg[9]{};
          *reinterpret_cast<unsigned int *>(msg + 3) = player->m_dwObjSerial;
          msg[2] = static_cast<char>(m_byColorInx);
          *reinterpret_cast<unsigned __int16 *>(msg) = static_cast<unsigned __int16>(player->m_fCurPos[0]);
          *reinterpret_cast<unsigned __int16 *>(msg + 5) = static_cast<unsigned __int16>(player->m_fCurPos[2]);
          g_Network.m_pProcess[0]->LoadSendMsg(pkPlayer->m_ObjID.m_wIndex, byType, msg, 9u);
        }
      }
    }
  }

  void CNormalGuildBattleGuild::NotifyLeftMinuteBeforeStart(char byLeftMin)
  {
    if (!m_pkGuild)
    {
      return;
    }

    char msg[32]{};
    msg[0] = byLeftMin;
    unsigned __int8 type[2] = {27, 65};
    for (int index = 0; index < 50; ++index)
    {
      if (m_kMember[index].IsExist())
      {
        g_Network.m_pProcess[0]->LoadSendMsg(m_kMember[index].GetIndex(), type, msg, 1u);
      }
    }
  }

  void CNormalGuildBattleGuild::SendRegenBall(int iPortalInx)
  {
    if (!m_pkGuild)
    {
      return;
    }

    char msg[32]{};
    *reinterpret_cast<int *>(msg) = iPortalInx;

    unsigned __int8 type[2] = {27, 69};
    for (int index = 0; index < 50; ++index)
    {
      if (m_kMember[index].IsExist())
      {
        g_Network.m_pProcess[0]->LoadSendMsg(m_kMember[index].GetIndex(), type, msg, 4u);
      }
    }
  }

  void CNormalGuildBattleGuild::SendGetGravityStone(
    CNormalGuildBattleGuild *pkTakeGuild,
    CPlayer *pkPlayer,
    int iTakePortalInx)
  {
    if (!m_pkGuild || !pkPlayer || !pkTakeGuild)
    {
      return;
    }

    _guild_battle_get_gravity_stone_result_zocl msg{};
    msg.byRet = 0;
    strcpy_0(msg.wszGuildName, pkTakeGuild->GetGuildName());
    strcpy_0(msg.wszCharName, pkPlayer->m_Param.GetCharNameW());
    msg.iTakePortalInx = iTakePortalInx;
    msg.byColor = pkTakeGuild->GetColorInx();

    unsigned __int8 type[2] = {27, 72};
    const unsigned __int16 length = static_cast<unsigned __int16>(msg.size());

    for (int index = 0; index < 50; ++index)
    {
      if (m_kMember[index].IsExist())
      {
        g_Network.m_pProcess[0]->LoadSendMsg(m_kMember[index].GetIndex(), type, reinterpret_cast<char *>(&msg), length);
      }
    }
  }

  void CNormalGuildBattleGuild::SendDeleteNotifyPositionMember(int iMemberInx)
  {
    CPlayer *player = m_kMember[iMemberInx].GetPlayer();
    if (!player)
    {
      return;
    }

    char msg[4]{};
    *reinterpret_cast<unsigned int *>(msg) = player->m_dwObjSerial;
    unsigned __int8 byType[2] = {27, 83};
    SendMsg(byType, msg, 4u, iMemberInx);
  }

  void CNormalGuildBattleGuild::MoveMap(unsigned int uiID, CNormalGuildBattleField *pkField)
  {
    if (!m_pkGuild || !pkField)
    {
      return;
    }

    m_pkGuild->m_bInGuildBattle = true;

    unsigned __int8 type[2]{};
    type[0] = 27;
    type[1] = 66;

    char msg[1]{static_cast<char>(-1)};
    for (int index = 0; index < 50; ++index)
    {
      if (!m_kMember[index].IsExist())
      {
        continue;
      }

      if (m_kMember[index].IsEnableStart())
      {
        m_kMember[index].StockOldInfo();
        m_kMember[index].SetBattleState(true, m_byColorInx);
        CPlayer *player = m_kMember[index].GetPlayer();
        pkField->Start(m_byColorInx, player);
      }
      else
      {
        g_Network.m_pProcess[0]->LoadSendMsg(m_kMember[index].GetIndex(), type, msg, 1u);
      }
    }
  }

  char CNormalGuildBattleGuild::SetReStartFlag(unsigned int dwSerial)
  {
    const int member = static_cast<int>(GetMember(dwSerial));
    if (member < 0)
    {
      return 0;
    }

    m_kMember[member].SetReStartFlag();
    return 1;
  }

  char CNormalGuildBattleGuild::MoveMember(
    int iMember,
    unsigned int uiID,
    CNormalGuildBattleField *pkField,
    CNormalGuildBattleLogger *kLogger)
  {
    if (!m_pkGuild || !pkField || iMember < 0)
    {
      return 0;
    }
    if (!m_kMember[iMember].IsExist())
    {
      return 0;
    }
    if (m_kMember[iMember].IsEnableStart())
    {
      m_kMember[iMember].StockOldInfo();
      m_kMember[iMember].SetBattleState(true, m_byColorInx);
      CPlayer *player = m_kMember[iMember].GetPlayer();
      if (pkField->Start(m_byColorInx, player))
      {
        unsigned int objSerial = player ? player->m_dwObjSerial : 0;
        const char *charName = player ? player->m_Param.GetCharNameW() : "NULL";
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleGuild::MoveMember( iMember(%d), uiID(%u), pkField ) : %s(%u) Move Start Position!",
          iMember,
          uiID,
          charName,
          objSerial);
        return 1;
      }
      return 0;
    }

    unsigned __int8 byType[2] = {27, 66};
    char msg[1] = {static_cast<char>(-1)};
    const unsigned __int16 index = m_kMember[iMember].GetIndex();
    g_Network.m_pProcess[0]->LoadSendMsg(index, byType, msg, 1u);
    return 1;
  }

  char CNormalGuildBattleGuild::NetClose(bool bInGuildBattle, unsigned int dwSerial, CNormalGuildBattleLogger *kLogger)
  {
const int iMemberInx = static_cast<int>(GetMember(dwSerial));
    if (iMemberInx < 0)
    {
      return 0;
    }
    if (bInGuildBattle)
    {
      SendDeleteNotifyPositionMember(iMemberInx);
      m_kMember[iMemberInx].NetClose();
    }
    if (m_dwCurJoinMember)
    {
      --m_dwCurJoinMember;
    }
    return 1;
  }

  bool CNormalGuildBattleGuild::IsMember(unsigned int dwSerial)
  {
    return GetMember(dwSerial) >= 0;
  }

  void CNormalGuildBattleGuild::LeaveGuild(
    unsigned int dwSerial,
    bool bInGuildBattle,
    CNormalGuildBattleLogger *kLogger)
  {
const int member = static_cast<int>(GetMember(dwSerial));
    if (member < 0)
    {
      return;
    }

    if (bInGuildBattle)
    {
      m_kMember[member].ReturnBindPos();
      unsigned __int8 byType[2] = {27, 91};
      m_kMember[member].Send(byType, nullptr, 0);
    }

    m_kMember[member].CleanUpBattle();
    if (m_dwCurJoinMember)
    {
      --m_dwCurJoinMember;
    }
  }

  void CNormalGuildBattleGuild::ReturnBindPosAll()
  {
    for (int j = 0; j < 150; ++j)
    {
      if (m_kMember[j].IsExist())
      {
        m_kMember[j].ReturnBindPos();
      }
    }
  }

  CNormalGuildBattle::CNormalGuildBattle(unsigned int dwID)
    : m_dwID(dwID),
      m_bInit(false),
      m_kLogger(),
      m_byGuildBattleNumber(0),
      m_k1P(0),
      m_k2P(1),
      m_pkField(nullptr),
      m_byWinResult(0),
      m_pkWin(nullptr),
      m_pkLose(nullptr),
      m_pkRed(nullptr),
      m_pkBlue(nullptr),
      m_pkStateList(&NORMAL_GUILD_BATTLE_STATE_NULL_11)
  {
  }

  bool CNormalGuildBattle::CreateLogger()
  {
    return m_kLogger.Init();
  }

  void CNormalGuildBattle::CreateLogFile()
  {
    bool bSuccess = true;
    const char *fallback = "NONE";
    char *leftName = m_k1P.GetANSIGuildName();
    char *rightName = m_k2P.GetANSIGuildName();
    if (!leftName)
    {
      leftName = const_cast<char *>(fallback);
      bSuccess = false;
    }
    if (!rightName)
    {
      rightName = const_cast<char *>(fallback);
      bSuccess = false;
    }

    char buffer[1032]{};
    sprintf(buffer, "[%u]_(%02d:00)_[%s]:[%s]_", m_dwID, m_dwID % 0x17u, leftName, rightName);
    m_kLogger.CreateLogFile(buffer);
    if (!bSuccess)
    {
      CGuildBattleLogger::Instance()->Log("CNormalGuildBattle::CreateLogFile() Fail!");
    }
  }

  void CNormalGuildBattle::Clear()
  {
    m_bInit = false;
    m_k1P.Clear();
    m_k2P.Clear();
    m_pkField = nullptr;
    m_byWinResult = 0;
    m_pkWin = nullptr;
    m_pkLose = nullptr;
    m_pkRed = nullptr;
    m_pkBlue = nullptr;
    m_byGuildBattleNumber = 0;
    m_pkStateList = &NORMAL_GUILD_BATTLE_STATE_NULL_11;
  }

  char CNormalGuildBattle::Init(
    bool bToday,
    unsigned int uiDayID,
    unsigned int dwID,
    unsigned int dwP1GuildSerial,
    unsigned int dwP2GuildSerial,
    unsigned int dwMapID,
    unsigned __int8 byNumber)
  {
    if (m_dwID != dwID)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattle::Init(%u,%u,%u,%u)GetID() != dwID Fail!",
        dwID,
        dwP1GuildSerial,
        dwP2GuildSerial,
        dwMapID);
      return 0;
    }

    if (!byNumber || byNumber > 0x32u)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattle::Init(%u,%u,%u,%u)byNumber(%u) Invalid!",
        dwID,
        dwP1GuildSerial,
        dwP2GuildSerial,
        dwMapID,
        byNumber);
      return 0;
    }

    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    CNormalGuildBattleStateListPool *stateListPool = CNormalGuildBattleStateListPool::Instance();
    CGuildBattleScheduleManager *scheduleManager = CGuildBattleScheduleManager::Instance();
    CGuild *pk1P = GetGuildDataFromSerial(g_Guild, 500, dwP1GuildSerial);
    CGuild *pk2P = GetGuildDataFromSerial(g_Guild, 500, dwP2GuildSerial);
    CNormalGuildBattleField *pkField = nullptr;
    CGuildBattleStateList *pkStateList = nullptr;
    CGuildBattleSchedule *updated = nullptr;
    if (pk1P)
    {
      pkField = fieldList->GetField(dwMapID);
    }
    if (pkField)
    {
      pkStateList = stateListPool->Get(m_dwID);
      updated = scheduleManager->UpdateUseFlag(uiDayID, dwMapID, m_dwID);
      if (pkStateList && updated)
      {
        updated->SetStateList(pkStateList);
      }
    }

    if (updated && pkField && pk1P && pk2P && pkStateList)
    {
      Init(pk1P, pk2P, pkField, byNumber, static_cast<CNormalGuildBattleStateList *>(pkStateList));
      if (!bToday)
      {
        pk1P->SetCopmlteGuildBattleSuggest();
        pk2P->SetCopmlteGuildBattleSuggest();
      }
      pkStateList->SetReady();
      ATL::CTimeSpan result;
      const ATL::CTimeSpan *battleTime = updated->GetBattleTime(&result);
      static_cast<CNormalGuildBattleStateList *>(pkStateList)->SetBattleTime(*battleTime);

      const char *dayLabel = bToday ? "today" : "tommorow";
      const unsigned int mapID = m_pkField->GetMapID();
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattle::Init( %u, %u, %u, %u, %u, %u ) : %s : Init( %s, %s, %d, %u, pkStateList )",
        uiDayID,
        dwID,
        dwP1GuildSerial,
        dwP2GuildSerial,
        dwMapID,
        byNumber,
        dayLabel,
        pk1P->m_aszName,
        pk2P->m_aszName,
        mapID,
        byNumber);
      return 1;
    }

    const char *dayLabel = bToday ? "today" : "tommorow";
    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattle::Init( %u, %u, %u, %u, %u, %u ) : %s : Init( ... ) Skip!",
      uiDayID,
      dwID,
      dwP1GuildSerial,
      dwP2GuildSerial,
      dwMapID,
      byNumber,
      dayLabel);
    return 0;
  }

  void CNormalGuildBattle::Init(
    CGuild *pk1P,
    CGuild *pk2P,
    CNormalGuildBattleField *pkField,
    unsigned __int8 byNumber,
    CNormalGuildBattleStateList *pkStateList)
  {
    m_k1P.SetGuild(pk1P);
    m_k2P.SetGuild(pk2P);
    m_pkField = pkField;
    m_byGuildBattleNumber = byNumber;
    m_pkStateList = pkStateList;
    m_bInit = true;
  }

  void CNormalGuildBattle::SendWebAddScheduleInfo()
  {
    char msg[63]{};
    msg[0] = static_cast<char>(m_dwID % 23);
    msg[1] = static_cast<char>(m_k1P.GetGuildRace());
    *reinterpret_cast<unsigned int *>(msg + 2) = m_k1P.GetGuildSerial();
    msg[6] = m_k1P.GetGuild()->m_byGrade;
    strcpy_s(msg + 7, 17, m_k1P.GetGuildName());
    *reinterpret_cast<unsigned int *>(msg + 24) = m_k2P.GetGuildSerial();
    msg[28] = m_k2P.GetGuild()->m_byGrade;
    strcpy_s(msg + 29, 17, m_k2P.GetGuildName());
    msg[46] = static_cast<char>(m_byGuildBattleNumber);
    *reinterpret_cast<int *>(msg + 47) = 5000;

    const char *mapCode = "";
    if (m_pkField && m_pkField->m_pkMap && m_pkField->m_pkMap->m_pMapSet)
    {
      mapCode = m_pkField->m_pkMap->m_pMapSet->m_strCode;
    }
    strcpy_s(msg + 51, 12, mapCode);

    unsigned __int8 type[2]{51, 16};
    if (g_Main.m_bConnectedWebAgentServer)
    {
      g_Network.m_pProcess[2]->LoadSendMsg(g_Main.m_byWebAgentServerNetInx, type, msg, 0x3Fu);
    }
  }

  void CNormalGuildBattle::AddComplete(unsigned __int8 byRet)
  {
    CGuild *srcGuild = m_k1P.GetGuild();
    CGuild *destGuild = m_k2P.GetGuild();
    destGuild->AddScheduleComplete(byRet, srcGuild);
    m_pkStateList->SetWait();
    SendWebAddScheduleInfo();
  }

  bool CNormalGuildBattle::IsEmpty()
  {
    return !m_bInit;
  }

  bool CNormalGuildBattle::IsMemberGuild(unsigned int dwGuildSerial)
  {
    return dwGuildSerial == m_k1P.GetGuildSerial() || dwGuildSerial == m_k2P.GetGuildSerial();
  }

  bool CNormalGuildBattle::IsReadyOrCountState()
  {
    return m_pkStateList->IsReadyOrCountState();
  }

  bool CNormalGuildBattle::IsInBattle()
  {
    return m_pkStateList->IsInBattle();
  }

  bool CNormalGuildBattle::IsProc()
  {
    return m_pkStateList->IsProc();
  }

  void CNormalGuildBattle::Process()
  {
    m_pkStateList->Process(this);
  }

  void CNormalGuildBattle::SetReadyState()
  {
    if (m_bInit && m_pkStateList)
    {
      m_pkStateList->SetReady();
    }
  }

  unsigned __int8 CNormalGuildBattle::Start(
    CPlayer *pkPlayer,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial)
  {

    _guild_battle_start_result msg{};
    msg.iRedPortalInx = -1;
    msg.iBluePortalInx = -1;
    msg.iRegenPortalInx[0] = -1;
    msg.iRegenPortalInx[1] = -1;
    msg.iRegenPortalInx[2] = -1;

    if (dwGuildSerial == m_k1P.GetGuildSerial())
    {
      if (!m_k1P.SetReStartFlag(dwCharacSerial))
      {
        return static_cast<unsigned __int8>(-111);
      }
      msg.byColor = static_cast<char>(m_k1P.GetColorInx());
    }
    else
    {
      if (dwGuildSerial != m_k2P.GetGuildSerial())
      {
        return static_cast<unsigned __int8>(-115);
      }
      if (!m_k2P.SetReStartFlag(dwCharacSerial))
      {
        return static_cast<unsigned __int8>(-111);
      }
      msg.byColor = static_cast<char>(m_k2P.GetColorInx());
    }

    if (m_pkRed)
    {
      msg.dwLeftRedGoalCnt = m_pkRed->m_dwGoalCnt;
      msg.dwLeftRedScore = m_pkRed->m_dwScore;
    }
    if (m_pkBlue)
    {
      msg.dwRightBlueGoalCnt = m_pkBlue->m_dwGoalCnt;
      msg.dwRightBlueScore = m_pkBlue->m_dwScore;
    }

    int regenPortalInx[5]{-1, -1, -1, -1, -1};
    if (m_pkField)
    {
      m_pkField->GetPortalIndexInfo(&msg.iRedPortalInx, &msg.iBluePortalInx, regenPortalInx);
      msg.iRegenPortalInx[0] = regenPortalInx[0];
      msg.iRegenPortalInx[1] = regenPortalInx[1];
      msg.iRegenPortalInx[2] = regenPortalInx[2];
    }

    CGuildBattleSchedule *schedule = CGuildBattleSchedulePool::Instance()->GetRef(m_dwID);
    if (!schedule)
    {
      const int mapCode = m_pkField ? static_cast<int>(m_pkField->GetMapCode()) : -1;
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattle::Start( %u, %u ) : CGuildBattleSchedulePool::Instance()->GetRef(%u) NULL!",
        dwGuildSerial,
        dwCharacSerial,
        mapCode);
      return 110;
    }

    schedule->GetLeftTime(&msg.byLeftHour, &msg.byLeftMin, &msg.byLeftSec);
    __trace("(%u)CNormalGuildBattle::Start : byColor(%u)\n", dwCharacSerial, static_cast<unsigned __int8>(msg.byColor));

    unsigned __int8 type[2]{};
    type[0] = 27;
    type[1] = 68;
    g_Network.m_pProcess[0]->LoadSendMsg(
      pkPlayer->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&msg),
      0x28u);

    char szStartLogFormat[] = "CNormalGuildBattle::Start( %u, %u ) : (%d) %s Guild : (%d) %s race(%d) Start!";
    m_kLogger.Log(
      szStartLogFormat,
      dwGuildSerial,
      dwCharacSerial,
      pkPlayer->m_Param.m_pGuild->m_dwSerial,
      pkPlayer->m_Param.m_pGuild->m_wszName,
      dwCharacSerial,
      pkPlayer->m_Param.GetCharNameW(),
      pkPlayer->m_Param.GetRaceCode());
    return 0;
  }

  unsigned __int8 CNormalGuildBattle::ReStart(
    CPlayer *pkPlayer,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial)
  {

    _guild_battle_restart_result msg{};
    msg.iRedPortalInx = -1;
    msg.iBluePortalInx = -1;
    msg.iRegenPortalInx[0] = -1;
    msg.iRegenPortalInx[1] = -1;
    msg.iRegenPortalInx[2] = -1;

    if (dwGuildSerial == m_k1P.GetGuildSerial())
    {
      if (!m_k1P.IsMember(dwCharacSerial))
      {
        return static_cast<unsigned __int8>(-111);
      }
      msg.byColor = static_cast<char>(m_k1P.GetColorInx());
    }
    else
    {
      if (dwGuildSerial != m_k2P.GetGuildSerial())
      {
        return static_cast<unsigned __int8>(-115);
      }
      if (!m_k2P.IsMember(dwCharacSerial))
      {
        return static_cast<unsigned __int8>(-111);
      }
      msg.byColor = static_cast<char>(m_k2P.GetColorInx());
    }

    if (m_pkRed)
    {
      msg.dwLeftRedGoalCnt = m_pkRed->m_dwGoalCnt;
      msg.dwLeftRedScore = m_pkRed->m_dwScore;
    }
    if (m_pkBlue)
    {
      msg.dwRightBlueGoalCnt = m_pkBlue->m_dwGoalCnt;
      msg.dwRightBlueScore = m_pkBlue->m_dwScore;
    }

    int regenPortalInx[5]{-1, -1, -1, -1, -1};
    if (m_pkField)
    {
      m_pkField->GetPortalIndexInfo(&msg.iRedPortalInx, &msg.iBluePortalInx, regenPortalInx);
      msg.iRegenPortalInx[0] = regenPortalInx[0];
      msg.iRegenPortalInx[1] = regenPortalInx[1];
      msg.iRegenPortalInx[2] = regenPortalInx[2];
    }

    CGuildBattleSchedule *schedule = CGuildBattleSchedulePool::Instance()->GetRef(m_dwID);
    if (!schedule)
    {
      const int mapCode = m_pkField ? static_cast<int>(m_pkField->GetMapCode()) : -1;
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattle::Start( %u, %u ) : CGuildBattleSchedulePool::Instance()->GetRef(%u) NULL!",
        dwGuildSerial,
        dwCharacSerial,
        mapCode);
      return 110;
    }

    schedule->GetLeftTime(&msg.byLeftHour, &msg.byLeftMin, &msg.byLeftSec);
    __trace("(%u)CNormalGuildBattle::Start : byColor(%u)\n", dwCharacSerial, static_cast<unsigned __int8>(msg.byColor));

    unsigned __int8 type[2]{};
    type[0] = 27;
    type[1] = 84;
    g_Network.m_pProcess[0]->LoadSendMsg(
      pkPlayer->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(&msg),
      0x28u);

    char szReStartLogFormat[] = "CNormalGuildBattle::Start( %u, %u ) : (%d) %s Guild : (%d) %s race(%d) Start!";
    m_kLogger.Log(
      szReStartLogFormat,
      dwGuildSerial,
      dwCharacSerial,
      pkPlayer->m_Param.m_pGuild->m_dwSerial,
      pkPlayer->m_Param.m_pGuild->m_wszName,
      dwCharacSerial,
      pkPlayer->m_Param.GetCharNameW(),
      pkPlayer->m_Param.GetRaceSexCode());
    return 0;
  }

  void CNormalGuildBattle::NotifyBeforeStart()
  {

    _guild_battle_before_start_notify msg{};
    msg.byLeftMin = static_cast<char>(ATL::CTimeSpan(0, 0, 5, 0).GetMinutes());

    const char *redGuildName = m_pkRed ? m_pkRed->GetGuildName() : nullptr;
    const char *blueGuildName = m_pkBlue ? m_pkBlue->GetGuildName() : nullptr;
    strcpy_0(msg.wszRedName, redGuildName ? redGuildName : "None");
    strcpy_0(msg.wszBlueName, blueGuildName ? blueGuildName : "None");

    unsigned __int8 type[2]{};
    type[0] = 27;
    type[1] = 88;

    const unsigned __int8 firstRace = m_k1P.GetGuildRace();
    const unsigned __int8 secondRace = m_k2P.GetGuildRace();
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *player = &g_Player[index];
      if (!player->m_bLive)
      {
        continue;
      }

      const int raceCode = player->m_Param.GetRaceCode();
      if (raceCode == firstRace || raceCode == secondRace)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x23u);
      }
    }
  }

  void CNormalGuildBattle::NotifyBattleResult(char byResult)
  {

    _guild_battle_result_notify msg{};
    msg.byResult = byResult;
    strcpy_0(msg.wszRedName, m_pkRed ? m_pkRed->GetGuildName() : "None");
    strcpy_0(msg.wszBlueName, m_pkBlue ? m_pkBlue->GetGuildName() : "None");

    unsigned __int8 type[2]{};
    type[0] = 27;
    type[1] = 89;

    const unsigned __int8 firstRace = m_k1P.GetGuildRace();
    const unsigned __int8 secondRace = m_k2P.GetGuildRace();
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *player = &g_Player[index];
      if (!player->m_bLive || player->m_bInGuildBattle)
      {
        continue;
      }

      const int raceCode = player->m_Param.GetRaceCode();
      if (raceCode == firstRace || raceCode == secondRace)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x23u);
      }
    }
  }

  void CNormalGuildBattle::SendWebBattleStartInfo()
  {
    char msg[3]{};
    msg[0] = static_cast<char>(m_dwID % 23);
    msg[1] = static_cast<char>(m_k1P.GetGuildRace());
    msg[2] = 2;

    unsigned __int8 type[2]{};
    type[0] = 51;
    type[1] = 17;
    if (g_Main.m_bConnectedWebAgentServer)
    {
      g_Network.m_pProcess[2]->LoadSendMsg(g_Main.m_byWebAgentServerNetInx, type, msg, 3u);
    }
  }

  void CNormalGuildBattle::SendWebBattleEndInfo()
  {
    char msg[3]{};
    msg[0] = static_cast<char>(m_dwID % 23);
    msg[1] = static_cast<char>(m_k1P.GetGuildRace());
    msg[2] = 3;

    unsigned __int8 type[2]{};
    type[0] = 51;
    type[1] = 17;
    if (g_Main.m_bConnectedWebAgentServer)
    {
      g_Network.m_pProcess[2]->LoadSendMsg(g_Main.m_byWebAgentServerNetInx, type, msg, 3u);
    }
  }

  void CNormalGuildBattle::SendGoalMsg(bool b1P, char *wszGuildName, CPlayer *pkPlayer)
  {
    unsigned __int8 type[2]{};
    type[0] = 27;
    type[1] = 74;

    _guild_battle_goal_result_zocl msg{};
    msg.byRet = 0;
    strcpy_0(msg.wszGuildName, wszGuildName);
    msg.dwObjSerial = pkPlayer->m_dwObjSerial;
    strcpy_0(msg.wszCharName, pkPlayer->m_Param.GetCharNameW());

    if (m_pkRed)
    {
      msg.dwLeftRedGoalCnt = m_pkRed->m_dwGoalCnt;
      msg.dwLeftRedScore = m_pkRed->m_dwScore;
    }
    if (m_pkBlue)
    {
      msg.dwRightBlueGoalCnt = m_pkBlue->m_dwGoalCnt;
      msg.dwRightBlueScore = m_pkBlue->m_dwScore;
    }

    CGuildBattleSchedule *schedule = CGuildBattleSchedulePool::Instance()->GetRef(m_dwID);
    if (!schedule)
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattle::SendKillInform() : CGuildBattleSchedulePool::Instance()->GetRef(%u) NULL!",
        m_dwID);
      return;
    }

    schedule->GetLeftTime(&msg.byLeftHour, &msg.byLeftMin, &msg.byLeftSec);

    const int raceCode = pkPlayer->m_Param.GetRaceCode();
    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *player = &g_Player[index];
      if (!player->m_bLive || player->m_bBlockGuildBattleMsg)
      {
        continue;
      }
      if (player->m_Param.GetRaceCode() == raceCode)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          player->m_ObjID.m_wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          msg.size());
      }
    }

    if (!b1P || m_k1P.GetGuildRace() == m_k2P.GetGuildRace())
    {
      if (m_k1P.GetGuildRace() != m_k2P.GetGuildRace())
      {
        m_k1P.SendMsg(type, reinterpret_cast<char *>(&msg), msg.size());
      }
    }
    else
    {
      m_k2P.SendMsg(type, reinterpret_cast<char *>(&msg), msg.size());
    }
  }

  void CNormalGuildBattle::GuildBattleResultLogNotifyWeb(const _qry_case_guild_battel_result_log *sheet)
  {
    static_assert(sizeof(_guild_battle_result_log_notify_web) == 0xA8, "guild battle web notify size mismatch");

    if (!sheet)
    {
      return;
    }

    _guild_battle_result_log_notify_web msg{};
    strcpy_s(msg.szStartTime, sizeof(msg.szStartTime), sheet->szStartTime);
    strcpy_s(msg.szEndTime, sizeof(msg.szEndTime), sheet->szEndTime);
    msg.dwRedSerial = sheet->dwRedSerial;
    strcpy_s(msg.wszRedName, sizeof(msg.wszRedName), sheet->wszRedName);
    msg.dwBlueSerial = sheet->dwBlueSerial;
    strcpy_s(msg.wszBlueName, sizeof(msg.wszBlueName), sheet->wszBlueName);
    msg.dwRedScore = sheet->dwRedScore;
    msg.dwBlueScore = sheet->dwBlueScore;
    msg.dwRedMaxJoinCnt = sheet->dwRedMaxJoinCnt;
    msg.dwBlueMaxJoinCnt = sheet->dwBlueMaxJoinCnt;
    msg.dwRedGoalCnt = sheet->dwRedGoalCnt;
    msg.dwBlueGoalCnt = sheet->dwBlueGoalCnt;
    msg.dwRedKillCntSum = sheet->dwRedKillCntSum;
    msg.dwBlueKillCntSum = sheet->dwBlueKillCntSum;
    msg.byBattleResult = sheet->byBattleResult;
    msg.dwMaxGoalCharacSerial = sheet->dwMaxGoalCharacSerial;
    strcpy_s(msg.wszMaxGoalCharacName, sizeof(msg.wszMaxGoalCharacName), sheet->wszMaxGoalCharacName);
    msg.dwMaxKillCharacSerial = sheet->dwMaxKillCharacSerial;
    strcpy_s(msg.wszMaxKillCharacName, sizeof(msg.wszMaxKillCharacName), sheet->wszMaxKillCharacName);
    msg.byJoinLimit = sheet->byJoinLimit;
    msg.dwGuildBattleCostGold = sheet->dwGuildBattleCostGold;
    strcpy_s(msg.szBattleMapCode, sizeof(msg.szBattleMapCode), sheet->szBattleMapCode);

    unsigned __int8 type[2]{};
    type[0] = 51;
    type[1] = 18;
    if (g_Main.m_bConnectedWebAgentServer)
    {
      g_Network.m_pProcess[2]->LoadSendMsg(
        g_Main.m_byWebAgentServerNetInx,
        type,
        reinterpret_cast<char *>(&msg),
        0xA8u);
    }
  }

  void CNormalGuildBattle::PushDQSWinLoseRank()
  {
    if (!m_pkWin || !m_pkLose)
    {
      return;
    }

    _qry_case_updatewinloseguildbattlerank query{};
    query.byWinRace = m_pkWin->GetGuild()->m_byRace;
    query.dwWinGuildSerial = m_pkWin->GetGuildSerial();
    query.byLoseRace = m_pkLose->GetGuild()->m_byRace;
    query.dwLoseGuildSerial = m_pkLose->GetGuildSerial();
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x1Fu, reinterpret_cast<char *>(&query), static_cast<int>(query.size()));
  }

  void CNormalGuildBattle::PushDQSDrawRank()
  {
    _qry_case_updatedrawguildbattlerank query{};
    query.by1PRace = m_k1P.GetGuild()->m_byRace;
    query.dw1PGuildSerial = m_k1P.GetGuildSerial();
    query.by2PRace = m_k2P.GetGuild()->m_byRace;
    query.dw2PGuildSerial = m_k2P.GetGuildSerial();
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x20u, reinterpret_cast<char *>(&query), static_cast<int>(query.size()));
  }

  void CNormalGuildBattle::AskJoin(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial)
  {
    if (dwGuildSerial == m_k1P.GetGuildSerial())
    {
      char *destName = m_k2P.GetGuildName();
      m_k1P.AskJoin(n, dwCharacSerial, m_byGuildBattleNumber, destName, &m_kLogger);
    }
    else if (dwGuildSerial == m_k2P.GetGuildSerial())
    {
      char *destName = m_k1P.GetGuildName();
      m_k2P.AskJoin(n, dwCharacSerial, m_byGuildBattleNumber, destName, &m_kLogger);
    }
  }

  void CNormalGuildBattle::LogIn(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial)
  {
    if (dwGuildSerial == m_k1P.GetGuildSerial())
    {
      char *destName = m_k2P.GetGuildName();
      m_k1P.LogIn(n, dwCharacSerial, m_byGuildBattleNumber, destName, m_dwID, m_pkField, &m_kLogger);
    }
    else if (dwGuildSerial == m_k2P.GetGuildSerial())
    {
      char *destName = m_k1P.GetGuildName();
      m_k2P.LogIn(n, dwCharacSerial, m_byGuildBattleNumber, destName, m_dwID, m_pkField, &m_kLogger);
    }
  }

  unsigned __int8 CNormalGuildBattle::NetClose(unsigned int dwCharacSerial, CPlayer *pkPlayer)
  {
    bool isFirstGuild = true;
    if (!m_k1P.NetClose(pkPlayer->m_bInGuildBattle, dwCharacSerial, &m_kLogger))
    {
      if (!m_k2P.NetClose(pkPlayer->m_bInGuildBattle, dwCharacSerial, &m_kLogger))
      {
        return static_cast<unsigned __int8>(-111);
      }
      isFirstGuild = false;
    }

    if (!pkPlayer->m_bInGuildBattle)
    {
      return 0;
    }

    bool droppedBall = false;
    if (pkPlayer->m_bTakeGravityStone)
    {
      const unsigned __int8 result = m_pkField->DropBall(pkPlayer);
      if (result)
      {
        CGuildBattleLogger::Instance()->Log(
          const_cast<char *>("CNormalGuildBattle::NetClose( %u ) : (%u) m_pkField->DropBall( %s ) Fail(%u)!"),
          dwCharacSerial,
          dwCharacSerial,
          pkPlayer->m_Param.GetCharNameW(),
          result);
      }
      else
      {
        NotifyDestoryBall(pkPlayer->m_dwObjSerial);
        droppedBall = true;
      }
    }

    pkPlayer->pc_SetInGuildBattle(false, 0xFFu);

    const bool isLeft = isFirstGuild ? (m_k1P.GetColorInx() != 1) : (m_k2P.GetColorInx() != 1);
    const char *dropInfo = droppedBall ? "Drop Ball" : "None";
    const char *guildName = isFirstGuild ? m_k1P.GetGuildName() : m_k2P.GetGuildName();
    const char *side = isLeft ? "Left" : "Right";
    m_kLogger.Log(
      const_cast<char *>("CNormalGuildBattle::NetClose( %u ) : %s %s %s %s"),
      dwCharacSerial,
      side,
      guildName,
      pkPlayer->m_Param.GetCharNameW(),
      dropInfo);
    return 0;
  }

  unsigned __int8 CNormalGuildBattle::LeaveGuild(CPlayer *pkPlayer)
  {
    bool isFirstGuild = true;
    bool droppedBall = false;
    const unsigned int dwSerial = pkPlayer->m_dwObjSerial;
    if (!m_k1P.IsMember(dwSerial))
    {
      if (!m_k2P.IsMember(dwSerial))
      {
        return static_cast<unsigned __int8>(-111);
      }
      isFirstGuild = false;
    }

    if (pkPlayer->m_bInGuildBattle)
    {
      if (pkPlayer->m_bTakeGravityStone)
      {
        const unsigned __int8 result = m_pkField->DropBall(pkPlayer);
        if (result)
        {
          CGuildBattleLogger::Instance()->Log(
            "CNormalGuildBattle::LeaveGuild( %u ) : (%u) m_pkField->DropBall( %s ) Fail(%u)!",
            dwSerial,
            dwSerial,
            pkPlayer->m_Param.GetCharNameW(),
            result);
        }
        else
        {
          NotifyDestoryBall(pkPlayer->m_dwObjSerial);
          droppedBall = true;
        }
      }

      bool isLeft = true;
      if (isFirstGuild)
      {
        m_k1P.LeaveGuild(dwSerial, pkPlayer->m_bInGuildBattle, &m_kLogger);
        if (m_k1P.GetColorInx() == 1)
        {
          isLeft = false;
        }
      }
      else
      {
        m_k2P.LeaveGuild(dwSerial, pkPlayer->m_bInGuildBattle, &m_kLogger);
        if (m_k2P.GetColorInx() == 1)
        {
          isLeft = false;
        }
      }

      pkPlayer->pc_SetInGuildBattle(false, 0xFFu);

      const char *dropInfo = droppedBall ? "Drop Ball" : "None";
      const char *guildName = isFirstGuild ? m_k1P.GetGuildName() : m_k2P.GetGuildName();
      const char *side = isLeft ? "Left" : "Right";
      m_kLogger.Log(
        const_cast<char *>("CNormalGuildBattle::LeaveGuild( %u ) : %s %s %s %s"),
        dwSerial,
        side,
        guildName,
        pkPlayer->m_Param.GetCharNameW(),
        dropInfo);
      return 0;
    }

    if (isFirstGuild)
    {
      m_k1P.LeaveGuild(dwSerial, pkPlayer->m_bInGuildBattle, &m_kLogger);
    }
    else
    {
      m_k2P.LeaveGuild(dwSerial, pkPlayer->m_bInGuildBattle, &m_kLogger);
    }
    return 0;
  }

  void CNormalGuildBattle::NotifyDestoryBall(unsigned int dwOwnerSerial)
  {
    char msg[4]{};
    *reinterpret_cast<unsigned int *>(msg) = dwOwnerSerial;
    unsigned __int8 byType[2] = {27, 81};
    m_k1P.SendMsg(byType, msg, 4u);
    m_k2P.SendMsg(byType, msg, 4u);
  }

  void CNormalGuildBattle::GuildBattleResultLogPushDBLog(
    _qry_case_guild_battel_result_log *sheet,
    CNormalGuildBattleGuildMember *pkTopGoalMember,
    CNormalGuildBattleGuildMember *pkTopKillMember)
  {
    if (!sheet)
    {
      return;
    }

    auto writeDefaultDate = [sheet]() {
      strcpy_s(sheet->szStartTime, sizeof(sheet->szStartTime), "1900-00-00 00:00");
      strcpy_s(sheet->szEndTime, sizeof(sheet->szEndTime), "1900-00-00 00:00");
    };

    CGuildBattleSchedule *schedule = CGuildBattleSchedulePool::Instance()->GetRef(m_dwID);
    if (schedule)
    {
      __int64 startTime = schedule->GetRealStartTime();
      ATL::CTimeSpan battleTime{};
      schedule->GetBattleTime(&battleTime);
      const __int64 endTime = startTime + battleTime.GetTotalSeconds();

      tm *startTm = localtime_2(&startTime);
      if (startTm)
      {
        sprintf_s(
          sheet->szStartTime,
          sizeof(sheet->szStartTime),
          "%04d-%02d-%02d %02d:%02d",
          startTm->tm_year + 1900,
          startTm->tm_mon + 1,
          startTm->tm_mday,
          startTm->tm_hour,
          startTm->tm_min);

        tm *endTm = localtime_2(&endTime);
        if (endTm)
        {
          sprintf_s(
            sheet->szEndTime,
            sizeof(sheet->szEndTime),
            "%04d-%02d-%02d %02d:%02d",
            endTm->tm_year + 1900,
            endTm->tm_mon + 1,
            endTm->tm_mday,
            endTm->tm_hour,
            endTm->tm_min);
        }
        else
        {
          strcpy_s(sheet->szEndTime, sizeof(sheet->szEndTime), "1900-00-00 00:00");
        }
      }
      else
      {
        writeDefaultDate();
      }
    }
    else
    {
      writeDefaultDate();
    }

    sheet->dwRedSerial = m_pkRed->GetGuildSerial();
    strcpy_s(sheet->wszRedName, sizeof(sheet->wszRedName), m_pkRed->GetGuildName());
    sheet->dwBlueSerial = m_pkBlue->GetGuildSerial();
    strcpy_s(sheet->wszBlueName, sizeof(sheet->wszBlueName), m_pkBlue->GetGuildName());
    sheet->dwRedScore = m_pkRed->m_dwScore;
    sheet->dwBlueScore = m_pkBlue->m_dwScore;
    sheet->dwRedMaxJoinCnt = m_pkRed->m_dwMaxJoinMemberCnt;
    sheet->dwBlueMaxJoinCnt = m_pkBlue->m_dwMaxJoinMemberCnt;
    sheet->dwRedGoalCnt = m_pkRed->m_dwGoalCnt;
    sheet->dwBlueGoalCnt = m_pkBlue->m_dwGoalCnt;
    sheet->dwRedKillCntSum = m_pkRed->m_dwKillCountSum;
    sheet->dwBlueKillCntSum = m_pkBlue->m_dwKillCountSum;
    sheet->byBattleResult = m_byWinResult;

    CPlayer *topGoalPlayer = nullptr;
    if (pkTopGoalMember && (topGoalPlayer = pkTopGoalMember->GetPlayer()) != nullptr)
    {
      sheet->dwMaxGoalCharacSerial = topGoalPlayer->m_dwObjSerial;
      strcpy_s(sheet->wszMaxGoalCharacName, sizeof(sheet->wszMaxGoalCharacName), topGoalPlayer->m_Param.GetCharNameW());
    }
    else
    {
      sheet->dwMaxGoalCharacSerial = 0;
      strcpy_s(sheet->wszMaxGoalCharacName, sizeof(sheet->wszMaxGoalCharacName), "NONE");
    }

    CPlayer *topKillPlayer = nullptr;
    if (pkTopKillMember && (topKillPlayer = pkTopKillMember->GetPlayer()) != nullptr)
    {
      sheet->dwMaxKillCharacSerial = topKillPlayer->m_dwObjSerial;
      strcpy_s(sheet->wszMaxKillCharacName, sizeof(sheet->wszMaxKillCharacName), topKillPlayer->m_Param.GetCharNameW());
    }
    else
    {
      sheet->dwMaxKillCharacSerial = 0;
      strcpy_s(sheet->wszMaxKillCharacName, sizeof(sheet->wszMaxKillCharacName), "NONE");
    }

    sheet->byJoinLimit = m_byGuildBattleNumber;
    sheet->dwGuildBattleCostGold = 5000;
    const char *mapCode = "NONE";
    if (m_pkField && m_pkField->m_pkMap && m_pkField->m_pkMap->m_pMapSet)
    {
      mapCode = m_pkField->m_pkMap->m_pMapSet->m_strCode;
    }
    strcpy_s(sheet->szBattleMapCode, sizeof(sheet->szBattleMapCode), mapCode);
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x54u, reinterpret_cast<char *>(sheet), sizeof(*sheet));
  }

  unsigned int CNormalGuildBattle::GetID()
  {
    return m_dwID;
  }

  CNormalGuildBattleGuild *CNormalGuildBattle::Get1P()
  {
    return &m_k1P;
  }

  CNormalGuildBattleGuild *CNormalGuildBattle::Get2P()
  {
    return &m_k2P;
  }

  unsigned __int8 CNormalGuildBattle::GetGuildBattleNumber()
  {
    return m_byGuildBattleNumber;
  }

  CNormalGuildBattleManager *CNormalGuildBattleManager::ms_Instance = nullptr;

  CNormalGuildBattleManager *CNormalGuildBattleManager::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CNormalGuildBattleManager();
    }
    return ms_Instance;
  }

  CNormalGuildBattleManager::CNormalGuildBattleManager()
    : m_bLoad(false),
      m_bDone(false),
      m_uiMapCnt(0),
      m_uiMaxBattleCnt(0),
      m_ppkNormalBattle(nullptr),
      m_ppkTodayBattle(nullptr),
      m_ppkTomorrowBattle(nullptr)
  {
  }

  bool CNormalGuildBattleManager::Init()
  {
    if (g_Main.IsReleaseServiceMode())
    {
      LIMIT_SRC_GRADE = 2;
      LIMIT_DEST_GRADE = 2;
      CUnmannedTraderEnvironmentValue::Unmanned_Trader_Check_Schedule_Delay = 300000;
    }

    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    m_uiMapCnt = fieldList->GetMapCnt();
    if (!m_uiMapCnt)
    {
      return false;
    }

    m_uiMaxBattleCnt = 46 * m_uiMapCnt;
    CGuildBattleLogger *logger = CGuildBattleLogger::Instance();
    m_ppkNormalBattle = new (std::nothrow) CNormalGuildBattle *[m_uiMaxBattleCnt];
    if (!m_ppkNormalBattle)
    {
      logger->Log(
        "CNormalGuildBattleManager::Init() : new CNormalGuildBattle * [%u] Fail!",
        m_uiMaxBattleCnt);
      return false;
    }

    memset_0(m_ppkNormalBattle, 0, sizeof(CNormalGuildBattle *) * m_uiMaxBattleCnt);
    for (unsigned int dwID = 0; dwID < m_uiMaxBattleCnt; ++dwID)
    {
      m_ppkNormalBattle[dwID] = new (std::nothrow) CNormalGuildBattle(dwID);
      if (!m_ppkNormalBattle[dwID])
      {
        logger->Log("GuildBattleManager::Init() : new CNormalGuildBattle( %u ) Fail!", dwID);
        return false;
      }
      if (!m_ppkNormalBattle[dwID]->CreateLogger())
      {
        logger->Log(
          "CNormalGuildBattleManager::Init() : m_ppkNormalBattle[i]->CreateLogger() Fail!",
          dwID);
        return false;
      }
    }

    m_ppkTodayBattle = m_ppkNormalBattle;
    m_ppkTomorrowBattle = &m_ppkNormalBattle[23 * m_uiMapCnt];
    return true;
  }

  bool CNormalGuildBattleManager::Load(
    int iCurDay,
    unsigned int uiOldMapCnt,
    int iToday,
    unsigned int iTodayDayID,
    int iTomorrow,
    unsigned int iTomorrowDayID)
  {
    if (!m_ppkNormalBattle)
    {
      return false;
    }

    m_bLoad = true;
    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    const unsigned int mapCnt = fieldList->GetMapCnt();
    const int rowCount = static_cast<int>(g_Main.m_pWorldDB->SelectRowCountGuildBattleInfo());
    if (mapCnt != uiOldMapCnt || m_uiMaxBattleCnt != static_cast<unsigned int>(rowCount))
    {
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : AddDefaultDBRecord()",
        iCurDay,
        uiOldMapCnt,
        iToday,
        iTodayDayID,
        iTomorrow,
        iTomorrowDayID);
      return AddDefaultDBRecord() != 0;
    }

    if (iToday == iCurDay)
    {
      m_ppkTodayBattle = &m_ppkNormalBattle[iTodayDayID * 23 * m_uiMapCnt];
      m_ppkTomorrowBattle = &m_ppkNormalBattle[iTomorrowDayID * 23 * m_uiMapCnt];
      if (!Load(true, iTodayDayID, m_ppkTodayBattle))
      {
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : Load( true, %d, m_ppkTodayBattle ) Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID,
          iTodayDayID);
        return false;
      }
      if (!Load(false, iTomorrowDayID, m_ppkTomorrowBattle))
      {
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : Load( false, %d, m_ppkTomorrowBattle ) Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID,
          iTomorrowDayID);
        return false;
      }
    }
    else if (iTomorrow == iCurDay)
    {
      m_ppkTodayBattle = &m_ppkNormalBattle[iTomorrowDayID * 23 * m_uiMapCnt];
      m_ppkTomorrowBattle = &m_ppkNormalBattle[iTodayDayID * 23 * m_uiMapCnt];
      Clear(m_ppkTomorrowBattle);
      if (!Load(true, iTomorrowDayID, m_ppkTodayBattle))
      {
        CGuildBattleLogger::Instance()->Log(
          "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iTomorrow == iCurDay ) : Load( true, %d, m_ppkTodayBattle ) Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID,
          iTomorrowDayID);
        return false;
      }
    }
    else
    {
      if (!AddDefaultDBRecord())
      {
        return false;
      }
      CGuildBattleLogger::Instance()->Log(
        "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) AddDefaultDBRecord()",
        iCurDay,
        uiOldMapCnt,
        iToday,
        iTodayDayID,
        iTomorrow);
    }

    CGuildBattleScheduleManager *scheduleManager = CGuildBattleScheduleManager::Instance();
    if (scheduleManager->CleanUpDanglingReservedSchedule())
    {
      return true;
    }
    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) CGuildBattleScheduleManager::Instance()->CleanUpDanglingReservedSchedule() Fail!",
      iCurDay,
      uiOldMapCnt,
      iToday,
      iTodayDayID,
      iTomorrow);
    return false;
  }

  char CNormalGuildBattleManager::Load(bool bToday, unsigned int uiDayID, CNormalGuildBattle **ppkStart)
  {
    _worlddb_guild_battle_info kInfo{};
    const unsigned int dwID = ppkStart[0]->GetID();
    if (!LoadDBGuildBattleInfo(dwID, &kInfo))
    {
      return 0;
    }

    for (unsigned int j = 0; j < kInfo.wCount; ++j)
    {
      const unsigned int battleIndex = kInfo.list[j].dwID % (23 * m_uiMapCnt);
      if (!ppkStart[battleIndex]->Init(
            bToday,
            uiDayID,
            kInfo.list[j].dwID,
            kInfo.list[j].dwP1GuildSerial,
            kInfo.list[j].dwP2GuildSerial,
            kInfo.list[j].dwMapID,
            kInfo.list[j].byNumber))
      {
        ppkStart[battleIndex]->Clear();
      }
    }

    return 1;
  }

  char CNormalGuildBattleManager::LoadDBGuildBattleInfo(unsigned int dwStartID, _worlddb_guild_battle_info *kInfo)
  {
    memset_0(kInfo, 0, sizeof(_worlddb_guild_battle_info));
    if (g_Main.m_pWorldDB->LoadGuildBattleInfo(dwStartID, 23 * m_uiMapCnt, kInfo))
    {
      return 1;
    }
    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattleManager::LoadDBGuildBattleInfo() : g_Main.m_pWorldDB->LoadGuildBattleInfo() Fail!");
    return 0;
  }

  char CNormalGuildBattleManager::AddDefaultDBRecord()
  {
    if (g_Main.m_pWorldDB->InsertGuildBattleDefaultRecord(46 * m_uiMapCnt))
    {
      return 1;
    }
    CGuildBattleLogger::Instance()->Log(
      "CNormalGuildBattleManager::AddDefaultDBRecord() g_Main.m_pWorldDB->InsertGuildBattleDefaultRecord( %u ) Fail!",
      46);
    return 0;
  }

  void CNormalGuildBattleManager::Loop()
  {
    if (!m_bLoad || !m_ppkTodayBattle)
    {
      return;
    }

    const unsigned int battleCount = 23 * m_uiMapCnt;
    for (unsigned int battleIndex = 0; battleIndex < battleCount; ++battleIndex)
    {
      if (m_ppkTodayBattle[battleIndex]->IsProc())
      {
        m_ppkTodayBattle[battleIndex]->Process();
      }
    }
  }

  void CNormalGuildBattleManager::Clear()
  {
    Clear(m_ppkTodayBattle);
    Clear(m_ppkTomorrowBattle);
  }

  void CNormalGuildBattleManager::Flip()
  {
    CNormalGuildBattle **tomorrowBattle = m_ppkTomorrowBattle;
    m_ppkTomorrowBattle = m_ppkTodayBattle;
    m_ppkTodayBattle = tomorrowBattle;
  }

  void CNormalGuildBattleManager::Clear(CNormalGuildBattle **ppkStart)
  {
    const unsigned int total = 23 * m_uiMapCnt;
    for (unsigned int j = 0; j < total; ++j)
    {
      ppkStart[j]->Clear();
    }
  }

  void CNormalGuildBattleManager::DoDayChangedWork()
  {
    Clear(m_ppkTomorrowBattle);
    SetReadyState(m_ppkTodayBattle);
  }

  void CNormalGuildBattleManager::SetReadyState(CNormalGuildBattle **ppkStart)
  {
    const unsigned int battleCount = 23 * m_uiMapCnt;
    for (unsigned int battleIndex = 0; battleIndex < battleCount; ++battleIndex)
    {
      if (!ppkStart[battleIndex]->IsEmpty())
      {
        ppkStart[battleIndex]->SetReadyState();
      }
    }
  }

  void CNormalGuildBattleManager::SetNextEvent()
  {
    m_bDone = false;
  }

  void CNormalGuildBattleManager::Join(unsigned int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial)
  {
    int ret = -1;
    if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      ret = 141;
    }
    else
    {
      CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
      if (!battle)
      {
        ret = 142;
      }
      else
      {
        JoinGuild(static_cast<int>(n), dwGuildSerial, dwCharacSerial);
      }
    }

    if (ret >= 0)
    {
      unsigned __int8 type[2]{};
      type[0] = 27;
      type[1] = 57;
      char msg[1]{static_cast<char>(ret)};
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, 1u);
    }
  }

  void CNormalGuildBattleManager::JoinGuild(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial)
  {
    if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      return;
    }

    CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
    if (battle && (battle->IsReadyOrCountState() || battle->IsInBattle()))
    {
      battle->AskJoin(n, dwGuildSerial, dwCharacSerial);
    }
  }

  unsigned __int8 CNormalGuildBattleManager::LeaveGuild(CPlayer *pkPlayer)
  {
    CNormalGuildBattle *battle =
      GetBattleByGuildSerial(pkPlayer->m_Param.m_pGuild->m_dwSerial);
    if (battle)
    {
      return battle->LeaveGuild(pkPlayer);
    }
    return static_cast<unsigned __int8>(-114);
  }

  unsigned __int8 CNormalGuildBattleManager::Add(
    CGuild *pSrcGuild,
    CGuild *pDestGuild,
    unsigned int dwStartTime,
    unsigned int dwElapseTimeCnt,
    unsigned __int8 byNumber,
    unsigned int dwMapCode)
  {
    if (!pSrcGuild || !pDestGuild)
    {
      return 110;
    }
    if (byNumber > 0x32u)
    {
      return 126;
    }
    unsigned int dwMapInx = 0;
    if (!CNormalGuildBattleFieldList::Instance()->GetMapInx(pSrcGuild->m_byRace, dwMapCode, &dwMapInx))
    {
      return 120;
    }

    unsigned int uiSLID = 0;
    CGuildBattleSchedule *pkSchedule = nullptr;
    CGuildBattleScheduleManager *scheduleManager = CGuildBattleScheduleManager::Instance();
    const unsigned __int8 scheduleResult = scheduleManager->Add(
      dwMapInx,
      dwStartTime,
      dwElapseTimeCnt,
      &pkSchedule,
      &uiSLID);
    if (scheduleResult || !pkSchedule)
    {
      return scheduleResult;
    }

    CNormalGuildBattleStateListPool *statePool = CNormalGuildBattleStateListPool::Instance();
    const unsigned int sid = pkSchedule->GetSID();
    CNormalGuildBattleStateList *pkStateList = statePool->Get(sid);
    if (!pkStateList)
    {
      return 110;
    }

    ATL::CTimeSpan battleTime{};
    pkSchedule->GetBattleTime(&battleTime);
    pkStateList->SetBattleTime(battleTime);
    pkSchedule->SetStateList(pkStateList);

    CNormalGuildBattleField *pkField = CNormalGuildBattleFieldList::Instance()->GetField(dwMapInx);
    if (!pkField)
    {
      return 110;
    }
    CNormalGuildBattle *pkBattle = GetBattle(pkSchedule->GetSID());
    if (!pkBattle)
    {
      return 110;
    }
    pkBattle->Init(pSrcGuild, pDestGuild, pkField, byNumber, pkStateList);
    return PushDQSData(dwMapInx, uiSLID, pkBattle, pkSchedule) ? 0 : 110;
  }

  void CNormalGuildBattleManager::AddComplete(unsigned __int8 byRet, unsigned int dwBattleID)
  {
    if (!m_ppkNormalBattle || m_uiMaxBattleCnt <= dwBattleID || !m_ppkNormalBattle[dwBattleID])
    {
      return;
    }

    if (byRet)
    {
      m_ppkNormalBattle[dwBattleID]->Clear();
    }
    m_ppkNormalBattle[dwBattleID]->AddComplete(byRet);
  }

  char CNormalGuildBattleManager::UpdateClearGuildBattleDayInfo(unsigned int dwStartSID, unsigned int dwEndSID)
  {
    if (g_Main.m_pWorldDB->UpdateClearGuildBattleInfo(dwStartSID, dwEndSID))
    {
      return 1;
    }

    g_Main.m_logDQS.Write(
      "CGuildBattleReservedScheduleMapGroup::UpdateClearGuildBattleDayInfo() : g_Main.m_pWorldDB->UpdateClearGuildBattleInfo( %u, %u ) Fail!",
      dwStartSID,
      dwEndSID);
    return 0;
  }

  void CNormalGuildBattleManager::LogIn(int n, unsigned int dwGuildSerial, unsigned int dwCharacSerial)
  {
    if (dwGuildSerial != static_cast<unsigned int>(-1))
    {
      CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
      if (battle)
      {
        if (battle->IsReadyOrCountState())
        {
          battle->AskJoin(n, dwGuildSerial, dwCharacSerial);
        }
        else if (battle->IsInBattle())
        {
          battle->LogIn(n, dwGuildSerial, dwCharacSerial);
        }
      }
    }
  }

  unsigned __int8 CNormalGuildBattleManager::NetClose(
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial,
    CPlayer *pkPlayer)
  {
    if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      return static_cast<unsigned __int8>(-115);
    }
    CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
    if (battle)
    {
      return battle->NetClose(dwCharacSerial, pkPlayer);
    }
    return static_cast<unsigned __int8>(-114);
  }

  void CNormalGuildBattleManager::CheckTakeGravityStone(
    int iPortalInx,
    unsigned int n,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial)
  {
    const unsigned __int8 ret = ProcCheckTakeGravityStone(iPortalInx, dwGuildSerial, dwCharacSerial);
    if (ret)
    {
      char msg[40]{};
      msg[0] = static_cast<char>(ret);
      unsigned __int8 type[2] = {27, 72};
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, static_cast<unsigned __int16>(sizeof(msg)));
    }
  }

  void CNormalGuildBattleManager::CheckGetGravityStone(
    unsigned __int16 wIndex,
    unsigned int dwObjSerial,
    unsigned int n,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial)
  {
    const unsigned __int8 ret = ProcCheckGetGravityStone(wIndex, dwObjSerial, dwGuildSerial, dwCharacSerial);
    if (ret)
    {
      char msg[40]{};
      msg[0] = static_cast<char>(ret);
      unsigned __int8 type[2] = {27, 72};
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, static_cast<unsigned __int16>(sizeof(msg)));
    }
  }

  void CNormalGuildBattleManager::CheckGoal(
    unsigned int n,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial,
    int iPortalInx)
  {
    const unsigned __int8 ret = ProcCheckGoal(dwGuildSerial, dwCharacSerial, iPortalInx);
    if (ret)
    {
      char msg[58]{};
      msg[0] = static_cast<char>(ret);
      unsigned __int8 type[2] = {27, 74};
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, static_cast<unsigned __int16>(sizeof(msg)));
    }
  }

  unsigned __int8 CNormalGuildBattleManager::ProcCheckTakeGravityStone(
    int iPortalInx,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial)
  {
if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      return static_cast<unsigned __int8>(-115);
    }
    CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
    if (!battle)
    {
      return static_cast<unsigned __int8>(-114);
    }
    if (!battle->IsInBattle())
    {
      return static_cast<unsigned __int8>(-112);
    }
    return static_cast<unsigned __int8>(-112);
  }

  unsigned __int8 CNormalGuildBattleManager::ProcCheckGetGravityStone(
    unsigned __int16 wIndex,
    unsigned int dwObjSerial,
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial)
  {
if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      return static_cast<unsigned __int8>(-115);
    }
    CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
    if (!battle)
    {
      return static_cast<unsigned __int8>(-114);
    }
    if (!battle->IsInBattle())
    {
      return static_cast<unsigned __int8>(-112);
    }
    return static_cast<unsigned __int8>(-112);
  }

  unsigned __int8 CNormalGuildBattleManager::ProcCheckGoal(
    unsigned int dwGuildSerial,
    unsigned int dwCharacSerial,
    int iPortalInx)
  {
if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      return static_cast<unsigned __int8>(-115);
    }
    CNormalGuildBattle *battle = GetBattleByGuildSerial(dwGuildSerial);
    if (!battle)
    {
      return static_cast<unsigned __int8>(-114);
    }
    if (!battle->IsInBattle())
    {
      return static_cast<unsigned __int8>(-112);
    }
    return static_cast<unsigned __int8>(-112);
  }

  CNormalGuildBattle *CNormalGuildBattleManager::GetBattleByGuildSerial(unsigned int dwGuildSerial)
  {
    if (!m_ppkNormalBattle)
    {
      return nullptr;
    }
    for (unsigned int j = 0; j < m_uiMapCnt; ++j)
    {
      CNormalGuildBattle **list = &m_ppkTodayBattle[23 * j];
      for (unsigned __int8 k = 0; k < 0x17u; ++k)
      {
        if (!list[k]->IsEmpty() && list[k]->IsMemberGuild(dwGuildSerial))
        {
          return list[k];
        }
      }
    }
    return nullptr;
  }

  CNormalGuildBattle *CNormalGuildBattleManager::GetBattle(unsigned int dwID)
  {
    if (!m_ppkNormalBattle || m_uiMaxBattleCnt <= dwID)
    {
      return nullptr;
    }
    if (m_ppkNormalBattle[dwID])
    {
      return m_ppkNormalBattle[dwID];
    }
    return nullptr;
  }

  bool CNormalGuildBattleManager::PushDQSData(
    unsigned int uiFieldInx,
    unsigned int uiSLID,
    CNormalGuildBattle *pkBattle,
    CGuildBattleSchedule *pkSchedule)
  {
    _qry_case_addguildbattleschedule qry{};
    qry.dwID = pkBattle->GetID();
    qry.dw1PGuildSerial = pkBattle->Get1P()->GetGuildSerial();
    qry.dw2PGuildSerial = pkBattle->Get2P()->GetGuildSerial();
    qry.dwMapID = uiFieldInx;
    qry.byBattleNumber = pkBattle->GetGuildBattleNumber();
    qry.dwSLID = uiSLID;
    qry.byState = static_cast<unsigned __int8>(pkSchedule->GetState());
    qry.tStartTime = pkSchedule->GetRealStartTime();
    qry.wTurmMin = static_cast<unsigned __int16>(pkSchedule->GetBattleTurm());

    const int nSize = qry.size();
    return g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x1Eu, reinterpret_cast<char *>(&qry), nSize) != nullptr;
  }

  CGuildBattleSchedule::CGuildBattleSchedule(unsigned int dwScheduleID) : m_dwScheduleID(dwScheduleID)
  {
    m_kNextStartTime = ATL::CTime();
    m_kBattleStartTime = ATL::CTime();
    m_kBattleTime = ATL::CTimeSpan();
    Clear();
  }

  unsigned int CGuildBattleSchedule::GetSID()
  {
    return m_dwScheduleID;
  }

  ATL::CTimeSpan *CGuildBattleSchedule::GetBattleTime(ATL::CTimeSpan *result)
  {
    *result = m_kBattleTime;
    return result;
  }

  void CGuildBattleSchedule::SetStateList(CGuildBattleStateList *pkStateList)
  {
    if (pkStateList)
    {
      m_pkStateList = pkStateList;
    }
  }

  unsigned int CGuildBattleSchedule::GetState()
  {
    return static_cast<unsigned int>(m_eState);
  }

  __int64 CGuildBattleSchedule::GetRealStartTime()
  {
    return m_kBattleStartTime.GetTime();
  }

  __int64 CGuildBattleSchedule::GetBattleTurm()
  {
    return m_kBattleTime.GetTotalMinutes();
  }

  bool CGuildBattleSchedule::IsProc()
  {
    return m_eState == GS_PROC;
  }

  int CGuildBattleSchedule::Check()
  {
    if (m_eState == GS_NONE || m_eState == GS_DONE)
    {
      return -1;
    }

    ATL::CTime now = ATL::CTime::GetTickCount();
    if (now < m_kNextStartTime)
    {
      return 1;
    }

    if (Process())
    {
      return 2;
    }

    m_eState = GS_DONE;
    return 0;
  }

  int CGuildBattleSchedule::Process()
  {
    const int nextResult = m_pkStateList->Next(false);
    if (!nextResult)
    {
      return 0;
    }

    ATL::CTimeSpan term;
    m_pkStateList->GetTerm(&term);
    m_kNextStartTime += term;
    return nextResult;
  }

  bool CGuildBattleSchedule::IsWait()
  {
    return m_eState == GS_WAIT;
  }

  void CGuildBattleSchedule::SetProcState()
  {
    m_eState = GS_PROC;
  }

  char CGuildBattleSchedule::GetLeftTime(
    unsigned __int8 *byHour,
    unsigned __int8 *byMin,
    unsigned __int8 *bySec)
  {
    ATL::CTime now = ATL::CTime::GetCurrentTime();
    ATL::CTimeSpan elapsed = now - m_kBattleStartTime;
    if (m_kBattleTime <= elapsed)
    {
      *byHour = 0;
      *byMin = 0;
      *bySec = 0;
    }
    else
    {
      ATL::CTimeSpan left = m_kBattleTime - elapsed;
      *byHour = static_cast<unsigned __int8>(left.GetHours());
      *byMin = static_cast<unsigned __int8>(left.GetMinutes());
      *bySec = static_cast<unsigned __int8>(left.GetSeconds());
    }
    return 1;
  }

  char CGuildBattleSchedule::Load(
    bool bToday,
    unsigned int dwScheduleID,
    unsigned __int8 ucState,
    __int64 tTime,
    unsigned __int16 wTumeMin)
  {
    if (ucState >= 4u || 1320 < wTumeMin)
    {
      return 0;
    }
    if (dwScheduleID != m_dwScheduleID)
    {
      return 0;
    }
    if (wTumeMin > 0x1Eu)
    {
      wTumeMin = 30;
    }

    m_eState = static_cast<GS_STATE>(ucState);
    m_kBattleStartTime = ATL::CTime(tTime);

    const ATL::CTimeSpan oneMinute(0, 0, 1, 0);
    const ATL::CTimeSpan fourMinutes(0, 0, 4, 0);
    const ATL::CTimeSpan fiveMinutes(0, 0, 5, 0);
    const ATL::CTimeSpan nineMinutes = fiveMinutes + fourMinutes;
    const ATL::CTimeSpan advance = nineMinutes + oneMinute;
    m_kNextStartTime = m_kBattleStartTime - advance;

    const ATL::CTime now = ATL::CTime::GetTickCount();
    if (m_kNextStartTime < now)
    {
      return 0;
    }

    const int curDay = bToday ? GetCurDay() : GetNextDay();
    if (now.GetYear() != m_kNextStartTime.GetYear())
    {
      return 0;
    }
    if (now.GetMonth() != m_kNextStartTime.GetMonth())
    {
      return 0;
    }
    if (curDay != m_kNextStartTime.GetDay())
    {
      return 0;
    }

    m_kBattleTime = ATL::CTimeSpan(0, 0, wTumeMin, 0);
    return 1;
  }

  ATL::CTime *CGuildBattleSchedule::GetTime(ATL::CTime *result)
  {
    *result = m_kNextStartTime;
    return result;
  }

  unsigned __int8 CGuildBattleSchedule::Set(unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt)
  {
    if (!dwStartTimeInx || dwStartTimeInx >= 0x17)
    {
      return 122;
    }

    const ATL::CTime now = ATL::CTime::GetCurrentTime();
    const ATL::CTime tomorrow = now + ATL::CTimeSpan(1, 0, 0, 0);
    const ATL::CTime battleStart(
      tomorrow.GetYear(),
      tomorrow.GetMonth(),
      tomorrow.GetDay(),
      dwStartTimeInx,
      0,
      0,
      -1);

    m_kBattleStartTime = battleStart;
    const ATL::CTimeSpan advance(0, 0, 10, 0);
    m_kNextStartTime = battleStart - advance;
    m_kBattleTime = ATL::CTimeSpan(0, 0, 30 * dwElapseTimeCnt, 0);
    m_eState = GS_WAIT;
    return 0;
  }

  bool CGuildBattleSchedule::IsEmpty()
  {
    return m_eState == GS_NONE;
  }

  bool CGuildBattleSchedule::IsDone()
  {
    return m_eState == GS_DONE;
  }

  void CGuildBattleSchedule::Clear()
  {
    m_eState = GS_NONE;
    m_kNextStartTime = ATL::CTime(0);
    m_kBattleTime = ATL::CTimeSpan(0);
    m_pkStateList = &STATE_LIST_NULL_10;
  }

  CGuildBattleSchedulePool *CGuildBattleSchedulePool::ms_Instance = nullptr;

  CGuildBattleSchedulePool *CGuildBattleSchedulePool::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CGuildBattleSchedulePool();
    }
    return ms_Instance;
  }

  CGuildBattleSchedulePool::CGuildBattleSchedulePool() : m_uiMapCnt(0), m_dwMaxScheduleCnt(0), m_ppkSchedule(nullptr)
  {
  }

  bool CGuildBattleSchedulePool::Init(unsigned int uiMapCnt)
  {
    if (!uiMapCnt)
    {
      return false;
    }

    m_dwMaxScheduleCnt = 46 * uiMapCnt;
    m_ppkSchedule = new (std::nothrow) CGuildBattleSchedule *[m_dwMaxScheduleCnt];
    if (!m_ppkSchedule)
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleSchedulePool() : new CGuildBattleSchedule * [%u] Fail!",
        23);
      return false;
    }

    memset_0(m_ppkSchedule, 0, sizeof(CGuildBattleSchedule *) * m_dwMaxScheduleCnt);
    for (unsigned int dwScheduleID = 0; dwScheduleID < m_dwMaxScheduleCnt; ++dwScheduleID)
    {
      m_ppkSchedule[dwScheduleID] = new (std::nothrow) CGuildBattleSchedule(dwScheduleID);
      if (!m_ppkSchedule[dwScheduleID])
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleSchedulePool() : new CGuildBattleSchedule( %u ) Fail!",
          dwScheduleID);
        return false;
      }
    }

    m_uiMapCnt = uiMapCnt;
    return true;
  }

  CGuildBattleSchedule *CGuildBattleSchedulePool::Get(unsigned int dwSID)
  {
    if (!m_ppkSchedule || m_dwMaxScheduleCnt <= dwSID)
    {
      return nullptr;
    }
    if (!m_ppkSchedule[dwSID])
    {
      return nullptr;
    }
    if (!m_ppkSchedule[dwSID]->IsEmpty() && !m_ppkSchedule[dwSID]->IsDone())
    {
      return nullptr;
    }
    m_ppkSchedule[dwSID]->Clear();
    return m_ppkSchedule[dwSID];
  }

  CGuildBattleSchedule *CGuildBattleSchedulePool::Get(unsigned int uiSLID, unsigned int dwStartInx)
  {
    const unsigned int dwSID = dwStartInx + 23 * uiSLID;
    return Get(dwSID);
  }

  unsigned int CGuildBattleSchedulePool::GetSID(unsigned int uiSLID, unsigned int dwStartInx)
  {
    CGuildBattleSchedule *schedule = Get(uiSLID, dwStartInx);
    return schedule->GetSID();
  }

  CGuildBattleSchedule *CGuildBattleSchedulePool::GetRef(unsigned int dwSID)
  {
    if (!m_ppkSchedule || m_dwMaxScheduleCnt <= dwSID)
    {
      return nullptr;
    }
    if (m_ppkSchedule[dwSID])
    {
      return m_ppkSchedule[dwSID];
    }
    return nullptr;
  }

  void CGuildBattleSchedulePool::ClearByDayID(unsigned int uiDayID)
  {
    if (!m_dwMaxScheduleCnt || !m_ppkSchedule || !m_uiMapCnt || uiDayID > 1)
    {
      return;
    }

    unsigned int startIndex = 0;
    unsigned int endIndex = 23 * m_uiMapCnt;
    if (uiDayID)
    {
      startIndex = endIndex;
      endIndex = 2 * startIndex;
    }

    for (unsigned int scheduleIndex = startIndex; scheduleIndex < endIndex; ++scheduleIndex)
    {
      if (m_ppkSchedule[scheduleIndex])
      {
        m_ppkSchedule[scheduleIndex]->Clear();
      }
    }
  }

  CGuildBattleReservedSchedule::CGuildBattleReservedSchedule(unsigned int uiScheduleListID)
    : m_uiScheduleListID(uiScheduleListID)
  {
    Clear();
  }

  void CGuildBattleReservedSchedule::Clear()
  {
    m_bDone = false;
    m_uiCurScheduleInx = 0;
    memset_0(m_bUseField, 0, sizeof(m_bUseField));
    memset_0(m_pkSchedule, 0, sizeof(m_pkSchedule));
  }

  char CGuildBattleReservedSchedule::Clear(unsigned int dwID)
  {
    const unsigned int scheduleIndex = dwID % 0x17u;
    if (!m_bUseField[scheduleIndex] || !m_pkSchedule[scheduleIndex])
    {
      return 0;
    }

    m_bUseField[scheduleIndex] = false;
    m_pkSchedule[scheduleIndex]->Clear();
    return 1;
  }

  unsigned int CGuildBattleReservedSchedule::GetID()
  {
    return m_uiScheduleListID;
  }

  bool CGuildBattleReservedSchedule::IsDone()
  {
    return m_bDone;
  }

  bool CGuildBattleReservedSchedule::Loop()
  {
    if (m_bDone)
    {
      return false;
    }

    if (m_pkSchedule[m_uiCurScheduleInx] || Next())
    {
      const int checkResult = m_pkSchedule[m_uiCurScheduleInx]->Check();
      return CheckNextEvent(checkResult) != 0;
    }

    m_bDone = true;
    return false;
  }

  char CGuildBattleReservedSchedule::Next()
  {
    ++m_uiCurScheduleInx;

    bool foundWaitSchedule = false;
    for (unsigned int scheduleIndex = m_uiCurScheduleInx; scheduleIndex < 0x17; ++scheduleIndex)
    {
      if (m_pkSchedule[scheduleIndex] && m_pkSchedule[scheduleIndex]->IsWait())
      {
        m_uiCurScheduleInx = scheduleIndex;
        m_pkSchedule[m_uiCurScheduleInx]->SetProcState();
        foundWaitSchedule = true;
        break;
      }
    }

    if (foundWaitSchedule)
    {
      return 1;
    }

    m_uiCurScheduleInx = 0;
    return 0;
  }

  char CGuildBattleReservedSchedule::CheckNextEvent(int iRet)
  {
    if (!iRet)
    {
      m_pkSchedule[m_uiCurScheduleInx] = nullptr;
      return 0;
    }

    if (iRet == 1)
    {
      return 1;
    }

    if (iRet == -1)
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleReservedSchedule::CheckNextEvent( iRet(%d) ) : m_uiScheduleID(%u) : -1 == m_pkSchedule[%u]->Check() Invalid!",
        -1,
        m_uiCurScheduleInx,
        m_uiCurScheduleInx);
    }

    return 0;
  }

  unsigned int CGuildBattleReservedSchedule::GetCurScheduleID()
  {
    if (m_bDone)
    {
      return static_cast<unsigned int>(-1);
    }
    if (m_pkSchedule[m_uiCurScheduleInx] && m_pkSchedule[m_uiCurScheduleInx]->IsProc())
    {
      return m_pkSchedule[m_uiCurScheduleInx]->GetSID();
    }
    CGuildBattleLogger::Instance()->Log(
      "CGuildBattleReservedSchedule::GetCurScheduleID() : m_pkSchedule[%u]->IsProc() Invalid State!",
      m_uiCurScheduleInx);
    return static_cast<unsigned int>(-1);
  }

  unsigned __int8 CGuildBattleReservedSchedule::Add(
    unsigned int dwStartTimeInx,
    unsigned int dwElapseTimeCnt,
    CGuildBattleSchedule **ppkSchedule)
  {
    const unsigned int dwStartInx = dwStartTimeInx;
    CGuildBattleSchedule *schedule = CGuildBattleSchedulePool::Instance()->Get(m_uiScheduleListID, dwStartInx);
    if (!schedule)
    {
      return 110;
    }
    const unsigned __int8 ret = schedule->Set(dwStartTimeInx, dwElapseTimeCnt);
    if (ret)
    {
      return ret;
    }
    UpdateUseField(dwStartTimeInx, dwElapseTimeCnt);
    m_pkSchedule[dwStartInx] = schedule;
    *ppkSchedule = schedule;
    return 0;
  }

  unsigned __int8 CGuildBattleReservedSchedule::IsEmptyTime(
    unsigned int dwStartTimeInx,
    unsigned int dwElapseTimeCnt)
  {
    if (!dwStartTimeInx && dwElapseTimeCnt)
    {
      return 125;
    }
    if (dwStartTimeInx >= 0x17)
    {
      return 122;
    }
    if (m_bUseField[dwStartTimeInx])
    {
      return 112;
    }
    return 0;
  }

  void CGuildBattleReservedSchedule::UpdateUseField(unsigned int dwStartTimeInx, unsigned int dwElapseTimeCnt)
  {
    const unsigned int end = dwStartTimeInx + dwElapseTimeCnt;
    for (unsigned int j = dwStartTimeInx; j < end; ++j)
    {
      m_bUseField[j] = true;
    }
  }

  char CGuildBattleReservedSchedule::Load(bool bToday, _worlddb_guild_battle_schedule_list *pkInfo)
  {
    if (!pkInfo)
    {
      return 0;
    }
    if (!pkInfo->wCount)
    {
      return 1;
    }

    auto *list = pkInfo->list;
    for (unsigned int j = 0; j < pkInfo->wCount; ++j)
    {
      if (m_uiScheduleListID != list[j].dwSLID)
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedSchedule::Load()  m_uiScheduleID(%u) != pkInfo->list[%u].uiSLID(%u) Fail!",
          m_uiScheduleListID,
          j,
          list->dwSLID);
        return 0;
      }
      if (list[j].ucState == 1)
      {
        CGuildBattleSchedulePool *pool = CGuildBattleSchedulePool::Instance();
        CGuildBattleSchedule *schedule = pool->Get(list[j].dwID);
        if (!schedule)
        {
          CGuildBattleLogger::Instance()->Log(
            "CGuildBattleReservedSchedule::Load() CGuildBattleSchedulePool::Instance()->Get( %u ) Fail!",
            list[j].dwID);
          return 0;
        }

        if (schedule->Load(bToday, list[j].dwID, list[j].ucState, list[j].tStartTime, list[j].wTumeMin))
        {
          const unsigned int idx = list[j].dwID % 0x17u;
          m_pkSchedule[idx] = schedule;

          ATL::CTime result;
          schedule->GetTime(&result);
          const char *dayLabel = bToday ? "today" : "tommorrow";
          CGuildBattleLogger::Instance()->Log(
            "CGuildBattleReservedSchedule::Load() : %s : pkSchedule->Load( %u, %u, %04d-%02d-%02d %02d:%02d:%02d, %u ) Load!",
            dayLabel,
            list[j].dwID,
            list[j].ucState,
            result.GetYear(),
            result.GetMonth(),
            result.GetDay(),
            result.GetHour(),
            result.GetMinute(),
            result.GetSecond(),
            list[j].wTumeMin);
        }
        else
        {
          schedule->Clear();
          const ATL::CTime startTime(list[j].tStartTime);
          const char *dayLabel = bToday ? "today" : "tommorrow";
          CGuildBattleLogger::Instance()->Log(
            "CGuildBattleReservedSchedule::Load() : %s : pkSchedule->Load( %u, %u, %04d-%02d-%02d %02d:%02d:%02d, %u ) Skip!",
            dayLabel,
            list[j].dwID,
            list[j].ucState,
            startTime.GetYear(),
            startTime.GetMonth(),
            startTime.GetDay(),
            startTime.GetHour(),
            startTime.GetMinute(),
            startTime.GetSecond(),
            list[j].wTumeMin);
        }
      }
    }

    return 1;
  }

  CGuildBattleSchedule *CGuildBattleReservedSchedule::UpdateUseFlag(unsigned int dwID)
  {
    const unsigned int idx = dwID % 0x17u;
    if (!m_pkSchedule[idx])
    {
      return nullptr;
    }
    m_bUseField[idx] = true;
    return m_pkSchedule[idx];
  }

  void CGuildBattleReservedSchedule::Flip()
  {
    ClearElapsedSchedule();
  }

  void CGuildBattleReservedSchedule::ClearElapsedSchedule()
  {
    CGuildBattleLogger *logger = CGuildBattleLogger::Instance();
    const ATL::CTime now = ATL::CTime::GetTickCount();
    for (unsigned int j = 0; j < 0x17; ++j)
    {
      if (m_bUseField[j] && m_pkSchedule[j])
      {
        ATL::CTime scheduleTime;
        m_pkSchedule[j]->GetTime(&scheduleTime);
        if (now >= scheduleTime)
        {
          m_pkSchedule[j]->Clear();
          logger->Log(
            "CGuildBattleReservedSchedule::Flip() : SLID(%u) m_pkSchedule[%u]->Clear() Elapased Time!",
            m_uiScheduleListID,
            j);
        }
      }
    }
  }

  char CGuildBattleReservedSchedule::CleanUpDanglingReservedSchedule()
  {
    for (unsigned int j = 0; j < 0x17; ++j)
    {
      if (m_pkSchedule[j] && !m_bUseField[j])
      {
        m_pkSchedule[j]->Clear();
        const unsigned int sid = m_pkSchedule[j]->GetSID();
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedSchedule::CleanUpDanglingReservedSchedule() : m_pkSchedule[%u]->Clear() ID(%u)!",
          j,
          sid);
      }
    }
    return 1;
  }

  _worlddb_guild_battle_schedule_list *CGuildBattleReservedScheduleMapGroup::ms_pkDBScheduleInfo = nullptr;

  CGuildBattleReservedScheduleMapGroup::CGuildBattleReservedScheduleMapGroup()
    : m_bDone(false), m_uiDayInx(0), m_uiMapCnt(0), m_ppkReservedSchedule(nullptr)
  {
  }

  bool CGuildBattleReservedScheduleMapGroup::Init(unsigned int uiDayInx, unsigned int uiMapCnt)
  {
    if (!uiMapCnt)
    {
      return false;
    }

    m_ppkReservedSchedule = new (std::nothrow) CGuildBattleReservedSchedule *[uiMapCnt];
    if (!m_ppkReservedSchedule)
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleReservedScheduleMapGroup::Init( %u, %u ) : new CGuildBattleReservedSchedule * [%u] Fail!",
        uiDayInx,
        uiMapCnt,
        uiMapCnt);
      return false;
    }

    const unsigned int baseID = uiMapCnt * uiDayInx;
    for (unsigned int j = 0; j < uiMapCnt; ++j)
    {
      const unsigned int scheduleID = j + baseID;
      m_ppkReservedSchedule[j] = new (std::nothrow) CGuildBattleReservedSchedule(scheduleID);
      if (!m_ppkReservedSchedule[j])
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleMapGroup::Init( %u, %u ) : new CGuildBattleReservedSchedule(%u) Fail!",
          uiDayInx,
          uiMapCnt,
          scheduleID);
        return false;
      }
    }

    m_uiDayInx = uiDayInx;
    m_uiMapCnt = uiMapCnt;
    return true;
  }

  unsigned __int8 CGuildBattleReservedScheduleMapGroup::Add(
    unsigned int uiFieldInx,
    unsigned int dwStartTimeInx,
    unsigned int dwElapseTimeCnt,
    CGuildBattleSchedule **ppkSchedule,
    unsigned int *uiSLID)
  {
    const unsigned __int8 ret = IsEmptyTime(uiFieldInx, dwStartTimeInx, dwElapseTimeCnt);
    if (ret)
    {
      return ret;
    }
    *uiSLID = m_ppkReservedSchedule[uiFieldInx]->GetID();
    return m_ppkReservedSchedule[uiFieldInx]->Add(dwStartTimeInx, dwElapseTimeCnt, ppkSchedule);
  }

  unsigned __int8 CGuildBattleReservedScheduleMapGroup::IsEmptyTime(
    unsigned int uiFieldInx,
    unsigned int dwStartTimeInx,
    unsigned int dwElapseTimeCnt)
  {
    if (m_uiMapCnt <= uiFieldInx)
    {
      return 120;
    }
    if (m_ppkReservedSchedule)
    {
      return m_ppkReservedSchedule[uiFieldInx]->IsEmptyTime(dwStartTimeInx, dwElapseTimeCnt);
    }
    return 110;
  }

  char CGuildBattleReservedScheduleMapGroup::Load(bool bToday)
  {
    if (!m_uiMapCnt || !m_ppkReservedSchedule)
    {
      return 0;
    }

    if (!ms_pkDBScheduleInfo)
    {
      ms_pkDBScheduleInfo = static_cast<_worlddb_guild_battle_schedule_list *>(
        operator new[](saturated_mul(m_uiMapCnt, sizeof(_worlddb_guild_battle_schedule_list))));
      if (!ms_pkDBScheduleInfo)
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleMapGroup::Load() new _worlddb_guild_battle_schedule_list[%u] Fail!",
          m_uiMapCnt);
        return 0;
      }
    }

    memset_0(ms_pkDBScheduleInfo, 0, sizeof(_worlddb_guild_battle_schedule_list) * m_uiMapCnt);
    for (unsigned int j = 0; j < m_uiMapCnt; ++j)
    {
      _worlddb_guild_battle_schedule_list *pkInfo = &ms_pkDBScheduleInfo[j];
      const unsigned int id = m_ppkReservedSchedule[j]->GetID();
      const unsigned __int8 ret =
        g_Main.m_pWorldDB->LoadGuildBattleScheduleInfo(id, 0x17u, pkInfo);
      if (ret == 1)
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleMapGroup::Load() g_Main.m_pWorldDB->LoadGuildBattleScheduleInfo( %u, %u, ms_pkDBInfo )",
          id,
          23);
        return 0;
      }
      if (ret != 2)
      {
        if (pkInfo->wCount)
        {
          if (!m_ppkReservedSchedule[j]->Load(bToday, pkInfo))
          {
            CGuildBattleLogger::Instance()->Log(
              "CGuildBattleReservedScheduleMapGroup::Load( %d ) m_ppkReservedSchedule[%u]->Load( ms_pkDBScheduleInfo )",
              static_cast<int>(bToday),
              j);
            return 0;
          }
        }
      }
    }

    return 1;
  }

  bool CGuildBattleReservedScheduleMapGroup::IsDone()
  {
    return m_bDone;
  }

  char CGuildBattleReservedScheduleMapGroup::Loop()
  {
    if (m_bDone || !m_ppkReservedSchedule)
    {
      return 0;
    }

    m_bDone = true;
    char hasNewEvent = 0;
    for (unsigned int mapIndex = 0; mapIndex < m_uiMapCnt; ++mapIndex)
    {
      if (!m_ppkReservedSchedule[mapIndex]->IsDone())
      {
        if (m_ppkReservedSchedule[mapIndex]->Loop())
        {
          hasNewEvent = 1;
        }
        m_bDone = false;
      }
    }

    return hasNewEvent;
  }

  bool CGuildBattleReservedScheduleMapGroup::Clear()
  {
    if (!m_ppkReservedSchedule)
    {
      return false;
    }

    m_bDone = false;
    for (unsigned int mapIndex = 0; mapIndex < m_uiMapCnt; ++mapIndex)
    {
      if (m_ppkReservedSchedule[mapIndex])
      {
        m_ppkReservedSchedule[mapIndex]->Clear();
      }
    }

    CGuildBattleSchedulePool *pool = CGuildBattleSchedulePool::Instance();
    pool->ClearByDayID(m_uiDayInx);
    return false;
  }

  char CGuildBattleReservedScheduleMapGroup::Clear(unsigned int uiMapID, unsigned int dwID)
  {
    if (!m_uiMapCnt || m_uiMapCnt <= uiMapID || !m_ppkReservedSchedule)
    {
      return 0;
    }

    if (m_ppkReservedSchedule[uiMapID]->Clear(dwID))
    {
      return 1;
    }

    CGuildBattleLogger::Instance()->Log(
      "CGuildBattleReservedScheduleMapGroup::Clear(%u,%u) m_ppkReservedSchedule[%u]->Clear(%u) Fail!",
      uiMapID,
      dwID,
      dwID,
      0);
    return 0;
  }

  CGuildBattleSchedule *CGuildBattleReservedScheduleMapGroup::UpdateUseFlag(unsigned int uiMapID, unsigned int dwID)
  {
    if (m_ppkReservedSchedule && m_uiMapCnt > uiMapID && m_ppkReservedSchedule[uiMapID])
    {
      return m_ppkReservedSchedule[uiMapID]->UpdateUseFlag(dwID);
    }
    return nullptr;
  }

  unsigned int CGuildBattleReservedScheduleMapGroup::GetDayID()
  {
    return m_uiDayInx;
  }

  char CGuildBattleReservedScheduleMapGroup::GetSLID(unsigned int uiMapID, unsigned int *uiSLID)
  {
    if (!m_uiMapCnt || m_uiMapCnt <= uiMapID || !m_ppkReservedSchedule[uiMapID])
    {
      return 0;
    }
    *uiSLID = m_ppkReservedSchedule[uiMapID]->GetID();
    return 1;
  }

  unsigned int CGuildBattleReservedScheduleMapGroup::GetCurScheduleID(unsigned int uiMapID)
  {
    if (m_uiMapCnt && m_uiMapCnt > uiMapID && m_ppkReservedSchedule[uiMapID])
    {
      return m_ppkReservedSchedule[uiMapID]->GetCurScheduleID();
    }
    return 0;
  }

  char CGuildBattleReservedScheduleMapGroup::CleanUpDanglingReservedSchedule()
  {
    for (unsigned int j = 0; j < m_uiMapCnt; ++j)
    {
      if (m_ppkReservedSchedule[j] && !m_ppkReservedSchedule[j]->CleanUpDanglingReservedSchedule())
      {
        return 0;
      }
    }
    return 1;
  }

  void CGuildBattleReservedScheduleMapGroup::PushDQSClear()
  {
    _qry_case_updateclearguildbattleDayInfo query{};
    query.dwStartSLID = m_ppkReservedSchedule[0]->GetID();
    query.dwEndSLID = m_ppkReservedSchedule[m_uiMapCnt - 1]->GetID();
    query.dwStartSID = CGuildBattleSchedulePool::Instance()->GetSID(query.dwStartSLID, 0);
    query.dwEndSID = CGuildBattleSchedulePool::Instance()->GetSID(query.dwEndSLID, 0x16u);

    const int querySize = static_cast<int>(query.size());
    g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x24u, reinterpret_cast<char *>(&query), querySize);
  }

  void CGuildBattleReservedScheduleMapGroup::Flip()
  {
    if (m_ppkReservedSchedule)
    {
      for (unsigned int j = 0; j < m_uiMapCnt; ++j)
      {
        if (m_ppkReservedSchedule[j])
        {
          m_ppkReservedSchedule[j]->Flip();
        }
      }
    }
  }

  CGuildBattleScheduleManager::CGuildBattleScheduleManager()
    : m_bLoad(false),
      m_pkOldDayTime(nullptr),
      m_pkTimer(nullptr),
      m_uiMapCnt(0),
      m_pkTodaySchedule(m_kSchdule),
      m_pkTomorrowSchedule(&m_kSchdule[1])
  {
  }

  bool CGuildBattleScheduleManager::Init(unsigned int uiMapCnt)
  {
    m_pkOldDayTime = new (std::nothrow) ATL::CTime();
    if (!m_pkOldDayTime)
    {
      CGuildBattleLogger::Instance()->Log("CGuildBattleScheduler::Init(%u) m_pkOldDayTime NULL!", uiMapCnt);
      return false;
    }
    *m_pkOldDayTime = ATL::CTime::GetCurrentTime();

    m_pkTimer = new (std::nothrow) CMyTimer();
    if (!m_pkTimer)
    {
      CGuildBattleLogger::Instance()->Log("CGuildBattleScheduler::Init(%u) new CMyTimer Fail!", uiMapCnt);
      return false;
    }

    if (!m_kSchdule[0].Init(0, uiMapCnt) || !m_kSchdule[1].Init(1u, uiMapCnt))
    {
      CGuildBattleLogger::Instance()->Log("CGuildBattleScheduler::Init(%u) m_kSchdule[].Init Fail!", uiMapCnt);
      return false;
    }

    m_pkTimer->BeginTimer(1000u);
    m_uiMapCnt = uiMapCnt;
    return true;
  }

  void CGuildBattleScheduleManager::Loop()
  {
    if (m_bLoad && (!m_pkTimer || m_pkTimer->CountingTimer()))
    {
      UpdateDayChangedWork();
      if (!m_pkTodaySchedule->IsDone() && m_pkTodaySchedule->Loop())
      {
        SetNextEvnet();
      }
    }
  }

  void CGuildBattleScheduleManager::UpdateDayChangedWork()
  {
    const int dayChangedResult = IsDayChanged();
    if (dayChangedResult == -1)
    {
      Clear();
      return;
    }

    if (dayChangedResult == 1)
    {
      Flip();
    }
  }

  int CGuildBattleScheduleManager::IsDayChanged()
  {
    ATL::CTime now = ATL::CTime::GetTickCount();
    const int previousDay = m_pkOldDayTime->GetDay();
    if (previousDay == now.GetDay())
    {
      return 0;
    }

    ATL::CTime nextDay = *m_pkOldDayTime + ATL::CTimeSpan(1, 0, 0, 0);
    *m_pkOldDayTime = now;
    if (now.GetDay() == nextDay.GetDay())
    {
      return 1;
    }

    return -1;
  }

  void CGuildBattleScheduleManager::Clear()
  {
    const int day = m_pkOldDayTime->GetDay();
    CGuildBattleLogger::Instance()->Log("CGuildBattleScheduleManager::Clear() : %d Day!", day);

    m_pkTodaySchedule->Clear();
    m_pkTomorrowSchedule->Clear();
    CGuildBattleController::Instance()->Clear();
    m_pkTodaySchedule->PushDQSClear();
    m_pkTomorrowSchedule->PushDQSClear();
  }

  void CGuildBattleScheduleManager::Flip()
  {
    const int day = m_pkOldDayTime->GetDay();
    CGuildBattleLogger::Instance()->Log("CGuildBattleScheduleManager::Flip() : %d Day!", day);

    CGuildBattleReservedScheduleMapGroup *todaySchedule = m_pkTodaySchedule;
    m_pkTodaySchedule = m_pkTomorrowSchedule;
    m_pkTomorrowSchedule = todaySchedule;

    m_pkTodaySchedule->Flip();
    m_pkTomorrowSchedule->Clear();
    m_pkTomorrowSchedule->PushDQSClear();
    CGuildBattleController::Instance()->Flip();
  }

  void CGuildBattleScheduleManager::SetNextEvnet()
  {
    CNormalGuildBattleManager::Instance()->SetNextEvent();
  }

  CGuildBattleScheduleManager *CGuildBattleScheduleManager::Instance()
  {
    static CGuildBattleScheduleManager s_instance;
    return &s_instance;
  }

  unsigned __int8 CGuildBattleScheduleManager::Add(
    unsigned int uiFieldInx,
    unsigned int dwStartTimeInx,
    unsigned int dwElapseTimeCnt,
    CGuildBattleSchedule **ppkSchedule,
    unsigned int *uiSLID)
  {
    return m_pkTomorrowSchedule->Add(uiFieldInx, dwStartTimeInx, dwElapseTimeCnt, ppkSchedule, uiSLID);
  }

  bool CGuildBattleScheduleManager::ClearTommorowScheduleByID(unsigned int uiMapID, unsigned int dwID)
  {
    return m_pkTomorrowSchedule->Clear(uiMapID, dwID) != 0;
  }

  char CGuildBattleScheduleManager::Load(
    int iCurDay,
    unsigned int uiOldMapCnt,
    int iToday,
    int iTodayDayID,
    int iTomorrow,
    int iTomorrowDayID)
  {
    m_bLoad = true;
    const int rowCount = static_cast<int>(g_Main.m_pWorldDB->SelectRowCountGuildBattleScheduleInfo());
    if (m_uiMapCnt != uiOldMapCnt || 46 * m_uiMapCnt != static_cast<unsigned int>(rowCount))
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : AddDefaultDBTable()",
        iCurDay,
        uiOldMapCnt,
        iToday,
        iTodayDayID,
        iTomorrow,
        iTomorrowDayID);
      return AddDefaultDBTable();
    }

    if (iToday == iCurDay)
    {
      m_pkTodaySchedule = &m_kSchdule[iTodayDayID];
      m_pkTomorrowSchedule = &m_kSchdule[iTomorrowDayID];
      if (!m_pkTodaySchedule->Load(true))
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : m_pkTodaySchedule->Load(true) Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID);
        return 0;
      }
      if (!m_pkTomorrowSchedule->Load(false))
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iToday == iCurDay ) : m_pkTomorrowSchedule->Load(false) Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID);
        return 0;
      }
    }
    else
    {
      if (iTomorrow != iCurDay)
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : AddDefaultDBTable()",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID);
        return AddDefaultDBTable();
      }

      m_pkTodaySchedule = &m_kSchdule[iTomorrowDayID];
      m_pkTomorrowSchedule = &m_kSchdule[iTodayDayID];
      m_pkTomorrowSchedule->Clear();
      if (!m_pkTodaySchedule->Load(true))
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleScheduleManager::Load( iCurDay(%d), uiOldMapCnt(%u), iToday(%d), iTodayDayID(%d), iTomorrow(%d) iTomorrowDayID(%d) : ( iTomorrow == iCurDay ) : m_pkTodaySchedule->Load(true) Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTodayDayID,
          iTomorrow,
          iTomorrowDayID);
        return 0;
      }
    }

    return 1;
  }

  unsigned int CGuildBattleScheduleManager::GetTodayDayID()
  {
    return m_pkTodaySchedule->GetDayID();
  }

  unsigned int CGuildBattleScheduleManager::GetTomorrowDayID()
  {
    return m_pkTomorrowSchedule->GetDayID();
  }

  unsigned int CGuildBattleScheduleManager::GetCurScheduleID(unsigned int uiMapID)
  {
    return m_pkTodaySchedule->GetCurScheduleID(uiMapID);
  }

  unsigned __int8 CGuildBattleScheduleManager::IsEmptyTime(unsigned int uiFieldInx, unsigned int dwStartTimeInx)
  {
    return m_pkTomorrowSchedule->IsEmptyTime(uiFieldInx, dwStartTimeInx, 1u);
  }

  char CGuildBattleScheduleManager::AddDefaultDBTable()
  {
    if (g_Main.m_pWorldDB->InsertGuildBattleScheduleDefaultRecord(2u, m_uiMapCnt, 0x17u, 1u))
    {
      return 1;
    }
    CGuildBattleLogger::Instance()->Log(
      "CGuildBattleScheduleManager::AddDefaultDBTable() :  g_Main.m_pWorldDB->InsertGuildBattleScheduleDefaultRecord( 2 DAY, %u, %u, %u ) Fail!",
      m_uiMapCnt,
      23,
      30);
    return 0;
  }

  CGuildBattleSchedule *CGuildBattleScheduleManager::UpdateUseFlag(
    unsigned int uiDayID,
    unsigned int uiMapID,
    unsigned int dwID)
  {
    if (uiDayID == m_pkTodaySchedule->GetDayID())
    {
      return m_pkTodaySchedule->UpdateUseFlag(uiMapID, dwID);
    }
    if (uiDayID == m_pkTomorrowSchedule->GetDayID())
    {
      return m_pkTomorrowSchedule->UpdateUseFlag(uiMapID, dwID);
    }
    return nullptr;
  }

  bool CGuildBattleScheduleManager::GetTodaySLIDByMap(unsigned int uiMap, unsigned int *uiSLID)
  {
    return m_pkTodaySchedule->GetSLID(uiMap, uiSLID) != 0;
  }

  bool CGuildBattleScheduleManager::GetTomorrowSLIDByMap(unsigned int uiMap, unsigned int *uiSLID)
  {
    return m_pkTomorrowSchedule->GetSLID(uiMap, uiSLID) != 0;
  }

  bool CGuildBattleScheduleManager::CleanUpDanglingReservedSchedule()
  {
    return m_pkTodaySchedule->CleanUpDanglingReservedSchedule() != 0
      && m_pkTomorrowSchedule->CleanUpDanglingReservedSchedule() != 0;
  }

  CGuildBattleLogger *CGuildBattleLogger::ms_Instance = nullptr;

  CGuildBattleLogger *CGuildBattleLogger::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CGuildBattleLogger();
    }
    return ms_Instance;
  }

  CGuildBattleLogger::CGuildBattleLogger() : m_pkLogger(nullptr)
  {
  }

  bool CGuildBattleLogger::Init()
  {
    CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\GuildBattle", nullptr);
    clear_file("..\\ZoneServerLog\\Systemlog\\GuildBattle", 0xFu);

    if (!m_pkLogger)
    {
      m_pkLogger = new (std::nothrow) CLogFile();
    }
    if (m_pkLogger)
    {
      CreateLogFile(const_cast<char *>("System"));
      return true;
    }

    g_Main.m_logLoadingError.Write("CGuildBattleLogger::Init() new CLogFile Fail!");
    return false;
  }

  void CGuildBattleLogger::CreateLogFile(char *szLogName)
  {
    const unsigned int korLocalTime = GetKorLocalTime();
    char buffer[128]{};
    if (szLogName)
    {
      sprintf_s(
        buffer,
        "..\\ZoneServerLog\\Systemlog\\GuildBattle\\GuildBattle%s%d.log",
        szLogName,
        korLocalTime);
    }
    else
    {
      sprintf_s(buffer, "..\\ZoneServerLog\\Systemlog\\GuildBattle\\GuildBattle%d.log", korLocalTime);
    }
    m_pkLogger->SetWriteLogFile(buffer, 1, 0, 1, 1);
  }

  void CGuildBattleLogger::Log(const char *fmt, ...)
  {
    if (!m_pkLogger)
    {
      return;
    }
    va_list va;
    va_start(va, fmt);
    m_pkLogger->WriteFromArg(fmt, va);
    va_end(va);
  }

  CGuildBattleRewardItem::CGuildBattleRewardItem() : m_ucD(0), m_ucTableCode(0xFFu), m_pFld(nullptr)
  {
  }

  bool CGuildBattleRewardItem::Init(unsigned __int16 usInx)
  {
    char buffer[1024]{};
    char returned[2048]{};
    sprintf(buffer, "item%d", usInx);
    GetPrivateProfileStringA("RewardItem", buffer, "X", returned, 0x800u, "./Initialize/NormalGuildBattle.ini");
    if (!strcmp_0(returned, "X"))
    {
      return false;
    }

    char itemCode[64]{};
    char amount[64]{};
    char *tokens[2] = {itemCode, amount};
    if (!ParsingCommandA(returned, 2, tokens, 64))
    {
      return false;
    }
    if (!SetItem(tokens[0]))
    {
      return false;
    }
    m_ucD = static_cast<unsigned __int8>(atoi(tokens[1]));
    return true;
  }

  bool CGuildBattleRewardItem::SetItem(char *szItemCode)
  {
    _base_fld *record = nullptr;
    unsigned __int8 tableCode = 0;
    for (; tableCode < 0x25u; ++tableCode)
    {
      record = g_Main.m_tblItemData[tableCode].GetRecord(szItemCode);
      if (record)
      {
        break;
      }
    }
    if (tableCode >= 0x25u)
    {
      return false;
    }
    if (GetItemKindCode(tableCode))
    {
      return false;
    }
    m_ucTableCode = tableCode;
    m_pFld = record;
    return true;
  }

  const CGuildBattleRewardItem *CGuildBattleRewardItem::Give(CPlayer *pkPlayer)
  {
    unsigned int durability = 0;
    if (IsOverLapItem(m_ucTableCode))
    {
      durability = m_ucD;
    }
    else
    {
      durability = GetItemDurPoint(m_ucTableCode, m_pFld->m_dwIndex);
    }

    unsigned __int8 sockets = 0;
    const unsigned __int8 defSockets = GetDefItemUpgSocketNum(m_ucTableCode, m_pFld->m_dwIndex);
    if (defSockets)
    {
      sockets = static_cast<unsigned __int8>(rand() % defSockets + 1);
    }
    const unsigned int dwLv = GetBitAfterSetLimSocket(sockets);

    _STORAGE_LIST::_db_con item{};
    item.m_byTableCode = m_ucTableCode;
    item.m_wItemIndex = m_pFld->m_dwIndex;
    item.m_dwDur = durability;
    item.m_dwLv = dwLv;
    if (pkPlayer->pc_GiveItem(&item, const_cast<char *>("GuildBattle Reward"), true))
    {
      return this;
    }
    return &ms_kNullObj;
  }

  char *CGuildBattleRewardItem::GetItemCode()
  {
    if (m_pFld)
    {
      return m_pFld->m_strCode;
    }
    return const_cast<char *>("None");
  }

  unsigned __int8 CGuildBattleRewardItem::GetAmount()
  {
    return m_ucD;
  }

  bool CGuildBattleRewardItem::IsNull()
  {
    return this == &ms_kNullObj;
  }

  CGuildBattleRewardItemManager *CGuildBattleRewardItemManager::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CGuildBattleRewardItemManager();
    }
    return ms_Instance;
  }

  CGuildBattleRewardItemManager::CGuildBattleRewardItemManager() = default;

  bool CGuildBattleRewardItemManager::Init()
  {
    const unsigned int count =
      GetPrivateProfileIntA("RewardItem", "ItemCnt", 0, "./Initialize/NormalGuildBattle.ini");
    if (!count || count > 10)
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleRewardItemManager::Init() : ItemCnt = %u MAX_CNT(%u)!",
        count,
        10);
      return false;
    }

    CGuildBattleRewardItem item;
    m_kItem.assign(count, item);
    for (unsigned int j = 0; j < count; ++j)
    {
      if (!m_kItem[j].Init(static_cast<unsigned __int16>(j)))
      {
        CGuildBattleLogger::Instance()->Log("CGuildBattleRewardItemManager::Init() : m_kItem[%d].Init()", j);
        return false;
      }
    }
    return true;
  }

  const CGuildBattleRewardItem *CGuildBattleRewardItemManager::Give(CPlayer *pkPlayer)
  {
    const size_t count = m_kItem.size();
    CGuildBattleRewardItem *item = &m_kItem[rand() % count];
    return item->Give(pkPlayer);
  }

  CGuildBattleRankManager *CGuildBattleRankManager::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CGuildBattleRankManager();
    }
    return ms_Instance;
  }

  CGuildBattleRankManager::CGuildBattleRankManager()
  {
    m_dwVer[0] = 0;
    m_dwVer[1] = 0;
    m_dwVer[2] = 0;
  }

  bool CGuildBattleRankManager::Init()
  {
    m_ppkList = new (std::nothrow) _guild_battle_rank_list_result_zocl *[3];
    if (!m_ppkList)
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleRankManager::Init() : NULL == new _guild_battle_rank_list_result_zocl * [%u]",
        3);
      return false;
    }

    memset_0(m_ppkList, 0, sizeof(_guild_battle_rank_list_result_zocl *) * 3);
    for (int j = 0; j < 3; ++j)
    {
      m_ppkList[j] = new (std::nothrow) _guild_battle_rank_list_result_zocl[30];
      if (!m_ppkList[j])
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleRankManager::Init() : NULL == new _guild_battle_rank_list_result_zocl[%u]",
          30);
        return false;
      }
      memset_0(m_ppkList[j], 0, sizeof(_guild_battle_rank_list_result_zocl) * 30);
    }

    memset_0(m_dwGuildSerial, 0, sizeof(m_dwGuildSerial));
    return true;
  }

  char CGuildBattleRankManager::Load()
  {
    for (unsigned __int8 j = 0; j < 3u; ++j)
    {
      if (!Load(j))
      {
        return 0;
      }
    }
    return 1;
  }

  char CGuildBattleRankManager::Load(unsigned __int8 byRace)
  {
    if (byRace >= 3u)
    {
      return 0;
    }

    unsigned __int16 listInx = 0;
    unsigned __int16 pageInx = 0;
    _guild_battle_rank_list_result_zocl_list *entry = nullptr;
    _worlddb_guild_battle_rank_list pkInfo{};
    if (!g_Main.m_pWorldDB->SelectGuildBattleRankList(byRace, &pkInfo))
    {
      return 0;
    }

    Clear(byRace);
    ++m_dwVer[byRace];
    for (unsigned __int16 j = 0; j < static_cast<unsigned int>(pkInfo.wCount); ++j)
    {
      if (listInx >= 0xAu)
      {
        m_ppkList[byRace][pageInx].dwCurVer = m_dwVer[byRace];
        m_ppkList[byRace][pageInx].byCnt = 10;
        listInx = 0;
        ++pageInx;
      }

      entry = &m_ppkList[byRace][pageInx].list[listInx];
      entry->nRank = pkInfo.list[j].nRank;
      entry->byGrade = pkInfo.list[j].byGrade;
      strcpy_0(entry->wszName, pkInfo.list[j].wszName);
      entry->dwWin = pkInfo.list[j].dwWin;
      entry->dwLose = pkInfo.list[j].dwLose;
      entry->dwDraw = pkInfo.list[j].dwDraw;
      entry->dwScore = pkInfo.list[j].dwScore;
      m_dwGuildSerial[byRace][pageInx][listInx++] = pkInfo.list[j].dwSerial;
    }

    m_ppkList[byRace][pageInx].byCnt = static_cast<unsigned __int8>(listInx);
    for (unsigned __int16 k = 0; k <= pageInx; ++k)
    {
      m_ppkList[byRace][k].dwCurVer = m_dwVer[byRace];
      m_ppkList[byRace][k].byMaxPage = static_cast<unsigned __int8>(pageInx + 1);
    }
    return 1;
  }

  char CGuildBattleRankManager::Find(
    unsigned __int8 byRace,
    unsigned int dwGuildSerial,
    int *iFindInx,
    unsigned __int8 *byFindPage)
  {
    if (dwGuildSerial == static_cast<unsigned int>(-1))
    {
      return 0;
    }
    for (unsigned int page = 0; page < 0x1Eu; ++page)
    {
      for (int slot = 0; slot < 10; ++slot)
      {
        if (m_dwGuildSerial[byRace][page][slot] == dwGuildSerial)
        {
          *byFindPage = static_cast<unsigned __int8>(page);
          *iFindInx = slot;
          return 1;
        }
      }
    }
    return 0;
  }

  void CGuildBattleRankManager::Send(
    unsigned int n,
    unsigned __int8 bySelfRace,
    unsigned int dwCurVer,
    char byTabRace,
    unsigned __int8 byPage,
    unsigned int dwGuildSerial)
  {
    const unsigned __int8 tabRace = static_cast<unsigned __int8>(byTabRace);
    if (bySelfRace < 3u && tabRace < 3u && byPage < 0x1Eu && m_dwVer[tabRace] != dwCurVer)
    {
      if (m_ppkList[tabRace]->byMaxPage)
      {
        if (m_ppkList[tabRace]->byMaxPage > static_cast<int>(byPage) && m_ppkList)
        {
          int findIndex = -1;
          unsigned __int8 findPage = static_cast<unsigned __int8>(-1);
          if (Find(bySelfRace, dwGuildSerial, &findIndex, &findPage))
          {
            m_ppkList[tabRace][byPage].byExistSelfGuildInfo = 1;
            m_ppkList[tabRace][byPage].list[10] = m_ppkList[bySelfRace][findPage].list[findIndex];
          }
          else
          {
            m_ppkList[tabRace][byPage].byExistSelfGuildInfo = 0;
          }

          unsigned __int8 type[2] = {27, 52};
          const unsigned __int16 len = static_cast<unsigned __int16>(m_ppkList[tabRace][byPage].size());
          g_Network.m_pProcess[0]->LoadSendMsg(
            n,
            type,
            reinterpret_cast<char *>(&m_ppkList[tabRace][byPage]),
            len);
        }
      }
      else
      {
        char msg[5]{};
        msg[0] = static_cast<char>(tabRace);
        *reinterpret_cast<unsigned int *>(msg + 1) = m_dwVer[tabRace];
        unsigned __int8 type[2] = {27, 90};
        g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, 5u);
      }
    }
  }

  void CGuildBattleRankManager::PushCreateGuildBattleRankTable()
  {
    char buffer[48]{};
    memset(buffer, 0, 10);

    ATL::CTime now = ATL::CTime::GetCurrentTime();
    const int day = now.GetDay();
    const int month = now.GetMonth();
    const int year = now.GetYear();
    sprintf_s(buffer, 0xAu, "%04d%02d%02d", year, month, day);

    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x22u, buffer, 10);
    CPvpUserAndGuildRankingSystem::Instance()->Log("CGuildBattleRankManager::PushCreateGuildBattleRankTable()");
  }

  void CGuildBattleRankManager::PushClearGuildBattleRank()
  {
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x23u, nullptr, 0);
    CPvpUserAndGuildRankingSystem::Instance()->Log("CGuildBattleRankManager::PushClearGuildBattleRank()");
  }

  bool CGuildBattleRankManager::CheckRecord(unsigned int dwGuildSerial)
  {
    return g_Main.m_pWorldDB->SelectGuildBattleRankRecord(dwGuildSerial)
        || g_Main.m_pWorldDB->InsertGuildBattleRankRecord(dwGuildSerial);
  }

  char CGuildBattleRankManager::UpdateWinLose(
    unsigned __int8 byWinRace,
    unsigned int dwWinGuildSerial,
    unsigned __int8 byLoseRace,
    unsigned int dwLoseGuildSerial)
  {
    if (byWinRace >= 3u || byLoseRace >= 3u)
    {
      return 0;
    }
    if (!CheckRecord(dwWinGuildSerial) || !CheckRecord(dwLoseGuildSerial))
    {
      return 0;
    }

    g_Main.m_pWorldDB->SetAutoCommitMode(false);
    if (g_Main.m_pWorldDB->UpdateWinGuildBattleResult(dwWinGuildSerial, 3u)
        && g_Main.m_pWorldDB->UpdateLoseGuildBattleResult(dwLoseGuildSerial, 0))
    {
      g_Main.m_pWorldDB->CommitTransaction();
      g_Main.m_pWorldDB->SetAutoCommitMode(true);
      return 1;
    }

    g_Main.m_pWorldDB->RollbackTransaction();
    g_Main.m_pWorldDB->SetAutoCommitMode(true);
    return 0;
  }

  char CGuildBattleRankManager::UpdateDraw(
    unsigned __int8 by1PRace,
    unsigned int dw1PGuildSerial,
    unsigned __int8 by2PRace,
    unsigned int dw2PGuildSerial)
  {
    if (by1PRace >= 3u || by2PRace >= 3u)
    {
      return 0;
    }
    if (!CheckRecord(dw1PGuildSerial) || !CheckRecord(dw2PGuildSerial))
    {
      return 0;
    }

    g_Main.m_pWorldDB->SetAutoCommitMode(false);
    if (g_Main.m_pWorldDB->UpdateDrawGuildBattleResult(dw1PGuildSerial, 1u)
        && g_Main.m_pWorldDB->UpdateDrawGuildBattleResult(dw2PGuildSerial, 1u))
    {
      g_Main.m_pWorldDB->CommitTransaction();
      g_Main.m_pWorldDB->SetAutoCommitMode(true);
      return 1;
    }

    g_Main.m_pWorldDB->RollbackTransaction();
    g_Main.m_pWorldDB->SetAutoCommitMode(true);
    return 0;
  }

  bool CGuildBattleRankManager::SelectGuildBattleRankList(
    unsigned __int8 byRace,
    _worlddb_guild_battle_rank_list *pOutData)
  {
    if (!pOutData)
    {
      return false;
    }

    memset_0(pOutData, 0, sizeof(_worlddb_guild_battle_rank_list));
    return g_Main.m_pWorldDB->SelectGuildBattleRankList(byRace, pOutData);
  }

  char CGuildBattleRankManager::Update(unsigned __int8 byRace, unsigned __int8 *pLoadData)
  {
    if (byRace >= 3u)
    {
      return 0;
    }

    unsigned __int16 pageInx = 0;
    unsigned __int16 listInx = 0;
    auto *loadData = pLoadData;

    Clear(byRace);
    ++m_dwVer[byRace];

    for (unsigned __int16 j = 0; j < *reinterpret_cast<unsigned __int16 *>(loadData); ++j)
    {
      if (pageInx >= 0xAu)
      {
        m_ppkList[byRace][listInx].dwCurVer = m_dwVer[byRace];
        m_ppkList[byRace][listInx].byCnt = 10;
        pageInx = 0;
        ++listInx;
      }

      auto *entry = &m_ppkList[byRace][listInx].list[pageInx];
      entry->nRank = *reinterpret_cast<unsigned int *>(&loadData[44 * j + 4]);
      entry->byGrade = loadData[44 * j + 8];
      strcpy_0(entry->wszName, reinterpret_cast<const char *>(&loadData[44 * j + 9]));
      entry->dwWin = *reinterpret_cast<unsigned int *>(&loadData[44 * j + 28]);
      entry->dwLose = *reinterpret_cast<unsigned int *>(&loadData[44 * j + 32]);
      entry->dwDraw = *reinterpret_cast<unsigned int *>(&loadData[44 * j + 36]);
      entry->dwScore = *reinterpret_cast<unsigned int *>(&loadData[44 * j + 40]);
      m_dwGuildSerial[byRace][listInx][pageInx++] = *reinterpret_cast<unsigned int *>(&loadData[44 * j + 44]);
    }

    m_ppkList[byRace][listInx].byCnt = static_cast<unsigned __int8>(pageInx);
    for (unsigned __int16 k = 0; k <= listInx; ++k)
    {
      m_ppkList[byRace][k].dwCurVer = m_dwVer[byRace];
      m_ppkList[byRace][k].byMaxPage = static_cast<unsigned __int8>(listInx + 1);
    }
    return 1;
  }

  void CGuildBattleRankManager::Clear()
  {
    for (int j = 0; j < 3; ++j)
    {
      ++m_dwVer[j];
      Clear(static_cast<unsigned __int8>(j));
    }
  }

  void CGuildBattleRankManager::Clear(unsigned __int8 byRace)
  {
    if (byRace >= 3u)
    {
      return;
    }

    memset_0(m_dwGuildSerial[byRace], 0, sizeof(m_dwGuildSerial[byRace]));
    if (!m_ppkList || !m_ppkList[byRace])
    {
      return;
    }

    for (int j = 0; j < 10; ++j)
    {
      m_ppkList[byRace][j].dwCurVer = m_dwVer[byRace];
      m_ppkList[byRace][j].byRace = byRace;
      m_ppkList[byRace][j].byCurPage = static_cast<unsigned __int8>(j);
      m_ppkList[byRace][j].byMaxPage = 0;
      m_ppkList[byRace][j].byExistSelfGuildInfo = static_cast<unsigned __int8>(-1);
      m_ppkList[byRace][j].byCnt = 0;
    }
  }

  CReservedGuildSchedulePage::CReservedGuildSchedulePage()
    : m_pkList(nullptr), m_ucPageInx(static_cast<unsigned __int8>(-1)), m_dwVer(static_cast<unsigned int>(-1))
  {
    memset_0(m_dw1PGuildSerial, 0, sizeof(m_dw1PGuildSerial));
    memset_0(m_dw2PGuildSerial, 0, sizeof(m_dw2PGuildSerial));
  }

  bool CReservedGuildSchedulePage::Init(unsigned __int8 ucPageInx)
  {
    m_pkList = new (std::nothrow) _guild_battle_reserved_schedule_result_zocl();
    if (!m_pkList)
    {
      CGuildBattleLogger::Instance()->Log(
        "CReservedGuildSchedulePage::Init(%u) : m_pkList = new _guild_battle_reserved_schedule_result_zocl NULL!",
        ucPageInx);
      return false;
    }

    m_ucPageInx = ucPageInx;
    return Clear();
  }

  bool CReservedGuildSchedulePage::Clear()
  {
    const int today = GetCurDay();
    const int tomorrow = GetNextDay();
    if (today == -1 || tomorrow == -1)
    {
      CGuildBattleLogger::Instance()->Log(
        "CReservedGuildSchedulePage::InitClear(%u) : -1 == iToDay || -1 == iTomorrow Fail!",
        m_ucPageInx);
      return false;
    }

    m_dwVer = 0;
    memset_0(m_dw1PGuildSerial, 0, sizeof(m_dw1PGuildSerial));
    memset_0(m_dw2PGuildSerial, 0, sizeof(m_dw2PGuildSerial));
    memset_0(m_pkList, 0, sizeof(_guild_battle_reserved_schedule_result_zocl));
    m_pkList->byToDay = static_cast<unsigned __int8>(today);
    m_pkList->byTomorrow = static_cast<unsigned __int8>(tomorrow);
    m_pkList->byPage = m_ucPageInx;
    m_pkList->bySelfScheduleInx = static_cast<unsigned __int8>(-1);
    return true;
  }

  char CReservedGuildSchedulePage::Find(unsigned int dwGuildSerial)
  {
    for (unsigned __int8 j = 0; j < 4u; ++j)
    {
      if (m_dw1PGuildSerial[j] == dwGuildSerial || m_dw2PGuildSerial[j] == dwGuildSerial)
      {
        m_pkList->bySelfScheduleInx = j;
        return 1;
      }
    }
    m_pkList->bySelfScheduleInx = static_cast<unsigned __int8>(-1);
    return 0;
  }

  void CReservedGuildSchedulePage::Send(
    unsigned int n,
    unsigned int dwVer,
    CReservedGuildSchedulePage *pkSelfInfoPage)
  {
    if (!m_pkList)
    {
      return;
    }

    if (m_dwVer == dwVer)
    {
      char msg[4]{};
      *reinterpret_cast<unsigned int *>(msg) = m_dwVer;
      unsigned __int8 type[2] = {27, 59};
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, 4u);
      return;
    }

    if (pkSelfInfoPage)
    {
      if (pkSelfInfoPage->m_pkList->bySelfScheduleInx >= 5u)
      {
        m_pkList->bySelfScheduleInx = 0;
      }
      else
      {
        const char *src = pkSelfInfoPage->m_pkList->List[pkSelfInfoPage->m_pkList->bySelfScheduleInx].wsz1PName;
        strcpy_0(m_pkList->List[4].wsz1PName, src);
        m_pkList->List[4].by1PRace = static_cast<unsigned __int8>(src[17]);
        strcpy_0(m_pkList->List[4].wsz2PName, src + 18);
        m_pkList->List[4].by2PRace = static_cast<unsigned __int8>(src[35]);
        m_pkList->List[4].byStartHour = static_cast<unsigned __int8>(src[36]);
        m_pkList->List[4].byStartMin = static_cast<unsigned __int8>(src[37]);
        m_pkList->List[4].byEndHour = static_cast<unsigned __int8>(src[38]);
        m_pkList->List[4].byEndMin = static_cast<unsigned __int8>(src[39]);
        m_pkList->bySelfScheduleInx = 1;
      }
    }
    else
    {
      m_pkList->bySelfScheduleInx = 0;
    }

    unsigned __int8 type[2] = {27, 61};
    const unsigned __int16 len = static_cast<unsigned __int16>(m_pkList->size());
    g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(m_pkList), len);
  }

  void CReservedGuildSchedulePage::IncVer()
  {
    ++m_dwVer;
    if (m_pkList)
    {
      m_pkList->dwVer = m_dwVer;
    }
  }

  void CReservedGuildSchedulePage::Flip()
  {
    if (!m_pkList)
    {
      return;
    }

    const int today = GetCurDay();
    const int tomorrow = GetNextDay();
    if (today == -1 || tomorrow == -1)
    {
      CGuildBattleLogger::Instance()->Log(
        "CReservedGuildSchedulePage::InitClear(%u) : -1 == iToDay || -1 == iTomorrow Fail!",
        m_ucPageInx);
      return;
    }

    m_pkList->dwVer = ++m_dwVer;
    m_pkList->byDate = 0;
    m_pkList->byToDay = static_cast<unsigned __int8>(today);
    m_pkList->byTomorrow = static_cast<unsigned __int8>(tomorrow);
  }

  CReservedGuildScheduleMapGroup::CReservedGuildScheduleMapGroup()
    : m_uiMapInx(static_cast<unsigned int>(-1)),
      m_byMaxPage(static_cast<unsigned __int8>(-1))
  {
  }

  bool CReservedGuildScheduleMapGroup::Init(unsigned int uiMapInx)
  {
    for (unsigned int j = 0; j < 6; ++j)
    {
      if (!m_kList[j].Init(static_cast<unsigned __int8>(j)))
      {
        return false;
      }
    }
    m_uiMapInx = uiMapInx;
    m_byMaxPage = 0;
    return true;
  }

  char CReservedGuildScheduleMapGroup::Clear()
  {
    for (unsigned int j = 0; j < 6; ++j)
    {
      if (!m_kList[j].Clear())
      {
        return 0;
      }
    }
    return 1;
  }

  char CReservedGuildScheduleMapGroup::Load(
    unsigned __int8 byDayID,
    _worlddb_guild_battle_reserved_schedule_info *kInfo)
  {
    m_byMaxPage = static_cast<unsigned __int8>(kInfo->wCount / 4);
    unsigned __int8 remain = static_cast<unsigned __int8>(kInfo->wCount & 3);
    if (!m_byMaxPage && !remain)
    {
      return 1;
    }

    if (m_byMaxPage >= 6u)
    {
      m_byMaxPage = 5;
      remain = 0;
    }

    unsigned __int16 listIndex = 0;
    char *dest = nullptr;
    _worlddb_guild_battle_reserved_schedule_info::__list *src = nullptr;
    unsigned __int8 page = 0;

    for (page = 0; page < m_byMaxPage; ++page)
    {
      unsigned __int8 slot = 0;
      while (slot < 4u)
      {
        dest = m_kList[page].m_pkList->List[slot].wsz1PName;
        src = &kInfo->list[listIndex];
        m_kList[page].m_dw1PGuildSerial[slot] = src->dw1PGuildSerial;
        strcpy_0(dest, src->wsz1PName);
        dest[17] = src->by1PRace;
        m_kList[page].m_dw2PGuildSerial[slot] = kInfo->list[listIndex].dw2PGuildSerial;
        strcpy_0(dest + 18, src->wsz2PName);
        dest[35] = src->by2PRace;
        dest[36] = src->byStartHour;
        dest[37] = src->byStartMin;
        dest[38] = src->byEndHour;
        dest[39] = src->byEndMin;
        if (kInfo->wCount <= listIndex)
        {
          break;
        }
        ++slot;
        ++listIndex;
      }

      m_kList[page].m_pkList->byDate = byDayID;
      m_kList[page].m_pkList->byCnt = 4;
    }

    if (remain)
    {
      if (++m_byMaxPage > 6u)
      {
        m_byMaxPage = 6;
      }
    }

    unsigned __int8 tailSlot = 0;
    while (tailSlot < remain)
    {
      dest = m_kList[page].m_pkList->List[tailSlot].wsz1PName;
      src = &kInfo->list[listIndex];
      m_kList[page].m_dw1PGuildSerial[tailSlot] = src->dw1PGuildSerial;
      strcpy_0(dest, src->wsz1PName);
      dest[17] = src->by1PRace;
      m_kList[page].m_dw2PGuildSerial[tailSlot] = kInfo->list[listIndex].dw2PGuildSerial;
      strcpy_0(dest + 18, src->wsz2PName);
      dest[35] = src->by2PRace;
      dest[36] = src->byStartHour;
      dest[37] = src->byStartMin;
      dest[38] = src->byEndHour;
      dest[39] = src->byEndMin;
      if (kInfo->wCount <= listIndex)
      {
        break;
      }
      ++tailSlot;
      ++listIndex;
    }

    if (remain)
    {
      m_kList[page].m_pkList->byDate = byDayID;
      m_kList[page].m_pkList->byCnt = static_cast<unsigned __int8>(listIndex & 3);
    }

    for (page = 0; page < m_byMaxPage; ++page)
    {
      m_kList[page].m_pkList->byMaxPage = m_byMaxPage;
      m_kList[page].IncVer();
    }
    return 1;
  }

  unsigned __int8 CReservedGuildScheduleMapGroup::GetMaxPage()
  {
    return m_byMaxPage;
  }

  CReservedGuildSchedulePage *CReservedGuildScheduleMapGroup::Find(unsigned int dwGuildSerial)
  {
    for (unsigned __int8 j = 0; j < static_cast<unsigned __int8>(m_byMaxPage); ++j)
    {
      if (m_kList[j].Find(dwGuildSerial))
      {
        return &m_kList[j];
      }
    }
    return nullptr;
  }

  void CReservedGuildScheduleMapGroup::Send(
    int n,
    unsigned int dwVer,
    unsigned __int8 byPage,
    CReservedGuildSchedulePage *pkSelfInfoPage)
  {
    if (m_byMaxPage > static_cast<int>(byPage))
    {
      m_kList[byPage].Send(n, dwVer, pkSelfInfoPage);
    }
  }

  void CReservedGuildScheduleMapGroup::Flip()
  {
    for (unsigned __int8 j = 0; j < m_byMaxPage; ++j)
    {
      m_kList[j].Flip();
    }
  }

  CReservedGuildScheduleDayGroup::CReservedGuildScheduleDayGroup() : m_uiMapCnt(0), m_pkList(nullptr)
  {
  }

  bool CReservedGuildScheduleDayGroup::Init(unsigned int uiMapCnt)
  {
    m_uiMapCnt = uiMapCnt;
    if (!m_uiMapCnt)
    {
      return false;
    }

    m_pkList = new (std::nothrow) CReservedGuildScheduleMapGroup[m_uiMapCnt];
    if (!m_pkList)
    {
      CGuildBattleLogger::Instance()->Log(
        "CReservedGuildScheduleDayGroup::Init(%u) m_pkList = new CReservedGuildScheduleDayGroup[%u] NULL!",
        uiMapCnt,
        uiMapCnt);
      return false;
    }

    for (unsigned int uiMapInx = 0; uiMapInx < m_uiMapCnt; ++uiMapInx)
    {
      if (!m_pkList[uiMapInx].Init(uiMapInx))
      {
        CGuildBattleLogger::Instance()->Log(
          "CReservedGuildScheduleDayGroup::Init(%u) m_pkList[i].Init( i ) Fail",
          m_uiMapCnt);
        return false;
      }
    }

    m_byToday = static_cast<unsigned __int8>(GetCurDay());
    m_byTommorow = static_cast<unsigned __int8>(GetNextDay());
    return true;
  }

  bool CReservedGuildScheduleDayGroup::Load(
    unsigned __int8 byDayID,
    unsigned int uiMapInx,
    _worlddb_guild_battle_reserved_schedule_info *kInfo)
  {
    return m_uiMapCnt > uiMapInx && m_pkList
           && m_pkList[uiMapInx].Load(byDayID, kInfo);
  }

  char CReservedGuildScheduleDayGroup::Clear()
  {
    for (unsigned int j = 0; j < m_uiMapCnt; ++j)
    {
      if (!m_pkList[j].Clear())
      {
        return 0;
      }
    }
    m_byToday = static_cast<unsigned __int8>(GetCurDay());
    m_byTommorow = static_cast<unsigned __int8>(GetNextDay());
    return 1;
  }

  CReservedGuildSchedulePage *CReservedGuildScheduleDayGroup::Find(unsigned int dwGuildSerial)
  {
    for (unsigned int j = 0; j < m_uiMapCnt; ++j)
    {
      CReservedGuildSchedulePage *page = m_pkList[j].Find(dwGuildSerial);
      if (page)
      {
        return page;
      }
    }
    return nullptr;
  }

  void CReservedGuildScheduleDayGroup::Send(
    char byDayID,
    unsigned int uiMapID,
    unsigned int n,
    unsigned int dwVer,
    unsigned __int8 byPage,
    unsigned int dwGuildSerial)
  {
    if (m_pkList && m_uiMapCnt > uiMapID)
    {
      if (m_pkList[uiMapID].GetMaxPage())
      {
        CReservedGuildSchedulePage *selfInfoPage = nullptr;
        if (dwGuildSerial != static_cast<unsigned int>(-1))
        {
          selfInfoPage = Find(dwGuildSerial);
        }
        m_pkList[uiMapID].Send(static_cast<int>(n), dwVer, byPage, selfInfoPage);
      }
      else
      {
        char msg[3]{};
        msg[0] = static_cast<char>(m_byToday);
        msg[1] = static_cast<char>(m_byTommorow);
        msg[2] = byDayID;
        unsigned __int8 type[2] = {27, 60};
        g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, 3u);
      }
    }
  }

  void CReservedGuildScheduleDayGroup::Flip()
  {
    m_byToday = static_cast<unsigned __int8>(GetCurDay());
    m_byTommorow = static_cast<unsigned __int8>(GetNextDay());
    if (m_pkList)
    {
      for (unsigned int j = 0; j < m_uiMapCnt; ++j)
      {
        m_pkList[j].Flip();
      }
    }
  }

  CGuildBattleReservedScheduleListManager *CGuildBattleReservedScheduleListManager::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CGuildBattleReservedScheduleListManager();
    }
    return ms_Instance;
  }

  CGuildBattleReservedScheduleListManager::CGuildBattleReservedScheduleListManager()
    : m_uiMapCnt(0), m_pkToday(m_kList), m_pkTomorrow(&m_kList[1])
  {
  }

  bool CGuildBattleReservedScheduleListManager::Init()
  {
    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    m_uiMapCnt = fieldList->GetMapCnt();
    if (!m_uiMapCnt)
    {
      return false;
    }
    return m_kList[0].Init(m_uiMapCnt) && m_kList[1].Init(m_uiMapCnt);
  }

  char CGuildBattleReservedScheduleListManager::Load(
    int iCurDay,
    unsigned int uiOldMapCnt,
    int iToday,
    int iTomorrow)
  {
    if (m_uiMapCnt != uiOldMapCnt)
    {
      return 1;
    }

    if (iToday == iCurDay)
    {
      if (!LoadTodaySchedule())
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::Load(%d,%u,%d,%d) : LoadTodaySchedule() Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTomorrow);
        return 0;
      }
      if (!LoadTomorrowSchedule())
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::Load(%d,%u,%d,%d) : LoadTomorrowSchedule() Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTomorrow);
        return 0;
      }
    }
    else if (iTomorrow == iCurDay)
    {
      if (!LoadTomorrowSchedule())
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::Load(%d,%u,%d,%d) : LoadTomorrowSchedule() Fail!",
          iCurDay,
          uiOldMapCnt,
          iToday,
          iTomorrow);
        return 0;
      }
      Flip();
    }

    return 1;
  }

  char CGuildBattleReservedScheduleListManager::LoadTodaySchedule()
  {
    if (!m_uiMapCnt)
    {
      return 0;
    }

    for (unsigned int uiMapID = 0; uiMapID < m_uiMapCnt; ++uiMapID)
    {
      if (!UpdateTodaySchedule(uiMapID))
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::LoadTodaySchedule() : UpdateTodaySchedule( %u ) Fail!",
          uiMapID);
        return 0;
      }
    }
    return 1;
  }

  char CGuildBattleReservedScheduleListManager::LoadTomorrowSchedule()
  {
    if (!m_uiMapCnt)
    {
      return 0;
    }

    for (unsigned int uiMapID = 0; uiMapID < m_uiMapCnt; ++uiMapID)
    {
      if (!UpdateTomorrowSchedule(uiMapID))
      {
        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::LoadTodaySchedule() : UpdateTomorrowSchedule( %u ) Fail!",
          uiMapID);
        return 0;
      }
    }
    return 1;
  }

  char CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(unsigned int uiMapID)
  {
    if (m_uiMapCnt <= uiMapID)
    {
      return 0;
    }

    unsigned int uiSLID = 0;
    CGuildBattleScheduleManager *scheduleManager = CGuildBattleScheduleManager::Instance();
    if (scheduleManager->GetTodaySLIDByMap(uiMapID, &uiSLID))
    {
      _worlddb_guild_battle_reserved_schedule_info pkInfo{};
      if (g_Main.m_pWorldDB->SelectGuildBattleRerservedList(uiSLID, uiSLID, &pkInfo))
      {
        if (m_pkToday->Load(0, uiMapID, &pkInfo))
        {
          return 1;
        }

        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(%u) : m_pkToday->Load( %u ) Fail!",
          uiMapID,
          uiMapID);
        return 0;
      }

      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(%u) : g_Main.m_pWorldDB->SelectGuildBattleRerservedList( %u, %u ) Fail!",
        uiMapID,
        uiSLID,
        uiSLID);
      return 0;
    }

    CGuildBattleLogger::Instance()->Log(
      "CGuildBattleReservedScheduleListManager::UpdateTodaySchedule(%u) : CGuildBattleScheduler::Instance()->GetTodaySLIDByMap( %u, %u ) Fail!",
      uiMapID,
      uiMapID,
      uiSLID);
    return 0;
  }

  char CGuildBattleReservedScheduleListManager::UpdateTomorrowSchedule(unsigned int uiMapID)
  {
    if (m_uiMapCnt <= uiMapID)
    {
      return 0;
    }

    unsigned int uiSLID = 0;
    CGuildBattleScheduleManager *scheduleManager = CGuildBattleScheduleManager::Instance();
    if (scheduleManager->GetTomorrowSLIDByMap(uiMapID, &uiSLID))
    {
      _worlddb_guild_battle_reserved_schedule_info pkInfo{};
      if (g_Main.m_pWorldDB->SelectGuildBattleRerservedList(uiSLID, uiSLID, &pkInfo))
      {
        if (m_pkTomorrow->Load(1u, uiMapID, &pkInfo))
        {
          return 1;
        }

        CGuildBattleLogger::Instance()->Log(
          "CGuildBattleReservedScheduleListManager::UpdateTomorrowScheduleLoad(%u) : m_pkTomorrow->Load( %u ) Fail!",
          uiMapID,
          uiMapID);
        return 0;
      }

      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleReservedScheduleListManager::UpdateTomorrowSchedule(%u) : g_Main.m_pWorldDB->SelectGuildBattleRerservedList( %u, %u ) Fail!",
        uiMapID,
        uiSLID,
        uiSLID);
      return 0;
    }

    CGuildBattleLogger::Instance()->Log(
      "CGuildBattleReservedScheduleListManager::UpdateTomorrowSchedule(%u) : CGuildBattleScheduler::Instance()->GetTomorrowSLIDRange( %u, %u ) Fail!",
      uiMapID,
      uiMapID,
      uiSLID);
    return 0;
  }

  void CGuildBattleReservedScheduleListManager::Clear()
  {
    m_pkToday->Clear();
    m_pkTomorrow->Clear();
  }

  void CGuildBattleReservedScheduleListManager::Flip()
  {
    CReservedGuildScheduleDayGroup *prevToday = m_pkToday;
    m_pkToday = m_pkTomorrow;
    m_pkTomorrow = prevToday;
    m_pkToday->Flip();
    m_pkTomorrow->Clear();
  }

  void CGuildBattleReservedScheduleListManager::Send(
    unsigned int uiMapID,
    int n,
    unsigned int dwVer,
    unsigned __int8 byDay,
    unsigned __int8 byPage,
    unsigned int dwGuildSerial)
  {
    if (byDay < 2u)
    {
      if (byDay)
      {
        m_pkTomorrow->Send(1u, uiMapID, n, dwVer, byPage, dwGuildSerial);
      }
      else
      {
        m_pkToday->Send(0, uiMapID, n, dwVer, byPage, dwGuildSerial);
      }
    }
  }

  void CGuildBattleReservedScheduleListManager::PushDQS(unsigned int dwMapID, unsigned int dwSLID)
  {
    _qry_case_updatereservedschedule qry{};
    qry.dwMapID = dwMapID;
    qry.dwSLID = dwSLID;
    qry.byLoadDataStartPosition = 0;

    const int size = static_cast<int>(qry.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x29u, reinterpret_cast<char *>(&qry), size);
  }

  bool CGuildBattleReservedScheduleListManager::UpdateReservedShedule(
    unsigned int dwSLID,
    _worlddb_guild_battle_reserved_schedule_info *byOutData)
  {
    if (!byOutData)
    {
      return false;
    }

    memset_0(byOutData, 0, sizeof(_worlddb_guild_battle_reserved_schedule_info));
    return g_Main.m_pWorldDB->SelectGuildBattleRerservedList(dwSLID, dwSLID, byOutData);
  }

  void CGuildBattleReservedScheduleListManager::UpdateTomorrowComplete(
    unsigned int dwMapID,
    _worlddb_guild_battle_reserved_schedule_info *pLoadData)
  {
    if (!m_pkTomorrow->Load(1u, dwMapID, pLoadData))
    {
      CGuildBattleLogger::Instance()->Log(
        "CGuildBattleReservedScheduleListManager::UpdateTomorrowComplete(%u) : m_pkToday->Load( %u ) Fail!",
        dwMapID,
        dwMapID);
    }
  }

  CGuildBattleScheduler *CGuildBattleScheduler::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CGuildBattleScheduler();
    }
    return ms_Instance;
  }

  bool CGuildBattleScheduler::Init()
  {
    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    const unsigned int uiMapCnt = fieldList->GetMapCnt();
    if (!CGuildBattleSchedulePool::Instance()->Init(uiMapCnt))
    {
      return false;
    }
    return CGuildBattleScheduleManager::Instance()->Init(uiMapCnt);
  }

  char CGuildBattleScheduler::UpdateClearGuildBattleScheduleDayInfo(unsigned int dwStartSLID, unsigned int dwEndSLID)
  {
    if (g_Main.m_pWorldDB->UpdateClearGuildBattleScheduleInfo(dwStartSLID, dwEndSLID))
    {
      return 1;
    }

    g_Main.m_logDQS.Write(
      "CGuildBattleReservedScheduleMapGroup::UpdateClearGuildBattleScheduleDayInfo() : g_Main.m_pWorldDB->UpdateClearGuildBattleScheduleInfo( %u, %u ) Fail!",
      dwStartSLID,
      dwEndSLID);
    return 0;
  }

  void CGuildBattleScheduler::Destroy()
  {
    if (ms_Instance)
    {
      delete ms_Instance;
      ms_Instance = nullptr;
    }
  }

  CCurrentGuildBattleInfoManager *CCurrentGuildBattleInfoManager::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CCurrentGuildBattleInfoManager();
    }
    return ms_Instance;
  }

  CCurrentGuildBattleInfoManager::CCurrentGuildBattleInfoManager() : m_bInit(false), m_uiMapCnt(0), m_pbUpdate(nullptr)
  {
  }

  bool CCurrentGuildBattleInfoManager::Init()
  {
    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    m_uiMapCnt = fieldList->GetMapCnt();
    if (!m_uiMapCnt)
    {
      return false;
    }

    m_pkInfo = new (std::nothrow) _guild_battle_current_battle_info_result_zocl[m_uiMapCnt];
    if (!m_pkInfo)
    {
      CGuildBattleLogger::Instance()->Log(
        "CCurrentGuildBattleInfoManager::Init()new _guild_battle_current_battle_info_result_zocl[%u] NULL!",
        m_uiMapCnt);
      return false;
    }
    memset_0(m_pkInfo, 0, sizeof(_guild_battle_current_battle_info_result_zocl) * m_uiMapCnt);

    m_pbUpdate = new (std::nothrow) bool[m_uiMapCnt];
    if (!m_pbUpdate)
    {
      CGuildBattleLogger::Instance()->Log("CCurrentGuildBattleInfoManager::Init()new bool[%u] NULL!", m_uiMapCnt);
      return false;
    }
    memset_0(m_pbUpdate, 0, sizeof(bool) * m_uiMapCnt);

    m_bInit = true;
    return true;
  }

  void CCurrentGuildBattleInfoManager::Send(unsigned int n, unsigned int uiMapID)
  {
    if (m_bInit && m_uiMapCnt > uiMapID)
    {
      unsigned __int8 type[2] = {27, 0};
      unsigned __int8 leftResult = 1;
      if (m_pbUpdate[uiMapID])
      {
        leftResult = GetLeftTime(uiMapID);
      }

      if (leftResult)
      {
        type[1] = 63;
        char msg[1]{};
        msg[0] = static_cast<char>(leftResult);
        g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, 1u);
      }
      else
      {
        type[1] = 64;
        g_Network.m_pProcess[0]->LoadSendMsg(
          n,
          type,
          reinterpret_cast<char *>(&m_pkInfo[uiMapID]),
          0x35u);
      }
    }
  }

  unsigned __int8 CCurrentGuildBattleInfoManager::GetLeftTime(unsigned int uiMapID)
  {
    CGuildBattleScheduleManager *scheduleManager = CGuildBattleScheduleManager::Instance();
    const unsigned int sid = scheduleManager->GetCurScheduleID(uiMapID);
    if (sid == static_cast<unsigned int>(-1))
    {
      CGuildBattleLogger::Instance()->Log(
        "CCurrentGuildBattleInfoManager::GetLeftTime( %u )CGuildBattleScheduleManager::Instance()->GetCurScheduleID( %u ) Fail!",
        uiMapID,
        uiMapID);
      return 1;
    }

    CGuildBattleSchedulePool *pool = CGuildBattleSchedulePool::Instance();
    CGuildBattleSchedule *schedule = pool->GetRef(sid);
    if (!schedule)
    {
      CGuildBattleLogger::Instance()->Log(
        "CCurrentGuildBattleInfoManager::GetLeftTime( %u )CGuildBattleSchedulePool::Instance()->GetRef(%u) NULL!",
        uiMapID,
        sid);
      return 2;
    }

    if (schedule->GetLeftTime(
          &m_pkInfo[uiMapID].byLeftHour,
          &m_pkInfo[uiMapID].byLeftMin,
          &m_pkInfo[uiMapID].byLeftSec))
    {
      return 0;
    }

    CGuildBattleLogger::Instance()->Log(
      "CCurrentGuildBattleInfoManager::GetLeftTime( %u )pCurSchedule->GetLeftTime( ... )",
      uiMapID);
    return 2;
  }

  void CCurrentGuildBattleInfoManager::UpdateGoalCnt(
    unsigned int uiMapID,
    unsigned __int8 byColorInx,
    unsigned int dwGoalCnt)
  {
    if (m_bInit && m_uiMapCnt > uiMapID && byColorInx < 2u)
    {
      if (byColorInx)
      {
        m_pkInfo[uiMapID].dwRighBluetGoalCnt = dwGoalCnt;
      }
      else
      {
        m_pkInfo[uiMapID].dwLeftRedGoalCnt = dwGoalCnt;
      }
      m_pbUpdate[uiMapID] = true;
    }
  }

  void CCurrentGuildBattleInfoManager::UpdateScore(
    unsigned int uiMapID,
    unsigned __int8 byColorInx,
    unsigned int dwScore)
  {
    if (m_bInit && m_uiMapCnt > uiMapID && byColorInx < 2u)
    {
      if (byColorInx)
      {
        m_pkInfo[uiMapID].dwRighBluetScore = dwScore;
      }
      else
      {
        m_pkInfo[uiMapID].dwLeftRedScore = dwScore;
      }
      m_pbUpdate[uiMapID] = true;
    }
  }

  CPossibleBattleGuildListManager *CPossibleBattleGuildListManager::ms_Instance = nullptr;

  CPossibleBattleGuildListManager *CPossibleBattleGuildListManager::Instance()
  {
    if (!ms_Instance)
    {
      ms_Instance = new (std::nothrow) CPossibleBattleGuildListManager();
    }
    return ms_Instance;
  }

  CPossibleBattleGuildListManager::CPossibleBattleGuildListManager()
    : m_bInit(false), m_pdwVer(nullptr), m_pMaxPage(nullptr), m_ppkList(nullptr)
  {
  }

  bool CPossibleBattleGuildListManager::Init()
  {
    m_pdwVer = new (std::nothrow) unsigned int[3];
    if (!m_pdwVer)
    {
      CGuildBattleLogger::Instance()->Log(
        "CPossibleBattleGuildListManager::Init()m_pdwVer == new DWORD[%u] NULL!",
        3);
      return false;
    }
    memset_0(m_pdwVer, 0, sizeof(unsigned int) * 3);

    m_pMaxPage = new (std::nothrow) unsigned __int8[3];
    if (!m_pMaxPage)
    {
      CGuildBattleLogger::Instance()->Log(
        "CPossibleBattleGuildListManager::Init()m_pMaxPage == new BYTE[%u] NULL!",
        3);
      return false;
    }
    memset_0(m_pMaxPage, 0, sizeof(unsigned __int8) * 3);

    m_ppkList = new (std::nothrow) _possible_battle_guild_list_result_zocl *[3];
    if (!m_ppkList)
    {
      CGuildBattleLogger::Instance()->Log(
        "CPossibleBattleGuildListManager::Init()m_ppkList = new _possible_battle_guild_list_result_zocl * [%u] Fail",
        3);
      return false;
    }

    for (int j = 0; j < 3; ++j)
    {
      m_ppkList[j] = new (std::nothrow) _possible_battle_guild_list_result_zocl[75];
      if (!m_ppkList[j])
      {
        CGuildBattleLogger::Instance()->Log(
          "CPossibleBattleGuildListManager::Init()m_ppkList[%d] = new _possible_battle_guild_list_result_zocl[%u] Fail",
          j,
          75);
        return false;
      }
      memset_0(m_ppkList[j], 0, sizeof(_possible_battle_guild_list_result_zocl) * 75);
    }

    m_bInit = true;
    return true;
  }

  char CPossibleBattleGuildListManager::Load()
  {
    if (!m_bInit)
    {
      return 0;
    }

    UpdateGuildList();
    CNormalGuildBattleFieldList *fieldList = CNormalGuildBattleFieldList::Instance();
    for (int j = 0; j < 3; ++j)
    {
      for (int k = 0; k < 75; ++k)
      {
        const unsigned __int8 count = static_cast<unsigned __int8>(
          fieldList->GetMapInxList(static_cast<unsigned __int8>(j), m_ppkList[j][k].byMapInx));
        m_ppkList[j][k].byMapCnt = count;
      }
    }
    return 1;
  }

  void CPossibleBattleGuildListManager::Clear()
  {
    DoDayChangedWork();
  }

  void CPossibleBattleGuildListManager::DoDayChangedWork()
  {
    for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
    {
      if (g_Guild[guildIndex].IsFill())
      {
        g_Guild[guildIndex].ClearGuildBattle();
      }
    }

    UpdateGuildList();
  }

  void CPossibleBattleGuildListManager::UpdateGuildList()
  {
    if (!m_bInit)
    {
      return;
    }

    unsigned __int16 lastGuildInx[8]{};
    for (unsigned int race = 0; race < 3; ++race)
    {
      m_pMaxPage[race] = 0;
      for (int page = 0; page < 75 && MakePage(static_cast<unsigned __int8>(race), page, lastGuildInx); ++page)
      {
        ++m_pMaxPage[race];
      }
      ++m_pdwVer[race];
      lastGuildInx[0] = 0;
    }

    for (unsigned int race = 0; race < 3; ++race)
    {
      for (unsigned __int8 page = 0; page < m_pMaxPage[race]; ++page)
      {
        m_ppkList[race][page].byMaxPage = m_pMaxPage[race];
        m_ppkList[race][page].dwCurVer = m_pdwVer[race];
      }
    }
  }

  bool CPossibleBattleGuildListManager::MakePage(
    unsigned __int8 byRace,
    unsigned __int8 ucPage,
    unsigned __int16 *wLastGuildInx)
  {
    if (!m_bInit || byRace >= 3u)
    {
      return false;
    }

    _possible_battle_guild_list_result_zocl::__list *destGuild = m_ppkList[byRace][ucPage].DestGuild;
    const int start = 4 * ucPage;
    const int end = start + 4;

    m_ppkList[byRace][ucPage].byCount = 0;
    memset_0(m_ppkList[byRace][ucPage].DestGuild, 0, sizeof(m_ppkList[byRace][ucPage].DestGuild));
    m_ppkList[byRace][ucPage].byPage = ucPage;
    m_ppkList[byRace][ucPage].dwBattleCost = 5000;

    unsigned __int8 *count = &m_ppkList[byRace][ucPage].byCount;
    for (int idx = start; idx < end; ++idx)
    {
      bool found = false;
      for (int guildIndex = *wLastGuildInx; guildIndex < MAX_GUILD; ++guildIndex)
      {
        if (g_Guild[guildIndex].IsFill()
            && LIMIT_DEST_GRADE <= g_Guild[guildIndex].m_byGrade
            && g_Guild[guildIndex].m_GuildBattleSugestMatter.eState
                 != _guild_battle_suggest_matter::COMPLETE_BATTLE_REQUEST
            && g_Guild[guildIndex].m_byRace == byRace)
        {
          *wLastGuildInx = static_cast<unsigned __int16>(guildIndex);
          found = true;
          break;
        }
      }

      if (!found)
      {
        break;
      }

      strcpy_0(destGuild[*count].wszName, g_Guild[*wLastGuildInx].m_wszName);
      destGuild[*count].byGrade = g_Guild[*wLastGuildInx].m_byGrade;
      destGuild[*count].byRace = g_Guild[*wLastGuildInx].m_byRace;
      destGuild[*count].dwGuildSerial = g_Guild[*wLastGuildInx].m_dwSerial;
      ++(*count);
      ++(*wLastGuildInx);
    }

    return *count != 0;
  }

  void CPossibleBattleGuildListManager::SendFirst(int n, unsigned __int8 byRace)
  {
    SendInfo(n, byRace, 0, 0);
  }

  void CPossibleBattleGuildListManager::SendErrorResult(unsigned int n, char byRet)
  {
    char msg[32]{};
    msg[0] = byRet;

    unsigned __int8 type[2] = {27, 48};
    g_Network.m_pProcess[0]->LoadSendMsg(n, type, msg, 1u);
  }

  unsigned __int8 CPossibleBattleGuildListManager::SendInfo(
    unsigned int n,
    unsigned __int8 byRace,
    unsigned __int8 byPage,
    unsigned int dwVer)
  {
    if (!m_bInit)
    {
      return 110;
    }
    if (byRace > 3u)
    {
      return static_cast<unsigned __int8>(-113);
    }
    if (!m_pMaxPage[byRace])
    {
      return static_cast<unsigned __int8>(-109);
    }
    if (m_pMaxPage[byRace] < static_cast<int>(byPage))
    {
      return static_cast<unsigned __int8>(-113);
    }
    if (m_pdwVer[byRace] == dwVer)
    {
      return 1;
    }
    unsigned __int8 pbyType[2] = {27, 49};
    const unsigned __int16 len = static_cast<unsigned __int16>(m_ppkList[byRace][byPage].size());
    g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&m_ppkList[byRace][byPage]), len);
    return 0;
  }
} // namespace GUILD_BATTLE
