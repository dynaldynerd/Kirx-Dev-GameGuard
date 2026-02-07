#include "pch.h"

#include "CGuild.h"

#include "CGuildMasterEffect.h"
#include "CNetworkEX.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CGuildBattleController.h"
#include "CHonorGuild.h"
#include "CMoneySupplyMgr.h"
#include "GuildBattleTypes.h"
#include "guild_member_refresh_data.h"
#include "guild_alter_member_grade_inform_zocl.h"
#include "guild_alter_member_state_inform_zocl.h"

CGuildList CGuild::s_GuildList;

void CGuild::Init(unsigned int index)
{
  m_nIndex = static_cast<int>(index);
}

bool CGuild::IsFill()
{
  return m_dwSerial != static_cast<unsigned int>(-1);
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

