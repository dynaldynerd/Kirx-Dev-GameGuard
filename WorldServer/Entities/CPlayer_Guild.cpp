#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "pnt_rect.h"
#include "CMainThread.h"
#include "CItemStore.h"
#include "CNetworkEX.h"
#include "StoreList_fld.h"
#include "CActionPointSystemMgr.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "CGuild.h"
#include "CUserDB.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CNationSettingManager.h"
#include "CNationSettingData.h"
#include "GlobalObjects.h"
#include "CGuildBattleController.h"
#include "CGuildMasterEffect.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CHolyStoneSystem.h"
#include "CPotionMgr.h"
#include "guild_honor_set_request_clzo.h"
#include "CMgrGuildHistory.h"
#include "CMgrAvatorItemHistory.h"
#include "CPostSystemManager.h"
#include "CPartyPlayer.h"
#include "LendItemMng.h"
#include "CUnmannedTraderController.h"
#include "cStaticMember_Player.h"
#include "CTransportShip.h"
#include "CLogFile.h"
#include "CItemUpgradeTable.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "COreCuttingTable.h"
#include "CDarkHole.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CDarkHoleDungeonQuestSetup.h"
#include "TicketItem_fld.h"
#include "darkhole_create_setdata.h"
#include "ENTER_DUNGEON_NEW_POS.h"
#include "QuestHappenEvent_fld.h"
#include "announ_message_receipt_udp.h"
#include "equip_up_item_lv_limit_zocl.h"
#include "sf_delay_download_result_zocl.h"
#include "darkhole_ask_reenter_inform_zocl.h"
#include "darkhole_answer_reenter_result_zocl.h"
#include "darkhole_clear_out_result_zocl.h"
#include "darkhole_enter_result_zocl.h"
#include "darkhole_giveup_out_result_zocl.h"
#include "darkhole_open_result_zocl.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "move_to_own_stonemap_inform_zocl.h"
#include "move_to_own_stonemap_result_zocl.h"
#include "pt_result_change_tax_rate_zocl.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "economy_history_data.h"
#include "DqsDbStructs.h"
#include "DqsOnRunPayloads.h"
#include "EconomySystemFunctions.h"
#include "TimeItem.h"
#include "qry_case_inputgmoney.h"
#include "trans_ship_renew_ticket_result_zocl.h"
#include "trans_gm_msg_inform_zocl.h"
#include "trunk_change_passwd_result_zocl.h"
#include "trunk_download_result_zocl.h"
#include "trunk_est_result_zocl.h"
#include "trunk_extend_result_zocl.h"
#include "trunk_hint_answer_result_zocl.h"
#include "trunk_io_money_result_zocl.h"
#include "trunk_io_result_zocl.h"
#include "trunk_potionsocket_division_result_zocl.h"
#include "trunk_pw_hint_index_result_zocl.h"
#include "trunk_res_division_result_zocl.h"
#include "guildroom_rent_request_clzo.h"
#include "guildroom_enter_request_clzo.h"
#include "guildroom_out_request_clzo.h"
#include "guildroom_resttime_request_clzo.h"
#include "GuardTowerItem_fld.h"
#include "UnitKeyItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "BulletItem_fld.h"
#include "ItemCombine_exp_fld.h"
#include "combine_ex_item_result_zocl.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_accept_result_zocl.h"
#include "qry_case_disjointguild.h"
#include "qry_case_selfleave.h"
#include "WorldServerUtil.h"
#include "NetCheckPackets.h"
#include "GlobalObjectDefs.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace
{
bool IsEconomyFeeLevel(int level)
{
  return level == 30 || level == 40 || level == 50 || level == 60;
}

bool IsNearGuildRoomNpc(CPlayer *player)
{
  if (!player || !player->m_pCurMap)
  {
    return false;
  }

  CItemStoreManager *storeManager = CItemStoreManager::Instance();
  if (!storeManager)
  {
    return false;
  }

  CMapItemStoreList *storeList = storeManager->GetMapItemStoreListBySerial(player->m_pCurMap->GetMapCode());
  if (!storeList)
  {
    return false;
  }

  for (int storeIndex = 0; storeIndex < storeList->m_nItemStoreNum; ++storeIndex)
  {
    CItemStore *store = &storeList->m_ItemStore[storeIndex];
    if (!store->m_bLive || !store->m_pRec)
    {
      continue;
    }

    bool isGuildRoomNpc = false;
    for (int classIndex = 0; classIndex < 10; ++classIndex)
    {
      if (store->m_pRec->m_nNpc_Class[classIndex] == 44)
      {
        isGuildRoomNpc = true;
        break;
      }
    }

    if (!isGuildRoomNpc)
    {
      continue;
    }

    if (GetSqrt(store->GetStorePos(), player->m_fCurPos) < 100.0f)
    {
      return true;
    }
  }

  return false;
}
} // namespace


void CPlayer::SendMsg_GuildJoinApplyResult(char byRetCode, CGuild *pApplyGuild)
{
  _guild_join_apply_result_zocl packet{};
  packet.byRetCode = byRetCode;
  if (pApplyGuild)
  {
    packet.dwGuildSerial = pApplyGuild->m_dwSerial;
    std::strcpy(packet.wszGuildName, pApplyGuild->m_wszName);
  }
  else
  {
    packet.dwGuildSerial = static_cast<unsigned int>(-1);
    packet.wszGuildName[0] = 0;
  }

  unsigned __int8 type[2] = {27, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildJoinApplyCancelResult(char byRetCode)
{
  _guild_join_apply_cancel_result_zocl payload{};
  payload.byRetCode = byRetCode;
  unsigned __int8 type[2] = {27, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::SendMsg_GuildJoinApplyRejectInform()
{
  _guild_join_apply_reject_inform_zocl payload{};
  payload.sDum = 0;
  unsigned __int8 type[2] = {27, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::SendMsg_GuildJoinAcceptFail(char byRetCode, unsigned int dwApplierSerial)
{
  _guild_join_accept_fail_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.dwApplierSerial = dwApplierSerial;

  unsigned __int8 type[2] = {27, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_VoteResult(unsigned int dwMatterVoteSynKey, unsigned __int8 byRetCode)
{
  _guild_vote_result_zocl packet{};
  packet.dwMatterVoteSynKey = dwMatterVoteSynKey;
  packet.byRetCode = static_cast<char>(byRetCode);

  unsigned __int8 type[2] = {27, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}



void CPlayer::SendMsg_CancelSuggestResult(char byRetCode)
{
  _guild_cancel_suggest_result_zocl payload{};
  payload.byRetCode = byRetCode;
  unsigned __int8 type[2] = {27, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::SendMsg_GuildSetHonorResult(char byRetCode)
{
  _guild_honor_set_result_zocl payload{};
  payload.byRetCode = byRetCode;
  unsigned __int8 type[2] = {27, 114};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::SendMsg_GuildPushMoneyResult(char byRetCode)
{
  _guild_push_money_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.dwLeftDalant = this->m_Param.GetDalant();
  packet.dwLeftGold = this->m_Param.GetGold();

  unsigned __int8 type[2] = {27, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}


void CPlayer::SendMsg_OfferSuggestResult(char byRetCode)
{
  _guild_offer_suggest_result_zocl payload{};
  payload.byRetCode = byRetCode;
  unsigned __int8 type[2] = {27, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}


void CPlayer::SendMsg_GuildRoomRentResult(unsigned __int8 byRetCode, unsigned __int8 bySubRetCode, unsigned __int8 byRoomType)
{
  _guildroom_rent_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySubRetCode = static_cast<char>(bySubRetCode);
  packet.byRoomType = byRoomType;

  unsigned __int8 type[2] = {27, 103};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildRoomEnterResult(unsigned __int8 byRetCode, unsigned __int8 bySubRetCode, unsigned __int8 byMapIndex, unsigned __int16 wMapLayer, float *pPos, int nRestTime)
{
  _guildroom_enter_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.byRetSubCode = static_cast<char>(bySubRetCode);
  packet.byMapIndex = byMapIndex;
  packet.wMapLayerIndex = wMapLayer;
  FloatToShort(pPos, packet.sNewPos, 3);
  packet.restTime = nRestTime;

  unsigned __int8 type[2] = {27, 105};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildRoomRestTimeResult()
{
  // this is not a stub
}


void CPlayer::SendMsg_GuildEstablishFail(char byRetCode)
{
  _guild_establish_fail_zocl msg{};
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2] = {27, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_GuildJoinOtherInform()
{
  _guild_join_other_inform_zocl msg{};
  msg.dwAvatorSerial = this->m_id.dwSerial;
  msg.dwGuildSerial = this->m_Param.m_pGuild ? this->m_Param.m_pGuild->m_dwSerial : static_cast<unsigned int>(-1);
  msg.wVisualVersion = this->m_wVisualVer;

  unsigned __int8 type[2] = {27, 39};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), false);
}


#if 0 // duplicate implementation exists in CPlayer.cpp

#endif




void CPlayer::Guild_Insert_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_insertguild_local *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->in_guildindex];
  _guild_member_info members[8]{};
  CPlayer::CashChangeStateFlag cashChangeState{0};

  guild->ReleaseTemp();
  for (unsigned __int8 memberSlot = 0; memberSlot < query->in_membernum; ++memberSlot)
  {
    CPlayer *member = &g_Player[query->in_memberindex[memberSlot]];
    if (member->m_bLive && member->m_dwObjSerial == query->in_memberserial[memberSlot])
    {
      member->m_Param.m_bGuildLock = 0;
    }
  }

  if (pData->m_byResult || guild->IsFill())
  {
    CPlayer *guildMaster = &g_Player[query->tmp_Esterindex];
    if (guildMaster->m_bLive && guildMaster->m_dwObjSerial == query->tmp_Esterserial)
    {
      char guildNameAnsi[136]{};
      char guildNameWide[17]{};
      const unsigned int estConsumeDalant =
        static_cast<unsigned int>(g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant());

      guildMaster->SendMsg_GuildEstablishFail(255);
      guildMaster->AddDalant(estConsumeDalant, 0);
      std::strcpy(guildNameWide, query->in_w_guildName);
      W2M(guildNameWide, guildNameAnsi, 128);
      CPlayer::s_MgrItemHistory.guild_est_money_rollback(
        guildMaster->m_ObjID.m_wIndex,
        guildNameAnsi,
        estConsumeDalant,
        guildMaster->m_Param.GetDalant(),
        guildMaster->m_szItemHistoryFileName);
    }
    return;
  }

  CPlayer *guildMaster = &g_Player[query->tmp_Esterindex];
  if (guildMaster->m_bLive && guildMaster->m_dwObjSerial == query->tmp_Esterserial)
  {
    guildMaster->SendMsg_AlterMoneyInform(0);
  }

  for (unsigned __int8 memberSlot = 0; memberSlot < query->in_membernum; ++memberSlot)
  {
    _guild_member_info &memberInfo = members[memberSlot];
    memberInfo.dwSerial = query->in_memberserial[memberSlot];
    std::strcpy(memberInfo.wszName, &query->tmp_w_membername[17 * memberSlot]);
    memberInfo.byLv = query->tmp_lv[memberSlot];
    memberInfo.dwPvpPoint = query->tmp_pvp[memberSlot];
    memberInfo.byClassInGuild = 0;
    memberInfo.byGrade = query->tmp_grade[memberSlot];
  }

  guild->EstGuild(
    query->out_guildserial,
    const_cast<char *>(query->in_w_guildName),
    query->in_guildRace,
    query->in_membernum,
    members);

  for (unsigned __int8 memberSlot = 0; memberSlot < query->in_membernum; ++memberSlot)
  {
    CPlayer *member = &g_Player[query->in_memberindex[memberSlot]];
    if (member->m_bLive && member->m_dwObjSerial == query->in_memberserial[memberSlot])
    {
      _guild_member_info *guildMember = guild->LoginMember(query->in_memberserial[memberSlot], member);
      guild->SendMsg_DownPacket(0, guildMember);
      member->m_pUserDB->m_AvatorData.dbAvator.m_dwGuildSerial = query->out_guildserial;
      member->m_Param.m_pGuild = guild;
      member->m_Param.m_pGuildMemPtr = guildMember;
      member->m_Param.SetClassInGuild(0);
      member->UpdateVisualVer(cashChangeState);
      member->SendMsg_GuildJoinOtherInform();
    }
  }

  CGuildBattleController::Instance()->UpdatePossibleBattleGuildList();
}

void CPlayer::Guild_Join_Accept_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_joinacguild_local *>(pData->m_sData);
  CPlayer *applicant = &g_Player[query->in_applierindex];
  CGuild *guild = &g_Guild[query->in_guildindex];

  if (applicant->m_bLive && applicant->m_dwObjSerial == query->in_applierserial)
  {
    applicant->m_Param.m_bGuildLock = 0;
  }

  if (guild->m_dwSerial != query->in_guildserial)
  {
    return;
  }

  --guild->m_nTempMemberNum;
  if (guild->m_nMemberNum >= 50 || pData->m_byResult)
  {
    return;
  }

  _guild_member_info *acceptor = guild->GetMemberFromSerial(query->in_accepterserial);
  if (!acceptor)
  {
    return;
  }

  _guild_applier_info *guildApplicant = guild->GetApplierFromSerial(query->in_applierserial);
  if (!guildApplicant)
  {
    return;
  }

  applicant = guildApplicant->pPlayer;
  applicant->m_Param.SetClassInGuild(0);
  applicant->m_Param.m_pApplyGuild = 0LL;
  guild->PopApplier(query->in_applierserial, 0);

  _guild_member_info memberSheet{};
  memberSheet.dwSerial = applicant->m_dwObjSerial;
  std::strcpy(memberSheet.wszName, applicant->m_Param.GetCharNameW());
  memberSheet.byLv = static_cast<unsigned __int8>(applicant->GetLevel());
  memberSheet.dwPvpPoint = static_cast<int>(applicant->m_Param.GetPvPPoint());
  memberSheet.byClassInGuild = applicant->m_Param.GetClassInGuild();
  memberSheet.pPlayer = applicant;

  _guild_member_info *guildMember = guild->PushMember(&memberSheet);
  if (guildMember)
  {
    guild->SendMsg_GuildJoinAcceptInform(guildMember, query->in_accepterserial);
  }

  CPlayer::CashChangeStateFlag cashChangeState{0};
  applicant->m_Param.m_pGuild = guild;
  applicant->m_Param.m_pGuildMemPtr = guildMember;
  applicant->UpdateVisualVer(cashChangeState);
  applicant->SendMsg_GuildJoinOtherInform();
  guild->SendMsg_GuildMemberLogin(
    applicant->m_dwObjSerial,
    applicant->m_wRegionMapIndex,
    applicant->m_wRegionIndex);

  char accepterNameAnsi[144]{};
  W2M(acceptor->wszName, accepterNameAnsi, 128);
  CGuild::s_MgrHistory.join_member(
    applicant->m_Param.GetCharNameA(),
    applicant->m_dwObjSerial,
    accepterNameAnsi,
    acceptor->dwSerial,
    guild->m_nMemberNum,
    guild->m_szHistoryFileName);

  CGuildBattleController::Instance()->JoinGuild(
    applicant->m_ObjID.m_wIndex,
    guild->m_dwSerial,
    applicant->m_dwObjSerial);
}

void CPlayer::Guild_Self_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_selfleave *>(pData->m_sData);
  CPlayer *player = &g_Player[query->tmp_leaverindex];
  CGuild *guild = &g_Guild[query->tmp_guildindex];

  if (player->m_bLive && player->m_dwObjSerial == query->in_leaverserial)
  {
    player->m_Param.m_bGuildLock = 0;
  }
  else
  {
    player = 0LL;
  }

  if (!guild->IsFill() || guild->m_dwSerial != query->tmp_guildserial)
  {
    if (player)
    {
      player->SendMsg_GuildSelfLeaveResult(255);
    }
    return;
  }

  if (player && player->m_pUserDB && player->m_Param.m_pGuild)
  {
    CGuildRoomSystem *guildRoomSystem = CGuildRoomSystem::GetInstance();
    if (guildRoomSystem->IsGuildRoomMemberIn(
          player->m_Param.m_pGuild->m_dwSerial,
          player->m_ObjID.m_wIndex,
          player->m_pUserDB->m_dwSerial))
    {
      if (guildRoomSystem->SetPlayerOut(
            player->m_Param.m_pGuild->m_dwSerial,
            player->m_ObjID.m_wIndex,
            player->m_pUserDB->m_dwSerial))
      {
        player->SendMsg_GuildSelfLeaveResult(255);
        return;
      }
    }
  }

  if (player)
  {
    CGuildBattleController::Instance()->LeaveGuild(player);
  }
  else
  {
    CGuildBattleController::Instance()->LeaveGuild(guild->m_dwSerial, query->in_leaverserial);
  }
  if (player && player->m_Param.m_byClassInGuild == 2)
  {
    CGuildMasterEffect::GetInstance()->out_player(player, guild->GetGrade());
  }

  guild->SendMsg_LeaveMember(query->in_leaverserial, 1, 0);
  guild->PopMember(query->in_leaverserial);

  if (player)
  {
    CPlayer::CashChangeStateFlag cashChangeState{0};
    player->m_Param.m_pGuild = 0LL;
    player->SendMsg_GuildSelfLeaveResult(0);
    player->UpdateVisualVer(cashChangeState);
    player->SendMsg_GuildJoinOtherInform();
    player->SetLastAttBuff(0);
    CGuild::s_MgrHistory.leave_member(
      player->m_Param.GetCharNameA(),
      player->m_dwObjSerial,
      1,
      guild->m_nMemberNum,
      guild->m_szHistoryFileName,
      0);
  }

  if (guild->GetMemberNum() <= 0)
  {
    _qry_case_disjointguild disjointQuery{};
    disjointQuery.in_guildserial = guild->m_dwSerial;
    disjointQuery.tmp_guildindex = guild->m_nIndex;
    g_Main.PushDQSData(
      -1,
      0LL,
      22,
      reinterpret_cast<char *>(&disjointQuery),
      static_cast<int>(disjointQuery.size()));
  }
}

void CPlayer::Guild_Force_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_forceleave *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->in_guildIndex];

  if (!guild->IsFill() || guild->m_dwSerial != query->in_guildserial)
  {
    return;
  }

  _guild_member_info *member = guild->GetMemberFromSerial(query->in_leaverserial);
  if (!member)
  {
    return;
  }

  if (member->pPlayer)
  {
    CPlayer *player = member->pPlayer;
    CGuildBattleController::Instance()->LeaveGuild(player);
    if (player->m_Param.m_pGuild)
    {
      CGuildRoomSystem *guildRoomSystem = CGuildRoomSystem::GetInstance();
      if (guildRoomSystem->IsGuildRoomMemberIn(
            player->m_Param.m_pGuild->m_dwSerial,
            player->m_ObjID.m_wIndex,
            player->m_pUserDB->m_dwSerial))
      {
        guildRoomSystem->SetPlayerOut(
          player->m_Param.m_pGuild->m_dwSerial,
          player->m_ObjID.m_wIndex,
          player->m_pUserDB->m_dwSerial);
      }
    }

    CPlayer::CashChangeStateFlag cashChangeState{0};
    player->m_Param.m_bGuildLock = 0;
    player->m_Param.m_pGuild = 0LL;
    player->m_Param.m_pGuildMemPtr = 0LL;
    player->UpdateVisualVer(cashChangeState);
    player->SendMsg_GuildJoinOtherInform();
    player->SetLastAttBuff(0);
    if (query->in_bPunish)
    {
      player->SendMsg_GuildForceLeaveBoradori();
    }

    CGuild::s_MgrHistory.leave_member(
      player->m_Param.GetCharNameA(),
      member->dwSerial,
      0,
      guild->m_nMemberNum - 1,
      guild->m_szHistoryFileName,
      query->in_bPunish);
  }
  else
  {
    CGuildBattleController::Instance()->LeaveGuild(guild->m_dwSerial, query->in_leaverserial);
  }

  guild->SendMsg_LeaveMember(query->in_leaverserial, 0, query->in_bPunish);
  guild->PopMember(query->in_leaverserial);
}

void CPlayer::Guild_Push_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_inputgmoney *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->tmp_guildindex];

  if (guild->m_dwSerial != query->in_guildserial)
  {
    return;
  }

  guild->m_bIOWait = 0;
  if (pData->m_byResult)
  {
    _guild_member_info *member = guild->GetMemberFromSerial(query->in_pusherserial);
    if (member && member->pPlayer)
    {
      CPlayer *player = member->pPlayer;
      player->AddGold(query->dwAddGold, 1);
      player->AddDalant(query->dwAddDalant, 1);
      CPlayer::s_MgrItemHistory.guild_pop_money_rollback(
        player->m_ObjID.m_wIndex,
        guild->m_aszName,
        query->dwAddDalant,
        query->dwAddGold,
        player->m_Param.GetDalant(),
        player->m_Param.GetGold(),
        player->m_szItemHistoryFileName);
    }
    return;
  }

  guild->m_byMoneyOutputKind = 0;
  unsigned __int8 date[4]{};
  std::memcpy(date, query->in_date, sizeof(date));
  guild->IOMoney(
    query->in_w_pushername,
    query->in_pusherserial,
    static_cast<double>(static_cast<int>(query->dwAddDalant)),
    static_cast<double>(static_cast<int>(query->dwAddGold)),
    query->out_totaldalant,
    query->out_totalgold,
    date,
    1);
}

void CPlayer::Guild_Pop_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_outputgmoney *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->tmp_guildindex];

  if (guild->m_dwSerial != query->in_guildserial)
  {
    return;
  }

  guild->m_bIOWait = 0;
  if (query->byProcRet)
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
      "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%u) SubG(%u) %s(%u)\r\n"
      "\t\t_qry_case_outputgmoney Ret(%u) Fail!",
      query->in_guildserial,
      query->out_totaldalant,
      query->out_totalgold,
      query->dwSubDalant,
      query->dwSubGold,
      query->in_w_popername,
      query->in_poperserial,
      query->byProcRet);
    return;
  }

  if (pData->m_byResult)
  {
    return;
  }

  unsigned __int8 date[4]{};
  std::memcpy(date, query->in_date, sizeof(date));
  guild->IOMoney(
    query->in_w_popername,
    query->in_poperserial,
    -0.0 - static_cast<double>(static_cast<int>(query->dwSubDalant)),
    -0.0 - static_cast<double>(static_cast<int>(query->dwSubGold)),
    query->out_totaldalant,
    query->out_totalgold,
    date,
    0);

  _guild_member_info *member = guild->GetMemberFromSerial(query->in_poperserial);
  if (!member)
  {
    return;
  }

  CPlayer *player = member->pPlayer;
  if (player)
  {
    player->AddDalant(query->dwSubDalant, 1);
    player->AddGold(query->dwSubGold, 1);
    CPlayer::s_MgrItemHistory.guild_pop_money(
      player->m_ObjID.m_wIndex,
      guild->m_aszName,
      query->dwSubDalant,
      query->dwSubGold,
      player->m_Param.GetDalant(),
      player->m_Param.GetGold(),
      player->m_szItemHistoryFileName);
    player->SendMsg_AlterMoneyInform(0);
    return;
  }

  if (query->dwSubDalant)
  {
    g_Main.Push_ChargeItem(query->in_poperserial, -1, query->dwSubDalant, 268435455, 1u);
  }
  if (query->dwSubGold)
  {
    g_Main.Push_ChargeItem(query->in_poperserial, -1, query->dwSubGold, 268435455, 2u);
  }

  g_Main.m_logSystemError.Write(
    "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
    "\t\tPush Charge Money BECAUSE Poper Connection Closed : Poper Serial(%u) Pop Dalant(%u) Pop Gold(%u)\r\n",
    query->in_poperserial,
    query->dwSubDalant,
    query->dwSubGold);
}

void CPlayer::Guild_Buy_Emblem_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_buyemblem *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->tmp_guildindex];

  if (guild->m_dwSerial != query->in_guildserial)
  {
    return;
  }

  guild->m_bIOWait = 0;
  if (query->byProcRet)
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::Guild_Buy_Emblem_Complete(...) : \r\n"
      "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%d) %s(%u)\r\n"
      "\t\tqry_case_buyemblem Ret(%u) Fail!",
      query->in_guildserial,
      query->out_totaldalant,
      query->out_totalgold,
      query->in_emblemdlant,
      query->tmp_w_suggestorname,
      query->in_suggestorSerial,
      query->byProcRet);
    return;
  }

  if (pData->m_byResult)
  {
    return;
  }

  guild->m_byMoneyOutputKind = 1;
  unsigned __int8 date[4]{};
  std::memcpy(date, query->in_date, sizeof(date));
  guild->IOMoney(
    query->tmp_w_suggestorname,
    query->in_suggestorSerial,
    static_cast<double>(query->in_emblemdlant),
    0.0,
    query->out_totaldalant,
    query->out_totalgold,
    date,
    0);
  guild->UpdateEmblem(query->in_emblemback, query->in_emblemmark);
}

void CPlayer::Guild_Disjoint_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_disjointguild *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->tmp_guildindex];

  if (guild && guild->m_dwSerial == query->in_guildserial && !pData->m_byResult)
  {
    if (guild->m_nApplierNum > 0)
    {
      guild->SendMsg_GuildDisjointInform();
    }
    guild->Release();
  }
}

void CPlayer::Guild_Update_GuildMater_Complete(_DB_QRY_SYN_DATA *pData)
{
  const auto *query = reinterpret_cast<_qry_case_update_guildmaster *>(pData->m_sData);
  CGuild *guild = &g_Guild[query->tmp_guildindex];

  if (guild && guild->m_dwSerial == query->in_guildserial && !pData->m_byResult)
  {
    guild->DB_Update_GuildMaster_Complete(
      query->in_guild_prev_masterSerial,
      query->in_guild_prev_masterPrevGrade,
      query->in_guild_new_masterSerial,
      query->in_guild_new_masterPrevGrade);
  }
}


void CPlayer::pc_GuildBattleBlock(bool bBlock)
{
  this->m_bBlockGuildBattleMsg = bBlock;
}


void CPlayer::pc_GuildEstablishRequest(char *pwszGuildName)
{
  unsigned __int8 resultCode = 0;
  int selectedGuildIndex = -1;
  unsigned __int8 memberCount = 0;
  int memberIndices[8] = {};
  unsigned int memberSerials[8] = {};
  unsigned int memberPvpPoints[8] = {};
  unsigned __int8 memberLevels[8] = {};
  unsigned __int8 memberGrades[8] = {};
  char memberNames[136] = {};

  std::memset(memberIndices, 255, sizeof(memberIndices));
  std::memset(memberSerials, 255, sizeof(memberSerials));

  CPartyPlayer *party = this->m_pPartyMgr;
  CPartyPlayer **partyMembers = party ? party->GetPtrPartyMember() : nullptr;
  CGuild *reservedGuild = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  if (!resultCode)
  {
    for (char *cursor = pwszGuildName; *cursor; ++cursor)
    {
      if (*cursor == ' ' || *cursor == '\'')
      {
        resultCode = 15;
        break;
      }
    }
  }

  if (!resultCode && !CTSingleton<CNationSettingManager>::Instance()->IsNormalString(pwszGuildName))
  {
    resultCode = 15;
  }

  if (!resultCode && !IsSQLValidString(pwszGuildName))
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::pc_GuildEstablishRequest() : %u(%s) ::IsSQLValidString(pwszGuildName(%s)) Invalid!",
      this->m_dwObjSerial,
      this->m_Param.GetCharNameA(),
      pwszGuildName);
    resultCode = 15;
  }

  if (this->IsPunished(1u, true))
  {
    return;
  }

  if (!resultCode && (!party || !party->IsPartyMode()))
  {
    resultCode = 1;
  }

  if (!resultCode && !party->IsPartyBoss())
  {
    resultCode = 2;
  }

  if (!resultCode && g_Main.IsReleaseServiceMode() && this->m_byUserDgr)
  {
    resultCode = 13;
  }

  if (!resultCode && this->m_Param.GetDalant() < g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant())
  {
    resultCode = 12;
  }

  if (!resultCode)
  {
    for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
    {
      if (!g_Guild[guildIndex].IsFill() && !g_Guild[guildIndex].m_bDBWait)
      {
        reservedGuild = &g_Guild[guildIndex];
        break;
      }
    }
    if (!reservedGuild)
    {
      resultCode = 10;
    }
  }

  if (!resultCode && !partyMembers)
  {
    resultCode = 1;
  }

  if (!resultCode)
  {
    for (int partySlot = 0; partySlot < 8 && partyMembers[partySlot]; ++partySlot)
    {
      CPlayer *member = &g_Player[partyMembers[partySlot]->m_wZoneIndex];
      if (!member->m_bLive)
      {
        resultCode = 9;
        break;
      }

      if (member->IsPunished(1u, true))
      {
        resultCode = 70;
        break;
      }

      if (member->m_Param.m_pGuild || member->m_Param.m_bGuildLock)
      {
        resultCode = 7;
        break;
      }

      if (member->m_Param.m_pApplyGuild)
      {
        resultCode = 8;
        break;
      }

      if (!member->m_Param.m_pClassHistory[0])
      {
        resultCode = 11;
        break;
      }

      if (member->m_Param.GetRaceCode() != this->m_Param.GetRaceCode())
      {
        resultCode = 70;
        break;
      }

      memberIndices[partySlot] = member->m_id.wIndex;
      memberSerials[partySlot] = member->m_id.dwSerial;
      memberPvpPoints[partySlot] = static_cast<unsigned int>(static_cast<int>(member->m_Param.GetPvPPoint()));
      memberLevels[partySlot] = static_cast<unsigned __int8>(member->m_Param.GetLevel());
      memberGrades[partySlot] = member->m_Param.m_byPvPGrade;
      std::strcpy(&memberNames[17 * partySlot], member->m_Param.GetCharNameW());
      ++memberCount;
    }
  }

  if (!resultCode && g_Main.IsReleaseServiceMode())
  {
    if (this->m_byUserDgr)
    {
      resultCode = 13;
    }
    else if (memberCount < 8u)
    {
      resultCode = 9;
    }
  }
  else if (!resultCode && !this->m_byUserDgr && memberCount < 8u)
  {
    resultCode = 9;
  }

  if (!resultCode)
  {
    for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
    {
      if ((g_Guild[guildIndex].IsFill() || g_Guild[guildIndex].m_bDBWait)
          && !std::strcmp(pwszGuildName, g_Guild[guildIndex].m_wszName))
      {
        resultCode = 4;
        break;
      }
    }
  }

  if (!resultCode)
  {
    for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
    {
      if (!g_Guild[guildIndex].IsFill() && !g_Guild[guildIndex].m_bDBWait)
      {
        g_Guild[guildIndex].m_nIndex = guildIndex;
        g_Guild[guildIndex].SetTemp(pwszGuildName);
        selectedGuildIndex = guildIndex;
        break;
      }
    }
    if (selectedGuildIndex == -1)
    {
      resultCode = 10;
    }
  }

  if (resultCode)
  {
    this->SendMsg_GuildEstablishFail(static_cast<char>(resultCode));
    return;
  }

  _qry_case_insertguild_local query{};
  query.in_guildindex = -1;
  query.out_guildserial = -1;
  query.tmp_Esterindex = -1;
  query.tmp_Esterserial = -1;
  query.in_guildindex = static_cast<unsigned int>(selectedGuildIndex);
  query.tmp_Esterindex = this->m_id.wIndex;
  query.tmp_Esterserial = this->m_id.dwSerial;
  query.in_guildRace = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
  query.in_membernum = memberCount;
  std::strcpy(query.in_w_guildName, pwszGuildName);
  std::memcpy(query.in_memberindex, memberIndices, sizeof(query.in_memberindex));
  std::memcpy(query.in_memberserial, memberSerials, sizeof(query.in_memberserial));
  std::memcpy(query.tmp_pvp, memberPvpPoints, sizeof(query.tmp_pvp));
  std::memcpy(query.tmp_lv, memberLevels, sizeof(query.tmp_lv));
  std::memcpy(query.tmp_grade, memberGrades, sizeof(query.tmp_grade));
  std::memcpy(query.tmp_w_membername, memberNames, sizeof(query.tmp_w_membername));

  if (!g_Main.PushDQSData(
        -1,
        nullptr,
        15,
        reinterpret_cast<char *>(&query),
        static_cast<int>(sizeof(query))))
  {
    return;
  }

  for (int partySlot = 0; partySlot < memberCount; ++partySlot)
  {
    CPlayer *member = &g_Player[partyMembers[partySlot]->m_wZoneIndex];
    member->m_Param.m_bGuildLock = true;
  }

  const unsigned int consumeDalant =
    static_cast<unsigned int>(g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant());
  this->SubDalant(consumeDalant);

  const int level = this->m_Param.GetLevel();
  if (IsEconomyFeeLevel(level))
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(this->m_Param.GetRaceCode()),
      level,
      consumeDalant);
  }
}

void CPlayer::pc_GuildDownLoadRequest()
{
  if (!this->m_Param.m_pGuild)
  {
    return;
  }

  this->m_bGuildDownload = true;
  if (this->m_Param.m_pGuildMemPtr)
  {
    this->m_Param.m_pGuild->SendMsg_DownPacket(1u, this->m_Param.m_pGuildMemPtr);
  }
}

void CPlayer::pc_GuildJoinApplyRequest(char *pwszGuildName)
{
  unsigned __int8 result = 0;
  CGuild *applyGuild = GetGuildPtrFromName(g_Guild, MAX_GUILD, pwszGuildName);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else
  {
    if (this->IsPunished(1u, true))
    {
      return;
    }

    if (!applyGuild)
    {
      result = static_cast<unsigned __int8>(-56);
    }
    else if (this->m_Param.m_bGuildLock)
    {
      result = static_cast<unsigned __int8>(-51);
    }
    else if (this->m_Param.m_pGuild)
    {
      result = 22;
    }
    else if (this->m_Param.m_pApplyGuild)
    {
      result = 23;
    }
    else if (applyGuild->m_nMemberNum >= 50)
    {
      result = 24;
    }
    else if (applyGuild->m_nApplierNum >= 32)
    {
      result = 25;
    }
    else if (this->m_Param.GetRaceCode() != static_cast<int>(applyGuild->GetRace()))
    {
      result = 70;
    }
    else if (g_Main.IsReleaseServiceMode() && this->m_byUserDgr)
    {
      result = static_cast<unsigned __int8>(-1);
    }
  }

  if (!result)
  {
    applyGuild->PushApplier(this);
    this->m_Param.m_pApplyGuild = applyGuild;
  }

  this->SendMsg_GuildJoinApplyResult(static_cast<char>(result), applyGuild);
}

void CPlayer::pc_GuildJoinApplyCancelRequest()
{
  unsigned __int8 result = 0;
  CGuild *applyGuild = this->m_Param.m_pApplyGuild;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else if (this->m_Param.m_bGuildLock)
  {
    result = static_cast<unsigned __int8>(-51);
  }
  else if (!applyGuild || !applyGuild->GetApplierFromSerial(this->m_dwObjSerial))
  {
    result = static_cast<unsigned __int8>(-52);
  }

  if (!result)
  {
    applyGuild->PopApplier(this->m_dwObjSerial, 2u);
    this->m_Param.m_pApplyGuild = nullptr;
  }

  this->SendMsg_GuildJoinApplyCancelResult(static_cast<char>(result));
}

void CPlayer::pc_GuildJoinAcceptRequest(unsigned int dwApplierSerial, bool bAccept)
{
  unsigned __int8 result = 0;
  CGuild *guild = this->m_Param.m_pGuild;
  _guild_applier_info *applierInfo = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else if (!guild)
  {
    result = static_cast<unsigned __int8>(-54);
  }
  else
  {
    applierInfo = guild->GetApplierFromSerial(dwApplierSerial);
    if (!applierInfo)
    {
      result = static_cast<unsigned __int8>(-52);
    }
    else if (!applierInfo->pPlayer)
    {
      result = 26;
    }
    else if (this->m_Param.m_pGuildMemPtr->byClassInGuild != 1
      && this->m_Param.m_pGuildMemPtr->byClassInGuild != 2)
    {
      result = static_cast<unsigned __int8>(-53);
    }
    else if (guild->GetMemberNumForJoin() >= 50)
    {
      result = static_cast<unsigned __int8>(-50);
    }
    else if (applierInfo->pPlayer->m_Param.GetRaceCode() != static_cast<int>(guild->GetRace()))
    {
      result = 70;
    }
    else if (guild->m_bRankWait)
    {
      result = 71;
    }
  }

  if (!result)
  {
    if (bAccept)
    {
      g_Main.m_logGuild.Write(
        "[%s] Join accept: approver=%s applicant=%s",
        guild->m_aszName,
        this->m_Param.GetCharNameA(),
        applierInfo->pPlayer->m_Param.GetCharNameA());

      _qry_case_joinacguild_local query{};
      query.in_guildindex = guild->m_nIndex;
      query.in_guildserial = guild->m_dwSerial;
      query.in_applierindex = applierInfo->pPlayer->m_ObjID.m_wIndex;
      query.in_applierserial = applierInfo->pPlayer->m_dwObjSerial;
      query.in_accepterserial = this->m_dwObjSerial;
      query.in_Grade = 0;
      query.in_MemberNum = static_cast<int>(guild->GetMemberNum()) + 1;

      if (g_Main.PushDQSData(-1, &this->m_id, 16, reinterpret_cast<char *>(&query), static_cast<int>(sizeof(query))))
      {
        applierInfo->pPlayer->m_Param.m_bGuildLock = true;
        ++guild->m_nTempMemberNum;
      }
      else
      {
        result = static_cast<unsigned __int8>(-1);
      }
    }
    else
    {
      applierInfo->pPlayer->SendMsg_GuildJoinApplyRejectInform();
      applierInfo->pPlayer->m_Param.m_pApplyGuild = nullptr;
      guild->PopApplier(dwApplierSerial, 1u);
    }
  }

  if (result)
  {
    this->SendMsg_GuildJoinAcceptFail(static_cast<char>(result), dwApplierSerial);
  }
}

void CPlayer::pc_GuildPushMoneyRequest(unsigned int dwPushDalant, unsigned int dwPushGold)
{
  unsigned __int8 result = 0;
  CGuild *guild = this->m_Param.m_pGuild;

  if (!guild || !guild->GetMemberFromSerial(this->m_dwObjSerial))
  {
    result = static_cast<unsigned __int8>(-54);
  }
  else if (guild->m_bIOWait)
  {
    result = 103;
  }
  else if (dwPushDalant > 3000000000 || dwPushGold > 3000000000)
  {
    result = 101;
  }
  else if (this->m_Param.GetDalant() < dwPushDalant || this->m_Param.GetGold() < dwPushGold)
  {
    result = 102;
  }
  else
  {
    const double totalDalant = static_cast<double>(guild->GetTotalDalant()) + static_cast<double>(dwPushDalant);
    const double totalGold = static_cast<double>(guild->GetTotalGold()) + static_cast<double>(dwPushGold);
    if (totalDalant > 1000000000.0 || totalGold > static_cast<double>(MAX_GOLD))
    {
      result = 104;
    }
  }

  if (!result)
  {
    _qry_case_inputgmoney query{};
    query.in_pusherserial = this->m_id.dwSerial;
    query.tmp_guildindex = guild->m_nIndex;
    query.in_guildserial = guild->m_dwSerial;
    query.dwAddGold = dwPushGold;
    query.dwAddDalant = dwPushDalant;
    query.in_date[0] = GetCurrentMonth();
    query.in_date[1] = GetCurrentDay();
    query.in_date[2] = GetCurrentHour();
    query.in_date[3] = GetCurrentMin();
    std::strcpy(query.in_w_pushername, this->m_Param.GetCharNameW());

    if (g_Main.PushDQSData(-1, nullptr, 19, reinterpret_cast<char *>(&query), static_cast<int>(query.size())))
    {
      guild->m_bIOWait = true;
      this->SubDalant(dwPushDalant);
      this->SubGold(dwPushGold);
      this->SendMsg_AlterMoneyInform(0);
    }
    else
    {
      result = static_cast<unsigned __int8>(-1);
    }
  }

  this->SendMsg_GuildPushMoneyResult(static_cast<char>(result));
}

void CPlayer::pc_GuildRoomRentRequest(_guildroom_rent_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 0;
  const unsigned __int8 roomType = pProtocol ? pProtocol->byRoomType : 0;

  if (roomType >= 2u)
  {
    retCode = 7;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    retCode = 6;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  if (this->m_Param.m_byClassInGuild != 1 && this->m_Param.m_byClassInGuild != 2)
  {
    retCode = 2;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (guild->m_bIOWait)
  {
    retCode = 11;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (static_cast<long double>(CGuildRoomInfo::sm_RoomInfo[roomType].dwCost) > guild->GetTotalDalant())
  {
    retCode = 8;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (guild->GetGrade() < CGuildRoomInfo::sm_RoomInfo[roomType].byRestrict)
  {
    retCode = 9;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (roomSystem->IsRoomRented(pProtocol->dwGuildSerial))
  {
    retCode = 3;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_pUserDB->m_AvatorData.dbAvator.m_byRaceSexCode >> 1);
  subRetCode = static_cast<unsigned __int8>(
    roomSystem->RentRoom(raceCode, roomType, guild->m_nIndex, guild->m_dwSerial, nullptr, false));
  if (subRetCode)
  {
    retCode = 5;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  retCode = 10;

  _qry_case_outputgmoney query{};
  query.in_poperserial = 0;
  query.tmp_guildindex = guild->m_nIndex;
  query.in_guildserial = guild->m_dwSerial;
  query.dwSubDalant = CGuildRoomInfo::sm_RoomInfo[roomType].dwCost;
  query.dwSubGold = 0;
  std::memset(&query.out_totalgold, 0, 17);
  query.in_date[0] = static_cast<unsigned __int8>(GetCurrentMonth());
  query.in_date[1] = static_cast<unsigned __int8>(GetCurrentDay());
  query.in_date[2] = static_cast<unsigned __int8>(GetCurrentHour());
  query.in_date[3] = static_cast<unsigned __int8>(GetCurrentMin());
  std::strcpy(query.in_w_popername, "GuildRoom Rent");
  g_Main.PushDQSData(
    -1,
    nullptr,
    20,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));

  CMapData *roomMap = roomSystem->GetMapData(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()), roomType);
  if (roomMap)
  {
    CMapItemStoreList *destStoreList = CItemStoreManager::Instance()->GetMapItemStoreListBySerial(roomMap->m_nMapIndex);
    CMapItemStoreList *emptyInstanceStore = CItemStoreManager::Instance()->GetEmptyInstanceItemStore();
    if (destStoreList && emptyInstanceStore && emptyInstanceStore->CopyItemStoreData(destStoreList))
    {
      emptyInstanceStore->SetTypeNSerial(1u, static_cast<int>(guild->m_dwSerial));
    }
  }

  this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
  guild->SendMsg_GuildRoomRented(static_cast<char>(roomType));
}

void CPlayer::pc_GuildRoomEnterRequest(_guildroom_enter_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 3;
  unsigned __int8 mapIndex = 0;
  unsigned __int16 mapLayer = 0;
  float startPos[3] = {0.0f, 0.0f, 0.0f};
  int restTime[5]{};

  if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    retCode = 5;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (this->IsRidingUnit())
  {
    retCode = 6;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (this->m_byStandType == 1)
  {
    retCode = 7;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    retCode = 1;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (!roomSystem->IsRoomRented(guild->m_dwSerial))
  {
    retCode = 2;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  // Yorozuya fix implementation (non-IDA): require the player to be near the guild-room NPC
  // before allowing the guild hall teleport instead of trusting the client-side UI restriction.
  if (!IsNearGuildRoomNpc(this))
  {
    retCode = 5;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  subRetCode = static_cast<unsigned __int8>(
    roomSystem->RoomIn(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial));
  if (subRetCode)
  {
    retCode = 3;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CMapData *mapDummy = nullptr;
  unsigned __int8 roomType = 2;
  if (!roomSystem->GetMapPos(guild->m_dwSerial, startPos, mapDummy, &mapLayer, &roomType))
  {
    roomSystem->RoomOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial);
    retCode = 4;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CMapData *roomMap = roomSystem->GetMapData(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()), roomType);
  if (!roomMap)
  {
    roomSystem->RoomOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial);
    retCode = 4;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  retCode = 10;
  this->OutOfMap(roomMap, mapLayer, 3u, startPos);
  mapIndex = static_cast<unsigned __int8>(roomMap->m_pMapSet->m_dwIndex);
  roomSystem->GetRestTime(guild->m_dwSerial, restTime);
  this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
}

void CPlayer::pc_GuildRoomOutRequest(_guildroom_out_request_clzo *pProtocol)
{
  if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    this->SendMsg_GuildRoomOutResult(4u, 0, 0, nullptr);
    return;
  }

  if (this->IsRidingUnit())
  {
    this->SendMsg_GuildRoomOutResult(5u, 0, 0, nullptr);
    return;
  }

  if (this->m_byStandType == 1)
  {
    this->SendMsg_GuildRoomOutResult(6u, 0, 0, nullptr);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    this->SendMsg_GuildRoomOutResult(1u, 0, 0, nullptr);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (!roomSystem->IsRoomRented(guild->m_dwSerial)
    || !roomSystem->SetPlayerOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial))
  {
    this->SendMsg_GuildRoomOutResult(2u, 0, 0, nullptr);
  }
}

void CPlayer::pc_GuildRoomRestTimeRequest(_guildroom_resttime_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 3;

  if (this->m_Param.m_pGuild && pProtocol && pProtocol->dwGuildSerial == this->m_Param.m_pGuild->m_dwSerial)
  {
    CGuild *guild = this->m_Param.m_pGuild;
    CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
    if (!roomSystem->IsRoomRented(guild->m_dwSerial))
    {
      retCode = 2;
    }
    this->SendMsg_GuildRoomRestTimeResult();
  }
  else
  {
    retCode = 1;
    this->SendMsg_GuildRoomRestTimeResult();
  }

  (void)retCode;
  (void)subRetCode;
}

void CPlayer::pc_GuildSetHonorRequest(_guild_honor_set_request_clzo *pData)
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial =
    CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);

  if (charSerial == raceBossSerial)
  {
    const unsigned __int8 result =
      CHonorGuild::Instance()->SetNextHonorGuild(static_cast<unsigned __int8>(raceCode), pData);
    this->SendMsg_GuildSetHonorResult(static_cast<char>(result));
  }
  else
  {
    this->SendMsg_GuildSetHonorResult(1u);
  }
}


void CPlayer::pc_GuildHonorListRequest(unsigned __int8 byUI)
{
  const int raceCode = this->m_Param.GetRaceCode();
  CHonorGuild::Instance()->SendCurrHonorGuildList(this->m_ObjID.m_wIndex, static_cast<unsigned __int8>(raceCode), byUI);
}





void CPlayer::pc_GuildQueryInfoRequest(unsigned int dwGuildSerial)
{
  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    CGuild &guild = g_Guild[guildIndex];
    if (!guild.IsFill() || guild.m_dwSerial != dwGuildSerial)
    {
      continue;
    }

    guild.SendMsg_QueryPacket_Info(this->m_ObjID.m_wIndex);
    return;
  }
}

void CPlayer::pc_GuildListRequest(unsigned __int8 byPage)
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
    static_cast<unsigned __int8>(raceCode),
    0);

  if (charSerial != raceBossSerial)
  {
    return;
  }

  CGuild::s_GuildList.SendList(
    this->m_ObjID.m_wIndex,
    static_cast<unsigned __int8>(this->m_Param.GetRaceCode()),
    byPage);
}

void CPlayer::pc_GuildNextHonorListRequest()
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
    static_cast<unsigned __int8>(raceCode),
    0);

  if (charSerial != raceBossSerial || raceCode < 0 || raceCode >= 3)
  {
    return;
  }

  CHonorGuild::Instance()->SendNextHonorGuildList(
    this->m_ObjID.m_wIndex,
    static_cast<unsigned __int8>(this->m_Param.GetRaceCode()));
}


void CPlayer::pc_GuildCancelSuggestRequest(unsigned int dwMatterVoteSynKey)
{
  unsigned __int8 resultCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    resultCode = static_cast<unsigned __int8>(-54);
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  else if (guild->m_dwSuggesterSerial != m_dwObjSerial)
  {
    resultCode = static_cast<unsigned __int8>(-53);
  }

  if (!resultCode)
    guild->CancelSuggestedMatter();

  this->SendMsg_CancelSuggestResult(static_cast<char>(resultCode));
}

void CPlayer::pc_GuildVoteRequest(unsigned int dwMatterVoteSynKey, unsigned __int8 byVoteCode)
{
  unsigned __int8 resultCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    resultCode = static_cast<unsigned __int8>(-54);
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  else if (!guild->m_bNowProcessSgtMter)
  {
    resultCode = 62;
  }
  else if (guild->m_SuggestedMatter.dwMatterVoteSynKey != dwMatterVoteSynKey)
  {
    resultCode = 63;
  }
  else if (!guild->m_SuggestedMatter.IsVotable(m_dwObjSerial))
  {
    resultCode = static_cast<unsigned __int8>(-53);
  }
  else if (m_Param.m_pGuildMemPtr->bVote)
  {
    resultCode = 66;
  }

  if (!resultCode)
    guild->ActVote(m_Param.m_pGuildMemPtr, byVoteCode);

  this->SendMsg_VoteResult(dwMatterVoteSynKey, resultCode);
}


void CPlayer::pc_GuildOfferSuggestRequest(
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *pwszComment,
  unsigned int dwMatterObj1,
  unsigned int dwMatterObj2,
  unsigned int dwMatterObj3)
{
  unsigned __int8 byRetCode = 0;
  CGuild *guild = m_Param.m_pGuild;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    byRetCode = 106;
  }
  else if (!guild)
  {
    byRetCode = static_cast<unsigned __int8>(-54);
  }
  else if (guild->m_bNowProcessSgtMter)
  {
    byRetCode = 61;
  }
  else if (guild->m_bRankWait)
  {
    byRetCode = 71;
  }
  else if (!guild->GetMemberFromSerial(dwMatterDst))
  {
    byRetCode = 63;
  }
  else if (guild->m_MasterData.IsFill() && guild->m_MasterData.dwSerial == dwMatterDst)
  {
    byRetCode = 67;
  }
  else if (!guild->m_bPossibleElectMaster)
  {
    byRetCode = static_cast<unsigned __int8>(-78);
  }

  if (!byRetCode
      && !guild->RegSuggestedMatter(
        m_dwObjSerial,
        byMatterType,
        dwMatterDst,
        pwszComment,
        dwMatterObj1,
        dwMatterObj2,
        dwMatterObj3))
  {
    byRetCode = 64;
  }

  this->SendMsg_OfferSuggestResult(static_cast<char>(byRetCode));
}

