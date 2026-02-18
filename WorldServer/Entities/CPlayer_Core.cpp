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

void CPlayer::SendMsg_TransShipRenewTicketResult(char byErrCode)
{
  _trans_ship_renew_ticket_result_zocl msg{};
  msg.byErrCode = static_cast<unsigned __int8>(byErrCode);

  unsigned __int8 pbyType[2] = {33, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, reinterpret_cast<char *>(&msg), 1u);
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

namespace
{
} // namespace









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














