#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "CAnimus.h"
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
#include "CParkingUnit.h"
#include "CBillingManager.h"
#include "PatriarchElectProcessor.h"
#include "TimeLimitJadeMng.h"
#include "TimeLimitJade.h"
#include "AutominePersonalMgr.h"
#include "GuildBattle.h"
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
#include "MasteryLimit_fld.h"
#include "skill_fld.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace
{
unsigned int s_ExpNotifyIndex = 0;
}

void CPlayer::SendMsg_MaxHFSP()
{
  _max_hfsp_zocl payload{};
  payload.wMaxHP = static_cast<unsigned __int16>(this->GetMaxHP());
  payload.wMaxFP = static_cast<unsigned __int16>(this->GetMaxFP());
  payload.wMaxST = static_cast<unsigned __int16>(this->GetMaxSP());

  unsigned __int8 type[2] = {11, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::CheckAlterMaxPoint()
{
  bool bAlter = false;

  const int maxHP = static_cast<int>(GetMaxHP());
  if (m_nOldPoint[0] != maxHP)
  {
    bAlter = true;
    m_nOldPoint[0] = maxHP;
  }

  const int maxFP = GetMaxFP();
  if (m_nOldPoint[1] != maxFP)
  {
    bAlter = true;
    m_nOldPoint[1] = maxFP;
  }

  const int maxSP = GetMaxSP();
  if (m_nOldPoint[2] != maxSP)
  {
    bAlter = true;
    m_nOldPoint[2] = maxSP;
  }

  if (bAlter)
  {
    SendMsg_MaxHFSP();
    SendMsg_Recover();
  }

  const int maxDP = GetMaxDP();
  if (m_nOldMaxDP != maxDP)
  {
    if (GetDP() > maxDP)
    {
      SetDP(maxDP, false);
      SendMsg_SetDPInform();
    }

    SendMsg_AlterMaxDP();
    m_nOldMaxDP = GetMaxDP();
  }
}

void CPlayer::SendData_PartyMemberMaxHFSP()
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

  _party_member_max_hfsp_upd payload{};
  payload.dwMemSerial = this->m_dwObjSerial;
  payload.wMaxHP = static_cast<unsigned __int16>(this->GetMaxHP());
  payload.wMaxFP = static_cast<unsigned __int16>(this->GetMaxFP());
  payload.wMaxSP = static_cast<unsigned __int16>(this->GetMaxSP());

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 25};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[index]->m_wZoneIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
    }
  }
}

void CPlayer::SendMsg_AlterHPInform()
{
  _alter_hp_inform_zocl payload{};
  payload.wHP = static_cast<unsigned __int16>(this->GetHP());

  unsigned __int8 type[2] = {11, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::SendMsg_Level(char nLevel)
{
  _level_up_zocl msg{};
  msg.dwSerial = this->m_dwObjSerial;
  msg.byLevel = nLevel;

  unsigned __int8 type[2] = {11, 1};
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(sizeof(msg)), true);
}

void CPlayer::SendData_PartyMemberLv()
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

  _party_member_lv_upd msg{};
  msg.dwMemSerial = this->m_dwObjSerial;
  msg.byLv = static_cast<char>(this->m_Param.GetLevel());

  const int memberCount = this->m_pPartyMgr->GetPopPartyMember();
  unsigned __int8 type[2] = {16, 23};

  for (int i = 0; i < memberCount; ++i)
  {
    if (partyMembers[i] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        partyMembers[i]->m_wZoneIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
    }
  }
}

void CPlayer::SendMsg_AlterExpInform()
{
  _exp_alter_zocl msg{};
  msg.dwExpRate = this->m_dwExpRate;

  unsigned __int8 type[2] = {11, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_NotifyGetExpInfo(long double dOldExp, long double dAlterExp, long double dCurExp)
{
  if (!g_Main.IsReleaseServiceMode())
  {
    _trans_gm_msg_inform_zocl msg{};
    std::memset(&msg, 0, sizeof(msg));
    msg.wMsgSize = _snprintf(
      msg.wszChatData,
      255,
      "%u Old Exp     : %f\n   Alter Exp   : %f\n   Cur Exp     : %f\n",
      ++s_ExpNotifyIndex,
      static_cast<double>(dOldExp),
      static_cast<double>(dAlterExp),
      static_cast<double>(dCurExp));

    unsigned __int8 type[2] = {2, 13};
    const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
}

void CPlayer::SendMsg_EquipItemLevelLimit(int nCurPlayerLv)
{
  _equip_up_item_lv_limit_zocl msg{};

  for (int i = 0; i < 8; ++i)
  {
    _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbEquip.m_pStorageList[i];
    if (!item->m_bLoad)
    {
      continue;
    }

    const int limitLevel = GetItemEquipUpLevel(item->m_byTableCode, item->m_wItemIndex);
    if (limitLevel <= 0 || nCurPlayerLv <= limitLevel)
    {
      continue;
    }

    switch (item->m_byTableCode)
    {
      case 0:
        msg.byEquipItemNum[1] = 1;
        break;
      case 1:
        msg.byEquipItemNum[2] = 1;
        break;
      case 2:
        msg.byEquipItemNum[7] = 1;
        break;
      case 3:
        msg.byEquipItemNum[4] = 1;
        break;
      case 4:
        msg.byEquipItemNum[0] = 1;
        break;
      case 5:
        msg.byEquipItemNum[6] = 1;
        break;
      case 6:
        msg.byEquipItemNum[3] = 1;
        break;
      case 7:
        msg.byEquipItemNum[5] = 1;
        break;
      default:
        break;
    }
  }

  unsigned __int8 type[2] = {11, 35};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    8u);
}

void CPlayer::AlterExp(
  double dAlterExp,
  bool bReward,
  bool bUseExpRecoverItem,
  bool bUseExpAdditionItem)
{
  const double playerPenalty = static_cast<double>(
    g_Main.m_pTimeLimitMgr->GetPlayerPenalty(this->m_id.wIndex));
  const double baseAlterExp = dAlterExp;

  if (!((this->m_byUserDgr && this->m_byUserDgr != 1)
        || this->m_Param.GetLevel() < this->m_Param.GetMaxLevel()))
  {
    return;
  }

  const int level = this->m_Param.GetLevel();
  const int maxLevel = this->m_Param.GetMaxLevel();
  if (level >= maxLevel || dAlterExp == 0.0)
  {
    return;
  }

  double adjustedAlterExp = 0.0;
  if (!IsApplyPcbangPrimium() || bReward)
  {
    const float rate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PLAYER_EXP_RATE;
    adjustedAlterExp = dAlterExp + baseAlterExp * static_cast<double>(rate - 1.0f);
  }
  else
  {
    const float rate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PCBANG_PRIMIUM_FAVOR::PLAYER_EXP;
    adjustedAlterExp = dAlterExp + baseAlterExp * static_cast<double>(rate - 1.0f);
  }

  if (adjustedAlterExp > 0.0 && !bUseExpRecoverItem && !bReward && !this->m_EP.m_bLock)
  {
    float extraRate = 0.0f;
    extraRate += this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_2) - 1.0f;
    extraRate += this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_5) - 1.0f;
    extraRate += this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_9) - 1.0f;

    if (IsApplyPcbangPrimium())
    {
      const float baseRate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PCBANG_PRIMIUM_FAVOR::PLAYER_EXP;
      const float eff71 = this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_71);
      if (eff71 > baseRate)
      {
        extraRate += eff71 - baseRate;
      }
      extraRate += this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_72) - 1.0f;
    }
    else
    {
      extraRate += this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_71) - 1.0f;
      extraRate += this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_72) - 1.0f;
    }

    if (extraRate > 0.0f)
    {
      adjustedAlterExp += baseAlterExp * static_cast<double>(extraRate);
    }
  }

  double finalAlterExp = 0.0;
  if (bUseExpRecoverItem)
  {
    finalAlterExp = baseAlterExp;
    this->m_Param.SetLossExp(0.0);
    this->m_pUserDB->Update_LossExp(0.0);
  }
  else if (bUseExpAdditionItem)
  {
    finalAlterExp = baseAlterExp;
    if (baseAlterExp < 0.0)
    {
      this->m_Param.SetLossExp(-baseAlterExp);
      this->m_pUserDB->Update_LossExp(-finalAlterExp);
    }
  }
  else
  {
    const float rate34 = this->m_EP.GetEff_Rate(EFF_RATE_EXPERIENCE_GAIN);
    const double withRate34 = adjustedAlterExp + baseAlterExp * static_cast<double>(rate34 - 1.0f);
    const float rate38 = this->m_EP.GetEff_Rate(EFF_RATE_UNKNOWN_38);
    finalAlterExp = withRate34 + baseAlterExp * static_cast<double>(rate38 - 1.0f);
    if (baseAlterExp < 0.0)
    {
      this->m_Param.SetLossExp(-finalAlterExp);
      this->m_pUserDB->Update_LossExp(-finalAlterExp);
    }
  }

  const long double penalizedAlter = static_cast<long double>(finalAlterExp * playerPenalty);
  const long double oldExp = this->m_Param.GetExp();
  const long double curExp = oldExp + penalizedAlter;
  SendMsg_NotifyGetExpInfo(oldExp, penalizedAlter, curExp);

  const int curLevel = this->m_Param.GetLevel();
  const long double limitExp = cStaticMember_Player::Instance()->GetLimitExp(curLevel);
  if (curExp < limitExp)
  {
    this->m_Param.SetExp(curExp);
    const long double exp = this->m_Param.GetExp();
    const int newLevel = this->m_Param.GetLevel();
    const long double newLimit = cStaticMember_Player::Instance()->GetLimitExp(newLevel);
    this->m_dwExpRate = static_cast<int>(exp / newLimit * 1000000.0);
    SendMsg_AlterExpInform();
  }
  else
  {
    const unsigned __int8 oldLevel = static_cast<unsigned __int8>(GetLevel());
    const unsigned __int8 nextLevel = static_cast<unsigned __int8>(oldLevel + 1);
    SetLevel(nextLevel);

    if (bReward)
    {
      if (this->m_Param.GetMaxLevel() > this->m_Param.GetLevel())
      {
        const long double oldLimitExp = cStaticMember_Player::Instance()->GetLimitExp(oldLevel);
        this->m_Param.SetExp(curExp - oldLimitExp);
        const long double exp = this->m_Param.GetExp();
        const long double newLimit = cStaticMember_Player::Instance()->GetLimitExp(nextLevel);
        this->m_dwExpRate = static_cast<int>(exp / newLimit * 1000000.0);
      }
      else
      {
        this->m_Param.SetExp(0.0);
        this->m_dwExpRate = 0;
      }
      SendMsg_AlterExpInform();
    }
    else
    {
      this->m_Param.SetExp(0.0);
      this->m_dwExpRate = 0;
      SendMsg_AlterExpInform();
    }

    this->m_bDownCheckEquipEffect = 1;
    SendMsg_EquipItemLevelLimit(nextLevel);
  }

  if (this->m_pUserDB)
  {
    const long double exp = this->m_Param.GetExp();
    this->m_pUserDB->Update_Exp(exp);
  }
}

void CPlayer::AlterExp_Potion(long double dAlterExp)
{
  if (!((this->m_byUserDgr && this->m_byUserDgr != 1) || this->m_Param.GetMaxLevel() != this->m_Param.GetLevel()))
  {
    return;
  }

  if (this->m_Param.GetMaxLevel() == this->m_Param.GetLevel())
  {
    return;
  }

  const float playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(this->m_id.wIndex);
  const long double finalAlterExp = dAlterExp * static_cast<long double>(playerPenalty);
  if (finalAlterExp <= 0.0)
  {
    return;
  }

  const long double oldExp = this->m_Param.GetExp();
  const long double curExp = oldExp + finalAlterExp;
  SendMsg_NotifyGetExpInfo(oldExp, finalAlterExp, curExp);

  const int lv = this->m_Param.GetLevel();
  const long double limitExp = cStaticMember_Player::Instance()->GetLimitExp(lv);
  if (curExp < limitExp)
  {
    this->m_Param.SetExp(curExp);
    const long double exp = this->m_Param.GetExp();
    const int curLevel = this->m_Param.GetLevel();
    const long double nextLimitExp = cStaticMember_Player::Instance()->GetLimitExp(curLevel);
    this->m_dwExpRate = static_cast<int>(exp / nextLimitExp * 1000000.0);
    SendMsg_AlterExpInform();
  }
  else
  {
    const unsigned __int8 oldLevel = static_cast<unsigned __int8>(GetLevel());
    const unsigned __int8 nextLevel = static_cast<unsigned __int8>(oldLevel + 1);
    SetLevel(nextLevel);

    if (this->m_Param.GetMaxLevel() > this->m_Param.GetLevel())
    {
      const long double oldLimitExp = cStaticMember_Player::Instance()->GetLimitExp(oldLevel);
      this->m_Param.SetExp(curExp - oldLimitExp);
      const long double exp = this->m_Param.GetExp();
      const long double nextLimitExp = cStaticMember_Player::Instance()->GetLimitExp(nextLevel);
      this->m_dwExpRate = static_cast<int>(exp / nextLimitExp * 1000000.0);
    }
    else
    {
      this->m_Param.SetExp(0.0);
      this->m_dwExpRate = 0;
    }

    SendMsg_AlterExpInform();
    this->m_bDownCheckEquipEffect = 1;
    SendMsg_EquipItemLevelLimit(nextLevel);
  }

  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_Exp(this->m_Param.GetExp());
  }
}

void CPlayer::SetLevel(unsigned __int8 byNewLevel)
{
  if ((this->m_byUserDgr && this->m_byUserDgr != 1) || byNewLevel <= this->m_Param.GetMaxLevel())
  {
    const unsigned __int8 level = this->m_Param.GetLevel();
    if (level < byNewLevel)
    {
      if (this->m_pUserDB)
      {
        const long double exp = this->m_Param.GetExp();
        this->m_pUserDB->Update_Level(byNewLevel, exp);
      }
      this->m_Param.SetLevel(byNewLevel);
      this->SetGrade(CPlayerDB::CalcCharGrade(byNewLevel, this->m_Param.m_dbChar.m_wRankRate));

      char *levelStr = cvt_string(byNewLevel);
      if (!this->Emb_CreateQuestEvent(quest_happen_type_lv, levelStr))
      {
        char *levelStr2 = cvt_string(byNewLevel);
        this->Emb_CheckActForQuest(6, levelStr2, 1u, 0);
      }

      if (byNewLevel == this->m_Param.GetMaxLevel())
      {
        char *levelStr3 = cvt_string(byNewLevel);
        this->Emb_CreateQuestEvent(quest_happen_type_maxlevel, levelStr3);
      }

      this->m_bUpCheckEquipEffect = 1;
      this->ReCalcMaxHFSP(1, 0);
      this->SetHP(this->GetMaxHP(), 1);
      this->SetFP(this->GetMaxFP(), 1);
      this->SetSP(this->GetMaxSP(), 1);
      this->SendMsg_Level(byNewLevel);
      this->SendData_PartyMemberLv();
      if (this->m_Param.IsClassChangeableLv())
      {
        this->SendMsg_ChangeClassCommand();
      }

      CPlayer::s_MgrLvHistory.upgrade_lv(
        this->m_ObjID.m_wIndex,
        byNewLevel,
        this->m_Param.m_byPvPGrade,
        this->m_nMaxPoint,
        this->m_szLvHistoryFileName);

      _quest_fail_result *lim = this->m_QuestMgr.CheckLimLv(byNewLevel);
      if (lim)
      {
        this->LimLvNpcQuestDelete(lim->m_List[0].byQuestDBSlot);
      }
    }
  }
}

void CPlayer::SetLevelD(unsigned __int8 byDownLevel)
{
  const unsigned __int8 level = this->m_Param.GetLevel();
  if (level > byDownLevel)
  {
    if (this->m_pUserDB)
    {
      const long double exp = this->m_Param.GetExp();
      this->m_pUserDB->Update_Level(byDownLevel, exp);
    }
    this->m_Param.SetLevel(byDownLevel);
    this->SetGrade(CPlayerDB::CalcCharGrade(byDownLevel, this->m_Param.m_dbChar.m_wRankRate));
    this->ReCalcMaxHFSP(1, 0);
    this->SetHP(this->GetMaxHP(), 1);
    this->SetFP(this->GetMaxFP(), 1);
    this->SetSP(this->GetMaxSP(), 1);
    this->SendMsg_Level(byDownLevel);
    this->SendData_PartyMemberLv();
    this->m_bDownCheckEquipEffect = 1;
    CPlayer::s_MgrLvHistory.downgrade_lv(
      this->m_ObjID.m_wIndex,
      byDownLevel,
      this->m_Param.m_byPvPGrade,
      this->m_nMaxPoint,
      this->m_szLvHistoryFileName);
  }
}

int CPlayer::GetGauge(int nParamCode)
{
  if (!nParamCode)
  {
    return GetHP();
  }
  if (nParamCode == 1)
  {
    return GetFP();
  }
  if (nParamCode == 2)
  {
    return GetSP();
  }
  return 0;
}

void CPlayer::SetGauge(int nParamCode, int nValue, bool bOver)
{
  if (!nParamCode)
  {
    SetHP(nValue, bOver);
    return;
  }

  if (nParamCode == 1)
  {
    SetFP(nValue, bOver);
  }
  else if (nParamCode == 2)
  {
    SetSP(nValue, bOver);
  }
}

void CPlayer::SendMsg_SetFPInform()
{
  _set_fp_inform_zocl fp{};
  fp.wFP = static_cast<unsigned __int16>(this->m_Param.GetFP());
  unsigned __int8 type[2] = {17, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&fp), sizeof(fp));
}

void CPlayer::SendMsg_SetSPInform()
{
  _set_sp_inform_zocl sp{};
  sp.wSP = static_cast<unsigned __int16>(this->m_Param.GetSP());
  unsigned __int8 type[2] = {17, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&sp), sizeof(sp));
}

void CPlayer::SendMsg_AlterSPInform()
{
  _alter_sp_inform_zocl sp{};
  sp.wNewSP = static_cast<unsigned __int16>(GetSP());
  unsigned __int8 type[2] = {11, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&sp), sizeof(sp));
}

void CPlayer::SendMsg_AlterTol()
{
  _alter_tol_inform_zocl tol{};
  std::memcpy(tol.zTol, this->m_zLastTol, sizeof(tol.zTol));
  unsigned __int8 type[2] = {11, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&tol), sizeof(tol));
}

void CPlayer::OnLoop_Static()
{
  CPlayer::s_MgrItemHistory.OnLoop();
  CPlayer::s_MgrLvHistory.OnLoop();
  CPlayer::s_AnimusReturnDelay.CheckOnLoop();
  CPlayer::s_BillingForceCloseDelay.CheckOnLoop();
}

void CPlayer::UpdatePvpPointLimiter(__int64 tCurTime)
{
  const long double pvpPoint = m_Param.GetPvPPoint();
  m_kPvpPointLimiter.Clear(tCurTime, pvpPoint, this);
}

void CPlayer::UpdatePvpOrderView(__int64 tCurTime)
{
  const long double pvpTempCash = m_kPvpOrderView.GetPvpTempCash();
  const long double pvpCashBefore = m_kPvpOrderView.GetPvpCash();
  CPlayer::s_MgrLvHistory.adjust_pvpcash(false, pvpCashBefore, pvpTempCash, m_szLvHistoryFileName);

  AlterPvPCashBag(pvpTempCash, pm_kill);
  m_kPvpOrderView.Update_RaceWarRecvr(false);

  const long double pvpPoint = m_Param.GetPvPPoint();
  m_kPvpOrderView.Update(tCurTime, false, false, 0.0, pvpPoint, 0.0);
  m_kPvpOrderView.ResetPvPOrderView();
  m_kPvpOrderView.Notify_PvpTempCash(m_ObjID.m_wIndex);
  m_kPvpOrderView.Notify_OrderView(m_ObjID.m_wIndex);
  SendMsg_AlterPvPCash(0);

  const long double pvpCashAfter = m_kPvpOrderView.GetPvpCash();
  const long double pvpTempCashAfter = m_kPvpOrderView.GetPvpTempCash();
  CPlayer::s_MgrLvHistory.adjust_pvpcash(true, pvpCashAfter, pvpTempCashAfter, m_szLvHistoryFileName);
}

void CPlayer::AutoRecover()
{
  float moveDelta = 1.0f;
  if (!m_byUserDgr)
  {
    if (m_byLastRecvMapIndex == m_pCurMap->m_pMapSet->m_dwIndex)
    {
      const float deltaX = std::fabs(m_fCurPos[0] - m_fLastRecvPos[0]);
      const float deltaZ = std::fabs(m_fCurPos[2] - m_fLastRecvPos[2]);
      moveDelta = (deltaX > deltaZ) ? deltaX : deltaZ;
    }

    if (moveDelta > 400.0f)
    {
      OutOfMap(m_pCurMap, m_wMapLayerIndex, 4u, m_fLastRecvPos);
      SendMsg_GotoRecallResult(0, static_cast<unsigned __int8>(m_pCurMap->m_pMapSet->m_dwIndex), m_fLastRecvPos, 4u);
      g_Main.m_logMove.Write(
        "move dist check >> char: %s (%d) id: %s (%d) dist: %d, map: %d",
        m_Param.GetCharNameA(),
        m_dwObjSerial,
        m_pUserDB->m_szAccountID,
        m_pUserDB->m_dwAccountSerial,
        static_cast<int>(moveDelta),
        m_pCurMap->m_pMapSet->m_dwIndex);
      return;
    }

    std::memcpy(m_fLastRecvPos, m_fCurPos, sizeof(m_fLastRecvPos));
    m_byLastRecvMapIndex = static_cast<unsigned __int8>(m_pCurMap->m_pMapSet->m_dwIndex);
  }

  if (IsRidingUnit())
  {
    return;
  }

  const int oldHp = GetHP();
  const int oldFp = GetFP();
  const int oldSp = GetSP();
  const int oldDp = GetDP();

  int hpDelta = 0;
  int fpDelta = 0;
  int spDelta = 0;
  int dpDelta = 0;

  if (!m_EP.GetEff_State(EFF_STATE_UNKNOWN_10))
  {
    if (!m_EP.GetEff_State(EFF_STATE_UNKNOWN_15))
    {
      if (oldHp < GetMaxHP())
      {
        int baseRecover = 0;
        if (m_bMove)
        {
          baseRecover = m_byMoveType ? 0 : 2;
        }
        else
        {
          baseRecover = (m_byStandType == 1) ? 6 : 4;
        }

        if (baseRecover > 0)
        {
          hpDelta = static_cast<int>(static_cast<float>(baseRecover) * m_EP.GetEff_Rate(EFF_RATE_HP_RECOVERY));
        }
      }
    }

    if (!m_EP.GetEff_State(EFF_STATE_UNKNOWN_16))
    {
      if (oldFp < GetMaxFP())
      {
        const int fpPlus = static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_FP_RECOVERY_RATE_POINTS));
        const float fpRate = m_EP.GetEff_Rate(EFF_RATE_FP_RECOVERY);
        if (m_bMove)
        {
          if (!m_byMoveType)
          {
            fpDelta = static_cast<int>((1.0f * fpRate) + static_cast<float>(fpPlus / 100));
          }
        }
        else
        {
          const float standMul = (m_byStandType == 1) ? 3.0f : 2.0f;
          fpDelta = static_cast<int>((standMul * fpRate) + static_cast<float>(fpPlus / 100));
        }
      }
    }

    if (!m_EP.GetEff_State(EFF_STATE_UNKNOWN_17))
    {
      const float spRate = m_EP.GetEff_Rate(EFF_RATE_SP_RECOVERY);
      if (m_bMove)
      {
        if (!m_byMoveType)
        {
          spDelta = static_cast<int>(1.0f * spRate);
        }
      }
      else
      {
        const float standMul = (m_byStandType == 1) ? 3.0f : 2.0f;
        spDelta = static_cast<int>(standMul * spRate);
      }
    }
  }

  if (!spDelta && moveDelta > 0.0f)
  {
    if (m_bMove && m_byMoveType == 1 && m_byPosRaceTown != m_Param.GetRaceCode())
    {
      spDelta = -2;
    }
    else if (m_bMove && m_byMoveType == 2 && m_byPosRaceTown != m_Param.GetRaceCode())
    {
      spDelta = -60;
    }
  }

  if (oldDp < GetMaxDP())
  {
    dpDelta = s_nRevDefPoint;
  }

  if (m_EP.GetEff_Plus(EFF_PLUS_HP_RECOVERY_FLAT) != 0.0f)
  {
    hpDelta += static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_HP_RECOVERY_FLAT));
  }
  if (m_EP.GetEff_Plus(EFF_PLUS_FP_RECOVERY_FLAT) != 0.0f)
  {
    fpDelta += static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_FP_RECOVERY_FLAT));
  }
  if (m_EP.GetEff_Plus(EFF_PLUS_SP_RECOVERY_FLAT) != 0.0f)
  {
    spDelta += static_cast<int>(m_EP.GetEff_Plus(EFF_PLUS_SP_RECOVERY_FLAT));
  }

  if (m_EP.GetEff_Have(EFF_HAVE_HP_RESTORE_FROM_MAX_HP_RATE) > 0.0f)
  {
    hpDelta += static_cast<int>(m_EP.GetEff_Have(EFF_HAVE_HP_RESTORE_FROM_MAX_HP_RATE) * static_cast<float>(GetMaxHP()));
  }
  if (m_EP.GetEff_Have(EFF_HAVE_FP_RESTORE_FROM_MAX_FP_RATE) > 0.0f)
  {
    fpDelta += static_cast<int>(m_EP.GetEff_Have(EFF_HAVE_FP_RESTORE_FROM_MAX_FP_RATE) * static_cast<float>(GetMaxFP()));
  }
  if (m_EP.GetEff_Have(EFF_HAVE_SP_RESTORE_FROM_MAX_SP_RATE) > 0.0f)
  {
    spDelta += static_cast<int>(m_EP.GetEff_Have(EFF_HAVE_SP_RESTORE_FROM_MAX_SP_RATE) * static_cast<float>(GetMaxSP()));
  }

  if (hpDelta)
  {
    if (hpDelta < 0)
    {
      const int minHp = GetMaxHP() / 10;
      if (hpDelta + oldHp <= minHp)
      {
        hpDelta = 0;
      }
    }

    if (hpDelta)
    {
      SetHP(oldHp + hpDelta, false);
    }
  }

  if (fpDelta)
  {
    SetFP(oldFp + fpDelta, false);
  }
  if (spDelta)
  {
    SetSP(oldSp + spDelta, false);
  }
  if (dpDelta)
  {
    SetDP(oldDp + dpDelta, false);
  }

  if (oldHp > GetMaxHP())
  {
    SetHP(GetMaxHP(), false);
  }
  if (oldDp > GetMaxDP())
  {
    SetDP(GetMaxDP(), false);
  }
  if (oldFp > GetMaxFP())
  {
    SetFP(GetMaxFP(), false);
  }
  if (oldSp > GetMaxSP())
  {
    SetSP(GetMaxSP(), false);
  }

  if (oldHp != GetHP() || oldFp != GetFP() || oldSp != GetSP() || oldDp != GetDP())
  {
    SendMsg_Recover();
  }
}

unsigned int CPlayer::SumMinuteOne(_SYSTEMTIME *tm)
{
  int dayCount = 0;
  int totalDays = 0;

  for (int monthIndex = 0; monthIndex < tm->wMonth - 1; ++monthIndex)
  {
    if (tm->wMonth == 2)
    {
      if ((((tm->wYear & 3) != 0) && (tm->wYear % 100)) || (tm->wYear % 400))
      {
        dayCount = 29;
      }
      else
      {
        dayCount = 28;
      }
    }
    else if (tm->wMonth == 4 || tm->wMonth == 6 || tm->wMonth == 9 || tm->wMonth == 11)
    {
      dayCount = 30;
    }
    else
    {
      dayCount = 31;
    }

    totalDays += dayCount;
  }

  totalDays += tm->wDay;
  totalDays *= 1440;
  return static_cast<unsigned int>(60 * tm->wHour + totalDays + tm->wMinute);
}

unsigned int CPlayer::SumMinuteBetween(_SYSTEMTIME *tmLast, _SYSTEMTIME *tmLocal)
{
  const unsigned __int16 yearDiff = tmLocal->wYear - tmLast->wYear;
  unsigned __int16 betweenYearCount = 0;
  if (yearDiff)
  {
    betweenYearCount = static_cast<unsigned __int16>(yearDiff - 1);
  }

  unsigned int totalMinutes = 0;
  if (betweenYearCount <= 1u)
  {
    const unsigned int localSum = SumMinuteOne(tmLocal);
    const unsigned int lastSum = SumMinuteOne(tmLast);
    return localSum - lastSum;
  }

  _SYSTEMTIME yearMarks[126]{};
  std::memcpy(&yearMarks[0], tmLast, sizeof(_SYSTEMTIME));
  std::memcpy(&yearMarks[yearDiff], tmLocal, sizeof(_SYSTEMTIME));

  if (betweenYearCount)
  {
    _SYSTEMTIME yearEnd{};
    yearEnd.wMonth = 12;
    yearEnd.wDay = 31;
    yearEnd.wHour = 23;
    yearEnd.wMinute = 59;

    for (int j = 0; j < betweenYearCount; ++j)
    {
      yearEnd.wYear = static_cast<unsigned __int16>(tmLast->wYear + j + 1);
      std::memcpy(&yearMarks[j + 1], &yearEnd, sizeof(_SYSTEMTIME));
    }
  }

  _SYSTEMTIME firstYearEnd{};
  firstYearEnd.wYear = tmLast->wYear;
  firstYearEnd.wMonth = 12;
  firstYearEnd.wDay = 31;
  firstYearEnd.wHour = 23;
  firstYearEnd.wMinute = 59;

  totalMinutes = SumMinuteOne(&firstYearEnd) - SumMinuteOne(tmLast);
  totalMinutes += SumMinuteOne(tmLocal);

  for (int k = 1; k < betweenYearCount; ++k)
  {
    totalMinutes += SumMinuteOne(&yearMarks[k]);
  }

  return totalMinutes;
}

void CPlayer::_CheckForcePullUnit()
{
  if (m_pParkingUnit)
  {
    const unsigned int currentTime = timeGetTime();
    const unsigned int elapsedTime = currentTime - m_dwLastTimeCheckUnitViewOver;
    if (elapsedTime > 2000)
    {
      m_dwLastTimeCheckUnitViewOver = currentTime;
      if (m_pCurMap == m_pParkingUnit->m_pCurMap
          && GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) < 540.0f
          && std::fabs(m_fCurPos[1] - m_pParkingUnit->m_fCurPos[1]) < 100.0f)
      {
        m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
      }
      else if (m_dwUnitViewOverTime == static_cast<unsigned int>(-1))
      {
        m_dwUnitViewOverTime = currentTime;
      }
      else if (currentTime - m_dwUnitViewOverTime > 60000)
      {
        ForcePullUnit(false);
      }
    }
  }
}

void CPlayer::UpdatedMasteryWriteHistory()
{
  const unsigned int loopTime = GetLoopTime();
  const unsigned int elapsed = loopTime - m_dwUMWHLastTime;
  if (elapsed > 900000)
  {
    m_dwUMWHLastTime = loopTime;
    s_MgrLvHistory.update_mastery(
      m_ObjID.m_wIndex,
      m_byUserDgr,
      m_Param.GetLevel(),
      m_Param.GetExp(),
      m_dwExpRate,
      m_Param.m_byPvPGrade,
      m_nMaxPoint,
      &m_pmMst,
      m_Param.m_dwAlterMastery,
      m_szLvHistoryFileName,
      0,
      nullptr);
    m_Param.InitAlterMastery();
  }
}

void CPlayer::_check_hp_send_party()
{
  if (m_pPartyMgr->IsPartyMode())
  {
    const int hpRate = static_cast<int>(CalcCurHPRate());
    if (std::abs(hpRate - static_cast<int>(m_wPointRate_PartySend[0])) > 500)
    {
      m_wPointRate_PartySend[0] = hpRate;
      SendData_PartyMemberHP();
    }

    const int fpRate = static_cast<int>(CalcCurFPRate());
    if (std::abs(fpRate - static_cast<int>(m_wPointRate_PartySend[1])) > 500)
    {
      m_wPointRate_PartySend[1] = fpRate;
      SendData_PartyMemberFP();
    }

    const int spRate = static_cast<int>(CalcCurSPRate());
    if (std::abs(spRate - static_cast<int>(m_wPointRate_PartySend[2])) > 500)
    {
      m_wPointRate_PartySend[2] = spRate;
      SendData_PartyMemberSP();
    }
  }
}

void CPlayer::UpdateChaosModeState(unsigned int dwCurTime)
{
  if (m_nChaosMode)
  {
    if (dwCurTime <= m_dwChaosModeEndTime)
    {
      if (m_nChaosMode == 1 && m_dwChaosModeEndTime - dwCurTime < m_dwChaosModeTime10Per)
      {
        m_nChaosMode = 2;
      }
      else if (m_nChaosMode == 2 && m_dwChaosModeEndTime - dwCurTime >= m_dwChaosModeTime10Per)
      {
        m_nChaosMode = 1;
      }
    }
    else
    {
      m_nChaosMode = 0;
    }
  }
}

void CPlayer::CheckBattleMode()
{
  if (m_byBattleMode)
  {
    DWORD delay = 0;
    if (m_byBattleMode == 1)
    {
      delay = 15000;
    }
    else
    {
      delay = 10000;
    }

    if (timeGetTime() - m_dwBattleTime >= delay)
    {
      m_byBattleMode = 0;
      m_dwBattleTime = 0;
    }
  }
}

void CPlayer::CheckNameChange()
{
  if (m_NameChangeBuddyInfo.bNameChange)
  {
    int checkedCount = 0;
    while (m_NameChangeBuddyInfo.nSendNum < MAX_PLAYER && checkedCount < 100)
    {
      ++checkedCount;

      CPlayer *target = &g_Player[m_NameChangeBuddyInfo.nSendNum];
      if (target->m_bLive && this != target)
      {
        const int targetRaceCode = target->m_Param.GetRaceCode();
        if (targetRaceCode == m_Param.GetRaceCode())
        {
          for (int j = 0; j < 50; ++j)
          {
            _BUDDY_LIST::__list *buddy = &target->m_pmBuddy.m_List[j];
            if (buddy->fill())
            {
              if (m_Param.GetCharSerial() == buddy->dwSerial)
              {
                char *newName = m_Param.GetCharNameW();
                if (std::strcmp(buddy->wszName, newName))
                {
                  std::strcpy(buddy->wszName, newName);
                  target->SendMsg_BuddyNameReNewal(buddy->dwSerial, buddy->wszName);
                  break;
                }
              }
            }
          }
        }
      }

      ++m_NameChangeBuddyInfo.nSendNum;
    }

    if (m_NameChangeBuddyInfo.nSendNum >= MAX_PLAYER)
    {
      m_NameChangeBuddyInfo.Init();
    }
  }
}

void CPlayer::Loop()
{
  if (!m_bOper)
  {
    return;
  }

  if (CCharacter::GetStealth(true) && IsRecallAnimus())
  {
    if (g_Main.IsReleaseServiceMode())
    {
      if (!m_byUserDgr)
      {
        _AnimusReturn(2u);
      }
    }
    else
    {
      _AnimusReturn(2u);
    }
  }

  const unsigned int currentTime = timeGetTime();
  LendItemSheet *lendSheet = LendItemMng::Instance()->GetSheet(m_ObjID.m_wIndex);
  int lendCheckResult = 0;
  if (lendSheet)
  {
    lendCheckResult = static_cast<int>(lendSheet->CheckTime());
  }
  if (lendCheckResult)
  {
    g_Main.m_logSystemError.Write(
      "Lend item error : %s >> CheckTime(%d)",
      m_Param.GetCharNameW(),
      lendCheckResult);
  }

  CCharacter::UpdateSFCont();
  g_PotionMgr.UpdatePotionContEffect(this);

  if (m_bUpCheckEquipEffect)
  {
    for (int effectIndex = 0; effectIndex < 15; ++effectIndex)
    {
      if (m_byEffectEquipCode[effectIndex] == 2)
      {
        _STORAGE_LIST::_db_con *item = nullptr;
        if (effectIndex >= 8)
        {
          item = &m_Param.m_dbEmbellish.m_pStorageList[effectIndex - 8];
        }
        else
        {
          item = &m_Param.m_dbEquip.m_pStorageList[effectIndex];
        }

        if (item->m_bLoad && IsEffectableEquip(item))
        {
          SetEquipEffect(item, true);
          m_byEffectEquipCode[effectIndex] = 1;
          if (item->m_byTableCode == 6 && !IsRidingUnit())
          {
            m_pmWpn.FixWeapon(item);
          }
        }
      }
    }
    m_bUpCheckEquipEffect = false;
  }

  if (m_bDownCheckEquipEffect)
  {
    for (int effectIndex = 0; effectIndex < 15; ++effectIndex)
    {
      if (m_byEffectEquipCode[effectIndex] == 1)
      {
        _STORAGE_LIST::_db_con *item = nullptr;
        if (effectIndex >= 8)
        {
          item = &m_Param.m_dbEmbellish.m_pStorageList[effectIndex - 8];
        }
        else
        {
          item = &m_Param.m_dbEquip.m_pStorageList[effectIndex];
        }

        if (item->m_bLoad && !IsEffectableEquip(item))
        {
          SetEquipEffect(item, false);
          m_byEffectEquipCode[effectIndex] = 2;
          if (item->m_byTableCode == 6 && !IsRidingUnit())
          {
            m_pmWpn.FixWeapon(nullptr);
          }
        }
      }
    }
    m_bDownCheckEquipEffect = false;
  }

  if (m_byNextRecallReturn != 255)
  {
    _AnimusReturn(m_byNextRecallReturn);
    m_byNextRecallReturn = 255;
  }

  if (m_dwSelfDestructionTime && m_dwSelfDestructionTime < currentTime)
  {
    pc_PlayAttack_SelfDestruction();
    m_dwSelfDestructionTime = 0;
    m_fSelfDestructionDamage = 0.0f;
  }

  if (m_tmrIntervalSec.CountingTimer())
  {
    _CheckForcePullUnit();
    if (m_pCurMap && m_pUserDB && !m_pCurMap->m_pMapSet->m_nMapType)
    {
      m_pUserDB->Update_Map(static_cast<unsigned __int8>(m_pCurMap->m_pMapSet->m_dwIndex), m_fCurPos);
    }
    CheckPosInTown();
    UpdatedMasteryWriteHistory();
    m_QuestMgr.Loop();

    unsigned __int16 currentTol[4]{};
    currentTol[0] = static_cast<unsigned __int16>(GetFireTol());
    currentTol[1] = static_cast<unsigned __int16>(GetWaterTol());
    currentTol[2] = static_cast<unsigned __int16>(GetSoilTol());
    currentTol[3] = static_cast<unsigned __int16>(GetWindTol());
    if (std::memcmp(m_zLastTol, currentTol, sizeof(currentTol)))
    {
      std::memcpy(m_zLastTol, currentTol, sizeof(currentTol));
      SendMsg_AlterTol();
    }

    m_byDefMatCount = 0;
    _check_hp_send_party();
  }

  UpdateChaosModeState(currentTime);
  SenseState();

  if (m_dwNextTimeDungeonDie && m_dwNextTimeDungeonDie < currentTime)
  {
    if (m_bCorpse && m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      pc_Revival(true);
    }
    m_dwNextTimeDungeonDie = 0;
  }

  if (m_bMineMode && GetLoopTime() > m_dwMineNextTime)
  {
    pc_MineComplete();
  }

  _check_target_object();

  if (m_bMove
      && GetLoopTime() > m_dwLastSetPointTime
      && GetLoopTime() - m_dwLastSetPointTime > 3000)
  {
    const bool isOutExtraStopPos = IsOutExtraStopPos(m_fCurPos);
    SendMsg_Stop(isOutExtraStopPos);
    CCharacter::Stop();
  }

  if (m_tmrBilling.CountingTimer() && !m_pUserDB->m_byUserDgr)
  {
    const bool hasBillingType = m_pUserDB->m_BillingInfo.iType != 0;
    if (hasBillingType)
    {
      CBillingManager::Instance()->Alive(m_pUserDB);
    }
    if (!hasBillingType)
    {
      m_pUserDB->SetBillingNoLogout(true);
      const unsigned __int16 closeDelay = CNationSettingManager::Instance()->GetBillingForceCloseDelay();
      SendMsg_BillingExipreInform(0, closeDelay);
      ReservationForceClose();
    }
  }

  if (m_Param.GetCurItemSerial() > 60000)
  {
    s_MgrItemHistory.item_serial_full(m_ObjID.m_wIndex, m_szItemHistoryFileName);
    m_pUserDB->ForceCloseCommand(7u, -1, true, "Item Serial Count Over");
  }

  if (m_tmrGroupTargeting.CountingTimer())
  {
    _check_party_target_object();
    _check_guild_target_object();
    _check_race_target_object();
  }

  if (m_byPatriarchAppointPropose != 255
      && currentTime - m_dwPatriarchAppointTime >= 300000)
  {
    char appointData[20]{};
    appointData[0] = 1;
    PatriarchElectProcessor::Instance()->Doit(_eRespAppoint, this, appointData);
  }

  if (currentTime - m_dwLastCheckRegionTime > 5000)
  {
    if (CGameObject::GetCurSecNum() != static_cast<unsigned int>(-1))
    {
      CheckPos_Region();
    }
    if (m_nCheckMovePacket > 17)
    {
      m_bCheckMovePacket = true;
      g_Main.m_logMove.Write(
        "id: %s >> %s ( %d ), Move Count : %d, map(%s)",
        m_pUserDB->m_szAccountID,
        m_Param.GetCharNameA(),
        m_dwObjSerial,
        m_nCheckMovePacket,
        m_pCurMap->m_pMapSet->m_strCode);
    }
    m_nCheckMovePacket = 0;
  }

  CheckAlterMaxPoint();
  m_kPvpOrderView.Loop(m_ObjID.m_wIndex);
  CheckBattleMode();

  if (IsApplyPcbangPrimium())
  {
    m_kPcBangCoupon.Loop(m_ObjID.m_wIndex);
  }

  TimeLimitJade *timeLimitJade = TimeLimitJadeMng::Instance()->GetSheet(m_ObjID.m_wIndex);
  if (timeLimitJade && m_tmrEffectEndTime.CountingTimer())
  {
    timeLimitJade->CheckStartTime();
    timeLimitJade->CheckEndTime();
  }

  if (m_tmrSiegeTime.CountingTimer())
  {
    m_bIsSiegeActing = false;
    m_tmrSiegeTime.StopTimer();
  }

  if (m_PotionBufUse.IsExtPotionUse())
  {
    m_PotionBufUse.CheckPotionTime(this);
  }

  if (m_Param.m_bTrunkOpen && !IsBeNearStore(this, 10))
  {
    m_Param.m_bTrunkOpen = false;
  }

  CheckNameChange();

  if (m_tmrAccumPlayingTime.CountingTimer())
  {
    _SYSTEMTIME localTime{};
    GetLocalTime(&localTime);
    const unsigned int accPlayTime = SumMinuteBetween(&m_tmCalcTime, &localTime);
    memcpy_s(&m_tmCalcTime, sizeof(m_tmCalcTime), &localTime, sizeof(localTime));
    m_pUserDB->Update_UserPlayTime(accPlayTime);
  }

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    if (m_pDHChannel)
    {
      unsigned __int8 raceSerial = 255;
      const int raceCode = m_Param.GetRaceCode();
      if (raceCode == 0)
      {
        raceSerial = 0;
      }
      else if (raceCode == 1)
      {
        raceSerial = 1;
      }
      else if (raceCode == 2)
      {
        raceSerial = 3;
      }

      CMapItemStoreList *storeList = CItemStoreManager::Instance()->GetMapItemStoreListBySerial(raceSerial);
      if (storeList)
      {
        for (int index = 0; index < storeList->m_nItemStoreNum; ++index)
        {
          static_cast<void>(&storeList->m_ItemStore[index]);
        }

        if (m_bCorpse)
        {
          pc_Revival(true);
        }
        else
        {
          CMapData *intoMap = nullptr;
          float outPos[3]{};
          if (m_bLive)
          {
            intoMap = g_MapOper.GetPosStartMap(static_cast<unsigned __int8>(m_Param.GetRaceCode()), false, outPos);
            if (intoMap)
            {
              pc_Resurrect(false);
              ForcePullUnit(false);
              OutOfMap(intoMap, 0, 3u, outPos);
              SendMsg_GotoBasePortalResult(0);
            }
          }
        }
      }
      SendMsg_TLStatusPenalty(7u);
    }

    if (m_pPartyMgr->IsPartyMode())
    {
      SendMsg_PartyLeaveSelfResult(nullptr, false);
      wa_PartySelfLeave(&m_id);
      SendMsg_TLStatusPenalty(2u);
    }

    if (m_bInGuildBattle)
    {
      const unsigned int guildSerial = m_Param.GetGuildSerial();
      GUILD_BATTLE::CNormalGuildBattle *battle =
        GUILD_BATTLE::CNormalGuildBattleManager::Instance()->GetBattleByGuildSerial(guildSerial);

      if (battle && battle->IsReadyOrCountState())
      {
        GUILD_BATTLE::CNormalGuildBattleManager::Instance()->LeaveGuild(this);
        SendMsg_TLStatusPenalty(6u);
      }
      if (battle && battle->IsInBattle())
      {
        GUILD_BATTLE::CNormalGuildBattleManager::Instance()->LeaveGuild(this);
        SendMsg_TLStatusPenalty(6u);
      }
    }

    if (m_bMineMode)
    {
      m_bMineMode = false;
      m_dwMineNextTime = static_cast<unsigned int>(-1);
      SendMsg_MineCancle();
      SendMsg_TLStatusPenalty(5u);
    }

    const int playerIndex = m_id.wIndex;
    if (AutominePersonalMgr::instance()->Is_MineRun(playerIndex))
    {
      AutominePersonalMgr::instance()->uninstall(m_id.wIndex);
      AutominePersonalMgr::instance()->extract_battery(m_id.wIndex);
      SendMsg_TLStatusPenalty(5u);
    }
  }

  for (int actionIndex = 0; actionIndex < 3; ++actionIndex)
  {
    CActionPointSystemMgr *actionPointMgr = CActionPointSystemMgr::Instance();
    if (actionPointMgr->IsSystemEnable(static_cast<unsigned __int8>(actionIndex)))
    {
      actionPointMgr = CActionPointSystemMgr::Instance();
      if (actionPointMgr->IsPointReset(static_cast<unsigned __int8>(actionIndex)))
      {
        if (m_pUserDB->GetActPoint(static_cast<unsigned __int8>(actionIndex)))
        {
          actionPointMgr = CActionPointSystemMgr::Instance();
          if (actionPointMgr->GetEventStatus(static_cast<unsigned __int8>(actionIndex)) == 3
              || (actionPointMgr = CActionPointSystemMgr::Instance(),
                  !actionPointMgr->GetEventStatus(static_cast<unsigned __int8>(actionIndex))))
          {
            m_pUserDB->Update_User_Action_Point(static_cast<unsigned __int8>(actionIndex), 0);
            SendMsg_Alter_Action_Point(static_cast<unsigned __int8>(actionIndex), 0);
          }
        }
      }
    }
  }

  if (m_tmrPremiumPVPInform.CountingTimer() && IsApplyPcbangPrimium())
  {
    const bool isPremium = IsApplyPcbangPrimium();
    const unsigned __int8 level = static_cast<unsigned __int8>(m_Param.GetLevel());
    const int maxTempPoint = m_kPvpCashPoint.GetMaxTempPoint(level, isPremium);
    SendMsg_MaxPvpPointInform(maxTempPoint);
  }
}

void CPlayer::SendMsg_SetHPInform()
{
  _set_hp_inform_zocl hp{};
  hp.wHP = static_cast<unsigned __int16>(m_Param.GetHP());
  unsigned __int8 type[2] = {17, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&hp), sizeof(hp));
}

__int64 CPlayer::SetDamage(
  int nDam,
  CCharacter *pDst,
  int nDstLv,
  bool bCrt,
  int nAttackType,
  unsigned int dwAttackSerial,
  bool bJadeReturn)
{
  (void)bCrt;
  (void)nAttackType;
  (void)dwAttackSerial;

  if (m_bCorpse || m_bMapLoading || CGameObject::GetCurSecNum() == static_cast<unsigned int>(-1))
  {
    return static_cast<unsigned int>(m_Param.GetHP());
  }

  if (pDst && m_EP.GetEff_Have(EFF_HAVE_JADE_RETURN_DAMAGE_RATE) > 0.0f && bJadeReturn && pDst->m_ObjID.m_byID == 0)
  {
    const int returnDamage = static_cast<int>(static_cast<float>(nDam) * m_EP.GetEff_Have(EFF_HAVE_JADE_RETURN_DAMAGE_RATE));
    pDst->SetDamage(returnDamage, this, static_cast<int>(GetLevel()), true, -1, 0, false);
  }

  if (pDst && m_EP.GetEff_Have(EFF_HAVE_JADE_RETURN_FULL_DAMAGE) > 0.0f && bJadeReturn && pDst->m_ObjID.m_byID == 0)
  {
    pDst->SetDamage(nDam, this, static_cast<int>(GetLevel()), true, -1, 0, false);
    return static_cast<unsigned int>(m_Param.GetHP());
  }

  CCharacter::BreakStealth();
  BreakCloakBooster();

  if (m_EP.GetEff_State(EFF_STATE_UNKNOWN_14))
  {
    CCharacter::RemoveSFContHelpByEffect(2, 14);
  }

  if (pDst && !IsRidingUnit() && nDam > 0)
  {
    const int attackerLevel = static_cast<int>(pDst->GetAttackLevel());
    const int damageLevel = static_cast<int>(GetDamageLevel(m_nLastBeatenPart));
    int defenseRate = static_cast<int>((static_cast<float>(attackerLevel - damageLevel + 23) / 33.0f) + 1.0f);
    if (defenseRate <= 0)
    {
      defenseRate = 0;
    }

    const int attackerDP = static_cast<int>(pDst->GetAttackDP());
    const int damageDP = static_cast<int>(GetDamageDP(m_nLastBeatenPart));
    const float baseDefPoint = pDst->m_ObjID.m_byID ? static_cast<float>(s_nMonDefPoint) : static_cast<float>(s_nStdDefPoint);
    float dpDamScale = baseDefPoint + (static_cast<float>(damageDP * attackerDP) / 2.0f);
    if (!pDst->m_ObjID.m_byID)
    {
      dpDamScale *= 1.25f;

      CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
      if (!dstPlayer->IsChaosMode())
      {
        const unsigned int now = timeGetTime();
        m_kPvpOrderView.m_dwLastDamagedTime = now;
        m_kPvpOrderView.m_bDamaged = true;
        m_kPvpOrderView.Notify_OrderView(m_ObjID.m_wIndex);
        dstPlayer->m_kPvpOrderView.m_dwLastAttackTime = now;
        dstPlayer->m_kPvpOrderView.m_bAttack = true;
        dstPlayer->m_kPvpOrderView.Notify_OrderView(dstPlayer->m_ObjID.m_wIndex);
      }
    }

    const int dpDamage = static_cast<int>(dpDamScale * static_cast<float>(defenseRate));
    if (dpDamage > 0)
    {
      const int oldDP = GetDP();
      SetDP(oldDP - dpDamage, false);
      if (oldDP != GetDP())
      {
        SendMsg_SetDPInform();
      }
    }
  }

  if (pDst && !IsRidingUnit() && nDam <= 0)
  {
    const int attackerLevel = static_cast<int>(pDst->GetAttackLevel());
    const int damageLevel = static_cast<int>(GetDamageLevel(m_nLastBeatenPart));
    int defenseRate = static_cast<int>((static_cast<float>(attackerLevel - damageLevel + 23) / 33.0f) + 1.0f);
    if (defenseRate <= 0)
    {
      defenseRate = 0;
    }

    const int attackerDP = static_cast<int>(pDst->GetAttackDP());
    const int damageDP = static_cast<int>(GetDamageDP(m_nLastBeatenPart));
    int dpDamage = 0;
    if (pDst->m_ObjID.m_byID)
    {
      const float dpScale =
        static_cast<float>(s_nMonDefPoint) + (static_cast<float>(damageDP * attackerDP) / 2.0f);
      dpDamage = static_cast<int>((dpScale * static_cast<float>(defenseRate)) / 3.0f);
    }
    else
    {
      const float dpScale =
        static_cast<float>(s_nStdDefPoint) + (static_cast<float>(damageDP * attackerDP) / 2.0f);
      dpDamage = static_cast<int>((dpScale * static_cast<float>(defenseRate) * 1.25f) / 3.0f);
    }

    if (dpDamage > 0)
    {
      const int oldDP = GetDP();
      SetDP(oldDP - dpDamage, false);
      if (oldDP != GetDP())
      {
        SendMsg_SetDPInform();
      }
    }
  }

  if (nDam <= 0)
  {
    if (nDam == -1)
    {
      const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
      pc_PlayAttack_Gen(pDst, static_cast<unsigned __int8>(attackPart), 65535, 65535, true);
    }
    else if (nDam == -2 && m_nLastBeatenPart == 5 && pDst && !IsInTown() && IsPassMasteryLimitLvDiff(nDstLv))
    {
      if (m_byDefMatCount++ < 2)
      {
        const unsigned int masteryCum = GetMasteryCumAfterAttack(nDstLv);
        if (pDst->m_ObjID.m_byID || GetObjRace() != pDst->GetObjRace())
        {
          Emb_AlterStat(2u, 0u, static_cast<int>(masteryCum), 0, "CPlayer::SetDamage()---3", true);
        }
        else
        {
          CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
          if (!dstPlayer->IsChaosMode() && !IsPunished(1u, false))
          {
            Emb_AlterStat(2u, 0u, static_cast<int>(masteryCum), 0, "CPlayer::SetDamage()---2", true);
          }
        }
      }
    }

    m_nLastBeatenPart = 0;
    SetBattleMode(false);
    return static_cast<unsigned int>(m_Param.GetHP());
  }

  if (IsRidingUnit())
  {
    if (!m_bNeverDie)
    {
      if (m_pUsingUnit->dwGauge <= static_cast<unsigned int>(nDam))
      {
        m_pUsingUnit->dwGauge = 0;
      }
      else
      {
        m_pUsingUnit->dwGauge -= static_cast<unsigned int>(nDam);
      }
    }

    if (m_pUsingUnit->dwGauge > 0)
    {
      SendMsg_AlterUnitHPInform(static_cast<char>(m_pUsingUnit->bySlotIndex), m_pUsingUnit->dwGauge);
    }
    else
    {
      SetHP(0, false);
      SendMsg_SetHPInform();
      Emb_RidindUnit(false, nullptr);
    }
  }
  else
  {
    if (m_Param.GetFP() > 0 && m_EP.GetEff_State(EFF_STATE_UNKNOWN_22))
    {
      int fpDamage = 2 * nDam;
      const int currentFp = m_Param.GetFP();
      if (fpDamage > currentFp)
      {
        const int overflow = fpDamage - currentFp;
        SetHP(m_Param.GetHP() - (overflow / 2), false);
        fpDamage = m_Param.GetFP();
      }

      SetFP(m_Param.GetFP() - fpDamage, false);
      SendMsg_SetFPInform();
    }
    else
    {
      SetHP(m_Param.GetHP() - nDam, false);
      SendMsg_SetHPInform();
    }

    if (pDst && !IsInTown() && IsPassMasteryLimitLvDiff(nDstLv) && GetHP() > 0)
    {
      const float hpRateDamage = static_cast<float>(nDam) / static_cast<float>(GetMaxHP());
      if (hpRateDamage >= 0.02f && m_byDefMatCount++ < 2)
      {
        const unsigned int masteryCum = GetMasteryCumAfterAttack(nDstLv);
        const int addValue = m_nAddDfnMstByClass * static_cast<int>(masteryCum);
        if (m_nLastBeatenPart != 5)
        {
          if (pDst->m_ObjID.m_byID || GetObjRace() != pDst->GetObjRace())
          {
            Emb_AlterStat(1u, 0u, addValue, 0, "CPlayer::SetDamage()---1", true);
          }
          else
          {
            CPlayer *dstPlayer = static_cast<CPlayer *>(pDst);
            if (!dstPlayer->IsChaosMode() && !IsPunished(1u, false))
            {
              Emb_AlterStat(1u, 0u, addValue, 0, "CPlayer::SetDamage()---0", true);
            }
          }

          if (m_pPartyMgr && m_pPartyMgr->IsPartyMode())
          {
            if (_STAT_DB_BASE::IsRangePerMastery(1u, 0u))
            {
              CPlayer *members[8]{};
              const unsigned __int8 memberCount = _GetPartyMemberInCircle(members, 8, true);
              for (int idx = 0; idx < memberCount; ++idx)
              {
                CPlayer *member = members[idx];
                if (!member || member == this || member->m_bCorpse)
                {
                  continue;
                }

                const int levelScaled = (static_cast<int>(GetLevel()) / 10 + 1) * addValue;
                float share = static_cast<float>(levelScaled) * 0.1f;
                if (share <= 0.0f)
                {
                  continue;
                }

                const unsigned int limitCum = member->_check_mastery_cum_lim(1u, 0u);
                const unsigned int curCum = member->m_pmMst.GetCumPerMast(1u, 0u);
                const int remainCum = static_cast<int>(limitCum) - static_cast<int>(curCum);
                if (remainCum <= 0)
                {
                  continue;
                }

                if (share > static_cast<float>(remainCum))
                {
                  share = static_cast<float>(remainCum);
                }

                share += 0.5f;
                if (share < 1.0f)
                {
                  continue;
                }

                const unsigned int alterCum = static_cast<unsigned int>(share);
                const int statIndex = _STAT_DB_BASE::GetStatIndex(1u, 0u);
                if (member->m_pmMst.m_BaseCum.m_dwDamWpCnt[statIndex] > 4000000000)
                {
                  continue;
                }

                unsigned int afterCum = 0;
                const bool updated =
                  member->m_pmMst.AlterCumPerMast(1u, 0u, alterCum, &afterCum);
                if (member->m_pmMst.m_bUpdateEquipMast)
                {
                  member->m_bUpCheckEquipEffect = true;
                }
                member->SendMsg_StatInform(static_cast<unsigned __int8>(statIndex), afterCum, 0);
                if (member->m_pmMst.m_MastUpData.bUpdate)
                {
                  member->ReCalcMaxHFSP(true, false);
                }
                if (member->m_pUserDB)
                {
                  member->m_pUserDB->Update_Stat(static_cast<unsigned __int8>(statIndex), afterCum, updated);
                }
                member->m_Param.m_dwAlterMastery[statIndex] += alterCum;
              }
            }
            else
            {
              const char *charName = m_Param.GetCharNameA();
              g_Main.m_logSystemError.Write("%s: PartyPlayer Div Defence Error", charName);
            }
          }
        }
      }
    }
  }

  if (GetHP() == 0)
  {
    if (pDst)
    {
      pDst->RecvKillMessage(this);
    }
    Corpse(pDst);
  }

  if (pDst && m_pRecalledAnimusChar && !pDst->GetStealth(true))
  {
    m_pRecalledAnimusChar->MasterBeAttacked_MasterInform(pDst);
  }

  m_nLastBeatenPart = 0;
  SetBattleMode(false);
  return static_cast<unsigned int>(m_Param.GetHP());
}

char CPlayer::IsOverOneDay()
{
  const unsigned int lastConnTime = this->m_pUserDB->m_AvatorData.dbAvator.m_dwLastConnTime;

  char dateBuffer[48]{};
  char timeBuffer[48]{};
  _strdate(dateBuffer);
  _strtime(timeBuffer);

  char yearString[48]{};
  char monthString[48]{};
  char dayString[48]{};
  char hourMinString[28]{};

  yearString[0] = dateBuffer[6];
  yearString[1] = dateBuffer[7];
  yearString[2] = 0;
  monthString[0] = dateBuffer[0];
  monthString[1] = dateBuffer[1];
  monthString[2] = 0;
  dayString[0] = dateBuffer[3];
  dayString[1] = dateBuffer[4];
  dayString[2] = 0;
  hourMinString[0] = timeBuffer[0];
  hourMinString[1] = timeBuffer[1];
  hourMinString[2] = timeBuffer[3];
  hourMinString[3] = timeBuffer[4];
  hourMinString[4] = 0;

  const int curYear = atoi(yearString);
  const int curMonth = atoi(monthString);
  const unsigned int curDay = static_cast<unsigned int>(atoi(dayString));
  (void)atoi(hourMinString);

  const unsigned int lastYearMonth = lastConnTime / 1000000;
  const unsigned int lastYear = lastYearMonth / 100;
  const unsigned int lastMonth = lastYearMonth % 100;
  const unsigned int lastDay = lastConnTime % 1000000 / 10000;

  if (curYear == static_cast<int>(lastYear))
  {
    if (curMonth - static_cast<int>(lastMonth) > 1)
    {
      return 1;
    }
    if (curMonth - static_cast<int>(lastMonth) == 1 && curDay >= lastDay)
    {
      return 1;
    }
  }
  else
  {
    if (curYear - static_cast<int>(lastYear) > 1)
    {
      return 1;
    }
    if (curMonth != 1 || lastMonth != 12)
    {
      return 1;
    }
    if (curDay >= lastDay)
    {
      return 1;
    }
  }

  return 0;
}

unsigned int CPlayer::_check_mastery_cum_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex)
{
  _MasteryLimit_fld *curRecord = nullptr;
  _MasteryLimit_fld *baseRecord = nullptr;

  _class_fld *curClass = this->m_Param.GetPtrCurClass();
  _class_fld *baseClass = this->m_Param.GetPtrBaseClass();
  if (curClass->m_nClass >= 4 || baseClass->m_nClass >= 4)
  {
    g_Main.m_logSystemError.Write(
      "_check_mastery_cum_lim.. cur_class : %d, base_class : %d",
      curClass->m_nClass,
      baseClass->m_nClass);
    return 0;
  }

  const int raceCode = this->m_Param.GetRaceCode();
  if (raceCode >= 3)
  {
    g_Main.m_logSystemError.Write("_check_mastery_cum_lim.. racecode : %d", raceCode);
    return 0;
  }

  const int maxLevel = this->m_Param.GetMaxLevel();
  const int level = static_cast<int>(GetLevel());
  if (level > maxLevel || level <= 0)
  {
    g_Main.m_logSystemError.Write("_check_mastery_cum_lim.. level : %d, max level : %d", level, maxLevel);
    return 0;
  }

  const int recordIndex = level - 1;
  if (this->m_Param.m_pClassHistory[0])
  {
    if (curClass->m_nClass == baseClass->m_nClass)
    {
      curRecord = reinterpret_cast<_MasteryLimit_fld *>(
        CPlayer::s_tblLimMasteryCumContinue[raceCode][curClass->m_nClass].GetRecord(recordIndex));
    }
    else
    {
      curRecord = reinterpret_cast<_MasteryLimit_fld *>(
        CPlayer::s_tblLimMasteryCum[raceCode][curClass->m_nClass].GetRecord(recordIndex));
      baseRecord = reinterpret_cast<_MasteryLimit_fld *>(
        CPlayer::s_tblLimMasteryCum[raceCode][baseClass->m_nClass].GetRecord(recordIndex));
    }
  }
  else
  {
    curRecord =
      reinterpret_cast<_MasteryLimit_fld *>(CPlayer::s_tblLimMasteryCum[raceCode][curClass->m_nClass].GetRecord(
        recordIndex));
  }

  if (!curRecord)
  {
    g_Main.m_logSystemError.Write("_check_mastery_cum_lim.. pCurFld : NULL");
    return 0;
  }

  unsigned int curLimit = 0;
  unsigned int baseLimit = 0;
  switch (byMasteryClass)
  {
    case 0:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsMMastery[byIndex]);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsMMastery[byIndex]);
      }
      break;
    case 1:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsDefMastery);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsDefMastery);
      }
      break;
    case 2:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsPryMastery);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsPryMastery);
      }
      break;
    case 3:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsSkillMastery[byIndex]);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsSkillMastery[byIndex]);
      }
      break;
    case 4:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsForceMastery[byIndex]);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsForceMastery[byIndex]);
      }
      break;
    case 5:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsMakeMastery[byIndex]);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsMakeMastery[byIndex]);
      }
      break;
    case 6:
      curLimit = static_cast<unsigned int>(curRecord->m_MasteryLim.m_nBnsSMastery);
      if (baseRecord)
      {
        baseLimit = static_cast<unsigned int>(baseRecord->m_MasteryLim.m_nBnsSMastery);
      }
      break;
    default:
      break;
  }

  return (curLimit <= baseLimit) ? baseLimit : curLimit;
}

void CPlayer::Emb_AlterStat_F(
  unsigned __int8 byMasteryClass,
  unsigned __int8 byIndex,
  float fAlter,
  unsigned __int8 byReason)
{
  if (!_STAT_DB_BASE::IsRangePerMastery(byMasteryClass, byIndex))
  {
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logSystemError.Write(
      "%s: _STAT_DB_BASE::IsRangePerMastery(%d, %d) == false",
      charName,
      byMasteryClass,
      byIndex);
    return;
  }

  if (!byReason)
  {
    fAlter *= static_cast<float>(static_cast<int>(GetLevel()) / 10 + 1);
  }

  unsigned __int8 checkIndex = byIndex;
  if (byMasteryClass == 3)
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(_MASTERY_PARAM::s_pSkillData->GetRecord(byIndex));
    if (record)
    {
      if (record->m_nMastIndex >= 8)
      {
        return;
      }
      checkIndex = static_cast<unsigned __int8>(record->m_nMastIndex);
    }
  }

  const unsigned int limitCum = _check_mastery_cum_lim(byMasteryClass, checkIndex);
  const int currentCum = static_cast<int>(m_pmMst.GetCumPerMast(byMasteryClass, checkIndex));
  int remainCum = static_cast<int>(limitCum) - currentCum;
  if (remainCum < 0)
  {
    remainCum = 0;
  }
  if (fAlter > static_cast<float>(remainCum))
  {
    fAlter = static_cast<float>(remainCum);
  }
  if (fAlter <= 0.0f)
  {
    return;
  }

  const int addCum = static_cast<int>(fAlter + 0.5f);
  const int statIndex = _STAT_DB_BASE::GetStatIndex(byMasteryClass, byIndex);
  if (m_pmMst.m_BaseCum.m_dwDamWpCnt[statIndex] > 4000000000)
  {
    return;
  }

  unsigned int newStat[4]{};
  const bool updateEquip = m_pmMst.AlterCumPerMast(byMasteryClass, byIndex, addCum, newStat);
  if (m_pmMst.m_bUpdateEquipMast)
  {
    m_bUpCheckEquipEffect = 1;
  }
  SendMsg_StatInform(static_cast<unsigned __int8>(statIndex), newStat[0], byReason);
  if (m_pmMst.m_MastUpData.bUpdate)
  {
    ReCalcMaxHFSP(1, 0);
  }
  if (m_pUserDB)
  {
    m_pUserDB->Update_Stat(static_cast<unsigned __int8>(statIndex), newStat[0], updateEquip);
  }
  m_Param.m_dwAlterMastery[statIndex] += addCum;
}

void CPlayer::Emb_AlterStat(
  unsigned __int8 byMasteryClass,
  unsigned __int8 byIndex,
  int dwAlter,
  unsigned __int8 byReason,
  const char *strErrorCodePos,
  bool bNoUseExpMasteryBonus)
{
if (!dwAlter || (!byReason && this->m_bInGuildBattle))
  {
    return;
  }

  if (!_STAT_DB_BASE::IsRangePerMastery(byMasteryClass, byIndex))
  {
    const char *charName = this->m_Param.GetCharNameA();
    if (strErrorCodePos)
    {
      g_Main.m_logSystemError.Write(
        "%s: _STAT_DB_BASE::IsRangePerMastery(%d, %d) == false : CodePos: %s",
        charName,
        byMasteryClass,
        byIndex,
        strErrorCodePos);
    }
    else
    {
      g_Main.m_logSystemError.Write(
        "%s: _STAT_DB_BASE::IsRangePerMastery(%d, %d) == false",
        charName,
        byMasteryClass,
        byIndex);
    }
    return;
  }

  int alterCum = dwAlter;
  if (!byReason)
  {
    alterCum *= 5 * (static_cast<int>(GetLevel()) / 10 + 1);
  }

  const int alterBase = alterCum;
  if (byReason != 1)
  {
    if (byMasteryClass != 6 || this->m_Param.GetRaceCode() != 1)
    {
      if (IsApplyPcbangPrimium())
      {
        alterCum = static_cast<int>(
          static_cast<float>(alterCum) + static_cast<float>(alterBase) * (PCBANG_PRIMIUM_FAVOR::BASE_MASTERY - 1.0f));
      }
      else
      {
        alterCum = static_cast<int>(
          static_cast<float>(alterCum) + static_cast<float>(alterBase) * (MASTERY_GET_RATE - 1.0f));
      }
    }
    if (byMasteryClass != 6 || this->m_Param.GetRaceCode() != 1)
    {
      float effHave = this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_6);
      const float eff82 = this->m_EP.GetEff_Have(EFF_HAVE_UNKNOWN_82);
      effHave = (effHave == 0.0f) ? eff82 : effHave + eff82;
      if (effHave > 1.0f)
      {
        alterCum =
          static_cast<int>(static_cast<float>(alterCum) + static_cast<float>(alterBase) * (effHave - 1.0f));
      }
    }
    if (byMasteryClass != 6 || this->m_Param.GetRaceCode() != 1)
    {
      const float rate = this->m_EP.GetEff_Rate(EFF_RATE_MASTERY_GAIN);
      alterCum = static_cast<int>(static_cast<float>(alterCum) + static_cast<float>(alterBase) * (rate - 1.0f));
    }
  }

  const double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(this->m_id.wIndex);
  unsigned int alterCapped = static_cast<unsigned int>(static_cast<double>(alterCum) * penalty);
  if (!alterCapped)
  {
    return;
  }

  unsigned __int8 checkIndex = byIndex;
  if (byMasteryClass == 3)
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(_MASTERY_PARAM::s_pSkillData->GetRecord(byIndex));
    if (record)
    {
      const unsigned __int8 skillIndex = static_cast<unsigned __int8>(record->m_nMastIndex);
      if (skillIndex >= 8u)
      {
        return;
      }
      checkIndex = skillIndex;
    }
  }

  const unsigned int limit = _check_mastery_cum_lim(byMasteryClass, checkIndex);
  const int current = static_cast<int>(this->m_pmMst.GetCumPerMast(byMasteryClass, checkIndex));
  unsigned int remain = limit - current;
  if (static_cast<int>(limit - current) < 0)
  {
    remain = 0;
  }
  if (alterCapped > remain)
  {
    alterCapped = remain;
  }
  if (!alterCapped)
  {
    return;
  }

  const int statIndex = _STAT_DB_BASE::GetStatIndex(byMasteryClass, byIndex);
  if (this->m_pmMst.m_BaseCum.m_dwDamWpCnt[statIndex] > 4000000000)
  {
    return;
  }

  unsigned int newStat[4]{};
  const bool updateEquip = this->m_pmMst.AlterCumPerMast(byMasteryClass, byIndex, alterCapped, newStat);

  if (this->m_pmMst.m_bUpdateEquipMast)
  {
    this->m_bUpCheckEquipEffect = 1;
  }

  SendMsg_StatInform(statIndex, newStat[0], byReason);
  if (this->m_pmMst.m_MastUpData.bUpdate)
  {
    ReCalcMaxHFSP(1, 0);
  }
  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_Stat(statIndex, newStat[0], updateEquip);
  }
  this->m_Param.m_dwAlterMastery[statIndex] += alterCapped;
}

