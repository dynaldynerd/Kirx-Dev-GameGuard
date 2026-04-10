#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CCharacter.h"
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
#include "AnimusItem_fld.h"
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
#include "ObjectCreateSetData.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace
{
constexpr int kAnimusRecallConsumeFP = 60;

unsigned __int16 GetPackedLowWord(unsigned int value)
{
  return static_cast<unsigned __int16>(value & 0xFFFFu);
}

unsigned __int16 GetPackedHighWord(unsigned int value)
{
  return static_cast<unsigned __int16>((value >> 16) & 0xFFFFu);
}

void SetPackedLowWord(unsigned int &value, unsigned __int16 lowWord)
{
  value = (value & 0xFFFF0000u) | static_cast<unsigned int>(lowWord);
}

void SetPackedHighWord(unsigned int &value, unsigned __int16 highWord)
{
  value = (value & 0x0000FFFFu) | (static_cast<unsigned int>(highWord) << 16);
}
}

_animus_create_setdata::_animus_create_setdata()
{
  pMaster = nullptr;
  nMaxAttackPnt = 0;
}

CAnimus *FindEmptyAnimus(CAnimus *pObjArray, int nMax)
{
  for (int index = 0; index < nMax; ++index)
  {
    if (!pObjArray[index].m_bLive)
    {
      return &pObjArray[index];
    }
  }
  return nullptr;
}

char __fastcall CreateAnimus(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  unsigned __int8 byClass,
  int nHP,
  int nFP,
  unsigned int dwExp,
  CPlayer *pMaster)
{
  _animus_create_setdata createData;
  createData.m_pMap = pMap;
  createData.m_nLayerIndex = wLayer;
  createData.m_pRecordSet = g_Main.m_tblAnimus.GetRecord(byClass);
  if (!createData.m_pRecordSet)
  {
    return 0;
  }
  std::memcpy(createData.m_fStartPos, fPos, sizeof(createData.m_fStartPos));
  createData.nHP = nHP;
  createData.nFP = nFP;
  createData.dwExp = dwExp;
  createData.pMaster = pMaster;
  CAnimus *slot = FindEmptyAnimus(g_Animus, MAX_ANIMUS);
  if (!slot)
  {
    return 0;
  }
  if (!slot->Create(&createData))
  {
    return 0;
  }
  return 1;
}

bool __fastcall DE_MakeZeroAnimusRecallTimeOnce(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet)
{
  (void)byRet;
  CPlayer *actPlayer = static_cast<CPlayer *>(pActChar);
  return actPlayer->SF_MakeZeroAnimusRecallTimeOnce(pTargetChar, fEffectValue);
}

bool __fastcall DE_RecoverAllReturnStateAnimusHPFull(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet)
{
  (void)byRet;
  CPlayer *actPlayer = static_cast<CPlayer *>(pActChar);
  return actPlayer->SF_RecoverAllReturnStateAnimusHPFull(pTargetChar, fEffectValue);
}

void CPlayer::SendMsg_AnimusHPInform()
{
  if (!this->m_pRecalledAnimusChar || !this->m_pRecalledAnimusItem)
  {
    return;
  }

  _animus_hp_inform_zocl msg{};
  msg.wAnimusItemSerial = this->m_pRecalledAnimusItem->m_wSerial;
  msg.wLeftHP = static_cast<unsigned __int16>(this->m_pRecalledAnimusChar->m_nHP);

  unsigned __int8 type[2] = {22, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_AnimusFPInform()
{
  if (!this->m_pRecalledAnimusChar || !this->m_pRecalledAnimusItem)
  {
    return;
  }

  _animus_fp_inform_zocl msg{};
  msg.wAnimusItemSerial = this->m_pRecalledAnimusItem->m_wSerial;
  msg.wLeftFP = static_cast<unsigned __int16>(this->m_pRecalledAnimusChar->m_nFP);

  unsigned __int8 type[2] = {22, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_AnimusExpInform()
{
  if (!this->m_pRecalledAnimusChar || !this->m_pRecalledAnimusItem)
  {
    return;
  }

  _animus_exp_inform_zocl msg{};
  msg.wAnimusItemSerial = this->m_pRecalledAnimusItem->m_wSerial;
  msg.dwExp = this->m_pRecalledAnimusChar->m_dwExp;

  unsigned __int8 type[2] = {22, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_AnimusModeInform(unsigned __int8 byMode)
{
  _animus_command_inform_zocl msg{};
  msg.byStateCode = byMode;

  unsigned __int8 type[2] = {22, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::Return_AnimusAsk(unsigned __int8 byReturnType)
{
  if (m_pRecalledAnimusItem && m_pRecalledAnimusChar)
  {
    m_byNextRecallReturn = byReturnType;
  }
}

bool CPlayer::IsRecallAnimus()
{
  return m_pRecalledAnimusItem != nullptr || m_pRecalledAnimusChar != nullptr;
}

bool CPlayer::SF_MakeZeroAnimusRecallTimeOnce(CCharacter *pDstObj, float fEffectValue)
{
  (void)fEffectValue;
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  m_bFreeRecallWaitTime = true;
  return true;
}

bool CPlayer::SF_RecoverAllReturnStateAnimusHPFull(CCharacter *pDstObj, float fEffectValue)
{
  (void)fEffectValue;
  if (pDstObj->m_ObjID.m_byID)
  {
    return false;
  }

  CPlayer *targetPlayer = static_cast<CPlayer *>(pDstObj);
  if (!targetPlayer->m_pUserDB)
  {
    return false;
  }

  if (targetPlayer->m_Param.GetRaceCode() != 1)
  {
    return false;
  }

  int changedCount = 0;
  for (int index = 0; index < 4; ++index)
  {
    _STORAGE_LIST::_db_con *animusItem = &targetPlayer->m_Param.m_dbAnimus.m_pStorageList[index];
    if (animusItem->m_bLoad && this->m_pRecalledAnimusItem != animusItem)
    {
      const unsigned int maxParam = GetMaxParamFromExp(animusItem->m_wItemIndex, animusItem->m_dwDur);
      if (maxParam != animusItem->m_dwLv)
      {
        animusItem->m_dwLv = maxParam;
        targetPlayer->m_pUserDB->Update_ItemUpgrade(4u, animusItem->m_byStorageIndex, maxParam, false);
        ++changedCount;
      }
    }
  }

  return changedCount > 0;
}

void CPlayer::AlterHP_Animus(unsigned __int16 nNewHP)
{
  if (m_pRecalledAnimusItem)
  {
    unsigned int *packedLevel = &m_pRecalledAnimusItem->m_dwLv;
    *reinterpret_cast<unsigned __int16 *>(packedLevel) = nNewHP;
    SendMsg_AnimusHPInform();
    if (m_pUserDB)
    {
      _STORAGE_LIST::_db_con *recalledItem = m_pRecalledAnimusItem;
      m_pUserDB->Update_ItemUpgrade(4u, recalledItem->m_byStorageIndex, recalledItem->m_dwLv, false);
    }
  }
}

void CPlayer::AlterFP_Animus(unsigned __int16 nNewFP)
{
  if (m_pRecalledAnimusItem)
  {
    unsigned int *packedLevel = &m_pRecalledAnimusItem->m_dwLv;
    *(reinterpret_cast<unsigned __int16 *>(packedLevel) + 1) = nNewFP;
    SendMsg_AnimusFPInform();
    if (m_pUserDB)
    {
      _STORAGE_LIST::_db_con *recalledItem = m_pRecalledAnimusItem;
      m_pUserDB->Update_ItemUpgrade(4u, recalledItem->m_byStorageIndex, recalledItem->m_dwLv, false);
    }
  }
}

void CPlayer::AlterExp_Animus(__int64 nAlterExp)
{
  if (!m_pRecalledAnimusItem)
  {
    return;
  }

  if (nAlterExp <= 0)
  {
    const unsigned __int64 oldExp = m_pRecalledAnimusItem->m_dwDur;
    if (m_pRecalledAnimusItem->m_dwDur < static_cast<unsigned __int64>(-nAlterExp))
    {
      m_pRecalledAnimusItem->m_dwDur = 0;
      g_Main.m_logSystemError.Write(
        "CPlayer::AlterExp_Animus( __int64 nAlterExp ) : m_pRecalledAnimusItem->m_dwDur(%I64u) < -nAlterExp(%I64d) Invalid!",
        m_pRecalledAnimusItem->m_dwDur,
        -nAlterExp);
    }
    else
    {
      m_pRecalledAnimusItem->m_dwDur += nAlterExp;
    }

    s_MgrLvHistory.down_animus_exp(
      oldExp,
      m_pRecalledAnimusItem->m_dwDur,
      nAlterExp,
      m_szLvHistoryFileName);
  }
  else
  {
    m_pRecalledAnimusItem->m_dwDur += nAlterExp;
    Emb_AlterStat(6u, 0, static_cast<int>(nAlterExp), 0, "CPlayer::AlterExp_Animus()---0", true);
  }

  SendMsg_AnimusExpInform();
  if (m_pUserDB)
  {
    _STORAGE_LIST::_db_con *recalledItem = m_pRecalledAnimusItem;
    m_pUserDB->Update_ItemDur(4u, recalledItem->m_byStorageIndex, recalledItem->m_dwDur, false);
  }
}

void CPlayer::AlterMode_Animus(unsigned __int8 byMode)
{
  if (m_pRecalledAnimusItem)
  {
    SendMsg_AnimusModeInform(byMode);
  }
}

void CPlayer::SendMsg_AnimusRecallWaitTimeFree(bool bFree)
{
  _animus_recall_wait_time_free_inform_zocl msg{};
  msg.bFree = bFree;

  unsigned __int8 type[2] = {22, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_AnimusTargetResult(char byRetCode)
{
  _animus_target_result_zocl msg{};
  msg.byResultCode = byRetCode;

  unsigned __int8 type[2] = {22, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::SendMsg_AnimusRecallResult(unsigned __int8 byResultCode, unsigned __int16 wLeftFP, CAnimus *pNewAnimus)
{
  _animus_recall_result_zocl packet{};
  packet.byResultCode = byResultCode;
  packet.wLeftFP = wLeftFP;
  if (byResultCode == 0 && pNewAnimus)
  {
    packet.dwAnimusSerial = pNewAnimus->m_dwObjSerial;
    packet.wAnimusHP = static_cast<unsigned __int16>(pNewAnimus->m_nHP);
    packet.wAnimusFP = static_cast<unsigned __int16>(pNewAnimus->m_nFP);
    packet.dwAnimusExp = pNewAnimus->m_dwExp;
  }

  unsigned __int8 type[2] = {22, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_AnimusInvenChange(unsigned __int8 byErrCode)
{
  _animus_inven_change_result_zocl msg{};
  msg.byErrCode = byErrCode;

  unsigned __int8 pbyType[2] = {13, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CPlayer::pc_AnimusInvenChange(_STORAGE_POS_INDIV *pItem, unsigned __int16 wReplaceSerial)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *sourceItem = nullptr;
  _STORAGE_LIST::_db_con *replaceItem = nullptr;
  _base_fld *record = nullptr;

  _STORAGE_LIST *sourceStorage = this->m_Param.m_pStoragePtr[pItem->byStorageCode];
  _STORAGE_LIST *targetStorage = nullptr;
  if (!pItem->byStorageCode)
  {
    targetStorage = this->m_Param.m_pStoragePtr[4];
  }
  else if (pItem->byStorageCode == 4)
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
    if (sourceItem->m_byTableCode == 24)
    {
      if (sourceItem->m_bLock)
      {
        resultCode = 11;
      }
      else if (sourceItem == this->m_pRecalledAnimusItem)
      {
        resultCode = 6;
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
          if (!replaceItem || replaceItem->m_byTableCode != 24)
          {
            resultCode = 2;
          }
          else if (replaceItem->m_bLock)
          {
            resultCode = 11;
          }
          else if (replaceItem->m_wItemIndex != sourceItem->m_wItemIndex)
          {
            resultCode = 3;
          }
          else if (replaceItem == this->m_pRecalledAnimusItem)
          {
            resultCode = 6;
          }
        }
        if (!resultCode && wReplaceSerial == 0xFFFF && targetStorage->GetIndexEmptyCon() == 255)
        {
          resultCode = 5;
        }
        else if (!resultCode && wReplaceSerial == 0xFFFF && !pItem->byStorageCode)
        {
          record = g_Main.m_tblItemData[24].GetRecord(sourceItem->m_wItemIndex);
          if (!record)
          {
            resultCode = 8;
          }
          else
          {
            bool canDuplicate = false;
            if (reinterpret_cast<_AnimusItem_fld *>(record)->m_nAnimusType == 1)
            {
              if (this->m_Param.m_pClassHistory[0] && this->m_Param.m_pClassHistory[0]->m_nClass == 3)
              {
                canDuplicate = this->m_Param.m_pClassData->m_nClass == 3;
              }
            }
            else
            {
              for (int index = 0; index < 4; ++index)
              {
                _class_fld *historyClass = *this->m_Param.m_ppHistoryEffect[index];
                if (!historyClass)
                {
                  break;
                }
                if (historyClass->m_bAnimusUsable)
                {
                  canDuplicate = true;
                  break;
                }
              }
            }

            if (this->m_bFreeSFByClass)
            {
              canDuplicate = true;
            }

            if (!canDuplicate)
            {
              resultCode = 7;
            }

            for (int index = 0; !resultCode && index < targetStorage->m_nUsedNum; ++index)
            {
              _STORAGE_LIST::_db_con *item = &targetStorage->m_pStorageList[index];
              if (item->m_bLoad && item->m_wItemIndex == sourceItem->m_wItemIndex)
              {
                resultCode = 4;
                break;
              }
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
          "CPlayer::pc_AnimusInvenChange() -- 0"))
    {
      this->SendMsg_AnimusInvenChange(0xFF);
      return;
    }

    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceBackup;
      std::memcpy(&replaceBackup, replaceItem, sizeof(replaceBackup));
      if (!this->Emb_AddStorage(sourceStorage->m_nListCode, &replaceBackup, true, false))
      {
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_AnimusInvenChange(0xFF);
        return;
      }
      if (!this->Emb_DelStorage(
            targetStorage->m_nListCode,
            replaceItem->m_byStorageIndex,
            false,
            false,
            "CPlayer::pc_AnimusInvenChange() -- 1"))
      {
        this->Emb_DelStorage(
          sourceStorage->m_nListCode,
          replaceItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_AnimusInvenChange() -- 1 Fail");
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_AnimusInvenChange(0xFF);
        return;
      }
    }

    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceForRestore;
      std::memcpy(&replaceForRestore, replaceItem, sizeof(replaceForRestore));
      if (!this->Emb_AddStorage(targetStorage->m_nListCode, &sourceBackup, true, false))
      {
        this->Emb_AddStorage(targetStorage->m_nListCode, &replaceForRestore, true, false);
        this->Emb_DelStorage(
          sourceStorage->m_nListCode,
          replaceItem->m_byStorageIndex,
          false,
          false,
          "CPlayer::pc_AnimusInvenChange() -- 1 Fail");
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_AnimusInvenChange(0xFF);
        return;
      }
    }
    else if (!this->Emb_AddStorage(targetStorage->m_nListCode, &sourceBackup, true, false))
    {
      this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
      this->SendMsg_AnimusInvenChange(0xFF);
      return;
    }
    this->Emb_EquipLink();
  }
  this->SendMsg_AnimusInvenChange(resultCode);
}

void CPlayer::pc_AnimusRecallRequest(
  unsigned __int16 wAnimusItemSerial,
  unsigned __int16 wAnimusClientHP,
  unsigned __int16 wAnimusClientFP)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *animusItem = nullptr;
  _animus_fld *animusField = nullptr;
  CAnimus *newAnimus = nullptr;
  _base_fld *animusItemRecord = nullptr;
  unsigned __int16 adjustedHP = 0;
  unsigned __int16 adjustedFP = 0;

  if (this->m_bObserver)
  {
    resultCode = 13;
  }
  else if (this->m_bInGuildBattle && this->m_bTakeGravityStone)
  {
    resultCode = 14;
  }
  else if (this->m_Param.GetRaceCode() != 1)
  {
    resultCode = 10;
  }
  else if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    resultCode = 6;
  }
  else if (this->m_pRecalledAnimusItem)
  {
    resultCode = 1;
  }
  else
  {
    for (int slot = 0; slot < 4; ++slot)
    {
      _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbAnimus.m_pStorageList[slot];
      if (item->m_bLoad && item->m_wSerial == wAnimusItemSerial)
      {
        animusItem = item;
        break;
      }
    }

    if (!animusItem)
    {
      resultCode = 2;
    }
    else if (animusItem->m_bLock)
    {
      resultCode = 13;
    }
    else
    {
      animusItemRecord = g_Main.m_tblItemData[24].GetRecord(animusItem->m_wItemIndex);
      if (!animusItemRecord)
      {
        resultCode = 8;
      }
      else
      {
        bool canRecallAnimus = false;
        if (this->m_bFreeSFByClass)
        {
          canRecallAnimus = true;
        }
        else if (reinterpret_cast<_AnimusItem_fld *>(animusItemRecord)->m_nAnimusType == 1)
        {
          if (this->m_Param.m_pClassHistory[0]
              && this->m_Param.m_pClassHistory[0]->m_nClass == 3
              && this->m_Param.m_pClassData->m_nClass == 3)
          {
            canRecallAnimus = this->m_Param.m_pClassData->m_bAnimusUsable != 0;
          }
        }
        else
        {
          for (int historyIndex = 0; historyIndex < 4; ++historyIndex)
          {
            _class_fld *historyClass = *this->m_Param.m_ppHistoryEffect[historyIndex];
            if (!historyClass)
            {
              break;
            }
            if (historyClass->m_bAnimusUsable)
            {
              canRecallAnimus = true;
              break;
            }
          }
        }

        if (!canRecallAnimus)
        {
          resultCode = 12;
        }
        else
        {
          if (this->m_bFreeRecallWaitTime)
          {
            this->m_bFreeRecallWaitTime = false;
          }
          else if (this->m_wTimeFreeRecallSerial != animusItem->m_wSerial)
          {
            const unsigned int elapsed = GetLoopTime() - this->m_dwLastRecallTime;
            if (elapsed < 30000u)
            {
              resultCode = 9;
            }
          }

          if (!resultCode)
          {
            animusField = GetAnimusFldFromExp(animusItem->m_wItemIndex, animusItem->m_dwDur);
            if (!animusField)
            {
              return;
            }

            adjustedHP = GetPackedLowWord(animusItem->m_dwLv);
            adjustedFP = GetPackedHighWord(animusItem->m_dwLv);

            if (this->m_wTimeFreeRecallSerial != animusItem->m_wSerial)
            {
              if (std::abs(static_cast<int>(adjustedHP) - static_cast<int>(wAnimusClientHP)) <= 4)
              {
                adjustedHP = wAnimusClientHP;
              }
              if (std::abs(static_cast<int>(adjustedFP) - static_cast<int>(wAnimusClientFP)) <= 4)
              {
                adjustedFP = wAnimusClientFP;
              }

              const float hpRatio =
                static_cast<float>(adjustedHP) / static_cast<float>(animusField->m_nMaxHP);
              if (hpRatio < 0.30000001f)
              {
                resultCode = 3;
              }
            }

            if (!resultCode && this->m_wTimeFreeRecallSerial != animusItem->m_wSerial)
            {
              const int mastery = static_cast<int>(this->m_pmMst.GetMasteryPerMast(6u, 0));
              if (animusField->m_nLevel < 50)
              {
                const double requiredMastery =
                  std::pow(static_cast<double>(animusField->m_nLevel - 5), 2.0) / 20.0;
                if (mastery < static_cast<int>(requiredMastery))
                {
                  resultCode = 11;
                }
              }
              else
              {
                if (mastery < 99)
                {
                  resultCode = 11;
                }
                else if (animusField->m_nLevel > 50)
                {
                  const int playerLevel = static_cast<int>(this->GetLevel());
                  if (animusField->m_nLevel > playerLevel + 1)
                  {
                    resultCode = 11;
                  }
                }
              }

              if (!resultCode && this->GetFP() < kAnimusRecallConsumeFP)
              {
                resultCode = 4;
              }
            }

            if (!resultCode)
            {
              newAnimus = FindEmptyAnimus(g_Animus, MAX_ANIMUS);
              if (!newAnimus)
              {
                resultCode = 5;
              }
            }
          }
        }
      }
    }
  }

  if (!resultCode)
  {
    if (this->m_wTimeFreeRecallSerial != animusItem->m_wSerial)
    {
      const int fpAfterConsume = this->GetFP() - kAnimusRecallConsumeFP;
      this->SetFP(fpAfterConsume, true);
    }

    animusItem->m_dwLv =
      static_cast<unsigned int>(adjustedHP)
      | (static_cast<unsigned int>(adjustedFP) << 16);

    _animus_create_setdata createData;
    createData.m_pMap = this->m_pCurMap;
    createData.m_nLayerIndex = this->m_wMapLayerIndex;
    createData.m_pRecordSet = g_Main.m_tblAnimus.GetRecord(animusItem->m_wItemIndex);
    std::memcpy(createData.m_fStartPos, this->m_fCurPos, sizeof(createData.m_fStartPos));
    createData.nHP = adjustedHP;
    createData.nFP = adjustedFP;
    createData.dwExp = animusItem->m_dwDur;
    createData.pMaster = this;
    createData.nMaxAttackPnt = this->m_nAnimusAttackPnt;
    newAnimus->Create(&createData);
    this->m_pRecalledAnimusItem = animusItem;
    this->m_pRecalledAnimusChar = newAnimus;
    this->m_wTimeFreeRecallSerial = static_cast<unsigned __int16>(-1);
    animusItem->lock(true);
  }

  const unsigned __int16 leftFP = static_cast<unsigned __int16>(this->GetFP());
  this->SendMsg_AnimusRecallResult(resultCode, leftFP, newAnimus);
}

void CPlayer::pc_AnimusReturnRequest()
{
  unsigned __int8 ret = 0;
  if (!this->m_pRecalledAnimusItem || !this->m_pRecalledAnimusChar)
  {
    ret = 7;
  }

  if (ret)
  {
    this->SendMsg_AnimusReturnResult(ret, 65535, 0);
  }
  else
  {
    this->_AnimusReturn(0);
  }
}

void CPlayer::pc_AnimusCommandRequest(unsigned __int8 byCommandCode)
{
  unsigned __int8 result = 0;
  if (!this->m_pRecalledAnimusItem || !this->m_pRecalledAnimusChar)
  {
    result = 7;
  }

  if (!result)
  {
    this->m_pRecalledAnimusChar->ChangeMode_MasterCommand(byCommandCode);
  }
}

void CPlayer::pc_AnimusTargetRequest(
  unsigned __int8 byObjectID,
  unsigned __int16 wObjectIndex,
  unsigned int dwObjectSerial)
{
  unsigned __int8 result = 0;
  CCharacter *target = reinterpret_cast<CCharacter *>(g_Main.GetObjectA(0, byObjectID, wObjectIndex));
  if (this->m_pRecalledAnimusItem && this->m_pRecalledAnimusChar)
  {
    if (target->m_bLive && !target->m_bCorpse)
    {
      if (GetSqrt(this->m_pRecalledAnimusChar->m_fCurPos, target->m_fCurPos) > 400.0f)
      {
        result = 8;
      }
    }
    else
    {
      result = 8;
    }
  }
  else
  {
    result = 7;
  }

  if (!result && !this->m_pRecalledAnimusChar->ChangeTarget_MasterCommand(target))
  {
    result = 8;
  }
  this->SendMsg_AnimusTargetResult(static_cast<char>(result));
}

void CPlayer::AutoRecover_Animus()
{
  for (int animusIndex = 0; animusIndex < 4; ++animusIndex)
  {
    _STORAGE_LIST::_db_con *animus = &m_Param.m_dbAnimus.m_pStorageList[animusIndex];
    if (!animus->m_bLoad || animus == m_pRecalledAnimusItem)
    {
      continue;
    }

    _animus_fld *animusFld = GetAnimusFldFromExp(animus->m_wItemIndex, animus->m_dwDur);
    if (!animusFld)
    {
      continue;
    }

    const unsigned int previousLv = animus->m_dwLv;
    unsigned int packedStatus = animus->m_dwLv;
    unsigned __int16 currentHP = GetPackedLowWord(packedStatus);
    unsigned __int16 currentFP = GetPackedHighWord(packedStatus);
    if (currentHP < animusFld->m_nMaxHP)
    {
      currentHP = static_cast<unsigned __int16>(
        currentHP + static_cast<unsigned __int16>(static_cast<unsigned int>(animusFld->m_nHPRecUnit) & 0xFFFFu));
      if (currentHP > animusFld->m_nMaxHP)
      {
        currentHP = static_cast<unsigned __int16>(animusFld->m_nMaxHP);
      }
      SetPackedLowWord(packedStatus, currentHP);
    }

    if (currentFP < animusFld->m_nMaxFP)
    {
      currentFP = static_cast<unsigned __int16>(
        currentFP + static_cast<unsigned __int16>(static_cast<unsigned int>(animusFld->m_nFPRecUnit) & 0xFFFFu));
      if (currentFP > animusFld->m_nMaxFP)
      {
        currentFP = static_cast<unsigned __int16>(animusFld->m_nMaxFP);
      }
      SetPackedHighWord(packedStatus, currentFP);
    }

    if (previousLv != packedStatus && m_pUserDB)
    {
      animus->m_dwLv = packedStatus;
      m_pUserDB->Update_ItemUpgrade(4u, static_cast<unsigned __int8>(animusIndex), animus->m_dwLv, false);
    }
    else
    {
      animus->m_dwLv = packedStatus;
    }
  }
}

