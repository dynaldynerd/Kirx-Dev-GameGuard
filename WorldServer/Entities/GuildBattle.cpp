
#include "pch.h"

#include "GuildBattle.h"
#include "GuildBattleTypes.h"

#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

#include "CLogFile.h"
#include "CMainThread.h"
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
#include "GlobalObjects.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "StorageList.h"
#include "WorldServerUtil.h"
#include "base_fld.h"

__int64 _qry_case_in_guildbattlecost::size()
{
  return 40;
}

__int64 _qry_case_dest_guild_out_guildbattlecost::size()
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

  CNormalGuildBattleStateInBattle::CNormalGuildBattleStateInBattle()
    : m_kInBattleTime(0LL),
      m_kRountStateList()
  {
  }

  void CNormalGuildBattleStateInBattle::SetBattleTime(ATL::CTimeSpan kTime)
  {
    m_kInBattleTime = kTime;
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

  bool CNormalGuildBattleStateList::IsReadyOrCountState()
  {
    return m_pkCurState == &READY || m_pkCurState == &COUNT;
  }

  bool CNormalGuildBattleStateList::IsInBattle()
  {
    return m_pkCurState == &INBATTLE;
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
    (void)tmp;

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

  void CNormalGuildBattleGuildMember::SetBattleState(bool bFlag, unsigned __int8 byColorInx)
  {
    m_pkMember->pPlayer->pc_SetInGuildBattle(bFlag, byColorInx);
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
    (void)kLogger;
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

  unsigned int CGuildBattleReservedSchedule::GetID()
  {
    return m_uiScheduleListID;
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

  void CPossibleBattleGuildListManager::SendFirst(int n, unsigned __int8 byRace)
  {
    SendInfo(n, byRace, 0, 0);
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
