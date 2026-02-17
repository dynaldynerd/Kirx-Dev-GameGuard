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

  char payload[6]{};
  *reinterpret_cast<unsigned int *>(payload) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 4) = this->m_wPointRate_PartySend[0];

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 20};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, payload, 6u);
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

  char payload[6]{};
  *reinterpret_cast<unsigned int *>(payload) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 4) = this->m_wPointRate_PartySend[1];

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 21};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, payload, 6u);
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

  char payload[6]{};
  *reinterpret_cast<unsigned int *>(payload) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 4) = this->m_wPointRate_PartySend[2];

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 22};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, payload, 6u);
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

  char payload[8]{};
  *reinterpret_cast<unsigned int *>(payload) = this->m_dwObjSerial;
  payload[4] = static_cast<char>(byAlterCode);
  *reinterpret_cast<unsigned __int16 *>(payload + 5) = wEffectCode;
  payload[7] = static_cast<char>(byLv);

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 26};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, payload, 8u);
    }
  }
}

void CPlayer::SendMsg_TargetObjectHPInform()
{
  char payload[8]{};
  payload[0] = static_cast<char>(this->m_TargetObject.byKind);
  payload[1] = static_cast<char>(this->m_TargetObject.byID);
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_TargetObject.dwSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 6) = this->m_TargetObject.wHPRate;

  unsigned __int8 type[2] = {13, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 8u);
}

void CPlayer::SendMsg_RefeshGroupTargetPosition(char byGroupType)
{
  char payload[0x13]{};
  payload[0] = byGroupType;

  const unsigned __int8 groupIndex = static_cast<unsigned __int8>(byGroupType);
  CGameObject *groupObject = this->m_GroupTargetObject[groupIndex].pObject;
  payload[1] = static_cast<char>(groupObject->m_pCurMap->m_pMapSet->m_dwIndex);
  payload[2] = static_cast<char>(this->m_GroupTargetObject[groupIndex].byID);
  *reinterpret_cast<unsigned int *>(payload + 3) = this->m_GroupTargetObject[groupIndex].dwSerial;
  memcpy_0(payload + 7, groupObject->m_fCurPos, 0xCuLL);

  unsigned __int8 type[2] = {13, 110};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x13u);
}

void CPlayer::SendMsg_BuddyNameReNewal(unsigned int dwSerial, char *wszName)
{
  char payload[0x15]{};
  *reinterpret_cast<unsigned int *>(payload) = dwSerial;
  strcpy_s(payload + 4, 0x11u, wszName);

  unsigned __int8 type[2] = {31, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x15u);
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

    if (loopTime - this->m_dwLastGroupMapPointTime[0] < 0xEA60)
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
        groupPlayer->SendMsg_SetGroupMapPoint(0, 0, mapCode, pzTar, 0x3Cu);
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

    if (loopTime - this->m_dwLastGroupMapPointTime[byGroupType] < 0x57E40)
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
        groupPlayer->SendMsg_SetGroupMapPoint(0, byGroupType, mapCode, pzTar, 0x3Cu);
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

    if (loopTime - this->m_dwLastGroupMapPointTime[byGroupType] < 0x2BF20)
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
        groupPlayer->SendMsg_SetGroupMapPoint(0, byGroupType, mapCode, pzTar, 0x3Cu);
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
  char msg[5]{};
  msg[0] = byRetCode;
  std::memcpy(&msg[1], &dwSerial, sizeof(dwSerial));

  unsigned __int8 type[2] = {31, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 5u);
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

