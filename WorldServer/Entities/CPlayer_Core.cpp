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























































constexpr int kUnitBulletPartByPartCode[6] = {-1, -1, -1, 0, 1, -1};
constexpr unsigned __int8 kUnitWeaponPartByBulletSlot[2] = {3, 4};
constexpr int kAnimusRecallConsumeFP = 60;


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

void CPlayer::SendMsg_JadeEffectErr(char byErrorCode)
{
  unsigned __int8 type[2] = {59, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byErrorCode, 1u);
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





namespace
{
} // namespace





























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




















