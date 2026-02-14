#include "pch.h"

#include "CGuild.h"

#include <cstdlib>

#include "CGuildMasterEffect.h"
#include "CMgrGuildHistory.h"
#include "CNationSettingManager.h"
#include "CNetworkEX.h"
#include "CGuildRoomSystem.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "GlobalObjects.h"
#include "CGuildBattleController.h"
#include "CHonorGuild.h"
#include "CMoneySupplyMgr.h"
#include "DqsDbStructs.h"
#include "GuildBattleTypes.h"
#include "qry_case_in_guildbattlerewardmoney.h"
#include "guild_battle_suggest_request_result_zocl.h"
#include "guild_member_refresh_data.h"
#include "guild_alter_member_grade_inform_zocl.h"
#include "guild_alter_member_state_inform_zocl.h"
#include "guild_vote_process_inform_zocl.h"
#include "WorldServerUtil.h"

CGuildList CGuild::s_GuildList;
CMgrGuildHistory CGuild::s_MgrHistory;

static const char kUnknownGuildEmblemName[] = "<unknown>";

namespace
{
  struct GuildRankSortEntry
  {
    unsigned __int8 byGrade;
    unsigned int dwPvpPoint;
    int nIndex;
  };

  int CompareGradeAndPvpPoint(const void *arg1, const void *arg2)
  {
    GuildRankSortEntry lhs{};
    GuildRankSortEntry rhs{};
    memcpy_0(&lhs, arg1, sizeof(lhs));
    memcpy_0(&rhs, arg2, sizeof(rhs));

    if (lhs.byGrade == rhs.byGrade)
    {
      if (lhs.dwPvpPoint == rhs.dwPvpPoint)
      {
        return 0;
      }
      return lhs.dwPvpPoint >= rhs.dwPvpPoint ? -1 : 1;
    }

    return lhs.byGrade >= rhs.byGrade ? -1 : 1;
  }

  void SortRankInGuild(CGuild *pGuild)
  {
    GuildRankSortEntry entries[50]{};

    for (int j = 0; j < pGuild->m_nMemberNum; ++j)
    {
      _guild_member_info *member = &pGuild->m_MemberData[j];
      if (member->IsFill())
      {
        entries[j].byGrade = member->byGrade;
        entries[j].dwPvpPoint = member->dwPvpPoint;
        entries[j].nIndex = j;
      }
    }

    qsort(entries, pGuild->m_nMemberNum, sizeof(GuildRankSortEntry), CompareGradeAndPvpPoint);

    for (int j = 0; j < pGuild->m_nMemberNum; ++j)
    {
      pGuild->m_MemberData[entries[j].nIndex].byRank = static_cast<unsigned __int8>(j);
    }
  }

  void BuildBuddyPacket(CGuild *pGuild)
  {
    pGuild->m_Buddy_List->wDataSize = 0;

    struct GuildBuddyData
    {
      unsigned int dwSerial;
      unsigned __int16 wMapCode;
      unsigned __int8 byRegionIndex;
    };

    GuildBuddyData buddyData[50]{};
    unsigned __int8 memberCount = 0;

    for (int j = 0; j < 50; ++j)
    {
      _guild_member_info *member = &pGuild->m_MemberData[j];
      if (member->IsFill() && member->pPlayer)
      {
        buddyData[memberCount].dwSerial = member->dwSerial;
        buddyData[memberCount].wMapCode = member->pPlayer->m_wRegionMapIndex;
        buddyData[memberCount].byRegionIndex = static_cast<unsigned __int8>(member->pPlayer->m_wRegionIndex);
        ++memberCount;
      }
    }

    int dataSize = 0;
    void *sData = pGuild->m_Buddy_List->sData;

    memcpy_0(sData, &memberCount, 1u);
    sData = static_cast<char *>(sData) + 1;
    ++dataSize;

    for (int j = 0; j < memberCount; ++j)
    {
      memcpy_0(sData, &buddyData[j].dwSerial, 4u);
      sData = static_cast<char *>(sData) + 4;
      dataSize += 4;

      memcpy_0(sData, &buddyData[j].wMapCode, 2u);
      sData = static_cast<char *>(sData) + 2;
      dataSize += 2;

      memcpy_0(sData, &buddyData[j].byRegionIndex, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;
    }

    pGuild->m_Buddy_List->wDataSize = static_cast<unsigned __int16>(dataSize);
  }

  void SendMsg_VoteStop(CGuild *pGuild, unsigned int dwMatterVoteSynKey)
  {
    char szMsg[4]{};
    *reinterpret_cast<unsigned int *>(szMsg) = dwMatterVoteSynKey;

    unsigned __int8 pbyType[2] = {27, 28};
    for (int j = 0; j < 50; ++j)
    {
      _guild_member_info *member = &pGuild->m_MemberData[j];
      if (member->IsFill() && member->pPlayer)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(member->pPlayer->m_ObjID.m_wIndex, pbyType, szMsg, 4u);
      }
    }
  }

  void SendMsg_VoteProcessInform_Continue(CGuild *pGuild, _guild_member_info *pMem)
  {
    if (!pGuild->m_bNowProcessSgtMter
      || !pMem->pPlayer
      || (pGuild->m_SuggestedMatter.byMatterType == 5 && pMem->byClassInGuild != 2))
    {
      return;
    }

    int loginSeniorNum = 0;
    for (int j = 0; j < pGuild->m_SuggestedMatter.nTotal_VotableMemNum; ++j)
    {
      if (pGuild->m_SuggestedMatter.VotableMem[j]->pPlayer)
      {
        ++loginSeniorNum;
      }
    }

    _guild_vote_process_inform_zocl msg{};
    msg.bStart = false;
    msg.byMatterType = pGuild->m_SuggestedMatter.byMatterType;
    msg.dwMatterDst = pGuild->m_SuggestedMatter.dwMatterDst;
    msg.dwMatterObj1 = pGuild->m_SuggestedMatter.dwMatterObj1;
    msg.dwMatterObj2 = pGuild->m_SuggestedMatter.dwMatterObj2;
    msg.dwMatterObj3 = pGuild->m_SuggestedMatter.dwMatterObj3;
    msg.dwMatterVoteSynKey = pGuild->m_SuggestedMatter.dwMatterVoteSynKey;
    msg.dwSuggesterSerial = pGuild->m_dwSuggesterSerial;
    msg.byTotalSeniorNum = static_cast<unsigned __int8>(pGuild->m_SuggestedMatter.nTotal_VotableMemNum);
    msg.byLoginSeniorNum = static_cast<unsigned __int8>(loginSeniorNum);
    msg.bActed = pMem->bVote;
    msg.byApprPoint = pGuild->m_SuggestedMatter.byVoteState[0];
    msg.byOppoPoint = pGuild->m_SuggestedMatter.byVoteState[1];
    msg.byCommentLen = static_cast<unsigned __int8>(strlen_0(pGuild->m_SuggestedMatter.wszComment));
    strcpy_0(msg.wszComment, pGuild->m_SuggestedMatter.wszComment);

    if (msg.byMatterType == 4)
    {
      strcpy_0(msg.wszDestGuildName, pGuild->m_GuildBattleSugestMatter.pkDest->m_wszName);
      msg.byDestGuildGrade = pGuild->m_GuildBattleSugestMatter.pkDest->m_byGrade;
      msg.byDestGuildRace = pGuild->m_GuildBattleSugestMatter.pkDest->m_byRace;
    }
    else if (msg.byMatterType == 5)
    {
      strcpy_0(msg.wszDestGuildName, pGuild->m_GuildBattleSugestMatter.pkSrc->m_wszName);
      msg.byDestGuildGrade = pGuild->m_GuildBattleSugestMatter.pkSrc->m_byGrade;
      msg.byDestGuildRace = pGuild->m_GuildBattleSugestMatter.pkSrc->m_byRace;
    }

    unsigned __int8 pbyType[2] = {27, 24};
    const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
      pMem->pPlayer->m_ObjID.m_wIndex,
      pbyType,
      reinterpret_cast<char *>(&msg.bStart),
      nLen);
  }
}

_guild_master_info::_guild_master_info()
{
  init();
}

void _guild_master_info::init()
{
  dwSerial = static_cast<unsigned int>(-1);
  byPrevGrade = 0;
  pMember = nullptr;
}

bool _guild_master_info::IsFill()
{
  return dwSerial != static_cast<unsigned int>(-1);
}

void CGuild::Init(unsigned int index)
{
  m_nIndex = static_cast<int>(index);
}

bool CGuild::IsFill()
{
  return m_dwSerial != static_cast<unsigned int>(-1);
}

void CGuild::ReleaseTemp()
{
  m_bDBWait = false;
}

void CGuild::EstGuild(
  unsigned int dwSerial,
  char *pwszName,
  unsigned __int8 byRace,
  int nMemberNum,
  _guild_member_info *pEstMember)
{
  m_dwSerial = dwSerial;
  strcpy_0(m_wszName, pwszName);
  W2M(m_wszName, m_aszName, 0x11u);

  s_MgrHistory.GetNewFileName(dwSerial, m_szHistoryFileName);

  m_nApplierNum = 0;
  m_nMemberNum = nMemberNum;
  memcpy_0(m_MemberData, pEstMember, sizeof(_guild_member_info) * nMemberNum);
  m_bDBWait = false;
  m_bIOWait = false;
  m_bRankWait = false;
  m_byMoneyOutputKind = 0;
  m_dwEmblemBack = static_cast<unsigned int>(-1);
  m_dwEmblemMark = static_cast<unsigned int>(-1);
  m_dTotalDalant = 0.0;
  m_dTotalGold = 0.0;
  m_nIOMoneyHistoryNum = 0;
  m_byGrade = 1;
  m_byRace = byRace;
  m_MasterData.init();

  SortRankInGuild(this);
  m_bPossibleElectMaster = true;

  MakeDownMemberPacket();
  MakeDownApplierPacket();
  MakeQueryInfoPacket();
  MakeMoneyIOPacket();
  BuildBuddyPacket(this);

  m_dwLastLoopTime = GetLoopTime();

  char *pwszMasterName = nullptr;
  if (m_MasterData.pMember)
  {
    pwszMasterName = m_MasterData.pMember->wszName;
  }
  else
  {
    CNationSettingManager *nationMgr = CTSingleton<CNationSettingManager>::Instance();
    pwszMasterName = nationMgr->GetNoneString();
  }

  s_GuildList.AddList(m_byRace, m_byGrade, m_wszName, pwszMasterName);
  s_MgrHistory.start_guild(this, m_szHistoryFileName);
}

void CGuild::ClearVote()
{
  if (m_bNowProcessSgtMter)
  {
    if (m_SuggestedMatter.byMatterType == 4)
    {
      m_GuildBattleSugestMatter.Clear();
    }
    else if (m_SuggestedMatter.byMatterType == 5)
    {
      m_GuildBattleSugestMatter.pkSrc->m_GuildBattleSugestMatter.Clear();
      m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
      m_GuildBattleSugestMatter.pkSrc->SendMsg_ApplyGuildBattleResultInform(0xACu, m_wszName);
      m_GuildBattleSugestMatter.Clear();
    }

    SendMsg_VoteComplete(false);
    InitVote();
  }
}

void CGuild::StartRankJob()
{
  m_bRankWait = true;
}

void CGuild::EndRankJob()
{
  m_bRankWait = false;
}

void CGuild::Loop(bool bChangeDay)
{
  if (bChangeDay && !m_bPossibleElectMaster && m_MasterData.IsFill() && !m_MasterData.pMember->pPlayer)
  {
    PushDQSGuildMasterLastConnn();
  }

  if (GetLoopTime() - m_dwLastLoopTime <= 0x3E8)
  {
    return;
  }

  m_dwLastLoopTime = GetLoopTime();
  if (!m_bNowProcessSgtMter)
  {
    return;
  }

  bool votePass = false;
  if (m_SuggestedMatter.nTotal_VotableMemNum > 0)
  {
    votePass = static_cast<float>(m_SuggestedMatter.byVoteState[0])
             / static_cast<float>(m_SuggestedMatter.nTotal_VotableMemNum)
             > 0.5f;
    if (static_cast<float>(m_SuggestedMatter.byVoteState[1]) / static_cast<float>(m_SuggestedMatter.nTotal_VotableMemNum)
        >= 0.5f)
    {
      if (m_SuggestedMatter.byMatterType == 4)
      {
        m_GuildBattleSugestMatter.Clear();
      }
      else if (m_SuggestedMatter.byMatterType == 5)
      {
        m_GuildBattleSugestMatter.pkSrc->m_GuildBattleSugestMatter.Clear();
        m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
        m_GuildBattleSugestMatter.pkSrc->SendMsg_ApplyGuildBattleResultInform(0xAAu, m_wszName);
        m_GuildBattleSugestMatter.Clear();
      }
      SendMsg_VoteComplete(false);
      InitVote();
      return;
    }
  }

  if (!votePass && GetLoopTime() - m_SuggestedMatter.dwStartTime > 0x6DDD00)
  {
    if (m_SuggestedMatter.byVoteState[0] > static_cast<int>(m_SuggestedMatter.byVoteState[1]))
    {
      votePass = true;
    }

    if (!votePass)
    {
      if (m_SuggestedMatter.byMatterType == 4)
      {
        m_GuildBattleSugestMatter.Clear();
      }
      else if (m_SuggestedMatter.byMatterType == 5)
      {
        m_GuildBattleSugestMatter.pkSrc->m_GuildBattleSugestMatter.Clear();
        m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
        m_GuildBattleSugestMatter.pkSrc->SendMsg_ApplyGuildBattleResultInform(0xAAu, m_wszName);
        m_GuildBattleSugestMatter.Clear();
      }
      SendMsg_VoteComplete(false);
      InitVote();
      return;
    }
  }

  if (!votePass)
  {
    return;
  }

  SendMsg_VoteComplete(true);
  const unsigned __int8 matterType = m_SuggestedMatter.byMatterType;
  if (matterType == 5)
  {
    unsigned __int8 result = CGuildBattleController::Instance()->Add(
      m_GuildBattleSugestMatter.pkSrc,
      m_GuildBattleSugestMatter.pkDest,
      m_SuggestedMatter.dwMatterObj1,
      m_SuggestedMatter.dwMatterObj2,
      m_SuggestedMatter.dwMatterObj3);
    if (result)
    {
      if (result == 112)
      {
        result = 119;
      }
      m_GuildBattleSugestMatter.pkSrc->m_GuildBattleSugestMatter.Clear();
      m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
    }
    else
    {
      PushDQSDestGuildOutputGuildBattleCost();
    }

    m_GuildBattleSugestMatter.pkSrc->SendMsg_ApplyGuildBattleResultInform(result, m_GuildBattleSugestMatter.pkDest->m_wszName);
    m_GuildBattleSugestMatter.Clear();
  }
  else if (matterType == 6)
  {
    _guild_member_info *newMaster = GetMemberFromSerial(m_SuggestedMatter.dwMatterDst);
    DB_Update_GuildMaster(newMaster);
  }
  InitVote();
}

void CGuild::PushDQSGuildMasterLastConnn()
{
  _qry_case_select_guild_master_lastconn query{};
  query.dwSerial = m_MasterData.dwSerial;
  query.dwGuildIndex = m_nIndex;
  query.dwGuildSerial = m_dwSerial;
  query.dwLimitConnTime = GetConnectTime_AddBySec(-1814400);

  g_Main.PushDQSData(
    0xFFFFFFFF,
    nullptr,
    0x90u,
    reinterpret_cast<char *>(&query),
    static_cast<int>(sizeof(query)));
}

void CGuild::SendMsg_DownPacket(unsigned __int8 bDowntype, _guild_member_info *pMem)
{
  if (!pMem->pPlayer)
  {
    return;
  }

  m_DownPacket_Member->byDownType = bDowntype;

  int guildRoomRestTime = 0;
  unsigned __int8 guildRoomType = 2;

  CGuildRoomSystem *guildRoomSystem = CGuildRoomSystem::GetInstance();
  guildRoomType = guildRoomSystem->GetRoomType(m_dwSerial);
  guildRoomSystem->GetRestTime(m_dwSerial, &guildRoomRestTime);

  m_DownPacket_Member->byGuildRoomType = guildRoomType;
  m_DownPacket_Member->GuildRoomRestTime = guildRoomRestTime;

  unsigned __int8 memberType[2] = {27, 3};
  const unsigned __int16 memberLen =
    static_cast<unsigned __int16>(sizeof(*m_DownPacket_Member) - sizeof(m_DownPacket_Member->sData) + m_DownPacket_Member->wDataSize);
  g_Network.m_pProcess[0]->LoadSendMsg(
    pMem->pPlayer->m_ObjID.m_wIndex,
    memberType,
    reinterpret_cast<char *>(m_DownPacket_Member),
    memberLen);

  if (pMem->byClassInGuild == 1 || pMem->byClassInGuild == 2)
  {
    unsigned __int8 applierType[2] = {27, 4};
    const unsigned __int16 applierLen =
      static_cast<unsigned __int16>(sizeof(*m_DownPacket_Applier) - sizeof(m_DownPacket_Applier->sData) + m_DownPacket_Applier->wDataSize);
    g_Network.m_pProcess[0]->LoadSendMsg(
      pMem->pPlayer->m_ObjID.m_wIndex,
      applierType,
      reinterpret_cast<char *>(m_DownPacket_Applier),
      applierLen);
  }

  if (m_bNowProcessSgtMter)
  {
    SendMsg_VoteProcessInform_Continue(this, pMem);
  }

  unsigned __int8 moneyType[2] = {27, 41};
  const unsigned __int16 moneyLen =
    static_cast<unsigned __int16>(sizeof(*m_MoneyIO_List) - sizeof(m_MoneyIO_List->sData) + m_MoneyIO_List->wDataSize);
  g_Network.m_pProcess[0]->LoadSendMsg(
    pMem->pPlayer->m_ObjID.m_wIndex,
    moneyType,
    reinterpret_cast<char *>(m_MoneyIO_List),
    moneyLen);

  BuildBuddyPacket(this);

  unsigned __int8 buddyType[2] = {27, 43};
  const unsigned __int16 buddyLen =
    static_cast<unsigned __int16>(sizeof(*m_Buddy_List) - sizeof(m_Buddy_List->sData) + m_Buddy_List->wDataSize);
  g_Network.m_pProcess[0]->LoadSendMsg(
    pMem->pPlayer->m_ObjID.m_wIndex,
    buddyType,
    reinterpret_cast<char *>(m_Buddy_List),
    buddyLen);
}

char CGuild::RegSuggestedMatter(
  unsigned int dwSuggesterSerial,
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *pwszComment,
  unsigned int dwMatterObj1,
  unsigned int dwMatterObj2,
  unsigned int dwMatterObj3)
{
  m_dwSuggesterSerial = dwSuggesterSerial;
  m_SuggestedMatter.byMatterType = byMatterType;
  m_SuggestedMatter.dwMatterDst = dwMatterDst;
  m_SuggestedMatter.wszMatterDst[0] = 0;
  m_SuggestedMatter.dwMatterObj1 = dwMatterObj1;
  m_SuggestedMatter.dwMatterObj2 = dwMatterObj2;
  m_SuggestedMatter.dwMatterObj3 = dwMatterObj3;

  if (!byMatterType || byMatterType == 1 || byMatterType == 2 || byMatterType == 6)
  {
    _guild_member_info *matterDstMember = GetMemberFromSerial(dwMatterDst);
    if (matterDstMember)
    {
      strcpy_0(m_SuggestedMatter.wszMatterDst, matterDstMember->wszName);
    }
  }

  memset_0(m_SuggestedMatter.byVoteState, 0, sizeof(m_SuggestedMatter.byVoteState));
  if (*pwszComment)
  {
    strcpy_0(m_SuggestedMatter.wszComment, pwszComment);
  }
  else
  {
    memset_0(m_SuggestedMatter.wszComment, 0, sizeof(m_SuggestedMatter.wszComment));
  }

  m_SuggestedMatter.dwStartTime = GetLoopTime();
  ++m_SuggestedMatter.dwMatterVoteSynKey;
  m_bNowProcessSgtMter = true;

  if (byMatterType != 5)
  {
    ++m_SuggestedMatter.byVoteState[0];
  }

  int votableMemberCount = 0;
  for (int memberIndex = 0; memberIndex < 50; ++memberIndex)
  {
    _guild_member_info *member = &m_MemberData[memberIndex];
    if (member->IsFill())
    {
      member->bVote = false;
      if (byMatterType == 5)
      {
        if (member->byClassInGuild == 2)
        {
          m_SuggestedMatter.VotableMem[votableMemberCount++] = member;
        }
      }
      else if (byMatterType == 6)
      {
        m_SuggestedMatter.VotableMem[votableMemberCount++] = member;
      }
    }
  }

  if (!votableMemberCount)
  {
    return 0;
  }

  if (byMatterType == 4)
  {
    m_GuildBattleSugestMatter.pkSrc = this;
    m_GuildBattleSugestMatter.pkDest = GetGuildDataFromSerial(g_Guild, MAX_GUILD, dwMatterDst);
    m_GuildBattleSugestMatter.eState = _guild_battle_suggest_matter::BATTLE_REQUEST_SUGGEST;
  }
  else if (byMatterType == 5)
  {
    m_GuildBattleSugestMatter.pkSrc = GetGuildDataFromSerial(g_Guild, MAX_GUILD, dwMatterDst);
    m_GuildBattleSugestMatter.pkDest = this;
    m_GuildBattleSugestMatter.eState = _guild_battle_suggest_matter::APPLY_BATTLE_REQUEST_SUGGEST;
  }

  m_SuggestedMatter.nTotal_VotableMemNum = votableMemberCount;
  SendMsg_VoteProcessInform_Start();
  return 1;
}

void CGuild::SendMsg_VoteProcessInform_Start()
{
  if (!m_bNowProcessSgtMter)
  {
    return;
  }

  int loginSeniorCount = 0;
  for (int memberIndex = 0; memberIndex < m_SuggestedMatter.nTotal_VotableMemNum; ++memberIndex)
  {
    if (m_SuggestedMatter.VotableMem[memberIndex]->pPlayer)
    {
      ++loginSeniorCount;
    }
  }

  _guild_vote_process_inform_zocl msg{};
  memset_0(&msg, 0, sizeof(msg));
  msg.bStart = true;
  msg.byMatterType = m_SuggestedMatter.byMatterType;
  msg.dwMatterDst = m_SuggestedMatter.dwMatterDst;
  msg.dwMatterObj1 = m_SuggestedMatter.dwMatterObj1;
  msg.dwMatterObj2 = m_SuggestedMatter.dwMatterObj2;
  msg.dwMatterObj3 = m_SuggestedMatter.dwMatterObj3;
  msg.dwMatterVoteSynKey = m_SuggestedMatter.dwMatterVoteSynKey;
  msg.dwSuggesterSerial = m_dwSuggesterSerial;
  msg.byTotalSeniorNum = static_cast<unsigned __int8>(m_SuggestedMatter.nTotal_VotableMemNum);
  msg.byLoginSeniorNum = static_cast<unsigned __int8>(loginSeniorCount);
  msg.bActed = false;
  msg.byApprPoint = m_SuggestedMatter.byVoteState[0];
  msg.byOppoPoint = m_SuggestedMatter.byVoteState[1];
  msg.byCommentLen = static_cast<unsigned __int8>(strlen_0(m_SuggestedMatter.wszComment));
  strcpy_0(msg.wszComment, m_SuggestedMatter.wszComment);

  if (msg.byMatterType == 4)
  {
    strcpy_0(msg.wszDestGuildName, m_GuildBattleSugestMatter.pkDest->m_wszName);
    msg.byDestGuildGrade = m_GuildBattleSugestMatter.pkDest->m_byGrade;
    msg.byDestGuildRace = m_GuildBattleSugestMatter.pkDest->m_byRace;
  }
  else if (msg.byMatterType == 5)
  {
    strcpy_0(msg.wszDestGuildName, m_GuildBattleSugestMatter.pkSrc->m_wszName);
    msg.byDestGuildGrade = m_GuildBattleSugestMatter.pkSrc->m_byGrade;
    msg.byDestGuildRace = m_GuildBattleSugestMatter.pkSrc->m_byRace;
  }

  unsigned __int8 pbyType[2] = {27, 24};
  for (int memberIndex = 0; memberIndex < 50; ++memberIndex)
  {
    _guild_member_info *member = &m_MemberData[memberIndex];
    if (member->IsFill() && member->pPlayer && (msg.byMatterType != 5 || member->byClassInGuild == 2))
    {
      const unsigned __int16 messageSize = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(
        member->pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg.bStart),
        messageSize);
    }
  }
}

void CGuild::SendMsg_VoteComplete(bool bPass)
{
  #pragma pack(push, 1)
  struct
  {
    unsigned int dwVoteKey;
    unsigned __int8 byVoteState0;
    unsigned __int8 byVoteState1;
    bool bPass;
  } msg{};
  #pragma pack(pop)

  msg.dwVoteKey = m_SuggestedMatter.dwMatterVoteSynKey;
  msg.byVoteState0 = m_SuggestedMatter.byVoteState[0];
  msg.byVoteState1 = m_SuggestedMatter.byVoteState[1];
  msg.bPass = bPass;

  unsigned __int8 pbyType[2] = {27, 29};
  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->pPlayer)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(member->pPlayer->m_ObjID.m_wIndex, pbyType, (char *)&msg, 7u);
    }
  }
}

void CGuild::InitVote()
{
  m_bNowProcessSgtMter = false;
}

char CGuild::LogoffMember(unsigned int dwMemberSerial)
{
  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->dwSerial == dwMemberSerial)
    {
      if (member->byClassInGuild == 2 && m_GuildBattleSugestMatter.eState == _guild_battle_suggest_matter::APPLY_BATTLE_REQUEST_SUGGEST)
      {
        ManageAcceptORRefuseGuildBattle(false);
      }
      member->pPlayer = nullptr;
      return 1;
    }
  }
  return 0;
}

_guild_member_info *CGuild::LoginMember(unsigned int dwMemberSerial, CPlayer *pPtr)
{
  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->dwSerial == dwMemberSerial)
    {
      member->pPlayer = pPtr;
      return member;
    }
  }
  return nullptr;
}

void CGuild::SendMsg_GuildMemberLogoff(unsigned int dwSerial)
{
  char szMsg[32]{};
  unsigned __int8 pbyType[2] = {27, 45};

  *reinterpret_cast<unsigned int *>(szMsg) = dwSerial;

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill())
    {
      if (m_MemberData[j].pPlayer)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
          pbyType,
          szMsg,
          4u);
      }
    }
  }
}

void CGuild::SendMsg_GuildMemberLogin(unsigned int dwSerial, unsigned __int16 wMapCode, unsigned __int16 wRegionIndex)
{
  char szMsg[7]{};
  unsigned __int8 pbyType[2] = {27, 44};

  *reinterpret_cast<unsigned int *>(szMsg) = dwSerial;
  *reinterpret_cast<unsigned __int16 *>(szMsg + 4) = wMapCode;
  szMsg[6] = static_cast<char>(wRegionIndex);

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer && m_MemberData[j].dwSerial != dwSerial)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        szMsg,
        7u);
    }
  }
}

void CGuild::SendMsg_GuildMemberPosInform(
  unsigned int dwSerial,
  unsigned __int16 wMapCode,
  unsigned __int16 wRegionIndex)
{
  char szMsg[7]{};
  unsigned __int8 pbyType[2] = {27, 46};

  *reinterpret_cast<unsigned int *>(szMsg) = dwSerial;
  *reinterpret_cast<unsigned __int16 *>(szMsg + 4) = wMapCode;
  szMsg[6] = static_cast<char>(wRegionIndex);

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill())
    {
      if (m_MemberData[j].pPlayer)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
          pbyType,
          szMsg,
          7u);
      }
    }
  }
}

_guild_applier_info *CGuild::GetApplierFromSerial(unsigned int dwApplierSerial)
{
  for (int j = 0; j < 32; ++j)
  {
    if (m_ApplierData[j].IsFill() && m_ApplierData[j].pPlayer->m_dwObjSerial == dwApplierSerial)
    {
      return &m_ApplierData[j];
    }
  }

  return nullptr;
}

void CGuild::SendMsg_GuildRoomRented(char byRoomType)
{
  char szMsg[1] = {byRoomType};
  unsigned __int8 pbyType[2] = {27, 110};

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        szMsg,
        1u);
    }
  }
}

char CGuild::PopApplier(unsigned int dwApplierSerial, unsigned __int8 byDelCode)
{
  for (int j = 0; j < 32; ++j)
  {
    _guild_applier_info *info = &m_ApplierData[j];
    if (info->IsFill() && info->pPlayer->m_dwObjSerial == dwApplierSerial)
    {
      SendMsg_DelJoinApplier(info, byDelCode);
      info->init();
      --m_nApplierNum;
      MakeDownApplierPacket();
      return 1;
    }
  }
  return 0;
}

char CGuild::PushApplier(CPlayer *pApplier)
{
  if (m_nApplierNum >= 32)
  {
    return 0;
  }
  for (int j = 0; j < 32; ++j)
  {
    _guild_applier_info *info = &m_ApplierData[j];
    if (!info->IsFill())
    {
      info->pPlayer = pApplier;
      info->dwApplyTime = GetLoopTime();
      ++m_nApplierNum;
      SendMsg_AddJoinApplier(info);
      MakeDownApplierPacket();
      return 1;
    }
  }
  return 0;
}

_guild_member_info *CGuild::PushMember(_guild_member_info *pSheet)
{
  if (m_nMemberNum >= 50)
  {
    return nullptr;
  }

  for (int j = 0; j < 50; ++j)
  {
    if (!m_MemberData[j].IsFill())
    {
      memcpy_0(&m_MemberData[j], pSheet, sizeof(m_MemberData[j]));
      m_MemberData[j].byRank = static_cast<unsigned __int8>(m_nMemberNum);
      ++m_nMemberNum;
      MakeDownMemberPacket();
      return &m_MemberData[j];
    }
  }

  return nullptr;
}

void CGuild::SendMsg_GuildJoinAcceptInform(_guild_member_info *p, unsigned int dwAcceptSerial)
{
  #pragma pack(push, 1)
  struct GuildJoinAcceptMsg
  {
    unsigned int dwAcceptSerial;
    unsigned int dwSerial;
    char wszNameAndClassAndLv[19];
    unsigned int dwPvpPoint;
    unsigned __int8 byRank;
  };
  #pragma pack(pop)

  GuildJoinAcceptMsg msg{};
  msg.dwAcceptSerial = dwAcceptSerial;
  msg.dwSerial = p->dwSerial;
  strcpy_0(msg.wszNameAndClassAndLv, p->wszName);
  msg.wszNameAndClassAndLv[17] = static_cast<char>(p->byClassInGuild);
  msg.wszNameAndClassAndLv[18] = static_cast<char>(p->byLv);
  msg.dwPvpPoint = p->dwPvpPoint;
  msg.byRank = p->byRank;

  unsigned __int8 pbyType[2] = {27, 15};

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer)
    {
      if (p == &m_MemberData[j])
      {
        SendMsg_DownPacket(2u, &m_MemberData[j]);
      }
      else
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
          pbyType,
          reinterpret_cast<char *>(&msg),
          0x20u);
      }
    }
  }
}

void CGuild::SendMsg_LeaveMember(unsigned int dwMemberSerial, char bSelf, char bPunish)
{
  #pragma pack(push, 1)
  struct GuildLeaveMsg
  {
    char bSelf;
    char bPunish;
    unsigned int dwMemberSerial;
  };
  #pragma pack(pop)

  GuildLeaveMsg msg{};
  msg.bSelf = bSelf;
  msg.bPunish = bPunish;
  msg.dwMemberSerial = dwMemberSerial;

  unsigned __int8 pbyType[2] = {27, 18};
  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill()
      && m_MemberData[j].pPlayer
      && dwMemberSerial != m_MemberData[j].pPlayer->m_dwObjSerial)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        6u);
    }
  }
}

char CGuild::PopMember(unsigned int dwMemberSerial)
{
  int memberIndex = -1;
  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].dwSerial == dwMemberSerial)
    {
      memberIndex = j;
      break;
    }
  }

  if (memberIndex < 0)
  {
    return 0;
  }

  for (int k = 0; k < 3; ++k)
  {
    if (m_pGuildCommittee[k] && m_pGuildCommittee[k]->dwSerial == dwMemberSerial)
    {
      m_pGuildCommittee[k] = nullptr;
      break;
    }
  }

  m_MemberData[memberIndex].init();
  --m_nMemberNum;
  MakeDownMemberPacket();

  if (m_bNowProcessSgtMter)
  {
    if (m_dwSuggesterSerial == dwMemberSerial)
    {
      InitVote();
      SendMsg_VoteStop(this, m_SuggestedMatter.dwMatterVoteSynKey);
    }

    if (m_SuggestedMatter.byMatterType == 6 && dwMemberSerial == m_SuggestedMatter.dwMatterDst)
    {
      InitVote();
      SendMsg_VoteStop(this, dwMemberSerial);
    }
  }

  if (m_MasterData.dwSerial == dwMemberSerial && m_MasterData.IsFill())
  {
    DB_Update_GuildMaster(nullptr);
  }

  return 1;
}

unsigned __int8 CGuild::GetGrade()
{
  return m_byGrade;
}

void CGuild::UpdateGrade(unsigned __int8 byGrade)
{
  if (m_MasterData.pMember && m_MasterData.pMember->pPlayer)
  {
    _guild_member_info *master = m_MasterData.pMember;
    CGuildMasterEffect::GetInstance()->change_player(master->pPlayer, m_byGrade, byGrade);
  }

  m_byGrade = byGrade;

  char msg[0x19];
  *reinterpret_cast<unsigned int *>(msg) = m_dwSerial;
  msg[4] = static_cast<char>(m_byGrade);
  *reinterpret_cast<unsigned int *>(msg + 5) = m_dwEmblemBack;
  *reinterpret_cast<unsigned int *>(msg + 9) = m_dwEmblemMark;

  unsigned __int8 pbyType[2] = {27, 40};
  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        msg,
        0x19u);
    }
  }

  CGuild::s_GuildList.SetGrade(m_byRace, m_wszName, m_byGrade);
}

void CGuild::UpdateEmblem(unsigned int dwEmblemBack, unsigned int dwEmblemMark)
{
  m_dwEmblemBack = dwEmblemBack;
  m_dwEmblemMark = dwEmblemMark;
  MakeQueryInfoPacket();
  MakeDownMemberPacket();
  m_byMoneyOutputKind = 0;
  SendMsg_GuildInfoUpdateInform();
}

void CGuild::SendMsg_AddJoinApplier(_guild_applier_info *p)
{
  #pragma pack(push, 1)
  struct AddJoinApplierMsg
  {
    unsigned int dwSerial;
    char wszName[18];
    int nPvpPoint;
    unsigned int dwApplyTime;
  };
  #pragma pack(pop)

  AddJoinApplierMsg msg{};
  msg.dwSerial = p->pPlayer->m_dwObjSerial;
  const char *charName = p->pPlayer->m_Param.GetCharNameW();
  strcpy_0(msg.wszName, charName);
  msg.wszName[17] = static_cast<char>(p->pPlayer->GetLevel());
  msg.nPvpPoint = static_cast<int>(p->pPlayer->m_Param.GetPvPPoint());
  msg.dwApplyTime = p->dwApplyTime;

  unsigned __int8 pbyType[2] = {27, 8};

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer
        && (m_MemberData[j].byClassInGuild == 1 || m_MemberData[j].byClassInGuild == 2))
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        0x1Eu);
    }
  }
}

void CGuild::SendMsg_DelJoinApplier(_guild_applier_info *p, unsigned __int8 byDelCode)
{
  #pragma pack(push, 1)
  struct DelJoinApplierMsg
  {
    unsigned int dwSerial;
    unsigned __int8 byDelCode;
  };
  #pragma pack(pop)

  DelJoinApplierMsg msg{};
  msg.dwSerial = p->pPlayer->m_dwObjSerial;
  msg.byDelCode = byDelCode;

  unsigned __int8 pbyType[2] = {27, 9};

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer
        && (m_MemberData[j].byClassInGuild == 1 || m_MemberData[j].byClassInGuild == 2))
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        5u);
    }
  }
}

void CGuild::MakeDownApplierPacket()
{
  m_DownPacket_Applier->wDataSize = 0;

  int dataSize = 0;
  void *sData = m_DownPacket_Applier->sData;
  memcpy_0(sData, &m_nApplierNum, 1u);
  sData = static_cast<char *>(sData) + 1;
  ++dataSize;

  for (int j = 0; j < 32; ++j)
  {
    _guild_applier_info *info = &m_ApplierData[j];
    if (info->IsFill())
    {
      CPlayer *player = info->pPlayer;
      const char *charName = player->m_Param.GetCharNameW();

      char nameLen = static_cast<char>(strlen_0(charName));
      memcpy_0(sData, &nameLen, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;

      memcpy_0(sData, charName, nameLen);
      sData = static_cast<char *>(sData) + nameLen;
      dataSize += nameLen;

      memcpy_0(sData, &player->m_dwObjSerial, 4u);
      sData = static_cast<char *>(sData) + 4;
      dataSize += 4;

      unsigned __int8 level = player->GetLevel();
      memcpy_0(sData, &level, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;

      int pvpPoint = static_cast<int>(player->m_Param.GetPvPPoint());
      memcpy_0(sData, &pvpPoint, 4u);
      sData = static_cast<char *>(sData) + 4;
      dataSize += 4;

      memcpy_0(sData, &info->dwApplyTime, 4u);
      sData = static_cast<char *>(sData) + 4;
      dataSize += 4;
    }
  }

  m_DownPacket_Applier->wDataSize = dataSize;
}

void CGuild::MakeDownMemberPacket()
{
  m_DownPacket_Member->dwGuildSerial = m_dwSerial;
  m_DownPacket_Member->byGuildGrade = m_byGrade;
  m_DownPacket_Member->dwEmblemBack = m_dwEmblemBack;
  m_DownPacket_Member->dwEmblemMark = m_dwEmblemMark;
  m_DownPacket_Member->dDalant = m_dTotalDalant;
  m_DownPacket_Member->dGold = m_dTotalGold;
  m_DownPacket_Member->byCurTax = static_cast<unsigned __int8>(-1);

  CUnmannedTraderTaxRateManager *taxMgr = CUnmannedTraderTaxRateManager::Instance();
  if (taxMgr->IsOwnerGuild(m_byRace, m_dwSerial))
  {
    float taxRate = taxMgr->GetTaxRate(m_byRace);
    m_DownPacket_Member->byCurTax = static_cast<unsigned __int8>(taxRate * 100.0f);
  }

  m_DownPacket_Member->dwTotWin = m_dwGuildBattleTotWin;
  m_DownPacket_Member->dwTotDraw = m_dwGuildBattleTotDraw;
  m_DownPacket_Member->dwTotLose = m_dwGuildBattleTotLose;
  m_DownPacket_Member->bPossibleElectMaster = m_bPossibleElectMaster;
  m_DownPacket_Member->wDataSize = 0;

  int dataSize = 0;
  void *sData = m_DownPacket_Member->sData;

  unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(m_wszName));
  memcpy_0(sData, &nameLen, 1u);
  sData = static_cast<char *>(sData) + 1;
  ++dataSize;

  memcpy_0(sData, m_wszName, nameLen);
  sData = static_cast<char *>(sData) + nameLen;
  dataSize += nameLen;

  memcpy_0(sData, &m_nMemberNum, 1u);
  sData = static_cast<char *>(sData) + 1;
  ++dataSize;

  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill())
    {
      nameLen = static_cast<unsigned __int8>(strlen_0(member->wszName));
      memcpy_0(sData, &nameLen, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;

      memcpy_0(sData, member->wszName, nameLen);
      sData = static_cast<char *>(sData) + nameLen;
      dataSize += nameLen;

      memcpy_0(sData, member, 4u);
      sData = static_cast<char *>(sData) + 4;
      dataSize += 4;

      memcpy_0(sData, &member->byLv, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;

      memcpy_0(sData, &member->dwPvpPoint, 4u);
      sData = static_cast<char *>(sData) + 4;
      dataSize += 4;

      memcpy_0(sData, &member->byClassInGuild, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;

      memcpy_0(sData, &member->byRank, 1u);
      sData = static_cast<char *>(sData) + 1;
      ++dataSize;
    }
  }

  m_DownPacket_Member->wDataSize = dataSize;
}

void CGuild::MakeQueryInfoPacket()
{
  m_QueryPacket_Info->dwGuildSerial = m_dwSerial;
  m_QueryPacket_Info->byGrade = m_byGrade;
  m_QueryPacket_Info->dwEmblemBack = m_dwEmblemBack;
  m_QueryPacket_Info->dwEmblemMark = m_dwEmblemMark;
  strcpy_0(m_QueryPacket_Info->wszGuildName, m_wszName);
  m_QueryPacket_Info->dwTotWin = m_dwGuildBattleTotWin;
  m_QueryPacket_Info->dwTotDraw = m_dwGuildBattleTotDraw;
  m_QueryPacket_Info->dwTotLose = m_dwGuildBattleTotLose;
  m_QueryPacket_Info->byCurTax = static_cast<unsigned __int8>(-1);

  CUnmannedTraderTaxRateManager *taxMgr = CUnmannedTraderTaxRateManager::Instance();
  if (taxMgr->IsOwnerGuild(m_byRace, m_dwSerial))
  {
    m_QueryPacket_Info->byCurTax = static_cast<unsigned __int8>(taxMgr->GetTaxRate(m_byRace) * 100.0f);
  }
}

void CGuild::SendMsg_GuildInfoUpdateInform()
{
  #pragma pack(push, 1)
  struct GuildInfoUpdateMsg
  {
    unsigned int dwSerial;
    unsigned __int8 byGrade;
    unsigned int dwEmblemBack;
    unsigned int dwEmblemMark;
    unsigned int dwTotWin;
    unsigned int dwTotDraw;
    unsigned int dwTotLose;
  };
  #pragma pack(pop)

  GuildInfoUpdateMsg msg{};
  msg.dwSerial = m_dwSerial;
  msg.byGrade = m_byGrade;
  msg.dwEmblemBack = m_dwEmblemBack;
  msg.dwEmblemMark = m_dwEmblemMark;
  msg.dwTotWin = m_dwGuildBattleTotWin;
  msg.dwTotDraw = m_dwGuildBattleTotDraw;
  msg.dwTotLose = m_dwGuildBattleTotLose;

  unsigned __int8 pbyType[2] = {27, 40};
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    CPlayer *player = &g_Player[j];
    if (player->m_bLive)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 0x19u);
    }
  }
}

void CGuild::SendMsg_GuildDisjointInform()
{
  if (m_nApplierNum <= 0)
  {
    return;
  }

  char szMsg[4]{};
  *reinterpret_cast<unsigned int *>(szMsg) = m_dwSerial;

  unsigned __int8 pbyType[2] = {27, 42};
  for (int j = 0; j < 32; ++j)
  {
    _guild_applier_info *applier = &m_ApplierData[j];
    if (applier->IsFill())
    {
      g_Network.m_pProcess[0]->LoadSendMsg(applier->pPlayer->m_ObjID.m_wIndex, pbyType, szMsg, 4u);
      applier->pPlayer->m_Param.m_pApplyGuild = nullptr;
    }
  }
}

void CGuild::RefreshGuildMemberData(_guild_member_refresh_data *pRefreshMember)
{
  for (int j = 0; j < m_nMemberNum; ++j)
  {
    for (int k = 0; k < pRefreshMember->wMemberCount; ++k)
    {
      if (m_MemberData[j].dwSerial == pRefreshMember->rMemberData[k].dwSerial)
      {
        m_MemberData[j].byRank = static_cast<unsigned __int8>(k);
        m_MemberData[j].byLv = pRefreshMember->rMemberData[k].byLv;
        m_MemberData[j].dwPvpPoint = pRefreshMember->rMemberData[k].dwPvpPoint;
      }
    }
  }

  MakeDownMemberPacket();
}

void CGuild::SendMsg_AlterMemberState()
{
  _guild_alter_member_state_inform_zocl msg{};
  int count = 0;

  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill())
    {
      msg.MemberList[count].dwMemberSerial = member->dwSerial;
      msg.MemberList[count].byLv = member->byLv;
      msg.MemberList[count].dwPvpPoint = member->dwPvpPoint;
      ++count;
    }
  }

  msg.byAlterMemberNum = static_cast<unsigned __int8>(count);
  unsigned __int8 pbyType[2] = {27, 31};

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer)
    {
      const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(m_MemberData[j].pPlayer->m_ObjID.m_wIndex, pbyType, (char *)&msg, nLen);
    }
  }
}

void CGuild::SendMsg_AlterMemberGrade()
{
  _guild_alter_member_grade_inform_zocl msg{};
  int count = 0;

  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill())
    {
      msg.MemberList[count].dwMemberSerial = member->dwSerial;
      msg.MemberList[count].byRank = member->byRank;
      msg.MemberList[count].byGrade = member->byClassInGuild;
      ++count;
    }
  }

  msg.byAlterMemberNum = static_cast<unsigned __int8>(count);
  unsigned __int8 pbyType[2] = {27, 32};

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer)
    {
      const unsigned __int16 nLen = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(m_MemberData[j].pPlayer->m_ObjID.m_wIndex, pbyType, (char *)&msg, nLen);
    }
  }
}

unsigned __int8 CGuild::ManageProposeGuildBattle(
  unsigned int dwDestGuild,
  unsigned int dwStartTimeIdx,
  unsigned int dwMemberCountIdx,
  unsigned int dwMapIdx)
{
  if (m_dTotalGold < 5000.0)
  {
    return 116;
  }

  unsigned __int8 retCode =
    CGuildBattleController::Instance()->IsAvailableSuggest(this, dwDestGuild, dwStartTimeIdx, dwMemberCountIdx, dwMapIdx);
  if (!retCode)
  {
    CGuild *destGuild = GetGuildDataFromSerial(g_Guild, MAX_GUILD, dwDestGuild);
    if (!destGuild)
    {
      return 111;
    }

    retCode = destGuild->CheckGuildBattleSuggestRequestToDestGuild(
      m_dwSerial,
      dwStartTimeIdx,
      dwMemberCountIdx,
      dwMapIdx);
    if (!retCode)
    {
      m_GuildBattleSugestMatter.pkSrc = this;
      m_GuildBattleSugestMatter.pkDest = destGuild;
      m_GuildBattleSugestMatter.eState = _guild_battle_suggest_matter::BATTLE_REQUEST_SUGGEST;
      m_GuildBattleSugestMatter.dwStartTime = dwStartTimeIdx;
      m_GuildBattleSugestMatter.dwNumber = dwMemberCountIdx;
      m_GuildBattleSugestMatter.dwMapIdx = dwMapIdx;
      PushDQSSourceGuildOutputGuildBattleCost();
    }
  }
  return retCode;
}

unsigned __int8 CGuild::ManageExpulseMember(unsigned int dwMemberSerial)
{
  _guild_member_info *member = GetMemberFromSerial(dwMemberSerial);
  if (!member)
  {
    return static_cast<unsigned __int8>(-54);
  }

  if (member->pPlayer)
  {
    member->pPlayer->m_Param.m_bGuildLock = true;
  }

  _qry_case_forceleave qry{};
  qry.in_leaverserial = dwMemberSerial;
  qry.in_guildIndex = m_nIndex;
  qry.in_guildserial = m_dwSerial;
  qry.in_seniornum = static_cast<int>(GetMemberNum() - 1);
  qry.in_apprnum = static_cast<unsigned __int8>(GetMemberNum() - 1);
  qry.in_MemberNum = static_cast<int>(GetMemberNum() - 1);
  qry.in_bPunish = false;

  const int nSize = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x12u, reinterpret_cast<char *>(&qry), nSize);
  return 0;
}

unsigned __int8 CGuild::ManagePopGuildMoney(unsigned int dwDest, unsigned int dwDalant, unsigned int dwGold)
{
  _guild_member_info *member = GetMemberFromSerial(dwDest);
  if (!member || !member->pPlayer)
  {
    return static_cast<unsigned __int8>(-54);
  }

  if (dwDalant > 0xB2D05E00u || dwGold > 0xB2D05E00u)
  {
    return 101;
  }

  if (static_cast<long double>(dwDalant) > GetTotalDalant()
    || static_cast<long double>(dwGold) > GetTotalGold())
  {
    return 102;
  }

  const unsigned int curDalant = member->pPlayer->m_Param.GetDalant();
  if (!CanAddMoneyForMaxLimMoney(dwDalant, curDalant))
  {
    return 105;
  }

  const unsigned int curGold = member->pPlayer->m_Param.GetGold();
  if (!CanAddMoneyForMaxLimGold(dwGold, curGold))
  {
    return 105;
  }

  _qry_case_outputgmoney qry{};
  qry.in_poperserial = dwDest;
  qry.tmp_guildindex = m_nIndex;
  qry.in_guildserial = m_dwSerial;
  qry.dwSubDalant = dwDalant;
  qry.dwSubGold = dwGold;
  qry.out_totalgold = 0.0;
  qry.out_totaldalant = 0.0;
  qry.in_date[0] = GetCurrentMonth();
  qry.in_date[1] = GetCurrentDay();
  qry.in_date[2] = GetCurrentHour();
  qry.in_date[3] = GetCurrentMin();
  strcpy_0(qry.in_w_popername, member->pPlayer->m_Param.GetCharNameW());

  const int nSize = static_cast<int>(qry.size());
  if (!g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x14u, reinterpret_cast<char *>(&qry), nSize))
  {
    return 6;
  }

  m_bIOWait = true;
  return 0;
}

unsigned __int8 CGuild::ManageBuyGuildEmblem(unsigned int dwBuyer, unsigned int dwBack, unsigned int dwMark)
{
  if (GetGrade() < 2u)
  {
    return static_cast<unsigned __int8>(-53);
  }

  const long double totalDalant = GetTotalDalant();
  const double emblemDalant = static_cast<double>(static_cast<int>(g_Main.m_GuildCreateEventInfo.GetEmblemDalant()));
  if (emblemDalant > totalDalant)
  {
    return 102;
  }

  for (int j = 0; j < MAX_GUILD; ++j)
  {
    if ((g_Guild[j].IsFill() || g_Guild[j].m_bDBWait)
      && dwBack == g_Guild[j].m_dwEmblemBack
      && dwMark == g_Guild[j].m_dwEmblemMark)
    {
      return 65;
    }
  }

  _qry_case_buyemblem qry{};
  qry.in_guildserial = m_dwSerial;
  qry.in_emblemback = dwBack;
  qry.in_emblemmark = dwMark;
  qry.in_emblemdlant = -static_cast<int>(g_Main.m_GuildCreateEventInfo.GetEmblemDalant());
  qry.tmp_guildindex = m_nIndex;
  qry.in_suggestorSerial = dwBuyer;
  strcpy_0(qry.tmp_w_suggestorname, kUnknownGuildEmblemName);
  qry.in_date[0] = GetCurrentMonth();
  qry.in_date[1] = GetCurrentDay();
  qry.in_date[2] = GetCurrentHour();
  qry.in_date[3] = GetCurrentMin();
  qry.byProcRet = 0;

  const int nSize = static_cast<int>(qry.size());
  if (!g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x15u, reinterpret_cast<char *>(&qry), nSize))
  {
    return 6;
  }

  m_bIOWait = true;
  return 0;
}

unsigned __int8 CGuild::ManageGuildCommittee(unsigned int dwDestSerial, bool bAppoint)
{
  _guild_member_info *member = GetMemberFromSerial(dwDestSerial);
  if (!member)
  {
    return static_cast<unsigned __int8>(-54);
  }

  if (member->pPlayer)
  {
    member->pPlayer->m_Param.m_bGuildLock = true;
  }

  if (bAppoint)
  {
    if (member->byClassInGuild == 1)
    {
      return static_cast<unsigned __int8>(-82);
    }
    if (member->byClassInGuild == 2)
    {
      return static_cast<unsigned __int8>(-81);
    }

    int emptyIndex = -1;
    for (int j = 0; j < 3; ++j)
    {
      if (!m_pGuildCommittee[j])
      {
        emptyIndex = j;
        break;
      }
    }
    if (emptyIndex == -1)
    {
      return static_cast<unsigned __int8>(-80);
    }

    _qry_case_update_user_guild_data qry{};
    qry.dwAvatorSerial = dwDestSerial;
    qry.dwGuildIndex = m_nIndex;
    qry.dwGuildSerial = m_dwSerial;
    qry.byGrade = 1;
    const int nSize = static_cast<int>(qry.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x8Fu, reinterpret_cast<char *>(&qry), nSize);
  }
  else
  {
    if (member->byClassInGuild != 1)
    {
      return static_cast<unsigned __int8>(-79);
    }

    int foundIndex = -1;
    for (int k = 0; k < 3; ++k)
    {
      if (m_pGuildCommittee[k] && m_pGuildCommittee[k]->dwSerial == dwDestSerial)
      {
        foundIndex = k;
        break;
      }
    }
    if (foundIndex == -1)
    {
      return static_cast<unsigned __int8>(-79);
    }

    _qry_case_update_user_guild_data qry{};
    qry.dwAvatorSerial = dwDestSerial;
    qry.dwGuildIndex = m_nIndex;
    qry.dwGuildSerial = m_dwSerial;
    qry.byGrade = 0;
    const int nSize = static_cast<int>(qry.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x8Fu, reinterpret_cast<char *>(&qry), nSize);
  }

  return 0;
}

unsigned __int8 CGuild::ManageAcceptORRefuseGuildBattle(bool bAccept)
{
  unsigned __int8 retCode = 0;

  if (m_GuildBattleSugestMatter.eState != _guild_battle_suggest_matter::APPLY_BATTLE_REQUEST_SUGGEST)
  {
    return 111;
  }

  if (bAccept)
  {
    retCode = CGuildBattleController::Instance()->Add(
      m_GuildBattleSugestMatter.pkSrc,
      m_GuildBattleSugestMatter.pkDest,
      m_GuildBattleSugestMatter.dwStartTime,
      m_GuildBattleSugestMatter.dwNumber,
      m_GuildBattleSugestMatter.dwMapIdx);

    if (retCode)
    {
      if (retCode == 112)
      {
        retCode = 119;
      }
      m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
      m_GuildBattleSugestMatter.pkSrc->SendMsg_ApplyGuildBattleResultInform(retCode, m_wszName);
    }
    else
    {
      PushDQSDestGuildOutputGuildBattleCost();
      CHonorGuild *honorGuild = CHonorGuild::Instance();
      if (honorGuild->CheckHonorGuild(m_byRace, m_dwSerial))
      {
        CMoneySupplyMgr::Instance()->UpdateHonorGuildMoneyData(1u, m_byRace, 0x1388u);
      }
      CGuild *srcGuild = m_GuildBattleSugestMatter.pkSrc;
      CHonorGuild *honorGuildSrc = CHonorGuild::Instance();
      if (honorGuildSrc->CheckHonorGuild(srcGuild->m_byRace, srcGuild->m_dwSerial))
      {
        CMoneySupplyMgr::Instance()->UpdateHonorGuildMoneyData(1u, srcGuild->m_byRace, 0x1388u);
      }
    }
  }
  else
  {
    m_GuildBattleSugestMatter.pkSrc->SendMsg_GuildBattleRefused(m_wszName);
    m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
  }

  m_GuildBattleSugestMatter.pkSrc->m_GuildBattleSugestMatter.Clear();
  m_GuildBattleSugestMatter.Clear();
  return retCode;
}

void CGuild::SendMsg_ApplyGuildBattleResultInform(char byRet, char *wszDestGuildName)
{
  #pragma pack(push, 1)
  struct ApplyGuildBattleResultMsg
  {
    char byRet;
    char wszDestGuildName[17];
  };
  #pragma pack(pop)

  ApplyGuildBattleResultMsg msg{};
  msg.byRet = byRet;
  strcpy_0(msg.wszDestGuildName, wszDestGuildName);

  unsigned __int8 pbyType[2] = {27, 54};

  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->pPlayer && (!byRet || member->byClassInGuild == 2))
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        member->pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        0x12u);
    }
  }
}

void CGuild::SendMsg_GuildBattleRefused(char *pwszName)
{
  char destination[44]{};
  unsigned __int8 pbyType[2] = {27, 93};

  strcpy_0(destination, pwszName);

  if (m_MasterData.pMember)
  {
    CPlayer *player = m_MasterData.pMember->pPlayer;
    if (player)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, pbyType, destination, 0x11u);
    }
  }
}

void CGuild::SendMsg_GuildBattleSuggestResult(unsigned __int8 byRet, char *wszDestGuildName)
{
  _guild_battle_suggest_request_result_zocl msg{};
  msg.byRet = byRet;
  if (!byRet && wszDestGuildName)
  {
    strcpy_0(msg.wszDestGuildName, wszDestGuildName);
  }

  unsigned __int8 type[2] = {27, 53};
  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->pPlayer && member->byClassInGuild == 2)
    {
      const unsigned __int16 len = msg.size();
      g_Network.m_pProcess[0]->LoadSendMsg(
        member->pPlayer->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }
}

void CGuild::AddScheduleComplete(unsigned __int8 byRet, CGuild *pSrcGuild)
{
  if (!pSrcGuild)
  {
    return;
  }

  if (byRet)
  {
    m_GuildBattleSugestMatter.Clear();
    pSrcGuild->m_GuildBattleSugestMatter.Clear();
    pSrcGuild->SendMsg_ApplyGuildBattleResultInform(byRet, m_wszName);
    SendMsg_ApplyGuildBattleResultInform(byRet, pSrcGuild->m_wszName);
    return;
  }

  pSrcGuild->SendMsg_ApplyGuildBattleResultInform(0, m_wszName);
  SendMsg_ApplyGuildBattleResultInform(0, pSrcGuild->m_wszName);
  SetCopmlteGuildBattleSuggest();
  pSrcGuild->SetCopmlteGuildBattleSuggest();
}

char CGuild::SendMsg_GuildBattleProposed(char *pwszName)
{
  char destination[44]{};
  unsigned __int8 type[2] = {27, 92};

  strcpy_0(destination, pwszName);
  if (!m_MasterData.pMember)
  {
    return 0;
  }

  CPlayer *player = m_MasterData.pMember->pPlayer;
  if (!player)
  {
    return 0;
  }

  g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, destination, 0x11u);
  return 1;
}

unsigned __int8 CGuild::CheckGuildBattleSuggestRequestToDestGuild(
  unsigned int dwSrcGuildSerial,
  unsigned int dwStartTimeInx,
  unsigned int dwMemberCntInx,
  unsigned int dwMapInx)
{
if (m_bRankWait)
  {
    return static_cast<unsigned __int8>(-92);
  }
  if (!m_MasterData.IsFill())
  {
    return static_cast<unsigned __int8>(-83);
  }
  if (!m_MasterData.pMember || !m_MasterData.pMember->pPlayer)
  {
    return static_cast<unsigned __int8>(-77);
  }
  if (m_dTotalGold < 5000.0)
  {
    return static_cast<unsigned __int8>(-90);
  }
  if (!GetGuildDataFromSerial(g_Guild, 500, dwSrcGuildSerial))
  {
    return 111;
  }
  if (GetGuildDataFromSerial(g_Guild, 500, m_dwSerial))
  {
    return DestGuildIsAvailableBattleRequestState();
  }
  return 111;
}

void CGuild::SetCopmlteGuildBattleSuggest()
{
  m_GuildBattleSugestMatter.eState = _guild_battle_suggest_matter::COMPLETE_BATTLE_REQUEST;
}

void CGuild::ClearGuildBattle()
{
  m_GuildBattleSugestMatter.Clear();
  m_bInGuildBattle = false;
}

unsigned __int8 CGuild::SrcGuildIsAvailableBattleRequestState()
{
  switch (m_GuildBattleSugestMatter.eState)
  {
    case _guild_battle_suggest_matter::WAIT_BATTLE_REQUEST:
      return 0;
    case _guild_battle_suggest_matter::BATTLE_REQUEST_SUGGEST:
      return 61;
    case _guild_battle_suggest_matter::COMPLETE_REQUEST:
      return static_cast<unsigned __int8>(-105);
    case _guild_battle_suggest_matter::APPLY_BATTLE_REQUEST_SUGGEST:
      return static_cast<unsigned __int8>(-104);
    case _guild_battle_suggest_matter::COMPLETE_BATTLE_REQUEST:
      return static_cast<unsigned __int8>(-103);
    default:
      return 110;
  }
}

unsigned __int8 CGuild::DestGuildIsAvailableBattleRequestState()
{
  switch (m_GuildBattleSugestMatter.eState)
  {
    case _guild_battle_suggest_matter::WAIT_BATTLE_REQUEST:
      return 0;
    case _guild_battle_suggest_matter::BATTLE_REQUEST_SUGGEST:
      return static_cast<unsigned __int8>(-91);
    case _guild_battle_suggest_matter::COMPLETE_REQUEST:
      return static_cast<unsigned __int8>(-95);
    case _guild_battle_suggest_matter::APPLY_BATTLE_REQUEST_SUGGEST:
      return static_cast<unsigned __int8>(-94);
    case _guild_battle_suggest_matter::COMPLETE_BATTLE_REQUEST:
      return static_cast<unsigned __int8>(-93);
    default:
      return 110;
  }
}

void CGuild::SetGuildBattleMatter(
  unsigned int dwSrcGuildSerial,
  unsigned int dwStartTime,
  unsigned int dwNumber,
  unsigned int dwMapIdx)
{
  m_GuildBattleSugestMatter.pkSrc = GetGuildDataFromSerial(g_Guild, 500, dwSrcGuildSerial);
  m_GuildBattleSugestMatter.pkDest = this;
  m_GuildBattleSugestMatter.eState = _guild_battle_suggest_matter::APPLY_BATTLE_REQUEST_SUGGEST;
  m_GuildBattleSugestMatter.dwStartTime = dwStartTime;
  m_GuildBattleSugestMatter.dwNumber = dwNumber;
  m_GuildBattleSugestMatter.dwMapIdx = dwMapIdx;
}

void CGuild::CompleteOutGuildbattleCost(
  unsigned int dwSrcGuildSerial,
  unsigned int dwStartTimeInx,
  unsigned int dwMemberCntInx,
  unsigned int dwMapInx)
{
  unsigned __int8 ret = m_GuildBattleSugestMatter.pkDest->CheckGuildBattleSuggestRequestToDestGuild(
    m_dwSerial,
    dwStartTimeInx,
    dwMemberCntInx,
    dwMapInx);
  if (ret)
  {
    m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
  }
  else if (m_GuildBattleSugestMatter.pkDest->SendMsg_GuildBattleProposed(m_wszName))
  {
    m_GuildBattleSugestMatter.pkDest->SetGuildBattleMatter(
      dwSrcGuildSerial,
      dwStartTimeInx,
      dwMemberCntInx,
      dwMapInx);
    m_GuildBattleSugestMatter.eState = _guild_battle_suggest_matter::COMPLETE_REQUEST;
  }
  else
  {
    ret = static_cast<unsigned __int8>(-77);
    m_GuildBattleSugestMatter.pkSrc->PushDQSInGuildBattleCost();
  }

  SendMsg_GuildBattleSuggestResult(ret, m_GuildBattleSugestMatter.pkDest->m_wszName);
  m_GuildBattleSugestMatter.Clear();
}

void CGuild::UpdateGuildBattleWinCnt(unsigned int dwTotWin, unsigned int dwTotDraw, unsigned int dwTotLose)
{
  m_dwGuildBattleTotWin = dwTotWin;
  m_dwGuildBattleTotDraw = dwTotDraw;
  m_dwGuildBattleTotLose = dwTotLose;
  MakeQueryInfoPacket();
  MakeDownMemberPacket();
  m_byMoneyOutputKind = 0;
  SendMsg_GuildInfoUpdateInform();
}

void CGuild::PushDQSInGuildBattleRewardMoney()
{
  _qry_case_in_guildbattlerewardmoney query{};
  query.dwGuildIndex = m_nIndex;
  query.dwGuildSerial = m_dwSerial;
  query.dwAddDalant = 0;
  query.dwAddGold = 10000;
  query.out_totalgold = 0.0;
  query.out_totaldalant = 0.0;
  query.byDate[0] = GetCurrentMonth();
  query.byDate[1] = GetCurrentDay();
  query.byDate[2] = GetCurrentHour();
  query.byDate[3] = GetCurrentMin();
  g_Main.PushDQSData(
    0xFFFFFFFF,
    nullptr,
    0x27u,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));
  m_bIOWait = true;
}

void CGuild::PushDQSInGuildBattleCost()
{
  _qry_case_in_guildbattlecost qry{};
  qry.dwGuildIndex = m_nIndex;
  qry.dwGuildSerial = m_dwSerial;
  qry.dwAddDalant = 0;
  qry.dwAddGold = 5000;
  qry.out_totalgold = 0.0;
  qry.out_totaldalant = 0.0;
  qry.byDate[0] = GetCurrentMonth();
  qry.byDate[1] = GetCurrentDay();
  qry.byDate[2] = GetCurrentHour();
  qry.byDate[3] = GetCurrentMin();

  const int nSize = qry.size();
  g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x25u, reinterpret_cast<char *>(&qry), nSize);
  m_bIOWait = true;
}

void CGuild::PushDQSSourceGuildOutputGuildBattleCost()
{
  _qry_case_src_guild_out_guildbattlecost qry{};
  qry.dwGuildIndex = m_nIndex;
  qry.dwGuildSerial = m_dwSerial;
  qry.dwSubDalant = 0;
  qry.dwSubGold = 5000;
  qry.out_totalgold = 0.0;
  qry.out_totaldalant = 0.0;
  qry.byDate[0] = GetCurrentMonth();
  qry.byDate[1] = GetCurrentDay();
  qry.byDate[2] = GetCurrentHour();
  qry.byDate[3] = GetCurrentMin();
  qry.dwSrcGuildSerial = m_dwSerial;
  qry.dwStartTimeInx = m_GuildBattleSugestMatter.dwStartTime;
  qry.dwMemberCntInx = m_GuildBattleSugestMatter.dwNumber;
  qry.dwMapInx = m_GuildBattleSugestMatter.dwMapIdx;
  qry.byProcRet = 0;

  const int nSize = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x26u, reinterpret_cast<char *>(&qry), nSize);
  m_bIOWait = true;
}

void CGuild::PushDQSDestGuildOutputGuildBattleCost()
{
  _qry_case_dest_guild_out_guildbattlecost qry{};
  qry.dwGuildIndex = m_nIndex;
  qry.dwGuildSerial = m_dwSerial;
  qry.dwSubDalant = 0;
  qry.dwSubGold = 5000;
  qry.out_totalgold = 0.0;
  qry.out_totaldalant = 0.0;
  qry.byDate[0] = GetCurrentMonth();
  qry.byDate[1] = GetCurrentDay();
  qry.byDate[2] = GetCurrentHour();
  qry.byDate[3] = GetCurrentMin();
  qry.dwSrcGuildSerial = m_dwSerial;
  qry.dwStartTimeInx = m_GuildBattleSugestMatter.dwStartTime;
  qry.dwMemberCntInx = m_GuildBattleSugestMatter.dwNumber;
  qry.dwMapInx = m_GuildBattleSugestMatter.dwMapIdx;
  qry.byProcRet = 0;

  const int nSize = qry.size();
  g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x76u, reinterpret_cast<char *>(&qry), nSize);
  m_bIOWait = true;
}

void CGuild::IOMoney(
  const char *pwszIOerName,
  unsigned int dwIOerSerial,
  long double dIODalant,
  long double dIOGold,
  long double dTotalDalant,
  long double dTotalGold,
  unsigned __int8 *pbyDate,
  bool bInPut)
{
  m_dTotalDalant = dTotalDalant;
  m_dTotalGold = dTotalGold;
  MakeDownMemberPacket();
  PushHistory_IOMoney(
    bInPut,
    pwszIOerName,
    dwIOerSerial,
    dIODalant,
    dIOGold,
    dTotalDalant,
    dTotalGold,
    pbyDate);
  MakeMoneyIOPacket();
}

void CGuild::PushHistory_IOMoney(
  bool bInput,
  const char *pwszIOerName,
  unsigned int dwIOerSerial,
  long double dIODalant,
  long double dIOGold,
  long double dLeftDalant,
  long double dLeftGold,
  unsigned __int8 *pbyDate)
{
  _io_money_data *entry = nullptr;
  if (m_nIOMoneyHistoryNum >= 100)
  {
    memcpy_0(m_IOMoneyHistory, &m_IOMoneyHistory[1], sizeof(_io_money_data) * 99);
    entry = &m_IOMoneyHistory[99];
  }
  else
  {
    entry = &m_IOMoneyHistory[m_nIOMoneyHistoryNum++];
  }

  strcpy_0(entry->wszIOerName, pwszIOerName);
  entry->dwIOerSerial = dwIOerSerial;
  entry->dIODalant = dIODalant;
  entry->dIOGold = dIOGold;
  entry->dLeftDalant = dLeftDalant;
  entry->dLeftGold = dLeftGold;
  memcpy_0(entry->byDate, pbyDate, sizeof(entry->byDate));

  SendMsg_IOMoney(dwIOerSerial, dIODalant, dIOGold, bInput, pbyDate);
}

void CGuild::MakeMoneyIOPacket()
{
  m_MoneyIO_List->wDataSize = 0;

  int dataSize = 0;
  void *sData = m_MoneyIO_List->sData;
  memcpy_0(sData, &m_nIOMoneyHistoryNum, 1uLL);
  sData = static_cast<char *>(sData) + 1;
  ++dataSize;

  int historyCount = m_nIOMoneyHistoryNum;
  for (int j = 0; j < m_nIOMoneyHistoryNum; ++j)
  {
    _io_money_data *entry = &m_IOMoneyHistory[--historyCount];
    unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(entry->wszIOerName));
    memcpy_0(sData, &nameLen, 1uLL);
    sData = static_cast<char *>(sData) + 1;
    ++dataSize;

    memcpy_0(sData, entry->wszIOerName, nameLen);
    sData = static_cast<char *>(sData) + nameLen;
    dataSize += nameLen;

    memcpy_0(sData, &entry->dIODalant, 8uLL);
    sData = static_cast<char *>(sData) + 8;
    dataSize += 8;

    memcpy_0(sData, &entry->dIOGold, 8uLL);
    sData = static_cast<char *>(sData) + 8;
    dataSize += 8;

    memcpy_0(sData, &entry->dLeftDalant, 8uLL);
    sData = static_cast<char *>(sData) + 8;
    dataSize += 8;

    memcpy_0(sData, &entry->dLeftGold, 8uLL);
    sData = static_cast<char *>(sData) + 8;
    dataSize += 8;

    memcpy_0(sData, entry->byDate, 4uLL);
    sData = static_cast<char *>(sData) + 4;
    dataSize += 4;
  }

  m_MoneyIO_List->wDataSize = dataSize;
}

void CGuild::SendMsg_IOMoney(
  unsigned int dwIOerSerial,
  long double dIODalant,
  long double dIOGold,
  bool bInPut,
  unsigned __int8 *pbyDate)
{
  #pragma pack(push, 1)
  struct GuildIOMoneyMsg
  {
    unsigned int dwIOerSerial;
    unsigned __int8 byMoneyOutputKind;
    bool bInPut;
    long double dIODalant;
    long double dIOGold;
    long double dTotalDalant;
    long double dTotalGold;
    unsigned __int8 byDate[4];
  };
  #pragma pack(pop)

  GuildIOMoneyMsg msg{};
  msg.dwIOerSerial = dwIOerSerial;
  msg.byMoneyOutputKind = m_byMoneyOutputKind;
  msg.bInPut = bInPut;
  msg.dIODalant = dIODalant;
  msg.dIOGold = dIOGold;
  msg.dTotalDalant = GetTotalDalant();
  msg.dTotalGold = GetTotalGold();
  memcpy_0(msg.byDate, pbyDate, sizeof(msg.byDate));

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 27;
  pbyType[1] = 37;

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill())
    {
      if (m_MemberData[j].pPlayer)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
          pbyType,
          reinterpret_cast<char *>(&msg),
          0x2Au);
      }
    }
  }
}

void CGuild::SetGreetingmsg_GUILD(char *wszgreetmsg)
{
  _qry_case_guild_greetingmsg qry{};
  if (strlen_0(wszgreetmsg) <= 0xFF)
  {
    strcpy_s(m_wszGreetingMsg, 0x100u, wszgreetmsg);
    m_wszGreetingMsg[255] = 0;
  }

  strcpy_s(qry.in_guildgreetingmsg, 0x100u, m_wszGreetingMsg);
  qry.in_guildserial = m_dwSerial;
  const int size = static_cast<int>(sizeof(qry));
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x4Cu, reinterpret_cast<char *>(&qry), size);
}

long double CGuild::GetTotalDalant()
{
  return m_dTotalDalant;
}

long double CGuild::GetTotalGold()
{
  return m_dTotalGold;
}

char CGuild::DB_Update_GuildMaster(_guild_member_info *pNewguildMaster)
{
  _qry_case_update_guildmaster qry{};
  qry.tmp_guildindex = m_nIndex;
  qry.in_guildserial = m_dwSerial;
  if (m_MasterData.IsFill())
  {
    qry.in_guild_prev_masterSerial = m_MasterData.dwSerial;
    qry.in_guild_prev_masterPrevGrade = m_MasterData.byPrevGrade;
  }
  else
  {
    qry.in_guild_prev_masterSerial = static_cast<unsigned int>(-1);
    qry.in_guild_prev_masterPrevGrade = static_cast<unsigned __int8>(-1);
  }

  if (pNewguildMaster)
  {
    qry.in_guild_new_masterSerial = pNewguildMaster->dwSerial;
    qry.in_guild_new_masterPrevGrade = pNewguildMaster->byClassInGuild;
  }
  else
  {
    qry.in_guild_new_masterSerial = static_cast<unsigned int>(-1);
    qry.in_guild_new_masterPrevGrade = static_cast<unsigned __int8>(-1);
  }

  const int nSize = static_cast<int>(qry.size());
  if (!g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x31u, reinterpret_cast<char *>(&qry), nSize))
  {
    return 0;
  }
  return 1;
}

void CGuild::DB_Update_GuildMaster_Complete(
  unsigned int in_guild_prev_masterSerial,
  unsigned __int8 in_guild_prev_masterPrevGrade,
  unsigned int in_guild_new_masterSerial,
  unsigned __int8 in_guild_new_masterPrevGrade)
{
  _guild_member_info *prevMember =
    (in_guild_prev_masterSerial == static_cast<unsigned int>(-1))
    ? nullptr
    : GetMemberFromSerial(in_guild_prev_masterSerial);
  _guild_member_info *newMember =
    (in_guild_new_masterSerial == static_cast<unsigned int>(-1))
    ? nullptr
    : GetMemberFromSerial(in_guild_new_masterSerial);

  _guild_alter_member_grade_inform_zocl msg{};
  int alterCount = 0;

  if (prevMember)
  {
    prevMember->byClassInGuild = 0;
    msg.MemberList[alterCount].dwMemberSerial = prevMember->dwSerial;
    msg.MemberList[alterCount].byRank = prevMember->byRank;
    msg.MemberList[alterCount].byGrade = 0;
    if (prevMember->pPlayer)
    {
      prevMember->pPlayer->m_Param.m_byClassInGuild = 0;
      CGuildMasterEffect::GetInstance()->out_player(prevMember->pPlayer, m_byGrade);
    }
    ++alterCount;
  }

  m_MasterData.init();
  m_bPossibleElectMaster = true;

  if (newMember)
  {
    for (int j = 0; j < 3; ++j)
    {
      if (m_pGuildCommittee[j] && m_pGuildCommittee[j]->dwSerial == newMember->dwSerial)
      {
        m_pGuildCommittee[j] = nullptr;
        break;
      }
    }

    newMember->byClassInGuild = 2;
    m_MasterData.pMember = newMember;
    m_MasterData.dwSerial = newMember->dwSerial;
    m_MasterData.byPrevGrade = in_guild_new_masterPrevGrade;

    msg.MemberList[alterCount].dwMemberSerial = newMember->dwSerial;
    msg.MemberList[alterCount].byRank = newMember->byRank;
    msg.MemberList[alterCount].byGrade = 2;
    if (newMember->pPlayer)
    {
      newMember->pPlayer->m_Param.m_byClassInGuild = 2;
      CGuildMasterEffect::GetInstance()->in_player(newMember->pPlayer, m_byGrade);
    }
    m_bPossibleElectMaster = false;
    ++alterCount;
    s_GuildList.SetGuildMaster(m_byRace, m_wszName, m_MasterData.pMember->wszName);
  }

  MakeDownMemberPacket();
  msg.byAlterMemberNum = static_cast<unsigned __int8>(alterCount);
  if (alterCount)
  {
    unsigned __int8 pbyType[2] = {27, 32};
    for (int k = 0; k < 50; ++k)
    {
      if (m_MemberData[k].IsFill() && m_MemberData[k].pPlayer)
      {
        const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
        g_Network.m_pProcess[0]->LoadSendMsg(
          m_MemberData[k].pPlayer->m_ObjID.m_wIndex,
          pbyType,
          reinterpret_cast<char *>(&msg),
          len);
      }
    }
  }
}

void CGuild::Complete_DB_Update_Committee(char *pData)
{
  char *payload = pData;
  _guild_member_info *member = GetMemberFromSerial(*reinterpret_cast<unsigned int *>(payload));
  if (!member)
  {
    return;
  }

  member->byClassInGuild = static_cast<unsigned __int8>(payload[12]);
  if (member->pPlayer)
  {
    member->pPlayer->m_Param.m_byClassInGuild = static_cast<unsigned __int8>(payload[12]);
    member->pPlayer->m_Param.m_bGuildLock = false;
  }

  if (payload[12] == 1)
  {
    int emptyIndex = -1;
    for (int j = 0; j < 3; ++j)
    {
      if (!m_pGuildCommittee[j])
      {
        emptyIndex = j;
        break;
      }
    }
    if (emptyIndex != -1)
    {
      m_pGuildCommittee[emptyIndex] = member;
    }
  }
  else if (payload[12] == 0)
  {
    for (int j = 0; j < 3; ++j)
    {
      if (m_pGuildCommittee[j] && m_pGuildCommittee[j]->dwSerial == *reinterpret_cast<unsigned int *>(payload))
      {
        m_pGuildCommittee[j] = nullptr;
        break;
      }
    }
  }

  MakeDownMemberPacket();

  _guild_alter_member_grade_inform_zocl msg{};
  msg.MemberList[0].dwMemberSerial = member->dwSerial;
  msg.MemberList[0].byRank = member->byRank;
  msg.MemberList[0].byGrade = member->byClassInGuild;
  msg.byAlterMemberNum = 1;

  unsigned __int8 pbyType[2]{};
  pbyType[0] = 27;
  pbyType[1] = 32;
  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].pPlayer)
    {
      const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(
        m_MemberData[j].pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        len);
    }
  }

  SendMsg_ManageGuildCommitteeResult(payload[12] != 0, member->wszName);
}

_guild_member_info *CGuild::GetMemberFromSerial(unsigned int dwMemberSerial)
{
  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill() && m_MemberData[j].dwSerial == dwMemberSerial)
    {
      return &m_MemberData[j];
    }
  }
  return nullptr;
}

unsigned int CGuild::GetGuildMasterSerial()
{
  return m_MasterData.dwSerial;
}

char *CGuild::GetGuildMasterName()
{
  int memberCount = 0;

  for (int j = 0; j < 50; ++j)
  {
    if (m_MemberData[j].IsFill())
    {
      if (m_MemberData[j].byClassInGuild == 2)
      {
        return m_MemberData[j].wszName;
      }

      ++memberCount;
      if (memberCount >= m_nMemberNum)
      {
        break;
      }
    }
  }

  return nullptr;
}

void CGuild::SendMsg_ManageGuildCommitteeResult(char bAppoint, char *pwszCommitteeName)
{
  struct CommitteeResultMsg
  {
    char byAppoint;
    char wszCommitteeName[17];
  };

  CommitteeResultMsg msg{};
  msg.byAppoint = bAppoint;
  strcpy_0(msg.wszCommitteeName, pwszCommitteeName);

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 27;
  pbyType[1] = 125;

  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->pPlayer)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        member->pPlayer->m_ObjID.m_wIndex,
        pbyType,
        reinterpret_cast<char *>(&msg),
        0x12u);
    }
  }
}

void CGuild::SendMsg_MasterElectPossible(char bPossible)
{
  char szMsg[32]{};
  szMsg[0] = bPossible;

  unsigned __int8 pbyType[16]{};
  pbyType[0] = 27;
  pbyType[1] = 126;

  for (int j = 0; j < 50; ++j)
  {
    _guild_member_info *member = &m_MemberData[j];
    if (member->IsFill() && member->pPlayer)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        member->pPlayer->m_ObjID.m_wIndex,
        pbyType,
        szMsg,
        1u);
    }
  }
}

void CGuild::CompleteSelectMasterLastConn(unsigned int dwLastConnTime)
{
  if (dwLastConnTime != static_cast<unsigned int>(-1))
  {
    m_bPossibleElectMaster = true;
    MakeDownMemberPacket();
    SendMsg_MasterElectPossible(m_bPossibleElectMaster);
  }
}

unsigned int CGuild::GetMemberNum()
{
  return static_cast<unsigned int>(m_nMemberNum);
}

void CGuild::Release()
{
  m_dwSerial = static_cast<unsigned int>(-1);
  m_nMemberNum = 0;
  m_nApplierNum = 0;
  m_bNowProcessSgtMter = false;
  m_nTempMemberNum = 0;
  m_dTotalDalant = 0.0;
  m_dTotalGold = 0.0;
  m_dwEmblemBack = static_cast<unsigned int>(-1);
  m_dwEmblemMark = static_cast<unsigned int>(-1);
  m_nIOMoneyHistoryNum = 0;
  m_MasterData.init();
  m_dwGuildBattleTotWin = 0;
  m_dwGuildBattleTotDraw = 0;
  m_dwGuildBattleTotLose = 0;
  m_bPossibleElectMaster = false;

  for (int j = 0; j < 3; ++j)
  {
    m_pGuildCommittee[j] = nullptr;
  }

  memset_0(m_IOMoneyHistory, 0, sizeof(m_IOMoneyHistory));

  m_wszName[0] = 0;
  m_aszName[0] = 0;
  m_byGrade = 0;
  m_byRace = static_cast<unsigned __int8>(-1);
  m_wszGreetingMsg[0] = 0;
  m_dwSuggesterSerial = 0;

  memset_0(&m_SuggestedMatter, 0, sizeof(m_SuggestedMatter));
  m_GuildBattleSugestMatter.Clear();
  m_bInGuildBattle = false;

  if (m_DownPacket_Member)
  {
    memset_0(m_DownPacket_Member, 0, sizeof(*m_DownPacket_Member));
  }

  if (m_QueryPacket_Info)
  {
    memset_0(m_QueryPacket_Info, 0, sizeof(*m_QueryPacket_Info));
  }

  m_bDBWait = false;
  m_bIOWait = false;
  m_bRankWait = false;
  m_byMoneyOutputKind = static_cast<unsigned __int8>(-1);
  m_dwLastLoopTime = 0;
  m_szHistoryFileName[0] = 0;
}

CGuild *GetGuildDataFromSerial(CGuild *pData, int nNum, unsigned int dwSerial)
{
  for (int j = 0; j < nNum; ++j)
  {
    if (pData[j].IsFill() && pData[j].m_dwSerial == dwSerial)
    {
      return &pData[j];
    }
  }
  return nullptr;
}

CGuild *GetGuildPtrFromName(CGuild *pData, int nNum, char *pwszGuildName)
{
  for (int j = 0; j < nNum; ++j)
  {
    if (pData[j].IsFill() && !strcmp_0(pData[j].m_wszName, pwszGuildName))
    {
      return &pData[j];
    }
  }
  return nullptr;
}
