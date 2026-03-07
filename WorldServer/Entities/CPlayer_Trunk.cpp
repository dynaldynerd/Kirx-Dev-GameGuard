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
void SyncTrunkMetaToUserDB(CPlayer *player)
{
  if (!player->m_pUserDB)
  {
    return;
  }

  player->m_pUserDB->m_AvatorData.dbTrunk.bySlotNum = player->m_Param.m_byTrunkSlotNum;
  player->m_pUserDB->m_AvatorData.dbTrunk.byExtSlotNum = player->m_Param.m_byExtTrunkSlotNum;
  strcpy_0(player->m_pUserDB->m_AvatorData.dbTrunk.wszPasswd, player->m_Param.m_wszTrunkPasswd);
  player->m_pUserDB->m_AvatorData.dbTrunk.dDalant = player->m_Param.m_dTrunkDalant;
  player->m_pUserDB->m_AvatorData.dbTrunk.dGold = player->m_Param.m_dTrunkGold;
  player->m_pUserDB->m_AvatorData.dbTrunk.byHintIndex = player->m_Param.m_byTrunkHintIndex;
  strcpy_0(player->m_pUserDB->m_AvatorData.dbTrunk.wszHintAnswer, player->m_Param.m_wszTrunkHintAnswer);
  player->m_pUserDB->m_bDataUpdate = true;
}

void UpdateStorageClientSlotInUserDB(
  CPlayer *player,
  unsigned __int8 byStorageCode,
  unsigned __int8 byStorageIndex,
  unsigned __int8 byClientSlotIndex)
{
  if (!player->m_pUserDB)
  {
    return;
  }

  if (byStorageCode == 0)
  {
    player->m_pUserDB->m_AvatorData.dbInven.m_List[byStorageIndex].Key.bySlotIndex = byClientSlotIndex;
    player->m_pUserDB->m_bDataUpdate = true;
    return;
  }

  if (byStorageCode == 5)
  {
    player->m_pUserDB->m_AvatorData.dbTrunk.m_List[byStorageIndex].Key.bySlotIndex = byClientSlotIndex;
    player->m_pUserDB->m_bDataUpdate = true;
    return;
  }

  if (byStorageCode == 7)
  {
    player->m_pUserDB->m_AvatorData.dbTrunk.m_ExtList[byStorageIndex].Key.bySlotIndex = byClientSlotIndex;
    player->m_pUserDB->m_bDataUpdate = true;
  }
}

bool GetTrunkSlotRaceBySerial(
  CPlayer *player,
  unsigned __int8 byStorageCode,
  unsigned __int16 wItemSerial,
  unsigned __int8 *pOutRace)
{
  if (byStorageCode == 5)
  {
    const unsigned __int8 maxSlot = player->m_Param.GetTrunkSlotNum();
    for (unsigned __int8 slot = 0; slot < maxSlot; ++slot)
    {
      _STORAGE_LIST::_db_con *item = &player->m_Param.m_dbTrunk.m_pStorageList[slot];
      if (item->m_bLoad && item->m_wSerial == wItemSerial)
      {
        *pOutRace = player->m_Param.m_dbTrunk.m_byItemSlotRace[slot];
        return true;
      }
    }
    return false;
  }

  if (byStorageCode == 7)
  {
    const unsigned __int8 maxSlot = player->m_Param.GetExtTrunkSlotNum();
    for (unsigned __int8 slot = 0; slot < maxSlot; ++slot)
    {
      _STORAGE_LIST::_db_con *item = &player->m_Param.m_dbExtTrunk.m_pStorageList[slot];
      if (item->m_bLoad && item->m_wSerial == wItemSerial)
      {
        *pOutRace = player->m_Param.m_dbExtTrunk.m_byItemSlotRace[slot];
        return true;
      }
    }
  }

  return false;
}

bool IsEconomyFeeLevel(int level)
{
  return level == 30 || level == 40 || level == 50 || level == 60;
}
}

void CPlayer::SendMsg_ExtTrunkExtendResult(char byRetCode, unsigned __int8 bySlotNum, unsigned __int8 byLackSlotNum)
{
  _Exttrunk_extend_result_zocl payload{};
  payload.byRetCode = byRetCode;
  payload.byPackNum = static_cast<char>(bySlotNum / 20);
  payload.byExtendNum = static_cast<char>(byLackSlotNum / 20);

  unsigned __int8 type[2] = {34, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), sizeof(payload));
}

void CPlayer::pc_TrunkEstRequest(char *pwszPassword, unsigned __int8 byHintIndex, char *pwszHintAnswer)
{
  unsigned __int8 byRetCode = 0;
  unsigned int dwSub = 100000;
  if (this->m_pUserDB && this->m_pUserDB->m_bCreateTrunkFree)
  {
    dwSub = 0;
  }

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (strlen_0(pwszPassword) < 2 || strlen_0(pwszPassword) > 12)
  {
    byRetCode = 3;
  }
  else if (this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 5;
  }
  else if (this->m_Param.GetDalant() < dwSub)
  {
    byRetCode = 6;
  }
  else if (this->m_pUserDB && !strcmp_0(pwszPassword, this->m_pUserDB->m_szUILock_PW))
  {
    byRetCode = 24;
  }
  else if (!IsSQLValidString(pwszPassword) || !IsSQLValidString(pwszHintAnswer))
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::pc_TrunkEstRequest() : %u(%s) !::IsSQLValidString( pwszPassword(%s) ) || !::IsSQLValidString(pwszHintAnswer(%s) ) Invalid!",
      this->m_dwObjSerial,
      this->m_Param.GetCharNameA(),
      pwszPassword,
      pwszHintAnswer);
    byRetCode = 25;
  }

  if (!byRetCode)
  {
    this->SubDalant(dwSub);
    this->m_Param.m_byTrunkSlotNum = 20;
    strcpy_0(this->m_Param.m_wszTrunkPasswd, pwszPassword);
    this->m_Param.m_byTrunkHintIndex = byHintIndex;
    strcpy_0(this->m_Param.m_wszTrunkHintAnswer, pwszHintAnswer);
    this->m_Param.m_dTrunkDalant = 0.0;
    this->m_Param.m_dTrunkGold = 0.0;
    this->m_Param.m_bTrunkOpen = true;
    this->m_Param.m_dbTrunk.SetUseListNum(20);

    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_TrunkSlotNum(this->m_Param.GetTrunkSlotNum());
      this->m_pUserDB->Update_TrunkPassword(this->m_Param.GetTrunkPasswdW());
      this->m_pUserDB->Update_TrunkMoney(this->m_Param.m_dTrunkDalant, this->m_Param.m_dTrunkGold);
      this->m_pUserDB->Update_TrunkHint(this->m_Param.m_byTrunkHintIndex, this->m_Param.m_wszTrunkHintAnswer);
    }

    const int level = this->m_Param.GetLevel();
    if (IsEconomyFeeLevel(level))
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(this->m_Param.GetRaceCode(), level, dwSub);
    }
  }

  this->SendMsg_TrunkEstResult(byRetCode, this->m_Param.GetDalant());
  if (!byRetCode)
  {
    this->SendMsg_TrunkDownloadResult(0);
  }
}

void CPlayer::pc_TrunkDownloadRequest(char *pwszPassword)
{
  unsigned __int8 byRetCode = 0;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else if (strcmp_0(pwszPassword, this->m_Param.GetTrunkPasswdW()))
  {
    byRetCode = 1;
  }

  if (!byRetCode)
  {
    this->m_Param.m_bTrunkOpen = true;
  }

  this->SendMsg_TrunkDownloadResult(byRetCode);
}

void CPlayer::pc_TrunkChangePasswdRequest(
  char *pwszPrevPassword,
  char *pwszChngPassword,
  unsigned __int8 byHintIndex,
  char *pwszHintAnswer)
{
  unsigned __int8 byRetCode = 0;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else if (
    strlen_0(pwszPrevPassword) < 2
    || strlen_0(pwszPrevPassword) > 12
    || strcmp_0(pwszPrevPassword, this->m_Param.GetTrunkPasswdW()))
  {
    byRetCode = 1;
  }
  else if (strlen_0(pwszChngPassword) < 2 || strlen_0(pwszChngPassword) > 12)
  {
    byRetCode = 3;
  }
  else if (this->m_pUserDB && !strcmp_0(pwszChngPassword, this->m_pUserDB->m_szUILock_PW))
  {
    byRetCode = 24;
  }
  else if (!IsSQLValidString(pwszChngPassword) || !IsSQLValidString(pwszHintAnswer))
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::pc_TrunkEstRequest() : %u(%s) !::IsSQLValidString( pwszPassword(%s) ) || !::IsSQLValidString(pwszHintAnswer(%s)) Invalid!",
      this->m_dwObjSerial,
      this->m_Param.GetCharNameA(),
      pwszChngPassword,
      pwszHintAnswer);
    byRetCode = 25;
  }

  if (!byRetCode)
  {
    strcpy_0(this->m_Param.m_wszTrunkPasswd, pwszChngPassword);
    this->m_Param.m_byTrunkHintIndex = byHintIndex;
    strcpy_0(this->m_Param.m_wszTrunkHintAnswer, pwszHintAnswer);
    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_TrunkPassword(this->m_Param.GetTrunkPasswdW());
      this->m_pUserDB->Update_TrunkHint(this->m_Param.m_byTrunkHintIndex, this->m_Param.m_wszTrunkHintAnswer);
    }
  }

  this->SendMsg_TrunkChangPasswdResult(static_cast<char>(byRetCode));
}

void CPlayer::pc_TrunkExtendRequest()
{
  unsigned __int8 byRetCode = 0;
  const unsigned int dwSub = 500000;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else if (this->m_Param.GetTrunkSlotNum() >= 100u)
  {
    byRetCode = 4;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else if (this->m_Param.GetDalant() < dwSub)
  {
    byRetCode = 6;
  }

  if (!byRetCode)
  {
    this->SubDalant(dwSub);
    this->m_Param.m_byTrunkSlotNum = static_cast<unsigned __int8>(this->m_Param.m_byTrunkSlotNum + 20);
    this->m_Param.m_dbTrunk.SetUseListNum(this->m_Param.m_byTrunkSlotNum);

    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_TrunkSlotNum(this->m_Param.GetTrunkSlotNum());
      SyncTrunkMetaToUserDB(this);
    }

    const int level = this->m_Param.GetLevel();
    if (IsEconomyFeeLevel(level))
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(this->m_Param.GetRaceCode(), level, dwSub);
    }
  }

  this->SendMsg_TrunkExtendResult(
    static_cast<char>(byRetCode),
    this->m_Param.GetTrunkSlotNum(),
    this->m_Param.GetDalant(),
    dwSub);
}

void CPlayer::pc_PotionUseTrunkExtend()
{
  unsigned __int8 byRetCode = 0;
  unsigned __int8 byLackSlotNum = 0;

  if (m_Param.GetTrunkSlotNum())
  {
    if (m_Param.GetExtTrunkSlotNum() < 40)
    {
      byLackSlotNum = static_cast<unsigned __int8>(100 - m_Param.GetTrunkSlotNum());
    }
    else
    {
      byRetCode = 4;
    }
  }
  else
  {
    byRetCode = 2;
  }

  if (!byRetCode)
  {
    m_Param.m_byTrunkSlotNum = static_cast<unsigned __int8>(m_Param.m_byTrunkSlotNum + byLackSlotNum);
    m_Param.m_byExtTrunkSlotNum = static_cast<unsigned __int8>(m_Param.m_byExtTrunkSlotNum + 20);
    m_Param.m_dbTrunk.SetUseListNum(m_Param.m_byTrunkSlotNum);
    m_Param.m_dbExtTrunk.SetUseListNum(m_Param.m_byExtTrunkSlotNum);

    const unsigned __int8 trunkSlotNum = m_Param.GetTrunkSlotNum();
    m_pUserDB->Update_TrunkSlotNum(trunkSlotNum);
    const unsigned __int8 extTrunkSlotNum = m_Param.GetExtTrunkSlotNum();
    m_pUserDB->Update_ExtTrunkSlotNum(extTrunkSlotNum);
  }

  const unsigned __int8 extTrunkSlotNum = m_Param.GetExtTrunkSlotNum();
  SendMsg_ExtTrunkExtendResult(static_cast<char>(byRetCode), extTrunkSlotNum, byLackSlotNum);
}

void CPlayer::pc_TrunkAlterItemSlotRequest(
  unsigned int dwItemSerial,
  unsigned __int8 byClientSlotIndex,
  unsigned __int8 byStorageIndex)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  _STORAGE_LIST::_db_con *item = this->m_Param.m_pStoragePtr[byStorageIndex]->GetPtrFromSerial(
    static_cast<unsigned __int16>(dwItemSerial));
  if (!item)
  {
    return;
  }

  item->m_byClientIndex = byClientSlotIndex;
  UpdateStorageClientSlotInUserDB(this, byStorageIndex, item->m_byStorageIndex, byClientSlotIndex);
}

void CPlayer::pc_TrunkResDivision(
  unsigned __int16 wStartSerial,
  unsigned __int16 wTarSerial,
  unsigned __int16 wMoveAmount,
  unsigned __int8 byStorageIndex)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *pStartOre = nullptr;
  _STORAGE_LIST::_db_con *pTargetOre = nullptr;

  if (!IsBeNearStore(this, 10))
  {
    byErrCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byErrCode = 14;
  }
  else
  {
    if (byStorageIndex == 5)
    {
      pStartOre = this->m_Param.m_dbTrunk.GetPtrFromSerial(wStartSerial);
    }
    else if (byStorageIndex == 7)
    {
      pStartOre = this->m_Param.m_dbExtTrunk.GetPtrFromSerial(wStartSerial);
    }

    if (!pStartOre)
    {
      byErrCode = 5;
    }
    else if (pStartOre->m_bLock)
    {
      byErrCode = 11;
    }
    else
    {
      if (byStorageIndex == 5)
      {
        pTargetOre = this->m_Param.m_dbTrunk.GetPtrFromSerial(wTarSerial);
      }
      else if (byStorageIndex == 7)
      {
        pTargetOre = this->m_Param.m_dbExtTrunk.GetPtrFromSerial(wTarSerial);
      }

      if (!pTargetOre)
      {
        byErrCode = 5;
      }
      else if (pTargetOre->m_bLock)
      {
        byErrCode = 11;
      }
      else if (!IsOverLapItem(pStartOre->m_byTableCode) || !IsOverLapItem(pTargetOre->m_byTableCode))
      {
        byErrCode = 3;
      }
      else if (pStartOre->m_byCsMethod && pTargetOre->m_byCsMethod && pStartOre->m_dwT != pTargetOre->m_dwT)
      {
        byErrCode = 23;
      }
      else if (
        pStartOre->m_byTableCode != pTargetOre->m_byTableCode
        || pStartOre->m_wItemIndex != pTargetOre->m_wItemIndex)
      {
        byErrCode = 6;
      }
      else if (pStartOre->m_dwDur < wMoveAmount)
      {
        byErrCode = 8;
      }
      else if (pTargetOre->m_dwDur + wMoveAmount > 99)
      {
        byErrCode = 9;
      }
    }
  }

  if (!byErrCode)
  {
    this->Emb_AlterDurPoint(byStorageIndex, pStartOre->m_byStorageIndex, -wMoveAmount, false, false);
    this->Emb_AlterDurPoint(byStorageIndex, pTargetOre->m_byStorageIndex, wMoveAmount, false, false);
  }

  this->SendMsg_TrunkResDivision(static_cast<char>(byErrCode), pStartOre, pTargetOre);
}

void CPlayer::pc_TrunkPotionDivision(
  unsigned __int16 wStartSerial,
  unsigned __int16 wTarSerial,
  unsigned __int16 wMoveAmount,
  unsigned __int8 byStorageIndex)
{
  char byRetCode = 0;
  _STORAGE_LIST::_db_con *pStartPotion = nullptr;
  _STORAGE_LIST::_db_con *pTargetPotion = nullptr;
  unsigned __int16 wParentAmount = 0;
  unsigned __int16 wChildAmount = 0;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else
  {
    pStartPotion = this->m_Param.m_pStoragePtr[byStorageIndex]->GetPtrFromSerial(wStartSerial);
    if (!pStartPotion)
    {
      byRetCode = static_cast<char>(-5);
    }
    else if (pStartPotion->m_bLock)
    {
      byRetCode = static_cast<char>(-11);
    }
    else
    {
      pTargetPotion = this->m_Param.m_pStoragePtr[byStorageIndex]->GetPtrFromSerial(wTarSerial);
      if (!pTargetPotion)
      {
        byRetCode = static_cast<char>(-5);
      }
      else if (pTargetPotion->m_bLock)
      {
        byRetCode = static_cast<char>(-11);
      }
      else if (pStartPotion->m_byTableCode != 13 || pTargetPotion->m_byTableCode != 13)
      {
        byRetCode = static_cast<char>(-3);
      }
      else if (pStartPotion->m_wItemIndex != pTargetPotion->m_wItemIndex)
      {
        byRetCode = static_cast<char>(-6);
      }
      else if (pStartPotion->m_dwDur < wMoveAmount || pTargetPotion->m_dwDur + wMoveAmount > 99)
      {
        byRetCode = static_cast<char>(-4);
      }
    }
  }

  if (!byRetCode)
  {
    wParentAmount = static_cast<unsigned __int16>(
      this->Emb_AlterDurPoint(byStorageIndex, pStartPotion->m_byStorageIndex, -wMoveAmount, false, false));
    wChildAmount = static_cast<unsigned __int16>(
      this->Emb_AlterDurPoint(byStorageIndex, pTargetPotion->m_byStorageIndex, wMoveAmount, false, false));
  }

  this->SendMsg_TrunkPotionDivision(wStartSerial, wParentAmount, wTarSerial, wChildAmount, byRetCode);
}

void CPlayer::pc_TrunkIoMoveRequest(
  unsigned __int8 byStartStorageIndex,
  unsigned __int8 byTarStorageIndex,
  unsigned __int16 wItemSerial,
  unsigned __int8 byClientSlotIndex)
{
  unsigned __int8 byRetCode = 0;
  unsigned int dwConsumDalant = 0;
  _STORAGE_LIST::_db_con *pFixingItem = nullptr;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else
  {
    if (byTarStorageIndex == 5 && byClientSlotIndex >= this->m_Param.GetTrunkSlotNum())
    {
      byRetCode = 8;
    }
    else if (byTarStorageIndex == 7 && byClientSlotIndex >= this->m_Param.GetExtTrunkSlotNum())
    {
      byRetCode = 8;
    }
    else if (this->m_Param.m_pStoragePtr[byTarStorageIndex]->GetNumEmptyCon() <= 0)
    {
      byRetCode = 8;
    }
    else
    {
      pFixingItem = this->m_Param.m_pStoragePtr[byStartStorageIndex]->GetPtrFromSerial(wItemSerial);
      if (!pFixingItem)
      {
        byRetCode = 9;
      }
      else if (pFixingItem->m_bLock)
      {
        byRetCode = 10;
      }
      else if (
        (byTarStorageIndex == 5 || byTarStorageIndex == 7)
        && !IsTrunkIOAble(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex))
      {
        byRetCode = 18;
      }
      else
      {
        if (byTarStorageIndex == 1)
        {
          if (pFixingItem->m_byTableCode >= 8u)
          {
            byRetCode = 44;
          }
          else
          {
            _STORAGE_LIST::_db_con *pEquipSlot = &this->m_Param.m_dbEquip.m_pStorageList[pFixingItem->m_byTableCode];
            if (pEquipSlot->m_bLoad || !this->_check_equip_part(pFixingItem))
            {
              byRetCode = 44;
            }
          }
        }
        else if (byTarStorageIndex == 2)
        {
          if (
            (pFixingItem->m_byTableCode != 8 && pFixingItem->m_byTableCode != 9 && pFixingItem->m_byTableCode != 10)
            || !this->_check_embel_part(pFixingItem))
          {
            byRetCode = 44;
          }
          else
          {
            int sameTypeCount = 0;
            for (int index = 0; index < 7; ++index)
            {
              _STORAGE_LIST::_db_con *pEmbellish = &this->m_Param.m_dbEmbellish.m_pStorageList[index];
              if (pEmbellish->m_bLoad && pEmbellish->m_byTableCode == pFixingItem->m_byTableCode)
              {
                ++sameTypeCount;
              }
            }

            if (sameTypeCount > 1)
            {
              byRetCode = 44;
            }
          }
        }

        if (!byRetCode)
        {
          if (byStartStorageIndex == 5 || byStartStorageIndex == 7)
          {
            dwConsumDalant = static_cast<unsigned int>(
              GetItemStoragePrice(pFixingItem->m_byTableCode, pFixingItem->m_wItemIndex, this->m_Param.GetRaceCode()));
            if (IsOverLapItem(pFixingItem->m_byTableCode))
            {
              dwConsumDalant *= static_cast<unsigned int>(pFixingItem->m_dwDur);
            }

            const unsigned __int64 taxAdjusted =
              static_cast<unsigned __int64>(eGetTexRate(this->m_Param.GetRaceCode()) + 10000)
              * static_cast<unsigned __int64>(dwConsumDalant);
            dwConsumDalant = static_cast<unsigned int>(taxAdjusted / 10000);
          }

          if (this->m_Param.GetDalant() < dwConsumDalant)
          {
            byRetCode = 6;
          }
          else
          {
            const int raceCode = this->m_Param.GetRaceCode();
            if (byStartStorageIndex == 5)
            {
              if (this->m_Param.GetTrunkSlotRace(wItemSerial) != raceCode)
              {
                byRetCode = 17;
              }
            }
            else if (byStartStorageIndex == 7)
            {
              if (this->m_Param.GetExtTrunkSlotRace(wItemSerial) != raceCode)
              {
                byRetCode = 17;
              }
            }
          }
        }
      }
    }
  }

  if (!byRetCode)
  {
    _STORAGE_LIST::_db_con copyItem{};
    memcpy_0(&copyItem, pFixingItem, sizeof(copyItem));

    if (!this->Emb_DelStorage(
          byStartStorageIndex,
          pFixingItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_TrunkIoMoveRequest()"))
    {
      this->SendMsg_TrunkIoResult(0, 18, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    _STORAGE_LIST::_db_con *pAdded = this->Emb_AddStorage(byTarStorageIndex, &copyItem, false, false);
    if (!pAdded)
    {
      this->Emb_AddStorage(byStartStorageIndex, &copyItem, false, false);
      this->SendMsg_TrunkIoResult(0, 18, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    pAdded->m_byClientIndex = byClientSlotIndex;
    if (byTarStorageIndex == 5 || byTarStorageIndex == 7)
    {
      UpdateStorageClientSlotInUserDB(this, byTarStorageIndex, pAdded->m_byStorageIndex, byClientSlotIndex);
    }

    if (dwConsumDalant)
    {
      this->SubDalant(dwConsumDalant);

      const int level = this->m_Param.GetLevel();
      if (IsEconomyFeeLevel(level))
      {
        CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(this->m_Param.GetRaceCode(), level, dwConsumDalant);
      }
    }
  }

  this->SendMsg_TrunkIoResult(0, static_cast<char>(byRetCode), this->m_Param.GetDalant(), dwConsumDalant);
}

void CPlayer::pc_TrunkIoSwapRequest(
  unsigned __int8 byStartStorageIndex,
  unsigned __int8 byTarStorageIndex,
  unsigned __int16 wStartItemSerial,
  unsigned __int16 wTarItemSerial)
{
  unsigned __int8 byRetCode = 0;
  unsigned int dwConsumDalant = 0;
  _STORAGE_LIST::_db_con *pStartItem = nullptr;
  _STORAGE_LIST::_db_con *pTargetItem = nullptr;
  _STORAGE_LIST::_db_con *pFeeItem = nullptr;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else
  {
    pStartItem = this->m_Param.m_pStoragePtr[byStartStorageIndex]->GetPtrFromSerial(wStartItemSerial);
    if (!pStartItem)
    {
      byRetCode = 9;
    }
    else if (pStartItem->m_bLock)
    {
      byRetCode = 10;
    }
    else
    {
      pTargetItem = this->m_Param.m_pStoragePtr[byTarStorageIndex]->GetPtrFromSerial(wTarItemSerial);
      if (!pTargetItem)
      {
        byRetCode = 9;
      }
      else if (pTargetItem->m_bLock)
      {
        byRetCode = 10;
      }
      else if (
        (byTarStorageIndex == 5 || byTarStorageIndex == 7)
        && !IsTrunkIOAble(pStartItem->m_byTableCode, pStartItem->m_wItemIndex))
      {
        byRetCode = 18;
      }
      else if (
        (byStartStorageIndex == 5 || byStartStorageIndex == 7)
        && !IsTrunkIOAble(pTargetItem->m_byTableCode, pTargetItem->m_wItemIndex))
      {
        byRetCode = 18;
      }
      else
      {
        if (byTarStorageIndex == 1)
        {
          if (pStartItem->m_byTableCode >= 8u)
          {
            byRetCode = 44;
          }
        }
        else if (
          byTarStorageIndex == 2
          && (pStartItem->m_byTableCode != 8 && pStartItem->m_byTableCode != 9 && pStartItem->m_byTableCode != 10))
        {
          byRetCode = 44;
        }

        if (!byRetCode && (byTarStorageIndex == 1 || byTarStorageIndex == 2))
        {
          if (pStartItem->m_byTableCode != pTargetItem->m_byTableCode)
          {
            byRetCode = 44;
          }
          else if (!IsItemEquipCivil(
                     pStartItem->m_byTableCode,
                     pStartItem->m_wItemIndex,
                     this->m_Param.GetRaceSexCode()))
          {
            byRetCode = 44;
          }
          else if (!this->IsEffectableEquip(pStartItem))
          {
            byRetCode = 44;
          }
        }

        if (!byRetCode)
        {
          if (byStartStorageIndex == 1)
          {
            if (pTargetItem->m_byTableCode >= 8u)
            {
              byRetCode = 44;
            }
          }
          else if (
            byStartStorageIndex == 2
            && (pTargetItem->m_byTableCode != 8 && pTargetItem->m_byTableCode != 9 && pTargetItem->m_byTableCode != 10))
          {
            byRetCode = 44;
          }
        }

        if (!byRetCode && (byStartStorageIndex == 1 || byStartStorageIndex == 2))
        {
          if (pTargetItem->m_byTableCode != pStartItem->m_byTableCode)
          {
            byRetCode = 44;
          }
          else if (!IsItemEquipCivil(
                     pTargetItem->m_byTableCode,
                     pTargetItem->m_wItemIndex,
                     this->m_Param.GetRaceSexCode()))
          {
            byRetCode = 44;
          }
          else if (!this->IsEffectableEquip(pTargetItem))
          {
            byRetCode = 44;
          }
        }

        if (!byRetCode)
        {
          if ((byStartStorageIndex == 5 && byTarStorageIndex != 7) || (byStartStorageIndex == 7 && byTarStorageIndex != 5))
          {
            pFeeItem = pStartItem;
          }
          if ((byTarStorageIndex == 5 && byStartStorageIndex != 7) || (byTarStorageIndex == 7 && byStartStorageIndex != 5))
          {
            pFeeItem = pTargetItem;
          }

          if (pFeeItem)
          {
            dwConsumDalant = static_cast<unsigned int>(
              GetItemStoragePrice(pFeeItem->m_byTableCode, pFeeItem->m_wItemIndex, this->m_Param.GetRaceCode()));
            if (IsOverLapItem(pFeeItem->m_byTableCode))
            {
              dwConsumDalant *= static_cast<unsigned int>(pFeeItem->m_dwDur);
            }

            const unsigned __int64 taxAdjusted =
              static_cast<unsigned __int64>(eGetTexRate(this->m_Param.GetRaceCode()) + 10000)
              * static_cast<unsigned __int64>(dwConsumDalant);
            dwConsumDalant = static_cast<unsigned int>(taxAdjusted / 10000);
          }

          if (this->m_Param.GetDalant() < dwConsumDalant)
          {
            byRetCode = 6;
          }
          else if (byStartStorageIndex == 5)
          {
            if (this->m_Param.GetTrunkSlotRace(wStartItemSerial) != this->m_Param.GetRaceCode())
            {
              byRetCode = 17;
            }
          }
          else if (byStartStorageIndex == 7)
          {
            if (this->m_Param.GetExtTrunkSlotRace(wStartItemSerial) != this->m_Param.GetRaceCode())
            {
              byRetCode = 17;
            }
          }
        }
      }
    }
  }

  if (!byRetCode)
  {
    const unsigned __int8 startClientIndex = pStartItem->m_byClientIndex;
    const unsigned __int8 targetClientIndex = pTargetItem->m_byClientIndex;

    _STORAGE_LIST::_db_con startCopy{};
    _STORAGE_LIST::_db_con targetCopy{};
    memcpy_0(&startCopy, pStartItem, sizeof(startCopy));
    memcpy_0(&targetCopy, pTargetItem, sizeof(targetCopy));

    if (!this->Emb_DelStorage(
          byStartStorageIndex,
          pStartItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_TrunkIoSwapRequest() -- 0"))
    {
      this->SendMsg_TrunkIoResult(1, 18, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    _STORAGE_LIST::_db_con *firstAdded = this->Emb_AddStorage(byStartStorageIndex, &targetCopy, false, false);
    if (!firstAdded)
    {
      this->Emb_AddStorage(byStartStorageIndex, &startCopy, false, false);
      this->SendMsg_TrunkIoResult(1, 18, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    firstAdded->m_byClientIndex = startClientIndex;
    if (byStartStorageIndex == 5 || byStartStorageIndex == 7)
    {
      UpdateStorageClientSlotInUserDB(this, byStartStorageIndex, firstAdded->m_byStorageIndex, startClientIndex);
    }

    if (!this->Emb_DelStorage(
          byTarStorageIndex,
          pTargetItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_TrunkIoSwapRequest() -- 1"))
    {
      this->Emb_DelStorage(
        byStartStorageIndex,
        firstAdded->m_byStorageIndex,
        false,
        false,
        "CPlayer::pc_TrunkIoSwapRequest() -- 1 Fail");
      this->Emb_AddStorage(byStartStorageIndex, &startCopy, false, false);
      this->SendMsg_TrunkIoResult(1, 18, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    _STORAGE_LIST::_db_con *secondAdded = this->Emb_AddStorage(byTarStorageIndex, &startCopy, false, false);
    if (!secondAdded)
    {
      this->Emb_AddStorage(byTarStorageIndex, &targetCopy, false, false);
      this->Emb_DelStorage(
        byStartStorageIndex,
        firstAdded->m_byStorageIndex,
        false,
        false,
        "CPlayer::pc_TrunkIoSwapRequest() -- 2 Fail");
      this->Emb_AddStorage(byStartStorageIndex, &startCopy, false, false);
      byRetCode = 18;
      this->SendMsg_TrunkIoResult(1, static_cast<char>(byRetCode), this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    secondAdded->m_byClientIndex = targetClientIndex;
    if (byTarStorageIndex == 5 || byTarStorageIndex == 7)
    {
      UpdateStorageClientSlotInUserDB(this, byTarStorageIndex, secondAdded->m_byStorageIndex, targetClientIndex);
    }

    this->SubDalant(dwConsumDalant);
    const int level = this->m_Param.GetLevel();
    if (IsEconomyFeeLevel(level))
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(this->m_Param.GetRaceCode(), level, dwConsumDalant);
    }
  }

  this->SendMsg_TrunkIoResult(1, static_cast<char>(byRetCode), this->m_Param.GetDalant(), dwConsumDalant);
}

void CPlayer::pc_TrunkIoMergeRequest(
  unsigned __int8 byStartStorageIndex,
  unsigned __int8 byTarStorageIndex,
  unsigned __int16 wStartItemSerial,
  unsigned __int16 wTarItemSerial,
  unsigned __int16 wMoveAmount)
{
  unsigned __int8 byRetCode = 0;
  unsigned int dwSub = 0;
  _STORAGE_LIST::_db_con *pStartItem = nullptr;
  _STORAGE_LIST::_db_con *pTargetItem = nullptr;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else
  {
    pStartItem = this->m_Param.m_pStoragePtr[byStartStorageIndex]->GetPtrFromSerial(wStartItemSerial);
    if (!pStartItem)
    {
      byRetCode = 9;
    }
    else if (pStartItem->m_bLock)
    {
      byRetCode = 10;
    }
    else
    {
      pTargetItem = this->m_Param.m_pStoragePtr[byTarStorageIndex]->GetPtrFromSerial(wTarItemSerial);
      if (!pTargetItem)
      {
        byRetCode = 9;
      }
      else if (pTargetItem->m_bLock)
      {
        byRetCode = 10;
      }
      else if (!IsTrunkIOAble(pStartItem->m_byTableCode, pStartItem->m_wItemIndex))
      {
        byRetCode = 18;
      }
      else if (!IsTrunkIOAble(pTargetItem->m_byTableCode, pTargetItem->m_wItemIndex))
      {
        byRetCode = 18;
      }
      else if (!IsOverLapItem(pStartItem->m_byTableCode) || !IsOverLapItem(pTargetItem->m_byTableCode))
      {
        byRetCode = 11;
      }
      else if (pStartItem->m_byCsMethod && pTargetItem->m_byCsMethod && pStartItem->m_dwT != pTargetItem->m_dwT)
      {
        byRetCode = 23;
      }
      else if (
        pStartItem->m_byTableCode != pTargetItem->m_byTableCode
        || pStartItem->m_wItemIndex != pTargetItem->m_wItemIndex)
      {
        byRetCode = 12;
      }
      else if (pStartItem->m_dwDur < wMoveAmount)
      {
        byRetCode = 7;
      }
      else if (pTargetItem->m_dwDur + wMoveAmount > 99)
      {
        byRetCode = 7;
      }
      else
      {
        if ((byStartStorageIndex == 5 && byTarStorageIndex != 7) || (byStartStorageIndex == 7 && byTarStorageIndex != 5))
        {
          dwSub = static_cast<unsigned int>(
            GetItemStoragePrice(pStartItem->m_byTableCode, pStartItem->m_wItemIndex, this->m_Param.GetRaceCode()));
          if (IsOverLapItem(pStartItem->m_byTableCode))
          {
            dwSub *= wMoveAmount;
          }

          const unsigned __int64 taxAdjusted =
            static_cast<unsigned __int64>(eGetTexRate(this->m_Param.GetRaceCode()) + 10000)
            * static_cast<unsigned __int64>(dwSub);
          dwSub = static_cast<unsigned int>(taxAdjusted / 10000);
        }

        if (this->m_Param.GetDalant() < dwSub)
        {
          byRetCode = 6;
        }
      }
    }
  }

  if (!byRetCode)
  {
    _STORAGE_LIST::_db_con ioItem{};
    memcpy_0(&ioItem, pStartItem, sizeof(ioItem));
    ioItem.m_dwDur -= wMoveAmount;

    this->Emb_AlterDurPoint(byStartStorageIndex, pStartItem->m_byStorageIndex, -wMoveAmount, false, false);
    this->Emb_AlterDurPoint(byTarStorageIndex, pTargetItem->m_byStorageIndex, wMoveAmount, false, false);

    if (dwSub)
    {
      this->SubDalant(dwSub);
      const int level = this->m_Param.GetLevel();
      if (IsEconomyFeeLevel(level))
      {
        CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(this->m_Param.GetRaceCode(), level, dwSub);
      }
    }
  }

  this->SendMsg_TrunkIoResult(2, static_cast<char>(byRetCode), this->m_Param.GetDalant(), dwSub);
}

void CPlayer::pc_TrunkIoMoneyRequest(unsigned __int8 byCase, int dwDalant, int dwGold)
{
  unsigned __int8 byRetCode = 0;
  const unsigned int dwFeeDalant = 0;

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.m_bTrunkOpen)
  {
    byRetCode = 14;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else if (byCase)
  {
    if (static_cast<double>(dwDalant) > this->m_Param.m_dTrunkDalant || static_cast<double>(dwGold) > this->m_Param.m_dTrunkGold)
    {
      byRetCode = 6;
    }
    else if (dwDalant && !CanAddMoneyForMaxLimMoney(static_cast<unsigned int>(dwDalant), this->m_Param.GetDalant()))
    {
      byRetCode = 19;
    }
    else if (dwGold && !CanAddMoneyForMaxLimGold(static_cast<unsigned int>(dwGold), this->m_Param.GetGold()))
    {
      byRetCode = 19;
    }
  }
  else if (this->m_Param.GetDalant() < static_cast<unsigned int>(dwDalant) || this->m_Param.GetGold() < static_cast<unsigned int>(dwGold))
  {
    byRetCode = 6;
  }
  else if (static_cast<unsigned int>(dwDalant) > 0x77359400u || static_cast<unsigned int>(dwGold) > 500000)
  {
    byRetCode = 21;
  }
  else
  {
    const double nextTrunkGold = this->m_Param.m_dTrunkGold + static_cast<double>(dwGold);
    const double nextTrunkDalant = this->m_Param.m_dTrunkDalant + static_cast<double>(dwDalant);
    if (nextTrunkGold > 500000.0 || nextTrunkDalant > 1000000000.0)
    {
      byRetCode = 20;
    }
  }

  if (!byRetCode)
  {
    if (byCase)
    {
      this->m_Param.SubTrunkDalant(dwDalant);
      this->m_Param.SubTrunkGold(dwGold);
      this->AddDalant(dwDalant, false);
      this->AddGold(dwGold, false);
    }
    else
    {
      this->SubDalant(static_cast<unsigned int>(dwDalant));
      this->SubGold(static_cast<unsigned int>(dwGold));
      this->m_Param.AddTrunkDalant(dwDalant);
      this->m_Param.AddTrunkGold(dwGold);
    }

    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_TrunkMoney(this->m_Param.m_dTrunkGold, this->m_Param.m_dTrunkDalant);
    }
  }

  this->SendMsg_TrunkIoMoneyResult(
    static_cast<char>(byRetCode),
    this->m_Param.m_dTrunkDalant,
    this->m_Param.m_dTrunkGold,
    this->m_Param.GetDalant(),
    this->m_Param.GetGold(),
    dwFeeDalant);
}

void CPlayer::pc_TrunkPwHintIndexRequest()
{
  unsigned __int8 byRetCode = 0;
  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }

  this->SendMsg_TrunkPwHintIndexResult(static_cast<char>(byRetCode), static_cast<char>(this->m_Param.m_byTrunkHintIndex));
}

void CPlayer::pc_TrunkHintAnswerRequest(char *pwszAnswer)
{
  unsigned __int8 byRetCode = 0;
  char wszPassword[13]{};

  if (!IsBeNearStore(this, 10))
  {
    byRetCode = 13;
  }
  else if (!this->m_Param.GetTrunkSlotNum())
  {
    byRetCode = 2;
  }
  else if (strcmp_0(this->m_Param.m_wszTrunkHintAnswer, pwszAnswer))
  {
    byRetCode = 15;
  }

  if (!byRetCode)
  {
    strcpy_0(wszPassword, this->m_Param.m_wszTrunkPasswd);
  }

  this->SendMsg_TrunkHintAnswerResult(static_cast<char>(byRetCode), wszPassword);
}

unsigned __int8 CPlayer::pc_TrunkCreateCostIsFreeRequest()
{
  if (this->m_Param.GetTrunkSlotNum())
  {
    return 2;
  }

  if (!this->m_pUserDB)
  {
    return 1;
  }

  return static_cast<unsigned __int8>(!this->m_pUserDB->m_bCreateTrunkFree);
}

void CPlayer::SendMsg_TrunkEstResult(unsigned __int8 byRetCode, unsigned int dwLeftDalant)
{
  _trunk_est_result_zocl msg{};
  msg.byRetCode = byRetCode;
  msg.dwLeftDalant = dwLeftDalant;
  if (!byRetCode)
  {
    CNationSettingManager *nationSetting = CNationSettingManager::Instance();
    strcpy_s(msg.szPW, sizeof(msg.szPW), nationSetting->m_pData->m_szVaildKey);
  }

  unsigned __int8 pbyType[2] = {34, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 21);
}

void CPlayer::SendMsg_TrunkDownloadResult(unsigned __int8 byRetCode)
{
  _trunk_download_result_zocl msg{};
  msg.byRetCode = byRetCode;
  msg.dDalant = this->m_Param.m_dTrunkDalant;
  msg.dGold = this->m_Param.m_dTrunkGold;
  msg.byListNum = 0;
  msg.byPackNum = 0;
  msg.byExtListNum = 0;
  msg.byExtPackNum = 0;

  __time32_t timeTemp[4]{};
  _time32(timeTemp);

  if (!byRetCode)
  {
    int listCount = 0;
    for (int slot = 0; slot < this->m_Param.GetTrunkSlotNum(); ++slot)
    {
      _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbTrunk.m_pStorageList[slot];
      if (!item->m_bLoad)
      {
        continue;
      }

      msg.List[listCount].wSerial = item->m_wSerial;
      msg.List[listCount].byTableCode = item->m_byTableCode;
      msg.List[listCount].byClientIndex = item->m_byClientIndex;
      msg.List[listCount].wItemIndex = item->m_wItemIndex;
      msg.List[listCount].dwUptInfo = item->m_dwLv;
      msg.List[listCount].dwDurPoint = item->m_dwDur;
      msg.List[listCount].byRace = this->m_Param.m_dbTrunk.m_byItemSlotRace[slot];
      msg.List[listCount].byCsMethod = item->m_byCsMethod;
      msg.List[listCount].dwT = item->m_dwT;
      ++listCount;
    }
    msg.byListNum = static_cast<unsigned __int8>(listCount);
    msg.byPackNum = static_cast<unsigned __int8>(this->m_Param.GetTrunkSlotNum() / 20);

    int extListCount = 0;
    for (int slot = 0; slot < this->m_Param.GetExtTrunkSlotNum(); ++slot)
    {
      _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbExtTrunk.m_pStorageList[slot];
      if (!item->m_bLoad)
      {
        continue;
      }

      msg.ExtList[extListCount].wSerial = item->m_wSerial;
      msg.ExtList[extListCount].byTableCode = item->m_byTableCode;
      msg.ExtList[extListCount].byClientIndex = static_cast<unsigned __int8>(item->m_byClientIndex + 100);
      msg.ExtList[extListCount].wItemIndex = item->m_wItemIndex;
      msg.ExtList[extListCount].dwUptInfo = item->m_dwLv;
      msg.ExtList[extListCount].dwDurPoint = item->m_dwDur;
      msg.ExtList[extListCount].byRace = this->m_Param.m_dbExtTrunk.m_byItemSlotRace[slot];
      msg.ExtList[extListCount].byCsMethod = item->m_byCsMethod;
      msg.ExtList[extListCount].dwT = item->m_dwT;
      ++extListCount;
    }
    msg.byExtListNum = static_cast<unsigned __int8>(extListCount);
    msg.byExtPackNum = static_cast<unsigned __int8>(this->m_Param.GetExtTrunkSlotNum() / 20);
  }

  unsigned __int8 pbyType[2] = {34, 4};
  const unsigned __int16 length = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), length);
}

void CPlayer::SendMsg_TrunkChangPasswdResult(char byRetCode)
{
  _trunk_change_passwd_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);

  unsigned __int8 pbyType[2] = {34, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 1u);
}

void CPlayer::SendMsg_TrunkExtendResult(
  char byRetCode,
  unsigned __int8 bySlotNum,
  unsigned int dwLeftDalant,
  unsigned int dwConsumDanlant)
{
  _trunk_extend_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.byPackNum = static_cast<unsigned __int8>(bySlotNum / 20);
  msg.dwLeftDalant = dwLeftDalant;
  msg.dwConsumDanlant = dwConsumDanlant;

  unsigned __int8 pbyType[2] = {34, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 10);
}

void CPlayer::SendMsg_TrunkResDivision(
  char byErrCode,
  _STORAGE_LIST::_db_con *pStartOre,
  _STORAGE_LIST::_db_con *pTargetOre)
{
  _trunk_res_division_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);
  if (!byErrCode)
  {
    msg.wParentSerial = pStartOre->m_wSerial;
    msg.wParentAmount = static_cast<unsigned __int16>(pStartOre->m_bLoad ? pStartOre->m_dwDur : 0);
    msg.wChildSerial = pTargetOre->m_wSerial;
    msg.wChildAmount = static_cast<unsigned __int16>(pTargetOre->m_dwDur);
  }

  unsigned __int8 pbyType[2] = {34, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 9u);
}

void CPlayer::SendMsg_TrunkPotionDivision(
  unsigned __int16 wSerial,
  unsigned __int16 wParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int16 wChildAmount,
  char nRet)
{
  _trunk_potionsocket_division_result_zocl msg{};
  msg.sErrorCode = nRet;
  msg.wSerial = wSerial;
  msg.wParentAmount = wParentAmount;
  msg.wChildSerial = wChildSerial;
  msg.wChildAmount = wChildAmount;

  unsigned __int8 pbyType[2] = {34, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 9u);
}

void CPlayer::SendMsg_TrunkIoResult(
  char byCase,
  char byRetCode,
  unsigned int dwLeftDalant,
  unsigned int dwConsumDanlant)
{
  _trunk_io_result_zocl msg{};
  msg.byCase = static_cast<unsigned __int8>(byCase);
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.dwLeftDalant = dwLeftDalant;
  msg.dwConsumDanlant = dwConsumDanlant;

  unsigned __int8 pbyType[2] = {34, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 10);
}

void CPlayer::SendMsg_TrunkIoMoneyResult(
  char byRetCode,
  long double dTrunkDalant,
  long double dTrunkGold,
  unsigned int dwDalant,
  unsigned int dwGold,
  unsigned int dwFeeDalant)
{
  _trunk_io_money_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.dTrunkDalant = dTrunkDalant;
  msg.dTrunkGold = dTrunkGold;
  msg.dwCharDalant = dwDalant;
  msg.dwCharGold = dwGold;
  msg.dwFeeDalant = dwFeeDalant;

  unsigned __int8 pbyType[2] = {34, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 29);
}

void CPlayer::SendMsg_TrunkPwHintIndexResult(char byRetCode, char byHintIndex)
{
  _trunk_pw_hint_index_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  msg.byHintIndex = static_cast<unsigned __int8>(byHintIndex);

  unsigned __int8 pbyType[2] = {34, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 2u);
}

void CPlayer::SendMsg_TrunkHintAnswerResult(char byRetCode, char *pwszPassword)
{
  _trunk_hint_answer_result_zocl msg{};
  msg.byRetCode = static_cast<unsigned __int8>(byRetCode);
  if (!byRetCode)
  {
    strcpy_0(msg.wszPassword, pwszPassword);
  }

  unsigned __int8 pbyType[2] = {34, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 14);
}

