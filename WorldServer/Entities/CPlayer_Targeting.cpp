#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "CGameObject.h"
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
#include "EconomySystemFunctions.h"
#include "TimeItem.h"
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
#include "WorldServerUtil.h"
#include "NetCheckPackets.h"
#include "GlobalObjectDefs.h"
#include "Packet/ZoneClientPacket.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace
{
bool IsTargetingObject(CGameObject *target)
{
  if (!target)
  {
    return false;
  }

  if (target->m_ObjID.m_byKind == 0 && target->m_ObjID.m_byID == 0)
  {
    CPlayer *targetPlayer = static_cast<CPlayer *>(target);
    if (targetPlayer->m_EP.GetEff_State(21))
    {
      return false;
    }
  }

  if (target->m_ObjID.m_byKind == 0)
  {
    return true;
  }

  return target->m_ObjID.m_byKind == 1 && target->m_ObjID.m_byID == 2;
}

bool IsCircleObjectCompat(CGameObject *viewer, int range, CGameObject *target)
{
  _sec_info *secInfo = viewer->m_pCurMap->GetSecInfo();
  _pnt_rect rect{};
  viewer->m_pCurMap->GetRectInRadius(&rect, range, viewer->m_dwCurSec);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = viewer->m_pCurMap->GetSectorListObj(viewer->m_wMapLayerIndex, secIndex);
      if (sectorList)
      {
        for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
        {
          if (target == node->m_pItem)
          {
            return true;
          }
        }
      }
    }
  }

  return false;
}
}

void CPlayer::SendData_PartyMemberHP()
{
  if (!this->m_pPartyMgr)
  {
    return;
  }

  CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  _party_member_hp_upd packet{};
  packet.dwMemSerial = this->m_dwObjSerial;
  packet.wHPRate = this->m_wPointRate_PartySend[0];

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 20};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
    }
  }
}

void CPlayer::SendData_PartyMemberFP()
{
  if (!this->m_pPartyMgr)
  {
    return;
  }

  CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  _party_member_fp_upd packet{};
  packet.dwMemSerial = this->m_dwObjSerial;
  packet.wFPRate = this->m_wPointRate_PartySend[1];

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 21};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
    }
  }
}

void CPlayer::SendData_PartyMemberSP()
{
  if (!this->m_pPartyMgr)
  {
    return;
  }

  CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  _party_member_sp_upd packet{};
  packet.dwMemSerial = this->m_dwObjSerial;
  packet.wSPRate = this->m_wPointRate_PartySend[2];

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 22};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
    }
  }
}

void CPlayer::SendData_PartyMemberEffect(unsigned __int8 byAlterCode, unsigned __int16 wEffectCode, unsigned __int8 byLv)
{
  if (!this->m_pPartyMgr)
  {
    return;
  }

  CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  _party_member_effect_upd packet{};
  packet.dwMemSerial = this->m_dwObjSerial;
  packet.byAlterCode = static_cast<char>(byAlterCode);
  packet.wEffectCode = wEffectCode;
  packet.byEffectLv = static_cast<char>(byLv);

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 26};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
    }
  }
}

void CPlayer::SendMsg_BuddyLogoffInform(unsigned int dwObjSerial)
{

  _buddy_off_reged_player_inform_zocl msg{};
  msg.dwSerial = dwObjSerial;

  unsigned __int8 type[2] = {31, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 4u);
}

void CPlayer::SendMsg_BuddyLoginInform(unsigned int dwObjSerial, unsigned __int8 byMapIndex, unsigned __int8 byPosCode)
{

  _buddy_in_reged_player_inform_zocl msg{};
  msg.dwSerial = dwObjSerial;
  msg.byMapIndex = byMapIndex;
  msg.byPositionCode = byPosCode;

  unsigned __int8 type[2] = {31, 1};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 6u);
}

void CPlayer::SendMsg_BuddyAddFail(char byRetCode, char *pwszDstName)
{

  _buddy_add_fail_zocl msg{};
  msg.byRetCode = byRetCode;
  strcpy_0(msg.wszDstName, pwszDstName);

  unsigned __int8 type[2] = {31, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_BuddyAddAsk(
  unsigned __int16 wAskerIndex,
  unsigned int dwAskerSerial,
  char *pwszAskerName)
{

  _buddy_add_ask_zocl msg{};
  msg.wAskerIndex = wAskerIndex;
  msg.dwAskerSerial = dwAskerSerial;
  strcpy_0(msg.wszAskerName, pwszAskerName);

  unsigned __int8 type[2] = {31, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_BuddyAddAnswerResult(
  char byRetCode,
  char bAccept,
  unsigned int dwAskerSerial,
  unsigned __int16 wIndex,
  unsigned int dwSerial,
  char *pwszCharName)
{

  _buddy_add_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.bAccept = bAccept != 0;
  msg.dwAskerSerial = dwAskerSerial;
  msg.wAdderIndex = wIndex;
  msg.dwAdderSerial = dwSerial;
  if (!byRetCode)
  {
    memcpy_0(msg.wszAdderName, pwszCharName, 16u);
    msg.wszAdderName[16] = 0;
  }

  unsigned __int8 type[2] = {31, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyLeaveSelfResult(CPartyPlayer *pLeaver, bool bWorldExit)
{

  _party_leave_self_result_zocl msg{};
  msg.bWorldExit = bWorldExit;
  msg.dwExiterSerial = pLeaver ? pLeaver->m_id.dwSerial : static_cast<unsigned int>(-1);

  unsigned __int8 type[2] = {16, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 5u);
}

void CPlayer::SendMsg_PartySuccessResult(CPartyPlayer *pSuccessor)
{
  _party_succession_result_zocl msg{};
  msg.dwSuccessorSerial = pSuccessor ? pSuccessor->m_id.dwSerial : static_cast<unsigned int>(-1);

  unsigned __int8 type[2] = {16, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyJoinInvitationQuestion(unsigned __int16 wJoinerIndex)
{

  _party_join_invitation_question_zocl msg{};
  msg.idBoss.wIndex = m_ObjID.m_wIndex;
  msg.idBoss.dwSerial = m_dwObjSerial;

  unsigned __int8 type[2] = {16, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(wJoinerIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyJoinApplicationQuestion(CPlayer *pJoiner)
{

  _party_join_application_question_zocl msg{};
  msg.idApplicant.wIndex = pJoiner->m_ObjID.m_wIndex;
  msg.idApplicant.dwSerial = pJoiner->m_dwObjSerial;

  unsigned __int8 type[2] = {16, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyJoinJoinerResult()
{


  CPartyPlayer **partyMembers = m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  _party_join_joiner_result_zocl msg{};
  msg.byLootShareMode = m_pPartyMgr->m_pPartyBoss->m_byLootShareSystem;
  msg.byListNum = static_cast<unsigned __int8>(m_pPartyMgr->GetPopPartyMember() - 1);
  for (int index = 0; index < msg.byListNum && partyMembers[index]; ++index)
  {
    if (partyMembers[index] != m_pPartyMgr)
    {
      msg.List[index].wIndex = partyMembers[index]->m_id.wIndex;
      msg.List[index].dwSerial = partyMembers[index]->m_id.dwSerial;
      strcpy_0(msg.List[index].wszAvatorName, partyMembers[index]->m_wszName);
    }
  }

  unsigned __int16 msgLength = static_cast<unsigned __int16>(
    2 + msg.byListNum * sizeof(_party_join_joiner_result_zocl::_list));
  unsigned __int8 type[2] = {16, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(
    m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    msgLength);
}

void CPlayer::SendMsg_PartyJoinMemberResult(CPartyPlayer *pJoiner, char byLootShareMode)
{

  _party_join_member_result_zocl msg{};
  msg.dwJoinerSerial = pJoiner->m_id.dwSerial;
  strcpy_0(msg.wszJoinerName, pJoiner->m_wszName);
  msg.byLootShareMode = static_cast<unsigned __int8>(byLootShareMode);
  msg.wIndex = pJoiner->m_id.wIndex;

  unsigned __int8 type[2] = {16, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyJoinFailLevel()
{
  _party_join_fail_level_inform_zocl msg{};
  unsigned __int8 type[2] = {16, 66};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AwayPartyRequestResult(char byRetCode)
{
  _away_party_invitation_result_zocl msg{};
  msg.byRetCode = byRetCode;
  unsigned __int8 type[2] = {16, 32};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_AwayPartyInvitationQuestion(unsigned __int16 wJoinerIndex)
{

  _away_party_join_invitation_question_zocl msg{};
  msg.idBoss.wIndex = m_ObjID.m_wIndex;
  msg.idBoss.dwSerial = m_dwObjSerial;
  strcpy_0(msg.wszCharName, m_Param.GetCharNameA());

  unsigned __int8 type[2] = {16, 33};
  g_Network.m_pProcess[0]->LoadSendMsg(wJoinerIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyLeaveCompulsionResult(CPartyPlayer *pLeaver)
{

  _party_leave_compulsion_result_zocl msg{};
  msg.dwExiterSerial = pLeaver ? pLeaver->m_id.dwSerial : static_cast<unsigned int>(-1);

  unsigned __int8 type[2] = {16, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyDisjointResult(char bSuccess)
{
  _party_disjoint_result_zocl msg{};
  msg.bySuccess = bSuccess;
  unsigned __int8 type[2] = {16, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyLockResult(char byRet)
{
  _party_lock_result_zocl msg{};
  msg.byLock = byRet;
  unsigned __int8 type[2] = {16, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_PartyAlterLootShareResult(char byLootShareMode)
{
  _alter_party_loot_share_result_zocl msg{};
  msg.byLootShareMode = byLootShareMode;
  unsigned __int8 type[2] = {16, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendData_PartyMemberInfo(unsigned __int16 wDstIndex)
{


  _party_member_info_upd msg{};
  msg.dwMemSerial = m_dwObjSerial;
  msg.wHPRate = static_cast<unsigned __int16>(CalcCurHPRate());
  msg.wFPRate = static_cast<unsigned __int16>(CalcCurFPRate());
  msg.wSPRate = static_cast<unsigned __int16>(CalcCurSPRate());
  msg.byLv = static_cast<unsigned __int8>(m_Param.GetLevel());
  msg.byMapCode = static_cast<unsigned __int8>(m_Param.GetMapCode());
  msg.zPos[0] = static_cast<__int16>(m_fCurPos[0]);
  msg.zPos[1] = static_cast<__int16>(m_fCurPos[2]);

  unsigned __int8 effectCount = 0;
  for (int row = 0; row < 2; ++row)
  {
    for (int col = 0; col < 8; ++col)
    {
      _sf_continous *cont = &m_SFCont[row][col];
      if (cont->m_bExist)
      {
        msg.Effect[effectCount].wEffectCode = CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex);
        msg.Effect[effectCount].byEffectLv = cont->m_byLv;
        ++effectCount;
      }
    }
  }

  msg.byContEffectNum = effectCount;

  const unsigned __int16 msgLength = static_cast<unsigned __int16>(17 + effectCount * sizeof(_party_member_info_upd::_EFFECT));
  unsigned __int8 type[2] = {16, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(wDstIndex, type, reinterpret_cast<char *>(&msg), msgLength);
}

void CPlayer::SendData_PartyMemberInfoToMembers()
{


  _party_member_info_upd msg{};
  msg.dwMemSerial = m_dwObjSerial;
  msg.wHPRate = static_cast<unsigned __int16>(CalcCurHPRate());
  msg.wFPRate = static_cast<unsigned __int16>(CalcCurFPRate());
  msg.wSPRate = static_cast<unsigned __int16>(CalcCurSPRate());
  msg.byLv = static_cast<unsigned __int8>(m_Param.GetLevel());
  msg.byMapCode = static_cast<unsigned __int8>(m_Param.GetMapCode());
  msg.zPos[0] = static_cast<__int16>(m_fCurPos[0]);
  msg.zPos[1] = static_cast<__int16>(m_fCurPos[2]);

  unsigned __int8 effectCount = 0;
  for (int row = 0; row < 2; ++row)
  {
    for (int col = 0; col < 8; ++col)
    {
      _sf_continous *cont = &m_SFCont[row][col];
      if (cont->m_bExist)
      {
        msg.Effect[effectCount].wEffectCode = CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex);
        msg.Effect[effectCount].byEffectLv = cont->m_byLv;
        ++effectCount;
      }
    }
  }

  msg.byContEffectNum = effectCount;

  CPartyPlayer **partyMembers = m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  const int popPartyMember = static_cast<int>(m_pPartyMgr->GetPopPartyMember());
  const unsigned __int16 msgLength = static_cast<unsigned __int16>(17 + effectCount * sizeof(_party_member_info_upd::_EFFECT));
  unsigned __int8 type[2] = {16, 19};
  for (int index = 0; index < popPartyMember; ++index)
  {
    if (partyMembers[index] != m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex,
        type,
        reinterpret_cast<char *>(&msg),
        msgLength);
    }
  }
}

void CPlayer::pc_PartyJoinInvitation(unsigned __int16 wDstIndex)
{
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *targetPlayer = &g_Player[wDstIndex];
  if (!targetPlayer->m_bLive || targetPlayer->m_bCorpse || targetPlayer->m_pCurMap != m_pCurMap)
  {
    return;
  }
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(targetPlayer->m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(3u);
    return;
  }
  if (targetPlayer->IsPunished(2u, false))
  {
    return;
  }
  if (targetPlayer->m_EP.GetEff_Have(50) > 0.0f)
  {
    SendMsg_JadeEffectErr(1u);
    return;
  }

  const int sourceRaceCode = m_Param.GetRaceCode();
  const int targetRaceCode = targetPlayer->m_Param.GetRaceCode();
  if (sourceRaceCode != targetRaceCode)
  {
    return;
  }
  if (targetPlayer->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      return;
    }
  }
  else if (m_byUserDgr)
  {
    return;
  }

  __int16 *dstBillingType = &targetPlayer->m_pUserDB->m_BillingInfo.iType;
  __int16 *srcBillingType = &m_pUserDB->m_BillingInfo.iType;
  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (nationSetting->IsPersonalFreeFixedAmountBillingType(srcBillingType, dstBillingType)
      || targetPlayer->m_pPartyMgr->IsPartyMode()
      || m_pPartyMgr->m_bLock)
  {
    return;
  }

  const float profileBonus = targetPlayer->m_EP.GetEff_Have(53);
  const int targetLevel = static_cast<int>(targetPlayer->GetLevel());
  if (!m_pPartyMgr->IsJoinPartyLevel(targetLevel, profileBonus))
  {
    SendMsg_PartyJoinFailLevel();
    return;
  }

  if (m_pPartyMgr->IsPartyBoss() || !m_pPartyMgr->IsPartyMode())
  {
    SendMsg_PartyJoinInvitationQuestion(wDstIndex);
  }
}

void CPlayer::pc_PartyJoinInvitationAnswer(_CLID *pidBoss)
{
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *partyBoss = &g_Player[pidBoss->wIndex];
  if (partyBoss->m_dwObjSerial != pidBoss->dwSerial || !partyBoss->m_bLive || partyBoss->IsPunished(2u, false))
  {
    return;
  }

  const int sourceRaceCode = m_Param.GetRaceCode();
  const int bossRaceCode = partyBoss->m_Param.GetRaceCode();
  if (sourceRaceCode != bossRaceCode)
  {
    return;
  }
  if (partyBoss->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      return;
    }
  }
  else if (m_byUserDgr)
  {
    return;
  }

  if (m_pPartyMgr->IsPartyMode() || partyBoss->m_pPartyMgr->m_bLock)
  {
    return;
  }

  const float profileBonus = m_EP.GetEff_Have(53);
  const int myLevel = static_cast<int>(GetLevel());
  if (!partyBoss->m_pPartyMgr->IsJoinPartyLevel(myLevel, profileBonus))
  {
    SendMsg_PartyJoinFailLevel();
    return;
  }

  wa_PartyJoin(&partyBoss->m_id, &m_id);
  if (m_pPartyMgr->IsPartyMode() && partyBoss->m_GroupTargetObject[0].pObject)
  {
    m_GroupTargetObject[0].init();
    m_GroupTargetObject[0].pObject = partyBoss->m_GroupTargetObject[0].pObject;
    m_GroupTargetObject[0].byKind = partyBoss->m_GroupTargetObject[0].byKind;
    m_GroupTargetObject[0].byID = partyBoss->m_GroupTargetObject[0].byID;
    m_GroupTargetObject[0].dwSerial = partyBoss->m_GroupTargetObject[0].dwSerial;
    SendMsg_SetGroupTargetObjectResult(0, 0);
  }
}

void CPlayer::pc_PartyJoinApplication(unsigned __int16 wBossIndex)
{
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *partyBoss = &g_Player[wBossIndex];
  if (!partyBoss->m_bLive || partyBoss->m_bCorpse)
  {
    return;
  }
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(partyBoss->m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(3u);
    return;
  }
  if (partyBoss->IsPunished(2u, false))
  {
    return;
  }

  const int sourceRaceCode = m_Param.GetRaceCode();
  const int bossRaceCode = partyBoss->m_Param.GetRaceCode();
  if (sourceRaceCode != bossRaceCode || m_pPartyMgr->IsPartyMode() || !partyBoss->m_pPartyMgr->IsPartyBoss())
  {
    return;
  }
  if (partyBoss->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      return;
    }
  }
  else if (m_byUserDgr)
  {
    return;
  }

  __int16 *srcBillingType = &m_pUserDB->m_BillingInfo.iType;
  CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
  if (nationSetting->IsPersonalFreeFixedAmountBillingType(srcBillingType, nullptr) || partyBoss->m_pPartyMgr->m_bLock)
  {
    return;
  }

  const float profileBonus = m_EP.GetEff_Have(53);
  const int myLevel = static_cast<int>(GetLevel());
  if (partyBoss->m_pPartyMgr->IsJoinPartyLevel(myLevel, profileBonus))
  {
    partyBoss->SendMsg_PartyJoinApplicationQuestion(this);
  }
  else
  {
    SendMsg_PartyJoinFailLevel();
  }
}

void CPlayer::pc_PartyJoinApplicationAnswer(_CLID *pidApplicant)
{
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    SendMsg_TLStatusPenalty(1u);
    return;
  }
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *applicant = &g_Player[pidApplicant->wIndex];
  if (applicant->m_id.dwSerial != pidApplicant->dwSerial
      || !applicant->m_bLive
      || applicant->m_bCorpse
      || applicant->m_pCurMap != m_pCurMap
      || applicant->IsPunished(2u, false))
  {
    return;
  }

  const int sourceRaceCode = m_Param.GetRaceCode();
  const int applicantRaceCode = applicant->m_Param.GetRaceCode();
  if (sourceRaceCode != applicantRaceCode)
  {
    return;
  }
  if (applicant->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      return;
    }
  }
  else if (m_byUserDgr)
  {
    return;
  }
  if (m_pPartyMgr->m_bLock)
  {
    return;
  }

  const float profileBonus = applicant->m_EP.GetEff_Have(53);
  const int applicantLevel = static_cast<int>(applicant->GetLevel());
  if (m_pPartyMgr->IsJoinPartyLevel(applicantLevel, profileBonus))
  {
    wa_PartyJoin(&m_id, &applicant->m_id);
  }
  else
  {
    SendMsg_PartyJoinFailLevel();
  }
}

void CPlayer::pc_PartyLeaveCompulsionReqeuest(unsigned int dwExiterSerial)
{
  CPartyPlayer *exiter = nullptr;
  if (m_pPartyMgr->IsPartyBoss() && (exiter = m_pPartyMgr->GetPtrFromSerial(dwExiterSerial)) != nullptr)
  {
    wa_PartyForceLeave(&m_id, &exiter->m_id);
  }
  else
  {
    SendMsg_PartyLeaveCompulsionResult(nullptr);
  }
}

void CPlayer::pc_PartyDisJointReqeuest()
{
  if (m_pPartyMgr->IsPartyBoss())
  {
    wa_PartyDisjoint(&m_id);
  }
  else
  {
    SendMsg_PartyDisjointResult(0);
  }
}

void CPlayer::pc_PartySuccessionReqeuest(unsigned int dwSuccessorSerial)
{
  CPartyPlayer *successor = nullptr;
  if (m_pPartyMgr->IsPartyBoss() && (successor = m_pPartyMgr->GetPtrFromSerial(dwSuccessorSerial)) != nullptr)
  {
    wa_PartySuccession(&m_id, &successor->m_id);
  }
  else
  {
    SendMsg_PartySuccessResult(nullptr);
  }
}

void CPlayer::pc_PartyLockReqeuest(bool bLock)
{
  if (!m_pPartyMgr->IsPartyBoss() || m_pPartyMgr->m_bLock == bLock)
  {
    SendMsg_PartyLockResult(static_cast<char>(255));
  }
  else
  {
    wa_PartyLock(&m_id, bLock);
  }
}

void CPlayer::pc_PartyAlterLootShareReqeuest(unsigned __int8 byLootShareMode)
{
  if (m_pPartyMgr->IsPartyBoss())
  {
    wa_PartyLootShareSystem(&m_id, byLootShareMode);
  }
  else
  {
    SendMsg_PartyAlterLootShareResult(static_cast<char>(255));
  }
}

void CPlayer::pc_AwaypartyInvitationRequest(char *pwszCharName)
{
  if (IsPunished(2u, true) || g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    return;
  }

  CPlayer *targetPlayer = g_Main.GetCharW(pwszCharName);
  if (!targetPlayer || targetPlayer->m_dwObjSerial == m_dwObjSerial)
  {
    SendMsg_AwayPartyRequestResult(1u);
    return;
  }
  if (targetPlayer->m_Param.GetRaceCode() != m_Param.GetRaceCode())
  {
    SendMsg_AwayPartyRequestResult(2u);
    return;
  }
  if (!targetPlayer->m_bOper || targetPlayer->m_bCorpse)
  {
    SendMsg_AwayPartyRequestResult(3u);
    return;
  }
  if (targetPlayer->IsPunished(2u, false))
  {
    return;
  }
  if (targetPlayer->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      SendMsg_AwayPartyRequestResult(3u);
      return;
    }
  }
  else if (m_byUserDgr)
  {
    SendMsg_AwayPartyRequestResult(3u);
    return;
  }

  if (targetPlayer->m_pPartyMgr->IsPartyMode())
  {
    SendMsg_AwayPartyRequestResult(4u);
    return;
  }
  if (m_pPartyMgr->IsPartyMode() && !m_pPartyMgr->IsPartyBoss())
  {
    SendMsg_AwayPartyRequestResult(8u);
    return;
  }
  if (m_pPartyMgr->m_bLock)
  {
    SendMsg_AwayPartyRequestResult(7u);
    return;
  }

  const float profileBonus = targetPlayer->m_EP.GetEff_Have(53);
  const int targetLevel = static_cast<int>(targetPlayer->GetLevel());
  if (!m_pPartyMgr->IsJoinPartyLevel(targetLevel, profileBonus))
  {
    SendMsg_AwayPartyRequestResult(5u);
    return;
  }
  if (targetPlayer->m_bBlockParty)
  {
    SendMsg_AwayPartyRequestResult(6u);
    return;
  }
  if (g_Main.m_bAwayPartyConsumeItem
      && targetPlayer->m_Param.m_dbInven.GetPtrFromItemCode(g_Main.m_strAwayPartyItemCode) == nullptr)
  {
    SendMsg_AwayPartyRequestResult(9u);
    return;
  }
  if (g_Main.m_bAwayPartyConsumeMoney && g_Main.m_dwAwayPartyMoney > targetPlayer->GetMoney(0))
  {
    SendMsg_AwayPartyRequestResult(11u);
    return;
  }

  SendMsg_AwayPartyRequestResult(0);
  SendMsg_AwayPartyInvitationQuestion(targetPlayer->m_ObjID.m_wIndex);
}

void CPlayer::pc_AwayPartyJoinInvitationAnswer(_CLID *pidBoss, unsigned __int8 byRetCode)
{
  if (IsPunished(2u, true))
  {
    return;
  }

  CPlayer *partyBoss = &g_Player[pidBoss->wIndex];
  if (partyBoss->m_dwObjSerial != pidBoss->dwSerial || !partyBoss->m_bLive || !partyBoss->m_bOper)
  {
    return;
  }
  if (m_Param.GetRaceCode() != partyBoss->m_Param.GetRaceCode() || partyBoss->IsPunished(2u, false))
  {
    return;
  }
  if (partyBoss->m_byUserDgr)
  {
    if (!m_byUserDgr)
    {
      return;
    }
  }
  else if (m_byUserDgr)
  {
    return;
  }

  if (partyBoss->m_pPartyMgr->m_bLock)
  {
    SendMsg_AwayPartyRequestResult(7u);
    partyBoss->SendMsg_AwayPartyRequestResult(7u);
    return;
  }
  if (partyBoss->m_pPartyMgr->IsPartyMode() && !partyBoss->m_pPartyMgr->IsPartyBoss())
  {
    SendMsg_AwayPartyRequestResult(8u);
    partyBoss->SendMsg_AwayPartyRequestResult(8u);
    return;
  }
  if (m_pPartyMgr->IsPartyMode())
  {
    SendMsg_AwayPartyRequestResult(4u);
    partyBoss->SendMsg_AwayPartyRequestResult(4u);
    return;
  }

  const float profileBonus = m_EP.GetEff_Have(53);
  const int myLevel = static_cast<int>(GetLevel());
  if (!partyBoss->m_pPartyMgr->IsJoinPartyLevel(myLevel, profileBonus))
  {
    SendMsg_AwayPartyRequestResult(5u);
    partyBoss->SendMsg_AwayPartyRequestResult(5u);
    return;
  }

  _STORAGE_LIST::_db_con *consumeItem = nullptr;
  if (g_Main.m_bAwayPartyConsumeItem)
  {
    consumeItem = m_Param.m_dbInven.GetPtrFromItemCode(g_Main.m_strAwayPartyItemCode);
    if (!consumeItem)
    {
      SendMsg_AwayPartyRequestResult(9u);
      partyBoss->SendMsg_AwayPartyRequestResult(9u);
      return;
    }
  }
  if (g_Main.m_bAwayPartyConsumeMoney && g_Main.m_dwAwayPartyMoney > GetMoney(0))
  {
    SendMsg_AwayPartyRequestResult(11u);
    partyBoss->SendMsg_AwayPartyRequestResult(11u);
    return;
  }

  if (byRetCode)
  {
    partyBoss->SendMsg_AwayPartyRequestResult(10u);
    return;
  }

  wa_PartyJoin(&partyBoss->m_id, &m_id);
  if (!m_pPartyMgr->IsPartyMode())
  {
    SendMsg_AwayPartyRequestResult(7u);
    partyBoss->SendMsg_AwayPartyRequestResult(7u);
    return;
  }

  if (g_Main.m_bAwayPartyConsumeItem && consumeItem)
  {
    const unsigned __int64 remainingDur =
      Emb_AlterDurPoint(0, consumeItem->m_byStorageIndex, -1, false, true);
    if (remainingDur)
    {
      SendMsg_AdjustAmountInform(0, consumeItem->m_wSerial, static_cast<unsigned int>(remainingDur));
    }
    else
    {
      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, consumeItem, m_szItemHistoryFileName);
    }
  }

  if (g_Main.m_bAwayPartyConsumeMoney)
  {
    SubDalant(g_Main.m_dwAwayPartyMoney);
    SendMsg_AlterMoneyInform(0);

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr *moneySupplyMgr = CMoneySupplyMgr::Instance();
      moneySupplyMgr->UpdateFeeMoneyData(m_Param.GetRaceCode(), level, g_Main.m_dwAwayPartyMoney);
    }
  }
}

void CPlayer::pc_BuddyDownloadRequest()
{

  _buddy_download_result_zocl msg{};
  int dataSize = 0;
  void *cursor = msg.sData;

  const unsigned __int8 buddyCount = static_cast<unsigned __int8>(m_pmBuddy.GetBuddyNum());
  memcpy_0(cursor, &buddyCount, 1uLL);
  cursor = static_cast<char *>(cursor) + 1;
  ++dataSize;

  for (int index = 0; index < 50; ++index)
  {
    _BUDDY_LIST::__list *buddy = &m_pmBuddy.m_List[index];
    if (buddy->fill())
    {
      const unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(buddy->wszName));
      memcpy_0(cursor, &nameLen, 1uLL);
      cursor = static_cast<char *>(cursor) + 1;
      ++dataSize;

      memcpy_0(cursor, buddy->wszName, nameLen);
      cursor = static_cast<char *>(cursor) + nameLen;
      dataSize += nameLen;

      memcpy_0(cursor, buddy, 4uLL);
      cursor = static_cast<char *>(cursor) + 4;
      dataSize += 4;

      const unsigned __int8 isOnline = buddy->pPtr ? 1 : 0;
      memcpy_0(cursor, &isOnline, 1uLL);
      cursor = static_cast<char *>(cursor) + 1;
      ++dataSize;

      if (buddy->pPtr)
      {
        memcpy_0(cursor, &buddy->pPtr->m_wRegionMapIndex, 1uLL);
        cursor = static_cast<char *>(cursor) + 1;
        ++dataSize;

        memcpy_0(cursor, &buddy->pPtr->m_wRegionIndex, 1uLL);
        cursor = static_cast<char *>(cursor) + 1;
        ++dataSize;
      }
    }
  }

  msg.wDataSize = static_cast<unsigned __int16>(dataSize);
  const unsigned __int16 msgLength = static_cast<unsigned __int16>(2 + msg.wDataSize);
  unsigned __int8 type[2] = {31, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msgLength);
}

void CPlayer::pc_BuddyAddRequest(unsigned __int16 wDstIndex, unsigned int dwDstSerial, char *pwszDstName)
{
  unsigned __int8 retCode = 0;
  CPlayer *targetPlayer = nullptr;
  unsigned int targetSerial = static_cast<unsigned int>(-1);

  if (!m_pmBuddy.GetEmptyData())
  {
    retCode = 1;
    SendMsg_BuddyAddFail(retCode, pwszDstName);
    return;
  }
  if (m_pmBuddy.IsBuddy(wDstIndex))
  {
    retCode = 2;
    SendMsg_BuddyAddFail(retCode, pwszDstName);
    return;
  }

  if (wDstIndex == static_cast<unsigned __int16>(-1))
  {
    targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, pwszDstName);
    if (!targetPlayer || !targetPlayer->m_bLive)
    {
      retCode = 3;
      SendMsg_BuddyAddFail(retCode, pwszDstName);
      return;
    }
    targetSerial = targetPlayer->m_dwObjSerial;
  }
  else
  {
    targetPlayer = &g_Player[wDstIndex];
    if (!targetPlayer->m_bLive || targetPlayer->m_dwObjSerial != dwDstSerial)
    {
      retCode = 3;
      SendMsg_BuddyAddFail(retCode, pwszDstName);
      return;
    }
    targetSerial = dwDstSerial;
  }

  if (targetPlayer->m_Param.GetRaceCode() == m_Param.GetRaceCode())
  {
    if (targetPlayer == this)
    {
      retCode = 3;
    }
    else if (!targetPlayer->m_pmBuddy.GetEmptyData() && !targetPlayer->m_pmBuddy.IsBuddy(m_dwObjSerial))
    {
      retCode = 4;
    }
  }
  else
  {
    retCode = 7;
  }

  if (retCode)
  {
    SendMsg_BuddyAddFail(retCode, pwszDstName);
  }
  else
  {
    m_pmBuddy.PushLastApplyTemp(targetSerial);
    targetPlayer->SendMsg_BuddyAddAsk(m_ObjID.m_wIndex, m_dwObjSerial, m_Param.GetCharNameA());
  }
}

void CPlayer::pc_BuddyAddAnswer(bool bAccept, unsigned __int16 wAskerIndex, unsigned int dwAskerSerial)
{
  unsigned __int8 retCode = 0;
  CPlayer *askerPlayer = nullptr;
  CPlayer *notifyAsker = nullptr;

  if (!bAccept || m_pmBuddy.GetEmptyData() || m_pmBuddy.IsBuddy(dwAskerSerial))
  {
    askerPlayer = &g_Player[wAskerIndex];
    if (askerPlayer->m_bLive)
    {
      if (askerPlayer->m_dwObjSerial == dwAskerSerial)
      {
        if (askerPlayer->m_pmBuddy.IsPushLastApply(m_dwObjSerial))
        {
          notifyAsker = askerPlayer;
          if (askerPlayer->m_Param.GetRaceCode() == m_Param.GetRaceCode())
          {
            if (notifyAsker == this)
            {
              retCode = 3;
            }
            else if (
              bAccept && !notifyAsker->m_pmBuddy.GetEmptyData() && !notifyAsker->m_pmBuddy.IsBuddy(m_dwObjSerial))
            {
              retCode = 4;
            }
          }
          else
          {
            retCode = 3;
          }
        }
        else
        {
          retCode = 5;
        }
      }
      else
      {
        retCode = 3;
      }
    }
    else
    {
      retCode = 3;
    }
  }
  else
  {
    retCode = 1;
  }

  if (retCode)
  {
    SendMsg_BuddyAddAnswerResult(
      retCode,
      static_cast<char>(bAccept),
      dwAskerSerial,
      static_cast<unsigned __int16>(-1),
      static_cast<unsigned int>(-1),
      const_cast<char *>("fail"));
    if (notifyAsker)
    {
      notifyAsker->SendMsg_BuddyAddAnswerResult(
        retCode,
        static_cast<char>(bAccept),
        dwAskerSerial,
        static_cast<unsigned __int16>(-1),
        static_cast<unsigned int>(-1),
        const_cast<char *>("fail"));
    }
    return;
  }

  if (bAccept)
  {
    int slotIndex = m_pmBuddy.PushBuddy(notifyAsker->m_dwObjSerial, notifyAsker->m_Param.GetCharNameA(), notifyAsker);
    if (slotIndex != -1)
    {
      m_pUserDB->Update_AddBuddy(slotIndex, notifyAsker->m_dwObjSerial, notifyAsker->m_Param.GetCharNameA());
    }

    slotIndex = notifyAsker->m_pmBuddy.PushBuddy(m_dwObjSerial, m_Param.GetCharNameA(), this);
    if (slotIndex != -1)
    {
      notifyAsker->m_pUserDB->Update_AddBuddy(slotIndex, m_dwObjSerial, m_Param.GetCharNameA());
    }
  }

  SendMsg_BuddyAddAnswerResult(
    retCode,
    static_cast<char>(bAccept),
    dwAskerSerial,
    notifyAsker->m_ObjID.m_wIndex,
    notifyAsker->m_dwObjSerial,
    notifyAsker->m_Param.GetCharNameA());
  SendMsg_BuddyPosInform(notifyAsker->m_dwObjSerial, notifyAsker->m_wRegionMapIndex, notifyAsker->m_wRegionIndex);

  notifyAsker->SendMsg_BuddyAddAnswerResult(
    retCode,
    static_cast<char>(bAccept),
    dwAskerSerial,
    m_ObjID.m_wIndex,
    m_dwObjSerial,
    m_Param.GetCharNameA());
  notifyAsker->SendMsg_BuddyPosInform(m_dwObjSerial, m_wRegionMapIndex, m_wRegionIndex);
  notifyAsker->m_pmBuddy.PopLastApplyTemp(m_dwObjSerial);
}

void wa_PartyJoin(_CLID *pidBoss, _CLID *pidJoiner)
{
  CPartyPlayer *partyBoss = &g_PartyPlayer[pidBoss->wIndex];
  CPartyPlayer *joiner = &g_PartyPlayer[pidJoiner->wIndex];

  if (partyBoss->m_id.dwSerial != pidBoss->dwSerial
      || joiner->m_id.dwSerial != pidJoiner->dwSerial
      || !partyBoss->m_bLogin
      || !joiner->m_bLogin
      || joiner->IsPartyMode()
      || (partyBoss->IsPartyMode() && !partyBoss->IsPartyBoss())
      || partyBoss->IsPartyLock())
  {
    return;
  }

  if (partyBoss->IsPartyBoss())
  {
    if (!partyBoss->InsertPartyMember(joiner))
    {
      return;
    }
  }
  else if (!partyBoss->IsPartyMode() && !partyBoss->FoundParty(joiner))
  {
    return;
  }

  CPlayer *joinerPlayer = &g_Player[joiner->m_wZoneIndex];
  for (int index = 0; index < 8; ++index)
  {
    CPartyPlayer *member = partyBoss->m_pPartyMember[index];
    if (!member)
    {
      break;
    }
    if (member != joiner)
    {
      CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
      memberPlayer->SendMsg_PartyJoinMemberResult(joiner, static_cast<char>(partyBoss->m_byLootShareSystem));
    }
  }

  if (joinerPlayer)
  {
    joinerPlayer->SendMsg_PartyJoinJoinerResult();
  }

  for (int index = 0; index < 8; ++index)
  {
    CPartyPlayer *member = partyBoss->m_pPartyMember[index];
    if (!member)
    {
      break;
    }
    if (member != joiner)
    {
      CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
      memberPlayer->SendData_PartyMemberInfo(joiner->m_wZoneIndex);
    }
  }

  if (joinerPlayer)
  {
    joinerPlayer->SendData_PartyMemberInfoToMembers();
  }
}

void wa_PartyForceLeave(_CLID *pidBoss, _CLID *pidLeaver)
{
  CPartyPlayer *partyBoss = &g_PartyPlayer[pidBoss->wIndex];
  CPartyPlayer *exiter = &g_PartyPlayer[pidLeaver->wIndex];

  if (partyBoss->m_id.dwSerial != pidBoss->dwSerial
      || exiter->m_id.dwSerial != pidLeaver->dwSerial
      || !partyBoss->m_bLogin
      || !exiter->m_bLogin
      || !exiter->IsPartyMode()
      || !partyBoss->IsPartyBoss()
      || exiter->m_pPartyBoss != partyBoss)
  {
    return;
  }

  CPartyPlayer *prevMembers[8]{};
  for (int index = 0; index < 8; ++index)
  {
    prevMembers[index] = partyBoss->m_pPartyMember[index];
  }

  CPartyPlayer *newBoss = nullptr;
  if (!partyBoss->RemovePartyMember(exiter, &newBoss))
  {
    return;
  }

  for (int index = 0; index < 8 && prevMembers[index]; ++index)
  {
    CPlayer *notifyPlayer = &g_Player[prevMembers[index]->m_wZoneIndex];
    notifyPlayer->SendMsg_PartyLeaveCompulsionResult(exiter);
  }
}

void wa_PartyDisjoint(_CLID *pidBoss)
{
  CPartyPlayer *partyBoss = &g_PartyPlayer[pidBoss->wIndex];
  if (partyBoss->m_id.dwSerial != pidBoss->dwSerial || !partyBoss->IsPartyBoss())
  {
    return;
  }

  CPartyPlayer *prevMembers[8]{};
  for (int index = 0; index < 8; ++index)
  {
    prevMembers[index] = partyBoss->m_pPartyMember[index];
  }

  partyBoss->DisjointParty();

  for (int index = 0; index < 8 && prevMembers[index]; ++index)
  {
    CPlayer *notifyPlayer = &g_Player[prevMembers[index]->m_wZoneIndex];
    notifyPlayer->SendMsg_PartyDisjointResult(1u);
  }
}

void wa_PartySuccession(_CLID *pidBoss, _CLID *pidSuccessor)
{
  CPartyPlayer *partyBoss = &g_PartyPlayer[pidBoss->wIndex];
  CPartyPlayer *successor = &g_PartyPlayer[pidSuccessor->wIndex];

  if (partyBoss->m_id.dwSerial != pidBoss->dwSerial
      || successor->m_id.dwSerial != pidSuccessor->dwSerial
      || !partyBoss->m_bLogin
      || !successor->m_bLogin
      || !partyBoss->IsPartyBoss()
      || !successor->IsPartyMode()
      || successor->m_pPartyBoss != partyBoss
      || !partyBoss->InheritBoss(successor))
  {
    return;
  }

  for (int index = 0; index < 8; ++index)
  {
    CPartyPlayer *member = successor->m_pPartyMember[index];
    if (!member)
    {
      break;
    }

    CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
    memberPlayer->SendMsg_PartySuccessResult(successor);
  }
}

void wa_PartyLock(_CLID *pidBoss, bool bLock)
{
  CPartyPlayer *partyBoss = &g_PartyPlayer[pidBoss->wIndex];
  if (partyBoss->m_id.dwSerial != pidBoss->dwSerial
      || !partyBoss->m_bLogin
      || !partyBoss->IsPartyBoss()
      || partyBoss->m_bLock == bLock
      || !partyBoss->SetLockMode(bLock))
  {
    return;
  }

  for (int index = 0; index < 8; ++index)
  {
    CPartyPlayer *member = partyBoss->m_pPartyMember[index];
    if (!member)
    {
      break;
    }

    CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
    memberPlayer->SendMsg_PartyLockResult(static_cast<char>(bLock));
  }
}

void wa_PartyLootShareSystem(_CLID *pidBoss, unsigned __int8 byLootShareMode)
{
  CPartyPlayer *partyBoss = &g_PartyPlayer[pidBoss->wIndex];
  if (partyBoss->m_id.dwSerial != pidBoss->dwSerial
      || !partyBoss->m_bLogin
      || !partyBoss->IsPartyBoss()
      || !partyBoss->SetLootShareMode(byLootShareMode))
  {
    return;
  }

  for (int index = 0; index < 8; ++index)
  {
    CPartyPlayer *member = partyBoss->m_pPartyMember[index];
    if (!member)
    {
      break;
    }

    CPlayer *memberPlayer = &g_Player[member->m_wZoneIndex];
    memberPlayer->SendMsg_PartyAlterLootShareResult(static_cast<char>(byLootShareMode));
  }
}

void CPlayer::pc_PartyLeaveSelfReqeuest()
{
  if (m_pPartyMgr->IsPartyMode())
  {
    wa_PartySelfLeave(&m_id);
  }
  else
  {
    SendMsg_PartyLeaveSelfResult(nullptr, false);
  }
}

void wa_PartySelfLeave(_CLID *pidLeaver)
{
  CPartyPlayer *exiter = &g_PartyPlayer[pidLeaver->wIndex];
  CPartyPlayer *partyBoss = exiter->m_pPartyBoss;

  if (exiter->m_id.dwSerial == pidLeaver->dwSerial && exiter->m_bLogin && exiter->IsPartyMode()
      && partyBoss->IsPartyBoss())
  {
    CPartyPlayer *prevMembers[8]{};
    for (int j = 0; j < 8; ++j)
    {
      prevMembers[j] = partyBoss->m_pPartyMember[j];
    }

    CPartyPlayer *newBoss = nullptr;
    if (partyBoss->RemovePartyMember(exiter, &newBoss))
    {
      for (int j = 0; j < 8 && prevMembers[j]; ++j)
      {
        CPlayer *notifyPlayer = &g_Player[prevMembers[j]->m_id.wIndex];
        notifyPlayer->SendMsg_PartyLeaveSelfResult(exiter, false);
      }
    }
  }
}

void wa_ExitWorld(_CLID *pidWorld)
{
  CPartyPlayer *leaver = &g_PartyPlayer[pidWorld->wIndex];
  if (leaver->m_id.dwSerial == pidWorld->dwSerial && leaver->m_bLogin)
  {
    CPartyPlayer *oldBoss = leaver->m_pPartyBoss;
    CPartyPlayer *newBoss = leaver->m_pPartyBoss;
    if (leaver->IsPartyMode())
    {
      CPartyPlayer **members = leaver->GetPtrPartyMember();
      for (int j = 0; j < 8 && members[j]; ++j)
      {
        if (members[j] != leaver)
        {
          CPlayer *notifyPlayer = &g_Player[members[j]->m_wZoneIndex];
          notifyPlayer->SendMsg_PartyLeaveSelfResult(leaver, true);
        }
      }
    }
    leaver->ExitWorld(&newBoss);
    if (newBoss && newBoss != oldBoss)
    {
      CPartyPlayer **members = newBoss->GetPtrPartyMember();
      for (int k = 0; k < 8 && members[k]; ++k)
      {
        CPlayer *notifyPlayer = &g_Player[members[k]->m_wZoneIndex];
        notifyPlayer->SendMsg_PartySuccessResult(newBoss);
      }
    }
  }
}

void CPlayer::_check_party_target_object()
{
  CGameObject *targetObject = nullptr;
  if (m_pPartyMgr->IsPartyMode() && m_pPartyMgr->IsPartyBoss())
  {
    targetObject = m_GroupTargetObject[0].pObject;
    if (targetObject)
    {
      if (targetObject->m_bLive
          && m_GroupTargetObject[0].byKind == targetObject->m_ObjID.m_byKind
          && m_GroupTargetObject[0].byID == targetObject->m_ObjID.m_byID
          && m_GroupTargetObject[0].dwSerial == targetObject->m_dwObjSerial)
      {
        pc_RefreshGroupTargetPosition(0, targetObject);
      }
      else
      {
        pc_ReleaseGroupTargetObjectRequest(0);
      }
    }
  }
}

void CPlayer::_check_guild_target_object()
{
  CGameObject *targetObject = nullptr;
  if (m_Param.m_pGuild)
  {
    const unsigned int guildMasterSerial = m_Param.m_pGuild->GetGuildMasterSerial();
    if (guildMasterSerial == m_Param.GetCharSerial())
    {
      targetObject = m_GroupTargetObject[1].pObject;
      if (targetObject)
      {
        if (targetObject->m_bLive
            && m_GroupTargetObject[1].byKind == targetObject->m_ObjID.m_byKind
            && m_GroupTargetObject[1].byID == targetObject->m_ObjID.m_byID
            && m_GroupTargetObject[1].dwSerial == targetObject->m_dwObjSerial)
        {
          pc_RefreshGroupTargetPosition(1u, targetObject);
        }
        else
        {
          pc_ReleaseGroupTargetObjectRequest(1u);
        }
      }
    }
  }
}

void CPlayer::_check_race_target_object()
{
  CGameObject *targetObject = nullptr;
  const unsigned __int8 raceCode = static_cast<unsigned __int8>(m_Param.GetRaceCode());
  const unsigned int currentRaceBossSerial =
    CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);

  if (currentRaceBossSerial == m_Param.GetCharSerial())
  {
    targetObject = m_GroupTargetObject[2].pObject;
    if (targetObject)
    {
      if (targetObject->m_bLive
          && m_GroupTargetObject[2].byKind == targetObject->m_ObjID.m_byKind
          && m_GroupTargetObject[2].byID == targetObject->m_ObjID.m_byID
          && m_GroupTargetObject[2].dwSerial == targetObject->m_dwObjSerial)
      {
        pc_RefreshGroupTargetPosition(2u, targetObject);
      }
      else
      {
        pc_ReleaseGroupTargetObjectRequest(2u);
      }
    }
  }
}

void CPlayer::pc_RefreshGroupTargetPosition(unsigned __int8 byGroupType, CGameObject *pObject)
{
  if (!pObject)
  {
    return;
  }

  if (byGroupType == 0)
  {
    CPartyPlayer **partyMembers = m_pPartyMgr->GetPtrPartyMember();
    if (!partyMembers)
    {
      return;
    }

    for (int j = 0; j < 8; ++j)
    {
      if (partyMembers[j])
      {
        CPlayer *player = &g_Player[partyMembers[j]->m_id.wIndex];
        if (player->m_bOper)
        {
          CGameObject *groupTarget = player->m_GroupTargetObject[0].pObject;
          if (groupTarget
              && player->m_pCurMap == groupTarget->m_pCurMap
              && player->m_wMapLayerIndex == groupTarget->m_wMapLayerIndex)
          {
            const int range = static_cast<int>(player->GetUseSectorRange());
            if (!IsCircleObjectCompat(player, range, groupTarget))
            {
              player->SendMsg_RefeshGroupTargetPosition(0);
            }
          }
        }
      }
    }

    return;
  }

  if (byGroupType == 1)
  {
    for (int j = 0; j < 50; ++j)
    {
      _guild_member_info *member = &m_Param.m_pGuild->m_MemberData[j];
      if (member->IsFill())
      {
        CPlayer *player = member->pPlayer;
        if (player && player->m_bOper)
        {
          CGameObject *groupTarget = player->m_GroupTargetObject[byGroupType].pObject;
          if (groupTarget
              && player->m_pCurMap == groupTarget->m_pCurMap
              && player->m_wMapLayerIndex == groupTarget->m_wMapLayerIndex)
          {
            const int range = static_cast<int>(player->GetUseSectorRange());
            if (!IsCircleObjectCompat(player, range, groupTarget))
            {
              player->SendMsg_RefeshGroupTargetPosition(byGroupType);
            }
          }
        }
      }
    }

    return;
  }

  if (byGroupType == 2)
  {
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *player = &g_Player[j];
      if (player->m_bOper)
      {
        if (player->m_Param.GetRaceCode() == m_Param.GetRaceCode())
        {
          CGameObject *groupTarget = player->m_GroupTargetObject[byGroupType].pObject;
          if (groupTarget
              && player->m_pCurMap == groupTarget->m_pCurMap
              && player->m_wMapLayerIndex == groupTarget->m_wMapLayerIndex)
          {
            const int range = static_cast<int>(player->GetUseSectorRange());
            if (!IsCircleObjectCompat(player, range, groupTarget))
            {
              player->SendMsg_RefeshGroupTargetPosition(byGroupType);
            }
          }
        }
      }
    }
  }
}

void CPlayer::_check_target_object()
{
  if (!this->m_TargetObject.pObject)
  {
    return;
  }

  CGameObject *targetObject = this->m_TargetObject.pObject;
  if (targetObject->m_bLive
      && this->m_TargetObject.byKind == targetObject->m_ObjID.m_byKind
      && this->m_TargetObject.byID == targetObject->m_ObjID.m_byID
      && this->m_TargetObject.dwSerial == targetObject->m_dwObjSerial
      && this->m_TargetObject.pObject->m_pCurMap == this->m_pCurMap)
  {
    const int hpRate = static_cast<int>(targetObject->CalcCurHPRate());
    if (std::abs(hpRate - static_cast<int>(this->m_TargetObject.wHPRate)) > 100)
    {
      this->m_TargetObject.wHPRate = static_cast<unsigned __int16>(hpRate);
      this->SendMsg_TargetObjectHPInform();
    }

    this->SendTargetMonsterSFContInfo();
    this->SendTargetPlayerDamageContInfo();
  }
  else
  {
    this->m_TargetObject.init();
  }
}

void CPlayer::SendMsg_TargetObjectHPInform()
{
  _target_object_hp_inform_clzo packet{};
  packet.byKind = static_cast<char>(this->m_TargetObject.byKind);
  packet.byID = static_cast<char>(this->m_TargetObject.byID);
  packet.dwSerial = this->m_TargetObject.dwSerial;
  packet.wHPRate = this->m_TargetObject.wHPRate;

  unsigned __int8 type[2] = {13, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_RefeshGroupTargetPosition(char byGroupType)
{
  _refresh_group_target_position_zocl packet{};
  packet.byGroupType = byGroupType;

  const unsigned __int8 groupIndex = static_cast<unsigned __int8>(byGroupType);
  CGameObject *groupObject = this->m_GroupTargetObject[groupIndex].pObject;
  packet.byMapCode = static_cast<char>(groupObject->m_pCurMap->m_pMapSet->m_dwIndex);
  packet.byID = static_cast<char>(this->m_GroupTargetObject[groupIndex].byID);
  packet.dwSerial = this->m_GroupTargetObject[groupIndex].dwSerial;
  memcpy_0(packet.fPos, groupObject->m_fCurPos, sizeof(packet.fPos));

  unsigned __int8 type[2] = {13, 110};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_BuddyNameReNewal(unsigned int dwSerial, char *wszName)
{
  _buddy_renewal_zocl packet{};
  packet.dwSerial = dwSerial;
  strcpy_s(packet.wszBuddyName, sizeof(packet.wszBuddyName), wszName);

  unsigned __int8 type[2] = {31, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_SetTargetObjectResult(char byRetCode, bool bForce)
{
  _set_target_object_result_zocl packet{};
  packet.byRetCode = byRetCode;

  if (!byRetCode && this->m_TargetObject.pObject)
  {
    packet.byKind = this->m_TargetObject.byKind;
    packet.byID = this->m_TargetObject.byID;
    packet.dwSerial = this->m_TargetObject.dwSerial;
    packet.wHPRate = this->m_TargetObject.wHPRate;

    if (this->m_TargetObject.byKind == 0 && this->m_TargetObject.byID == 0)
    {
      CPlayer *targetPlayer = static_cast<CPlayer *>(this->m_TargetObject.pObject);
      packet.byGrade = static_cast<char>(targetPlayer->m_bCorpse);
    }
  }

  packet.bForce = bForce;

  unsigned __int8 type[2] = {13, 27};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GroupTargetInform(char byGroupType, const char *pwszName)
{
  _group_target_inform_zocl packet{};
  packet.byGroupType = byGroupType;
  strcpy_0(packet.wszTargetName, pwszName);

  unsigned __int8 type[2] = {13, 111};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendData_PartyMemberPos()
{
  if (!this->m_pPartyMgr)
  {
    return;
  }

  CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  _party_member_pos_upd msg{};

  msg.dwMemSerial = this->m_dwObjSerial;
  msg.byMapCode = static_cast<unsigned __int8>(this->m_Param.GetMapCode());
  msg.zPos[0] = static_cast<unsigned __int16>(this->m_fCurPos[0]);
  msg.zPos[1] = static_cast<unsigned __int16>(this->m_fCurPos[2]);

  const int memberCount = this->m_pPartyMgr->GetPopPartyMember();
  unsigned __int8 type[2] = {16, 24};

  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&msg), 9u);
    }
  }
}

void CPlayer::SendMsg_BuddyPosInform(
  unsigned int dwDstSerial,
  unsigned __int8 byMapIndex,
  unsigned __int8 byPosCode)
{
  _buddy_pos_inform_zocl msg{};

  msg.dwSerial = dwDstSerial;
  msg.byMapIndex = byMapIndex;
  msg.byPositionCode = byPosCode;

  unsigned __int8 type[2] = {31, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 6u);
}

void CPlayer::pc_PartyReqBlock(bool bBlock)
{
  if (this->m_bBlockParty != bBlock)
  {
    this->m_bBlockParty = bBlock;
  }
}

void CPlayer::pc_WhisperBlock(bool bBlock)
{
  if (this->m_bBlockWhisper != bBlock)
  {
    this->m_bBlockWhisper = bBlock;
  }
}

void CPlayer::pc_TradeBlock(bool bBlock)
{
  if (this->m_bBlockTrade != bBlock)
  {
    this->m_bBlockTrade = bBlock;
  }
}

bool CPlayer::FixTargetWhile(CCharacter *pkTarget, unsigned int dwMiliSecond)
{
  (void)dwMiliSecond;

  pc_SetTargetObjectRequest(pkTarget, pkTarget->m_dwObjSerial, true);
  return true;
}

void CPlayer::pc_SetTargetObjectRequest(CGameObject *pTar, unsigned int dwSerial, bool bForce)
{
  unsigned __int8 result = 0;

  if (!pTar)
  {
    result = 1;
  }
  else if (!pTar->m_bLive)
  {
    result = 2;
  }
  else if (pTar->m_dwObjSerial != dwSerial)
  {
    result = 3;
  }
  else if (!IsTargetingObject(pTar))
  {
    result = 4;
  }

  if (!result)
  {
    this->m_TargetObject.init();
    this->m_TargetObject.pObject = pTar;
    this->m_TargetObject.byKind = pTar->m_ObjID.m_byKind;
    this->m_TargetObject.byID = pTar->m_ObjID.m_byID;
    this->m_TargetObject.dwSerial = pTar->m_dwObjSerial;
    const float currentHp = static_cast<float>(pTar->GetHP());
    const float maxHp = static_cast<float>(pTar->GetMaxHP());
    this->m_TargetObject.wHPRate = static_cast<unsigned __int16>(static_cast<int>((currentHp / maxHp) * 10000.0f));
  }

  this->SendMsg_SetTargetObjectResult(static_cast<char>(result), bForce);
  this->SendTargetMonsterSFContInfo();
  this->SendTargetPlayerDamageContInfo();
}

void CPlayer::pc_SetGroupTargetObjectRequest(CGameObject *pTar, unsigned int dwSerial, unsigned __int8 byGroupType)
{
  unsigned __int8 result = 0;

  if (!pTar)
  {
    result = 1;
    this->SendMsg_SetGroupTargetObjectResult(static_cast<char>(result), static_cast<char>(byGroupType));
    return;
  }

  if (!pTar->m_bLive)
  {
    result = 2;
    this->SendMsg_SetGroupTargetObjectResult(static_cast<char>(result), static_cast<char>(byGroupType));
    return;
  }

  if (pTar->m_dwObjSerial != dwSerial)
  {
    result = 3;
    this->SendMsg_SetGroupTargetObjectResult(static_cast<char>(result), static_cast<char>(byGroupType));
    return;
  }

  if (!IsTargetingObject(pTar) || pTar->m_ObjID.m_byID == 2)
  {
    result = 4;
    this->SendMsg_SetGroupTargetObjectResult(static_cast<char>(result), static_cast<char>(byGroupType));
    return;
  }

  if (byGroupType == 0)
  {
    if (this->m_pPartyMgr->IsPartyMode() && this->m_pPartyMgr->IsPartyBoss())
    {
      CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
      if (partyMembers)
      {
        for (int index = 0; index < 8; ++index)
        {
          CPartyPlayer *partyMember = partyMembers[index];
          if (!partyMember)
          {
            continue;
          }

          CPlayer *groupPlayer = &g_Player[partyMember->m_id.wIndex];
          if (!groupPlayer->m_bOper)
          {
            continue;
          }

          if (groupPlayer->m_pCurMap != this->m_pCurMap || groupPlayer->m_wMapLayerIndex != this->m_wMapLayerIndex)
          {
            continue;
          }

          groupPlayer->m_GroupTargetObject[0].init();
          groupPlayer->m_GroupTargetObject[0].pObject = pTar;
          groupPlayer->m_GroupTargetObject[0].byKind = pTar->m_ObjID.m_byKind;
          groupPlayer->m_GroupTargetObject[0].byID = pTar->m_ObjID.m_byID;
          groupPlayer->m_GroupTargetObject[0].dwSerial = pTar->m_dwObjSerial;
          groupPlayer->SendMsg_SetGroupTargetObjectResult(0, 0);
        }
      }
    }
    return;
  }

  if (byGroupType == 1)
  {
    if (!this->m_Param.m_pGuild)
    {
      return;
    }

    const unsigned int guildMasterSerial = this->m_Param.m_pGuild->GetGuildMasterSerial();
    if (guildMasterSerial != this->m_Param.GetCharSerial())
    {
      return;
    }

    for (int memberIndex = 0; memberIndex < 50; ++memberIndex)
    {
      _guild_member_info *member = &this->m_Param.m_pGuild->m_MemberData[memberIndex];
      if (!member->IsFill() || !member->pPlayer)
      {
        continue;
      }

      CPlayer *groupPlayer = member->pPlayer;
      if (!groupPlayer->m_bOper)
      {
        continue;
      }

      if (groupPlayer->m_pCurMap != this->m_pCurMap || groupPlayer->m_wMapLayerIndex != this->m_wMapLayerIndex)
      {
        continue;
      }

      groupPlayer->m_GroupTargetObject[1].init();
      groupPlayer->m_GroupTargetObject[1].pObject = pTar;
      groupPlayer->m_GroupTargetObject[1].byKind = pTar->m_ObjID.m_byKind;
      groupPlayer->m_GroupTargetObject[1].byID = pTar->m_ObjID.m_byID;
      groupPlayer->m_GroupTargetObject[1].dwSerial = pTar->m_dwObjSerial;
      groupPlayer->SendMsg_SetGroupTargetObjectResult(0, 1);
    }
    return;
  }

  if (byGroupType == 2)
  {
    const int raceCode = this->m_Param.GetRaceCode();
    const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
      static_cast<unsigned __int8>(raceCode),
      0);
    if (raceBossSerial != this->m_Param.GetCharSerial())
    {
      return;
    }

    char targetName[17]{};
    if (pTar->m_ObjID.m_byKind == 0 && pTar->m_ObjID.m_byID == 0)
    {
      CPlayer *targetPlayer = static_cast<CPlayer *>(pTar);
      if (targetPlayer->m_bOper)
      {
        strcpy_0(targetName, targetPlayer->m_Param.GetCharNameW());
      }
    }

    for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
    {
      CPlayer *groupPlayer = &g_Player[playerIndex];
      if (!groupPlayer->m_bOper)
      {
        continue;
      }

      if (groupPlayer->m_Param.GetRaceCode() != raceCode)
      {
        continue;
      }

      if (targetName[0] && groupPlayer->GetLevel() >= 25)
      {
        groupPlayer->SendMsg_GroupTargetInform(2, targetName);
      }

      if (groupPlayer->m_pCurMap != this->m_pCurMap || groupPlayer->m_wMapLayerIndex != this->m_wMapLayerIndex)
      {
        continue;
      }

      groupPlayer->m_GroupTargetObject[2].init();
      groupPlayer->m_GroupTargetObject[2].pObject = pTar;
      groupPlayer->m_GroupTargetObject[2].byKind = pTar->m_ObjID.m_byKind;
      groupPlayer->m_GroupTargetObject[2].byID = pTar->m_ObjID.m_byID;
      groupPlayer->m_GroupTargetObject[2].dwSerial = pTar->m_dwObjSerial;
      groupPlayer->SendMsg_SetGroupTargetObjectResult(0, 2);
    }
  }
}

void CPlayer::pc_ReleaseTargetObjectRequest()
{
  this->m_TargetObject.pObject = nullptr;
}

void CPlayer::pc_ReleaseGroupTargetObjectRequest(unsigned __int8 byGroupType)
{
  if (!this->m_GroupTargetObject[byGroupType].pObject)
  {
    return;
  }

  if (byGroupType == 0)
  {
    if (this->m_pPartyMgr->IsPartyMode() && this->m_pPartyMgr->IsPartyBoss())
    {
      CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
      if (partyMembers)
      {
        for (int index = 0; index < 8; ++index)
        {
          CPartyPlayer *partyMember = partyMembers[index];
          if (!partyMember)
          {
            continue;
          }

          CPlayer *groupPlayer = &g_Player[partyMember->m_id.wIndex];
          if (groupPlayer->m_bOper && groupPlayer->m_GroupTargetObject[0].pObject)
          {
            groupPlayer->m_GroupTargetObject[0].init();
            groupPlayer->SendMsg_ReleaseGroupTargetObjectResult(0);
          }
        }
      }
    }

    return;
  }

  if (byGroupType == 1)
  {
    if (!this->m_Param.m_pGuild)
    {
      return;
    }

    const unsigned int guildMasterSerial = this->m_Param.m_pGuild->GetGuildMasterSerial();
    if (guildMasterSerial != this->m_Param.GetCharSerial())
    {
      return;
    }

    for (int index = 0; index < 50; ++index)
    {
      _guild_member_info *guildMember = &this->m_Param.m_pGuild->m_MemberData[index];
      if (!guildMember->IsFill())
      {
        continue;
      }

      CPlayer *groupPlayer = guildMember->pPlayer;
      if (groupPlayer && groupPlayer->m_bOper && groupPlayer->m_GroupTargetObject[byGroupType].pObject)
      {
        groupPlayer->m_GroupTargetObject[byGroupType].init();
        groupPlayer->SendMsg_ReleaseGroupTargetObjectResult(byGroupType);
      }
    }

    return;
  }

  if (byGroupType == 2)
  {
    const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
    CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
    if (rankingSystem->GetCurrentRaceBossSerial(raceCode, 0) != this->m_Param.GetCharSerial())
    {
      return;
    }

    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *groupPlayer = &g_Player[index];
      if (!groupPlayer->m_bOper)
      {
        continue;
      }

      if (groupPlayer->m_Param.GetRaceCode() == this->m_Param.GetRaceCode()
          && groupPlayer->m_GroupTargetObject[byGroupType].pObject)
      {
        groupPlayer->m_GroupTargetObject[byGroupType].init();
        groupPlayer->SendMsg_ReleaseGroupTargetObjectResult(byGroupType);
      }
    }
  }
}

void CPlayer::pc_SetGroupMapPointRequest(unsigned __int8 byGroupType, float *pzTar)
{
  const unsigned int loopTime = GetLoopTime();
  float probePos[3]{pzTar[0], 0.0f, pzTar[1]};
  const char mapCode = static_cast<char>(this->m_pCurMap->m_pMapSet->m_dwIndex);

  if (this->m_pCurMap->m_Level.mBsp->GetFirstYpos(probePos, -1) == 65535.0f)
  {
    this->SendMsg_SetGroupMapPoint(5, byGroupType, mapCode, pzTar, 0);
    return;
  }

  if (byGroupType == 0)
  {
    if (!this->m_pPartyMgr->IsPartyMode() || !this->m_pPartyMgr->IsPartyBoss())
    {
      this->SendMsg_SetGroupMapPoint(1, byGroupType, mapCode, pzTar, 0);
      return;
    }

    if (loopTime - this->m_dwLastGroupMapPointTime[0] < 60000)
    {
      pzTar[0] = static_cast<float>((60000 - (loopTime - this->m_dwLastGroupMapPointTime[0])) / 1000);
      this->SendMsg_SetGroupMapPoint(2, 0, mapCode, pzTar, 0);
      return;
    }

    CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
    if (!partyMembers)
    {
      return;
    }

    for (int index = 0; index < 8; ++index)
    {
      CPartyPlayer *partyMember = partyMembers[index];
      if (!partyMember)
      {
        continue;
      }

      CPlayer *groupPlayer = &g_Player[partyMember->m_id.wIndex];
      if (groupPlayer->m_bOper
          && groupPlayer->m_pCurMap == this->m_pCurMap
          && groupPlayer->m_wMapLayerIndex == this->m_wMapLayerIndex)
      {
        groupPlayer->SendMsg_SetGroupMapPoint(0, 0, mapCode, pzTar, 60);
      }
    }
  }
  else if (byGroupType == 1)
  {
    if (!this->m_Param.m_pGuild || this->m_Param.m_pGuild->GetGuildMasterSerial() != this->m_Param.GetCharSerial())
    {
      this->SendMsg_SetGroupMapPoint(1, byGroupType, mapCode, pzTar, 0);
      return;
    }

    if (loopTime - this->m_dwLastGroupMapPointTime[byGroupType] < 360000)
    {
      pzTar[0] = static_cast<float>(
        (360000 - (loopTime - this->m_dwLastGroupMapPointTime[byGroupType])) / 1000);
      this->SendMsg_SetGroupMapPoint(2, byGroupType, mapCode, pzTar, 0);
      return;
    }

    for (int index = 0; index < 50; ++index)
    {
      _guild_member_info *guildMember = &this->m_Param.m_pGuild->m_MemberData[index];
      if (!guildMember->IsFill())
      {
        continue;
      }

      CPlayer *groupPlayer = guildMember->pPlayer;
      if (groupPlayer
          && groupPlayer->m_bOper
          && groupPlayer->m_pCurMap == this->m_pCurMap
          && groupPlayer->m_wMapLayerIndex == this->m_wMapLayerIndex)
      {
        groupPlayer->SendMsg_SetGroupMapPoint(0, byGroupType, mapCode, pzTar, 60);
      }
    }
  }
  else if (byGroupType == 2)
  {
    const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
    CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
    if (rankingSystem->GetCurrentRaceBossSerial(raceCode, 0) != this->m_Param.GetCharSerial())
    {
      this->SendMsg_SetGroupMapPoint(1, byGroupType, mapCode, pzTar, 0);
      return;
    }

    if (strcmp_0(this->m_pCurMap->m_pMapSet->m_strCode, "resources"))
    {
      this->SendMsg_SetGroupMapPoint(3, byGroupType, mapCode, pzTar, 0);
      return;
    }

    if (g_HolySys.GetSceneCode() != 1)
    {
      this->SendMsg_SetGroupMapPoint(4, byGroupType, mapCode, pzTar, 0);
      return;
    }

    if (loopTime - this->m_dwLastGroupMapPointTime[byGroupType] < 180000)
    {
      pzTar[0] = static_cast<float>(
        (180000 - (loopTime - this->m_dwLastGroupMapPointTime[byGroupType])) / 1000);
      this->SendMsg_SetGroupMapPoint(2, byGroupType, mapCode, pzTar, 0);
      return;
    }

    for (int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *groupPlayer = &g_Player[index];
      if (groupPlayer->m_bOper
          && groupPlayer->m_Param.GetRaceCode() == this->m_Param.GetRaceCode()
          && groupPlayer->m_pCurMap == this->m_pCurMap)
      {
        groupPlayer->SendMsg_SetGroupMapPoint(0, byGroupType, mapCode, pzTar, 60);
      }
    }
  }

  this->m_dwLastGroupMapPointTime[byGroupType] = loopTime;
  this->m_fGroupMapPoint[byGroupType][0] = pzTar[0];
  this->m_fGroupMapPoint[byGroupType][1] = pzTar[1];
  this->m_byGroupMapPointMapCode[byGroupType] = this->m_pCurMap->GetMapCode();
  this->m_wGroupMapPointLayerIndex[byGroupType] = this->m_wMapLayerIndex;
}

void CPlayer::SendMsg_BuddyDelResult(char byRetCode, unsigned int dwSerial)
{
  _buddy_del_result_clzo msg{};
  msg.byRetCode = byRetCode;
  msg.dwSerial = dwSerial;

  unsigned __int8 type[2] = {31, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::pc_BuddyDelRequest(unsigned int dwSerial)
{
  CPlayer *poppedPlayer = nullptr;
  unsigned __int8 retCode = 6;
  const int slot = this->m_pmBuddy.PopBuddy(dwSerial, &poppedPlayer);
  if (slot != -1)
  {
    if (poppedPlayer)
    {
      poppedPlayer->SendMsg_BuddyLogoffInform(this->m_dwObjSerial);
    }
    this->m_pUserDB->Update_DelBuddy(static_cast<unsigned __int8>(slot));
    retCode = 0;
  }

  this->SendMsg_BuddyDelResult(static_cast<char>(retCode), dwSerial);
}

void CPlayer::SendMsg_CastVoteResult(char byRetCode)
{
  _cast_vote_result_zocl packet{};
  packet.byRetCode = byRetCode;

  if (byRetCode == 0)
  {
    packet.nVoteSerial = this->m_nVoteSerial;
    const int raceCode = this->m_Param.GetRaceCode();
    for (int index = 0; index < 3; ++index)
    {
      packet.wPoint[index] = static_cast<unsigned __int16>(g_VoteSys[raceCode].m_dwPoint[index]);
    }
  }

  unsigned __int8 type[2] = {26, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_MoveToOwnStoneMapInform(unsigned __int8 byStoneMapMoveInfo)
{
  _move_to_own_stonemap_inform_zocl msg{};
  msg.byStoneMapMoveInfo = byStoneMapMoveInfo;

  unsigned __int8 type[2] = {25, 26};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_MoveToOwnStoneMapResult(unsigned __int8 byRetCode, unsigned __int8 byMapIndex, float *pos)
{
  _move_to_own_stonemap_result_zocl msg{};
  msg.byRetCode = byRetCode;
  msg.byMapIndex = byMapIndex;
  FloatToShort(pos, msg.sNewPos, 3);

  unsigned __int8 type[2] = {25, 25};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::pc_MoveToOwnStoneMapRequest()
{
  float pos[3] = {0.0f, 0.0f, 0.0f};
  CMapData *intoMap = nullptr;
  _portal_dummy *portalDummy = nullptr;

  if (this->m_byStoneMapMoveInfo == 1)
  {
    if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
    {
      this->SendMsg_MoveToOwnStoneMapResult(2u, 0, pos);
    }
    else if (this->IsRidingUnit())
    {
      this->SendMsg_MoveToOwnStoneMapResult(3u, 0, pos);
    }
    else if (this->m_byStandType == 1)
    {
      this->SendMsg_MoveToOwnStoneMapResult(4u, 0, pos);
    }
    else if (this->m_pmTrd.bDTradeMode)
    {
      this->SendMsg_MoveToOwnStoneMapResult(5u, 0, pos);
    }
    else if (this->IsSiegeMode())
    {
      this->SendMsg_MoveToOwnStoneMapResult(6u, 0, pos);
    }
    else if (this->m_bCorpse)
    {
      this->SendMsg_MoveToOwnStoneMapResult(7u, 0, pos);
    }
    else if (this->m_pCurMap == g_TransportShip[this->m_Param.GetRaceCode()].m_pLinkShipMap)
    {
      this->SendMsg_MoveToOwnStoneMapResult(8u, 0, pos);
    }
    else if (this->m_bInGuildBattle)
    {
      this->SendMsg_MoveToOwnStoneMapResult(11u, 0, pos);
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType || this->m_EP.GetEff_State(20) || this->m_EP.GetEff_State(28))
    {
      this->SendMsg_MoveToOwnStoneMapResult(12u, 0, pos);
    }
    else
    {
      intoMap = g_HolySys.GetMapData();
      portalDummy = g_HolySys.GetPortalDummy(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()));
      if (portalDummy && intoMap)
      {
        if (intoMap->GetRandPosInDummy(portalDummy->m_pDumPos, pos, true))
        {
          this->OutOfMap(intoMap, 0, 3u, pos);
          this->m_byStoneMapMoveInfo = 2;
          this->SendMsg_MoveToOwnStoneMapResult(
            50u,
            static_cast<unsigned __int8>(intoMap->m_pMapSet->m_dwIndex),
            pos);
        }
        else
        {
          this->SendMsg_MoveToOwnStoneMapResult(10u, 0, pos);
        }
      }
      else
      {
        this->SendMsg_MoveToOwnStoneMapResult(9u, 0, pos);
      }
    }
  }
  else
  {
    this->SendMsg_MoveToOwnStoneMapResult(1u, 0, pos);
  }
}

void CPlayer::SendMsg_GestureInform(unsigned __int8 byGestureType)
{
_gesture_inform_zocl msg{};

  msg.dwActorSerial = this->m_dwObjSerial;
  msg.byGestureType = byGestureType;

  unsigned __int8 type[2] = {13, 23};
  CircleReport(type, reinterpret_cast<char *>(&msg), 5, true);
}

void CPlayer::SendMsg_ResultChangeTaxRate(unsigned __int8 byRetCode, unsigned __int8 byNextTax)
{
  _pt_result_change_tax_rate_zocl message{};
  message.byRet = byRetCode;
  message.byNextTax = byNextTax;

  unsigned __int8 type[2]{13, 121};
  const unsigned __int16 len = static_cast<unsigned __int16>(message.size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&message),
    len);
}

void CPlayer::SendMsg_RaceBossCryMsg()
{
  _notify_raceboss_cry_msg_request_zocl packet{};

  for (int index = 0; index < 10; ++index)
  {
    strcpy_0(packet.wszCryMsg[index], this->m_pmCryMsg.m_List[index].wszCryMsg);
  }

  unsigned __int8 type[2]{13, 105};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::pc_NotifyRaceBossCryMsg()
{
  if (this->m_pUserDB)
  {
    this->SendMsg_RaceBossCryMsg();
  }
}

void CPlayer::pc_SetRaceBossCryMsg(unsigned __int8 bySlot, char *pwszCryMsg)
{
  if (this->m_pUserDB && IsSQLValidString(pwszCryMsg))
  {
    CNationSettingManager *nationSetting = CTSingleton<CNationSettingManager>::Instance();
    if (nationSetting->IsNormalString(pwszCryMsg))
    {
      strcpy_0(this->m_pmCryMsg.m_List[bySlot].wszCryMsg, pwszCryMsg);
      this->m_pUserDB->Update_BossCryMsg(bySlot, pwszCryMsg);
    }
  }
}

void CPlayer::pc_AlterWindowInfoRequest(
  unsigned int *pdwSkill,
  unsigned int *pdwForce,
  unsigned int *pdwChar,
  unsigned int *pdwAnimus,
  unsigned int dwInven,
  unsigned int *pdwInvenBag)
{
  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_WindowInfo(pdwSkill, pdwForce, pdwChar, pdwAnimus, dwInven, pdwInvenBag);
  }
}

void CPlayer::pc_MacroUpdate(char *pBuf)
{
  this->m_pUserDB->Update_Macro(pBuf);
}

void CPlayer::pc_RequestTaxRate()
{
  const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
  CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
  if (rankingSystem->GetCurrentRaceBossSerial(raceCode, 5u) == this->m_dwObjSerial)
  {
    CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
    taxRateManager->SendTaxRatePatriarch(this->m_ObjID.m_wIndex, raceCode);
  }
}

void CPlayer::pc_RequestChangeTaxRate(unsigned __int8 byTaxRate)
{
  unsigned __int8 errorCode = 0;
  const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
  CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();

  if (rankingSystem->GetCurrentRaceBossSerial(raceCode, 5u) == this->m_dwObjSerial)
  {
    if (byTaxRate >= 5u && byTaxRate <= 20)
    {
      CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
      const unsigned int suggestedTime = taxRateManager->GetSuggestedTime(raceCode);
      const unsigned int now = GetKorLocalTime();
      if (now - suggestedTime < 60)
      {
        errorCode = 2;
      }
    }
    else
    {
      errorCode = 3;
    }
  }
  else
  {
    errorCode = 1;
  }

  if (errorCode)
  {
    this->SendMsg_ResultChangeTaxRate(errorCode, 255);
  }
  else
  {
    CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
    taxRateManager->SetSuggested(
      raceCode,
      0,
      this->m_Param.GetCharSerial(),
      this->m_Param.GetCharNameW(),
      byTaxRate);
    this->SendMsg_ResultChangeTaxRate(errorCode, byTaxRate);
  }
}

void CPlayer::pc_ChangeModeType(unsigned __int8 nModeType, unsigned __int8 nStandType)
{
  this->m_byModeType = nModeType;
  this->m_byStandType = nStandType;
  this->SenseState();
}

void CPlayer::pc_GestureRequest(unsigned __int8 byGestureType)
{
  this->SendMsg_GestureInform(byGestureType);
}

void CPlayer::pc_ProposeVoteRequest(unsigned __int8 byLimGrade, char *pwszCont)
{
  unsigned __int8 result = 0;
  int raceCode = 0;
  if (!g_Main.IsReleaseServiceMode()
    || ((raceCode = this->m_Param.GetRaceCode()),
        CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
          static_cast<unsigned __int8>(raceCode),
          0)
          == this->m_dwObjSerial))
  {
    if (g_VoteSys[raceCode].m_bActive)
    {
      result = 2;
    }
  }
  else
  {
    result = 1;
  }

  if (!result)
  {
    raceCode = this->m_Param.GetRaceCode();
    if (!g_VoteSys[raceCode].StartVote(pwszCont, byLimGrade, static_cast<unsigned __int8>(raceCode)))
    {
      result = 7;
    }
  }

  this->SendMsg_ProposeVoteResult(static_cast<char>(result));
}

void CPlayer::SetVote(int nSerial)
{
  this->m_nVoteSerial = nSerial;
}

void CPlayer::pc_CastVoteRequest(int nVoteSerial, unsigned __int8 byCode)
{
  unsigned __int8 result = 0;
  const int raceCode = this->m_Param.GetRaceCode();
  if (g_VoteSys[raceCode].m_bActive)
  {
    if (g_VoteSys[raceCode].m_nSerial == nVoteSerial)
    {
      if (this->m_nVoteSerial == nVoteSerial)
      {
        result = 5;
      }
      else if (this->m_Param.m_byPvPGrade < g_VoteSys[raceCode].m_byLimGrade)
      {
        result = 6;
      }
    }
    else
    {
      result = 4;
    }
  }
  else
  {
    result = 3;
  }

  if (!result)
  {
    this->SetVote(nVoteSerial);
    g_VoteSys[raceCode].ActVote(this->m_dwObjSerial, byCode);
  }

  this->SendMsg_CastVoteResult(static_cast<char>(result));
}

void CPlayer::SendMsg_GM_Greeting(char *wszGMName, char *wszMsg)
{
  if (!this->m_pUserDB || this->m_pUserDB->m_bChatLock)
  {
    return;
  }

  _announ_message_receipt_udp msg{};
  msg.byMessageType = 15;
  msg.bySenderRace = this->m_Param.GetRaceCode();
  msg.dwSenderSerial = this->m_dwObjSerial;
  strcpy_s(msg.wszSenderName, sizeof(msg.wszSenderName), wszGMName);
  msg.bySize = static_cast<unsigned __int8>(strlen_0(wszMsg));
  memcpy_0(msg.wszChatData, wszMsg, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;

  unsigned __int8 type[2] = {2, 11};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_RACE_Greeting(char *wszBossName, char *wszMsg)
{
  if (!this->m_pUserDB || this->m_pUserDB->m_bChatLock)
  {
    return;
  }

  _announ_message_receipt_udp msg{};
  msg.byMessageType = 16;
  msg.bySenderRace = this->m_Param.GetRaceCode();
  msg.dwSenderSerial = this->m_dwObjSerial;
  strcpy_s(msg.wszSenderName, sizeof(msg.wszSenderName), wszBossName);
  msg.bySize = static_cast<unsigned __int8>(strlen_0(wszMsg));
  memcpy_0(msg.wszChatData, wszMsg, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;

  unsigned __int8 type[2] = {2, 11};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_GUILD_Greeting(char *wszName, char *wszMsg)
{
  if (!this->m_pUserDB || this->m_pUserDB->m_bChatLock)
  {
    return;
  }

  _announ_message_receipt_udp msg{};
  msg.byMessageType = 17;
  msg.bySenderRace = this->m_Param.GetRaceCode();
  msg.dwSenderSerial = this->m_dwObjSerial;
  strcpy_s(msg.wszSenderName, sizeof(msg.wszSenderName), wszName);
  msg.bySize = static_cast<unsigned __int8>(strlen_0(wszMsg));
  memcpy_0(msg.wszChatData, wszMsg, msg.bySize);
  msg.wszChatData[msg.bySize] = 0;

  unsigned __int8 type[2] = {2, 11};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
}

void CPlayer::SendMsg_Notify_ExceptFromRaceRanking(int bExcepted)
{
  _notify_excepted_from_raceranking packet{};
  packet.bExcepted = bExcepted;
  unsigned __int8 type[2] = {59, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_AlterTownOrField()
{
  _town_or_field_inform_zocl packet{};
  packet.byTown = this->m_byPosRaceTown;
  unsigned __int8 pbyType[2]{4, 34};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_AlterRegionInform(__int16 nRegionIndex)
{
  _alter_region_inform_zocl packet{};
  packet.wRegionIndex = static_cast<unsigned __int16>(nRegionIndex);
  unsigned __int8 pbyType[2]{8, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::CheckPosInTown()
{
  if (this->m_pBeforeTownCheckMap != this->m_pCurMap
      || std::abs(this->m_fCurPos[0] - this->m_fBeforeTownCheckPos[0]) > 50.0f
      || std::abs(this->m_fCurPos[2] - this->m_fBeforeTownCheckPos[1]) > 50.0f)
  {
    const unsigned __int8 raceCode = this->m_Param.GetRaceCode();
    const unsigned __int8 raceTown = this->m_pCurMap->GetRaceTown(this->m_fCurPos, raceCode);
    if (this->m_byPosRaceTown != raceTown || !this->m_pBeforeTownCheckMap)
    {
      this->m_byPosRaceTown = raceTown;
      SendMsg_AlterTownOrField();
    }
    this->m_pBeforeTownCheckMap = this->m_pCurMap;
    this->m_fBeforeTownCheckPos[0] = this->m_fCurPos[0];
    this->m_fBeforeTownCheckPos[1] = this->m_fCurPos[2];
  }
}

void CPlayer::CheckPos_Region()
{
  _bsp_info *bspInfo = this->m_pCurMap->GetBspInfo();
  const float localX = this->m_fCurPos[0] - static_cast<float>(bspInfo->m_nMapMinSize[0]);
  const float localZ = static_cast<float>(bspInfo->m_nMapMaxSize[2]) - this->m_fCurPos[2];

  DWORD startTime = 0;
  if (!g_Main.IsReleaseServiceMode())
  {
    startTime = timeGetTime();
  }

  _map_fld *mapSet = this->m_pCurMap->m_pMapSet;
  const unsigned __int16 regionIndex = static_cast<unsigned __int16>(GetRegionIndex(
    mapSet->m_dwIndex,
    static_cast<int>(localX),
    static_cast<int>(localZ),
    bspInfo->m_nMapSize[0],
    bspInfo->m_nMapSize[2]));

  if (!g_Main.IsReleaseServiceMode())
  {
    const DWORD endTime = timeGetTime();
    const DWORD delta = endTime - startTime;
    if (endTime != startTime)
    {
      __trace("Calc Region time : %d", delta);
    }
  }

  if (regionIndex != this->m_wRegionIndex || this->m_wRegionMapIndex != mapSet->m_dwIndex)
  {
    this->m_wRegionIndex = regionIndex;
    this->m_wRegionMapIndex = mapSet->m_dwIndex;
    SendMsg_AlterRegionInform(this->m_wRegionIndex);

    for (int index = 0; index < 50; ++index)
    {
      _BUDDY_LIST::__list *buddy = &this->m_pmBuddy.m_List[index];
      if (buddy->fill() && buddy->pPtr && buddy->pPtr->m_EP.GetEff_Have(50) <= 0.0f)
      {
        buddy->pPtr->SendMsg_BuddyPosInform(
          this->m_dwObjSerial,
          static_cast<unsigned __int8>(this->m_pCurMap->m_pMapSet->m_dwIndex),
          static_cast<unsigned __int8>(this->m_wRegionIndex));
      }
    }

    if (this->m_Param.m_pGuild && this->m_EP.GetEff_Have(50) <= 0.0f)
    {
      this->m_Param.m_pGuild->SendMsg_GuildMemberPosInform(
        this->m_dwObjSerial,
        this->m_wRegionMapIndex,
        this->m_wRegionIndex);
    }
  }

  this->m_dwLastCheckRegionTime = GetLoopTime();
}

void CPlayer::AlterSec()
{
  if (m_pPartyMgr->IsPartyMode())
  {
    SendData_PartyMemberPos();
  }
}

void CPlayer::SendMsg_TeleportError(char byErrorCode, unsigned int dwMapIndex)
{
  _teleport_error_result_zocl packet{};
  packet.byErrorCode = byErrorCode;
  packet.dwMapIndex = dwMapIndex;

  unsigned __int8 type[2] = {17, 43};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_ApexInform(unsigned __int16 dwRecvSize, char *pMsg)
{
  unsigned __int8 type[2] = {99, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, pMsg, dwRecvSize);
}

void wa_EnterWorld(_WA_AVATOR_CODE *pData, unsigned __int16 wZoneIndex)
{
  CPartyPlayer *partyPlayer = &g_PartyPlayer[static_cast<unsigned int>(pData->m_id.wIndex)];
  if (!partyPlayer->m_bLogin)
  {
    partyPlayer->EnterWorld(pData, wZoneIndex);
  }
}

