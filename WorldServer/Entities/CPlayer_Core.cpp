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
char s_playerObjectName[256]{};

int ClampToleranceValue(int value)
{
  if (value < -100)
  {
    return -100;
  }
  if (value > 100)
  {
    return 100;
  }
  return value;
}

bool IsXmasSnowEffect(const _sf_continous *sfCont)
{
  return sfCont
      && CPlayer::ms_pXmas_Snow_Effect
      && sfCont->m_byEffectCode == 3
      && sfCont->m_wEffectIndex == CPlayer::ms_pXmas_Snow_Effect->m_dwIndex;
}





















































char IsOtherTowerNear(CGameObject *pEster, float *pfEstPos, CGuardTower *pEstObj);

bool IsSameTargetAsPerformer(const CPlayer *player, const _CHRID *targetId)
{
  return targetId
    && player->m_ObjID.m_byID == targetId->byID
    && player->m_dwObjSerial == targetId->dwSerial;
}

bool IsUsableTempEffectAtStoneState(int nTempEffectType)
{
  if (nTempEffectType < 0)
  {
    return false;
  }

  if (nTempEffectType == 5)
  {
    return true;
  }

  return (nTempEffectType > 6 && nTempEffectType <= 14) || nTempEffectType == 28;
}







void FillThrowEffectList(_throw_skill_result_one_zocl::__effected_list *outList, char *outCount)
{
  *outCount = g_tmpEffectedNum;
  for (int index = 0; index < g_tmpEffectedNum; ++index)
  {
    outList[index].byObjID = g_tmpEffectedList[index].pMem->m_ObjID.m_byID;
    outList[index].dwObjSerial = g_tmpEffectedList[index].pMem->m_dwObjSerial;
    outList[index].byRetCode = g_tmpEffectedList[index].byRetCode;
    outList[index].wEffectValue = g_tmpEffectedList[index].wEffectValue;
  }
}







char IsOtherInvalidObjNear(CGameObject *pEster, float *pfEstPos, CTrap *pEstObj, _TrapItem_fld *pEstTrapItemInfo)
{
  if (!pEster || !pfEstPos)
  {
    return 0;
  }

  const float checkRange = pEstTrapItemInfo ? pEstTrapItemInfo->m_fReactionDst : 50.0f;
  for (int index = 0; index < MAX_TRAP; ++index)
  {
    CTrap *trap = &g_Trap[index];
    if (!trap->m_bLive || trap == pEstObj)
    {
      continue;
    }
    if (std::fabs(trap->m_fCurPos[1] - pfEstPos[1]) > 100.0f)
    {
      continue;
    }
    if (GetSqrt(trap->m_fCurPos, pfEstPos) < checkRange)
    {
      return 10;
    }
  }

  if (IsOtherTowerNear(pEster, pfEstPos, nullptr))
  {
    return 10;
  }

  return 0;
}

constexpr int kUnitBulletPartByPartCode[6] = {-1, -1, -1, 0, 1, -1};
constexpr unsigned __int8 kUnitWeaponPartByBulletSlot[2] = {3, 4};
constexpr int kAnimusRecallConsumeFP = 60;

unsigned int GenerateTransientObjSerial()
{
  static unsigned int s_serialCounter = 1;
  return ++s_serialCounter;
}

CAnimus *FindEmptyAnimusSlot()
{
  for (int index = 0; index < MAX_ANIMUS; ++index)
  {
    if (!g_Animus[index].m_bLive)
    {
      return &g_Animus[index];
    }
  }
  return nullptr;
}

CParkingUnit *FindEmptyParkingUnitSlot()
{
  const unsigned int now = timeGetTime();
  for (int index = 0; index < MAX_PARKING_UNIT; ++index)
  {
    CParkingUnit *slot = &g_ParkingUnit[index];
    if (!slot->m_bLive && now - slot->m_dwLastDestroyTime > 30000u)
    {
      return slot;
    }
  }
  return nullptr;
}

CTrap *FindEmptyTrapSlot()
{
  for (int index = 0; index < MAX_TRAP; ++index)
  {
    if (!g_Trap[index].m_bLive)
    {
      return &g_Trap[index];
    }
  }
  return nullptr;
}

_UnitKeyItem_fld *GetUnitKeyMatchFrame(unsigned __int8 byFrameCode)
{
  const int recordNum = g_Main.m_tblItemData[19].GetRecordNum();
  for (int index = 0; index < recordNum; ++index)
  {
    _base_fld *record = g_Main.m_tblItemData[19].GetRecord(index);
    if (record && *reinterpret_cast<int *>(&record[3].m_strCode[60]) == byFrameCode)
    {
      return reinterpret_cast<_UnitKeyItem_fld *>(record);
    }
  }
  return nullptr;
}



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

  _base_fld *linkRecord = linkTable->GetRecord(linkItemCode);
  if (!linkRecord)
  {
    return false;
  }

  for (unsigned __int8 index = 0; index < 100; ++index)
  {
    const char *entryCode = reinterpret_cast<const char *>(&linkRecord[1]) + 64u * index;
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
    _base_fld *linkRecord = ItemCombineMgr::ms_tbl_ItemCombine_Link_Result.GetRecord(outputCode);
    if (!linkRecord || linkTableIndex >= 100u)
    {
      return 12;
    }
    outputCode = reinterpret_cast<const char *>(&linkRecord[1]) + 64u * linkTableIndex;
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

CCharacter *GetLiveTargetInSameMap(CPlayer *player, _CHRID *targetId)
{
  if (!player || !targetId)
  {
    return nullptr;
  }

  CCharacter *target = reinterpret_cast<CCharacter *>(g_Main.GetObjectA(0, targetId->byID, targetId->wIndex));
  if (!target || !target->m_bLive || target->m_pCurMap != player->m_pCurMap)
  {
    return nullptr;
  }
  return target;
}

CGuardTower *CreateGuardTower(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  _STORAGE_LIST::_db_con *pItem,
  CPlayer *pMaster,
  unsigned __int8 byRaceCode,
  bool bQuick)
{
  CGuardTower *tower = nullptr;
  for (int index = 0; index < MAX_TOWER; ++index)
  {
    if (!g_Tower[index].m_bLive)
    {
      tower = &g_Tower[index];
      break;
    }
  }

  if (!tower)
  {
    return nullptr;
  }

  _tower_create_setdata data{};
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = g_Main.m_tblItemData[25].GetRecord(pItem->m_wItemIndex);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }

  std::memcpy(data.m_fStartPos, fPos, sizeof(data.m_fStartPos));
  data.pMaster = pMaster;
  data.byRaceCode = byRaceCode;
  data.pItem = pItem;
  data.bQuick = bQuick;

  return tower->Create(&data) ? tower : nullptr;
}

char IsOtherTowerNear(CGameObject *pEster, float *pfEstPos, CGuardTower *pEstObj)
{
  if (!pEster || !pEster->m_pCurMap)
  {
    return 0;
  }

  _pnt_rect rect{};
  pEster->m_pCurMap->GetRectInRadius(&rect, 3, static_cast<int>(pEster->GetCurSecNum()));

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      _sec_info *secInfo = pEster->m_pCurMap->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = pEster->m_pCurMap->GetSectorListObj(pEster->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      _object_list_point *node = sectorList->m_Head.m_pNext;
      while (node != &sectorList->m_Tail)
      {
        CGuardTower *tower = static_cast<CGuardTower *>(node->m_pItem);
        node = node->m_pNext;

        if (!tower->m_ObjID.m_byKind
            && tower->m_ObjID.m_byID == 4
            && tower != pEstObj
            && std::abs(tower->m_fCurPos[1] - pfEstPos[1]) <= 100.0f)
        {
          const int nearRange = *reinterpret_cast<int *>(&tower->m_pRecordSet[5].m_strCode[24]);
          if (static_cast<double>(nearRange) > GetSqrt(tower->m_fCurPos, pfEstPos))
          {
            return 1;
          }
        }
      }
    }
  }

  return 0;
}

CTrap *CreateTrapObject(CPlayer *player, unsigned __int16 trapItemIndex, float *position)
{
  CTrap *slot = FindEmptyTrapSlot();
  if (!slot)
  {
    return nullptr;
  }

  _character_create_setdata createData{};
  createData.m_pMap = player->m_pCurMap;
  createData.m_nLayerIndex = player->m_wMapLayerIndex;
  createData.m_pRecordSet = g_Main.m_tblItemData[26].GetRecord(trapItemIndex);
  if (!createData.m_pRecordSet)
  {
    return nullptr;
  }
  std::memcpy(createData.m_fStartPos, position, sizeof(createData.m_fStartPos));
  if (!slot->Create(&createData))
  {
    return nullptr;
  }

  slot->m_dwObjSerial = GenerateTransientObjSerial();
  slot->m_pMaster = player;
  slot->m_dwMasterSerial = player->m_dwObjSerial;
  slot->m_byRaceCode = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
  slot->m_dwStartMakeTime = GetLoopTime();
  slot->m_nTrapMaxAttackPnt = player->m_nTrapMaxAttackPnt;

  _TrapItem_fld *trapInfo = reinterpret_cast<_TrapItem_fld *>(createData.m_pRecordSet);
  slot->m_nHP = static_cast<int>(trapInfo->m_fMaxHP);
  return slot;
}

CParkingUnit *CreateParkingUnitObject(CPlayer *player, unsigned __int8 bySlotIndex, float *position)
{
  _UNIT_DB_BASE::_LIST *unitData = &player->m_Param.m_UnitDB.m_List[bySlotIndex];
  CParkingUnit *slot = FindEmptyParkingUnitSlot();
  if (!slot)
  {
    return nullptr;
  }

  _parkingunit_create_setdata createData{};
  createData.byFrame = unitData->byFrame;
  std::memcpy(createData.byPartCode, unitData->byPart, sizeof(createData.byPartCode));
  createData.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame);
  createData.pOwner = player;
  createData.byCreateType = 0;
  createData.m_pMap = player->m_pCurMap;
  createData.m_nLayerIndex = player->m_wMapLayerIndex;
  std::memcpy(createData.m_fStartPos, position, sizeof(createData.m_fStartPos));
  createData.byTransDistCode = 0;
  createData.wHPRate = 10000;
  if (!createData.m_pRecordSet || !slot->Create(&createData))
  {
    return nullptr;
  }
  return slot;
}

CAnimus *CreateAnimusObject(CPlayer *player, _STORAGE_LIST::_db_con *animusItem)
{
  if (!player || !animusItem)
  {
    return nullptr;
  }

  CAnimus *slot = FindEmptyAnimusSlot();
  if (!slot)
  {
    return nullptr;
  }

  _animus_create_setdata createData{};
  createData.m_pMap = player->m_pCurMap;
  createData.m_nLayerIndex = player->m_wMapLayerIndex;
  createData.m_pRecordSet = g_Main.m_tblAnimus.GetRecord(animusItem->m_wItemIndex);
  if (!createData.m_pRecordSet)
  {
    return nullptr;
  }
  std::memcpy(createData.m_fStartPos, player->m_fCurPos, sizeof(createData.m_fStartPos));
  createData.nHP = LOWORD(animusItem->m_dwLv);
  createData.nFP = HIWORD(animusItem->m_dwLv);
  createData.dwExp = animusItem->m_dwDur;
  createData.pMaster = player;
  createData.nMaxAttackPnt = player->m_nAnimusAttackPnt;

  if (!slot->Create(&createData))
  {
    return nullptr;
  }
  return slot;
}


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

int GetResDummyDelayForMining(const _res_dummy &resDummy, int sectorIndex, bool isPcBang)
{
  if (sectorIndex < 0 || sectorIndex >= 3)
  {
    return 0;
  }

  const int gradeIndex = isPcBang ? 1 : 0;
  return static_cast<int>(resDummy.m_dwDelay[sectorIndex][gradeIndex]);
}
















}



unsigned __int16 GetTargetMonsterContInfoSize(_target_monster_contsf_allinform_zocl *info)
{
  if (info->byContCount > 8u)
  {
    info->byContCount = 0;
  }
  return static_cast<unsigned __int16>(21 - 2 * (8 - info->byContCount));
}

bool IsSameTargetMonsterContInfo(
  const _target_monster_contsf_allinform_zocl &left,
  const _target_monster_contsf_allinform_zocl &right)
{
  if (left.dwSerial != right.dwSerial)
  {
    return false;
  }

  for (int index = 0; index < 8; ++index)
  {
    if (left.m_MonContSf[index].wSfcode != right.m_MonContSf[index].wSfcode)
    {
      return false;
    }
  }
  return true;
}

unsigned __int16 GetTargetPlayerDamageContInfoSize(_target_player_damage_contsf_allinform_zocl *info)
{
  if (info->byContCount > 8u)
  {
    info->byContCount = 0;
  }
  return static_cast<unsigned __int16>(29 - 3 * (8 - info->byContCount));
}

bool IsSameTargetPlayerDamageContInfo(
  const _target_player_damage_contsf_allinform_zocl &left,
  const _target_player_damage_contsf_allinform_zocl &right)
{
  if (left.dwSerial != right.dwSerial)
  {
    return false;
  }

  for (int index = 0; index < 8; ++index)
  {
    if (left.m_PlayerContSf[index].wSfcode != right.m_PlayerContSf[index].wSfcode ||
        left.m_PlayerContSf[index].byContCount != right.m_PlayerContSf[index].byContCount)
    {
      return false;
    }
  }
  return true;
}

void CPlayer::SendMsg_DamageResult(_STORAGE_LIST::_db_con *pItem)
{
  char payload[5]{};
  payload[0] = static_cast<char>(this->m_nLastBeatenPart);
  *reinterpret_cast<unsigned __int16 *>(payload + 1) = pItem->m_wSerial;
  *reinterpret_cast<__int16 *>(payload + 3) = static_cast<__int16>(pItem->m_dwDur);

  unsigned __int8 type[2] = {5, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 5u);
}



void CPlayer::SendMsg_ItemStorageRefresh(unsigned __int8 byStorageCode)
{
  _STORAGE_LIST *storage = this->m_Param.m_pStoragePtr[byStorageCode];
  char payload[0xCA]{};

  payload[0] = static_cast<char>(byStorageCode);
  const unsigned __int8 itemCount = static_cast<unsigned __int8>(storage->GetNumUseCon());
  payload[1] = static_cast<char>(itemCount);

  unsigned __int16 *serialList = reinterpret_cast<unsigned __int16 *>(payload + 2);
  int outIndex = 0;
  for (int slotIndex = 0; slotIndex < itemCount; ++slotIndex)
  {
    _STORAGE_LIST::_db_con *entry = &storage->m_pStorageList[slotIndex];
    if (entry->m_bLoad)
    {
      serialList[outIndex++] = entry->m_wSerial;
    }
  }

  unsigned __int8 type[2] = {3, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0xCAu);
}

void CPlayer::SendMsg_UsPotionResultOther(
  char byRetcode,
  unsigned __int16 wPotionIndex,
  CPlayer *pUsePlayer,
  bool bCircle)
{
  char payload[0x17]{};
  payload[0] = byRetcode;
  payload[1] = static_cast<char>(pUsePlayer->m_ObjID.m_byID);
  *reinterpret_cast<unsigned __int16 *>(payload + 2) = pUsePlayer->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 4) = pUsePlayer->m_dwObjSerial;
  payload[8] = static_cast<char>(this->m_ObjID.m_byID);
  *reinterpret_cast<unsigned __int16 *>(payload + 9) = this->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 11) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 15) = wPotionIndex;
  *reinterpret_cast<__int16 *>(payload + 17) = static_cast<__int16>(this->m_Param.GetHP());
  *reinterpret_cast<__int16 *>(payload + 19) = static_cast<__int16>(this->m_Param.GetFP());
  *reinterpret_cast<__int16 *>(payload + 21) = static_cast<__int16>(this->m_Param.GetSP());

  unsigned __int8 type[2] = {7, 108};
  if (bCircle)
  {
    this->CircleReport(type, payload, 23, true);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x17u);
  }
}



























void CPlayer::SendMsg_MineCompleteResult(
  char byErrCode,
  unsigned __int8 byNewOreIndex,
  unsigned __int16 dwOreSerial,
  unsigned __int8 byOreDur,
  unsigned __int16 dwBatteryLeftDurPoint)
{
  char payload[9]{};
  payload[0] = byErrCode;
  *reinterpret_cast<__int16 *>(payload + 1) = static_cast<__int16>(this->m_Param.m_dbEquip.m_pStorageList[6].m_dwDur);
  *reinterpret_cast<unsigned __int16 *>(payload + 3) = dwBatteryLeftDurPoint;
  payload[5] = static_cast<char>(byNewOreIndex);
  *reinterpret_cast<unsigned __int16 *>(payload + 6) = dwOreSerial;
  payload[8] = static_cast<char>(byOreDur);

  unsigned __int8 type[2] = {14, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 9u);
}

void CPlayer::SendMsg_HSKQuestSucc(char byQuestCode, char bSucc)
{
  char payload[2]{};
  payload[0] = byQuestCode;
  payload[1] = bSucc;

  unsigned __int8 type[2] = {25, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
}

void CPlayer::SendMsg_InsertQuestItemInform(_STORAGE_LIST::_db_con *pItem)
{
  char payload[0x0B]{};
  payload[0] = static_cast<char>(pItem->m_byTableCode);
  *reinterpret_cast<unsigned __int16 *>(payload + 1) = pItem->m_wItemIndex;
  *reinterpret_cast<unsigned int *>(payload + 3) = static_cast<unsigned int>(pItem->m_dwDur);
  *reinterpret_cast<unsigned int *>(payload + 7) = pItem->m_dwLv;

  unsigned __int8 type[2] = {24, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x0Bu);
}

void CPlayer::SFContInsertMessage(
  unsigned __int8 byContCode,
  unsigned __int8 byListIndex,
  bool bAuraSkill,
  CPlayer *pPlayerAct)
{
  _sf_continous *sfCont = bAuraSkill ? &m_SFContAura[byContCode][byListIndex] : &m_SFCont[byContCode][byListIndex];

  EquipItemSFAgent.StartContSF(sfCont);
  if (IsXmasSnowEffect(sfCont))
  {
    m_bSnowMan = true;
  }

  if (pPlayerAct)
  {
    const char *playerName = pPlayerAct->m_Param.GetCharNameW();
    const unsigned int playerSerial = pPlayerAct->m_Param.GetCharSerial();
    const unsigned __int16 effectBit = static_cast<unsigned __int16>(CalcEffectBit(sfCont->m_byEffectCode, sfCont->m_wEffectIndex));
    SendMsg_AddEffect(effectBit, sfCont->m_byLv, sfCont->m_wDurSec, playerSerial, const_cast<char *>(playerName));
  }
  else
  {
    const unsigned __int16 effectBit = static_cast<unsigned __int16>(CalcEffectBit(sfCont->m_byEffectCode, sfCont->m_wEffectIndex));
    SendMsg_AddEffect(effectBit, sfCont->m_byLv, sfCont->m_wDurSec, 0u, nullptr);
  }

  if (m_pUserDB && !bAuraSkill)
  {
    m_pUserDB->Update_SFContInsert(
      byContCode,
      byListIndex,
      sfCont->m_byEffectCode,
      sfCont->m_wEffectIndex,
      static_cast<unsigned __int8>(sfCont->m_byLv - 1),
      sfCont->m_wDurSec);
  }
}

void CPlayer::SFContDelMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bSend, bool bAura)
{
  if (!m_bOper)
  {
    return;
  }

  if (bSend)
  {
    _sf_continous *sfCont = bAura ? &m_SFContAura[byContCode][byListIndex] : &m_SFCont[byContCode][byListIndex];

    SendMsg_DelEffect(sfCont->m_byEffectCode, sfCont->m_wEffectIndex, sfCont->m_byLv);
    EquipItemSFAgent.EndContSF(sfCont);
    if (IsXmasSnowEffect(sfCont))
    {
      m_bSnowMan = false;
    }

    _base_fld *afterEffect = g_Main.m_tblEffectData[3].GetRecord("17");
    if (afterEffect && sfCont->m_byEffectCode == 3 && sfCont->m_wEffectIndex == afterEffect->m_dwIndex)
    {
      m_bAfterEffect = false;
    }
  }

  if (m_pUserDB && !bAura)
  {
    m_pUserDB->Update_SFContDelete(byContCode, byListIndex);
  }
}

void CPlayer::SFContUpdateTimeMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, int nLeftTime)
{
  if (m_pUserDB && nLeftTime > 0)
  {
    m_pUserDB->Update_SFContUpdate(byContCode, byListIndex, static_cast<unsigned __int16>(nLeftTime));
  }
}

void CPlayer::SendMsg_AddEffect(
  unsigned __int16 wEffectCode,
  unsigned __int8 byLv,
  unsigned __int16 wDurSec,
  unsigned int dwPlayerSerial,
  char *wszPlayerName)
{
  char payload[0x1A]{};
  payload[0] = static_cast<char>(byLv);
  *reinterpret_cast<unsigned __int16 *>(payload + 1) = wEffectCode;
  *reinterpret_cast<unsigned __int16 *>(payload + 3) = wDurSec;
  *reinterpret_cast<unsigned int *>(payload + 5) = dwPlayerSerial;
  if (dwPlayerSerial != 0)
  {
    strcpy_s(payload + 9, 0x11u, wszPlayerName);
  }

  unsigned __int8 type[2] = {17, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x1Au);
  this->SendData_PartyMemberEffect(0, wEffectCode, byLv);
}

void CPlayer::SendMsg_DelEffect(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex, unsigned __int8 byLv)
{
  const unsigned __int16 effectBit =
    static_cast<unsigned __int16>(this->CalcEffectBit(byEffectCode, wEffectIndex));

  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = effectBit;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;

  unsigned __int8 type[2] = {17, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 6u);
  this->SendData_PartyMemberEffect(1, effectBit, byLv);
}

void CPlayer::SendMsg_DTradeAskInform(CPlayer *pAsker)
{
  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = pAsker->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = pAsker->m_dwObjSerial;

  unsigned __int8 type[2] = {18, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 6u);
}

void CPlayer::SendMsg_DTradeAnswerResult(char byErrCode)
{
  char payload[1]{};
  payload[0] = byErrCode;

  unsigned __int8 type[2] = {18, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_DTradeStartInform(CPlayer *pAsker, CPlayer *pAnswer, unsigned int *pdwKey)
{
  char payload[0x1E]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = pAsker->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = pAsker->m_dwObjSerial;
  payload[6] = static_cast<char>(pAsker->m_pmTrd.byEmptyInvenNum);
  *reinterpret_cast<unsigned __int16 *>(payload + 7) = pAnswer->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 9) = pAnswer->m_dwObjSerial;
  payload[13] = static_cast<char>(pAnswer->m_pmTrd.byEmptyInvenNum);
  if (pdwKey)
  {
    memcpy_0(payload + 14, pdwKey, 0x10uLL);
  }

  unsigned __int8 type[2] = {18, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x1Eu);
}

void CPlayer::SendMsg_DTradeLockResult(char byErrCode)
{
  char payload[1]{};
  payload[0] = byErrCode;

  unsigned __int8 type[2] = {18, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_DTradeLockInform()
{
  char payload[1]{};

  unsigned __int8 type[2] = {18, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_DTradeAddResult(char byErrCode)
{
  char payload[1]{};
  payload[0] = byErrCode;

  unsigned __int8 type[2] = {18, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_DTradeAddInform(char bySlotIndex, _STORAGE_LIST::_db_con *pItem, unsigned __int8 byAmount)
{
  char payload[0x17]{};
  payload[0] = bySlotIndex;
  payload[1] = static_cast<char>(pItem->m_byTableCode);
  *reinterpret_cast<unsigned __int16 *>(payload + 2) = pItem->m_wItemIndex;
  *reinterpret_cast<unsigned __int64 *>(payload + 4) = pItem->m_dwDur;
  *reinterpret_cast<unsigned int *>(payload + 12) = pItem->m_dwLv;
  payload[16] = static_cast<char>(byAmount);
  payload[17] = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);
  payload[18] = static_cast<char>(pItem->m_byCsMethod);
  *reinterpret_cast<unsigned int *>(payload + 19) = pItem->m_dwT;

  unsigned __int8 type[2] = {18, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x17u);
}

void CPlayer::SendMsg_DTradeDelResult(char byErrCode)
{
  char payload[1]{};
  payload[0] = byErrCode;

  unsigned __int8 type[2] = {18, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_DTradeDelInform(char bySlotIndex)
{
  char payload[2]{};
  payload[0] = bySlotIndex;
  payload[1] = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);

  unsigned __int8 type[2] = {18, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
}

void CPlayer::SendMsg_DTradeBetResult(char byErrCode)
{
  char payload[1]{};
  payload[0] = byErrCode;

  unsigned __int8 type[2] = {18, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_DTradeBetInform(char byUnitCode, unsigned int dwAmount)
{
  char payload[5]{};
  payload[0] = byUnitCode;
  *reinterpret_cast<unsigned int *>(payload + 1) = dwAmount;

  unsigned __int8 type[2] = {18, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 5u);
}

void CPlayer::SendMsg_DTradeUnitInfoInform(char byTradeSlotIndex, _UNIT_DB_BASE::_LIST *pUnitData)
{
  char payload[0x38]{};
  payload[0] = byTradeSlotIndex;
  payload[1] = static_cast<char>(pUnitData->byFrame);
  *reinterpret_cast<unsigned int *>(payload + 2) = pUnitData->dwGauge;
  memcpy_0(payload + 6, pUnitData->byPart, sizeof(pUnitData->byPart));
  memcpy_0(payload + 12, pUnitData->dwBullet, sizeof(pUnitData->dwBullet));
  memcpy_0(payload + 20, pUnitData->dwSpare, sizeof(pUnitData->dwSpare));
  *reinterpret_cast<int *>(payload + 52) = pUnitData->nPullingFee;

  unsigned __int8 type[2] = {18, 27};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x38u);
}

void CPlayer::SendMsg_DTradeUnitAddInform(unsigned __int16 wUnitKeySerial, _UNIT_DB_BASE::_LIST *pUnitData)
{
  char payload[0x3A]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = wUnitKeySerial;
  payload[2] = static_cast<char>(pUnitData->bySlotIndex);
  payload[3] = static_cast<char>(pUnitData->byFrame);
  *reinterpret_cast<unsigned int *>(payload + 4) = pUnitData->dwGauge;
  memcpy_0(payload + 8, pUnitData->byPart, sizeof(pUnitData->byPart));
  memcpy_0(payload + 14, pUnitData->dwBullet, sizeof(pUnitData->dwBullet));
  memcpy_0(payload + 22, pUnitData->dwSpare, sizeof(pUnitData->dwSpare));
  *reinterpret_cast<int *>(payload + 54) = pUnitData->nPullingFee;

  unsigned __int8 type[2] = {18, 28};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x3Au);
}

void CPlayer::SendMsg_StartShopping()
{
  char payload[1]{};

  unsigned __int8 type[2] = {12, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_CastVoteResult(char byRetCode)
{
  char payload[0x0B]{};
  payload[0] = byRetCode;

  if (byRetCode == 0)
  {
    *reinterpret_cast<int *>(payload + 1) = this->m_nVoteSerial;
    const int raceCode = this->m_Param.GetRaceCode();
    for (int index = 0; index < 3; ++index)
    {
      *reinterpret_cast<unsigned __int16 *>(payload + 5 + (2 * index)) =
        static_cast<unsigned __int16>(g_VoteSys[raceCode].m_dwPoint[index]);
    }
  }

  unsigned __int8 type[2] = {26, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x0Bu);
}

void CPlayer::SendMsg_ExtTrunkExtendResult(char byRetCode, unsigned __int8 bySlotNum, unsigned __int8 byLackSlotNum)
{
  char payload[3]{};
  payload[0] = byRetCode;
  payload[1] = static_cast<char>(bySlotNum / 20);
  payload[2] = static_cast<char>(byLackSlotNum / 20);

  unsigned __int8 type[2] = {34, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 3u);
}

void CPlayer::SendMsg_RemainTimeInform(__int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate)
{
  char payload[0x16]{};
  *reinterpret_cast<__int16 *>(payload) = iType;
  *reinterpret_cast<int *>(payload + 2) = lRemainTime;
  if (pstEndDate)
  {
    memcpy_0(payload + 6, pstEndDate, 0x10uLL);
  }

  unsigned __int8 type[2] = {29, 1};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x16u);
}

void CPlayer::SendMsg_BillingTypeChangeInform(
  __int16 iType,
  int lRemainTime,
  _SYSTEMTIME *pstEndDate,
  unsigned __int8 byReason)
{
  char payload[0x19]{};
  *reinterpret_cast<__int16 *>(payload) = this->m_pUserDB->m_BillingInfo.iType;
  *reinterpret_cast<__int16 *>(payload + 2) = iType;
  *reinterpret_cast<int *>(payload + 4) = lRemainTime;
  if (pstEndDate)
  {
    memcpy_0(payload + 8, pstEndDate, 0x10uLL);
  }
  payload[24] = static_cast<char>(byReason);

  unsigned __int8 type[2] = {29, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x19u);
}

void CPlayer::SendTargetMonsterSFContInfo()
{
  CGameObject *targetObject = this->GetTargetObj();
  if (!targetObject || targetObject->m_ObjID.m_byKind != 0 || targetObject->m_ObjID.m_byID != 1)
  {
    return;
  }

  CMonster *targetMonster = reinterpret_cast<CMonster *>(targetObject);
  _target_monster_contsf_allinform_zocl currentInfo{};
  currentInfo.Init();
  currentInfo.dwSerial = targetMonster->m_dwObjSerial;

  unsigned __int8 contCount = 0;
  for (int groupIndex = 0; groupIndex < 2; ++groupIndex)
  {
    for (int effectIndex = 0; effectIndex < targetMonster->GetMaxDMGSFContCount(); ++effectIndex)
    {
      _sf_continous *cont = &targetMonster->m_SFCont[groupIndex][effectIndex];
      if (!cont->m_bExist)
      {
        continue;
      }

      if (contCount < targetMonster->GetMaxDMGSFContCount())
      {
        currentInfo.m_MonContSf[contCount].wSfcode =
          static_cast<unsigned __int16>(this->CalcEffectBit(cont->m_byEffectCode, cont->m_wEffectIndex));
        ++contCount;
        currentInfo.byContCount = contCount;
      }
    }
  }

  if (IsSameTargetMonsterContInfo(currentInfo, this->m_TargetObject.m_PrevTargetMonsterContInfo))
  {
    return;
  }

  unsigned __int8 type[2] = {13, 100};
  const unsigned __int16 length = GetTargetMonsterContInfoSize(&currentInfo);
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&currentInfo),
    length);

  memcpy_0(
    &this->m_TargetObject.m_PrevTargetMonsterContInfo,
    &currentInfo,
    sizeof(this->m_TargetObject.m_PrevTargetMonsterContInfo));
}

void CPlayer::SendTargetPlayerDamageContInfo()
{
  CGameObject *targetObject = this->GetTargetObj();
  if (!targetObject || targetObject->m_ObjID.m_byKind != 0 || targetObject->m_ObjID.m_byID != 0)
  {
    return;
  }

  _target_player_damage_contsf_allinform_zocl currentInfo{};
  currentInfo.Init();
  currentInfo.dwSerial = targetObject->m_dwObjSerial;

  unsigned __int8 contCount = 0;
  for (int index = 0; index < 8; ++index)
  {
    char *contData = reinterpret_cast<char *>(&targetObject[1].m_fAbsPos[12 * index + 2]);
    if (!contData[0])
    {
      continue;
    }

    currentInfo.m_PlayerContSf[contCount].wSfcode = static_cast<unsigned __int16>(this->CalcEffectBit(
      static_cast<unsigned __int8>(contData[1]),
      *reinterpret_cast<unsigned __int16 *>(contData + 2)));
    currentInfo.m_PlayerContSf[contCount].byContCount = static_cast<unsigned __int8>(contData[20]);
    ++contCount;
    currentInfo.byContCount = contCount;
  }

  if (IsSameTargetPlayerDamageContInfo(currentInfo, this->m_TargetObject.m_PrevTargetPlayerDamageContInfo))
  {
    return;
  }

  unsigned __int8 type[2] = {13, 112};
  const unsigned __int16 length = GetTargetPlayerDamageContInfoSize(&currentInfo);
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&currentInfo),
    length);

  memcpy_0(
    &this->m_TargetObject.m_PrevTargetPlayerDamageContInfo,
    &currentInfo,
    sizeof(this->m_TargetObject.m_PrevTargetPlayerDamageContInfo));
}

void CPlayer::SendMsg_TeleportError(char byErrorCode, unsigned int dwMapIndex)
{
  char payload[5]{};
  payload[0] = byErrorCode;
  *reinterpret_cast<unsigned int *>(payload + 1) = dwMapIndex;

  unsigned __int8 type[2] = {17, 43};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 5u);
}

void CPlayer::SendMsg_RevivalOfJade(unsigned __int16 wSuccRate)
{
  (void)wSuccRate;
  char payload[1]{};

  unsigned __int8 type[2] = {3, 58};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_UseJadeResult(unsigned __int8 byErrCode, unsigned __int16 wItemSerial)
{
  char payload[6]{};
  *reinterpret_cast<unsigned int *>(payload) = byErrCode;
  *reinterpret_cast<unsigned __int16 *>(payload + 4) = wItemSerial;

  unsigned __int8 type[2] = {7, 65};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 6u);
}

void CPlayer::SendMsg_NotifyEffectForGetItem(
  char byBoxType,
  unsigned int dwCharSerial,
  char *szCharName,
  _STORAGE_LIST::_db_con *pItem,
  bool bCircle)
{
  char payload[0x1A]{};
  payload[0] = byBoxType;
  payload[1] = static_cast<char>(pItem->m_byTableCode);
  *reinterpret_cast<unsigned __int16 *>(payload + 2) = pItem->m_wItemIndex;
  payload[4] = static_cast<char>(pItem->m_dwDur);
  *reinterpret_cast<unsigned int *>(payload + 5) = dwCharSerial;
  strcpy_0(payload + 9, szCharName);

  unsigned __int8 type[2] = {13, static_cast<unsigned __int8>(-106)};
  if (bCircle)
  {
    this->CircleReport(type, payload, 26, true);
  }
  else
  {
    for (unsigned int index = 0; index < MAX_PLAYER; ++index)
    {
      CPlayer *targetPlayer = &g_Player[index];
      if (targetPlayer && targetPlayer->m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(targetPlayer->m_ObjID.m_wIndex, type, payload, 0x1Au);
      }
    }
  }
}

void CPlayer::SendMsg_ApexInform(unsigned __int16 dwRecvSize, char *pMsg)
{
  unsigned __int8 type[2] = {99, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, pMsg, dwRecvSize);
}

void CPlayer::SendMsg_MaxPvpPointInform(int nMax)
{
  char payload[4]{};
  *reinterpret_cast<int *>(payload) = nMax;

  unsigned __int8 type[2] = {59, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 4u);
}

void CPlayer::SendMsg_NewMovePotionResult()
{
  char payload[1]{};
  payload[0] = 1;

  unsigned __int8 type[2] = {17, 45};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}



void CPlayer::SendMsg_Die()
{
  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;

  unsigned __int8 type[2] = {3, 23};
  this->CircleReport(type, payload, 6, true);
}

void CPlayer::SendMsg_BreakdownEquipItem(unsigned __int8 byPartIndex, unsigned __int16 wSerial)
{
  char payload[0x0B]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 6) = this->GetVisualVer();
  payload[8] = static_cast<char>(byPartIndex);
  *reinterpret_cast<unsigned __int16 *>(payload + 9) = wSerial;

  unsigned __int8 type[2] = {5, 19};
  this->CircleReport(type, payload, 11, true);
}

void CPlayer::SendMsg_Notify_Gravity_Stone_Owner_Die()
{
  char payload[5]{};
  payload[0] = static_cast<char>(this->m_ObjID.m_byID);
  *reinterpret_cast<unsigned int *>(payload + 1) = this->m_dwObjSerial;

  unsigned __int8 type[2] = {27, 86};
  this->CircleReport(type, payload, 5, true);
}

void CPlayer::SendMsg_CreateTowerResult(char byErrCode, unsigned int dwTowerObjSerial)
{

  _make_tower_result_zocl packet{};
  packet.byErrCode = byErrCode;
  packet.dwTowerObjSerial = dwTowerObjSerial;
  packet.wLeftFP = static_cast<unsigned __int16>(this->GetFP());

  unsigned __int8 type[2] = {17, 19};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_BackTowerResult(char byErrCode, unsigned __int16 wItemSerial, unsigned __int16 wLeftHP)
{

  _back_tower_result_zocl packet{};
  packet.byErrCode = byErrCode;
  packet.wItemSerial = wItemSerial;
  packet.wLeftHP = wLeftHP;

  unsigned __int8 type[2] = {17, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_CreateTrapResult(char byErrCode, unsigned int dwTrapObjSerial)
{

  _make_trap_result_zocl packet{};
  packet.byErrCode = byErrCode;
  packet.dwTrapObjSerial = dwTrapObjSerial;
  packet.wLeftFP = static_cast<unsigned __int16>(this->GetFP());

  unsigned __int8 type[2] = {17, 28};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_BackTrapResult(char byErrCode)
{
  unsigned __int8 type[2] = {17, 39};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byErrCode, 1u);
}

void CPlayer::SendMsg_DTradeOKResult(char byErrCode)
{
  unsigned __int8 type[2] = {18, 23};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byErrCode, 1u);
}

void CPlayer::SendMsg_DTradeOKInform()
{
  char payload = 0;
  unsigned __int8 type[2] = {18, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &payload, 1u);
}

void CPlayer::SendMsg_DTradeAccomplishInform(bool bSucc, unsigned __int16 wStartSerial)
{

  _d_trade_accomplish_inform_zocl packet{};
  packet.dwDalant = this->m_Param.GetDalant();
  packet.dwGold = this->m_Param.GetGold();
  packet.wStartSerial = wStartSerial;
  packet.bSucc = bSucc;

  unsigned __int8 type[2] = {18, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}



















void CPlayer::SendMsg_NpcQuestListResult(_NPCQuestIndexTempData *pQuestIndexData)
{

  _npc_quest_list_result_zocl packet{};
  packet.byQuestNum = static_cast<unsigned __int8>(pQuestIndexData->nQuestNum);
  const int questCount = std::min<int>(pQuestIndexData->nQuestNum, 30);
  for (int index = 0; index < questCount; ++index)
  {
    packet.QuestIndexList[index] = pQuestIndexData->IndexData[index].dwQuestIndex;
  }

  unsigned __int8 type[2] = {24, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_ResultNpcQuest(char bSucc)
{
  unsigned __int8 type[2] = {24, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &bSucc, 1u);
}

void CPlayer::SendMsg_SkillResult(unsigned __int8 byErrCode, const _CHRID *targetId, unsigned __int8 bySkillIndex, unsigned __int8 nSFLv)
{


  _skill_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  unsigned __int8 selfType[2] = {17, 5};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (!((byErrCode == 0 || byErrCode == 100)
      && (this->m_nCirclePlayerNum <= 500 || !IsSameTargetAsPerformer(this, targetId))))
  {
    return;
  }

  _skill_result_other_zocl toOther{};
  toOther.byRetCode = byErrCode;
  if (targetId)
  {
    toOther.idDster = *targetId;
  }
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.bySkillIndex = bySkillIndex;
  toOther.bySkillLv = nSFLv;

  unsigned __int8 otherType[2] = {17, 6};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ClassSkillResult(unsigned __int8 byErrCode, const _CHRID *targetId, unsigned __int16 wSkillIndex)
{


  _class_skill_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  unsigned __int8 selfType[2] = {17, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (!((byErrCode == 0 || byErrCode == 100)
      && (this->m_nCirclePlayerNum <= 500 || !IsSameTargetAsPerformer(this, targetId))))
  {
    return;
  }

  _class_skill_result_other_zocl toOther{};
  toOther.byRetCode = byErrCode;
  if (targetId)
  {
    toOther.idDster = *targetId;
  }
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.wSkillIndex = wSkillIndex;

  unsigned __int8 otherType[2] = {17, 9};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ForceResult(unsigned __int8 byErrCode, const _CHRID *targetId, _STORAGE_LIST::_db_con *pForceItem, unsigned __int8 nSFLv)
{


  _force_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  if (pForceItem)
  {
    toSelf.dwItemCum = pForceItem->m_dwDur;
  }

  unsigned __int8 selfType[2] = {17, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (!((byErrCode == 0 || byErrCode == 100)
      && (this->m_nCirclePlayerNum <= 500 || !IsSameTargetAsPerformer(this, targetId))))
  {
    return;
  }

  _force_result_other_zocl toOther{};
  toOther.byRetCode = byErrCode;
  if (targetId)
  {
    toOther.idDster = *targetId;
  }
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  if (pForceItem && CPlayer::s_pnLinkForceItemToEffect)
  {
    toOther.byForceIndex = static_cast<unsigned __int8>(CPlayer::s_pnLinkForceItemToEffect[pForceItem->m_wItemIndex]);
  }
  else
  {
    toOther.byForceIndex = static_cast<unsigned __int8>(-1);
  }
  toOther.byForceLv = nSFLv;

  unsigned __int8 otherType[2] = {17, 3};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ThrowSkillResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int8 bySkillIndex)
{
  _throw_skill_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  memcpy_0(&toSelf.idDster, pidDst, sizeof(toSelf.idDster));
  FillThrowEffectList(toSelf.list, &toSelf.byEffectedNum);

  unsigned __int8 selfType[2] = {17, 100};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (byErrCode != 0)
  {
    return;
  }

  _throw_skill_result_other_zocl toOther{};
  toOther.byRetCode = 0;
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.bySkillIndex = bySkillIndex;
  memcpy_0(&toOther.idDster, pidDst, sizeof(toOther.idDster));
  toOther.byEffectedNum = toSelf.byEffectedNum;
  memcpy_0(toOther.list, toSelf.list, 8LL * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 101};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ThrowUnitResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int16 wBulletIndex)
{
  _throw_unit_result_one_zocl toSelf{};
  toSelf.byErrCode = byErrCode;
  memcpy_0(&toSelf.idDster, pidDst, sizeof(toSelf.idDster));
  FillThrowEffectList(toSelf.list, &toSelf.byEffectedNum);

  unsigned __int8 selfType[2] = {17, 103};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    selfType,
    reinterpret_cast<char *>(&toSelf),
    static_cast<unsigned __int16>(sizeof(toSelf)));

  if (byErrCode != 0)
  {
    return;
  }

  _throw_unit_result_other_zocl toOther{};
  toOther.byRetCode = 0;
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.wBulletIndex = wBulletIndex;
  memcpy_0(&toOther.idDster, pidDst, sizeof(toOther.idDster));
  toOther.byEffectedNum = toSelf.byEffectedNum;
  memcpy_0(toOther.list, toSelf.list, 8LL * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 104};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}



void CPlayer::SendMsg_UILock_Init_Result(char resultCode)
{
  char payload[2] = {};
  payload[0] = resultCode;
  unsigned __int8 type[2] = {13, 0x80};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
}

void CPlayer::SendMsg_UILock_Login_Result(char resultCode, char failCount)
{
  char payload[2] = {};
  payload[0] = resultCode;
  payload[1] = failCount;
  unsigned __int8 type[2] = {13, 0x82};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
}

void CPlayer::SendMsg_UILock_Update_Result(char resultCode)
{
  unsigned __int8 type[2] = {13, 0x84};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &resultCode, 1u);
}

void CPlayer::SendMsg_UILock_FindPW_Result(char resultCode, const char *password, char failCount)
{
  char payload[15] = {};
  payload[0] = resultCode;
  payload[1] = failCount;
  if (password)
  {
    strcpy_s(&payload[2], 13uLL, password);
  }

  unsigned __int8 type[2] = {13, 0x87};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 15u);
}

void CPlayer::SendMsg_MineStartResult(unsigned __int8 resultCode)
{
  char payload = static_cast<char>(resultCode);
  unsigned __int8 type[2] = {14, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &payload, 1u);
}

void CPlayer::SendMsg_OreIntoBagResult(char resultCode, unsigned __int16 newSerial, unsigned __int8 lendType, unsigned int lendTime)
{
  char payload[8] = {};
  payload[0] = resultCode;
  *reinterpret_cast<unsigned __int16 *>(&payload[1]) = newSerial;
  payload[3] = static_cast<char>(lendType);
  *reinterpret_cast<unsigned int *>(&payload[4]) = lendTime;

  unsigned __int8 type[2] = {14, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 8u);
}

void CPlayer::SendMsg_OreCuttingResult(unsigned __int8 resultCode, unsigned __int8 leftOreCount, unsigned int consumedDalant)
{
  char payload[11 + 100 * 3] = {};
  payload[0] = static_cast<char>(resultCode);

  if (!resultCode)
  {
    payload[1] = static_cast<char>(leftOreCount);
    *reinterpret_cast<unsigned int *>(&payload[2]) = this->m_Param.GetDalant();
    *reinterpret_cast<unsigned int *>(&payload[6]) = consumedDalant;

    unsigned __int8 resultCount = 0;
    const int maxResKind = GetMaxResKind();
    for (int resIndex = 0; resIndex < maxResKind && resultCount < 100; ++resIndex)
    {
      const unsigned __int16 addedAmount = this->m_Param.m_wCuttingResBuffer[resIndex];
      if (!addedAmount)
      {
        continue;
      }

      const int baseOffset = 11 + resultCount * 3;
      *reinterpret_cast<unsigned __int16 *>(&payload[baseOffset]) = static_cast<unsigned __int16>(resIndex);
      payload[baseOffset + 2] = static_cast<char>(addedAmount);
      ++resultCount;
    }
    payload[10] = static_cast<char>(resultCount);
  }

  const unsigned __int8 resultCount = static_cast<unsigned __int8>(payload[10]);
  const unsigned __int16 payloadLength = static_cast<unsigned __int16>(11 + 3 * resultCount);
  unsigned __int8 type[2] = {14, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, payloadLength);
}

void CPlayer::SendMsg_UILock_Init_Request_ToAccount(
  unsigned int accountSerial,
  unsigned __int16 userIndex,
  const char *password,
  unsigned __int8 hintIndex,
  const char *hintAnswer)
{
  char payload[0x25] = {};
  *reinterpret_cast<unsigned __int16 *>(&payload[0]) = userIndex;
  *reinterpret_cast<unsigned int *>(&payload[2]) = accountSerial;
  strcpy_s(&payload[6], 13uLL, password);
  payload[19] = static_cast<char>(hintIndex);
  strcpy_s(&payload[20], 17uLL, hintAnswer);

  unsigned __int8 type[2] = {1, 15};
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, payload, 0x25u);
}

void CPlayer::SendMsg_UILock_Update_Request_ToAccount(
  unsigned int accountSerial,
  unsigned __int16 userIndex,
  const char *password,
  unsigned __int8 hintIndex,
  const char *hintAnswer)
{
  char payload[0x25] = {};
  *reinterpret_cast<unsigned __int16 *>(&payload[0]) = userIndex;
  *reinterpret_cast<unsigned int *>(&payload[2]) = accountSerial;
  strcpy_s(&payload[6], 13uLL, password);
  payload[19] = static_cast<char>(hintIndex);
  strcpy_s(&payload[20], 17uLL, hintAnswer);

  unsigned __int8 type[2] = {1, 17};
  g_Network.m_pProcess[1]->LoadSendMsg(0, type, payload, 0x25u);
}

char CPlayer::Load(CUserDB *pUser, bool bFirstStart)
{
  _AVATOR_DATA pData{};
  memcpy_0(&pData, &pUser->m_AvatorData, sizeof(pData));

  this->m_id.dwSerial = pData.dbAvator.m_dwRecordNum;
  this->m_pUserDB = nullptr;
  this->m_byUserDgr = pUser->m_byUserDgr;
  this->m_bySubDgr = pUser->m_bySubDgr;
  this->m_bFirstStart = bFirstStart;
  this->m_Param.InitPlayerDB(this);
  this->m_bMapLoading = 0;
  this->m_bOper = 0;
  this->m_bFullMode = pUser->m_bWndFullMode;
  this->m_bCheat_100SuccMake = 0;
  this->m_bCheat_makeitem_no_use_matrial = 0;
  this->m_bCheat_Matchless = 0;
  this->m_nMaxAttackPnt = 0;
  this->m_nAnimusAttackPnt = 0;
  this->m_nTrapMaxAttackPnt = 0;
  this->m_byDamagePart = static_cast<unsigned __int8>(-1);
  this->m_bFreeRecallWaitTime = 0;
  this->m_bFreeSFByClass = 0;
  this->m_bLootFree = 0;
  this->m_bOutOfMap = 0;
  this->m_dwNextTimeDungeonDie = 0;
  this->m_bNeverDie = 0;
  this->m_nVoteSerial = -1;
  this->m_bBlockParty = 0;
  this->m_bBlockWhisper = 0;
  this->m_bBlockTrade = 0;
  this->m_bSpyGM = 0;
  this->m_bTakeGravityStone = 0;
  this->m_bBlockGuildBattleMsg = 0;
  this->m_bInGuildBattle = 0;
  this->m_bNotifyPosition = 0;
  this->m_byGuildBattleColorInx = static_cast<unsigned __int8>(-1);
  this->m_bTakeSoccerBall = 0;
  this->m_pSoccerItem = nullptr;
  this->m_dwSelfDestructionTime = 0;
  this->m_fSelfDestructionDamage = 0.0f;
  this->m_fTalik_DefencePoint = 0.0f;
  this->m_fTalik_AvoidPoint = 0.0f;
  this->m_bCheckMovePacket = 0;
  this->m_bLinkBoardDownload = 0;
  this->m_bSpecialDownload = 0;
  this->m_bQuestDownload = 0;
  this->m_bCumDownload = 0;
  this->m_bForceDownload = 0;
  this->m_bInvenDownload = 0;
  this->m_bBaseDownload = 0;
  this->m_bAMPInvenDownload = 0;
  this->m_bBuddyListDownload = 0;
  this->m_bGuildDownload = 0;
  this->m_bGuildListDownload = 0;
  this->m_byMapInModeBuffer = 0;
  this->m_pBeforeTownCheckMap = nullptr;
  this->m_bCreateComplete = 0;
  this->m_bUpCheckEquipEffect = 1;
  this->m_bDownCheckEquipEffect = 0;
  memset_0(this->m_byEffectEquipCode, 0, sizeof(this->m_byEffectEquipCode));
  this->m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);

  CMapData *map = g_MapOper.GetMap(pData.dbAvator.m_byMapCode);
  const int raceIndex = pData.dbAvator.m_byRaceSexCode / 2;
  if (g_TransportShip[raceIndex].IsMemberBeforeLogoff(pData.dbAvator.m_dwRecordNum))
  {
    map = g_TransportShip[raceIndex].m_pLinkShipMap;
    pData.dbAvator.m_byMapCode = map->m_pMapSet->m_dwIndex;
    g_TransportShip[raceIndex].GetStartPosInShip(pData.dbAvator.m_fStartPos);
  }
  else
  {
    if (!map || !map->IsMapIn(pData.dbAvator.m_fStartPos) || map->m_pMapSet->m_nMapType)
    {
      CMapData *posStartMap = g_MapOper.GetPosStartMap(
        pData.dbAvator.m_byRaceSexCode / 2,
        1,
        pData.dbAvator.m_fStartPos);
      if (!posStartMap)
      {
        g_Main.m_logSystemError.Write(
          "Load() : failure : %s.. no find start position(race:%d, x:%d, y:%d, z:%d)..",
          pUser->m_aszAvatorName,
          pData.dbAvator.m_byRaceSexCode / 2,
          static_cast<int>(pData.dbAvator.m_fStartPos[0]),
          static_cast<int>(pData.dbAvator.m_fStartPos[1]),
          static_cast<int>(pData.dbAvator.m_fStartPos[2]));
        return 0;
      }
      pData.dbAvator.m_byMapCode = posStartMap->m_pMapSet->m_dwIndex;
    }
    map = g_MapOper.GetMap(pData.dbAvator.m_byMapCode);
    if (!map->m_bUse)
    {
      g_Main.m_logSystemError.Write(
        "Load() : failure : %s.. no used map(%s)..",
        pUser->m_aszAvatorName,
        map->m_pMapSet->m_strCode);
      return 0;
    }
  }

  LendItemMng::Instance()->Release(this->m_ObjID.m_wIndex);
  if (this->m_Param.ConvertAvatorDB(&pData)
      && this->m_Param.ConvertGeneralDB(&pData, &pUser->m_AvatorData))
  {
    CUnmannedTraderController::Instance()->Load(
      this->m_ObjID.m_wIndex,
      this->m_id.dwSerial,
      &pData.dbTrade);
    this->m_bLoad = 1;
    this->m_pUserDB = pUser;
    this->m_bMapLoading = 1;
    this->m_dwLastCheckRegionTime = 0;
    this->m_wRegionIndex = -1;
    this->m_wRegionMapIndex = -1;

    const long double exp = this->m_Param.GetExp();
    const int level = this->m_Param.GetLevel();
    const long double limitExp = cStaticMember_Player::Instance()->GetLimitExp(level);
    this->m_dwExpRate = static_cast<int>(exp / limitExp * 1000000.0);

    const unsigned __int8 raceCode = this->m_Param.GetRaceCode();
    this->m_pmMst.Init(&pUser->m_AvatorData.dbStat, raceCode);
    this->m_pmTrd.Init();
    this->m_pmWpn.Init();
    this->m_pmTwr.Init();
    this->m_pmTrp.Init();
    this->m_pmBuddy.Init();
    this->m_pmCryMsg.Init();

    this->CalcAddPointByClass();
    this->m_nMaxPoint[0] = this->_CalcMaxHP();
    this->m_nMaxPoint[1] = this->_CalcMaxFP();
    this->m_nMaxPoint[2] = this->_CalcMaxSP();
    this->CalcEquipMaxDP(1);
    memcpy_0(this->m_nOldPoint, this->m_nMaxPoint, sizeof(this->m_nOldPoint));
    this->m_nOldMaxDP = this->GetMaxDP();

    if (bFirstStart)
    {
      this->SetHP(this->m_nMaxPoint[0], 0);
      this->SetFP(this->m_nMaxPoint[1], 0);
      this->SetSP(this->m_nMaxPoint[2], 0);
      this->SetDP(this->GetMaxDP(), 0);
    }

    this->m_fEquipSpeed = 0.0;
    for (int j = 0; j < 50; ++j)
    {
      if (this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].IsFilled())
      {
        if (this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].wszName[0] == 42)
        {
          this->m_pUserDB->Update_DelBuddy(j);
        }
        this->m_pmBuddy.m_List[j].dwSerial = this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].dwSerial;
        strcpy_0(this->m_pmBuddy.m_List[j].wszName, this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].wszName);
      }
    }

    this->m_NameChangeBuddyInfo.Init();

    const unsigned int mapEnvCode = map->m_Level.GetEnvironment();
    this->SendMsg_MapEnvInform(map->m_pMapSet->m_dwIndex, mapEnvCode);
    this->m_pDHChannel = nullptr;
    if (this->m_pUserDB->m_byUserDgr == 2)
    {
      this->m_bSpyGM = !strncmp(&this->m_pUserDB->m_szAccountID[1], "SPY", 3u);
    }

    _PVPPOINT_LIMIT_DB_BASE *pkInfo = &this->m_pUserDB->m_AvatorData.dbPvpPointLimit;
    const long double pvpPoint = this->m_Param.GetPvPPoint();
    if (this->m_kPvpPointLimiter.Set(pvpPoint, pkInfo))
    {
      this->m_pUserDB->UpdateContUserSave(1);
    }

    for (int k = 0; k < 10; ++k)
    {
      strcpy_0(this->m_pmCryMsg.m_List[k].wszCryMsg, this->m_pUserDB->m_AvatorData.dbBossCry.m_List[k].wszCryMsg);
    }

    _PVP_ORDER_VIEW_DB_BASE *p_dbPvpOrderView = &this->m_pUserDB->m_AvatorData.dbPvpOrderView;
    const long double pvpPointView = this->m_Param.GetPvPPoint();
    if (this->m_kPvpOrderView.SetPvpOrderView(pvpPointView, p_dbPvpOrderView, this))
    {
      this->m_pUserDB->UpdateContUserSave(1);
    }

    for (int m = 0; m < 3; ++m)
    {
      this->m_fGroupMapPoint[m][0] = 0.0;
      this->m_fGroupMapPoint[m][1] = 0.0;
      this->m_byGroupMapPointMapCode[m] = 0;
      this->m_wGroupMapPointLayerIndex[m] = 0;
      this->m_dwLastGroupMapPointTime[m] = 0;
    }

    this->m_byPlusKey = rand();
    this->m_wXorKey = rand();
    this->m_dwMoveCount = 0;
    this->m_dwTargetCount = 0;
    this->m_dwAttackCount = 0;
    this->m_bSFDelayNotCheck = 0;

    this->m_kPvpCashPoint.Init(&this->m_pUserDB->m_AvatorData.dbPvpOrderView);

    _PCBANG_PLAY_TIME *p_dbPlayTimeInPcbang = &this->m_pUserDB->m_AvatorData.dbPlayTimeInPcbang;
    CUserDB *m_pUserDB = this->m_pUserDB;
    this->m_kPcBangCoupon.LoadData(m_pUserDB->m_dwAccountSerial, p_dbPlayTimeInPcbang);

    this->SetLastAttBuff(this->m_pUserDB->m_AvatorData.dbSupplement.bLastAttBuff);

    for (int n = 0; n < 38; ++n)
    {
      this->m_PotionParam.SetPotionActDelay(
        n,
        0,
        this->m_pUserDB->m_AvatorData.dbPotionNextUseTime.dwPotionNextUseTime[n]);
    }

    this->m_bCntEnable = 0;
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "Load() : failure : %d:%s",
    pData.dbAvator.m_dwRecordNum,
    pUser->m_aszAvatorName);
  return 0;
}





void CPlayer::LimLvNpcQuestDelete(unsigned __int8 byQuestDBSlot)
{
  SendMsg_QuestFailure(6u, byQuestDBSlot);
  this->m_QuestMgr.DeleteQuestData(byQuestDBSlot);
  this->m_pUserDB->Update_QuestDelete(byQuestDBSlot);
}

void CPlayer::IncCriEffPvPCashBag(double dAlter)
{
  if (dAlter > 0.0 && this->m_byHSKQuestCode != 100)
  {
    CHolyStone *stone = &g_Stone[static_cast<int>(this->m_Param.GetRaceCode())];
    if (stone->m_bOper && stone->m_pCurMap == this->m_pCurMap)
    {
      this->m_nHSKPvpPoint = static_cast<int>(static_cast<double>(this->m_nHSKPvpPoint) + dAlter);
      SendMsg_HSKQuestActCum();
      this->m_pUserDB->m_AvatorData.m_iPvpPoint = this->m_nHSKPvpPoint;
    }
  }
}







CPvpPointLimiter *CPlayer::GetPvpPointLimiter(CPvpPointLimiter *result)
{
  *result = m_kPvpPointLimiter;
  return result;
}

CPvpOrderView *CPlayer::GetPvpOrderView()
{
  return &m_kPvpOrderView;
}



void CPlayer::SendMsg_FcitemInform(unsigned __int16 wItemSerial, unsigned int dwNewStat)
{
  char szMsg[2]; // [rsp+34h] [rbp-44h] BYREF
  unsigned int v7; // [rsp+36h] [rbp-42h]
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  *(_WORD *)szMsg = wItemSerial;
  v7 = dwNewStat;
  pbyType[0] = 3;
  pbyType[1] = 44;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 6u);
}

#if 0 // duplicate implementation exists in CPlayer.cpp

#endif

void CPlayer::SendMsg_InsertQuestFailure(char byEventType, unsigned int dwEventIndex, unsigned __int8 byEventNodeIndex)
{
  char szMsg; // [rsp+34h] [rbp-44h] BYREF
  unsigned int dwIndex; // [rsp+35h] [rbp-43h]
  unsigned __int8 byNode; // [rsp+39h] [rbp-3Fh]
  unsigned __int8 pbyType[36]{}; // [rsp+54h] [rbp-24h] BYREF

  szMsg = byEventType;
  dwIndex = dwEventIndex;
  byNode = byEventNodeIndex;
  pbyType[0] = 24;
  pbyType[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, &szMsg, 6u);
}

#if 0 // duplicate implementation exists in CPlayer.cpp

#endif





void CPlayer::AlterPvPCashBag(long double dAlter, int IOCode)
{
  double PvpCash; // [rsp+20h] [rbp-18h]
  long double dAltera; // [rsp+28h] [rbp-10h]

  PvpCash = this->m_kPvpOrderView.GetPvpCash();
  dAltera = PvpCash + dAlter;
  if ( PvpCash + dAlter > 999999.0 )
    dAltera = 999999.0;
  if ( dAltera < -999999.0 )
    dAltera = -999999.0;
  if ( this->GetLevel() > 39 && this->m_Param.m_pClassData->m_nGrade >= 1 )
  {
    this->m_kPvpOrderView.SetPvpCash(dAltera);
    if ( dAltera > PvpCash && dAlter > 0.0 )
      this->IncCriEffPvPCashBag(dAlter);
    const long double currentCash = this->m_kPvpOrderView.GetPvpCash();
    if ( PvpCash != currentCash )
    {
      this->m_kPvpOrderView.UpdatePvpCash(dAltera);
      this->SendMsg_AlterPvPCash(IOCode);
    }
  }
}

#if 0 // duplicate implementation exists in CPlayer.cpp

#endif

#if 0 // duplicate implementation exists in CPlayer.cpp

#endif

float CPlayer::GetAddSpeed()
{
  const float effectSpeed = this->m_EP.GetEff_Plus(20) * 100.0f;
  const float baseSpeed = static_cast<float>(static_cast<__int16>(static_cast<int>(effectSpeed)));

  if (this->m_byMoveType == 2 && this->EquipItemSFAgent.IsUseBooster())
  {
    const float boosterSpeed = this->EquipItemSFAgent.GetBoosterAddSpeed() * 100.0f;
    return static_cast<float>(static_cast<__int16>(static_cast<int>(boosterSpeed)));
  }

  return baseSpeed;
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

  const unsigned int lastYearMonth = lastConnTime / 0xF4240;
  const unsigned int lastYear = lastYearMonth / 0x64;
  const unsigned int lastMonth = lastYearMonth % 0x64;
  const unsigned int lastDay = lastConnTime % 0xF4240 / 0x2710;

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

void CPlayer::BreakCloakBooster()
{
  this->EquipItemSFAgent.ReleaseSFCont(7);
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

void CPlayer::CheckUnitCutTime()
{
  if (!this->m_pUserDB)
  {
    return;
  }

  const unsigned int now = GetKorLocalTime();
  for (int index = 0; index < 4; ++index)
  {
    _UNIT_DB_BASE::_LIST *unit = &this->m_Param.m_UnitDB.m_List[index];
    if (unit->byFrame != 255 && unit->dwCutTime && now - unit->dwCutTime < 5)
    {
      unit->nPullingFee = 0;
      SendMsg_UnitAlterFeeInform(index, 0);
      this->m_pUserDB->Update_UnitData(index, unit);

      float newPos[3]{this->m_fCurPos[0], this->m_fCurPos[1], this->m_fCurPos[2]};
      this->m_pCurMap->GetRandPosInRange(this->m_fCurPos, 20, newPos);
      pc_UnitDeliveryRequest(index, nullptr, 0, newPos, 0);
      break;
    }
  }

  for (int index = 0; index < 4; ++index)
  {
    _UNIT_DB_BASE::_LIST *unit = &this->m_Param.m_UnitDB.m_List[index];
    if (unit->byFrame != 255 && unit->dwCutTime)
    {
      unit->dwCutTime = 0;
      this->m_pUserDB->Update_UnitData(index, unit);
    }
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

void CPlayer::SendMsg_EconomyHistoryInform()
{
  char msg[0x48]{};
  _economy_history_data *history = eGetGuideHistory();

  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < 12; ++index)
    {
      *reinterpret_cast<unsigned __int16 *>(&msg[24 * race + 2 * index]) = history[index].wEconomyGuide[race];
    }
  }

  unsigned __int8 type[2] = {12, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 0x48u);
}

void CPlayer::SendMsg_EconomyRateInform(char bStart)
{
_economy_rate_inform_zocl msg{};

  msg.bStart = bStart;
  msg.fPayExgRate = static_cast<float>(eGetRate(this->m_Param.GetRaceCode()));
  msg.fTexRate = eGetTex(this->m_Param.GetRaceCode());
  msg.wMgrValue = static_cast<unsigned __int16>(eGetMgrValue());
  msg.fOreSellRate = eGetOreRate(this->m_Param.GetRaceCode());

  if (!bStart)
  {
    for (int race = 0; race < 3; ++race)
    {
      msg.wEconomyGuide[race] = eGetGuide(race);
    }
  }

  unsigned __int8 type[2] = {12, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x15u);
}

void CPlayer::SendMsg_SFDelayRequest()
{
  const DWORD now = timeGetTime();
  _sf_delay_download_result_zocl msg{};

  int effCount = 0;
  int masCount = 0;
  for (int index = 0; index < 10; ++index)
  {
    if (this->m_AttDelayChker.EFF[index].byEffectCode != 255)
    {
      msg.EFF[effCount].byEffectCode = this->m_AttDelayChker.EFF[index].byEffectCode;
      msg.EFF[effCount].wEffectIndex = this->m_AttDelayChker.EFF[index].wEffectIndex;
      msg.EFF[effCount].dwRemainTime = this->m_AttDelayChker.EFF[index].dwNextTime - now;
      ++effCount;
    }

    if (this->m_AttDelayChker.MAS[index].byEffectCode != 255)
    {
      msg.MAS[masCount].byEffectCode = this->m_AttDelayChker.MAS[index].byEffectCode;
      msg.MAS[masCount].byMastery = this->m_AttDelayChker.MAS[index].byMastery;
      msg.MAS[masCount].dwRemainTime = this->m_AttDelayChker.MAS[index].dwNextTime - now;
      ++masCount;
    }
  }

  unsigned __int8 type[2] = {3, 56};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x82u);
}

void CPlayer::SendMsg_PotionDelayTime(unsigned int *pdwPotionNextUseTime, unsigned int dwCurTime)
{
  _potion_delay_time_information_zocl msg{};

  msg.nMaxNum = 38;
  for (int index = 0; index < 38; ++index)
  {
    const int remain = static_cast<int>(pdwPotionNextUseTime[index] - dwCurTime);
    msg.dwPotionDelayTime[index] = (remain <= 0) ? 0u : static_cast<unsigned int>(remain);
  }

  unsigned __int8 type[2] = {3, 60};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0x9Cu);
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
  unsigned __int8 type[2] = {59, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&bExcepted), 4u);
}

void CPlayer::SendMsg_MadeTrapNumInform(char byNum)
{
  unsigned __int8 type[2] = {17, 29};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byNum, 1u);
}

void CPlayer::SendMsg_TowerContinue(unsigned __int16 wItemSerial, CGuardTower *pTwr)
{
_continue_tower_inform msg{};

  msg.wItemSerial = wItemSerial;
  msg.wTwrRecIndex = static_cast<unsigned __int16>(pTwr->m_pRecordSet->m_dwIndex);
  msg.wTwrIndex = pTwr->m_ObjID.m_wIndex;
  msg.dwTwrSerial = pTwr->m_dwObjSerial;

  unsigned __int8 type[2] = {17, 30};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), 0xAu);
}

void CPlayer::SendMsg_ReEnterAsk(unsigned __int16 wChannelIndex, unsigned int dwChannelSerial)
{
  _darkhole_ask_reenter_inform_zocl msg{};
  msg.wChannelIndex = wChannelIndex;
  msg.dwChannelSerial = dwChannelSerial;

  unsigned __int8 type[2] = {35, static_cast<unsigned __int8>(-47)};
  const unsigned __int16 len = msg.size();
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
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
      intoMap = g_HolySys.m_HolyKeeperData.pCreateMap;
      portalDummy = g_HolySys.m_pPortalDummy[this->m_Param.GetRaceCode()];
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

void CPlayer::SendMsg_JadeEffectErr(char byErrorCode)
{
  unsigned __int8 type[2] = {59, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byErrorCode, 1u);
}

void CPlayer::SendMsg_DTradeAskResult(char byErrCode)
  {
    unsigned __int8 type[2] = {18, 2};
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byErrCode, 1u);
  }

void CPlayer::SendMsg_AlterTownOrField()
{
  char szMsg[32]{};
  unsigned __int8 pbyType[36]{};

  szMsg[0] = this->m_byPosRaceTown;
  pbyType[0] = 4;
  pbyType[1] = 34;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}

void CPlayer::SendMsg_AlterRegionInform(__int16 nRegionIndex)
{
  char szMsg[32]{};
  unsigned __int8 pbyType[36]{};

  *reinterpret_cast<__int16 *>(szMsg) = nRegionIndex;
  pbyType[0] = 8;
  pbyType[1] = 6;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 2u);
}

void CPlayer::SendMsg_GestureInform(unsigned __int8 byGestureType)
{
_gesture_inform_zocl msg{};

  msg.dwActorSerial = this->m_dwObjSerial;
  msg.byGestureType = byGestureType;

  unsigned __int8 type[2] = {13, 23};
  CircleReport(type, reinterpret_cast<char *>(&msg), 5, true);
}

void CPlayer::UpdateLastCriTicket(
  unsigned __int16 byCurrentYear,
  unsigned __int8 byCurrentMonth,
  unsigned __int8 byCurrentDay,
  unsigned __int8 byCurrentHour,
  unsigned __int8 byNumOfTime)
{
  this->m_MinigTicket.SetLastCriTicket(byCurrentYear, byCurrentMonth, byCurrentDay, byCurrentHour, byNumOfTime);
  if (this->m_pUserDB)
  {
    const unsigned int lastTicket = this->m_MinigTicket.GetLastCriTicket();
    this->m_pUserDB->Update_TakeLastCriTicket(lastTicket);
  }
}

void CPlayer::_set_db_sf_effect(_SFCONT_DB_BASE *pDBBase)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  const unsigned int curTime = _sf_continous::GetSFContCurTime();
  for (int group = 0; group < 2; ++group)
  {
    for (int index = 0; index < 8; ++index)
    {
      _SFCONT_DB_BASE::_LIST *dbEntry = &pDBBase->m_List[group][index];
      if (!dbEntry->IsFilled())
      {
        continue;
      }

      _sf_continous *cont = &this->m_SFCont[group][index];
      const unsigned __int8 effectCode = static_cast<unsigned __int8>(dbEntry->GetEffectCode());
      const unsigned __int16 effectIndex = static_cast<unsigned __int16>(dbEntry->GetEffectIndex());
      const unsigned __int8 effectLevel = static_cast<unsigned __int8>(dbEntry->GetLv() + 1);
      unsigned int startSec = curTime;
      unsigned __int16 durSec = static_cast<unsigned __int16>(dbEntry->GetLeftTime());
      const unsigned __int8 order = static_cast<unsigned __int8>(dbEntry->GetOrder());

      if (curTime > order)
      {
        startSec = curTime - order;
        durSec = static_cast<unsigned __int16>(durSec + order);
      }

      bool invalid = false;
      if (effectCode < 4u)
      {
        if (g_Main.m_tblEffectData[effectCode].GetRecord(effectIndex))
        {
          if (effectLevel > 7u)
          {
            invalid = true;
          }
        }
        else
        {
          invalid = true;
        }
      }
      else
      {
        invalid = true;
      }

      if (invalid)
      {
        this->m_pUserDB->Update_SFContDelete(group, index);
        const char *charName = this->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "%s: error stored effect, code: %d, idx: %d: lv: %d",
          charName,
          effectCode,
          effectIndex,
          effectLevel);
        continue;
      }

      this->_set_sf_cont(cont, effectCode, effectIndex, effectLevel, startSec, durSec, 0);
      this->EquipItemSFAgent.StartContSF(cont);

      _base_fld *record = g_Main.m_tblEffectData[3].GetRecord("17");
      if (record && cont->m_byEffectCode == 3 && cont->m_wEffectIndex == record->m_dwIndex)
      {
        this->m_bAfterEffect = 1;
      }

      SendMsg_StartContSF(cont);
      this->m_bLastContEffectUpdate = 1;
    }
  }
}

unsigned int CPlayer::_check_mastery_cum_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex)
{
  _base_fld *curRecord = nullptr;
  _base_fld *baseRecord = nullptr;

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
      curRecord = CPlayer::s_tblLimMasteryCumContinue[raceCode][curClass->m_nClass].GetRecord(recordIndex);
    }
    else
    {
      curRecord = CPlayer::s_tblLimMasteryCum[raceCode][curClass->m_nClass].GetRecord(recordIndex);
      baseRecord = CPlayer::s_tblLimMasteryCum[raceCode][baseClass->m_nClass].GetRecord(recordIndex);
    }
  }
  else
  {
    curRecord = CPlayer::s_tblLimMasteryCum[raceCode][curClass->m_nClass].GetRecord(recordIndex);
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
      curLimit = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[4 * byIndex]);
      if (baseRecord)
      {
        baseLimit = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[4 * byIndex]);
      }
      break;
    case 1:
      curLimit = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[12]);
      if (baseRecord)
      {
        baseLimit = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[12]);
      }
      break;
    case 2:
      curLimit = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[16]);
      if (baseRecord)
      {
        baseLimit = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[16]);
      }
      break;
    case 3:
      curLimit = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[4 * byIndex + 32]);
      if (baseRecord)
      {
        baseLimit = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[4 * byIndex + 32]);
      }
      break;
    case 4:
      curLimit = *(reinterpret_cast<unsigned int *>(&curRecord[2].m_dwIndex) + byIndex);
      if (baseRecord)
      {
        baseLimit = *(reinterpret_cast<unsigned int *>(&baseRecord[2].m_dwIndex) + byIndex);
      }
      break;
    case 5:
      curLimit = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[4 * byIndex + 20]);
      if (baseRecord)
      {
        baseLimit = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[4 * byIndex + 20]);
      }
      break;
    case 6:
      curLimit = *reinterpret_cast<unsigned int *>(&curRecord[1].m_strCode[8]);
      if (baseRecord)
      {
        baseLimit = *reinterpret_cast<unsigned int *>(&baseRecord[1].m_strCode[8]);
      }
      break;
    default:
      break;
  }

  return (curLimit <= baseLimit) ? baseLimit : curLimit;
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
      float effHave = this->m_EP.GetEff_Have(6);
      const float eff82 = this->m_EP.GetEff_Have(82);
      effHave = (effHave == 0.0f) ? eff82 : effHave + eff82;
      if (effHave > 1.0f)
      {
        alterCum =
          static_cast<int>(static_cast<float>(alterCum) + static_cast<float>(alterBase) * (effHave - 1.0f));
      }
    }
    if (byMasteryClass != 6 || this->m_Param.GetRaceCode() != 1)
    {
      const float rate = this->m_EP.GetEff_Rate(35);
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
    _base_fld *record = _MASTERY_PARAM::s_pSkillData->GetRecord(byIndex);
    if (record)
    {
      const unsigned __int8 skillIndex = static_cast<unsigned __int8>(record[1].m_strCode[4]);
      if (skillIndex >= 8u)
      {
        return;
      }
      checkIndex = skillIndex;
    }
  }

  const unsigned int limit = _check_mastery_cum_lim(byMasteryClass, checkIndex);
    const int current = static_cast<int>(this->m_pmMst.GetCumPerMast(byMasteryClass, checkIndex));
  unsigned int remain = (limit > static_cast<unsigned int>(current)) ? (limit - current) : 0u;
  if (alterCapped > remain)
  {
    alterCapped = remain;
  }
  if (!alterCapped)
  {
    return;
  }

  const int statIndex = _STAT_DB_BASE::GetStatIndex(byMasteryClass, byIndex);
  if (this->m_pmMst.m_BaseCum.m_dwDamWpCnt[statIndex] > 0xEE6B2800)
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

void CPlayer::SendMsg_DarkHoleOpenFail(unsigned int n, unsigned __int8 byErrCode)
{
  _darkhole_open_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wHoleIndex = static_cast<unsigned __int16>(-1);
  msg.dwHoleSerial = static_cast<unsigned int>(-1);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-54)};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_DarkHoleOpenResult(
  unsigned int n,
  int bPartyOnly,
  unsigned __int8 byErrCode,
  unsigned __int16 wHoleIndex,
  unsigned int dwHoleSerial)
{
  _darkhole_open_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wHoleIndex = wHoleIndex;
  msg.dwHoleSerial = dwHoleSerial;

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-54)};
  if (bPartyOnly)
  {
    CPlayer *partyMembers[8]{};
    const unsigned __int8 memberCount = _GetPartyMemberInCircle(partyMembers, 8, true);
    for (unsigned __int8 j = 0; j < memberCount; ++j)
    {
      CPlayer *member = partyMembers[j];
      if (member && member->m_bLive)
      {
        g_Network.m_pProcess[0]->LoadSendMsg(
          member->m_id.wIndex,
          type,
          reinterpret_cast<char *>(&msg),
          msg.size());
      }
    }
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), msg.size());
    CircleReport(type, reinterpret_cast<char *>(&msg), msg.size(), false);
  }
}

void CPlayer::SendMsg_EnterDarkHole(unsigned __int8 byErrCode, unsigned int dwHoleSerial)
{
  _darkhole_enter_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.dwHoleSerial = dwHoleSerial;
  msg.byMapCode = static_cast<unsigned __int8>(m_pCurMap->m_pMapSet->m_dwIndex);
  FloatToShort(m_fCurPos, msg.zPos, 3);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-52)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_GiveupDarkHole(unsigned __int8 byErrCode)
{
  _darkhole_giveup_out_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wTarMapIndex = m_pCurMap->m_pMapSet->m_dwIndex;
  FloatToShort(m_fCurPos, msg.zTarPos, 3);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-50)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_ClearDarkHole(unsigned __int8 byErrCode)
{
  _darkhole_clear_out_result_zocl msg{};
  msg.byRetCode = byErrCode;
  msg.wTarMapIndex = m_pCurMap->m_pMapSet->m_dwIndex;
  FloatToShort(m_fCurPos, msg.zTarPos, 3);

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-48)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

void CPlayer::SendMsg_ReEnterDarkHoleResult(unsigned __int8 byRetCode)
{
  _darkhole_answer_reenter_result_zocl msg{};
  msg.byRetCode = byRetCode;

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-45)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), msg.size());
}

int CPlayer::GetRewardItems_DarkDungeon(
  _dh_reward_sub_setup *pSetup,
  _STORAGE_LIST::_db_con *pItems,
  bool bRealBoss)
{
  if (!pSetup || !pItems)
  {
    return 0;
  }

  unsigned int minMatchedPercent = 0x7FFF8000u;
  int rewardCount = 0;

  const int high = rand() << 16;
  unsigned int drawValue = static_cast<unsigned int>(rand() + high);
  if (!bRealBoss)
  {
    drawValue *= 10;
  }

  for (int index = 0; index < pSetup->nItemNum; ++index)
  {
    const unsigned int percent = pSetup->m_dwGivePercent[index];
    if (drawValue <= percent)
    {
      if (minMatchedPercent > percent)
      {
        minMatchedPercent = percent;
      }
    }
  }

  for (int index = 0; index < pSetup->nItemNum; ++index)
  {
    if (pSetup->m_dwGivePercent[index] == minMatchedPercent)
    {
      const unsigned __int8 raceSexCode = m_pUserDB->m_AvatorData.dbAvator.m_byRaceSexCode;
      _STORAGE_LIST::_db_con *candidate = pSetup->Item[index];
      if (candidate
          && IsItemEquipCivil(candidate->m_byTableCode, candidate->m_wItemIndex, raceSexCode))
      {
        memcpy_0(&pItems[rewardCount], candidate, sizeof(_STORAGE_LIST::_db_con));
        ++rewardCount;
      }
    }
  }

  return rewardCount;
}

void CPlayer::Reward_DarkDungeon(
  _dh_reward_sub_setup *pSetup,
  char *pszTitle,
  bool bRealBoss,
  _STORAGE_LIST::_db_con *pItem,
  int *bIsRewarded)
{
  if (!pSetup || !bIsRewarded)
  {
    return;
  }

  _STORAGE_LIST::_db_con rewardCandidates[128]{};
  const long double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
  const int rewardCandidateCount = GetRewardItems_DarkDungeon(pSetup, rewardCandidates, bRealBoss);

  if (rewardCandidateCount > 0 && g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) != 99)
  {
    _STORAGE_LIST::_db_con rewardItem{};
    const int pick = rand() % rewardCandidateCount;
    memcpy_0(&rewardItem, &rewardCandidates[pick], sizeof(rewardItem));

    if (m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
    {
      CreateItemBox(
        &rewardItem,
        this,
        static_cast<unsigned int>(-1),
        false,
        nullptr,
        3u,
        m_pCurMap,
        m_wMapLayerIndex,
        m_fCurPos,
        false);

      char historyClause[132]{};
      sprintf(historyClause, "Quest G (%s)", pszTitle ? pszTitle : "");
      CPlayer::s_MgrItemHistory.reward_add_item(
        m_ObjID.m_wIndex,
        historyClause,
        &rewardItem,
        m_szItemHistoryFileName);
    }
    else
    {
      rewardItem.m_wSerial = m_Param.GetNewItemSerial();

      const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(rewardItem.m_byTableCode, rewardItem.m_wItemIndex);
      if (timeRec && timeRec->m_nCheckType)
      {
        __time32_t now[11]{};
        _time32(now);
        rewardItem.m_byCsMethod = static_cast<unsigned __int8>(timeRec->m_nCheckType);
        rewardItem.m_dwT = timeRec->m_nUseTime + now[0];
        rewardItem.m_dwLendRegdTime = now[0];
      }

      if (!Emb_AddStorage(0, &rewardItem, false, true))
      {
        CPlayer::s_MgrItemHistory.add_storage_fail(
          m_ObjID.m_wIndex,
          &rewardItem,
          "CPlayer::Reward_DarkDungeon - Emb_AddStorage() Fail",
          m_szItemHistoryFileName);
      }
      else
      {
        SendMsg_RewardAddItem(&rewardItem, 2u);

        char historyClause[160]{};
        sprintf(historyClause, "Quest (%s)", pszTitle ? pszTitle : "");
        CPlayer::s_MgrItemHistory.reward_add_item(
          m_ObjID.m_wIndex,
          historyClause,
          &rewardItem,
          m_szItemHistoryFileName);
      }
    }

    SendMsg_FanfareItem(4u, &rewardItem, nullptr);

    if (pItem)
    {
      *bIsRewarded = 1;
      pItem->m_byTableCode = rewardItem.m_byTableCode;
      pItem->m_wItemIndex = rewardItem.m_wItemIndex;
    }
  }
  else if (rewardCandidateCount <= 0)
  {
    *bIsRewarded = 0;
  }

  if (pSetup->dwDalant)
  {
    AddDalant(pSetup->dwDalant, true);
    SendMsg_ExchangeMoneyResult(0);

    const unsigned int addDalant =
      static_cast<unsigned int>(static_cast<double>(static_cast<int>(pSetup->dwDalant)) * static_cast<double>(playerPenalty));
    const unsigned int sumGold = m_Param.GetGold();
    const unsigned int sumDalant = m_Param.GetDalant();

    char historyClause[132]{};
    sprintf(historyClause, "Quest (%s)", pszTitle ? pszTitle : "");
    CPlayer::s_MgrItemHistory.reward_add_money(
      m_ObjID.m_wIndex,
      historyClause,
      addDalant,
      0,
      sumDalant,
      sumGold,
      m_szItemHistoryFileName);

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr *moneySupplyMgr = CMoneySupplyMgr::Instance();
      moneySupplyMgr->UpdateQuestRewardMoneyData(
        static_cast<unsigned __int8>(m_Param.GetRaceCode()),
        level,
        const_cast<char *>(m_Param.GetPtrCurClass()->m_strCode),
        pSetup->dwDalant);
    }
  }

  if (pSetup->dExp > 0.0)
  {
    AlterExp(static_cast<double>(pSetup->dExp), true, false, true);
  }

  if (pSetup->dwPvp)
  {
    AlterPvPPoint(static_cast<double>(static_cast<int>(pSetup->dwPvp)), quest_inc, static_cast<unsigned int>(-1));
  }
}

void CPlayer::SendMsg_DarkHoleRewardMessage(
  _STORAGE_LIST::_db_con *pItem,
  unsigned int dwMemberIndex,
  int isRewarded)
{

  _darkhole_reward_message_inform_zocl msg{};
  msg.bIsRewarded = isRewarded;
  msg.dwPartyMemberIndex = dwMemberIndex;
  if (pItem)
  {
    msg.byTableCode = pItem->m_byTableCode;
    msg.wItemIndex = pItem->m_wItemIndex;
  }

  unsigned __int8 type[2]{35, static_cast<unsigned __int8>(-44)};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::pc_DarkHoleOpenRequest(unsigned __int16 dwItemSerial)
{
  unsigned __int8 errCode = 0;
  _STORAGE_LIST::_db_con *item = nullptr;
  _base_fld *record = nullptr;
  CDarkHole *hole = nullptr;
  _dh_quest_setup *questSetup = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 28;
  }
  else if (IsExistDarkHoleOpenGate())
  {
    errCode = 3;
  }
  else if (m_pmTrd.bDTradeMode || m_bCorpse)
  {
    errCode = 16;
  }
  else if (m_bInGuildBattle)
  {
    errCode = 20;
  }
  else
  {
    hole = SerarchEmptyDarkHole();
    if (hole)
    {
      item = m_Param.m_dbInven.GetPtrFromSerial(dwItemSerial);
      if (item)
      {
        if (item->m_bLock)
        {
          errCode = 13;
        }
        else if (item->m_byTableCode == 23)
        {
          record = g_Main.m_tblItemData[23].GetRecord(item->m_wItemIndex);
          if (record)
          {
            questSetup = g_DarkHoleQuest.GetQuestSetupPtr(item->m_wItemIndex);
            if (questSetup)
            {
              if (g_DarkHoleQuest.CanOpenChannel(record->m_dwIndex))
              {
                if (!questSetup->bPartyOnly || m_pPartyMgr->IsPartyMode())
                {
                  const int levelMin = GetLevel();
                  if (levelMin < questSetup->nLimitLvMin || GetLevel() > questSetup->nLimitLvMax)
                  {
                    errCode = 19;
                  }
                }
                else
                {
                  errCode = 17;
                }
              }
              else
              {
                errCode = 3;
              }
            }
            else
            {
              errCode = 100;
            }
          }
          else
          {
            errCode = 100;
          }
        }
        else
        {
          errCode = 2;
        }
      }
      else
      {
        errCode = 1;
      }
    }
    else
    {
      errCode = 3;
    }
  }

  unsigned int holeSerial = static_cast<unsigned int>(-1);
  if (!errCode)
  {
    _darkhole_create_setdata param;
    param.m_pRecordSet = record;
    param.m_pMap = m_pCurMap;
    param.m_nLayerIndex = m_wMapLayerIndex;
    m_pCurMap->GetRandPosInRange(m_fCurPos, 10, param.m_fStartPos);
    param.pOpener = this;
    if (hole->Create(&param))
    {
      Emb_AlterDurPoint(0, item->m_byStorageIndex, -1, false, false);
      CPlayer::s_MgrItemHistory.consume_del_item(
        m_ObjID.m_wIndex,
        item,
        m_szItemHistoryFileName);
      const char *holeCode = record[2].m_strCode;
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logDungeon.Write("OPEN: %s, DARKHOLE:%s", charName, holeCode);
      holeSerial = hole->m_dwObjSerial;
      m_pPartyMgr->m_pDarkHole = hole;
    }
    else
    {
      errCode = 4;
    }
  }

  if (errCode)
  {
    SendMsg_DarkHoleOpenFail(m_id.wIndex, errCode);
  }
  else
  {
    SendMsg_DarkHoleOpenResult(
      m_id.wIndex,
      questSetup->bPartyOnly,
      0,
      hole->m_ObjID.m_wIndex,
      holeSerial);
  }
}

void CPlayer::pc_DarkHoleEnterRequest(unsigned __int16 wHoleIndex, unsigned int dwHoleSerial)
{
  unsigned __int8 errCode = 0;
  _dh_quest_setup *questSetup = nullptr;
  CDarkHole *hole = &g_DarkHole[wHoleIndex];
  _ENTER_DUNGEON_NEW_POS newPos{};

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 28;
  }
  else if (m_bInGuildBattle)
  {
    errCode = 20;
  }
  else if (hole->m_bLive && hole->m_dwObjSerial == dwHoleSerial && hole->m_pChannel)
  {
    if (m_pCurMap->m_pMapSet->m_nMapType)
    {
      errCode = 11;
    }
    else if (GetCurSecNum() == -1 || m_bMapLoading)
    {
      errCode = 8;
    }
    else if (hole->IsNewEnterAbleNum())
    {
      if (hole->IsNewEnterAblePlayer(this))
      {
        if (hole->GetEnterNewPos(&newPos))
        {
          questSetup = hole->m_pChannel->m_pQuestSetup;
          if (!questSetup)
          {
            return;
          }
          const int levelMin = GetLevel();
          if (levelMin < questSetup->nLimitLvMin || GetLevel() > questSetup->nLimitLvMax)
          {
            errCode = 19;
          }
        }
        else
        {
          errCode = 7;
        }
      }
      else
      {
        errCode = 6;
      }
    }
    else
    {
      errCode = 18;
    }
  }
  else
  {
    errCode = 5;
  }

  if (!errCode)
  {
    CMapData *oldMap = m_pCurMap;
    float oldPos[3]{m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};
    const unsigned __int16 oldLayer = m_wMapLayerIndex;
    CMapData *intoMap = g_MapOper.GetMap(newPos.byMapCode);
    OutOfMap(intoMap, newPos.wLayerIndex, 5u, newPos.fPos);
    hole->EnterPlayer(this, oldMap, oldLayer, oldPos, false);
    if (hole->m_pRecordSet)
    {
      const char *holeCode = hole->m_pRecordSet[2].m_strCode;
      const char *openerName = hole->m_aszOpenerName;
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logDungeon.Write(
        "ENTER: %s, (OPENER:%s, DARKHOLE:%s)",
        charName,
        openerName,
        holeCode);
    }
    else
    {
      const char *openerName = hole->m_aszOpenerName;
      const char *charName = m_Param.GetCharNameA();
      g_Main.m_logDungeon.Write(
        "ENTER: %s, (OPENER:%s, DARKHOLE:%s)",
        charName,
        openerName,
        "InvalidDungenName");
    }
  }

  SendMsg_EnterDarkHole(errCode, dwHoleSerial);
}

void CPlayer::pc_DarkHoleGiveupOutRequest()
{
  unsigned __int8 errCode = 0;
  _dh_player_mgr *playerInfo = nullptr;

  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (m_pDHChannel)
    {
      playerInfo = m_pDHChannel->GetPlayerInfo(m_dwObjSerial);
      if (!playerInfo)
      {
        errCode = 9;
      }
    }
    else
    {
      errCode = 9;
    }
  }
  else
  {
    errCode = 9;
  }

  if (!errCode)
  {
    OutOfMap(playerInfo->LastPos.pMap, 0, 4u, playerInfo->LastPos.fPos);
    m_pDHChannel->ClearMember(this, false, nullptr);
  }
  SendMsg_GiveupDarkHole(errCode);
}

void CPlayer::pc_DarkHoleClearOutRequest()
{
  unsigned __int8 errCode = 0;
  _dh_player_mgr *playerInfo = nullptr;

  if (m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    if (m_pDHChannel)
    {
      playerInfo = m_pDHChannel->GetPlayerInfo(m_dwObjSerial);
      if (!playerInfo)
      {
        errCode = 9;
      }
    }
    else
    {
      errCode = 9;
    }
  }
  else
  {
    errCode = 9;
  }

  if (!errCode)
  {
    OutOfMap(playerInfo->LastPos.pMap, 0, 4u, playerInfo->LastPos.fPos);
    m_pDHChannel->ClearMember(this, false, nullptr);
  }
  SendMsg_ClearDarkHole(errCode);
}

void CPlayer::pc_DarkHoleAnswerReenterRequest(
  bool bEnter,
  unsigned __int16 wChannelIndex,
  unsigned int dwChannelSerial)
{
unsigned __int8 errCode = 0;
  _ENTER_DUNGEON_NEW_POS newPos{};
  CDarkHoleChannel *channel = g_DarkHoleQuest.GetChannel(wChannelIndex);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    errCode = 28;
  }
  else if (m_pDHChannel)
  {
    errCode = 15;
  }
  else if (channel->IsFill() && channel->m_dwChannelSerial == dwChannelSerial)
  {
    if (channel->IsReEnterable(m_dwObjSerial))
    {
      if (m_pCurMap->m_pMapSet->m_nMapType)
      {
        errCode = 11;
      }
      else if (GetCurSecNum() == -1 || m_bMapLoading)
      {
        errCode = 8;
      }
      else if (IsRidingUnit())
      {
        errCode = 12;
      }
      else
      {
        const unsigned int memberNum = channel->GetCurrentMemberNum();
        if (memberNum < channel->m_pQuestSetup->dwMaxMember)
        {
          if (!channel->GetEnterNewPos(&newPos))
          {
            errCode = 7;
          }
        }
        else
        {
          errCode = 18;
        }
      }
    }
    else
    {
      errCode = 5;
    }
  }
  else
  {
    errCode = 5;
  }

  if (!errCode)
  {
    CMapData *oldMap = m_pCurMap;
    float oldPos[3]{m_fCurPos[0], m_fCurPos[1], m_fCurPos[2]};
    const unsigned __int16 oldLayer = m_wMapLayerIndex;
    CMapData *intoMap = g_MapOper.GetMap(newPos.byMapCode);
    OutOfMap(intoMap, newPos.wLayerIndex, 5u, newPos.fPos);
    channel->PushMember(this, true, oldMap, oldLayer, oldPos);
    const char *questTitle = channel->m_pQuestSetup->szQuestTitle;
    const char *openerName = channel->m_aszOpenerName;
    const char *charName = m_Param.GetCharNameA();
    g_Main.m_logDungeon.Write(
      "REENTER: %s, (OPENER:%s, DARKHOLE:%s)",
      charName,
      openerName,
      questTitle);
  }

  SendMsg_ReEnterDarkHoleResult(errCode);
}

void CPlayer::SendMsg_PostItemGold(char byErrCode)
{
  char payload[1];
  payload[0] = byErrCode;

  unsigned __int8 type[2]{58, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_PostDelete(unsigned __int8 byErrCode, unsigned int dwPostSerial)
{
  char payload[5];
  memcpy_0(payload, &dwPostSerial, sizeof(dwPostSerial));
  payload[4] = static_cast<char>(byErrCode);

  unsigned __int8 type[2]{58, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, 5u);
}

void CPlayer::SendMsg_PostReturnConfirm(char byErrCode, unsigned int dwPostSerial)
{
  char payload[5];
  payload[0] = byErrCode;
  memcpy_0(payload + 1, &dwPostSerial, sizeof(dwPostSerial));

  unsigned __int8 type[2]{58, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, 5u);
}

bool CPlayer::UpdateDelPost(unsigned int dwPostSerial, int nIndex)
{
  return m_pUserDB->Update_DelPost(dwPostSerial, static_cast<unsigned int>(nIndex)) != 0;
}

void CPlayer::DelPostData(unsigned int dwIndex)
{
  m_Param.m_PostStorage.DelPostData(dwIndex);
  m_pUserDB->DelPostData(dwIndex);
}

void CPlayer::SortPost(int nNumber)
{
  for (unsigned int index = 0; index < 50; ++index)
  {
    CPostData *post = &m_Param.m_PostStorage.m_PostData[index];
    if (post->GetState() != 255 && post->m_nNumber > nNumber)
    {
      --post->m_nNumber;
      UpdatePost(index);
    }
  }
}

void CPlayer::UpdatePostAddLog(int dwIndex, bool bLog, int nItemKey)
{
  _POSTDATA_DB_BASE *postData = &m_pUserDB->m_AvatorData.dbPostData;
  _POSTDATA_DB_BASE *backupData = &m_pUserDB->m_AvatorData_bk.dbPostData;
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (!post)
  {
    CPostSystemManager::Instace()->Log("CPlayer::UpdatePostAddLog() : pPost Is Null : Index(%u)", dwIndex);
    return;
  }

  if (post->m_dwPSSerial && post->m_dwPSSerial != postData->dbPost.m_PostList[dwIndex].dwPSSerial)
  {
    CPostSystemManager::Instace()->Log(
      "GetItem Wrong DB Data >> Name:%s >> Inx:%d >> Storage(S:%d) : DB(S:%d)",
      m_Param.GetCharNameW(),
      dwIndex,
      post->m_dwPSSerial,
      postData->dbPost.m_PostList[dwIndex].dwPSSerial);
  }

  const int key = post->m_Key.CovDBKey();
  m_pUserDB->Update_Post(
    dwIndex,
    post->m_dwPSSerial,
    post->m_nNumber,
    post->m_byState,
    key,
    post->m_dwDur,
    post->m_dwUpt,
    post->m_dwGold,
    post->m_lnUID);

  if (bLog
      && !postData->dbPost.m_PostList[dwIndex].bNew
      && postData->dbPost.m_PostList[dwIndex].dwPSSerial
      && postData->dbPost.m_PostList[dwIndex].nKey == backupData->dbPost.m_PostList[dwIndex].nKey)
  {
    CPostSystemManager::Instace()->Log(
      "GetItem KeyError >> Name:%s >> INX(%d) >> ORIGIN(%d) : NEW(%d) PS(%u) : OLD(%d) PS(%u)",
      m_Param.GetCharNameW(),
      dwIndex,
      nItemKey,
      postData->dbPost.m_PostList[dwIndex].nKey,
      postData->dbPost.m_PostList[dwIndex].dwPSSerial,
      backupData->dbPost.m_PostList[dwIndex].nKey,
      backupData->dbPost.m_PostList[dwIndex].dwPSSerial);

    if (postData->dbPost.m_PostList[dwIndex].dwPSSerial == backupData->dbPost.m_PostList[dwIndex].dwPSSerial)
    {
      CPostSystemManager::Instace()->Log("And Post Serial Is Equal!");
    }
    else
    {
      bool found = false;
      for (int j = 0; j < 50; ++j)
      {
        if (postData->dbPost.m_PostList[dwIndex].dwPSSerial == backupData->dbPost.m_PostList[j].dwPSSerial)
        {
          CPostSystemManager::Instace()->Log(
            "And Post DB Base Index Wrong >> Name:%s >> NEW : INX(%d) PS(%u) >> OLD : INX(%d) PS(%u)",
            m_Param.GetCharNameW(),
            dwIndex,
            postData->dbPost.m_PostList[dwIndex].dwPSSerial,
            j,
            backupData->dbPost.m_PostList[j].dwPSSerial);
          found = true;
          break;
        }
      }

      if (!found)
      {
        CPostSystemManager::Instace()->Log("And Matched Serial Is None!");
      }
    }
  }
}

bool CPlayer::IsReturnPostUpdate()
{
  return m_pUserDB->IsReturnPostUpdate();
}

void CPlayer::UpdateReturnPost(unsigned int dwSerial)
{
  m_pUserDB->Update_ReturnPost(dwSerial);
}

void CPlayer::pc_PostListRequest()
{
  _qry_case_post_storage_list_get qry{};
  qry.dwMasterSerial = m_pUserDB->m_dwSerial;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x4Fu, reinterpret_cast<char *>(&qry), size);
}

void CPlayer::pc_PostContentRequest(unsigned int dwIndex)
{
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (post)
  {
    if (post->GetState() != 1)
    {
      post->SetState(1u);
      UpdatePost(dwIndex);
    }
    if (m_Param.m_PostStorage.IsContentLoad(dwIndex))
    {
      SendMsg_PostContent(
        0,
        dwIndex,
        post->m_wszContent,
        post->m_Key.byTableCode,
        post->m_Key.wItemIndex,
        post->m_dwDur,
        post->m_dwUpt,
        post->m_dwGold);
    }
    else
    {
      _qry_case_post_content_get qry{};
      qry.dwMasterSerial = m_pUserDB->m_dwSerial;
      qry.dwSerial = post->m_dwPSSerial;
      qry.dwIndex = dwIndex;
      const int size = static_cast<int>(qry.size());
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x52u, reinterpret_cast<char *>(&qry), size);
    }
  }
  else
  {
    SendMsg_PostItemGold(0xBu);
  }
}

void CPlayer::pc_PostDeleteRequest(unsigned int dwIndex)
{
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (post)
  {
    if (UpdateDelPost(post->m_dwPSSerial, dwIndex))
    {
      if (post->m_Key.IsFilled() || post->m_dwGold)
      {
        CPlayer::s_MgrItemHistory.post_delete(post, m_szItemHistoryFileName);
      }
      const int number = post->m_nNumber;
      DelPostData(dwIndex);
      SortPost(number);
      SendMsg_PostDelete(0, dwIndex);
    }
    else
    {
      SendMsg_PostDelete(8u, dwIndex);
    }
  }
  else
  {
    SendMsg_PostDelete(0xBu, dwIndex);
  }
}

void CPlayer::pc_PostItemGoldRequest(unsigned int dwIndex)
{
  CPostData *post = m_Param.m_PostStorage.GetPostDataFromInx(dwIndex);
  if (!post)
  {
    SendMsg_PostItemGold(0xBu);
    return;
  }

  bool hasItem = false;
  int itemKey = -1;
  _STORAGE_LIST::_db_con *addedItem = nullptr;
  bool hadChange = false;

  if (post->m_Key.IsFilled() && m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    SendMsg_PostItemGold(0xEu);
    return;
  }

  if (post->m_dwGold)
  {
    const unsigned int money = GetMoney(1u);
    if (post->m_dwGold + money > 0x7A120)
    {
      SendMsg_PostItemGold(0xFu);
      return;
    }
  }

  if (post->m_Key.IsFilled())
  {
    _STORAGE_LIST::_db_con item;
    item.m_byTableCode = post->m_Key.byTableCode;
    item.m_wItemIndex = post->m_Key.wItemIndex;
    item.m_dwDur = post->m_dwDur;
    item.m_dwLv = post->m_dwUpt;
    item.m_lnUID = post->m_lnUID;
    item.m_wSerial = m_Param.GetNewItemSerial();
    addedItem = Emb_AddStorage(0, &item, false, true);
    if (!addedItem)
    {
      SendMsg_PostItemGold(2u);
      return;
    }
    SendMsg_RewardAddItem(addedItem, 5u);
    hasItem = true;
    itemKey = post->m_Key.CovDBKey();
    hadChange = true;
  }

  if (post->m_dwGold)
  {
    AddGold(post->m_dwGold, true);
    SendMsg_AlterMoneyInform(0);
    hadChange = true;
  }

  if (addedItem || post->m_dwGold)
  {
    CPlayer::s_MgrItemHistory.post_getpresent(
      post->m_wszSendName,
      post->m_dwPSSerial,
      addedItem,
      post->m_dwDur,
      post->m_dwGold,
      m_szItemHistoryFileName);
  }

  post->m_Key.SetRelease();
  post->m_dwDur = 0;
  post->m_dwUpt = 0xFFFFFFF;
  post->m_lnUID = 0;
  post->m_dwGold = 0;
  if (hadChange)
  {
    UpdatePostAddLog(dwIndex, hasItem, itemKey);
    SendMsg_PostItemGold(0);
  }
  else
  {
    SendMsg_PostItemGold(0xCu);
  }
}

void CPlayer::pc_PostReturnConfirmRequest(unsigned int dwPostSerial)
{
  if (!dwPostSerial)
  {
    return;
  }

  CPostData *post = m_Param.m_ReturnPostStorage.GetPostDataFromSerial(dwPostSerial);
  if (!post)
  {
    SendMsg_PostReturnConfirm(0xBu, dwPostSerial);
    return;
  }
  if (!IsReturnPostUpdate())
  {
    SendMsg_PostReturnConfirm(8u, dwPostSerial);
    return;
  }

  _STORAGE_LIST::_db_con *addedItem = nullptr;
  if (post->m_Key.IsFilled() && m_Param.m_dbInven.GetIndexEmptyCon() == 255)
  {
    SendMsg_PostReturnConfirm(0xEu, dwPostSerial);
    return;
  }

  const unsigned int money = GetMoney(1u);
  if (post->m_dwGold + money + 5 > 0x7A120)
  {
    SendMsg_PostReturnConfirm(0xFu, dwPostSerial);
    return;
  }

  if (post->m_Key.IsFilled())
  {
    _STORAGE_LIST::_db_con item;
    item.m_byTableCode = post->m_Key.byTableCode;
    item.m_wItemIndex = post->m_Key.wItemIndex;
    item.m_dwDur = post->m_dwDur;
    item.m_dwLv = post->m_dwUpt;
    item.m_lnUID = post->m_lnUID;
    item.m_wSerial = m_Param.GetNewItemSerial();
    addedItem = Emb_AddStorage(0, &item, false, true);
    if (!addedItem)
    {
      SendMsg_PostReturnConfirm(8u, dwPostSerial);
      return;
    }
    SendMsg_RewardAddItem(addedItem, 5u);
  }

  if (post->m_dwGold)
  {
    AddGold(post->m_dwGold, true);
    SendMsg_AlterMoneyInform(0);
  }

  if (addedItem || post->m_dwGold)
  {
    CPlayer::s_MgrItemHistory.post_return(
      post->m_wszRecvName,
      post->m_dwPSSerial,
      addedItem,
      post->m_dwDur,
      post->m_dwGold,
      m_szItemHistoryFileName);
  }

  post->m_Key.SetRelease();
  post->m_dwDur = 0;
  post->m_dwUpt = 0xFFFFFFF;
  post->m_lnUID = 0;
  post->m_dwGold = 0;
  AddGold(5u, true);
  SendMsg_AlterMoneyInform(0);
  UpdateReturnPost(dwPostSerial);
  m_Param.m_ReturnPostStorage.DelPostData(dwPostSerial);
  SendMsg_PostReturnConfirm(0, dwPostSerial);
}

void CPlayer::pc_UpdateDataForPostSend()
{
  _qry_case_update_data_for_post_send qry{};
  qry.dwSerial = m_pUserDB->m_dwSerial;
  qry.dwGlod = m_Param.GetGold();
  qry.pNewData = &m_pUserDB->m_AvatorData;
  qry.pOldData = &m_pUserDB->m_AvatorData_bk;
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0xAFu, reinterpret_cast<char *>(&qry), size);
}

void wa_EnterWorld(_WA_AVATOR_CODE *pData, unsigned __int16 wZoneIndex)
{
  CPartyPlayer *partyPlayer = &g_PartyPlayer[static_cast<unsigned int>(pData->m_id.wIndex)];
  if (!partyPlayer->m_bLogin)
  {
    partyPlayer->EnterWorld(pData, wZoneIndex);
  }
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

void CPlayer::SendMsg_Circle_DelEffect(
  unsigned __int8 byEffectCode,
  unsigned __int16 wEffectIndex,
  unsigned __int8 byLv,
  bool bToOne)
{

  _effect_remove_inform_zocl msg{};
  msg.wEffectCode = static_cast<unsigned __int16>(this->CalcEffectBit(byEffectCode, wEffectIndex));
  msg.dwPlayerSerial = this->m_dwObjSerial;

  unsigned __int8 type[2]{17, 11};
  this->CircleReport(type, reinterpret_cast<char *>(&msg), sizeof(msg), bToOne);
}

void CPlayer::SendMsg_RaceBossCryMsg()
{
  char messages[650]{};

  for (int index = 0; index < 10; ++index)
  {
    strcpy_0(&messages[65 * index], this->m_pmCryMsg.m_List[index].wszCryMsg);
  }

  unsigned __int8 type[2]{13, 105};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    messages,
    static_cast<unsigned __int16>(sizeof(messages)));
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
    if (byTaxRate >= 5u && byTaxRate <= 0x14u)
    {
      CUnmannedTraderTaxRateManager *taxRateManager = CUnmannedTraderTaxRateManager::Instance();
      const unsigned int suggestedTime = taxRateManager->GetSuggestedTime(raceCode);
      const unsigned int now = GetKorLocalTime();
      if (now - suggestedTime < 0x3Cu)
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
    this->SendMsg_ResultChangeTaxRate(errorCode, 0xFFu);
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

void CPlayer::pc_ReleaseSiegeModeRequest()
{
  unsigned __int8 byRetCode = 0;
  if (!this->IsSiegeMode())
  {
    byRetCode = 11;
  }

  if (byRetCode)
  {
    this->SendMsg_ReleaseSiegeModeResult(static_cast<char>(byRetCode));
    return;
  }

  this->m_bIsSiegeActing = true;
  this->m_tmrSiegeTime.BeginTimer(0x7D0u);
  this->SetSiege(nullptr);
}

void CPlayer::pc_TransformSiegeModeRequest(unsigned __int16 wItemSerial)
{
  unsigned __int8 byRetCode = 0;
  _STORAGE_LIST::_db_con *pInvenItem = this->m_Param.m_dbInven.GetPtrFromSerial(wItemSerial);
  _STORAGE_LIST::_db_con *pWeapon = this->m_Param.m_dbEquip.m_pStorageList + 6;

  if (!pInvenItem || pInvenItem->m_byTableCode != 27)
  {
    byRetCode = 1;
  }
  else
  {
    _base_fld *pSiegeRecord = g_Main.m_tblItemData[27].GetRecord(pInvenItem->m_wItemIndex);
    if (!pSiegeRecord)
    {
      byRetCode = 1;
    }
    else if (this->IsSiegeMode())
    {
      byRetCode = 3;
    }
    else if (!pInvenItem->m_dwDur)
    {
      byRetCode = 4;
    }
    else if (!pWeapon->m_bLoad)
    {
      byRetCode = 5;
    }
    else
    {
      _base_fld *pWeaponRecord = g_Main.m_tblItemData[6].GetRecord(pWeapon->m_wItemIndex);
      if (!pWeaponRecord)
      {
        byRetCode = 6;
      }
      else if (
        this->m_pmWpn.byWpType != *reinterpret_cast<int *>(&pSiegeRecord[3].m_strCode[4])
        || *reinterpret_cast<int *>(&pWeaponRecord[6].m_strCode[12])
             != *reinterpret_cast<int *>(&pSiegeRecord[3].m_strCode[8]))
      {
        byRetCode = 6;
      }
      else if (!IsItemEquipCivil(pInvenItem->m_byTableCode, pInvenItem->m_wItemIndex, this->m_Param.GetRaceSexCode()))
      {
        byRetCode = 7;
      }
      else if (!this->IsEffectableEquip(pInvenItem))
      {
        byRetCode = 8;
      }
      else if (this->IsRidingUnit())
      {
        byRetCode = 9;
      }
      else if (pInvenItem->m_bLock)
      {
        byRetCode = 10;
      }
    }
  }

  if (byRetCode)
  {
    this->SendMsg_TransformSiegeModeResult(static_cast<char>(byRetCode));
    return;
  }

  this->m_bIsSiegeActing = true;
  this->m_tmrSiegeTime.BeginTimer(0x7D0u);
  this->SetSiege(pInvenItem);
}

void CPlayer::pc_TransShipRenewTicketRequest(unsigned __int16 wTicketItemSerial)
{
  unsigned __int8 byErrCode = 0;
  CTransportShip *ship = &g_TransportShip[this->m_Param.GetRaceCode()];
  _STORAGE_LIST::_db_con *pTicketItem = this->m_Param.m_dbInven.GetPtrFromSerial(wTicketItemSerial);

  if (this->m_pCurMap != ship->m_pLinkShipMap
      || !ship->IsMemberBeforeLogoff(this->m_Param.GetCharSerial())
      || !ship->m_bAnchor)
  {
    byErrCode = 1;
  }
  else
  {
    const int level = static_cast<int>(this->GetLevel());
    const int minLevel = ship->GetRideLimLevel();
    const int maxLevel = ship->GetRideUpLimLevel();

    if (level < minLevel || (maxLevel != -1 && level > maxLevel))
    {
      byErrCode = 14;
    }
    else if (!pTicketItem || pTicketItem->m_byTableCode != 28)
    {
      byErrCode = 1;
    }
    else
    {
      _TicketItem_fld *pTicketRecord =
        reinterpret_cast<_TicketItem_fld *>(g_Main.m_tblItemData[28].GetRecord(pTicketItem->m_wItemIndex));
      if (!pTicketRecord || !ship->GetCurRideShipThisTicket(pTicketRecord))
      {
        byErrCode = 1;
      }
    }
  }

  if (!byErrCode)
  {
    byErrCode = static_cast<unsigned __int8>(!ship->RenewOldMember(this));
  }

  this->SendMsg_TransShipRenewTicketResult(static_cast<char>(byErrCode));
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
      SyncTrunkMetaToUserDB(this);
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
  else if (strcmp_0(pwszPassword, this->m_Param.m_wszTrunkPasswd))
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
    || strcmp_0(pwszPrevPassword, this->m_Param.m_wszTrunkPasswd))
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
    SyncTrunkMetaToUserDB(this);
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
            if (byStartStorageIndex == 5 || byStartStorageIndex == 7)
            {
              unsigned __int8 itemRace = 0;
              if (!GetTrunkSlotRaceBySerial(this, byStartStorageIndex, wItemSerial, &itemRace) || itemRace != raceCode)
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
      this->SendMsg_TrunkIoResult(0, 0x12, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    _STORAGE_LIST::_db_con *pAdded = this->Emb_AddStorage(byTarStorageIndex, &copyItem, false, false);
    if (!pAdded)
    {
      this->Emb_AddStorage(byStartStorageIndex, &copyItem, false, false);
      this->SendMsg_TrunkIoResult(0, 0x12, this->m_Param.GetDalant(), dwConsumDalant);
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
          else if (byStartStorageIndex == 5 || byStartStorageIndex == 7)
          {
            unsigned __int8 itemRace = 0;
            if (!GetTrunkSlotRaceBySerial(this, byStartStorageIndex, wStartItemSerial, &itemRace)
                || itemRace != this->m_Param.GetRaceCode())
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
      this->SendMsg_TrunkIoResult(1, 0x12, this->m_Param.GetDalant(), dwConsumDalant);
      return;
    }

    _STORAGE_LIST::_db_con *firstAdded = this->Emb_AddStorage(byStartStorageIndex, &targetCopy, false, false);
    if (!firstAdded)
    {
      this->Emb_AddStorage(byStartStorageIndex, &startCopy, false, false);
      this->SendMsg_TrunkIoResult(1, 0x12, this->m_Param.GetDalant(), dwConsumDalant);
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
      this->SendMsg_TrunkIoResult(1, 0x12, this->m_Param.GetDalant(), dwConsumDalant);
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
  else if (static_cast<unsigned int>(dwDalant) > 0x77359400u || static_cast<unsigned int>(dwGold) > 0x7A120u)
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
      this->m_Param.m_dTrunkDalant -= static_cast<double>(dwDalant);
      this->m_Param.m_dTrunkGold -= static_cast<double>(dwGold);
      this->AddDalant(dwDalant, false);
      this->AddGold(dwGold, false);
    }
    else
    {
      this->SubDalant(static_cast<unsigned int>(dwDalant));
      this->SubGold(static_cast<unsigned int>(dwGold));
      this->m_Param.m_dTrunkDalant += static_cast<double>(dwDalant);
      this->m_Param.m_dTrunkGold += static_cast<double>(dwGold);
    }

    SyncTrunkMetaToUserDB(this);
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

void CPlayer::SendMsg_TransShipRenewTicketResult(char byErrCode)
{
  _trans_ship_renew_ticket_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);

  unsigned __int8 pbyType[2] = {33, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 1u);
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
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 0x15u);
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
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 0xAu);
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
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 0xAu);
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
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 0x1Du);
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
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 0xEu);
}

void CPlayer::SendMsg_ResSeparation(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pNewOre)
{
  unsigned __int8 pbyType[2] = {13, 7};
  char msg[7]{};
  msg[0] = byErrCode;
  if (!byErrCode && pStartOre && pNewOre)
  {
    std::memcpy(&msg[1], &pStartOre->m_wSerial, sizeof(pStartOre->m_wSerial));
    msg[3] = static_cast<char>(pStartOre->m_dwDur);
    std::memcpy(&msg[4], &pNewOre->m_wSerial, sizeof(pNewOre->m_wSerial));
    msg[6] = static_cast<char>(pNewOre->m_dwDur);
  }

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, msg, 7u);
}

void CPlayer::SendMsg_PotionSeparation(
  unsigned __int16 wParentSerial,
  unsigned __int8 byParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int8 byChildAmount,
  char nRet)
{
  unsigned __int8 pbyType[2] = {13, 13};
  char msg[7]{};
  msg[0] = nRet;
  std::memcpy(&msg[1], &wParentSerial, sizeof(wParentSerial));
  msg[3] = static_cast<char>(byParentAmount);
  std::memcpy(&msg[4], &wChildSerial, sizeof(wChildSerial));
  msg[6] = static_cast<char>(byChildAmount);

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, msg, 7u);
}

void CPlayer::SendMsg_ResDivision(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pTargetOre)
{
  unsigned __int8 pbyType[2] = {13, 9};
  char msg[7]{};
  msg[0] = byErrCode;
  if (!byErrCode && pStartOre && pTargetOre)
  {
    std::memcpy(&msg[1], &pStartOre->m_wSerial, sizeof(pStartOre->m_wSerial));
    msg[3] = pStartOre->m_bLoad ? static_cast<char>(pStartOre->m_dwDur) : 0;
    std::memcpy(&msg[4], &pTargetOre->m_wSerial, sizeof(pTargetOre->m_wSerial));
    msg[6] = static_cast<char>(pTargetOre->m_dwDur);
  }

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, msg, 7u);
}

void CPlayer::SendMsg_PotionDivision(
  unsigned __int16 wSerial,
  unsigned __int8 byParentAmount,
  unsigned __int16 wChildSerial,
  unsigned __int8 byChildAmount,
  char nRet)
{
  unsigned __int8 pbyType[2] = {13, 15};
  char msg[7]{};
  msg[0] = nRet;
  std::memcpy(&msg[1], &wSerial, sizeof(wSerial));
  msg[3] = static_cast<char>(byParentAmount);
  std::memcpy(&msg[4], &wChildSerial, sizeof(wChildSerial));
  msg[6] = static_cast<char>(byChildAmount);

  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, msg, 7u);
}

void CPlayer::SendMsg_ForceInvenChange(char byErrCode)
{
  unsigned __int8 pbyType[2] = {13, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, &byErrCode, 1u);
}



void CPlayer::pc_ResSeparation(unsigned __int16 wStartSerial, unsigned __int8 byMoveAmount)
{
SendMsg_ResSeparation(1u, nullptr, nullptr);
}

void CPlayer::pc_PotionSeparation(unsigned __int16 wSerial, unsigned __int8 byAmount)
{
  SendMsg_PotionSeparation(wSerial, 0, 0xFFu, byAmount, static_cast<char>(-1));
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
                if (targetOre->m_dwDur + byMoveAmount > 0x63)
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
              if (targetPotion->m_dwDur + byAmount > 0x63)
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
        linkCode = 0xFF;
        linkIndex = 0xFFFF;
      }
    }
    else if (linkCode == 0xFF)
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
        if (wReplaceSerial != 0xFFFF)
        {
          replaceItem = targetStorage->GetPtrFromSerial(wReplaceSerial);
          if (!replaceItem || replaceItem->m_byTableCode != 15)
          {
            resultCode = 2;
            goto SEND_FORCE_CHANGE_RESULT;
          }
          if (replaceItem->m_bLock)
          {
            resultCode = 11;
            goto SEND_FORCE_CHANGE_RESULT;
          }
          if (CPlayer::s_pnLinkForceItemToEffect[sourceItem->m_wItemIndex]
              != CPlayer::s_pnLinkForceItemToEffect[replaceItem->m_wItemIndex])
          {
            resultCode = 3;
            goto SEND_FORCE_CHANGE_RESULT;
          }
        }
        if (wReplaceSerial == 0xFFFF && targetStorage->GetIndexEmptyCon() == 255)
        {
          resultCode = 5;
        }
        else if (wReplaceSerial == 0xFFFF && !pItem->byStorageCode)
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

SEND_FORCE_CHANGE_RESULT:
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
      this->SendMsg_ForceInvenChange(0xFFu);
      return;
    }

    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceBackup;
      std::memcpy(&replaceBackup, replaceItem, sizeof(replaceBackup));
      if (!this->Emb_AddStorage(sourceStorage->m_nListCode, &replaceBackup, true, false))
      {
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_ForceInvenChange(0xFFu);
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
        this->SendMsg_ForceInvenChange(0xFFu);
        return;
      }

      _STORAGE_LIST::_db_con replaceForRestore;
      std::memcpy(&replaceForRestore, replaceItem, sizeof(replaceForRestore));
      this->Emb_AddStorage(targetStorage->m_nListCode, &replaceForRestore, true, false);
      this->Emb_DelStorage(sourceStorage->m_nListCode, replaceItem->m_byStorageIndex, false, false, nullptr);
      this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
      this->SendMsg_ForceInvenChange(0xFFu);
      return;
    }

    if (!this->Emb_AddStorage(targetStorage->m_nListCode, &sourceBackup, true, false))
    {
      this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
      this->SendMsg_ForceInvenChange(0xFFu);
      return;
    }
    this->Emb_EquipLink();
  }
  this->SendMsg_ForceInvenChange(resultCode);
}



void CPlayer::pc_MineCancle()
{
  if (this->m_bMineMode)
  {
    this->m_bMineMode = false;
    this->m_dwMineNextTime = static_cast<unsigned int>(-1);
    this->SendMsg_MineCancle();
  }
}





void CPlayer::pc_SelectQuestAfterHappenEvent(unsigned __int8 bySelectIndex)
{
  if (!this->m_QuestMgr.m_LastHappenEvent.isset())
  {
    return;
  }

  _happen_event_cont happenEvent = this->m_QuestMgr.m_LastHappenEvent;
  if (!this->Emb_StartQuest(bySelectIndex, &happenEvent))
  {
    this->SendMsg_InsertQuestFailure(
      happenEvent.m_QtHpType,
      static_cast<unsigned int>(happenEvent.m_nIndexInType),
      static_cast<unsigned __int8>(happenEvent.m_nRaceCode));
  }
}

void CPlayer::pc_BriefPass(unsigned __int8 byQuestSlotIndex)
{
  _QUEST_DB_BASE::_LIST *quest = &this->m_Param.m_QuestDB.m_List[byQuestSlotIndex];
  if (quest->byQuestType == 0xFF)
  {
    return;
  }

  _base_fld *record = CQuestMgr::s_tblQuest->GetRecord(quest->wIndex);
  if (record)
  {
    this->Emb_CheckActForQuest(15, record->m_strCode, 1u, false);
  }
}

void CPlayer::pc_RequestDialogWithNPC(CItemStore *pStore)
{
  if (GetSqrt(this->m_fCurPos, pStore->GetStorePos()) > 60.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int raceCode = this->m_Param.GetRaceCode();
  if (npcRace != raceCode && npcRace != 0xFF)
  {
    return;
  }

  char *npcCode = pStore->GetNpcCode();
  if (!npcCode)
  {
    return;
  }

  this->Emb_CheckActForQuest(14, npcCode, 1u, false);
  this->Emb_CheckActForQuest(1, npcCode, 1u, false);
}

void CPlayer::pc_RequestWatchingWithNPC(CItemStore *pStore)
{
  if (GetSqrt(this->m_fCurPos, pStore->GetStorePos()) > 60.0f)
  {
    return;
  }

  const int npcRace = pStore->m_byNpcRaceCode;
  const int raceCode = this->m_Param.GetRaceCode();
  if (npcRace != raceCode && npcRace != 0xFF)
  {
    return;
  }

  char *npcCode = pStore->GetNpcCode();
  if (npcCode)
  {
    this->Emb_CheckActForQuest(17, npcCode, 1u, false);
  }
}

void CPlayer::SendMsg_CuttingCompleteResult(unsigned __int8 byRet)
{
  char msg[5]{};
  const unsigned int gold = this->m_Param.GetGold();
  std::memcpy(msg, &gold, sizeof(gold));
  msg[4] = static_cast<char>(byRet);

  unsigned __int8 type[2] = {14, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 5u);
}

void CPlayer::pc_CuttingComplete(unsigned __int8 byNpcRace)
{
unsigned __int64 addGold = 0;
  const unsigned int texRate = 10000 - eGetTexRate(this->m_Param.GetRaceCode());
  bool hasResource = false;

  for (int resourceIndex = 0; resourceIndex < GetMaxResKind(); ++resourceIndex)
  {
    if (this->m_Param.m_wCuttingResBuffer[resourceIndex])
    {
      _base_fld *record = g_Main.m_tblItemData[18].GetRecord(resourceIndex);
      const int unitPrice = *reinterpret_cast<int *>(&record[5].m_strCode[12]);
      addGold += static_cast<unsigned __int64>(this->m_Param.m_wCuttingResBuffer[resourceIndex]) * unitPrice;
      hasResource = true;
    }
  }

  addGold = addGold * texRate / 10000u;

  if (hasResource)
  {
    if (!CanAddMoneyForMaxLimGold(addGold, this->m_Param.GetGold()))
    {
      this->SendMsg_CuttingCompleteResult(1u);
      return;
    }

    this->AddGold(static_cast<int>(addGold), true);
    CPlayer::s_MgrItemHistory.cut_clear_item(
      this->m_ObjID.m_wIndex,
      this->m_Param.m_wCuttingResBuffer,
      static_cast<unsigned int>(addGold),
      this->m_Param.GetGold(),
      this->m_szItemHistoryFileName);
  }

  this->m_Param.InitResBuffer();

  if (!this->m_byUserDgr)
  {
    eAddGold(this->m_Param.GetRaceCode(), static_cast<int>(addGold));
  }

  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_CuttingEmpty();
  }

  this->SendMsg_CuttingCompleteResult(0);
}

void CPlayer::pc_DTradeCancleRequest()
{
  CPlayer *tradeDst = nullptr;
  const bool differentTradeState = !DTradeEqualPerson(this, &tradeDst);
  if (differentTradeState)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);

    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }

    return;
  }

  this->m_pmTrd.Init();
  tradeDst->m_pmTrd.Init();
  this->SendMsg_DTradeCancleResult(static_cast<unsigned __int8>(differentTradeState));
  tradeDst->SendMsg_DTradeCancleInform();
}

void CPlayer::pc_DTradeAskRequest(unsigned __int16 wDstIndex)
{
  unsigned __int8 result = 0;
  CPlayer *dst = &g_Player[wDstIndex];

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    dst->m_pmTrd.bDTradeMode = false;
    result = 23;
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(dst->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    dst->m_pmTrd.bDTradeMode = false;
    result = 24;
  }
  else if (dst->m_bLive
    && dst->m_bOper
    && dst->m_Param.GetRaceCode() == this->m_Param.GetRaceCode()
    && !dst->m_bCorpse
    && dst->m_pCurMap == this->m_pCurMap
    && dst->GetCurSecNum() != static_cast<unsigned int>(-1)
    && dst->m_EP.GetEff_Have(50) <= 0.0f)
  {
    if (this->m_pmTrd.bDTradeMode || dst->m_pmTrd.bDTradeMode)
    {
      result = 1;
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 3;
    }
    else if (this->m_byUserDgr == dst->m_byUserDgr)
    {
      if (GetSqrt(dst->m_fCurPos, this->m_fCurPos) > 100.0f)
      {
        result = 2;
      }
    }
    else
    {
      result = 3;
    }
  }
  else
  {
    result = 3;
  }

  if (!result)
  {
    this->m_pmTrd.wDTradeDstIndex = dst->m_ObjID.m_wIndex;
    this->m_pmTrd.dwDTradeDstSerial = dst->m_dwObjSerial;

    char askMsg[6] = {};
    memcpy_0(askMsg, &this->m_ObjID.m_wIndex, sizeof(this->m_ObjID.m_wIndex));
    memcpy_0(askMsg + 2, &this->m_dwObjSerial, sizeof(this->m_dwObjSerial));
    unsigned __int8 askType[2] = {18, 3};
    g_Network.m_pProcess[0]->LoadSendMsg(dst->m_ObjID.m_wIndex, askType, askMsg, 6u);
  }

  this->SendMsg_DTradeAskResult(static_cast<char>(result));
}

void CPlayer::pc_DTradeAnswerRequest(_CLID *pidAsker)
{
  unsigned __int8 result = 0;
  CPlayer *asker = &g_Player[pidAsker->wIndex];

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    asker->m_pmTrd.bDTradeMode = false;
    result = 23;
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(asker->m_ObjID.m_wIndex) == 99)
  {
    this->m_pmTrd.bDTradeMode = false;
    asker->m_pmTrd.bDTradeMode = false;
    result = 24;
  }
  else if (asker->m_dwObjSerial != pidAsker->dwSerial)
  {
    result = 1;
  }
  else if (asker->m_pmTrd.wDTradeDstIndex != this->m_ObjID.m_wIndex
    || asker->m_pmTrd.dwDTradeDstSerial != this->m_dwObjSerial)
  {
    result = 4;
  }
  else if (this->m_pmTrd.bDTradeMode || asker->m_pmTrd.bDTradeMode)
  {
    result = 2;
  }
  else if (!(asker->m_bLive
      && asker->m_Param.GetRaceCode() == this->m_Param.GetRaceCode()
      && !asker->m_bCorpse
      && asker->m_pCurMap == this->m_pCurMap
      && asker->GetCurSecNum() != static_cast<unsigned int>(-1)))
  {
    result = 3;
  }
  else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    result = 6;
  }
  else if (this->m_byUserDgr != asker->m_byUserDgr)
  {
    result = 6;
  }
  else if (GetSqrt(asker->m_fCurPos, this->m_fCurPos) > 100.0f)
  {
    result = 5;
  }

  if (result)
  {
    char answerMsg[1] = {static_cast<char>(result)};
    unsigned __int8 answerType[2] = {18, 5};
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, answerType, answerMsg, 1u);
    return;
  }

  unsigned int codeA[4] = {};
  unsigned int codeB[4] = {};
  for (int index = 0; index < 4; ++index)
  {
    codeA[index] = static_cast<unsigned int>(rand() + (rand() << 16));
    codeB[index] = static_cast<unsigned int>(rand() + (rand() << 16));
  }

  unsigned int *keyA = CalcCodeKey(codeA);
  unsigned int *keyB = CalcCodeKey(codeB);

  auto setTradeStart = [](_DTRADE_PARAM &trade, unsigned __int16 dstIndex, unsigned int dstSerial, unsigned __int8 emptyCount, const unsigned int *key)
  {
    trade.bDTradeMode = true;
    trade.bDTradeLock = false;
    trade.bDTradeOK = false;
    trade.wDTradeDstIndex = dstIndex;
    trade.dwDTradeDstSerial = dstSerial;
    trade.dwDTrade_Dalant = 0;
    trade.dwDTrade_Gold = 0;
    trade.byEmptyInvenNum = emptyCount;
    trade.bySellItemNum = 0;
    for (int slotIndex = 0; slotIndex < 15; ++slotIndex)
    {
      trade.DItemNode[slotIndex].bLoad = false;
    }
    memcpy_0(trade.dwKey, key, sizeof(trade.dwKey));
  };

  setTradeStart(
    this->m_pmTrd,
    asker->m_ObjID.m_wIndex,
    asker->m_dwObjSerial,
    static_cast<unsigned __int8>(this->m_Param.m_dbInven.GetNumEmptyCon()),
    keyA);
  setTradeStart(
    asker->m_pmTrd,
    this->m_ObjID.m_wIndex,
    this->m_dwObjSerial,
    static_cast<unsigned __int8>(asker->m_Param.m_dbInven.GetNumEmptyCon()),
    keyB);


  _d_trade_start_inform_zocl startToThis{};
  startToThis.idAsker.wIndex = asker->m_ObjID.m_wIndex;
  startToThis.idAsker.dwSerial = asker->m_dwObjSerial;
  startToThis.byAskerEmptyNum = asker->m_pmTrd.byEmptyInvenNum;
  startToThis.idAnswer.wIndex = this->m_ObjID.m_wIndex;
  startToThis.idAnswer.dwSerial = this->m_dwObjSerial;
  startToThis.byAnswerEmptyNum = this->m_pmTrd.byEmptyInvenNum;
  memcpy_0(startToThis.dwKey, codeA, sizeof(startToThis.dwKey));

  _d_trade_start_inform_zocl startToAsker{};
  startToAsker.idAsker.wIndex = asker->m_ObjID.m_wIndex;
  startToAsker.idAsker.dwSerial = asker->m_dwObjSerial;
  startToAsker.byAskerEmptyNum = asker->m_pmTrd.byEmptyInvenNum;
  startToAsker.idAnswer.wIndex = this->m_ObjID.m_wIndex;
  startToAsker.idAnswer.dwSerial = this->m_dwObjSerial;
  startToAsker.byAnswerEmptyNum = this->m_pmTrd.byEmptyInvenNum;
  memcpy_0(startToAsker.dwKey, codeB, sizeof(startToAsker.dwKey));

  unsigned __int8 startType[2] = {18, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    startType,
    reinterpret_cast<char *>(&startToThis),
    static_cast<unsigned __int16>(sizeof(startToThis)));
  g_Network.m_pProcess[0]->LoadSendMsg(
    asker->m_ObjID.m_wIndex,
    startType,
    reinterpret_cast<char *>(&startToAsker),
    static_cast<unsigned __int16>(sizeof(startToAsker)));
}

void CPlayer::pc_DTradeLockRequest()
{
  unsigned __int8 result = 0;
  CPlayer *tradeDst = nullptr;
  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pmTrd.bDTradeLock)
    {
      result = 2;
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  this->m_pmTrd.bDTradeLock = true;

  char lockInform[1] = {0};
  unsigned __int8 lockInformType[2] = {18, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(tradeDst->m_ObjID.m_wIndex, lockInformType, lockInform, 1u);

  char lockResult[1] = {0};
  unsigned __int8 lockResultType[2] = {18, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, lockResultType, lockResult, 1u);
}

void CPlayer::pc_DTradeAddRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byStorageCode,
  unsigned int dwSerial,
  unsigned __int8 byAmount)
{
  unsigned __int8 result = 0;
  _STORAGE_LIST::_db_con *item = nullptr;
  CPlayer *tradeDst = nullptr;

  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
    else if (this->m_pmTrd.bDTradeLock)
    {
      result = 2;
    }
    else if (bySlotIndex < 15u)
    {
      if (this->m_pmTrd.DItemNode[bySlotIndex].bLoad)
      {
        result = 4;
      }
      else
      {
        item = this->m_Param.m_pStoragePtr[byStorageCode]->GetPtrFromSerial(dwSerial);
        if (!item)
        {
          result = 5;
        }
        else if (item->m_bLock)
        {
          result = 9;
        }
        else if (item->m_byTableCode == 19)
        {
          result = 7;
        }
        else if (!IsExchangeItem(item->m_byTableCode, item->m_wItemIndex))
        {
          result = 10;
        }
        else if (IsOverLapItem(item->m_byTableCode) && (byAmount == 0 || byAmount > item->m_dwDur))
        {
          result = 6;
        }
        else
        {
          for (int slotIndex = 0; slotIndex < 15; ++slotIndex)
          {
            if (this->m_pmTrd.DItemNode[slotIndex].bLoad && this->m_pmTrd.DItemNode[slotIndex].dwSerial == dwSerial)
            {
              result = 6;
              break;
            }
          }
        }
      }
    }
    else
    {
      result = 3;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  if (byStorageCode == 0)
  {
    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur == byAmount)
      {
        ++this->m_pmTrd.byEmptyInvenNum;
      }
    }
    else
    {
      ++this->m_pmTrd.byEmptyInvenNum;
    }
  }

  ++this->m_pmTrd.bySellItemNum;
  _DTRADE_ITEM &tradeItem = this->m_pmTrd.DItemNode[bySlotIndex];
  tradeItem.bLoad = true;
  tradeItem.byStorageCode = byStorageCode;
  tradeItem.dwSerial = dwSerial;
  tradeItem.byAmount = byAmount;

  char inform[23] = {};
  inform[0] = static_cast<char>(bySlotIndex);
  inform[1] = static_cast<char>(item->m_byTableCode);
  memcpy_0(inform + 2, &item->m_wItemIndex, sizeof(item->m_wItemIndex));
  memcpy_0(inform + 4, &item->m_dwDur, sizeof(item->m_dwDur));
  memcpy_0(inform + 12, &item->m_dwLv, sizeof(item->m_dwLv));
  inform[16] = static_cast<char>(byAmount);
  inform[17] = static_cast<char>(this->m_pmTrd.byEmptyInvenNum);
  inform[18] = static_cast<char>(item->m_byCsMethod);
  memcpy_0(inform + 19, &item->m_dwT, sizeof(item->m_dwT));
  unsigned __int8 informType[2] = {18, 15};
  g_Network.m_pProcess[0]->LoadSendMsg(tradeDst->m_ObjID.m_wIndex, informType, inform, 23u);

  char addResult[1] = {0};
  unsigned __int8 addResultType[2] = {18, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, addResultType, addResult, 1u);
}

void CPlayer::pc_DTradeDelRequest(unsigned __int8 bySlotIndex)
{
  unsigned __int8 result = 0;
  CPlayer *tradeDst = nullptr;
  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
    else if (this->m_pmTrd.bDTradeLock)
    {
      result = 2;
    }
    else if (bySlotIndex < 15u)
    {
      if (!this->m_pmTrd.DItemNode[bySlotIndex].bLoad)
      {
        result = 4;
      }
    }
    else
    {
      result = 3;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  _DTRADE_ITEM &tradeItem = this->m_pmTrd.DItemNode[bySlotIndex];
  if (tradeItem.byStorageCode == 0)
  {
    _STORAGE_LIST::_db_con *item = this->m_Param.m_pStoragePtr[tradeItem.byStorageCode]->GetPtrFromSerial(tradeItem.dwSerial);
    if (!item)
    {
      return;
    }

    if (IsOverLapItem(item->m_byTableCode))
    {
      if (item->m_dwDur == tradeItem.byAmount)
      {
        --this->m_pmTrd.byEmptyInvenNum;
      }
    }
    else
    {
      --this->m_pmTrd.byEmptyInvenNum;
    }
  }

  --this->m_pmTrd.bySellItemNum;
  tradeItem.bLoad = false;

  char delInform[2] = {
    static_cast<char>(bySlotIndex),
    static_cast<char>(this->m_pmTrd.byEmptyInvenNum),
  };
  unsigned __int8 delInformType[2] = {18, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(tradeDst->m_ObjID.m_wIndex, delInformType, delInform, 2u);

  char delResult[1] = {0};
  unsigned __int8 delResultType[2] = {18, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, delResultType, delResult, 1u);
}

void CPlayer::pc_DTradeBetRequest(unsigned __int8 byMoneyUnit, unsigned int dwBetAmount)
{
  unsigned __int8 result = 0;
  CPlayer *tradeDst = nullptr;
  if (DTradeEqualPerson(this, &tradeDst))
  {
    if (this->m_pmTrd.bDTradeLock)
    {
      result = 4;
    }
    else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
    {
      result = 6;
    }
    else if (byMoneyUnit)
    {
      if (dwBetAmount > this->m_Param.GetGold())
      {
        result = 2;
      }
    }
    else if (dwBetAmount > this->m_Param.GetDalant())
    {
      result = 1;
    }
  }
  else
  {
    result = 1;
  }

  if (result)
  {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
    return;
  }

  if (byMoneyUnit)
  {
    this->m_pmTrd.dwDTrade_Gold = dwBetAmount;
  }
  else
  {
    this->m_pmTrd.dwDTrade_Dalant = dwBetAmount;
  }

  char betInform[5] = {0};
  betInform[0] = static_cast<char>(byMoneyUnit);
  memcpy_0(betInform + 1, &dwBetAmount, sizeof(dwBetAmount));
  unsigned __int8 betInformType[2] = {18, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(tradeDst->m_ObjID.m_wIndex, betInformType, betInform, 5u);

  char betResult[1] = {0};
  unsigned __int8 betResultType[2] = {18, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, betResultType, betResult, 1u);
}

void CPlayer::pc_UpdateDataForTrade(CPlayer *pTrader)
{
  if (!this->m_pUserDB || !pTrader || !pTrader->m_pUserDB)
  {
    return;
  }

  _qry_case_update_data_for_trade query;
  query.tradelist[0].dwSerial = this->m_pUserDB->m_dwSerial;
  query.tradelist[0].dwDalant = this->m_Param.GetDalant();
  query.tradelist[0].dwGlod = this->m_Param.GetGold();
  query.tradelist[0].pNewData = &this->m_pUserDB->m_AvatorData;
  query.tradelist[0].pOldData = &this->m_pUserDB->m_AvatorData_bk;
  query.tradelist[1].dwSerial = pTrader->m_pUserDB->m_dwSerial;
  query.tradelist[1].dwDalant = pTrader->m_Param.GetDalant();
  query.tradelist[1].dwGlod = pTrader->m_Param.GetGold();
  query.tradelist[1].pNewData = &pTrader->m_pUserDB->m_AvatorData;
  query.tradelist[1].pOldData = &pTrader->m_pUserDB->m_AvatorData_bk;

  g_Main.PushDQSData(
    0xFFFFFFFFu,
    nullptr,
    0xB0u,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));
}

void CPlayer::pc_DTradeOKRequest(unsigned int *pdwKey)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  CPlayer *tradeDst = nullptr;
  _STORAGE_LIST::_db_con tradeItems[2][15] = {};
  int tradedItemCount[2] = {0, 0};
  CPlayer *players[2] = {this, nullptr};

  auto closeTrade = [&]() {
    this->m_pmTrd.Init();
    this->SendMsg_DTradeCloseInform(0);
    if (tradeDst)
    {
      tradeDst->m_pmTrd.Init();
      tradeDst->SendMsg_DTradeCloseInform(0);
    }
  };

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    resultCode = 23;
  }
  else if (!DTradeEqualPerson(this, &tradeDst))
  {
    resultCode = 1;
  }
  else if (this->m_pCurMap->m_pMapSet->m_nMapType == 1)
  {
    resultCode = 6;
  }
  else if (!(this->m_pmTrd.bDTradeLock && tradeDst->m_pmTrd.bDTradeLock))
  {
    resultCode = 2;
  }
  else if (this->m_pmTrd.bDTradeOK)
  {
    resultCode = 3;
  }
  else if (std::memcmp(this->m_pmTrd.dwKey, pdwKey, sizeof(this->m_pmTrd.dwKey)) != 0)
  {
    resultCode = 13;
  }
  else if (this->m_pmTrd.bySellItemNum > tradeDst->m_pmTrd.byEmptyInvenNum)
  {
    resultCode = 100;
  }
  else
  {
    players[1] = tradeDst;
    for (int side = 0; side < 2; ++side)
    {
      CPlayer *current = players[side];
      if (current->m_pmTrd.dwDTrade_Dalant > current->m_Param.GetDalant()
          || current->m_pmTrd.dwDTrade_Gold > current->m_Param.GetGold())
      {
        resultCode = 5;
        break;
      }

      if (current->m_pmTrd.dwDTrade_Dalant > 0x77359400u
          || current->m_pmTrd.dwDTrade_Gold > 0x7A120u)
      {
        resultCode = 5;
        break;
      }
    }

    if (!resultCode && tradeDst->m_pmTrd.bDTradeOK)
    {
      for (int side = 0; side < 2; ++side)
      {
        CPlayer *current = players[side];
        CPlayer *other = players[(side + 1) & 1];

        const double dalantDelta =
          static_cast<double>(other->m_pmTrd.dwDTrade_Dalant)
          - static_cast<double>(current->m_pmTrd.dwDTrade_Dalant);
        if (dalantDelta > 0.0)
        {
          if (!CanAddMoneyForMaxLimMoney(static_cast<unsigned int>(dalantDelta), current->m_Param.GetDalant()))
          {
            resultCode = 6;
            break;
          }
        }

        const double goldDelta =
          static_cast<double>(other->m_pmTrd.dwDTrade_Gold)
          - static_cast<double>(current->m_pmTrd.dwDTrade_Gold);
        if (goldDelta > 0.0)
        {
          if (!CanAddMoneyForMaxLimGold(static_cast<unsigned int>(goldDelta), current->m_Param.GetGold()))
          {
            resultCode = 6;
            break;
          }
        }

        int copiedCount = 0;
        for (int tradeSlot = 0; tradeSlot < 15; ++tradeSlot)
        {
          _DTRADE_ITEM *tradeItem = &current->m_pmTrd.DItemNode[tradeSlot];
          if (!tradeItem->bLoad)
          {
            continue;
          }

          _STORAGE_LIST *storage = current->m_Param.m_pStoragePtr[tradeItem->byStorageCode];
          _STORAGE_LIST::_db_con *source = storage->GetPtrFromSerial(static_cast<unsigned __int16>(tradeItem->dwSerial));
          if (!source)
          {
            resultCode = 4;
            break;
          }

          if (source->m_bLock || source->m_byTableCode == 19)
          {
            resultCode = 9;
            break;
          }

          std::memcpy(&tradeItems[side][copiedCount], source, sizeof(_STORAGE_LIST::_db_con));
          if (IsOverLapItem(source->m_byTableCode) && source->m_dwDur > tradeItem->byAmount)
          {
            tradeItems[side][copiedCount].m_dwDur = tradeItem->byAmount;
          }
          ++copiedCount;
        }

        tradedItemCount[side] = copiedCount;
        if (resultCode)
        {
          break;
        }
      }
    }
  }

  if (resultCode)
  {
    closeTrade();
    return;
  }

  this->SendMsg_DTradeOKResult(0);
  tradeDst->SendMsg_DTradeOKInform();
  this->m_pmTrd.bDTradeOK = true;

  if (!(tradeDst->m_pmTrd.bDTradeOK && this->m_pmTrd.bDTradeOK))
  {
    return;
  }

  players[0] = this;
  players[1] = tradeDst;
  for (int side = 0; side < 2; ++side)
  {
    const int oppositeSide = (side + 1) & 1;
    CPlayer *current = players[side];
    CPlayer *other = players[oppositeSide];

    for (int itemIndex = 0; itemIndex < tradedItemCount[side]; ++itemIndex)
    {
      _STORAGE_LIST::_db_con &item = tradeItems[side][itemIndex];
      if (!item.m_pInList)
      {
        resultCode = 4;
        break;
      }

      const unsigned __int8 storageCode = static_cast<unsigned __int8>(item.m_pInList->m_nListCode);
      if (IsOverLapItem(item.m_byTableCode))
      {
        current->Emb_AlterDurPoint(
          storageCode,
          item.m_byStorageIndex,
          -static_cast<int>(item.m_dwDur),
          true,
          false);
      }
      else if (!current->Emb_DelStorage(
                 storageCode,
                 item.m_byStorageIndex,
                 false,
                 true,
                 "CPlayer::pc_DTradeOKRequest()"))
      {
        if (side == 0 && tradedItemCount[0] > 0)
        {
          for (int restored = 0; restored < itemIndex; ++restored)
          {
            current->Emb_AddStorage(0, &tradeItems[side][restored], false, true);
            current->SendMsg_RewardAddItem(&tradeItems[side][restored], 0);
          }
          closeTrade();
          return;
        }
      }
    }

    unsigned __int16 firstReceivedSerial = static_cast<unsigned __int16>(-1);
    int addedIncomingCount = 0;
    bool addFailed = false;

    for (int itemIndex = 0; itemIndex < tradedItemCount[oppositeSide]; ++itemIndex)
    {
      _STORAGE_LIST::_db_con &incoming = tradeItems[oppositeSide][itemIndex];
      incoming.m_wSerial = current->m_Param.GetNewItemSerial();
      if (firstReceivedSerial == static_cast<unsigned __int16>(-1))
      {
        firstReceivedSerial = incoming.m_wSerial;
      }

      _STORAGE_LIST::_db_con *added =
        current->Emb_AddStorage(0, &incoming, false, true);
      if (!added)
      {
        if (side == 0 && tradedItemCount[oppositeSide] > 0)
        {
          addFailed = true;
          break;
        }
        continue;
      }

      incoming.m_byStorageIndex = added->m_byStorageIndex;
      ++addedIncomingCount;
    }

    if (addFailed)
    {
      for (int removed = 0; removed < addedIncomingCount; ++removed)
      {
        current->Emb_DelStorage(
          0,
          tradeItems[oppositeSide][removed].m_byStorageIndex,
          false,
          true,
          "CPlayer::pc_DTradeOKRequest() - Emb_Add Item Fail");
      }

      for (int restored = 0; restored < tradedItemCount[side]; ++restored)
      {
        current->Emb_AddStorage(0, &tradeItems[side][restored], false, true);
        current->SendMsg_RewardAddItem(&tradeItems[side][restored], 0);
      }

      closeTrade();
      return;
    }

    double dalantDelta =
      static_cast<double>(other->m_pmTrd.dwDTrade_Dalant)
      - static_cast<double>(current->m_pmTrd.dwDTrade_Dalant);
    double goldDelta =
      static_cast<double>(other->m_pmTrd.dwDTrade_Gold)
      - static_cast<double>(current->m_pmTrd.dwDTrade_Gold);

    const double playerPenalty =
      g_Main.m_pTimeLimitMgr->GetPlayerPenalty(this->m_id.wIndex);
    dalantDelta *= playerPenalty;
    goldDelta *= playerPenalty;

    current->AlterDalant(dalantDelta);
    current->AlterGold(goldDelta);
    current->SendMsg_DTradeAccomplishInform(true, firstReceivedSerial);
  }

  this->pc_UpdateDataForTrade(tradeDst);
  g_Main.m_logDTrade.Write(
    "[%d]%s(%d): D(%d) G(%d) # [%d]%s(%d): D(%d) G(%d)",
    static_cast<int>(this->GetLevel()),
    this->m_Param.GetCharNameA(),
    this->m_dwObjSerial,
    this->m_pmTrd.dwDTrade_Dalant,
    this->m_pmTrd.dwDTrade_Gold,
    static_cast<int>(tradeDst->GetLevel()),
    tradeDst->m_Param.GetCharNameA(),
    tradeDst->m_dwObjSerial,
    tradeDst->m_pmTrd.dwDTrade_Dalant,
    tradeDst->m_pmTrd.dwDTrade_Gold);

  this->m_pmTrd.Init();
  tradeDst->m_pmTrd.Init();
}



void CPlayer::SendMsg_DTradeCancleResult(char byErrCode)
{
  char msg[1] = {byErrCode};
  unsigned __int8 type[2] = {18, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 1u);
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



void CPlayer::pc_CastVoteRequest(int nVoteSerial, unsigned __int8 byCode)
{
  unsigned __int8 result = 0;
  const int raceCode = this->m_Param.GetRaceCode();
  if (raceCode < 0 || raceCode >= 3 || !g_VoteSys[raceCode].m_bActive)
  {
    result = 3;
  }
  else if (g_VoteSys[raceCode].m_nSerial != nVoteSerial)
  {
    result = 4;
  }
  else if (this->m_nVoteSerial == nVoteSerial)
  {
    result = 5;
  }
  else if (this->m_Param.m_byPvPGrade < g_VoteSys[raceCode].m_byLimGrade)
  {
    result = 6;
  }

  if (!result)
  {
    this->m_nVoteSerial = nVoteSerial;
    g_VoteSys[raceCode].ActVote(this->m_dwObjSerial, byCode);
  }

  char msg[11] = {};
  msg[0] = static_cast<char>(result);
  if (!result)
  {
    memcpy_0(msg + 1, &this->m_nVoteSerial, sizeof(this->m_nVoteSerial));
    for (int pointIndex = 0; pointIndex < 3; ++pointIndex)
    {
      const unsigned __int16 point = static_cast<unsigned __int16>(g_VoteSys[raceCode].m_dwPoint[pointIndex]);
      memcpy_0(msg + 5 + pointIndex * sizeof(point), &point, sizeof(point));
    }
  }

  unsigned __int8 type[2] = {26, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 11u);
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
        char linkMaterialBuffer[80];
        std::memcpy(linkMaterialBuffer, combineRecord->m_Material, sizeof(linkMaterialBuffer));

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
            char *materialInfo = &linkMaterialBuffer[16 * materialIndex];
            if (CheckSameItem(materialInfo, materialRecord->m_strCode, &selectedLinkIndex, true)
                && *reinterpret_cast<int *>(materialInfo + 12) > 0)
            {
              *reinterpret_cast<int *>(materialInfo + 12) -= materialRequest[slotIndex].byAmount;
              if (*reinterpret_cast<int *>(materialInfo + 8) != -1
                  && *reinterpret_cast<unsigned int *>(materialInfo + 8) == materialStorage[slotIndex]->m_dwLv)
              {
                *reinterpret_cast<int *>(materialInfo + 8) = -1;
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
            const char *materialInfo = &linkMaterialBuffer[16 * materialIndex];
            if (*reinterpret_cast<const int *>(materialInfo + 12) > 0
                || *reinterpret_cast<const int *>(materialInfo + 8) != -1)
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
      && strlen_0(szUILockPW)
      && strlen_0(szUILockPW_Confirm)
      && strlen_0(uszUILock_HintAnswer))
  {
    if (strlen_0(szUILockPW) <= 12
        && strlen_0(szUILockPW_Confirm) <= 12
        && strlen_0(uszUILock_HintAnswer) <= 16)
    {
      if (!strcmp_0(szUILockPW, szUILockPW_Confirm))
      {
        if (!strcmp_0(szUILockPW, pUserDB->m_szAccount_PW))
        {
          resultCode = 2;
        }
        else if (m_Param.GetTrunkSlotNum()
                 && !strcmp_0(szUILockPW, m_Param.m_wszTrunkPasswd))
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
  if (uszUILockPW && strlen_0(uszUILockPW))
  {
    if (pUserDB->m_byUILock)
    {
      if (pUserDB->m_byUILock == 2)
      {
        resultCode = 2;
      }
      else if (strcmp_0(pUserDB->m_szUILock_PW, uszUILockPW))
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
      && strlen_0(uszUILockPW)
      && strlen_0(uszUILockPW_Confirm)
      && strlen_0(uszUILock_HintAnswer))
  {
    if (strlen_0(uszUILockPW) <= 12
        && strlen_0(uszUILockPW_Confirm) <= 12
        && strlen_0(uszUILock_HintAnswer) <= 16)
    {
      if (m_pUserDB->m_byUILock == 2)
      {
        if (!strcmp_0(uszUILockPWOld, m_pUserDB->m_szUILock_PW))
        {
          if (!strcmp_0(uszUILockPW, m_pUserDB->m_szUILock_PW))
          {
            resultCode = 8;
          }
          else if (!strcmp_0(uszUILockPW, uszUILockPW_Confirm))
          {
            if (!strcmp_0(uszUILockPW, m_pUserDB->m_szAccount_PW))
            {
              resultCode = 2;
            }
            else if (m_Param.GetTrunkSlotNum()
                     && !strcmp_0(uszUILockPW, m_Param.m_wszTrunkPasswd))
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
  if (uszHintAnswer && strlen_0(uszHintAnswer))
  {
    if (pUserDB->m_byUILock)
    {
      if (pUserDB->m_byUILock == 2)
      {
        resultCode = 3;
      }
      else if (strcmp_0(pUserDB->m_uszUILock_HintAnswer, uszHintAnswer))
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

void CPlayer::pc_MineStart(
  unsigned __int8 byMineIndex,
  unsigned __int8 byOreIndex,
  unsigned __int16 wBatterySerial)
{
  unsigned __int8 resultCode = 0;
  _base_fld *oreRecord = g_Main.m_tblItemData[17].GetRecord(byOreIndex);
  _res_dummy *resDummy = nullptr;
  int resDummySector = -1;
  _STORAGE_LIST::_db_con *toolItem = nullptr;
  _STORAGE_LIST::_db_con *batteryItem = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 33;
  }
  else if (m_Param.m_pAPM && m_Param.m_pAPM->is_installed())
  {
    resultCode = 25;
  }
  else if (m_dwMineNextTime != static_cast<unsigned int>(-1))
  {
    resultCode = 7;
  }
  else if (!oreRecord || *reinterpret_cast<unsigned int *>(&oreRecord[3].m_strCode[0]) > 2u)
  {
    resultCode = 9;
  }
  else if (byMineIndex >= m_pCurMap->m_nResDumNum)
  {
    resultCode = 1;
  }
  else
  {
    resDummy = &m_pCurMap->m_pResDummy[byMineIndex];
    if (!resDummy || resDummy->m_byQualityGrade == 2)
    {
      resultCode = 28;
    }
    else if (resDummy->m_byQualityGrade == 2 || IsMiningByMinigTicket())
    {
      resDummySector = m_pCurMap->GetResDummySector(byMineIndex, m_fCurPos);
      if (resDummySector == -1)
      {
        resultCode = 2;
      }
      else
      {
        toolItem = &m_Param.m_dbEquip.m_pStorageList[6];
        if (!toolItem->m_bLoad)
        {
          resultCode = 4;
        }
        else if (toolItem->m_bLock)
        {
          resultCode = 12;
        }
        else
        {
          _base_fld *toolRecord = g_Main.m_tblItemData[6].GetRecord(toolItem->m_wItemIndex);
          if (!toolRecord || *reinterpret_cast<int *>(&toolRecord[6].m_strCode[8]) != 10)
          {
            resultCode = 5;
          }
          else
          {
            batteryItem = m_Param.m_dbInven.GetPtrFromSerial(wBatterySerial);
            if (!batteryItem)
            {
              resultCode = 8;
            }
            else if (batteryItem->m_byTableCode != 16)
            {
              resultCode = 8;
            }
            else if (batteryItem->m_wItemIndex >= 6u && batteryItem->m_wItemIndex <= 9u)
            {
              resultCode = 8;
            }
            else if (batteryItem->m_bLock)
            {
              resultCode = 12;
            }
            else if (!COreAmountMgr::Instance()->IsOreRemain())
            {
              resultCode = 27;
            }
          }
        }
      }
    }
    else
    {
      resultCode = 26;
    }
  }

  if (!resultCode)
  {
    m_bMineMode = true;

    const bool isPcBang = m_pUserDB->m_BillingInfo.IsPcBangType();
    int delayMs = GetResDummyDelayForMining(m_pCurMap->m_pResDummy[byMineIndex], resDummySector, isPcBang);

    const float mineSpeedBuff = m_EP.GetEff_Have(7);
    if (mineSpeedBuff > 1.0f)
    {
      delayMs = static_cast<int>(static_cast<float>(delayMs) / mineSpeedBuff);
    }

    const float mineSpeedRate = m_EP.GetEff_Rate(37);
    if (mineSpeedRate > 1.0f)
    {
      delayMs = static_cast<int>(static_cast<float>(delayMs) / mineSpeedRate);
    }

    int penaltyScale = 0;
    const double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
    if (penalty != 0.0)
    {
      penaltyScale = static_cast<int>(1.0 / penalty);
    }
    delayMs *= penaltyScale;

    m_dwMineNextTime = static_cast<unsigned int>(delayMs) + GetLoopTime();
    m_byDelaySec = static_cast<unsigned __int8>(delayMs / 1000);
    m_wBatterySerialTmp = wBatterySerial;
    m_bySelectOreIndex = byOreIndex;
    m_zMinePos[0] = static_cast<int>(m_fCurPos[0]);
    m_zMinePos[1] = static_cast<int>(m_fCurPos[2]);

    if (m_bMove)
    {
      const bool outExtraStopPos = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtraStopPos);
      Stop();
    }
    BreakStealth();
  }

  this->SendMsg_MineStartResult(resultCode);
}

void CPlayer::pc_OreCutting(unsigned __int16 wOreSerial, unsigned __int8 byProcessNum)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *oreItem = nullptr;
  _base_fld *oreRecord = nullptr;
  unsigned int leftOreCount = 0;
  unsigned int consumedDalant = 0;

  const int raceCode = m_Param.GetRaceCode();
  const unsigned int texRate = eGetTexRate(raceCode) + 10000;

  bool hasBufferedRes = false;
  const int maxResKind = GetMaxResKind();
  for (int index = 0; index < maxResKind; ++index)
  {
    if (m_Param.m_wCuttingResBuffer[index])
    {
      hasBufferedRes = true;
      break;
    }
  }

  if (hasBufferedRes)
  {
    resultCode = 4;
  }
  else
  {
    oreItem = m_Param.m_dbInven.GetPtrFromSerial(wOreSerial);
    if (!oreItem)
    {
      resultCode = 1;
    }
    else if (oreItem->m_byTableCode != 17)
    {
      resultCode = 1;
    }
    else if (oreItem->m_bLock)
    {
      resultCode = 11;
    }
    else
    {
      oreRecord = g_Main.m_tblItemData[17].GetRecord(oreItem->m_wItemIndex);
      if (!oreRecord)
      {
        resultCode = 1;
      }
      else if (byProcessNum > oreItem->m_dwDur)
      {
        resultCode = 2;
      }
      else
      {
        const int baseDalant = *reinterpret_cast<int *>(&oreRecord[4].m_strCode[4]);
        const float managerScale = static_cast<float>(eGetMgrValue());
        const float unitDalant = static_cast<float>(baseDalant)
                               + static_cast<float>(baseDalant) / 1000.0f * managerScale;
        unsigned long long totalDalant = static_cast<unsigned long long>(
          static_cast<int>(unitDalant * static_cast<float>(byProcessNum)));
        totalDalant = totalDalant * texRate / 10000u;
        consumedDalant = static_cast<unsigned int>(totalDalant);
        if (totalDalant > m_Param.GetDalant())
        {
          resultCode = 3;
        }
      }
    }
  }

  if (!resultCode)
  {
    const int minCutCount = static_cast<int>(oreRecord[4].m_dwIndex);
    const int maxCutCount = *reinterpret_cast<int *>(&oreRecord[4].m_strCode[0]);
    for (int processIndex = 0; processIndex < byProcessNum; ++processIndex)
    {
      int cutCount = minCutCount;
      if (maxCutCount >= minCutCount)
      {
        cutCount += rand() % (maxCutCount - minCutCount + 1);
      }

      for (int cutIndex = 0; cutIndex < cutCount; ++cutIndex)
      {
        const unsigned int rate = static_cast<unsigned int>(rand() + (rand() << 16));
        const unsigned int resIndex = g_Main.m_tblOreCutting.GetOreIndexFromRate(
          oreRecord->m_dwIndex,
          rate);
        if (resIndex != static_cast<unsigned int>(-1)
            && resIndex < static_cast<unsigned int>(maxResKind)
            && m_Param.m_wCuttingResBuffer[resIndex] < 0xFFu)
        {
          ++m_Param.m_wCuttingResBuffer[resIndex];
        }
      }
    }

    leftOreCount = Emb_AlterDurPoint(0, oreItem->m_byStorageIndex, -byProcessNum, false, false);
    SubDalant(consumedDalant);

    if (!m_byUserDgr)
    {
      const int managerDivisor = eGetMgrValue() + 1000;
      if (managerDivisor > 0)
      {
        const int dalantDrain = static_cast<int>(consumedDalant / static_cast<unsigned int>(managerDivisor));
        if (dalantDrain > 0)
        {
          eAddDalant(m_Param.GetRaceCode(), -dalantDrain);
        }
      }
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(m_Param.GetRaceCode(), level, consumedDalant);
    }

    if (m_pUserDB && CActionPointSystemMgr::Instance()->GetEventStatus(0) == 2)
    {
      const unsigned int currentPoint = m_pUserDB->GetActPoint(0);
      const unsigned int pointPerCut = *reinterpret_cast<unsigned int *>(&oreRecord[3].m_strCode[36]);
      const unsigned int nextPoint = currentPoint + byProcessNum * pointPerCut;
      m_pUserDB->Update_User_Action_Point(0, nextPoint);
      SendMsg_Alter_Action_Point(0, nextPoint);
    }
  }

  this->SendMsg_OreCuttingResult(resultCode, static_cast<unsigned __int8>(leftOreCount), consumedDalant);
}

void CPlayer::pc_OreIntoBag(
  unsigned __int16 wResIndex,
  unsigned __int16 wSerial,
  unsigned __int8 byAddAmount)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *targetItem = nullptr;
  unsigned __int16 newSerial = wSerial;
  _STORAGE_LIST::_db_con newItem;
  const _TimeItem_fld *timeRecord = TimeItem::FindTimeRec(18, wResIndex);

  if (byAddAmount > m_Param.m_wCuttingResBuffer[wResIndex])
  {
    resultCode = 2;
  }
  else if (wSerial == 0xFFFF && m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
  {
    resultCode = 4;
  }
  else if (wSerial != 0xFFFF)
  {
    targetItem = m_Param.m_dbInven.GetPtrFromSerial(wSerial);
    if (!targetItem)
    {
      resultCode = 5;
    }
    else if (targetItem->m_byTableCode != 18 || targetItem->m_wItemIndex != wResIndex)
    {
      resultCode = 7;
    }
    else if (targetItem->m_bLock)
    {
      resultCode = 11;
    }
    else if (targetItem->m_dwDur + byAddAmount > 99)
    {
      resultCode = 6;
    }
  }

  if (!resultCode)
  {
    m_Param.m_wCuttingResBuffer[wResIndex] -= byAddAmount;
    if (!targetItem || timeRecord)
    {
      newItem = _STORAGE_LIST::_db_con();
      newItem.m_byTableCode = 18;
      newItem.m_wItemIndex = wResIndex;
      newItem.m_dwDur = byAddAmount;
      newItem.m_dwLv = 0xFFFFFFF;
      newItem.m_wSerial = m_Param.GetNewItemSerial();

      if (timeRecord && timeRecord->m_nCheckType)
      {
        __time32_t currentTime = 0;
        _time32(&currentTime);
        newItem.m_byCsMethod = static_cast<unsigned __int8>(timeRecord->m_nCheckType);
        newItem.m_dwT = timeRecord->m_nUseTime + currentTime;
        newItem.m_dwLendRegdTime = currentTime;
      }

      if (!Emb_AddStorage(0, &newItem, false, false))
      {
        this->SendMsg_OreIntoBagResult(static_cast<char>(0xFF), newSerial, 0, 0);
        return;
      }
      newSerial = newItem.m_wSerial;
    }
    else
    {
      Emb_AlterDurPoint(0, targetItem->m_byStorageIndex, byAddAmount, false, false);
    }

    _base_fld *resRecord = g_Main.m_tblItemData[18].GetRecord(wResIndex);
    if (resRecord)
    {
      Emb_CheckActForQuest(13, resRecord->m_strCode, byAddAmount, false);
    }
  }

  this->SendMsg_OreIntoBagResult(static_cast<char>(resultCode), newSerial, newItem.m_byCsMethod, newItem.m_dwT);
}

void CPlayer::pc_SelectQuestReward(
  unsigned __int8 byQuestDBSlot,
  unsigned __int8 bySelectItemSlotIndex,
  unsigned __int8 bySelectLinkQuestIndex)
{
  _QUEST_DB_BASE::_LIST *questSlot = &m_Param.m_QuestDB.m_List[byQuestDBSlot];
  if (questSlot->byQuestType == 0xFF)
  {
    return;
  }

  for (int index = 0; index < 3; ++index)
  {
    if (questSlot->wNum[index] != 0xFFFF)
    {
      return;
    }
  }

  _base_fld *questRecord = CQuestMgr::s_tblQuest->GetRecord(questSlot->wIndex);
  if (!questRecord)
  {
    return;
  }

  const int hasSelectReward = *reinterpret_cast<int *>(&questRecord[13].m_strCode[60]);
  if (bySelectItemSlotIndex == 0xFF)
  {
    if (hasSelectReward)
    {
      return;
    }
  }
  else if (!hasSelectReward)
  {
    return;
  }

  if (bySelectItemSlotIndex != 0xFF)
  {
    const char *rewardCode = reinterpret_cast<const char *>(&questRecord[14]) + 72 * bySelectItemSlotIndex;
    if (!strncmp(rewardCode, "-1", 2u))
    {
      return;
    }
  }

  if (bySelectLinkQuestIndex != 0xFF)
  {
    const char *linkCode = &questRecord[22].m_strCode[64 * static_cast<unsigned __int64>(bySelectItemSlotIndex) + 44];
    if (!strncmp(linkCode, "-1", 2u))
    {
      return;
    }
  }

  Emb_CompleteQuest(byQuestDBSlot, bySelectItemSlotIndex, bySelectLinkQuestIndex);
}

namespace
{
void ApplyGaugeConsumeAndSendRecover(CPlayer *player, const unsigned __int16 *pwDelPoint)
{
  for (int paramCode = 0; paramCode < 3; ++paramCode)
  {
    if (!pwDelPoint[paramCode])
    {
      continue;
    }

    const int gauge = player->GetGauge(paramCode);
    const int consumeAmount = static_cast<int>(pwDelPoint[paramCode]);
    const int remain = (gauge > consumeAmount) ? (gauge - consumeAmount) : 0;
    player->SetGauge(paramCode, remain, true);
  }
  player->SendMsg_Recover();
}
} // namespace

unsigned __int8 CPlayer::skill_process_for_item(int nSkillIndex, _CHRID *pidDst, int *pnLv)
{
  (void)nSkillIndex;
  (void)pidDst;
  (void)pnLv;
  // this is not a stub
  return 0;
}

void CPlayer::skill_process_for_aura(int nSkillIndex)
{
  _skill_fld *sourceSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(nSkillIndex));
  if (!sourceSkill || sourceSkill->m_nClass != 4)
  {
    return;
  }

  _skill_fld *auraSkill =
    reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(sourceSkill->m_strRangeEffCode));
  if (!auraSkill)
  {
    return;
  }

  const bool beneficial = auraSkill->m_nContEffectType == 1;
  CCharacter *targets[31]{};
  const int affectedCount = static_cast<int>(_GetAreaEffectMember(
    this,
    beneficial,
    sourceSkill->m_nBonusDistance,
    m_fCurPos,
    auraSkill->m_strActableDst,
    targets));

  unsigned __int8 errorCode[32]{};
  bool upMastery = false;
  for (int index = 0; index < affectedCount; ++index)
  {
    CCharacter *target = targets[index];
    if (!target || target->m_ObjID.m_byKind || target->m_ObjID.m_byID)
    {
      continue;
    }

    CPlayer *targetPlayer = static_cast<CPlayer *>(target);
    const unsigned int targetSerial = targetPlayer->m_Param.GetCharSerial();
    const int raceCode = targetPlayer->m_Param.GetRaceCode();
    const unsigned __int8 bossType =
      CPvpUserAndGuildRankingSystem::Instance()->GetBossType(static_cast<unsigned __int8>(raceCode), targetSerial);
    if (bossType != 4 && bossType != 8)
    {
      AssistSkill(target, 0, auraSkill, 7, errorCode, &upMastery);
    }
  }
}

unsigned __int8 CPlayer::skill_process(
  int nEffectCode,
  int nSkillIndex,
  _CHRID *pidDst,
  unsigned __int16 *pConsumeSerial,
  int *pnLv)
{
  _skill_fld *skillField = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[nEffectCode].GetRecord(nSkillIndex));
  unsigned __int16 gaugeCost[3]{};
  int skillLevel = 1;
  CCharacter *target = nullptr;
  int classGrade[4]{};
  classGrade[0] = -1;

  if (nEffectCode)
  {
    if (nEffectCode == 2 && !m_Param.IsActableClassSkill(skillField->m_strCode, classGrade))
    {
      return 15;
    }
  }
  else
  {
    if (skillField->m_nMastIndex > 8u)
    {
      return 8;
    }
    if (!_pre_check_skill_enable(skillField))
    {
      return 21;
    }
    if (!_pre_check_skill_gradelimit(skillField))
    {
      return 34;
    }
    if (skillField->m_nClass == 4 && (IsChaosMode() || IsPunished(1u, false)))
    {
      return 35;
    }
  }

  if ((nEffectCode == 0 || nEffectCode == 2) && m_bInGuildBattle && m_bTakeGravityStone)
  {
    return 27;
  }

  unsigned __int8 masteryIndex = static_cast<unsigned __int8>(-1);
  if (nEffectCode)
  {
    if (nEffectCode == 2)
    {
      masteryIndex = static_cast<unsigned __int8>(classGrade[0]);
    }
  }
  else
  {
    if (skillField->m_nMastIndex < 8u)
    {
      masteryIndex = static_cast<unsigned __int8>(skillField->m_nMastIndex);
    }
    if (skillField->m_nClass == 2)
    {
      masteryIndex = static_cast<unsigned __int8>(-1);
    }
  }

  if (!m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(
        &m_AttDelayChker,
        static_cast<unsigned __int8>(nEffectCode),
        static_cast<unsigned __int16>(skillField->m_dwIndex),
        masteryIndex))
  {
    return 9;
  }

  if (skillField->m_nTempEffectType >= 150)
  {
    return 21;
  }
  if (skillField->m_nContEffectType == 1 && m_EP.GetEff_State(2))
  {
    return 7;
  }
  if (IsRidingUnit())
  {
    return 14;
  }
  if (static_cast<unsigned int>(m_Param.GetRaceCode()) == 2 && IsActingSiegeMode())
  {
    return 21;
  }
  if (m_dwSelfDestructionTime)
  {
    return 26;
  }

  target = reinterpret_cast<CCharacter *>(g_Main.GetObjectA(0, pidDst->byID, pidDst->wIndex));
  if (!target)
  {
    return 2;
  }
  if (!IsEffectableDst(skillField->m_strActableDst, target))
  {
    return 5;
  }
  if (!target->m_bLive)
  {
    return 2;
  }
  if (skillField->m_nTempEffectType == -1 && skillField->m_nContEffectType == -1)
  {
    return 8;
  }
  if (skillField->m_nContEffectType != -1 && !target->IsRecvableContEffect())
  {
    return 13;
  }
  if (target->m_EP.GetEff_State(20))
  {
    if (skillField->m_nTempEffectType == -1 || !IsUsableTempEffectAtStoneState(skillField->m_nTempEffectType))
    {
      return 24;
    }
  }
  if (target->m_EP.GetEff_State(28))
  {
    return 24;
  }
  if (m_EP.GetEff_State(21))
  {
    return 25;
  }
  if (!skillField->m_nContEffectType && !IsAttackableInTown() && !target->IsAttackableInTown())
  {
    if (IsInTown() || target->IsInTown())
    {
      return 18;
    }
    if (m_Param.m_pGuild
        && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
          m_Param.m_pGuild->m_dwSerial,
          m_ObjID.m_wIndex,
          m_pUserDB->m_dwSerial))
    {
      return 18;
    }
  }

  if (!nEffectCode)
  {
    if (skillField->m_nClass == 2)
    {
      const unsigned __int8 equipSkillError = EquipItemSFAgent.IsEnableSkill(skillField);
      if (equipSkillError)
      {
        return equipSkillError;
      }
    }
    else
    {
      if (!IsSFUsableSFMastery(3u, skillField->m_nMastIndex))
      {
        return 3;
      }
      if (!IsSFActableByClass(0, skillField))
      {
        return 16;
      }
    }
  }

  if (m_byMoveType == 2)
  {
    return 28;
  }
  if (!IsSFUseableRace(static_cast<unsigned __int8>(nEffectCode), static_cast<unsigned __int16>(skillField->m_dwIndex)))
  {
    return 4;
  }
  if (!IsSFUsableGauge(
        static_cast<unsigned __int8>(nEffectCode),
        static_cast<unsigned __int16>(skillField->m_dwIndex),
        gaugeCost))
  {
    return 6;
  }

  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCount[3]{};
  bool overlap[3]{};
  if (!GetUseConsumeItem(skillField->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCount, overlap))
  {
    return 32;
  }

  if (!nEffectCode)
  {
    const float totalSkillLevel = static_cast<float>(m_pmMst.GetSkillLv(static_cast<unsigned __int8>(nSkillIndex)))
      + m_EP.GetEff_Plus(19);
    skillLevel = static_cast<int>(totalSkillLevel);
    if (skillLevel > 7)
    {
      skillLevel = 7;
    }
  }
  if (pnLv)
  {
    *pnLv = skillLevel;
  }

  unsigned __int8 errorCode = 0;
  bool upMastery = false;
  const bool assistSuccess = AssistSkill(target, nEffectCode, skillField, skillLevel, &errorCode, &upMastery) != 0;
  if (assistSuccess)
  {
    if (upMastery && !IsInTown())
    {
      bool inGuildRoom = false;
      if (m_Param.m_pGuild)
      {
        inGuildRoom = CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
          m_Param.m_pGuild->m_dwSerial,
          m_ObjID.m_wIndex,
          m_pUserDB->m_dwSerial);
      }

      if (!inGuildRoom
          && skillField->m_nClass != 2
          && skillField->m_nClass != 4
          && !nEffectCode
          && skillField->m_nMastIndex < 8u)
      {
        unsigned int alterValue = 1;
        if (skillField->m_nClass == 1)
        {
          alterValue = 2;
        }

        if (GetObjRace() != target->GetObjRace() || !IsChaosMode() || skillField->m_nContEffectType)
        {
          if (GetObjRace() != target->GetObjRace() || target->m_ObjID.m_byID)
          {
            Emb_AlterStat(
              3u,
              static_cast<unsigned __int8>(skillField->m_dwIndex),
              static_cast<int>(alterValue),
              0,
              "CPlayer::skill_process()---0",
              true);
          }
          else
          {
            CPlayer *targetPlayer = static_cast<CPlayer *>(target);
            if (!targetPlayer->IsPunished(1u, false))
            {
              Emb_AlterStat(
                3u,
                static_cast<unsigned __int8>(skillField->m_dwIndex),
                static_cast<int>(alterValue),
                0,
                "CPlayer::skill_process()---0",
                true);
            }
          }
        }
      }
    }

    if (skillField->m_nClass == 4)
    {
      m_tmrAuraSkill.BeginTimer(0x1388u);
      skill_process_for_aura(static_cast<int>(skillField->m_dwIndex));
    }

    for (int paramCode = 0; paramCode < 3; ++paramCode)
    {
      if (!gaugeCost[paramCode])
      {
        continue;
      }

      const int currentGauge = GetGauge(paramCode);
      const int nextGauge = (currentGauge - gaugeCost[paramCode] > 0) ? (currentGauge - gaugeCost[paramCode]) : 0;
      SetGauge(paramCode, nextGauge, true);
    }

    SendMsg_Recover();
    DeleteUseConsumeItem(consumeItems, consumeCount, overlap);
    const float addDelay = m_EP.GetEff_Plus(12);
    _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(skillField->m_fActDelay + addDelay));
  }

  return errorCode;
}

void CPlayer::pc_SkillRequest(unsigned __int8 bySkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  int skillLevel = 1;
  const bool wasStealth = GetStealth(true);
  const unsigned __int8 errCode = skill_process(0, bySkillIndex, pidDst, pConsumeSerial, &skillLevel);
  if ((!errCode || errCode == 100) && wasStealth)
  {
    BreakStealth();
  }
  SendMsg_SkillResult(errCode, pidDst, bySkillIndex, skillLevel);
}

void CPlayer::pc_ClassSkillRequest(unsigned __int16 wSkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  const bool wasStealth = GetStealth(true);
  const unsigned __int8 errCode = skill_process(2, wSkillIndex, pidDst, pConsumeSerial, nullptr);
  if ((!errCode || errCode == 100) && wasStealth)
  {
    BreakStealth();
  }
  SendMsg_ClassSkillResult(errCode, pidDst, wSkillIndex);
}

void CPlayer::pc_ForceRequest(unsigned __int16 wForceSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *forceItem = nullptr;
  _force_fld *forceFld = nullptr;
  CCharacter *target = nullptr;
  unsigned __int16 delPoint[3]{};
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int forceLevel = 1;

  if (IsRidingUnit())
  {
    byErrCode = 14;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 28;
  }
  else if (m_EP.GetEff_State(20) || m_EP.GetEff_State(28))
  {
    byErrCode = 24;
  }
  else if (m_EP.GetEff_State(21))
  {
    byErrCode = 25;
  }
  else if (m_dwSelfDestructionTime)
  {
    byErrCode = 26;
  }
  else if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    byErrCode = 27;
  }

  if (!byErrCode)
  {
    forceItem = m_Param.m_dbForce.GetPtrFromSerial(wForceSerial);
    if (!forceItem)
    {
      byErrCode = 1;
    }
    else if (forceItem->m_bLock)
    {
      byErrCode = 17;
    }
  }

  if (!byErrCode)
  {
    const int effectIndex = CPlayer::s_pnLinkForceItemToEffect
      ? CPlayer::s_pnLinkForceItemToEffect[forceItem->m_wItemIndex]
      : -1;
    forceFld = reinterpret_cast<_force_fld *>(g_Main.m_tblEffectData[1].GetRecord(effectIndex));
    if (!forceFld)
    {
      byErrCode = 1;
    }
  }

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 1u, forceFld->m_dwIndex, forceFld->m_nMastIndex))
  {
    byErrCode = 9;
  }
  if (!byErrCode && forceFld->m_nTempEffectType >= 150)
  {
    byErrCode = 21;
  }

  if (!byErrCode)
  {
    target = GetLiveTargetInSameMap(this, pidDst);
    if (!target)
    {
      byErrCode = 2;
    }
    else if (!IsEffectableDst(forceFld->m_strActableDst, target))
    {
      byErrCode = 5;
    }
    else if (forceFld->m_nContEffectType == 0 && !IsAttackableInTown() && !target->IsAttackableInTown())
    {
      const bool inGuildRoom = m_Param.m_pGuild
        && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
          m_Param.m_pGuild->m_dwSerial,
          m_ObjID.m_wIndex,
          m_pUserDB->m_dwSerial);
      if (IsInTown() || target->IsInTown() || inGuildRoom)
      {
        byErrCode = 18;
      }
    }
  }

  if (!byErrCode && forceFld->m_nTempEffectType == -1 && forceFld->m_nContEffectType == -1)
  {
    byErrCode = 8;
  }
  if (!byErrCode && forceFld->m_nContEffectType != -1 && !target->IsRecvableContEffect())
  {
    byErrCode = 13;
  }
  if (!byErrCode && target->m_EP.GetEff_State(20)
      && (forceFld->m_nTempEffectType == -1 || !IsUsableTempEffectAtStoneState(forceFld->m_nTempEffectType)))
  {
    byErrCode = 24;
  }
  if (!byErrCode && target->m_EP.GetEff_State(28))
  {
    byErrCode = 24;
  }

  if (!byErrCode && !IsSFUsableSFMastery(4u, forceFld->m_nMastIndex))
  {
    byErrCode = 3;
  }
  if (!byErrCode && !IsSFActableByClass(1u, reinterpret_cast<_base_fld *>(forceFld)))
  {
    byErrCode = 16;
  }
  if (!byErrCode && !IsSFUsableGauge(1u, static_cast<unsigned __int16>(forceFld->m_dwIndex), delPoint))
  {
    byErrCode = 6;
  }
  if (!byErrCode && !GetUseConsumeItem(forceFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode)
  {
    forceLevel = GetSFLevel(forceFld->m_nLv, forceItem->m_dwDur);
    const bool hadStealth = GetStealth(true);
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistForce(target, forceFld, forceLevel, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      if (upMastery && !IsInTown())
      {
        bool canGainMastery = true;
        if (IsChaosMode() && GetObjRace() == target->GetObjRace() && forceFld->m_nContEffectType == 0)
        {
          if (target->m_ObjID.m_byID == 0)
          {
            CPlayer *targetPlayer = static_cast<CPlayer *>(target);
            if (targetPlayer->IsPunished(1u, 0))
            {
              canGainMastery = false;
            }
          }
          else
          {
            canGainMastery = false;
          }
        }

        if (canGainMastery)
        {
          Emb_AlterStat(4u, forceFld->m_nMastIndex, 1u, 0, "CPlayer::pc_ForceRequest()---0", true);

          const int level = static_cast<int>(GetLevel());
          const int baseAlter = level / 10 + 1;
          int alter = baseAlter;
          if (IsApplyPcbangPrimium())
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(PCBANG_PRIMIUM_FAVOR::SKILL_FORCE_MASTERY - 1.0f));
          }
          else
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(FORCE_LIVER_ACCUM_RATE - 1.0f));
          }

          const float effHave = m_EP.GetEff_Have(6);
          if (effHave > 1.0f)
          {
            alter = static_cast<int>(
              static_cast<float>(alter)
              + static_cast<float>(baseAlter)
                  * static_cast<float>(effHave - 1.0f));
          }

          const unsigned int oldDur = forceItem->m_dwDur;
          unsigned int newDur = oldDur;
          if (alter > 0)
          {
            newDur = Emb_AlterDurPoint(forceItem->m_pInList->m_nListCode, forceItem->m_byStorageIndex, alter, 0, 0);
          }
          if (newDur != oldDur)
          {
            SendMsg_FcitemInform(wForceSerial, newDur);
          }
        }
      }

      ApplyGaugeConsumeAndSendRecover(this, delPoint);
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(13);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(forceFld->m_fActDelay + addDelay));
      if (hadStealth)
      {
        BreakStealth();
      }
    }
  }

  this->SendMsg_ForceResult(byErrCode, pidDst, forceItem, static_cast<unsigned __int8>(forceLevel));
}

void CPlayer::pc_ThrowSkillRequest(unsigned __int16 wBulletSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *bulletItem = nullptr;
  _BulletItem_fld *bulletRecord = nullptr;
  _skill_fld *skillFld = nullptr;
  CCharacter *target = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  unsigned __int8 bySkillIndex = static_cast<unsigned __int8>(-1);

  if (IsRidingUnit())
  {
    byErrCode = 14;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 28;
  }
  else if (m_EP.GetEff_State(20) || m_EP.GetEff_State(28))
  {
    byErrCode = 24;
  }
  else if (m_EP.GetEff_State(21))
  {
    byErrCode = 25;
  }
  else if (m_pmWpn.byWpType != 11 && m_pmWpn.byWpType != 6)
  {
    byErrCode = 22;
  }

  if (!byErrCode && m_pmWpn.byWpType == 6
      && (!m_pmWpn.pFixWp
          || m_pmWpn.pFixWp->m_byTableCode != 6
          || m_pmWpn.pFixWp->m_wItemIndex != 8518))
  {
    byErrCode = 22;
  }

  if (!byErrCode)
  {
    bulletItem = IsBulletValidity(wBulletSerial);
    if (!bulletItem)
    {
      byErrCode = 19;
    }
    else if (bulletItem->m_bLock)
    {
      byErrCode = 20;
    }
    else
    {
      bulletRecord = reinterpret_cast<_BulletItem_fld *>(g_Main.m_tblItemData[10].GetRecord(bulletItem->m_wItemIndex));
      if (!bulletRecord)
      {
        byErrCode = 19;
      }
      else if (!strncmp(bulletRecord->m_strEffectIndex, "-1", 2))
      {
        byErrCode = 19;
      }
      else
      {
        skillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[3].GetRecord(bulletRecord->m_strEffectIndex));
        if (!skillFld)
        {
          byErrCode = 8;
        }
      }
    }
  }

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    byErrCode = 9;
  }

  if (!byErrCode)
  {
    target = GetLiveTargetInSameMap(this, pidDst);
    if (!target)
    {
      byErrCode = 2;
    }
    else if (!IsEffectableDst(skillFld->m_strActableDst, target))
    {
      byErrCode = 5;
    }
    else if (skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
    {
      byErrCode = 8;
    }
    else if (skillFld->m_nContEffectType != -1)
    {
      if (!target->IsRecvableContEffect())
      {
        byErrCode = 13;
      }
      else if (skillFld->m_nContEffectType == 0 && !IsAttackableInTown() && !target->IsAttackableInTown())
      {
        const bool inGuildRoom = m_Param.m_pGuild
          && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial);
        if (IsInTown() || target->IsInTown() || inGuildRoom)
        {
          byErrCode = 18;
        }
      }
    }
  }

  if (!byErrCode && !GetUseConsumeItem(skillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode && skillFld->m_nContEffectType != -1)
  {
    CPlayer *dstPlayer = target && target->m_ObjID.m_byID == 0 ? static_cast<CPlayer *>(target) : nullptr;
    if (skillFld->m_nContEffectType == 1)
    {
      if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, false))
      {
        byErrCode = 13;
      }
    }
    else if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, true))
    {
      byErrCode = 13;
    }
  }

  g_tmpEffectedNum = 0;
  if (!byErrCode)
  {
    bySkillIndex = static_cast<unsigned __int8>(skillFld->m_dwIndex);
    const bool hadStealth = GetStealth(true);
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistSkill(target, 3u, skillFld, 1, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      const unsigned __int16 leftDur = Emb_AlterDurPoint(2u, bulletItem->m_byStorageIndex, -1, 0, 1);
      if (leftDur)
      {
        SendMsg_AlterWeaponBulletInform(bulletItem->m_wSerial, leftDur);
      }
      else
      {
        CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, bulletItem, m_szItemHistoryFileName);
      }

      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);

      const int addDelay = static_cast<int>(CalcEquipAttackDelay());
      const int level = static_cast<int>(GetLevel());
      int attackDelay = static_cast<int>(m_pmWpn.GetAttackDelay(level, addDelay));
      if (m_pmWpn.byWpType != 11 && !m_pmWpn.byWpClass)
      {
        attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(9));
      }
      if (m_pmWpn.byWpType == 7 || m_pmWpn.byWpType == 11)
      {
        attackDelay = static_cast<int>(static_cast<float>(attackDelay) + m_EP.GetEff_Plus(11));
      }
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(attackDelay));

      if (hadStealth)
      {
        BreakStealth();
      }
    }
  }

  this->SendMsg_ThrowSkillResult(byErrCode, pidDst, bySkillIndex);
}

void CPlayer::pc_ThrowUnitRequest(_CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  _UnitBullet_fld *bulletRecord = nullptr;
  _UnitPart_fld *weaponRecord = nullptr;
  _skill_fld *skillFld = nullptr;
  CCharacter *target = nullptr;
  unsigned int *bulletParam = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  unsigned __int16 wBulletIndex = static_cast<unsigned __int16>(-1);

  if (!IsRidingUnit())
  {
    byErrCode = 23;
  }
  else
  {
    bulletParam = &m_pUsingUnit->dwBullet[1];
    const unsigned __int16 bulletLeft = HIWORD(*bulletParam);
    const unsigned __int16 bulletItemIndex = LOWORD(*bulletParam);
    if (!bulletLeft || bulletLeft == 0xFFFF)
    {
      byErrCode = 19;
    }
    else
    {
      bulletRecord = reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(bulletItemIndex));
      weaponRecord = reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[4].GetRecord(m_pUsingUnit->byPart[4]));
      if (!bulletRecord || !weaponRecord)
      {
        byErrCode = 21;
      }
      else if (!strncmp(bulletRecord->m_strEffectIndex, "-1", 2))
      {
        byErrCode = 19;
      }
      else
      {
        skillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[3].GetRecord(bulletRecord->m_strEffectIndex));
        if (!skillFld)
        {
          byErrCode = 8;
        }
      }
    }
  }

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0xFFu, 0xFFu, 0xFFu))
  {
    byErrCode = 9;
  }

  if (!byErrCode)
  {
    target = GetLiveTargetInSameMap(this, pidDst);
    if (!target)
    {
      byErrCode = 2;
    }
    else if (!IsEffectableDst(skillFld->m_strActableDst, target))
    {
      byErrCode = 5;
    }
    else if (skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
    {
      byErrCode = 8;
    }
    else if (skillFld->m_nContEffectType != -1)
    {
      if (!target->IsRecvableContEffect())
      {
        byErrCode = 13;
      }
      else if (skillFld->m_nContEffectType == 0 && !IsAttackableInTown() && !target->IsAttackableInTown())
      {
        const bool inGuildRoom = m_Param.m_pGuild
          && CGuildRoomSystem::GetInstance()->IsGuildRoomMemberIn(
            m_Param.m_pGuild->m_dwSerial,
            m_ObjID.m_wIndex,
            m_pUserDB->m_dwSerial);
        if (IsInTown() || target->IsInTown() || inGuildRoom)
        {
          byErrCode = 18;
        }
      }
    }
  }

  if (!byErrCode && !GetUseConsumeItem(skillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode && skillFld->m_nContEffectType != -1)
  {
    CPlayer *dstPlayer = target && target->m_ObjID.m_byID == 0 ? static_cast<CPlayer *>(target) : nullptr;
    if (skillFld->m_nContEffectType == 1)
    {
      if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, false))
      {
        byErrCode = 13;
      }
    }
    else if (dstPlayer && _pre_check_in_guild_battle_race(dstPlayer, true))
    {
      byErrCode = 13;
    }
  }

  g_tmpEffectedNum = 0;
  if (!byErrCode)
  {
    wBulletIndex = static_cast<unsigned __int16>(bulletRecord->m_dwIndex);
    const bool hadStealth = GetStealth(true);
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistSkill(target, 3u, skillFld, 1, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      unsigned __int16 bulletLeft = HIWORD(*bulletParam);
      if (bulletLeft > 0)
      {
        --bulletLeft;
      }
      SendMsg_AlterUnitBulletInform(1u, bulletLeft);
      if (!bulletLeft)
      {
        *bulletParam = static_cast<unsigned int>(-1);
      }

      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      _ATTACK_DELAY_CHECKER::SetDelay(&m_AttDelayChker, static_cast<unsigned int>(weaponRecord->m_nAttackDel));
      if (hadStealth)
      {
        BreakStealth();
      }
    }
  }

  this->SendMsg_ThrowUnitResult(byErrCode, pidDst, wBulletIndex);
}

void CPlayer::pc_MakeTowerRequest(
  unsigned __int16 wSkillIndex,
  unsigned __int16 wTowerItemSerial,
  unsigned __int8 byMaterialNum,
  _make_tower_request_clzo::__material *pMaterial,
  float *pfPos,
  unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  unsigned int dwTowerObjSerial = static_cast<unsigned int>(-1);
  _skill_fld *classSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  _GuardTowerItem_fld *towerItemInfo = nullptr;
  _STORAGE_LIST::_db_con *towerItem = nullptr;
  _STORAGE_LIST::_db_con *materialItems[30]{};
  unsigned __int8 materialTotals[3]{};
  int classGrade[2] = {-1, -1};
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int needFp = 0;

  if (m_byPosRaceTown != 0xFF)
  {
    byErrCode = 16;
  }
  else if (!classSkill)
  {
    byErrCode = 13;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 13;
  }
  else if (m_pmTwr.m_nCount >= 6)
  {
    byErrCode = 11;
  }
  else if (!m_Param.IsActableClassSkill(classSkill->m_strCode, classGrade))
  {
    byErrCode = 13;
  }
  else if (!m_bSFDelayNotCheck
           && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, classSkill->m_dwIndex, classGrade[0]))
  {
    byErrCode = 18;
  }
  else if (!GetUseConsumeItem(classSkill->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 20;
  }
  else if (GetSqrt(m_fCurPos, pfPos) > 40.0)
  {
    byErrCode = 9;
  }
  else if (IsOtherTowerNear(this, pfPos, nullptr))
  {
    byErrCode = 15;
  }
  else if (classSkill->m_nTempEffectType == -1)
  {
    byErrCode = 12;
  }

  if (!byErrCode)
  {
    needFp = static_cast<int>(static_cast<float>(classSkill->m_nNeedFP) * m_EP.GetEff_Rate(7));
    if (needFp > GetFP())
    {
      byErrCode = 14;
    }
  }

  if (!byErrCode)
  {
    towerItem = m_Param.m_dbInven.GetPtrFromSerial(wTowerItemSerial);
    if (!towerItem)
    {
      byErrCode = 2;
    }
    else if (towerItem->m_byTableCode != 25)
    {
      byErrCode = 3;
    }
    else if (towerItem->m_bLock)
    {
      byErrCode = 17;
    }
    else
    {
      towerItemInfo = reinterpret_cast<_GuardTowerItem_fld *>(g_Main.m_tblItemData[25].GetRecord(towerItem->m_wItemIndex));
      if (!towerItemInfo)
      {
        byErrCode = 12;
      }
    }
  }

  if (!byErrCode)
  {
    for (int index = 0; index < byMaterialNum; ++index)
    {
      const unsigned __int16 serial = pMaterial[index].wItemSerial;
      const unsigned __int8 slot = pMaterial[index].byMaterSlotIndex;
      const unsigned __int8 amount = pMaterial[index].byAmount;
      if (slot >= 3)
      {
        byErrCode = 4;
        break;
      }

      for (int prev = 0; prev < index; ++prev)
      {
        if (serial == pMaterial[prev].wItemSerial)
        {
          byErrCode = 4;
          break;
        }
      }
      if (byErrCode)
      {
        break;
      }

      _STORAGE_LIST::_db_con *material = m_Param.m_dbInven.GetPtrFromSerial(serial);
      materialItems[index] = material;
      if (!material)
      {
        byErrCode = 5;
        break;
      }

      _base_fld *materialRecord = g_Main.m_tblItemData[material->m_byTableCode].GetRecord(material->m_wItemIndex);
      if (!materialRecord || strncmp(materialRecord->m_strCode, towerItemInfo->m_Material[slot].strMaterialCode, 7))
      {
        byErrCode = 6;
        break;
      }
      if (static_cast<unsigned __int64>(amount) > material->m_dwDur)
      {
        byErrCode = 7;
        break;
      }
      materialTotals[slot] = static_cast<unsigned __int8>(materialTotals[slot] + amount);
    }

    if (!byErrCode)
    {
      for (int slot = 0; slot < 3; ++slot)
      {
        if (materialTotals[slot] != towerItemInfo->m_Material[slot].nMaterialNum)
        {
          byErrCode = 8;
          break;
        }
      }
    }
  }

  if (!byErrCode)
  {
    const int currentFp = GetFP();
    const int remainFp = (currentFp > needFp) ? (currentFp - needFp) : 0;
    SetFP(remainFp, true);

    CGuardTower *tower = CreateGuardTower(
      m_pCurMap,
      m_wMapLayerIndex,
      pfPos,
      towerItem,
      this,
      static_cast<unsigned __int8>(m_Param.GetRaceCode()),
      false);
    if (!tower)
    {
      byErrCode = 1;
    }
    else
    {
      for (int index = 0; index < byMaterialNum; ++index)
      {
        Emb_AlterDurPoint(0u, materialItems[index]->m_byStorageIndex, -pMaterial[index].byAmount, false, false);
      }
      towerItem->lock(true);
      m_pmTwr.PushList(towerItem, tower);
      ++m_pmTwr.m_nCount;
      dwTowerObjSerial = tower->m_dwObjSerial;
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(classSkill->m_fActDelay + addDelay));
    }
  }

  this->SendMsg_CreateTowerResult(static_cast<char>(byErrCode), dwTowerObjSerial);
}

void CPlayer::pc_BackTowerRequest(unsigned int dwTowerObjSerial)
{
  unsigned __int8 byErrCode = 0;
  _STORAGE_LIST::_db_con *towerItem = nullptr;
  CGuardTower *tower = nullptr;
  int towerSlot = -1;

  for (int index = 0; index < 6; ++index)
  {
    if (m_pmTwr.m_List[index].m_pTowerItem
        && m_pmTwr.m_List[index].m_pTowerObj
        && m_pmTwr.m_List[index].m_pTowerObj->m_dwObjSerial == dwTowerObjSerial)
    {
      towerSlot = index;
      tower = m_pmTwr.m_List[index].m_pTowerObj;
      towerItem = m_Param.m_dbInven.GetPtrFromSerial(m_pmTwr.m_List[index].m_pTowerItem->m_wSerial);
      break;
    }
  }

  if (!towerItem)
  {
    byErrCode = 2;
  }
  else if (towerItem->m_byTableCode != 25)
  {
    byErrCode = 3;
  }

  unsigned __int16 wItemSerial = static_cast<unsigned __int16>(-1);
  unsigned __int16 wLeftHp = 0;
  if (!byErrCode)
  {
    wItemSerial = towerItem->m_wSerial;
    if (tower)
    {
      const int nAlter = tower->m_nHP - static_cast<int>(towerItem->m_dwDur);
      Emb_AlterDurPoint(0u, towerItem->m_byStorageIndex, nAlter, false, false);
      int hp = tower->m_nHP;
      if (hp < 0)
      {
        hp = 0;
      }
      else if (hp > 0xFFFF)
      {
        hp = 0xFFFF;
      }
      wLeftHp = static_cast<unsigned __int16>(hp);
      tower->Destroy(2u, false);
    }
    towerItem->lock(false);
    if (towerSlot >= 0)
    {
      m_pmTwr.m_List[towerSlot].init();
      --m_pmTwr.m_nCount;
    }
  }

  this->SendMsg_BackTowerResult(static_cast<char>(byErrCode), wItemSerial, wLeftHp);
}

void CPlayer::pc_MakeTrapRequest(
  unsigned __int16 wSkillIndex,
  unsigned __int16 wTrapItemSerial,
  float *pfPos,
  unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  unsigned int dwTrapObjSerial = static_cast<unsigned int>(-1);
  _skill_fld *classSkill = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  _TrapItem_fld *trapItemInfo = nullptr;
  _STORAGE_LIST::_db_con *trapItem = nullptr;
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};
  int classGrade[2] = {-1, -1};
  int needFp = 0;

  if (m_byPosRaceTown != 0xFF)
  {
    byErrCode = 16;
  }
  else if (!classSkill)
  {
    byErrCode = 13;
  }
  else if (m_byMoveType == 2)
  {
    byErrCode = 13;
  }
  else if (m_pmTrp.m_nCount >= m_Param.m_nMakeTrapMaxNum)
  {
    byErrCode = 11;
  }
  else if (!m_Param.IsActableClassSkill(classSkill->m_strCode, classGrade))
  {
    byErrCode = 13;
  }
  else if (!m_bSFDelayNotCheck
           && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, classSkill->m_dwIndex, classGrade[0]))
  {
    byErrCode = 18;
  }
  else if (!GetUseConsumeItem(classSkill->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 20;
  }
  else if (GetSqrt(m_fCurPos, pfPos) > 40.0)
  {
    byErrCode = 9;
  }
  else
  {
    trapItem = m_Param.m_dbInven.GetPtrFromSerial(wTrapItemSerial);
    if (!trapItem)
    {
      byErrCode = 2;
    }
    else if (trapItem->m_byTableCode != 26)
    {
      byErrCode = 3;
    }
    else
    {
      trapItemInfo = reinterpret_cast<_TrapItem_fld *>(g_Main.m_tblItemData[26].GetRecord(trapItem->m_wItemIndex));
      if (!trapItemInfo)
      {
        byErrCode = 2;
      }
      else
      {
        byErrCode = static_cast<unsigned __int8>(IsOtherInvalidObjNear(this, pfPos, nullptr, trapItemInfo));
      }
    }
  }

  if (!byErrCode && classSkill->m_nTempEffectType == -1)
  {
    byErrCode = 12;
  }
  if (!byErrCode)
  {
    needFp = static_cast<int>(static_cast<float>(classSkill->m_nNeedFP) * m_EP.GetEff_Rate(7));
    if (needFp > GetFP())
    {
      byErrCode = 14;
    }
    else if (trapItemInfo->m_nLevelLim >= 40
             && (!m_Param.m_pClassHistory[0]
                 || m_Param.m_pClassHistory[0]->m_nClass != 1
                 || !m_Param.m_pClassHistory[1]
                 || m_Param.m_pClassHistory[1]->m_nClass != 1))
    {
      byErrCode = 19;
    }
  }

  if (!byErrCode)
  {
    const int currentFp = GetFP();
    const int remainFp = (currentFp > needFp) ? (currentFp - needFp) : 0;
    SetFP(remainFp, true);

    CTrap *trap = CreateTrapObject(this, trapItem->m_wItemIndex, pfPos);
    if (!trap)
    {
      byErrCode = 1;
    }
    else
    {
      if (IsOverLapItem(26))
      {
        Emb_AlterDurPoint(0u, trapItem->m_byStorageIndex, -1, true, true);
      }
      else if (!Emb_DelStorage(0u, trapItem->m_byStorageIndex, false, true, "CPlayer::pc_MakeTrapRequest()"))
      {
        this->SendMsg_CreateTrapResult(12, dwTrapObjSerial);
        return;
      }

      CPlayer::s_MgrItemHistory.consume_del_item(m_ObjID.m_wIndex, trapItem, m_szItemHistoryFileName);
      m_pmTrp.PushItem(trap, trap->m_dwObjSerial);
      SendMsg_MadeTrapNumInform(static_cast<char>(m_pmTrp.m_nCount));
      dwTrapObjSerial = trap->m_dwObjSerial;
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(classSkill->m_fActDelay + addDelay));
    }
  }

  this->SendMsg_CreateTrapResult(static_cast<char>(byErrCode), dwTrapObjSerial);
}

void CPlayer::pc_BackTrapRequest(unsigned int dwTrapObjSerial, unsigned __int16 wAddSerial)
{
  unsigned __int8 byErrCode = 2;
  CTrap *trap = nullptr;
  int trapSlot = -1;

  if (m_pmTrp.m_nCount > 0)
  {
    for (int index = 0; index < 20; ++index)
    {
      if (!m_pmTrp.m_Item[index].isLoad() || m_pmTrp.m_Item[index].dwSerial != dwTrapObjSerial)
      {
        continue;
      }

      trapSlot = index;
      trap = m_pmTrp.m_Item[index].pItem;
      if (!trap)
      {
        byErrCode = 2;
      }
      else if (trap->m_dwMasterSerial != m_dwObjSerial)
      {
        byErrCode = 21;
      }
      else if (GetSqrt(m_fCurPos, trap->m_fCurPos) > 150.0)
      {
        byErrCode = 22;
      }
      else
      {
        byErrCode = 0;
      }
      break;
    }

    if (byErrCode == 2)
    {
      g_Main.m_logSystemError.Write(
        "CPlayer::pc_BackTrapRequest() : Can't find trap (Player:%s Count:%d)",
        m_Param.GetCharNameA(),
        m_pmTrp.m_nCount);
    }
  }
  else
  {
    if (m_pmTrp.m_nCount < 0)
    {
      g_Main.m_logSystemError.Write(
        "CPlayer::pc_BackTrapRequest() : m_pmTrp.m_nCount ZERO and less ( %d )",
        m_pmTrp.m_nCount);
    }
    byErrCode = 2;
  }

  if (!byErrCode && trap)
  {
    const unsigned __int16 trapIndex = static_cast<unsigned __int16>(trap->m_pRecordSet->m_dwIndex);
    bool added = false;

    if (IsOverLapItem(26) && wAddSerial != 0xFFFF)
    {
      _STORAGE_LIST::_db_con *addTarget = m_Param.m_dbInven.GetPtrFromSerial(wAddSerial);
      if (addTarget && addTarget->m_byTableCode == 26 && addTarget->m_wItemIndex == trapIndex)
      {
        const unsigned __int64 dur = Emb_AlterDurPoint(0u, addTarget->m_byStorageIndex, 1, true, true);
        SendMsg_AdjustAmountInform(0u, addTarget->m_wSerial, static_cast<unsigned int>(dur));
        added = true;
      }
    }

    if (!added)
    {
      _STORAGE_LIST::_storage_con item{};
      item.m_bLoad = true;
      item.m_byTableCode = 26;
      item.m_wItemIndex = trapIndex;
      item.m_dwDur = 1;
      item.m_dwLv = 0;
      item.m_wSerial = m_Param.GetNewItemSerial();
      _STORAGE_LIST::_db_con *newItem = Emb_AddStorage(0u, &item, true, true);
      if (!newItem)
      {
        byErrCode = 2;
      }
      else
      {
        SendMsg_RewardAddItem(newItem, 0);
        added = true;
      }
    }

    if (!byErrCode && added)
    {
      trap->Destroy();
      if (trapSlot >= 0)
      {
        m_pmTrp.m_Item[trapSlot].init();
        --m_pmTrp.m_nCount;
      }
      SendMsg_MadeTrapNumInform(static_cast<char>(m_pmTrp.m_nCount));
    }
  }

  this->SendMsg_BackTrapResult(static_cast<char>(byErrCode));
}

void CPlayer::pc_RequestQuestListFromNPC(CItemStore *pStore)
{
  if (!pStore)
  {
    return;
  }

  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 80.0)
  {
    return;
  }

  const int npcRaceCode = pStore->m_byNpcRaceCode;
  const int raceCode = m_Param.GetRaceCode();
  if (npcRaceCode != raceCode && npcRaceCode != 255)
  {
    return;
  }

  char *eventCode = pStore->GetNpcCode();
  if (!eventCode)
  {
    return;
  }

  std::memset(&m_NPCQuestIndexTempData, 0, sizeof(m_NPCQuestIndexTempData));
  m_QuestMgr.CheckNPCQuestList(
    eventCode,
    static_cast<unsigned __int8>(m_Param.GetRaceCode()),
    &m_NPCQuestIndexTempData);
  this->SendMsg_NpcQuestListResult(&m_NPCQuestIndexTempData);
}

void CPlayer::pc_RequestQuestFromNPC(CItemStore *pStore, unsigned int dwNPCQuestIndex)
{
  if (!pStore)
  {
    return;
  }

  float *storePos = pStore->GetStorePos();
  if (GetSqrt(m_fCurPos, storePos) > 80.0)
  {
    return;
  }

  const int npcRaceCode = pStore->m_byNpcRaceCode;
  const int raceCode = m_Param.GetRaceCode();
  if (npcRaceCode != raceCode && npcRaceCode != 255)
  {
    return;
  }

  char *eventCode = pStore->GetNpcCode();
  if (!eventCode)
  {
    return;
  }

  const bool created = Emb_CreateNPCQuest(eventCode, dwNPCQuestIndex);
  this->SendMsg_ResultNpcQuest(created ? 1 : 0);
}

bool CPlayer::Emb_CreateNPCQuest(char *pszEventCode, unsigned int dwNPCQuestIndex)
{
  unsigned int happenIndex = 0;
  bool found = false;
  for (int index = 0; index < 30; ++index)
  {
    if (m_NPCQuestIndexTempData.IndexData[index].dwQuestIndex == dwNPCQuestIndex)
    {
      happenIndex = m_NPCQuestIndexTempData.IndexData[index].dwQuestHappenIndex;
      found = true;
      break;
    }
  }
  if (!found)
  {
    return false;
  }

  _happen_event_cont *happenEvent = m_QuestMgr.CheckNPCQuestStartable(
    pszEventCode,
    static_cast<unsigned __int8>(m_Param.GetRaceCode()),
    static_cast<int>(dwNPCQuestIndex),
    happenIndex);
  if (!happenEvent)
  {
    return false;
  }

  if (Emb_StartQuest(0xFFu, happenEvent))
  {
    return true;
  }

  for (int index = 0; index < 3; ++index)
  {
    if (!m_QuestMgr.m_pTempHappenEvent[index].isset())
    {
      std::memcpy(&m_QuestMgr.m_pTempHappenEvent[index], happenEvent, sizeof(_happen_event_cont));
      break;
    }
  }

  return false;
}
















void CPlayer::SendMsg_AlterBooster()
{
  char payload[3]{};
  payload[0] = static_cast<char>(m_pUsingUnit->bySlotIndex);
  *reinterpret_cast<unsigned __int16 *>(&payload[1]) = m_pUsingUnit->wBooster;

  unsigned __int8 type[2]{23, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(m_ObjID.m_wIndex, type, payload, sizeof(payload));
}

void CPlayer::AutoCharge_Booster()
{
  if (!m_pUsingUnit)
  {
    return;
  }

  _base_fld *unitPartRecord = g_Main.m_tblUnitPart[5].GetRecord(m_pUsingUnit->byPart[5]);
  if (!unitPartRecord)
  {
    return;
  }

  const int maxBooster = *reinterpret_cast<int *>(&unitPartRecord[5].m_strCode[56]);
  if (!maxBooster)
  {
    return;
  }

  const int currentBooster = m_pUsingUnit->wBooster;
  int delta = 0;
  if (m_bMove && !m_pParkingUnit)
  {
    if (m_byMoveType == 1)
    {
      delta = -2;
    }
  }
  else
  {
    delta = 2;
  }

  if (!delta)
  {
    return;
  }

  int nextBooster = currentBooster + delta;
  if (nextBooster < 0)
  {
    nextBooster = 0;
  }
  else if (nextBooster > maxBooster)
  {
    nextBooster = maxBooster;
  }

  m_pUsingUnit->wBooster = static_cast<unsigned __int16>(nextBooster);
  if (currentBooster != nextBooster)
  {
    SendMsg_AlterBooster();
  }
}

void CPlayer::AlterSec()
{
  if (m_pPartyMgr && m_pPartyMgr->IsPartyMode())
  {
    SendData_PartyMemberPos();
  }
}

__int64 CPlayer::GetAvoidRate()
{
  if (m_fTalik_AvoidPoint <= 0.0f)
  {
    return static_cast<unsigned int>(static_cast<int>(m_EP.GetEff_Plus(3)));
  }

  const int maxDp = GetMaxDP();
  float dpRate = 0.0f;
  if (maxDp > 0)
  {
    dpRate = (2.0f * static_cast<float>(GetDP()) / static_cast<float>(maxDp)) - 0.4f;
    if (dpRate < 0.0f)
    {
      dpRate = 0.0f;
    }
    else if (dpRate > 1.0f)
    {
      dpRate = 1.0f;
    }
  }

  const int talikPenalty = static_cast<int>(m_fTalik_AvoidPoint * (1.0f - dpRate));
  const float avoidValue = m_EP.GetEff_Plus(3) - static_cast<float>(talikPenalty);
  return static_cast<unsigned int>(static_cast<int>(avoidValue));
}

__int64 CPlayer::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
  m_nLastBeatenPart = nAttactPart;

  float defenseValue = 0.0f;
  if (IsRidingUnit())
  {
    defenseValue = static_cast<float>(m_nUnitDefFc) * m_fUnitPv_DefFc;
  }
  else
  {
    bool ignoreShieldByEffect = false;
    if (pAttChar && pAttChar->m_ObjID.m_byID == 0)
    {
      ignoreShieldByEffect = pAttChar->m_EP.GetEff_State(11);
      if (!ignoreShieldByEffect)
      {
        float ignoreProb = pAttChar->m_EP.GetEff_Plus(28);
        if (ignoreProb > 0.0f)
        {
          ignoreProb += pAttChar->m_EP.GetEff_Plus(41);
          if ((rand() % 100) <= static_cast<int>(ignoreProb))
          {
            ignoreShieldByEffect = true;
          }
        }
      }
      if (ignoreShieldByEffect)
      {
        pAttChar->SendMsg_AttackActEffect(1u, this);
      }
    }

    _STORAGE_LIST::_db_con *shieldItem = &m_Param.m_dbEquip.m_pStorageList[5];
    bool shieldEnabled = false;
    if (shieldItem->m_bLoad)
    {
      shieldEnabled = GetEffectEquipCode(1u, 5u) == 1;
      if (shieldEnabled)
      {
        _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
        if (weaponItem->m_bLoad)
        {
          _base_fld *weaponRecord = g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex);
          if (weaponRecord && *reinterpret_cast<int *>(&weaponRecord[4].m_strCode[12]) == 100)
          {
            shieldEnabled = false;
          }
        }
      }
    }

    if (pAttChar && shieldEnabled && !ignoreShieldByEffect)
    {
      const int masteryValue = m_pmMst.GetMasteryPerMast(2u, 0u);
      int blockChance = static_cast<int>((static_cast<float>(masteryValue) / 99.0f) * 20.0f + 5.0f);
      blockChance += static_cast<int>(m_EP.GetEff_Plus(29));
      if (blockChance > 100)
      {
        blockChance = 100;
      }
      if ((rand() % 100) < blockChance)
      {
        m_nLastBeatenPart = 5;
        if (pnConvertPart)
        {
          *pnConvertPart = m_nLastBeatenPart;
        }
        return 0xFFFFFFFEu;
      }
    }

    if (m_nLastBeatenPart < 0 || m_nLastBeatenPart >= 5)
    {
      m_nLastBeatenPart = rand() % 5;
    }

    const int partIndex = m_nLastBeatenPart;
    int partCode = m_Param.m_dbChar.m_byDftPart[partIndex];
    _STORAGE_LIST::_db_con *partItem = &m_Param.m_dbEquip.m_pStorageList[partIndex];
    if (partItem->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(partIndex)) == 1)
    {
      partCode = partItem->m_wItemIndex;
    }

    _base_fld *partRecord = g_Main.m_tblItemData[partIndex].GetRecord(partCode);
    if (partRecord)
    {
      defenseValue = *reinterpret_cast<float *>(&partRecord[5].m_strCode[44]);
    }

    if (shieldEnabled && m_nLastBeatenPart == 5)
    {
      _base_fld *shieldRecord = g_Main.m_tblItemData[5].GetRecord(shieldItem->m_wItemIndex);
      if (shieldRecord)
      {
        defenseValue = *reinterpret_cast<float *>(&shieldRecord[5].m_strCode[44]);
      }
    }

    if (pnConvertPart)
    {
      *pnConvertPart = m_nLastBeatenPart;
    }

    defenseValue *= m_EP.GetEff_Rate(33);
    if (IsSiegeMode())
    {
      defenseValue *= m_EP.GetEff_Rate(24);
    }
  }

  if (!m_bInGuildBattle)
  {
    const unsigned int charSerial = m_Param.GetCharSerial();
    const int raceCode = m_Param.GetRaceCode();
    const unsigned __int8 bossType =
      CPvpUserAndGuildRankingSystem::Instance()->GetBossType(raceCode, charSerial);
    switch (bossType)
    {
    case 0:
      defenseValue *= 1.3f;
      break;
    case 1:
    case 5:
      defenseValue *= 1.5f;
      break;
    case 3:
    case 7:
      defenseValue *= 1.2f;
      break;
    default:
      break;
    }
  }

  if (g_HolySys.GetDestroyerSerial() == m_dwObjSerial || IsLastAttBuff())
  {
    defenseValue *= 1.3f;
  }

  if (!IsRidingUnit())
  {
    const int maxDp = GetMaxDP();
    float dpRate = 0.0f;
    if (maxDp > 0)
    {
      dpRate = (2.0f * static_cast<float>(GetDP()) / static_cast<float>(maxDp)) - 0.4f;
      if (dpRate < 0.0f)
      {
        dpRate = 0.0f;
      }
      else if (dpRate > 1.0f)
      {
        dpRate = 1.0f;
      }
    }

    if (m_fTalik_DefencePoint > 0.0f)
    {
      const float talikPenalty = m_fTalik_DefencePoint * (1.0f - dpRate);
      defenseValue *= (m_EP.GetEff_Rate(6) - talikPenalty);
    }
    else
    {
      defenseValue *= m_EP.GetEff_Rate(6);
    }
  }

  return static_cast<unsigned int>(static_cast<int>(defenseValue));
}

float CPlayer::GetDefFacing(int nPart)
{
  if (IsRidingUnit())
  {
    if (m_pUsingUnit)
    {
      _base_fld *record = g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
      if (record)
      {
        return *reinterpret_cast<float *>(&record[1].m_strCode[8]);
      }
    }
    return FLOAT_0_5;
  }

  if (nPart >= 8)
  {
    return FLOAT_0_5;
  }

  int partCode = m_Param.m_dbChar.m_byDftPart[nPart];
  _STORAGE_LIST::_db_con *equipPart = &m_Param.m_dbEquip.m_pStorageList[nPart];
  if (equipPart->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(nPart)) == 1)
  {
    partCode = equipPart->m_wItemIndex;
  }

  _base_fld *record = g_Main.m_tblItemData[nPart].GetRecord(partCode);
  if (!record)
  {
    return FLOAT_0_5;
  }
  return *reinterpret_cast<float *>(&record[5].m_strCode[60]);
}

float CPlayer::GetDefGap(int nPart)
{
  if (IsRidingUnit())
  {
    if (m_pUsingUnit)
    {
      _base_fld *record = g_Main.m_tblUnitFrame.GetRecord(m_pUsingUnit->byFrame);
      if (record)
      {
        return *reinterpret_cast<float *>(&record[1].m_strCode[4]);
      }
    }
    return FLOAT_0_5;
  }

  if (nPart >= 8)
  {
    return FLOAT_0_5;
  }

  int partCode = m_Param.m_dbChar.m_byDftPart[nPart];
  _STORAGE_LIST::_db_con *equipPart = &m_Param.m_dbEquip.m_pStorageList[nPart];
  if (equipPart->m_bLoad && GetEffectEquipCode(1u, static_cast<unsigned __int8>(nPart)) == 1)
  {
    partCode = equipPart->m_wItemIndex;
  }

  _base_fld *record = g_Main.m_tblItemData[nPart].GetRecord(partCode);
  if (!record)
  {
    return FLOAT_0_5;
  }
  return *reinterpret_cast<float *>(&record[5].m_strCode[56]);
}

__int64 CPlayer::GetDefSkill(bool bBackAttackDamage)
{
if (!IsRidingUnit())
  {
    return m_pmMst.GetMasteryPerMast(1u, 0u);
  }

  int totalDefSkill = 0;
  _base_fld *leftPart = g_Main.m_tblUnitPart[0].GetRecord(m_pUsingUnit->byPart[0]);
  if (leftPart)
  {
    totalDefSkill += *reinterpret_cast<int *>(&leftPart[5].m_strCode[12]);
  }

  _base_fld *rightPart = g_Main.m_tblUnitPart[1].GetRecord(m_pUsingUnit->byPart[1]);
  if (rightPart)
  {
    totalDefSkill += *reinterpret_cast<int *>(&rightPart[5].m_strCode[12]);
  }

  return static_cast<unsigned int>(totalDefSkill);
}

__int64 CPlayer::GetFireTol()
{
  const float total = static_cast<float>(m_nTolValue[0]) + m_EP.GetEff_Plus(15);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(25));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

char *CPlayer::GetObjName()
{
  std::snprintf(
    s_playerObjectName,
    sizeof(s_playerObjectName),
    "[PLAYER][%d] >> %s (pos: %s {%d, %d, %d})",
    static_cast<int>(GetObjRace()),
    m_Param.GetCharNameA(),
    m_pCurMap ? m_pCurMap->m_pMapSet->m_strCode : "<null-map>",
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]));
  return s_playerObjectName;
}

__int64 CPlayer::GetObjRace()
{
  return static_cast<unsigned int>(m_Param.GetRaceCode());
}

__int64 CPlayer::GetSoilTol()
{
  const float total = static_cast<float>(m_nTolValue[2]) + m_EP.GetEff_Plus(17);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(27));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

__int64 CPlayer::GetWaterTol()
{
  const float total = static_cast<float>(m_nTolValue[1]) + m_EP.GetEff_Plus(16);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(26));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

float CPlayer::GetWeaponAdjust()
{
  if (IsRidingUnit())
  {
    return FLOAT_1_0;
  }

  _STORAGE_LIST::_db_con *weaponItem = &m_Param.m_dbEquip.m_pStorageList[6];
  if (!weaponItem->m_bLoad || GetEffectEquipCode(1u, 6u) != 1)
  {
    return 0.0f;
  }

  _base_fld *weaponRecord = g_Main.m_tblItemData[6].GetRecord(weaponItem->m_wItemIndex);
  if (!weaponRecord)
  {
    return 0.0f;
  }
  if (*reinterpret_cast<int *>(&weaponRecord[6].m_strCode[8]) == 10)
  {
    return 0.0f;
  }

  return *reinterpret_cast<float *>(&weaponRecord[9].m_strCode[40]);
}

__int64 CPlayer::GetWeaponClass()
{
  return m_pmWpn.byWpClass;
}

float CPlayer::GetWidth()
{
  return *reinterpret_cast<float *>(&m_pRecordSet[2].m_strCode[12]);
}

__int64 CPlayer::GetWindTol()
{
  const float total = static_cast<float>(m_nTolValue[3]) + m_EP.GetEff_Plus(18);
  int value = static_cast<int>(total * m_EP.GetEff_Rate(28));
  value = ClampToleranceValue(value);
  if (m_EP.GetEff_State(19) && value > 0)
  {
    value = -value;
  }
  return static_cast<unsigned int>(value);
}

char CPlayer::IsBeDamagedAble(CCharacter *pAtter)
{
  if (!pAtter)
  {
    return 0;
  }

  if (pAtter->m_ObjID.m_byID != 0)
  {
    return 1;
  }

  CPlayer *attacker = static_cast<CPlayer *>(pAtter);
  if (!attacker->m_bInGuildBattle && m_bInGuildBattle)
  {
    return 0;
  }
  if (attacker->m_bInGuildBattle && !m_bInGuildBattle)
  {
    return 0;
  }
  if (!attacker->m_bInGuildBattle && !m_bInGuildBattle)
  {
    return 1;
  }

  return attacker->m_byGuildBattleColorInx != m_byGuildBattleColorInx;
}

char CPlayer::IsRecvableContEffect()
{
  if (IsRidingUnit())
  {
    return 0;
  }
  if (m_EP.GetEff_State(20))
  {
    return 0;
  }
  return m_EP.GetEff_State(28) ? 0 : 1;
}

bool CPlayer::Is_Battle_Mode()
{
  return m_byBattleMode != 0;
}



void CPlayer::OutOfSec()
{
  if (m_pUserDB)
  {
    char buffer[144]{};
    std::snprintf(
      buffer,
      sizeof(buffer),
      "CLOSE>> OutOfSec() ID: %s, NM: %s",
      m_pUserDB->m_szAccountID,
      m_Param.GetCharNameA());
    g_Network.Close(0, m_ObjID.m_wIndex, false, buffer);
  }
  else
  {
    g_Network.Close(0, m_ObjID.m_wIndex, false, nullptr);
  }

  m_bOutOfMap = true;
}

void CPlayer::RecvKillMessage(CCharacter *pDier)
{
  if (!pDier)
  {
    return;
  }

  if (pDier->m_ObjID.m_byID)
  {
    if (pDier->m_ObjID.m_byID != 1)
    {
      return;
    }

    CMonster *deadMonster = static_cast<CMonster *>(pDier);
    m_QuestMgr.CheckFailLoop(3, deadMonster->m_pRecordSet->m_strCode);

    if (m_pUserDB && CActionPointSystemMgr::Instance()->GetEventStatus(1u) == 2)
    {
      const int killerLevel = static_cast<int>(m_Param.GetLevel());
      const int deadLevel = static_cast<int>(deadMonster->GetLevel());
      if (std::abs(killerLevel - deadLevel) < 10)
      {
        const unsigned int currentPoint = m_pUserDB->GetActPoint(1u);
        unsigned int addPoint = 0;
        if (deadMonster->m_pMonRec && deadMonster->m_pMonRec->m_nKillPoint > 0)
        {
          addPoint = static_cast<unsigned int>(deadMonster->m_pMonRec->m_nKillPoint);
        }
        const unsigned int nextPoint = currentPoint + addPoint;
        m_pUserDB->Update_User_Action_Point(1u, nextPoint);
        SendMsg_Alter_Action_Point(1u, nextPoint);
      }
    }

    bool handledByDarkHole = false;
    if (m_pDHChannel && deadMonster->m_pMonRec)
    {
      handledByDarkHole =
        m_pDHChannel->CheckEvent(dh_event_hunt, 255, static_cast<int>(deadMonster->m_pMonRec->m_dwIndex), 1, deadMonster)
        != 0;
    }

    if (!handledByDarkHole)
    {
      Emb_CheckActForQuest(3, deadMonster->m_pRecordSet->m_strCode, 1u, false);
      if (m_pPartyMgr->IsPartyMode())
      {
        Emb_CheckActForQuestParty(3, deadMonster->m_pRecordSet->m_strCode, 1u);
      }
    }
    return;
  }

  CPlayer *deadPlayer = static_cast<CPlayer *>(pDier);
  char *raceCode = cvt_string(deadPlayer->m_Param.GetRaceCode());
  if (!Emb_CreateQuestEvent(quest_happen_type_pk, raceCode))
  {
    Emb_CheckActForQuest(2, raceCode, 1u, false);
  }
}

void CPlayer::SendMsg_FixPosition(int n)
{

  if (m_bObserver && !g_Player[n].m_byUserDgr)
  {
    return;
  }

  _player_fixpositon_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wEquipVer = static_cast<unsigned __int16>(GetVisualVer());
  msg.byRaceCode = m_Param.GetRaceSexCode();
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.wLastEffectCode = static_cast<unsigned __int16>(m_wLastContEffect);
  msg.dwStateFlag = GetStateFlag();
  msg.byColor = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {4, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CPlayer::SendMsg_RealMovePoint(int n)
{

  if (m_bObserver && !g_Player[n].m_byUserDgr)
  {
    return;
  }

  _player_real_move_zocl msg{};
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.dwEquipVer = static_cast<unsigned __int16>(GetVisualVer());
  msg.byRaceCode = m_Param.GetRaceSexCode();
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
  msg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));
  msg.wLastEffectCode = static_cast<unsigned __int16>(m_wLastContEffect);
  msg.dwStateFlag = GetStateFlag();
  msg.nAddSpeed = static_cast<__int16>(static_cast<int>(GetAddSpeed()));
  msg.byDirect = m_byMoveDirect;
  msg.byColor = m_byGuildBattleColorInx;

  unsigned __int8 type[2] = {4, 21};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}










