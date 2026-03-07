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
#include "ItemCombine_link_fld.h"
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
unsigned int CombineExCheckKeyGen(unsigned int dwtimeGetTime, unsigned int dwCombineExcelRecordIndex)
{
  return (dwCombineExcelRecordIndex << 16) | ((dwtimeGetTime << 8) >> 16);
}

unsigned int GetCheckTimeFromCombineExCheckKey(unsigned int dwCombineExCheckKey)
{
  return (dwCombineExCheckKey << 8) & 0xFFFFFF;
}

bool CheckSameItem(
  const char *linkItemCode,
  const char *dstItemCode,
  unsigned __int8 *selectedLinkIndex,
  bool checkStuffTable)
{
if (!linkItemCode || !dstItemCode)
  {
    return false;
  }

  if (strncmp(linkItemCode, "LL", 2u) != 0 && strncmp(linkItemCode, "LR", 2u) != 0)
  {
    return strncmp(linkItemCode, dstItemCode, 7u) == 0;
  }

  CRecordData *linkTable = nullptr;
  if (strncmp(linkItemCode, "LL", 2u) == 0)
  {
    linkTable = &ItemCombineMgr::ms_tbl_ItemCombine_Link_Stuff;
  }
  else
  {
    linkTable = &ItemCombineMgr::ms_tbl_ItemCombine_Link_Result;
  }

  const auto *linkRecord = reinterpret_cast<const _ItemCombine_link_fld *>(linkTable->GetRecord(linkItemCode));
  if (!linkRecord)
  {
    return false;
  }

  for (unsigned __int8 index = 0; index < 100; ++index)
  {
    const char *entryCode = linkRecord->m_list[index].m_itmPdCode;
    if (strncmp(entryCode, "-", 1u) == 0 || strncmp(entryCode, "-1", 2u) == 0)
    {
      return false;
    }
    if (strncmp(entryCode, dstItemCode, 7u) == 0)
    {
      if (selectedLinkIndex)
      {
        *selectedLinkIndex = index;
      }
      return true;
    }
  }

  return false;
}

unsigned __int8 MakeCombineItem(
  CPlayer *master,
  _combine_ex_item_result_zocl::__item *item,
  _ItemCombine_exp_fld *combineRecord,
  int outputIndex,
  unsigned __int8 linkTableIndex)
{
  if (!master || !item || !combineRecord || outputIndex < 0 || outputIndex >= 24)
  {
    return 12;
  }

  const _ItemCombine_exp_fld::_output_list &output = combineRecord->m_listOutput[outputIndex];
  const char *outputCode = output.m_itmPdOutput;

  if (strncmp(outputCode, "LR", 2u) == 0)
  {
    const auto *linkRecord = reinterpret_cast<const _ItemCombine_link_fld *>(
      ItemCombineMgr::ms_tbl_ItemCombine_Link_Result.GetRecord(outputCode));
    if (!linkRecord || linkTableIndex >= 100u)
    {
      return 12;
    }
    outputCode = linkRecord->m_list[linkTableIndex].m_itmPdCode;
  }

  const int tableCode = GetItemTableCode(outputCode);
  if (tableCode < 0 || tableCode >= 0x25)
  {
    return 12;
  }

  _base_fld *recordByHash = g_Main.m_tblItemData[tableCode].GetRecordByHash(outputCode, 2, 5);
  if (!recordByHash)
  {
    return 12;
  }

  item->Key.byTableCode = static_cast<unsigned __int8>(tableCode);
  item->Key.wItemIndex = static_cast<unsigned __int16>(recordByHash->m_dwIndex);
  item->Key.byRewardIndex = static_cast<unsigned __int8>(outputIndex);
  item->dwDur = static_cast<unsigned int>(output.m_nOutNum);

  const unsigned __int8 itemKindCode = GetItemKindCode(tableCode);
  if (itemKindCode == 1)
  {
    item->dwDur = output.m_dwUpt;
    item->dwUpt = GetMaxParamFromExp(recordByHash->m_dwIndex, output.m_dwUpt);
  }
  else if (!itemKindCode && output.m_dwUpt == 0xFFFFFFFFu)
  {
    const unsigned __int8 defaultSocketNum = GetDefItemUpgSocketNum(tableCode, recordByHash->m_dwIndex);
    const int socketNum = defaultSocketNum ? (rand() % defaultSocketNum) + 1 : 0;
    item->dwUpt = GetBitAfterSetLimSocket(static_cast<unsigned __int8>(socketNum));
  }
  else
  {
    item->dwUpt = output.m_dwUpt;
  }

  return 0;
}

bool DestroyCombineItem(
  CPlayer *master,
  _combine_ex_item_result_zocl::__item *item,
  _STORAGE_LIST::_db_con *savedItem,
  _combine_ex_item_request_clzo::_list *materialRequest,
  int socketIndex)
{
if (!item || !savedItem || !materialRequest)
  {
    return false;
  }

  item->Key.byTableCode = savedItem->m_byTableCode;
  item->Key.wItemIndex = savedItem->m_wItemIndex;
  item->Key.byRewardIndex = 0;
  item->dwDur = materialRequest->byAmount;
  item->dwUpt = savedItem->m_dwLv;
  return true;
}

void TakeOutLotto(int *buffer, unsigned int count)
{
  if (!buffer || !count)
  {
    return;
  }

  std::memset(buffer, 0xFF, sizeof(int) * count);
  int nextValue = 0;
  int randomSeed = rand();
  for (unsigned int draw = 0; draw < count; ++draw)
  {
    const int remain = static_cast<int>(count) - nextValue;
    if (remain <= 0)
    {
      break;
    }

    const int pick = randomSeed % remain;
    int emptyIndex = 0;
    for (unsigned int slot = 0; slot < count; ++slot)
    {
      if (buffer[slot] == -1)
      {
        if (emptyIndex >= pick)
        {
          buffer[slot] = nextValue++;
          break;
        }
        ++emptyIndex;
      }
    }
  }
}

bool BuildFailItemShortBuffer(
  int *outputBuffer,
  unsigned __int8 materialSlotCount,
  _ItemCombine_exp_fld::_material *materialList,
  _STORAGE_LIST::_db_con **inventoryMaterials)
{
  if (!outputBuffer || !materialList || !inventoryMaterials || materialSlotCount > 5u)
  {
    return false;
  }

  int matchedCount = 0;
  for (unsigned __int8 materialIndex = 0; materialIndex < materialSlotCount; ++materialIndex)
  {
    bool found = false;
    for (unsigned __int8 slot = 0; slot < materialSlotCount; ++slot)
    {
      _STORAGE_LIST::_db_con *savedItem = inventoryMaterials[slot];
      if (!savedItem)
      {
        return false;
      }

      _base_fld *savedRecord = g_Main.m_tblItemData[savedItem->m_byTableCode].GetRecord(savedItem->m_wItemIndex);
      if (!savedRecord)
      {
        continue;
      }

      unsigned __int8 linkIndex = 0;
      if (CheckSameItem(materialList[materialIndex].m_itmPdMat, savedRecord->m_strCode, &linkIndex, true))
      {
        outputBuffer[materialIndex] = slot;
        ++matchedCount;
        found = true;
        break;
      }
    }

    if (!found)
    {
      return false;
    }
  }

  if (matchedCount != materialSlotCount)
  {
    return false;
  }

  int tempBuffer[8] = {};
  for (int index = 0; index < matchedCount; ++index)
  {
    tempBuffer[index] = outputBuffer[index];
  }
  for (int index = 0; index < matchedCount; ++index)
  {
    outputBuffer[index] = tempBuffer[matchedCount - index - 1];
  }

  return true;
}
} // namespace

ItemCombineMgr::ItemCombineMgr()
{
  // this is not a stub
}

void ItemCombineMgr::InitMgr(CPlayer *pOne)
{
  this->m_pMaster = pOne;
}

void ItemCombineMgr::OnPlayerCreateCompleteProc()
{
  _combine_ex_item_result_zocl loadData{};
  if (!LoadDB_CombineResult(&loadData))
  {
    m_pMaster->SendMsg_CombineItemExResult(&loadData);
  }
}

unsigned __int8 ItemCombineMgr::LoadDB_CombineResult(_combine_ex_item_result_zocl *pLoadData)
{
  unsigned __int8 resultCode = 0;
  if (!pLoadData)
  {
    return 13;
  }

  _ITEMCOMBINE_DB_BASE *combineDb = &m_pMaster->m_Param.m_ItemCombineDB;
  if (!combineDb->IsCombineData())
  {
    pLoadData->byErrCode = 14;
    return 14;
  }

  pLoadData->dwCheckKey = combineDb->m_dwCheckKey;
  pLoadData->byDlgType = combineDb->m_byDlgType;
  pLoadData->dwDalant = combineDb->m_dwDalant;
  pLoadData->bySelectItemCount = combineDb->m_bySelectItemCount;
  pLoadData->ItemBuff.Init();
  pLoadData->ItemBuff.byItemListNum = combineDb->m_byItemListNum;

  const int itemCount = std::min<int>(combineDb->m_byItemListNum, 24);
  for (int index = 0; index < itemCount; ++index)
  {
    pLoadData->ItemBuff.RewardItemList[index].Key = combineDb->m_List[index].Key;
    pLoadData->ItemBuff.RewardItemList[index].dwDur = combineDb->m_List[index].dwDur;
    pLoadData->ItemBuff.RewardItemList[index].dwUpt = combineDb->m_List[index].dwUpt;
  }

  pLoadData->byErrCode = resultCode;
  pLoadData->dwResultEffectType = combineDb->m_dwResultEffectType;
  pLoadData->dwResultEffectMsgCode = combineDb->m_dwResultEffectMsgCode;
  return resultCode;
}

char ItemCombineMgr::ClearDB_CombineResult()
{
  m_pMaster->m_Param.m_ItemCombineDB.Init();
  if (m_pMaster->m_pUserDB)
  {
    m_pMaster->m_pUserDB->Update_CombineExResult_Pop();
  }
  return 0;
}

char ItemCombineMgr::UpdateDB_CombineResult(_combine_ex_item_result_zocl *pSaveData)
{
  m_pMaster->m_Param.m_ItemCombineDB.Init();
  _ITEMCOMBINE_DB_BASE *combineDb = &m_pMaster->m_Param.m_ItemCombineDB;

  combineDb->m_dwCheckKey = pSaveData->dwCheckKey;
  combineDb->m_byDlgType = pSaveData->byDlgType;
  combineDb->m_dwDalant = pSaveData->dwDalant;
  combineDb->m_bySelectItemCount = pSaveData->bySelectItemCount;
  combineDb->m_byItemListNum = pSaveData->ItemBuff.byItemListNum;

  const int itemCount = std::min<int>(combineDb->m_byItemListNum, 24);
  for (int index = 0; index < itemCount; ++index)
  {
    combineDb->m_List[index].Key = pSaveData->ItemBuff.RewardItemList[index].Key;
    combineDb->m_List[index].dwDur = pSaveData->ItemBuff.RewardItemList[index].dwDur;
    combineDb->m_List[index].dwUpt = pSaveData->ItemBuff.RewardItemList[index].dwUpt;
  }

  combineDb->m_bIsResult = true;
  combineDb->m_dwResultEffectType = pSaveData->dwResultEffectType;
  combineDb->m_dwResultEffectMsgCode = pSaveData->dwResultEffectMsgCode;

  if (m_pMaster->m_pUserDB)
  {
    m_pMaster->m_pUserDB->Update_CombineExResult_Push(combineDb);
  }
  return 0;
}

unsigned __int8 ItemCombineMgr::RequestCombineProcess(
  _combine_ex_item_request_clzo *pRecv,
  _combine_ex_item_result_zocl *pSend)
{
  unsigned __int8 resultCode = 0;
  unsigned __int8 selectedLinkIndex = 0;

  const int combineManualIndex = static_cast<int>(pRecv->wManualIndex);
  const unsigned __int8 materialSlotCount =
    pRecv->byCombineSlotNum >= 5u ? 5u : pRecv->byCombineSlotNum;
  _combine_ex_item_request_clzo::_list *materialRequest = pRecv->iCombineSlot;

  _ItemCombine_exp_fld *combineRecord =
    reinterpret_cast<_ItemCombine_exp_fld *>(ms_tbl_ItemCombine.GetRecord(combineManualIndex));
  _STORAGE_LIST::_db_con *materialStorage[6] = {};

  if (m_pMaster->m_Param.m_ItemCombineDB.IsCombineData())
  {
    const unsigned int nowMasked = timeGetTime() & 0xFFFFFFu;
    const unsigned int checkTime =
      GetCheckTimeFromCombineExCheckKey(m_pMaster->m_Param.m_ItemCombineDB.m_dwCheckKey);
    const int elapsed = static_cast<int>(nowMasked) - static_cast<int>(checkTime);
    if (std::abs(elapsed) < 0x5265C00)
    {
      resultCode = 17;
    }
  }
  else if (combineRecord && combineRecord->m_bCombineExist)
  {
    const int raceSexCode = m_pMaster->m_Param.GetRaceSexCode();
    if (combineRecord->m_strCivil[raceSexCode] == '1')
    {
      if (combineRecord->m_dwCommit <= m_pMaster->m_Param.GetDalant())
      {
        _ItemCombine_exp_fld::_material requiredMaterial[5]{};
        std::memcpy(requiredMaterial, combineRecord->m_Material, sizeof(requiredMaterial));

        for (int slotIndex = 0; slotIndex < materialSlotCount; ++slotIndex)
        {
          materialStorage[slotIndex] = m_pMaster->m_Param.m_dbInven.GetPtrFromSerial(materialRequest[slotIndex].wItemSerial);
          if (!materialStorage[slotIndex])
          {
            m_pMaster->SendMsg_AdjustAmountInform(0, materialRequest[slotIndex].wItemSerial, 0);
            resultCode = 4;
            break;
          }
          if (materialStorage[slotIndex]->m_bLock)
          {
            resultCode = 5;
            break;
          }
          if (IsOverLapItem(materialStorage[slotIndex]->m_byTableCode)
              && materialRequest[slotIndex].byAmount > materialStorage[slotIndex]->m_dwDur)
          {
            m_pMaster->SendMsg_AdjustAmountInform(
              0,
              materialRequest[slotIndex].wItemSerial,
              static_cast<unsigned int>(materialStorage[slotIndex]->m_dwDur));
            resultCode = 6;
            break;
          }

          for (int duplicateCheck = 0; duplicateCheck < slotIndex; ++duplicateCheck)
          {
            if (materialRequest[duplicateCheck].wItemSerial == materialRequest[slotIndex].wItemSerial)
            {
              resultCode = 6;
              break;
            }
          }
          if (resultCode)
          {
            break;
          }

          _base_fld *materialRecord = nullptr;
          if (materialStorage[slotIndex]->m_byTableCode < 0x25u)
          {
            materialRecord = g_Main.m_tblItemData[materialStorage[slotIndex]->m_byTableCode]
              .GetRecord(materialStorage[slotIndex]->m_wItemIndex);
          }
          if (!materialRecord)
          {
            resultCode = 1;
            break;
          }

          bool matched = false;
          for (int materialIndex = 0; materialIndex < 5; ++materialIndex)
          {
            _ItemCombine_exp_fld::_material *materialInfo = &requiredMaterial[materialIndex];
            if (CheckSameItem(materialInfo->m_itmPdMat, materialRecord->m_strCode, &selectedLinkIndex, true)
                && materialInfo->m_nDur > 0)
            {
              materialInfo->m_nDur -= materialRequest[slotIndex].byAmount;
              if (materialInfo->m_dwUpt != static_cast<unsigned int>(-1)
                  && materialInfo->m_dwUpt == materialStorage[slotIndex]->m_dwLv)
              {
                materialInfo->m_dwUpt = static_cast<unsigned int>(-1);
              }
              matched = true;
              break;
            }
          }

          if (!matched)
          {
            resultCode = 8;
            break;
          }
        }

        if (!resultCode)
        {
          for (int materialIndex = 0; materialIndex < 5; ++materialIndex)
          {
            const _ItemCombine_exp_fld::_material *materialInfo = &requiredMaterial[materialIndex];
            if (materialInfo->m_nDur > 0
                || materialInfo->m_dwUpt != static_cast<unsigned int>(-1))
            {
              resultCode = 9;
              break;
            }
          }
        }
      }
      else
      {
        resultCode = 3;
      }
    }
    else
    {
      resultCode = 2;
    }
  }
  else
  {
    resultCode = 1;
  }

  pSend->byErrCode = resultCode;
  if (resultCode)
  {
    pSend->byDlgType = static_cast<unsigned __int8>(-1);
    return resultCode;
  }

  unsigned __int8 updated = ConsumeMeterial_And_CalculateNewItems(
    materialStorage,
    materialSlotCount,
    materialRequest,
    pSend,
    combineRecord,
    selectedLinkIndex,
    1);

  if (!updated)
  {
    updated = UpdateDB_CombineResult(pSend);
    if (updated)
    {
      pSend->byDlgType = static_cast<unsigned __int8>(-1);
      pSend->byErrCode = updated;
      return updated;
    }
    return resultCode;
  }

  if (updated == 10)
  {
    pSend->byDlgType = 2;
    pSend->byErrCode = 0;
  }
  else
  {
    pSend->byDlgType = static_cast<unsigned __int8>(-1);
    pSend->byErrCode = updated;
  }

  return updated;
}

unsigned __int8 ItemCombineMgr::RequestCombineAcceptProcess(
  _combine_ex_item_accept_request_clzo *pRecv,
  _combine_ex_item_accept_result_zocl *pSend)
{
  unsigned __int8 resultCode = 0;
  _ITEMCOMBINE_DB_BASE playerCombineDB;
  std::memcpy(&playerCombineDB, &m_pMaster->m_Param.m_ItemCombineDB, sizeof(playerCombineDB));

  ClearDB_CombineResult();

  if (playerCombineDB.IsCombineData())
  {
    if (playerCombineDB.m_dwCheckKey == pRecv->dwCheckKey && playerCombineDB.m_byDlgType == pRecv->byDlgType)
    {
      if (MakeNewItems(&playerCombineDB, pRecv, pSend))
      {
        resultCode = 16;
      }
    }
    else
    {
      resultCode = 15;
    }
  }
  else
  {
    resultCode = 14;
  }

  pSend->byErrCode = resultCode;
  return resultCode;
}

unsigned __int8 ItemCombineMgr::MakeNewItems(
  _ITEMCOMBINE_DB_BASE *pPlayerItemDB,
  _combine_ex_item_accept_request_clzo *pRecv,
  _combine_ex_item_accept_result_zocl *pSend)
{
int selectedCount = pRecv->SelectItemBuff.bySelectNum >= 0x18u ? 24 : pRecv->SelectItemBuff.bySelectNum;
  if (selectedCount >= pPlayerItemDB->m_bySelectItemCount)
  {
    selectedCount = pPlayerItemDB->m_bySelectItemCount;
  }

  unsigned __int8 selectedMask[24] = {};
  unsigned __int8 rewardTypeList[24] = {};
  unsigned __int64 rewardUIDList[24] = {};

  if (!pPlayerItemDB->m_byDlgType)
  {
    for (int index = 0; index < selectedCount; ++index)
    {
      const unsigned __int8 selectedIndex = pRecv->SelectItemBuff.bySelectIndexList[index];
      if (selectedIndex < 24u)
      {
        selectedMask[selectedIndex] = 1;
      }
    }
  }
  else if (pPlayerItemDB->m_byDlgType == 1)
  {
    const int itemCount = std::min<int>(pPlayerItemDB->m_byItemListNum, 24);
    selectedCount = itemCount;
    std::memset(selectedMask, 1, static_cast<size_t>(itemCount));
  }

  for (int index = 0; index < std::min<int>(pPlayerItemDB->m_byItemListNum, 24); ++index)
  {
    if (!selectedMask[index] || !pPlayerItemDB->m_List[index].Key.IsFilled())
    {
      continue;
    }

    _ITEMCOMBINE_DB_BASE::_LIST *rewardData = &pPlayerItemDB->m_List[index];
    const int tableCode = rewardData->Key.byTableCode;
    const int itemIndex = rewardData->Key.wItemIndex;

    _STORAGE_LIST::_db_con rewardItem;
    rewardItem.m_byTableCode = static_cast<unsigned __int8>(tableCode);
    rewardItem.m_wItemIndex = static_cast<unsigned __int16>(itemIndex);

    if (IsOverLapItem(tableCode))
    {
      rewardItem.m_dwDur = rewardData->dwDur;
    }
    else
    {
      rewardItem.m_dwDur = GetItemDurPoint(tableCode, itemIndex);
    }

    const unsigned __int8 itemKindCode = GetItemKindCode(tableCode);
    if (itemKindCode == 1)
    {
      rewardItem.m_dwDur = rewardData->dwDur;
      rewardItem.m_dwLv = rewardData->dwUpt;
    }
    else if (!itemKindCode && rewardData->dwUpt == 0xFFFFFFF)
    {
      const unsigned __int8 defaultSocketCount = GetDefItemUpgSocketNum(tableCode, itemIndex);
      const int socketCount = defaultSocketCount ? (rand() % defaultSocketCount) + 1 : 0;
      rewardItem.m_dwLv = GetBitAfterSetLimSocket(static_cast<unsigned __int8>(socketCount));
    }
    else
    {
      rewardItem.m_dwLv = rewardData->dwUpt;
    }

    if (m_pMaster->m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
    {
      CreateItemBox(
        &rewardItem,
        m_pMaster,
        0xFFFFFFFFu,
        false,
        nullptr,
        3u,
        m_pMaster->m_pCurMap,
        m_pMaster->m_wMapLayerIndex,
        m_pMaster->m_fCurPos,
        false);
      rewardTypeList[index] = 2;
    }
    else
    {
      rewardItem.m_wSerial = m_pMaster->m_Param.GetNewItemSerial();
      const _TimeItem_fld *timeRecord = TimeItem::FindTimeRec(tableCode, itemIndex);
      if (timeRecord && timeRecord->m_nCheckType)
      {
        __time32_t currentTime = 0;
        _time32(&currentTime);
        rewardItem.m_byCsMethod = static_cast<unsigned __int8>(timeRecord->m_nCheckType);
        rewardItem.m_dwT = timeRecord->m_nUseTime + currentTime;
        rewardItem.m_dwLendRegdTime = currentTime;
      }

      _STORAGE_LIST::_db_con *addedItem = m_pMaster->Emb_AddStorage(0, &rewardItem, false, true);
      if (!addedItem)
      {
        CPlayer::s_MgrItemHistory.add_storage_fail(
          m_pMaster->m_ObjID.m_wIndex,
          &rewardItem,
          "RewardChangeClass - Emb_AddStorage() Fail",
          m_pMaster->m_szItemHistoryFileName);
        continue;
      }

      m_pMaster->SendMsg_RewardAddItem(&rewardItem, 0);
      rewardTypeList[index] = 1;
      rewardUIDList[index] = addedItem->m_lnUID;
    }

    if (pPlayerItemDB->m_byItemListNum == 1)
    {
      if (pPlayerItemDB->m_dwResultEffectType == 1)
      {
        m_pMaster->SendMsg_FanfareItem(1u, &rewardItem, nullptr);
      }
    }
    else
    {
      m_pMaster->SendMsg_FanfareItem(1u, &rewardItem, nullptr);
    }
  }
return 0;
}

char ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems(
  _STORAGE_LIST::_db_con **pMt_Sv_Inv,
  unsigned __int8 byMtSlotNum,
  _combine_ex_item_request_clzo::_list *pipMaterials,
  _combine_ex_item_result_zocl *pSaveData,
  _ItemCombine_exp_fld *pfld,
  unsigned __int8 byLinkTableIndex,
  int nType)
{
  unsigned __int8 materialSlotCount = byMtSlotNum > 5u ? 5u : byMtSlotNum;
  unsigned __int8 materialUsed[16] = {};
  unsigned __int8 resultCode = 0;

  pSaveData->byDlgType = pfld->m_bSelectItem == 0;
  pSaveData->bySelectItemCount = pfld->m_bSelectItem ? static_cast<unsigned __int8>(pfld->m_nOperationCount) : 0;
  pSaveData->dwDalant = pfld->m_dwCommit;
  pSaveData->dwCheckKey = CombineExCheckKeyGen(timeGetTime(), pfld->m_dwIndex);

  int selectedOutputIndices[24];
  std::memset(selectedOutputIndices, 0xFF, sizeof(selectedOutputIndices));

  int selectedCount = 0;
  int operationCount = pfld->m_nOperationCount;
  unsigned int remainWeight = 10000u;

  if (pSaveData->byDlgType == 1)
  {
    for (int operationIndex = 0; operationIndex < operationCount; ++operationIndex)
    {
      unsigned int randomWeight = 0;
      unsigned int cumulativeWeight = 0;
      bool found = false;

      if (nType)
      {
        randomWeight = remainWeight ? (rand() % remainWeight) : 0;
        for (int outputIndex = 0; outputIndex < 24; ++outputIndex)
        {
          char *outputCode = pfld->m_listOutput[outputIndex].m_itmPdOutput;
          if (!strncmp(outputCode, "-", 1u) || !strncmp(outputCode, "-1", 2u))
          {
            continue;
          }

          bool duplicated = false;
          for (int selectedIndex = 0; selectedIndex < selectedCount; ++selectedIndex)
          {
            if (selectedOutputIndices[selectedIndex] == outputIndex)
            {
              duplicated = true;
              break;
            }
          }
          if (duplicated)
          {
            continue;
          }

          cumulativeWeight += static_cast<unsigned int>(pfld->m_listOutput[outputIndex].m_nOutNum);
          if (randomWeight < cumulativeWeight)
          {
            selectedOutputIndices[selectedCount++] = outputIndex;
            remainWeight -= static_cast<unsigned int>(pfld->m_listOutput[outputIndex].m_nOutNum);
            found = true;
            break;
          }
        }
      }
      else
      {
        randomWeight = rand() % 10000u;
        for (int outputIndex = 0; outputIndex < 24; ++outputIndex)
        {
          char *outputCode = pfld->m_listOutput[outputIndex].m_itmPdOutput;
          if (!strncmp(outputCode, "-", 1u) || !strncmp(outputCode, "-1", 2u))
          {
            continue;
          }

          cumulativeWeight += static_cast<unsigned int>(pfld->m_listOutput[outputIndex].m_nOutNum);
          if (randomWeight < cumulativeWeight)
          {
            selectedOutputIndices[selectedCount++] = outputIndex;
            found = true;
            break;
          }
        }
      }

      if (!found)
      {
        resultCode = 10;
        break;
      }
    }

    if (!resultCode)
    {
      if (selectedCount <= operationCount)
      {
        pSaveData->ItemBuff.Init();
        pSaveData->ItemBuff.byItemListNum = static_cast<unsigned __int8>(selectedCount);
        for (int index = 0; index < selectedCount; ++index)
        {
          if (MakeCombineItem(m_pMaster, &pSaveData->ItemBuff.RewardItemList[index], pfld, selectedOutputIndices[index], byLinkTableIndex))
          {
            resultCode = 12;
            break;
          }
        }

        if (!resultCode)
        {
          if (selectedCount == 1)
          {
            pSaveData->dwResultEffectType =
              static_cast<unsigned int>(pfld->m_listOutput[selectedOutputIndices[0]].m_dwEffectType);
            pSaveData->dwResultEffectMsgCode = pfld->m_listOutput[selectedOutputIndices[0]].m_dwResultEffectMsgCode;
          }
          else
          {
            pSaveData->dwResultEffectType = static_cast<unsigned int>(-1);
            pSaveData->dwResultEffectMsgCode = 0;
          }
        }
      }
      else
      {
        resultCode = 11;
      }
    }
  }
  else
  {
    selectedCount = 0;
    pSaveData->ItemBuff.Init();
    for (int outputIndex = 0; outputIndex < 24; ++outputIndex)
    {
      char *outputCode = pfld->m_listOutput[outputIndex].m_itmPdOutput;
      if (strncmp(outputCode, "-", 1u) && strncmp(outputCode, "-1", 2u))
      {
        selectedOutputIndices[selectedCount++] = outputIndex;
      }
    }

    pSaveData->ItemBuff.byItemListNum = static_cast<unsigned __int8>(selectedCount);
    for (int index = 0; index < selectedCount; ++index)
    {
      if (MakeCombineItem(m_pMaster, &pSaveData->ItemBuff.RewardItemList[index], pfld, selectedOutputIndices[index], byLinkTableIndex))
      {
        resultCode = 12;
        break;
      }
    }
  }

  if (resultCode)
  {
    if (resultCode == 10)
    {
      int failBuffer[7] = {};
      unsigned int failCount = 0;
      int socketIndex = -1;

      if (pfld->m_nFailOutItem >= 0)
      {
        failCount = materialSlotCount - static_cast<unsigned int>(pfld->m_nFailOutItem);
        if ((failCount & 0x80000000u) != 0 || !materialSlotCount)
        {
          return 11;
        }
        TakeOutLotto(failBuffer, materialSlotCount);
      }
      else
      {
        failCount = materialSlotCount - static_cast<unsigned int>(std::abs(pfld->m_nFailOutItem));
        if ((failCount & 0x80000000u) != 0 || !materialSlotCount)
        {
          return 11;
        }
        if (!BuildFailItemShortBuffer(failBuffer, materialSlotCount, pfld->m_Material, pMt_Sv_Inv))
        {
          return 11;
        }
      }

      pSaveData->ItemBuff.Init();
      pSaveData->ItemBuff.byItemListNum = static_cast<unsigned __int8>(failCount);

      for (int failIndex = 0; failIndex < static_cast<int>(failCount); ++failIndex)
      {
        socketIndex = failBuffer[failIndex];
        if (socketIndex < 0 || socketIndex >= materialSlotCount)
        {
          return 11;
        }
        if (!DestroyCombineItem(
              m_pMaster,
              &pSaveData->ItemBuff.RewardItemList[failIndex],
              pMt_Sv_Inv[socketIndex],
              &pipMaterials[socketIndex],
              socketIndex))
        {
          return 4;
        }
      }

      for (int failIndex = 0; failIndex < static_cast<int>(failCount); ++failIndex)
      {
        socketIndex = failBuffer[failIndex];
        m_pMaster->Emb_AlterDurPoint(
          0,
          pMt_Sv_Inv[socketIndex]->m_byStorageIndex,
          -pipMaterials[socketIndex].byAmount,
          true,
          true);
        materialUsed[socketIndex] = pipMaterials[socketIndex].byAmount;
      }

      if (pfld->m_dwCommit)
      {
        m_pMaster->SubDalant(pfld->m_dwCommit);
        const int level = m_pMaster->m_Param.GetLevel();
        if (level == 30 || level == 40 || level == 50 || level == 60)
        {
          CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
            static_cast<unsigned __int8>(m_pMaster->m_Param.GetRaceCode()),
            level,
            pfld->m_dwCommit);
        }
      }

      m_pMaster->SendMsg_AlterMoneyInform(0);
    }
    return static_cast<char>(resultCode);
  }

  for (int slotIndex = 0; slotIndex < materialSlotCount; ++slotIndex)
  {
    if (IsOverLapItem(pMt_Sv_Inv[slotIndex]->m_byTableCode))
    {
      m_pMaster->Emb_AlterDurPoint(
        0,
        pMt_Sv_Inv[slotIndex]->m_byStorageIndex,
        -pipMaterials[slotIndex].byAmount,
        true,
        true);
    }
    else if (!m_pMaster->Emb_DelStorage(
               0,
               pMt_Sv_Inv[slotIndex]->m_byStorageIndex,
               true,
               true,
               "ItemCombineMgr::ConsumeMeterial_And_CalculateNewItems()"))
    {
      return 5;
    }

    materialUsed[slotIndex] = pipMaterials[slotIndex].byAmount;
  }

  const unsigned int feeDalant = pfld->m_dwCommit;
  if (feeDalant)
  {
    m_pMaster->SubDalant(feeDalant);
    const int level = m_pMaster->m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
        static_cast<unsigned __int8>(m_pMaster->m_Param.GetRaceCode()),
        level,
        feeDalant);
    }
  }

  m_pMaster->SendMsg_AlterMoneyInform(0);
  return static_cast<char>(resultCode);
}

