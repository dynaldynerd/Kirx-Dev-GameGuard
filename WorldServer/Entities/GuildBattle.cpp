
#include "pch.h"

#include "GuildBattle.h"
#include "GuildBattleTypes.h"

#include <cstdarg>
#include <new>

#include "CLogFile.h"
#include "CMainThread.h"
#include "CMapData.h"
#include "CNetworkEX.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CGuild.h"
#include "GlobalObjects.h"

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
      m_dPvpPoint = CPlayerDB::GetPvPPoint(&m_pkMember->pPlayer->m_Param);
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
        const char *charName = player ? CPlayerDB::GetCharNameW(&player->m_Param) : "NULL";
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
