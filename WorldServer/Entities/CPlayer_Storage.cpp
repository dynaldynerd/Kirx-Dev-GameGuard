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
#include "Packet/ZoneAccountPacket.h"
#include "Packet/ZoneClientPacket.h"
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
bool IsAsciiAlpha(char ch);
bool IsAsciiDigit(char ch);
bool IsAsciiAlnum(char ch);
bool IsAlphaString(const char *text);
bool IsDigitString(const char *text);
bool IsAlnumMixedString(const char *text);
}

void CPlayer::SendMsg_DamageResult(_STORAGE_LIST::_db_con *pItem)
{
  _shield_equip_dur_dec_zocl packet{};
  packet.byPartIndex = static_cast<char>(this->m_nLastBeatenPart);
  packet.wSerial = pItem->m_wSerial;
  packet.wLeftDurPoint = static_cast<unsigned __int16>(pItem->m_dwDur);

  unsigned __int8 type[2] = {5, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_ItemStorageRefresh(unsigned __int8 byStorageCode)
{
  _STORAGE_LIST *storage = this->m_Param.m_pStoragePtr[byStorageCode];
  _storage_refresh_inform_zocl packet{};

  packet.byStorageCode = static_cast<char>(byStorageCode);
  const unsigned __int8 itemCount = static_cast<unsigned __int8>(storage->GetNumUseCon());
  packet.byItemNum = static_cast<char>(itemCount);

  int outIndex = 0;
  for (int slotIndex = 0; slotIndex < itemCount; ++slotIndex)
  {
    _STORAGE_LIST::_db_con *entry = &storage->m_pStorageList[slotIndex];
    if (entry->m_bLoad)
    {
      packet.wSerial[outIndex++] = entry->m_wSerial;
    }
  }

  unsigned __int8 type[2] = {3, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_UsPotionResultOther(
  char byRetcode,
  unsigned __int16 wPotionIndex,
  CPlayer *pUsePlayer,
  bool bCircle)
{
  _use_potion_result_other_zocl packet{};
  packet.byRetCode = byRetcode;
  packet.wPotionIndex = wPotionIndex;
  packet.idPerformer.byID = pUsePlayer->m_ObjID.m_byID;
  packet.idPerformer.wIndex = pUsePlayer->m_ObjID.m_wIndex;
  packet.idPerformer.dwSerial = pUsePlayer->m_dwObjSerial;
  packet.idDster.byID = this->m_ObjID.m_byID;
  packet.idDster.wIndex = this->m_ObjID.m_wIndex;
  packet.idDster.dwSerial = this->m_dwObjSerial;
  packet.wHP = static_cast<unsigned __int16>(this->m_Param.GetHP());
  packet.wFP = static_cast<unsigned __int16>(this->m_Param.GetFP());
  packet.wSP = static_cast<unsigned __int16>(this->m_Param.GetSP());

  unsigned __int8 type[2] = {7, 108};
  if (bCircle)
  {
    this->CircleReport(type, reinterpret_cast<char *>(&packet), sizeof(packet), true);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
  }
}

void CPlayer::SendMsg_ResSeparation(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pNewOre)
{
  unsigned __int8 pbyType[2] = {13, 7};
  _res_separation_result_zocl packet{};
  packet.byErrCode = byErrCode;
  if (!byErrCode && pStartOre && pNewOre)
  {
    packet.wParentSerial = pStartOre->m_wSerial;
    packet.byParentAmount = static_cast<char>(pStartOre->m_dwDur);
    packet.wChildSerial = pNewOre->m_wSerial;
    packet.byChildAmount = static_cast<char>(pNewOre->m_dwDur);
  }

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_PotionSeparation(
  unsigned __int16 wParentSerial,
  unsigned __int8 byParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int8 byChildAmount,
  char nRet)
{
  unsigned __int8 pbyType[2] = {13, 13};
  _potionsocket_separation_result_zocl packet{};
  packet.sErrorCode = nRet;
  packet.wParentSerial = wParentSerial;
  packet.byParentAmount = static_cast<char>(byParentAmount);
  packet.wChildSerial = wChildSerial;
  packet.byChildAmount = static_cast<char>(byChildAmount);

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_ResDivision(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pTargetOre)
{
  unsigned __int8 pbyType[2] = {13, 9};
  _res_division_result_zocl packet{};
  packet.byErrCode = byErrCode;
  if (!byErrCode)
  {
    packet.wParentSerial = pStartOre->m_wSerial;
    packet.byParentAmount = pStartOre->m_bLoad ? static_cast<char>(pStartOre->m_dwDur) : 0;
    packet.wChildSerial = pTargetOre->m_wSerial;
    packet.byChildAmount = static_cast<char>(pTargetOre->m_dwDur);
  }

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_PotionDivision(
  unsigned __int16 wSerial,
  unsigned __int8 byParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int8 byChildAmount,
  char nRet)
{
  unsigned __int8 pbyType[2] = {13, 15};
  _potionsocket_division_result_zocl packet{};
  packet.sErrorCode = nRet;
  packet.wSerial = wSerial;
  packet.byParentAmount = static_cast<char>(byParentAmount);
  packet.wChildSerial = wChildSerial;
  packet.byChildAmount = static_cast<char>(byChildAmount);

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_ForceInvenChange(char byErrCode)
{
  unsigned __int8 pbyType[2] = {13, 3};
  _force_inven_change_result_zocl packet{};
  packet.byErrCode = byErrCode;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_RevivalOfJade(unsigned __int16 wSuccRate)
{
  (void)wSuccRate;
  _inform_revival_jade_effect_zocl packet{};

  unsigned __int8 type[2] = {3, 58};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_UseJadeResult(unsigned __int8 byErrCode, unsigned __int16 wItemSerial)
{
  _use_active_jade_result_zocl packet{};
  packet.nErrorCode = byErrCode;
  packet.wSerial = wItemSerial;

  unsigned __int8 type[2] = {7, 65};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_NotifyEffectForGetItem(
  char byBoxType,
  unsigned int dwCharSerial,
  char *szCharName,
  _STORAGE_LIST::_db_con *pItem,
  bool bCircle)
{
  _notify_effect_for_get_gold_box_item_zocl packet{};
  packet.byBoxType = byBoxType;
  packet.byTableCode = static_cast<char>(pItem->m_byTableCode);
  packet.wItemIndex = pItem->m_wItemIndex;
  packet.byBoxDur = static_cast<char>(pItem->m_dwDur);
  packet.dwCharSerial = dwCharSerial;
  std::strcpy(packet.szCharacterName, szCharName);

  unsigned __int8 type[2] = {13, static_cast<unsigned __int8>(-106)};
  if (bCircle)
  {
    this->CircleReport(type, reinterpret_cast<char *>(&packet), sizeof(packet), true);
  }
  else
  {
    for (unsigned int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *targetPlayer = &g_Player[index];
      if (targetPlayer && targetPlayer->m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          targetPlayer->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
      }
    }
  }
}

void CPlayer::SendMsg_NewMovePotionResult()
{
  _newmovepotion_stone_result_zocl packet{};
  packet.bStone = true;

  unsigned __int8 type[2] = {17, 45};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_JadeEffectErr(char byErrorCode)
{
  unsigned __int8 type[2] = {59, 4};
  _notify_jade_effect_error_zocl packet{};
  packet.byErrorCode = byErrorCode;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::pc_ResSeparation(unsigned __int16 wStartSerial, unsigned __int8 byMoveAmount)
{
SendMsg_ResSeparation(1u, nullptr, nullptr);
}

void CPlayer::pc_PotionSeparation(unsigned __int16 wSerial, unsigned __int8 byAmount)
{
  SendMsg_PotionSeparation(wSerial, 0, 255, byAmount, static_cast<char>(-1));
}

void CPlayer::pc_ResDivision(unsigned __int16 wStartSerial, unsigned __int16 wTarSerial, unsigned __int8 byMoveAmount)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *startOre = this->m_Param.m_dbInven.GetPtrFromSerial(wStartSerial);
  _STORAGE_LIST::_db_con *targetOre = nullptr;

  if (startOre)
  {
    if (startOre->m_bLock)
    {
      resultCode = 11;
    }
    else
    {
      targetOre = this->m_Param.m_dbInven.GetPtrFromSerial(wTarSerial);
      if (targetOre)
      {
        if (targetOre->m_bLock)
        {
          resultCode = 11;
        }
        else if (IsOverLapItem(startOre->m_byTableCode) && IsOverLapItem(targetOre->m_byTableCode))
        {
          if (startOre->m_byCsMethod && targetOre->m_byCsMethod && startOre->m_dwT != targetOre->m_dwT)
          {
            resultCode = 23;
          }
          else if (startOre->m_byTableCode == targetOre->m_byTableCode)
          {
            if (startOre->m_wItemIndex == targetOre->m_wItemIndex)
            {
              if (startOre->m_dwDur >= byMoveAmount)
              {
                if (targetOre->m_dwDur + byMoveAmount > MAX_ITEM_OVERLAP)
                {
                  resultCode = 9;
                }
              }
              else
              {
                resultCode = 8;
              }
            }
            else
            {
              resultCode = 6;
            }
          }
          else
          {
            resultCode = 6;
          }
        }
        else
        {
          resultCode = 3;
        }
      }
      else
      {
        resultCode = 5;
      }
    }
  }
  else
  {
    resultCode = 5;
  }

  if (!resultCode)
  {
    Emb_AlterDurPoint(0, startOre->m_byStorageIndex, -byMoveAmount, false, false);
    Emb_AlterDurPoint(0, targetOre->m_byStorageIndex, byMoveAmount, false, false);
  }
  SendMsg_ResDivision(resultCode, startOre, targetOre);
}

void CPlayer::pc_PotionDivision(unsigned __int16 wSerial, unsigned __int16 wTarSerial, unsigned __int8 byAmount)
{
  char resultCode = 0;
  _STORAGE_LIST::_db_con *sourcePotion = this->m_Param.m_pStoragePtr[0]->GetPtrFromSerial(wSerial);
  _STORAGE_LIST::_db_con *targetPotion = nullptr;
  unsigned __int8 sourceRet = 0;
  unsigned __int8 targetRet = 0;

  if (sourcePotion)
  {
    if (sourcePotion->m_bLock)
    {
      resultCode = -11;
    }
    else
    {
      targetPotion = this->m_Param.m_pStoragePtr[0]->GetPtrFromSerial(wTarSerial);
      if (targetPotion)
      {
        if (targetPotion->m_bLock)
        {
          resultCode = -11;
        }
        else if (sourcePotion->m_byTableCode == 13 && targetPotion->m_byTableCode == 13)
        {
          if (sourcePotion->m_wItemIndex == targetPotion->m_wItemIndex)
          {
            if (sourcePotion->m_dwDur >= byAmount)
            {
              if (targetPotion->m_dwDur + byAmount > MAX_ITEM_OVERLAP)
              {
                resultCode = -4;
              }
            }
            else
            {
              resultCode = -4;
            }
          }
          else
          {
            resultCode = -6;
          }
        }
        else
        {
          resultCode = -3;
        }
      }
      else
      {
        resultCode = -5;
      }
    }
  }
  else
  {
    resultCode = -5;
  }

  if (!resultCode)
  {
    sourceRet = static_cast<unsigned __int8>(Emb_AlterDurPoint(0, sourcePotion->m_byStorageIndex, -byAmount, false, false));
    targetRet = static_cast<unsigned __int8>(Emb_AlterDurPoint(0, targetPotion->m_byStorageIndex, byAmount, false, false));
  }
  SendMsg_PotionDivision(wSerial, sourceRet, wTarSerial, targetRet, resultCode);
}

void CPlayer::pc_AlterItemSlotRequest(unsigned __int8 byNum, _alter_item_slot_request_clzo::__list *pList)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  for (int index = 0; index < byNum; ++index)
  {
    _STORAGE_LIST::_db_con *item = this->m_Param.m_pStoragePtr[pList[index].byStorageIndex]->GetPtrFromSerial(
      pList[index].dwItemSerial);
    if (item)
    {
      item->m_byClientIndex = pList[index].byClientSlotIndex;
      this->m_pUserDB->Update_ItemSlot(
        pList[index].byStorageIndex,
        item->m_byStorageIndex,
        pList[index].byClientSlotIndex);
    }
  }
}

void CPlayer::pc_AlterLinkBoardSlotRequest(
  unsigned __int8 byNum,
  _alter_link_slot_request_clzo::__list *pList,
  unsigned __int8 byLBLock)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  for (int index = 0; index < byNum; ++index)
  {
    _alter_link_slot_request_clzo::__list &entry = pList[index];
    unsigned __int8 linkCode = entry.byLinkCode;
    unsigned __int16 linkIndex = entry.wIndex;

    if (linkCode == 4)
    {
      unsigned __int8 storageCode = 0;
      _STORAGE_LIST::_db_con *item = this->m_Param.GetPtrItemStorage(entry.wIndex, &storageCode);
      if (item)
      {
        linkIndex = static_cast<unsigned __int16>(item->m_byStorageIndex | (storageCode << 8));
        this->m_Param.PushLink(entry.bySlotIndex, item->m_wSerial, false);
      }
      else
      {
        linkCode = 255;
        linkIndex = 65535;
      }
    }
    else if (linkCode == 255)
    {
      this->m_Param.PopLink(entry.bySlotIndex);
    }

    this->m_pUserDB->Update_LinkBoardSlot(entry.bySlotIndex, linkCode, linkIndex);
  }
  this->m_pUserDB->Update_LinkBoardLock(byLBLock);
}

void CPlayer::pc_ForceInvenChange(_STORAGE_POS_INDIV *pItem, unsigned __int16 wReplaceSerial)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *sourceItem = nullptr;
  _STORAGE_LIST::_db_con *replaceItem = nullptr;

  _STORAGE_LIST *sourceStorage = this->m_Param.m_pStoragePtr[pItem->byStorageCode];
  _STORAGE_LIST *targetStorage = nullptr;
  if (!pItem->byStorageCode)
  {
    targetStorage = this->m_Param.m_pStoragePtr[3];
  }
  else if (pItem->byStorageCode == 3)
  {
    targetStorage = this->m_Param.m_pStoragePtr[0];
  }
  else
  {
    return;
  }

  sourceItem = sourceStorage->GetPtrFromSerial(pItem->wItemSerial);
  if (sourceItem)
  {
    if (sourceItem->m_byTableCode == 15)
    {
      if (sourceItem->m_bLock)
      {
        resultCode = 11;
      }
      else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
      {
        resultCode = 12;
      }
      else
      {
        if (wReplaceSerial != 65535)
        {
          replaceItem = targetStorage->GetPtrFromSerial(wReplaceSerial);
          if (!replaceItem || replaceItem->m_byTableCode != 15)
          {
            resultCode = 2;
          }
          else if (replaceItem->m_bLock)
          {
            resultCode = 11;
          }
          else if (
            CPlayer::s_pnLinkForceItemToEffect[sourceItem->m_wItemIndex]
            != CPlayer::s_pnLinkForceItemToEffect[replaceItem->m_wItemIndex])
          {
            resultCode = 3;
          }
        }
        if (!resultCode && wReplaceSerial == 0xFFFF && targetStorage->GetIndexEmptyCon() == 255)
        {
          resultCode = 5;
        }
        else if (!resultCode && wReplaceSerial == 0xFFFF && !pItem->byStorageCode)
        {
          for (int index = 0; index < targetStorage->m_nUsedNum; ++index)
          {
            _STORAGE_LIST::_db_con *item = &targetStorage->m_pStorageList[index];
            if (item->m_bLoad
                && CPlayer::s_pnLinkForceItemToEffect[item->m_wItemIndex]
                     == CPlayer::s_pnLinkForceItemToEffect[sourceItem->m_wItemIndex])
            {
              resultCode = 4;
              break;
            }
          }
        }
      }
    }
    else
    {
      resultCode = 1;
    }
  }
  else
  {
    resultCode = 1;
  }

  if (!resultCode)
  {
    _STORAGE_LIST::_db_con sourceBackup;
    std::memcpy(&sourceBackup, sourceItem, sizeof(sourceBackup));
    if (!this->Emb_DelStorage(
          sourceStorage->m_nListCode,
          sourceItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_ForceInvenChange() -- 0"))
    {
      this->SendMsg_ForceInvenChange(255);
      return;
    }

    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceBackup;
      std::memcpy(&replaceBackup, replaceItem, sizeof(replaceBackup));
      if (!this->Emb_AddStorage(sourceStorage->m_nListCode, &replaceBackup, true, false))
      {
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_ForceInvenChange(255);
        return;
      }
      if (!this->Emb_DelStorage(
            targetStorage->m_nListCode,
            replaceItem->m_byStorageIndex,
            false,
            false,
            "CPlayer::pc_ForceInvenChange() -- 1"))
      {
        this->Emb_DelStorage(sourceStorage->m_nListCode, replaceItem->m_byStorageIndex, false, false, nullptr);
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_ForceInvenChange(255);
        return;
      }

      _STORAGE_LIST::_db_con replaceForRestore;
      std::memcpy(&replaceForRestore, replaceItem, sizeof(replaceForRestore));
      this->Emb_AddStorage(targetStorage->m_nListCode, &replaceForRestore, true, false);
      this->Emb_DelStorage(sourceStorage->m_nListCode, replaceItem->m_byStorageIndex, false, false, nullptr);
      this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
      this->SendMsg_ForceInvenChange(255);
      return;
    }

    if (!this->Emb_AddStorage(targetStorage->m_nListCode, &sourceBackup, true, false))
    {
      this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
      this->SendMsg_ForceInvenChange(255);
      return;
    }
    this->Emb_EquipLink();
  }
  this->SendMsg_ForceInvenChange(resultCode);
}

void CPlayer::SendMsg_UILock_Init_Result(char resultCode)
{
  _uilock_init_result_zocl packet{};
  packet.byRet = resultCode;
  packet.byUILock_HintIndex = 0;
  unsigned __int8 type[2] = {13, 128};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_UILock_Login_Result(char resultCode, char failCount)
{
  _uilock_user_certify_result_zocl packet{};
  packet.byRet = resultCode;
  packet.byFailCount = failCount;
  unsigned __int8 type[2] = {13, 130};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_UILock_Update_Result(char resultCode)
{
  unsigned __int8 type[2] = {13, 132};
  _uilock_update_info_result_zocl packet{};
  packet.byRet = resultCode;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_UILock_FindPW_Result(char resultCode, const char *password, char failCount)
{
  _uilock_find_pw_result_zocl packet{};
  packet.byRet = resultCode;
  packet.byFindPassFailCount = failCount;
  if (password)
  {
    strcpy_s(packet.uszUILockPW, sizeof(packet.uszUILockPW), password);
  }

  unsigned __int8 type[2] = {13, 135};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CPlayer::SendMsg_UILock_Init_Request_ToAccount(
  unsigned int accountSerial,
  unsigned __int16 userIndex,
  const char *password,
  unsigned __int8 hintIndex,
  const char *hintAnswer)
{
  _uilock_init_request_wrac payload{};
  payload.wUserIndex = userIndex;
  payload.dwAccountSerial = accountSerial;
  strcpy_s(payload.uszUILockPW, sizeof(payload.uszUILockPW), password);
  payload.byHintIndex = static_cast<char>(hintIndex);
  strcpy_s(payload.uszHintAnswer, sizeof(payload.uszHintAnswer), hintAnswer);

  unsigned __int8 type[2] = {1, 15};
  g_Network.m_pProcess[1]->LoadSendMsg(
    0,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::SendMsg_UILock_Update_Request_ToAccount(
  unsigned int accountSerial,
  unsigned __int16 userIndex,
  const char *password,
  unsigned __int8 hintIndex,
  const char *hintAnswer)
{
  _uilock_update_request_wrac payload{};
  payload.wUserIndex = userIndex;
  payload.dwAccountSerial = accountSerial;
  strcpy_s(payload.uszUILockPW, sizeof(payload.uszUILockPW), password);
  payload.byHintIndex = static_cast<char>(hintIndex);
  strcpy_s(payload.uszHintAnswer, sizeof(payload.uszHintAnswer), hintAnswer);

  unsigned __int8 type[2] = {1, 17};
  g_Network.m_pProcess[1]->LoadSendMsg(
    0,
    type,
    reinterpret_cast<char *>(&payload),
    static_cast<unsigned __int16>(sizeof(payload)));
}

void CPlayer::pc_RequestUILockInit(
  CUserDB *pUserDB,
  char *szUILockPW,
  char *szUILockPW_Confirm,
  unsigned __int8 byUILock_HintIndex,
  char *uszUILock_HintAnswer)
{
  if (!pUserDB || pUserDB->m_byUserDgr)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  if (szUILockPW
      && szUILockPW_Confirm
      && uszUILock_HintAnswer
      && std::strlen(szUILockPW)
      && std::strlen(szUILockPW_Confirm)
      && std::strlen(uszUILock_HintAnswer))
  {
    if (std::strlen(szUILockPW) <= 12
        && std::strlen(szUILockPW_Confirm) <= 12
        && std::strlen(uszUILock_HintAnswer) <= 16)
    {
      if (!std::strcmp(szUILockPW, szUILockPW_Confirm))
      {
        if (!std::strcmp(szUILockPW, pUserDB->m_szAccount_PW))
        {
          resultCode = 2;
        }
        else if (m_Param.GetTrunkSlotNum()
                 && !std::strcmp(szUILockPW, m_Param.GetTrunkPasswdW()))
        {
          resultCode = 2;
        }
        else if (IsAlphaString(szUILockPW) || IsDigitString(szUILockPW))
        {
          resultCode = 5;
        }
        else if (IsAlnumMixedString(szUILockPW))
        {
          if (!IsSQLValidString(uszUILock_HintAnswer))
          {
            g_Main.m_logSystemError.Write(
              "CPlayer::pc_CharacterRenameCheck() : Account %u(%s) !::IsSQLValidString( uszUILock_HintAnswer(%s) ) Invalid!",
              pUserDB->m_dwAccountSerial,
              pUserDB->m_szAccountID,
              uszUILock_HintAnswer);
            resultCode = 6;
          }
          else if (pUserDB->m_byUILock)
          {
            resultCode = 11;
          }
        }
        else
        {
          resultCode = 5;
        }
      }
      else
      {
        resultCode = 1;
      }
    }
    else
    {
      resultCode = 6;
    }
  }
  else
  {
    resultCode = 6;
  }

  if (resultCode)
  {
    this->SendMsg_UILock_Init_Result(static_cast<char>(resultCode));
    return;
  }

  this->SendMsg_UILock_Init_Request_ToAccount(pUserDB->m_dwAccountSerial, pUserDB->m_idWorld.wIndex, szUILockPW, byUILock_HintIndex, uszUILock_HintAnswer);
}

void CPlayer::pc_RequestUILockCertify(CUserDB *pUserDB, char *uszUILockPW)
{
  if (!pUserDB || pUserDB->m_byUserDgr)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  if (uszUILockPW && std::strlen(uszUILockPW))
  {
    if (pUserDB->m_byUILock)
    {
      if (pUserDB->m_byUILock == 2)
      {
        resultCode = 2;
      }
      else if (std::strcmp(pUserDB->m_szUILock_PW, uszUILockPW))
      {
        ++pUserDB->m_byUILock_FailCnt;
        resultCode = pUserDB->m_byUILock_FailCnt < 5 ? 1 : 4;
      }
    }
    else
    {
      resultCode = 3;
    }
  }
  else
  {
    resultCode = 5;
  }

  if (resultCode)
  {
    if (resultCode == 4)
    {
      this->SendMsg_UILock_Login_Result(static_cast<char>(resultCode), static_cast<char>(pUserDB->m_byUILock_FailCnt));
      pUserDB->ForceCloseCommand(8u, 0, true, "UILOCK Certify Fail");
    }
    else
    {
      this->SendMsg_UILock_Login_Result(static_cast<char>(resultCode), static_cast<char>(pUserDB->m_byUILock_FailCnt));
    }
    return;
  }

  pUserDB->m_byUILock = 2;
  pUserDB->m_byUILock_FailCnt = 0;
  pUserDB->m_byUILockFindPassFailCount = 0;
  this->SendMsg_UILock_Login_Result(0, 0);
}

void CPlayer::pc_RequestUILockUpdate(
  char *uszUILockPWOld,
  char *uszUILockPW,
  char *uszUILockPW_Confirm,
  unsigned __int8 byUILock_HintIndex,
  char *uszUILock_HintAnswer)
{
  if (!m_pUserDB || m_pUserDB->m_byUserDgr)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  if (uszUILockPW
      && uszUILockPW_Confirm
      && uszUILock_HintAnswer
      && std::strlen(uszUILockPW)
      && std::strlen(uszUILockPW_Confirm)
      && std::strlen(uszUILock_HintAnswer))
  {
    if (std::strlen(uszUILockPW) <= 12
        && std::strlen(uszUILockPW_Confirm) <= 12
        && std::strlen(uszUILock_HintAnswer) <= 16)
    {
      if (m_pUserDB->m_byUILock == 2)
      {
        if (!std::strcmp(uszUILockPWOld, m_pUserDB->m_szUILock_PW))
        {
          if (!std::strcmp(uszUILockPW, m_pUserDB->m_szUILock_PW))
          {
            resultCode = 8;
          }
          else if (!std::strcmp(uszUILockPW, uszUILockPW_Confirm))
          {
            if (!std::strcmp(uszUILockPW, m_pUserDB->m_szAccount_PW))
            {
              resultCode = 2;
            }
            else if (m_Param.GetTrunkSlotNum()
                     && !std::strcmp(uszUILockPW, m_Param.GetTrunkPasswdW()))
            {
              resultCode = 2;
            }
            else if (IsAlphaString(uszUILockPW) || IsDigitString(uszUILockPW))
            {
              resultCode = 5;
            }
            else if (IsAlnumMixedString(uszUILockPW))
            {
              if (!IsSQLValidString(uszUILock_HintAnswer))
              {
                g_Main.m_logSystemError.Write(
                  "CPlayer::pc_RequestUILockUpdate() : Account : %u(%s) Character : %u !::IsSQLValidString( uszUILock_HintAnswer(%s) ) Invalid!",
                  m_pUserDB->m_dwAccountSerial,
                  m_pUserDB->m_szAccountID,
                  m_dwObjSerial,
                  uszUILock_HintAnswer);
                resultCode = 6;
              }
            }
            else
            {
              resultCode = 5;
            }
          }
          else
          {
            resultCode = 1;
          }
        }
        else
        {
          resultCode = 9;
        }
      }
      else
      {
        resultCode = 13;
      }
    }
    else
    {
      resultCode = 6;
    }
  }
  else
  {
    resultCode = 6;
  }

  if (resultCode)
  {
    this->SendMsg_UILock_Update_Result(static_cast<char>(resultCode));
    return;
  }

  this->SendMsg_UILock_Update_Request_ToAccount(m_pUserDB->m_dwAccountSerial, m_pUserDB->m_idWorld.wIndex, uszUILockPW, byUILock_HintIndex, uszUILock_HintAnswer);
}

void CPlayer::pc_RequestUILockFindPW(CUserDB *pUserDB, char *uszHintAnswer)
{
  if (!pUserDB || pUserDB->m_byUserDgr)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  if (uszHintAnswer && std::strlen(uszHintAnswer))
  {
    if (pUserDB->m_byUILock)
    {
      if (pUserDB->m_byUILock == 2)
      {
        resultCode = 3;
      }
      else if (std::strcmp(pUserDB->m_uszUILock_HintAnswer, uszHintAnswer))
      {
        ++pUserDB->m_byUILockFindPassFailCount;
        resultCode = pUserDB->m_byUILockFindPassFailCount < 5 ? 1 : 5;
      }
    }
    else
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 4;
  }

  if (resultCode)
  {
    this->SendMsg_UILock_FindPW_Result(static_cast<char>(resultCode), nullptr, static_cast<char>(pUserDB->m_byUILockFindPassFailCount));
    if (resultCode == 5)
    {
      pUserDB->ForceCloseCommand(9u, 0, true, "UILOCK Find Password Fail");
    }
    return;
  }

  pUserDB->m_byUILockFindPassFailCount = 0;
  this->SendMsg_UILock_FindPW_Result(0, pUserDB->m_szUILock_PW, 0);
}


namespace
{

bool IsAsciiAlpha(char ch)
{
  return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

bool IsAsciiDigit(char ch)
{
  return ch >= '0' && ch <= '9';
}

bool IsAsciiAlnum(char ch)
{
  return IsAsciiAlpha(ch) || IsAsciiDigit(ch);
}

bool IsAlphaString(const char *text)
{
  if (!text || !text[0])
  {
    return false;
  }

  for (const char *cursor = text; *cursor; ++cursor)
  {
    if (!IsAsciiAlpha(*cursor))
    {
      return false;
    }
  }
  return true;
}

bool IsDigitString(const char *text)
{
  if (!text || !text[0])
  {
    return false;
  }

  for (const char *cursor = text; *cursor; ++cursor)
  {
    if (!IsAsciiDigit(*cursor))
    {
      return false;
    }
  }
  return true;
}

bool IsAlnumMixedString(const char *text)
{
  if (!text || !text[0])
  {
    return false;
  }

  bool hasAlpha = false;
  bool hasDigit = false;
  for (const char *cursor = text; *cursor; ++cursor)
  {
    if (!IsAsciiAlnum(*cursor))
    {
      return false;
    }
    hasAlpha = hasAlpha || IsAsciiAlpha(*cursor);
    hasDigit = hasDigit || IsAsciiDigit(*cursor);
  }
  return hasAlpha && hasDigit;
}

} // namespace

