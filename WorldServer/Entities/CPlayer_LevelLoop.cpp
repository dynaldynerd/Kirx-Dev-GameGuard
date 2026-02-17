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
unsigned int s_ExpNotifyIndex = 0;
}

void CPlayer::SendMsg_MaxHFSP()
{
  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = static_cast<unsigned __int16>(this->GetMaxHP());
  *reinterpret_cast<__int16 *>(payload + 2) = static_cast<__int16>(this->GetMaxFP());
  *reinterpret_cast<__int16 *>(payload + 4) = static_cast<__int16>(this->GetMaxSP());

  unsigned __int8 type[2] = {11, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 6u);
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

  char payload[10]{};
  *reinterpret_cast<unsigned int *>(payload) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 4) = static_cast<unsigned __int16>(this->GetMaxHP());
  *reinterpret_cast<__int16 *>(payload + 6) = static_cast<__int16>(this->GetMaxFP());
  *reinterpret_cast<__int16 *>(payload + 8) = static_cast<__int16>(this->GetMaxSP());

  const int memberCount = static_cast<int>(this->m_pPartyMgr->GetPopPartyMember());
  unsigned __int8 type[2] = {16, 25};
  for (int index = 0; index < memberCount; ++index)
  {
    if (partyMembers[index] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[index]->m_wZoneIndex, type, payload, 0xAu);
    }
  }
}

void CPlayer::SendMsg_AlterHPInform()
{
  char payload[2]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = static_cast<unsigned __int16>(this->GetHP());

  unsigned __int8 type[2] = {11, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
}

void CPlayer::SendMsg_Level(char nLevel)
{
  char msg[5]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_dwObjSerial;
  msg[4] = nLevel;

  unsigned __int8 type[2] = {11, 1};
  CircleReport(type, msg, 5, true);
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

  char msg[5]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_dwObjSerial;
  msg[4] = static_cast<char>(this->m_Param.GetLevel());

  const int memberCount = this->m_pPartyMgr->GetPopPartyMember();
  unsigned __int8 type[2] = {16, 23};

  for (int i = 0; i < memberCount; ++i)
  {
    if (partyMembers[i] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[i]->m_wZoneIndex, type, msg, 5u);
    }
  }
}

void CPlayer::SendMsg_AlterExpInform()
{
  char msg[4]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_dwExpRate;

  unsigned __int8 type[2] = {11, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 4u);
}

void CPlayer::SendMsg_NotifyGetExpInfo(long double dOldExp, long double dAlterExp, long double dCurExp)
{
  if (!g_Main.IsReleaseServiceMode())
  {
    _trans_gm_msg_inform_zocl msg{};
    memset_0(&msg, 0, sizeof(msg));
    msg.wMsgSize = _snprintf(
      msg.wszChatData,
      0xFFu,
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
    extraRate += this->m_EP.GetEff_Have(2) - 1.0f;
    extraRate += this->m_EP.GetEff_Have(5) - 1.0f;
    extraRate += this->m_EP.GetEff_Have(9) - 1.0f;

    if (IsApplyPcbangPrimium())
    {
      const float baseRate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PCBANG_PRIMIUM_FAVOR::PLAYER_EXP;
      const float eff71 = this->m_EP.GetEff_Have(71);
      if (eff71 > baseRate)
      {
        extraRate += eff71 - baseRate;
      }
      extraRate += this->m_EP.GetEff_Have(72) - 1.0f;
    }
    else
    {
      extraRate += this->m_EP.GetEff_Have(71) - 1.0f;
      extraRate += this->m_EP.GetEff_Have(72) - 1.0f;
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
    const float rate34 = this->m_EP.GetEff_Rate(34);
    const double withRate34 = adjustedAlterExp + baseAlterExp * static_cast<double>(rate34 - 1.0f);
    const float rate38 = this->m_EP.GetEff_Rate(38);
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
  unsigned __int16 fp = static_cast<unsigned __int16>(this->m_Param.GetFP());
  unsigned __int8 type[2] = {17, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&fp), 2u);
}

void CPlayer::SendMsg_SetSPInform()
{
  unsigned __int16 sp = static_cast<unsigned __int16>(this->m_Param.GetSP());
  unsigned __int8 type[2] = {17, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&sp), 2u);
}

void CPlayer::SendMsg_AlterSPInform()
{
  unsigned __int16 sp = static_cast<unsigned __int16>(GetSP());
  unsigned __int8 type[2] = {11, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&sp), 2u);
}

void CPlayer::SendMsg_AlterTol()
{
  unsigned __int8 type[2] = {11, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(this->m_zLastTol), 8u);
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
  AlterPvPCashBag(pvpTempCash, pm_kill);
  m_kPvpOrderView.Update_RaceWarRecvr(false);

  const long double pvpPoint = m_Param.GetPvPPoint();
  m_kPvpOrderView.Update(tCurTime, false, false, 0.0, pvpPoint, 0.0);
  m_kPvpOrderView.ResetPvPOrderView();
  m_kPvpOrderView.Notify_PvpTempCash(m_ObjID.m_wIndex);
  m_kPvpOrderView.Notify_OrderView(m_ObjID.m_wIndex);
  SendMsg_AlterPvPCash(0);
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

    memcpy_0(m_fLastRecvPos, m_fCurPos, sizeof(m_fLastRecvPos));
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

  if (!m_EP.GetEff_State(10))
  {
    if (!m_EP.GetEff_State(15))
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
          hpDelta = static_cast<int>(static_cast<float>(baseRecover) * m_EP.GetEff_Rate(14));
        }
      }
    }

    if (!m_EP.GetEff_State(16))
    {
      if (oldFp < GetMaxFP())
      {
        const int fpPlus = static_cast<int>(m_EP.GetEff_Plus(25));
        const float fpRate = m_EP.GetEff_Rate(15);
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

    if (!m_EP.GetEff_State(17))
    {
      const float spRate = m_EP.GetEff_Rate(16);
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

  if (m_EP.GetEff_Plus(32) != 0.0f)
  {
    hpDelta += static_cast<int>(m_EP.GetEff_Plus(32));
  }
  if (m_EP.GetEff_Plus(33) != 0.0f)
  {
    fpDelta += static_cast<int>(m_EP.GetEff_Plus(33));
  }
  if (m_EP.GetEff_Plus(34) != 0.0f)
  {
    spDelta += static_cast<int>(m_EP.GetEff_Plus(34));
  }

  if (m_EP.GetEff_Have(12) > 0.0f)
  {
    hpDelta += static_cast<int>(m_EP.GetEff_Have(12) * static_cast<float>(GetMaxHP()));
  }
  if (m_EP.GetEff_Have(13) > 0.0f)
  {
    fpDelta += static_cast<int>(m_EP.GetEff_Have(13) * static_cast<float>(GetMaxFP()));
  }
  if (m_EP.GetEff_Have(14) > 0.0f)
  {
    spDelta += static_cast<int>(m_EP.GetEff_Have(14) * static_cast<float>(GetMaxSP()));
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

void CPlayer::Loop()
{
  if (!m_bOper)
  {
    return;
  }

  const unsigned int currentTime = timeGetTime();

  if (CCharacter::GetStealth(true) && m_pRecalledAnimusChar != nullptr)
  {
    if (!g_Main.IsReleaseServiceMode() || m_byUserDgr)
    {
      _AnimusReturn(2u);
    }
  }

  LendItemSheet *lendSheet = LendItemMng::Instance()->GetSheet(m_ObjID.m_wIndex);
  if (lendSheet)
  {
    const int lendCheckResult = static_cast<int>(lendSheet->CheckTime());
    if (lendCheckResult)
    {
      g_Main.m_logSystemError.Write(
        "Lend item error : %s >> CheckTime(%d)",
        m_Param.GetCharNameW(),
        lendCheckResult);
    }
  }

  g_PotionMgr.UpdatePotionContEffect(this);

  if (m_bUpCheckEquipEffect)
  {
    for (int effectIndex = 0; effectIndex < 15; ++effectIndex)
    {
      if (m_byEffectEquipCode[effectIndex] != 2)
      {
        continue;
      }

      _STORAGE_LIST::_db_con *item = nullptr;
      if (effectIndex >= 8)
      {
        item = &m_Param.m_dbEmbellish.m_pStorageList[effectIndex - 8];
      }
      else
      {
        item = &m_Param.m_dbEquip.m_pStorageList[effectIndex];
      }

      if (!item->m_bLoad || !IsEffectableEquip(item))
      {
        continue;
      }

      SetEquipEffect(item, true);
      m_byEffectEquipCode[effectIndex] = 1;
      if (item->m_byTableCode == 6 && !IsRidingUnit())
      {
        m_pmWpn.FixWeapon(item);
      }
    }

    m_bUpCheckEquipEffect = false;
  }

  if (m_bDownCheckEquipEffect)
  {
    for (int effectIndex = 0; effectIndex < 15; ++effectIndex)
    {
      if (m_byEffectEquipCode[effectIndex] != 1)
      {
        continue;
      }

      _STORAGE_LIST::_db_con *item = nullptr;
      if (effectIndex >= 8)
      {
        item = &m_Param.m_dbEmbellish.m_pStorageList[effectIndex - 8];
      }
      else
      {
        item = &m_Param.m_dbEquip.m_pStorageList[effectIndex];
      }

      if (!item->m_bLoad || IsEffectableEquip(item))
      {
        continue;
      }

      SetEquipEffect(item, false);
      m_byEffectEquipCode[effectIndex] = 2;
      if (item->m_byTableCode == 6 && !IsRidingUnit())
      {
        m_pmWpn.FixWeapon(nullptr);
      }
    }

    m_bDownCheckEquipEffect = false;
  }

  if (m_byNextRecallReturn != 0xFF)
  {
    _AnimusReturn(m_byNextRecallReturn);
    m_byNextRecallReturn = 0xFF;
  }

  if (m_dwSelfDestructionTime && m_dwSelfDestructionTime < currentTime)
  {
    pc_PlayAttack_SelfDestruction();
    m_dwSelfDestructionTime = 0;
    m_fSelfDestructionDamage = 0.0f;
  }

  if (m_tmrIntervalSec.CountingTimer())
  {
    if (m_pCurMap && m_pUserDB && m_pCurMap->m_pMapSet && !m_pCurMap->m_pMapSet->m_nMapType)
    {
      m_pUserDB->Update_Map(static_cast<unsigned __int8>(m_pCurMap->m_pMapSet->m_dwIndex), m_fCurPos);
    }

    CheckPosInTown();
    AutoRecover();
    AutoRecover_Animus();
    AutoCharge_Booster();

    unsigned __int16 currentTol[4]{};
    currentTol[0] = static_cast<unsigned __int16>(GetFireTol());
    currentTol[1] = static_cast<unsigned __int16>(GetWaterTol());
    currentTol[2] = static_cast<unsigned __int16>(GetSoilTol());
    currentTol[3] = static_cast<unsigned __int16>(GetWindTol());
    if (memcmp_0(m_zLastTol, currentTol, sizeof(currentTol)) != 0)
    {
      memcpy_0(m_zLastTol, currentTol, sizeof(currentTol));
      SendMsg_AlterTol();
    }

    m_byDefMatCount = 0;
  }

  SenseState();

  if (m_dwNextTimeDungeonDie && m_dwNextTimeDungeonDie < currentTime)
  {
    if (m_bCorpse && m_pCurMap && m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      pc_Revival(true);
    }
    m_dwNextTimeDungeonDie = 0;
  }

  if (m_bMineMode && GetLoopTime() > m_dwMineNextTime)
  {
    pc_MineCancle();
  }

  if (m_bMove && GetLoopTime() > m_dwLastSetPointTime && (GetLoopTime() - m_dwLastSetPointTime) > 3000)
  {
    const bool outOfStopRange = IsOutExtraStopPos(m_fCurPos);
    SendMsg_Stop(outOfStopRange);
    CCharacter::Stop();
  }

  if ((currentTime - m_dwLastCheckRegionTime) > 0x1388)
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
        m_pUserDB ? m_pUserDB->m_szAccountID : "<null-id>",
        m_Param.GetCharNameA(),
        m_dwObjSerial,
        m_nCheckMovePacket,
        (m_pCurMap && m_pCurMap->m_pMapSet) ? m_pCurMap->m_pMapSet->m_strCode : "<null-map>");
    }

    m_nCheckMovePacket = 0;
    m_dwLastCheckRegionTime = currentTime;
  }

  CheckAlterMaxPoint();

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
}

void CPlayer::SendMsg_SetHPInform()
{
  unsigned __int16 hp = static_cast<unsigned __int16>(m_Param.GetHP());
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
if (m_bCorpse || m_bMapLoading || CGameObject::GetCurSecNum() == static_cast<unsigned int>(-1))
  {
    return static_cast<unsigned int>(m_Param.GetHP());
  }

  if (pDst && m_EP.GetEff_Have(54) > 0.0f && bJadeReturn && pDst->m_ObjID.m_byID == 0)
  {
    const int returnDamage = static_cast<int>(static_cast<float>(nDam) * m_EP.GetEff_Have(54));
    if (returnDamage > 0)
    {
      pDst->SetDamage(returnDamage, this, static_cast<int>(GetLevel()), false, -1, 0, false);
    }
  }

  CCharacter::BreakStealth();
  BreakCloakBooster();

  if (m_EP.GetEff_State(14))
  {
    CCharacter::RemoveSFContHelpByEffect(2, 14);
  }

  if (nDam <= 0)
  {
    if (nDam == -1)
    {
      const int attackPart = pDst ? static_cast<int>(pDst->GetAttackRandomPart()) : static_cast<int>(GetAttackRandomPart());
      pc_PlayAttack_Gen(pDst, static_cast<unsigned __int8>(attackPart), 0xFFFFu, 0xFFFFu, true);
    }
    else if (nDam == -2 && m_nLastBeatenPart == 5 && pDst && !IsInTown() && IsPassMasteryLimitLvDiff(nDstLv))
    {
      if (m_byDefMatCount < 2)
      {
        ++m_byDefMatCount;
        const unsigned int masteryCum = GetMasteryCumAfterAttack(nDstLv);
        Emb_AlterStat(2u, 0u, static_cast<int>(masteryCum), 0, "CPlayer::SetDamage() shield-defense", true);
      }
    }

    m_nLastBeatenPart = 0;
    SetBattleMode(false);
    return static_cast<unsigned int>(m_Param.GetHP());
  }

  if (IsRidingUnit() && m_pUsingUnit)
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
    if (m_Param.GetFP() > 0 && m_EP.GetEff_State(22))
    {
      int fpDamage = 2 * nDam;
      const int currentFp = m_Param.GetFP();
      if (fpDamage > currentFp)
      {
        const int overflow = fpDamage - currentFp;
        SetHP(m_Param.GetHP() - (overflow / 2), false);
        SendMsg_SetHPInform();
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
      if (hpRateDamage >= 0.02f && m_byDefMatCount < 2)
      {
        ++m_byDefMatCount;
        const unsigned int masteryCum = GetMasteryCumAfterAttack(nDstLv);
        const int addValue = m_nAddDfnMstByClass * static_cast<int>(masteryCum);
        if (m_nLastBeatenPart != 5 && addValue > 0)
        {
          Emb_AlterStat(1u, 0u, addValue, 0, "CPlayer::SetDamage() defense", true);
        }
      }
    }
  }

  if (GetHP() == 0)
  {
    m_bCorpse = true;
    m_bMove = false;
  }

  if (bCrt)
  {
    SetStun(true);
  }

  m_nLastBeatenPart = 0;
  SetBattleMode(false);
  return static_cast<unsigned int>(m_Param.GetHP());
}

