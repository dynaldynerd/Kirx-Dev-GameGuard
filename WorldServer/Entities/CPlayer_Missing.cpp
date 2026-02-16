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
#include "CPlayerMissingLocalPacketStructs.h"

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

#pragma pack(push, 1)
struct GuildJoinApplyResultPacket
{
  char byRetCode;
  unsigned int dwGuildSerial;
  char szGuildName[17];
};

struct GuildJoinAcceptFailPacket
{
  char byRetCode;
  unsigned int dwApplierSerial;
};

struct GuildVoteResultPacket
{
  unsigned int dwMatterVoteSynKey;
  unsigned __int8 byRetCode;
};

struct GuildPushMoneyResultPacket
{
  char byRetCode;
  unsigned int dwDalant;
  unsigned int dwGold;
};

struct SetTargetObjectResultPacket
{
  char byRetCode;
  char byKind;
  char byID;
  unsigned int dwSerial;
  unsigned __int16 wHPRate;
  bool bCorpse;
  bool bForce;
};

struct GroupTargetInformPacket
{
  char byGroupType;
  char szName[65];
};

struct GuildRoomRentResultPacket
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySubRetCode;
  unsigned __int8 byRoomType;
};

struct GuildRoomEnterResultPacket
{
  unsigned __int8 byRetCode;
  unsigned __int8 bySubRetCode;
  unsigned __int8 byMapIndex;
  unsigned __int16 wMapLayerIndex;
  short sNewPos[3];
  int nRestTime;
};
#pragma pack(pop)

struct __declspec(align(4)) _qry_case_joinacguild_local
{
  unsigned int in_guildindex;
  unsigned int in_guildserial;
  unsigned int in_applierindex;
  unsigned int in_applierserial;
  unsigned int in_accepterserial;
  int in_Grade;
  int in_MemberNum;

  __int64 size() const
  {
    return sizeof(*this);
  }
};

struct __cppobj __unaligned __declspec(align(1)) _qry_case_insertguild_local
{
  unsigned int in_guildindex;
  char in_w_guildName[17];
  unsigned __int8 in_guildRace;
  unsigned __int8 in_membernum;
  unsigned __int8 in_padding;
  unsigned int in_memberindex[8];
  unsigned int in_memberserial[8];
  char tmp_w_membername[136];
  unsigned __int8 tmp_lv[8];
  unsigned __int8 tmp_grade[8];
  unsigned int tmp_pvp[8];
  unsigned int out_guildserial;
  unsigned int tmp_Esterindex;
  unsigned int tmp_Esterserial;

  _qry_case_insertguild_local()
  {
    memset_0(this, 0, sizeof(*this));
    in_guildindex = static_cast<unsigned int>(-1);
    out_guildserial = static_cast<unsigned int>(-1);
    tmp_Esterindex = static_cast<unsigned int>(-1);
    tmp_Esterserial = static_cast<unsigned int>(-1);
  }

  __int64 size() const
  {
    return sizeof(*this);
  }
};

struct __declspec(align(8)) _qry_case_inputgmoney_local
{
  unsigned int in_pusherserial;
  char in_w_pushername[17];
  unsigned int tmp_guildindex;
  unsigned int in_guildserial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 in_date[4];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned __int8 byProcRet;

  __int64 size() const
  {
    return sizeof(*this);
  }
};





















































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







#pragma pack(push, 1)
struct ThrowResultEffectInfo
{
  unsigned __int8 byObjID;
  unsigned int dwObjSerial;
  unsigned __int8 byRetCode;
  unsigned __int16 wEffectValue;
};

struct ThrowSkillResultSelf
{
  unsigned __int8 byErrCode;
  _CHRID idDster;
  unsigned __int8 byEffectedNum;
  ThrowResultEffectInfo list[30];
  unsigned __int8 byPadding;
};

struct ThrowSkillResultOther
{
  unsigned __int8 byRetCode;
  _CHRID idPerformer;
  unsigned __int8 bySkillIndex;
  _CHRID idDster;
  unsigned __int8 byEffectedNum;
  ThrowResultEffectInfo list[30];
  unsigned __int8 byPadding;
};

struct ThrowUnitResultOther
{
  unsigned __int8 byRetCode;
  _CHRID idPerformer;
  unsigned __int16 wBulletIndex;
  _CHRID idDster;
  unsigned __int8 byEffectedNum;
  ThrowResultEffectInfo list[30];
  unsigned __int8 byPadding;
};

struct AnimusRecallResultPacket
{
  unsigned __int8 byResultCode;
  unsigned int dwAnimusSerial;
  unsigned __int16 wAnimusHP;
  unsigned __int16 wAnimusFP;
  unsigned __int64 dwAnimusExp;
  unsigned __int16 wLeftFP;
};
#pragma pack(pop)

void FillThrowEffectList(ThrowResultEffectInfo *outList, unsigned __int8 *outCount)
{
  const unsigned __int8 effected = (g_tmpEffectedNum < 30) ? g_tmpEffectedNum : 30;
  for (int index = 0; index < effected; ++index)
  {
    outList[index].byObjID = g_tmpEffectedList[index].pMem->m_ObjID.m_byID;
    outList[index].dwObjSerial = g_tmpEffectedList[index].pMem->m_dwObjSerial;
    outList[index].byRetCode = g_tmpEffectedList[index].byRetCode;
    outList[index].wEffectValue = g_tmpEffectedList[index].wEffectValue;
  }
  *outCount = effected;
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

unsigned __int16 DeleteUnitKey(CPlayer *player, unsigned __int8 bySlotIndex)
{
  for (int index = 0; index < player->m_Param.m_dbInven.m_nUsedNum; ++index)
  {
    _STORAGE_LIST::_db_con *item = &player->m_Param.m_dbInven.m_pStorageList[index];
    if (item->m_bLoad && item->m_dwLv == bySlotIndex)
    {
      const unsigned __int16 serial = item->m_wSerial;
      if (player->Emb_DelStorage(0, static_cast<unsigned __int8>(index), false, true, "CPlayer::_DeleteUnitKey()"))
      {
        return serial;
      }
      return static_cast<unsigned __int16>(-1);
    }
  }
  return static_cast<unsigned __int16>(-1);
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

  _object_create_setdata createData{};
  createData.m_pMap = player->m_pCurMap;
  createData.m_nLayerIndex = player->m_wMapLayerIndex;
  createData.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame);
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
  slot->m_pOwner = player;
  slot->m_dwOwnerSerial = player->m_dwObjSerial;
  slot->m_byFrame = unitData->byFrame;
  std::memcpy(slot->m_byPartCode, unitData->byPart, sizeof(slot->m_byPartCode));
  slot->m_byCreateType = 0;
  slot->m_byTransDistCode = 0;
  slot->m_dwParkingStartTime = GetLoopTime();
  slot->m_wHPRate = 10000;
  return slot;
}

CAnimus *CreateAnimusObject(CPlayer *player, _STORAGE_LIST::_db_con *animusItem)
{
  CAnimus *slot = FindEmptyAnimusSlot();
  if (!slot)
  {
    return nullptr;
  }

  _character_create_setdata createData{};
  createData.m_pMap = player->m_pCurMap;
  createData.m_nLayerIndex = player->m_wMapLayerIndex;
  createData.m_pRecordSet = g_Main.m_tblAnimus.GetRecord(animusItem->m_wItemIndex);
  if (!createData.m_pRecordSet)
  {
    return nullptr;
  }
  std::memcpy(createData.m_fStartPos, player->m_fCurPos, sizeof(createData.m_fStartPos));
  if (!slot->Create(&createData))
  {
    return nullptr;
  }

  _animus_fld *animusFld = GetAnimusFldFromExp(animusItem->m_wItemIndex, animusItem->m_dwDur);
  if (!animusFld)
  {
    slot->Destroy();
    return nullptr;
  }

  slot->m_dwObjSerial = GenerateTransientObjSerial();
  slot->m_pMaster = player;
  slot->m_dwMasterSerial = player->m_dwObjSerial;
  slot->m_byClassCode = static_cast<unsigned __int8>(animusItem->m_wItemIndex);
  slot->m_nHP = LOWORD(animusItem->m_dwLv);
  slot->m_nFP = HIWORD(animusItem->m_dwLv);
  slot->m_dwExp = animusItem->m_dwDur;
  slot->m_nMaxAttackPnt = player->m_nAnimusAttackPnt;
  slot->m_pRecord = animusFld;
  slot->m_nMaxHP = animusFld->m_nMaxHP;
  slot->m_nMaxFP = animusFld->m_nMaxFP;
  return slot;
}

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

void CPlayer::SendMsg_MaxHFSP()
{
  char payload[6]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = static_cast<unsigned __int16>(this->GetMaxHP());
  *reinterpret_cast<__int16 *>(payload + 2) = static_cast<__int16>(this->GetMaxFP());
  *reinterpret_cast<__int16 *>(payload + 4) = static_cast<__int16>(this->GetMaxSP());

  unsigned __int8 type[2] = {11, 3};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 6u);
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

void CPlayer::SendMsg_UnitReturnResult(char byRetCode, unsigned int dwPayDalant)
{
  char payload[9]{};
  payload[0] = byRetCode;
  *reinterpret_cast<unsigned int *>(payload + 1) = dwPayDalant;
  *reinterpret_cast<unsigned int *>(payload + 5) = this->m_Param.GetDalant();

  unsigned __int8 type[2] = {23, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 9u);
}

void CPlayer::SendMsg_UnitBulletReplaceResult(char byRetCode)
{
  char payload[1]{};
  payload[0] = byRetCode;

  unsigned __int8 type[2] = {23, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_UnitDestroy(char bySlotIndex)
{
  char payload[1]{};
  payload[0] = bySlotIndex;

  unsigned __int8 type[2] = {23, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_AnimusHPInform()
{
  if (!this->m_pRecalledAnimusChar || !this->m_pRecalledAnimusItem)
  {
    return;
  }

  char payload[4]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_pRecalledAnimusItem->m_wSerial;
  *reinterpret_cast<__int16 *>(payload + 2) = static_cast<__int16>(this->m_pRecalledAnimusChar->m_nHP);

  unsigned __int8 type[2] = {22, 9};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 4u);
}

void CPlayer::SendMsg_AnimusFPInform()
{
  if (!this->m_pRecalledAnimusChar || !this->m_pRecalledAnimusItem)
  {
    return;
  }

  char payload[4]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_pRecalledAnimusItem->m_wSerial;
  *reinterpret_cast<__int16 *>(payload + 2) = static_cast<__int16>(this->m_pRecalledAnimusChar->m_nFP);

  unsigned __int8 type[2] = {22, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 4u);
}

void CPlayer::SendMsg_AnimusExpInform()
{
  if (!this->m_pRecalledAnimusChar || !this->m_pRecalledAnimusItem)
  {
    return;
  }

  char payload[10]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_pRecalledAnimusItem->m_wSerial;
  *reinterpret_cast<unsigned __int64 *>(payload + 2) = this->m_pRecalledAnimusChar->m_dwExp;

  unsigned __int8 type[2] = {22, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0xAu);
}

void CPlayer::SendMsg_AnimusModeInform(char byMode)
{
  char payload[1]{};
  payload[0] = byMode;

  unsigned __int8 type[2] = {22, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
}

void CPlayer::SendMsg_AnimusRecallWaitTimeFree(char bFree)
{
  char payload[1]{};
  payload[0] = bFree;

  unsigned __int8 type[2] = {22, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 1u);
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

void CPlayer::SendMsg_AlterHPInform()
{
  char payload[2]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = static_cast<unsigned __int16>(this->GetHP());

  unsigned __int8 type[2] = {11, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 2u);
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

void CPlayer::SendMsg_BuddyNameReNewal(unsigned int dwSerial, char *wszName)
{
  char payload[0x15]{};
  *reinterpret_cast<unsigned int *>(payload) = dwSerial;
  strcpy_s(payload + 4, 0x11u, wszName);

  unsigned __int8 type[2] = {31, 17};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 0x15u);
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

void CPlayer::SendMsg_UnitRideChange(bool bTake, CParkingUnit *pUnit)
{
  char payload[0x13]{};
  *reinterpret_cast<unsigned __int16 *>(payload) = this->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(payload + 2) = this->m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 6) = this->GetVisualVer();
  payload[8] = bTake ? 1 : 0;
  *reinterpret_cast<unsigned int *>(payload + 9) = pUnit->m_dwObjSerial;

  short shortPos[3]{};
  FloatToShort(this->m_fCurPos, shortPos, 3);
  memcpy_0(payload + 13, shortPos, sizeof(shortPos));

  unsigned __int8 type[2] = {3, 35};
  this->CircleReport(type, payload, 19, false);
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

void CPlayer::SendMsg_GuildJoinApplyResult(char byRetCode, CGuild *pApplyGuild)
{
  GuildJoinApplyResultPacket packet{};
  packet.byRetCode = byRetCode;
  if (pApplyGuild)
  {
    packet.dwGuildSerial = pApplyGuild->m_dwSerial;
    strcpy_0(packet.szGuildName, pApplyGuild->m_wszName);
  }
  else
  {
    packet.dwGuildSerial = static_cast<unsigned int>(-1);
  }

  unsigned __int8 type[2] = {27, 7};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildJoinApplyCancelResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_GuildJoinApplyRejectInform()
{
  char payload = 0;
  unsigned __int8 type[2] = {27, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &payload, 1u);
}

void CPlayer::SendMsg_GuildJoinAcceptFail(char byRetCode, unsigned int dwApplierSerial)
{
  GuildJoinAcceptFailPacket packet{};
  packet.byRetCode = byRetCode;
  packet.dwApplierSerial = dwApplierSerial;

  unsigned __int8 type[2] = {27, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_VoteResult(unsigned int dwMatterVoteSynKey, unsigned __int8 byRetCode)
{
  GuildVoteResultPacket packet{};
  packet.dwMatterVoteSynKey = dwMatterVoteSynKey;
  packet.byRetCode = byRetCode;

  unsigned __int8 type[2] = {27, 26};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_AnimusTargetResult(char byRetCode)
{
  unsigned __int8 type[2] = {22, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_CancelSuggestResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_GuildSetHonorResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 114};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_GuildPushMoneyResult(char byRetCode)
{
  GuildPushMoneyResultPacket packet{};
  packet.byRetCode = byRetCode;
  packet.dwDalant = this->m_Param.GetDalant();
  packet.dwGold = this->m_Param.GetGold();

  unsigned __int8 type[2] = {27, 36};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
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

void CPlayer::SendMsg_UnitFrameBuyResult(char byRetCode, char byFrameCode, char byUnitSlotIndex, unsigned __int16 wKeyIndex, unsigned __int16 wKeySerial, unsigned int *pdwConsumMoney)
{

  _unit_frame_buy_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.byFrameCode = byFrameCode;
  packet.byAddUnitSlot = byUnitSlotIndex;
  packet.byKeyIndex = static_cast<unsigned __int8>(wKeyIndex);
  packet.wKeySerial = wKeySerial;
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();
  packet.dwConsumMoney[0] = pdwConsumMoney[0];
  packet.dwConsumMoney[1] = pdwConsumMoney[1];

  unsigned __int8 type[2] = {23, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitSellResult(char byRetCode, char bySlotIndex, unsigned __int16 wKeySerial, int nAddMoney, unsigned int dwTotalNonpay, unsigned int dwSumDalant, unsigned int dwSumGold)
{

  _unit_sell_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.wKeySerial = wKeySerial;
  packet.dwNonPayDalant = dwTotalNonpay;
  packet.nAddMoney[0] = nAddMoney;
  packet.dwLeftMoney[0] = dwSumDalant;
  packet.dwLeftMoney[1] = dwSumGold;

  unsigned __int8 type[2] = {23, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitPartTuningResult(char byRetCode, char bySlotIndex, int *pnCost)
{

  _unit_part_tuning_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  std::memcpy(packet.byPart, this->m_Param.m_UnitDB.m_List[static_cast<unsigned __int8>(bySlotIndex)].byPart, sizeof(packet.byPart));
  std::memcpy(packet.dwBullet, this->m_Param.m_UnitDB.m_List[static_cast<unsigned __int8>(bySlotIndex)].dwBullet, sizeof(packet.dwBullet));
  packet.nCost[0] = pnCost[0];
  packet.nCost[1] = pnCost[1];
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();

  unsigned __int8 type[2] = {23, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitFrameRepairResult(char byRetCode, char bySlotIndex, unsigned int dwNewGauge, unsigned int dwConsumDalant)
{

  _unit_frame_repair_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.dwNewGauge = dwNewGauge;
  packet.dwConsumDalant = dwConsumDalant;
  packet.dwLeftDalant = this->m_Param.GetDalant();

  unsigned __int8 type[2] = {23, 8};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitBulletFillResult(char byRetCode, char bySlotIndex, unsigned __int16 *pwBulletIndex, unsigned int *pdwConsumMoney)
{

  _unit_bullet_fill_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.wBulletIndex[0] = pwBulletIndex[0];
  packet.wBulletIndex[1] = pwBulletIndex[1];
  packet.dwComsumMoney[0] = static_cast<int>(pdwConsumMoney[0]);
  packet.dwComsumMoney[1] = pdwConsumMoney[1];
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();

  unsigned __int8 type[2] = {23, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitPackFillResult(unsigned __int8 byRetCode, unsigned __int8 bySlotIndex, unsigned __int8 byFillNum, _unit_pack_fill_request_clzo::__list *pList, unsigned int *pdwConsumMoney)
{

  _unit_pack_fill_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.byFillNum = byFillNum;
  if (byFillNum > 0)
  {
    std::memcpy(packet.List, pList, static_cast<size_t>(byFillNum) * sizeof(_unit_pack_fill_request_clzo::__list));
  }
  packet.dwComsumMoney[0] = pdwConsumMoney[0];
  packet.dwComsumMoney[1] = pdwConsumMoney[1];
  packet.dwLeftMoney[0] = this->m_Param.GetDalant();
  packet.dwLeftMoney[1] = this->m_Param.GetGold();

  unsigned __int8 type[2] = {23, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitDeliveryResult(char byRetCode, char bySlotIndex, unsigned int dwParkingUnitSerial, unsigned int dwPayDalant)
{

  _unit_delivery_result_zocl packet{};
  packet.byRetCode = byRetCode;
  packet.bySlotIndex = bySlotIndex;
  packet.dwParkingUnitSerial = dwParkingUnitSerial;
  packet.dwPayDalant = dwPayDalant;
  packet.dwLeftDalant = this->m_Param.GetDalant();

  unsigned __int8 type[2] = {23, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_UnitTakeResult(char byRetCode)
{
  unsigned __int8 type[2] = {23, 18};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
}

void CPlayer::SendMsg_UnitLeaveResult(char byRetCode)
{
  unsigned __int8 type[2] = {23, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
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

void CPlayer::SendMsg_OfferSuggestResult(char byRetCode)
{
  unsigned __int8 type[2] = {27, 20};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &byRetCode, 1u);
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
  ThrowSkillResultSelf toSelf{};
  toSelf.byErrCode = byErrCode;
  if (pidDst)
  {
    toSelf.idDster = *pidDst;
  }
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

  ThrowSkillResultOther toOther{};
  toOther.byRetCode = 0;
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.bySkillIndex = bySkillIndex;
  if (pidDst)
  {
    toOther.idDster = *pidDst;
  }
  toOther.byEffectedNum = toSelf.byEffectedNum;
  std::memcpy(toOther.list, toSelf.list, sizeof(ThrowResultEffectInfo) * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 101};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_ThrowUnitResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int16 wBulletIndex)
{
  ThrowSkillResultSelf toSelf{};
  toSelf.byErrCode = byErrCode;
  if (pidDst)
  {
    toSelf.idDster = *pidDst;
  }
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

  ThrowUnitResultOther toOther{};
  toOther.byRetCode = 0;
  toOther.idPerformer.byID = this->m_ObjID.m_byID;
  toOther.idPerformer.wIndex = this->m_ObjID.m_wIndex;
  toOther.idPerformer.dwSerial = this->m_dwObjSerial;
  toOther.wBulletIndex = wBulletIndex;
  if (pidDst)
  {
    toOther.idDster = *pidDst;
  }
  toOther.byEffectedNum = toSelf.byEffectedNum;
  std::memcpy(toOther.list, toSelf.list, sizeof(ThrowResultEffectInfo) * toSelf.byEffectedNum);

  unsigned __int8 otherType[2] = {17, 104};
  this->CircleReport(otherType, reinterpret_cast<char *>(&toOther), sizeof(toOther), false);
}

void CPlayer::SendMsg_AnimusRecallResult(unsigned __int8 byResultCode, unsigned __int16 wLeftFP, CAnimus *pNewAnimus)
{
  AnimusRecallResultPacket packet{};
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

void CPlayer::SendMsg_SetTargetObjectResult(char byRetCode, bool bForce)
{
  SetTargetObjectResultPacket packet{};
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
      packet.bCorpse = targetPlayer->m_bCorpse;
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
  GroupTargetInformPacket packet{};
  packet.byGroupType = byGroupType;
  if (pwszName)
  {
    strcpy_0(packet.szName, pwszName);
  }

  unsigned __int8 type[2] = {13, 111};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildRoomRentResult(unsigned __int8 byRetCode, unsigned __int8 bySubRetCode, unsigned __int8 byRoomType)
{
  GuildRoomRentResultPacket packet{};
  packet.byRetCode = byRetCode;
  packet.bySubRetCode = bySubRetCode;
  packet.byRoomType = byRoomType;

  unsigned __int8 type[2] = {27, 103};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
}

void CPlayer::SendMsg_GuildRoomEnterResult(unsigned __int8 byRetCode, unsigned __int8 bySubRetCode, unsigned __int8 byMapIndex, unsigned __int16 wMapLayer, float *pPos, int nRestTime)
{
  GuildRoomEnterResultPacket packet{};
  packet.byRetCode = byRetCode;
  packet.bySubRetCode = bySubRetCode;
  packet.byMapIndex = byMapIndex;
  packet.wMapLayerIndex = wMapLayer;
  if (pPos)
  {
    FloatToShort(pPos, packet.sNewPos, 3);
  }
  packet.nRestTime = nRestTime;

  unsigned __int8 type[2] = {27, 105};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&packet),
    static_cast<unsigned __int16>(sizeof(packet)));
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

CPvpPointLimiter *CPlayer::GetPvpPointLimiter(CPvpPointLimiter *result)
{
  *result = m_kPvpPointLimiter;
  return result;
}

CPvpOrderView *CPlayer::GetPvpOrderView()
{
  return &m_kPvpOrderView;
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

void CPlayer::SendMsg_GuildEstablishFail(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {27, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_GuildJoinOtherInform()
{
  char msg[10]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_id.dwSerial;
  *reinterpret_cast<unsigned int *>(msg + 4) = this->m_Param.m_pGuild ? this->m_Param.m_pGuild->m_dwSerial
                                                                     : static_cast<unsigned int>(-1);
  *reinterpret_cast<unsigned __int16 *>(msg + 8) = this->m_wVisualVer;

  unsigned __int8 type[2] = {27, 39};
  CircleReport(type, msg, 10, false);
}


#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::SendMsg_SelectQuestReward(char byQuestDBSlot)
{
  char szMsg[32]; // [rsp+34h] [rbp-44h] BYREF
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  szMsg[0] = byQuestDBSlot;
  pbyType[0] = 24;
  pbyType[1] = 8;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}
#endif

void CPlayer::SendMsg_AlterUnitHPInform(char bySlotIndex, unsigned int dwGauge)
{
  char szMsg; // [rsp+34h] [rbp-44h] BYREF
  unsigned int v7; // [rsp+35h] [rbp-43h]
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  szMsg = bySlotIndex;
  v7 = dwGauge;
  pbyType[0] = 5;
  pbyType[1] = 23;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, &szMsg, 5u);
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
void CPlayer::SendMsg_QuestFailure(char byFailCode, char byQuestDBSlot)
{
  char szMsg[32]{}; // [rsp+34h] [rbp-44h] BYREF
  unsigned __int8 pbyType[36]{}; // [rsp+54h] [rbp-24h] BYREF

  szMsg[0] = byFailCode;
  szMsg[1] = byQuestDBSlot;
  pbyType[0] = 24;
  pbyType[1] = 7;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 2u);
}
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
void CPlayer::SendMsg_InsertNextQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB)
{
  _insert_next_quest_inform_zocl msg{};
  unsigned __int8 pbyType[28]{};

  msg.byQuestDBSlot = bySlotIndex;
  memcpy_0(&msg.NewQuestData, pQuestDB, sizeof(msg.NewQuestData));
  pbyType[0] = 24;
  pbyType[1] = 111;
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xEu);
}
#endif

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
void CPlayer::Emb_ItemUpgrade(
  unsigned __int8 byUpgradeType,
  unsigned __int8 byStorageCode,
  unsigned __int8 byStorageIndex,
  unsigned int dwGradeInfo)
{
  _STORAGE_LIST::_storage_con *pItem = &this->m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];
  if (byStorageCode == 1)
  {
    if (this->GetEffectEquipCode(byStorageCode, byStorageIndex) == 1)
    {
      this->SetEquipEffect(static_cast<_STORAGE_LIST::_db_con *>(pItem), 0);
    }
    this->SetEffectEquipCode(byStorageCode, byStorageIndex, 0);
  }

  if (byUpgradeType)
  {
    if (byUpgradeType == 1)
    {
      this->m_Param.m_pStoragePtr[byStorageCode]->GradeDown(byStorageIndex, dwGradeInfo);
    }
    else if (byUpgradeType == 2)
    {
      const unsigned __int8 limSocket = GetItemUpgLimSocket(pItem->m_dwLv);
      const unsigned int dwUptInfo = GetBitAfterSetLimSocket(limSocket);
      this->m_Param.m_pStoragePtr[byStorageCode]->SetGrade(byStorageIndex, 0, dwUptInfo);
    }
  }
  else
  {
    this->m_Param.m_pStoragePtr[byStorageCode]->GradeUp(byStorageIndex, dwGradeInfo);
  }

  if (byStorageCode == 1)
  {
    if (this->IsEffectableEquip(pItem))
    {
      this->SetEquipEffect(static_cast<_STORAGE_LIST::_db_con *>(pItem), 1);
      this->SetEffectEquipCode(byStorageCode, byStorageIndex, 1u);
    }
    else
    {
      this->SetEffectEquipCode(byStorageCode, byStorageIndex, 2u);
    }
    if (pItem->m_byTableCode == 6)
    {
      this->m_pmWpn.FixWeapon(static_cast<_STORAGE_LIST::_db_con *>(pItem));
    }
  }

  if (byStorageCode == 1)
  {
    CPlayer::CashChangeStateFlag changeFlag(0);
    this->UpdateVisualVer(changeFlag);
    this->SendMsg_EquipPartChange(pItem->m_byTableCode);
  }

  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_ItemUpgrade(byStorageCode, byStorageIndex, dwGradeInfo, 1);
  }
}
#endif

#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::Emb_CompleteQuest(
  unsigned __int8 byQuestDBSlot,
  unsigned __int8 byRewardItemIndex,
  unsigned __int8 byLinkQuestIndex)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  unsigned int Level; // eax
  int m_wIndex; // ecx
  unsigned int v8; // eax
  int v9; // ecx
  unsigned __int8 MapCode; // al
  __int64 v11; // [rsp+0h] [rbp-198h] BYREF
  int nGrade; // [rsp+30h] [rbp-168h]
  int *pnMaxPoint; // [rsp+38h] [rbp-160h]
  _MASTERY_PARAM *pData; // [rsp+40h] [rbp-158h]
  unsigned int *pdwAlter; // [rsp+48h] [rbp-150h]
  char *pszFileName; // [rsp+50h] [rbp-148h]
  unsigned __int8 byLogType; // [rsp+58h] [rbp-140h]
  char *pszTitle; // [rsp+60h] [rbp-138h]
  void *Src; // [rsp+70h] [rbp-128h]
  _QUEST_DB_BASE::_LIST v20; // [rsp+88h] [rbp-110h] BYREF
  _Quest_fld *Record; // [rsp+A8h] [rbp-F0h]
  int j; // [rsp+B0h] [rbp-E8h]
  _Quest_fld *pQuestFld; // [rsp+B8h] [rbp-E0h]
  unsigned __int8 inserted; // [rsp+C0h] [rbp-D8h]
  _QUEST_DB_BASE::_NPC_QUEST_HISTORY *pHisData; // [rsp+C8h] [rbp-D0h]
  float pNewPos[8]; // [rsp+D8h] [rbp-C0h] BYREF
  _dummy_position *pPos; // [rsp+F8h] [rbp-A0h]
  int v28; // [rsp+100h] [rbp-98h]
  unsigned __int8 v29; // [rsp+104h] [rbp-94h]
  _QUEST_DB_BASE::_LIST *pSlotData; // [rsp+108h] [rbp-90h]
  int k; // [rsp+110h] [rbp-88h]
  int m; // [rsp+114h] [rbp-84h]
  char *m_szLvHistoryFileName; // [rsp+120h] [rbp-78h]
  unsigned int *m_dwAlterMastery; // [rsp+128h] [rbp-70h]
  _MASTERY_PARAM *p_m_pmMst; // [rsp+130h] [rbp-68h]
  int *m_nMaxPoint; // [rsp+138h] [rbp-60h]
  int m_byPvPGrade; // [rsp+140h] [rbp-58h]
  long double dExp; // [rsp+148h] [rbp-50h]
  char *m_strCode; // [rsp+150h] [rbp-48h]
  char *v40; // [rsp+158h] [rbp-40h]
  unsigned int *v41; // [rsp+160h] [rbp-38h]
  _MASTERY_PARAM *v42; // [rsp+168h] [rbp-30h]
  int *v43; // [rsp+170h] [rbp-28h]
  int v44; // [rsp+178h] [rbp-20h]
  long double Exp; // [rsp+180h] [rbp-18h]

  if ( this->m_pUserDB )
  {
    Src = &this->m_Param.m_QuestDB.m_List[byQuestDBSlot];
    memcpy_0(&v20, Src, sizeof(v20));
    Record = 0LL;
    if ( *(unsigned __int8 *)Src != 255 )
    {
      for ( j = 0; j < 3; ++j )
      {
        if ( *(unsigned __int16 *)((char *)Src + 2 * j + 3) != 0xFFFF )
          return;
      }
      m_szLvHistoryFileName = this->m_szLvHistoryFileName;
      m_dwAlterMastery = this->m_Param.m_dwAlterMastery;
      p_m_pmMst = &this->m_pmMst;
      m_nMaxPoint = this->m_nMaxPoint;
      m_byPvPGrade = this->m_Param.m_byPvPGrade;
      dExp = this->m_Param.GetExp();
      Level = this->m_Param.GetLevel();
      m_wIndex = this->m_ObjID.m_wIndex;
      pszTitle = 0LL;
      byLogType = 0;
      pszFileName = m_szLvHistoryFileName;
      pdwAlter = m_dwAlterMastery;
      pData = p_m_pmMst;
      pnMaxPoint = m_nMaxPoint;
      nGrade = m_byPvPGrade;
      CPlayer::s_MgrLvHistory.update_mastery(
        m_wIndex,
        this->m_byUserDgr,
        Level,
        dExp,
        this->m_dwExpRate,
        m_byPvPGrade,
        m_nMaxPoint,
        p_m_pmMst,
        m_dwAlterMastery,
        m_szLvHistoryFileName,
        0,
        nullptr);
      this->m_Param.InitAlterMastery();
      pQuestFld = reinterpret_cast<_Quest_fld *>(
        CQuestMgr::s_tblQuest
          ? CQuestMgr::s_tblQuest->GetRecord(*(unsigned __int16 *)((char *)Src + 1))
          : nullptr);
      Record = this->_Reward_Quest(pQuestFld, byRewardItemIndex);
      this->SendMsg_QuestComplete(byQuestDBSlot);
      if ( *(_BYTE *)Src == 1 )
      {
        inserted = this->m_QuestMgr.InsertNpcQuestHistory(
          pQuestFld->m_strCode,
          pQuestFld->m_nDifficultyLevel,
          pQuestFld->m_dRepeatTime);
        pHisData = &this->m_Param.m_QuestDB.m_History[inserted];
        this->m_pUserDB->Update_NPCQuestHistory( inserted, pHisData);
        this->SendMsg_NpcQuestHistoryInform(inserted);
      }
      this->m_QuestMgr.DeleteQuestData(byQuestDBSlot);
      this->m_pUserDB->Update_QuestDelete( byQuestDBSlot);
      m_strCode = pQuestFld->m_strCode;
      v40 = this->m_szLvHistoryFileName;
      v41 = this->m_Param.m_dwAlterMastery;
      v42 = &this->m_pmMst;
      v43 = this->m_nMaxPoint;
      v44 = this->m_Param.m_byPvPGrade;
      Exp = this->m_Param.GetExp();
      v8 = this->m_Param.GetLevel();
      v9 = this->m_ObjID.m_wIndex;
      pszTitle = m_strCode;
      byLogType = 2;
      pszFileName = v40;
      pdwAlter = v41;
      pData = v42;
      pnMaxPoint = v43;
      nGrade = v44;
      CPlayer::s_MgrLvHistory.update_mastery(
        v9,
        this->m_byUserDgr,
        v8,
        Exp,
        this->m_dwExpRate,
        v44,
        v43,
        v42,
        v41,
        v40,
        2u,
        m_strCode);
      this->m_Param.InitAlterMastery();
      this->m_dwUMWHLastTime = GetLoopTime();
      if ( pQuestFld->m_nLinkDummyCond == 1 )
      {
        if ( strcmp_0(pQuestFld->m_strLinkDummyCode, "-1") )
        {
          pPos = this->m_pCurMap->GetDummyPostion(pQuestFld->m_strLinkDummyCode);
          if ( pPos )
          {
            if ( this->m_pCurMap->GetRandPosInDummy(pPos, pNewPos, 1) )
            {
              this->OutOfMap(this->m_pCurMap, this->m_wMapLayerIndex, 3u, pNewPos);
              MapCode = this->m_Param.GetMapCode();
              this->SendMsg_GotoRecallResult(0, MapCode, pNewPos, 4u);
            }
          }
        }
      }
      if ( !Record )
      {
        v28 = 0;
        for ( j = 0; j < 5; ++j )
        {
          if ( strncmp(pQuestFld->m_strLinkQuest[(__int64)j], "-1", 2uLL) )
            ++v28;
        }
        if ( v28 > 0 )
        {
          v29 = byLinkQuestIndex;
          if ( byLinkQuestIndex == 255 )
            v29 = rand() % v28;
          Record = reinterpret_cast<_Quest_fld *>(
            CQuestMgr::s_tblQuest
              ? CQuestMgr::s_tblQuest->GetRecord(pQuestFld->m_strLinkQuest[static_cast<unsigned __int64>(v29)])
              : nullptr);
        }
      }
      if ( Record )
      {
        pSlotData = &this->m_Param.m_QuestDB.m_List[byQuestDBSlot];
        pSlotData->byQuestType = v20.byQuestType;
        pSlotData->wIndex = Record->m_dwIndex;
        pSlotData->dwPassSec = 0;
        for ( k = 0; k < 3; ++k )
        {
          if ( Record->m_ActionNode[k].m_nActType != -1 )
            pSlotData->wNum[k] = 0;
        }
        this->m_pUserDB->Update_QuestInsert( byQuestDBSlot, pSlotData);
        this->SendMsg_InsertNextQuest(byQuestDBSlot, pSlotData);
      }
      for ( m = 0; m < 3; ++m )
      {
        if (this->m_QuestMgr.m_pTempHappenEvent[m].isset())
        {
          memcpy_0(
            &this->m_QuestMgr.m_LastHappenEvent,
            &this->m_QuestMgr.m_pTempHappenEvent[m],
            sizeof(this->m_QuestMgr.m_LastHappenEvent));
          this->Emb_StartQuest(0xFFu, &this->m_QuestMgr.m_pTempHappenEvent[m]);
          if ( this->m_QuestMgr.m_pTempHappenEvent[m].m_QtHpType == quest_happen_type_maxlevel )
          {
            this->m_Param.SetMaxLevel( 50);
            if ( this->m_pUserDB )
              this->m_pUserDB->Update_MaxLevel( 0x32u);
          }
          this->m_QuestMgr.m_pTempHappenEvent[m].init();
        }
      }
    }
  }
}
#endif

void CPlayer::Guild_Insert_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int EstConsumeDalant; // eax
  unsigned int v4; // eax
  CPlayer::CashChangeStateFlag *v5; // rax
  CGuildBattleController *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-4C8h] BYREF
  int v8; // [rsp+30h] [rbp-498h]
  unsigned int dwSerial; // [rsp+34h] [rbp-494h]
  char Destination[28]; // [rsp+48h] [rbp-480h] BYREF
  unsigned __int8 v11; // [rsp+64h] [rbp-464h]
  unsigned __int8 v12; // [rsp+65h] [rbp-463h]
  unsigned int v13[16]; // [rsp+78h] [rbp-450h] BYREF
  unsigned int dwMemberSerial[18]; // [rsp+B8h] [rbp-410h] BYREF
  char Source[168]; // [rsp+100h] [rbp-3C8h] BYREF
  _BYTE v16[32]; // [rsp+1A8h] [rbp-320h] BYREF
  _BYTE v17[32]; // [rsp+1C8h] [rbp-300h] BYREF
  _DWORD v18[11]; // [rsp+1E8h] [rbp-2E0h] BYREF
  int v19; // [rsp+214h] [rbp-2B4h]
  int v20; // [rsp+218h] [rbp-2B0h]
  char *m_sData; // [rsp+220h] [rbp-2A8h]
  CGuild *v22; // [rsp+228h] [rbp-2A0h]
  int j; // [rsp+230h] [rbp-298h]
  CPlayer *v24; // [rsp+238h] [rbp-290h]
  CPlayer *v25; // [rsp+240h] [rbp-288h]
  char szTran[136]; // [rsp+260h] [rbp-268h] BYREF
  CPlayer *v27; // [rsp+2E8h] [rbp-1E0h]
  _guild_member_info __t[8]; // [rsp+300h] [rbp-1C8h] BYREF
  CPlayer *pPtr; // [rsp+488h] [rbp-40h]
  _guild_member_info *pMem; // [rsp+490h] [rbp-38h]
  CPlayer::CashChangeStateFlag v31{0}; // [rsp+4A0h] [rbp-28h] BYREF
  char *pszFileName; // [rsp+4A8h] [rbp-20h]
  unsigned int dwLeftDalant; // [rsp+4B0h] [rbp-18h]

  v8 = -1;
  dwSerial = -1;
  v11 = 0;
  v12 = 0;
  v19 = -1;
  v20 = -1;
  m_sData = pData->m_sData;
  v8 = *(_DWORD *)pData->m_sData;
  dwSerial = *(_DWORD *)&pData->m_sData[272];
  v19 = *(_DWORD *)&pData->m_sData[276];
  v20 = *(_DWORD *)&pData->m_sData[280];
  v11 = pData->m_sData[21];
  v12 = pData->m_sData[22];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(v13, m_sData + 24, 0x20uLL);
  memcpy_0(dwMemberSerial, m_sData + 56, 0x20uLL);
  memcpy_0(v18, m_sData + 240, 0x20uLL);
  memcpy_0(v16, m_sData + 224, 8uLL);
  memcpy_0(v17, m_sData + 232, 8uLL);
  memcpy_0(Source, m_sData + 88, 0x88uLL);
  v22 = &g_Guild[v8];
  v22->ReleaseTemp();
  for ( j = 0; j < v12; ++j )
  {
    v24 = &g_Player[v13[j]];
    if ( v24->m_bLive && v24->m_dwObjSerial == dwMemberSerial[j] )
      v24->m_Param.m_bGuildLock = 0;
  }
  if ( pData->m_byResult || v22->IsFill() )
  {
    v25 = &g_Player[v19];
    if ( v25->m_bLive && v25->m_dwObjSerial == v20 )
    {
      v25->SendMsg_GuildEstablishFail( 0xFFu);
      EstConsumeDalant = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
      v25->AddDalant( EstConsumeDalant, 0);
      W2M(Destination, szTran, 0x80u);
      pszFileName = v25->m_szItemHistoryFileName;
      dwLeftDalant = v25->m_Param.GetDalant();
      v4 = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
      CPlayer::s_MgrItemHistory.guild_est_money_rollback(
        v25->m_ObjID.m_wIndex,
        szTran,
        v4,
        dwLeftDalant,
        pszFileName);
    }
  }
  else
  {
    v27 = &g_Player[v19];
    if ( v27->m_bLive && v27->m_dwObjSerial == v20 )
      v27->SendMsg_AlterMoneyInform( 0);
    for ( j = 0; j < v12; ++j )
    {
      __t[j].dwSerial = dwMemberSerial[j];
      strcpy_0(__t[j].wszName, &Source[17 * j]);
      __t[j].byLv = v16[j];
      __t[j].dwPvpPoint = v18[j];
      __t[j].byClassInGuild = 0;
      __t[j].byGrade = v17[j];
    }
    v22->EstGuild( dwSerial, Destination, v11, v12, __t);
    for ( j = 0; j < v12; ++j )
    {
      pPtr = &g_Player[v13[j]];
      if ( pPtr->m_bLive && pPtr->m_dwObjSerial == dwMemberSerial[j] )
      {
        pMem = v22->LoginMember( dwMemberSerial[j], pPtr);
        v22->SendMsg_DownPacket( 0, pMem);
        pPtr->m_pUserDB->m_AvatorData.dbAvator.m_dwGuildSerial = dwSerial;
        pPtr->m_Param.m_pGuild = v22;
        pPtr->m_Param.m_pGuildMemPtr = pMem;
        pPtr->m_Param.SetClassInGuild( 0);
        pPtr->UpdateVisualVer(v31);
        pPtr->SendMsg_GuildJoinOtherInform();
      }
    }
    v6 = CGuildBattleController::Instance();
    v6->UpdatePossibleBattleGuildList();
  }
}

void CPlayer::Guild_Join_Accept_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  const char *CharNameW; // rax
  CPlayer::CashChangeStateFlag *v4; // rax
  char *CharNameA; // rax
  CGuildBattleController *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-198h] BYREF
  int v8; // [rsp+40h] [rbp-158h]
  int v9; // [rsp+44h] [rbp-154h]
  int v10; // [rsp+48h] [rbp-150h]
  unsigned int dwApplierSerial; // [rsp+4Ch] [rbp-14Ch]
  unsigned int dwMemberSerial; // [rsp+50h] [rbp-148h]
  char *m_sData; // [rsp+58h] [rbp-140h]
  CPlayer *pPlayer; // [rsp+60h] [rbp-138h]
  CGuild *v15; // [rsp+68h] [rbp-130h]
  _guild_member_info *MemberFromSerial; // [rsp+70h] [rbp-128h]
  _guild_applier_info *ApplierFromSerial; // [rsp+78h] [rbp-120h]
  _guild_member_info pSheet; // [rsp+88h] [rbp-110h] BYREF
  _guild_member_info *p; // [rsp+C8h] [rbp-D0h]
  char szTran[144]; // [rsp+E0h] [rbp-B8h] BYREF
  CPlayer::CashChangeStateFlag v21{0}; // [rsp+170h] [rbp-28h] BYREF
  char *pszFileName; // [rsp+178h] [rbp-20h]
  int n; // [rsp+180h] [rbp-18h]

  v8 = -1;
  v9 = -1;
  v10 = -1;
  dwApplierSerial = -1;
  dwMemberSerial = -1;
  m_sData = pData->m_sData;
  v8 = *(_DWORD *)pData->m_sData;
  v9 = *(_DWORD *)&pData->m_sData[4];
  v10 = *(_DWORD *)&pData->m_sData[8];
  dwApplierSerial = *(_DWORD *)&pData->m_sData[12];
  dwMemberSerial = *(_DWORD *)&pData->m_sData[16];
  pPlayer = &g_Player[v10];
  if ( pPlayer->m_bLive && pPlayer->m_dwObjSerial == dwApplierSerial )
    pPlayer->m_Param.m_bGuildLock = 0;
  v15 = &g_Guild[v8];
  if ( v15->m_dwSerial == v9 )
  {
    --v15->m_nTempMemberNum;
    if ( v15->m_nMemberNum < 50 && !pData->m_byResult )
    {
      MemberFromSerial = v15->GetMemberFromSerial( dwMemberSerial);
      if ( MemberFromSerial )
      {
        ApplierFromSerial = v15->GetApplierFromSerial( dwApplierSerial);
        if ( ApplierFromSerial )
        {
          pPlayer = ApplierFromSerial->pPlayer;
          pPlayer->m_Param.SetClassInGuild( 0);
          pPlayer->m_Param.m_pApplyGuild = 0LL;
          v15->PopApplier( dwApplierSerial, 0);
          pSheet.dwSerial = pPlayer->m_dwObjSerial;
          CharNameW = pPlayer->m_Param.GetCharNameW();
          strcpy_0(pSheet.wszName, CharNameW);
          pSheet.byLv = pPlayer->GetLevel();
          pSheet.dwPvpPoint = (int)pPlayer->m_Param.GetPvPPoint();
          pSheet.byClassInGuild = pPlayer->m_Param.GetClassInGuild();
          pSheet.pPlayer = pPlayer;
          p = v15->PushMember( &pSheet);
          if ( p )
            v15->SendMsg_GuildJoinAcceptInform( p, dwMemberSerial);
          pPlayer->m_Param.m_pGuild = v15;
          pPlayer->m_Param.m_pGuildMemPtr = p;
          pPlayer->UpdateVisualVer(v21);
          pPlayer->SendMsg_GuildJoinOtherInform();
          
            v15->SendMsg_GuildMemberLogin(
            pPlayer->m_dwObjSerial,
            pPlayer->m_wRegionMapIndex,
            pPlayer->m_wRegionIndex);
          W2M(MemberFromSerial->wszName, szTran, 0x80u);
          pszFileName = v15->m_szHistoryFileName;
          CharNameA = pPlayer->m_Param.GetCharNameA();
          CGuild::s_MgrHistory.join_member(
            CharNameA,
            pPlayer->m_dwObjSerial,
            szTran,
            MemberFromSerial->dwSerial,
            v15->m_nMemberNum,
            pszFileName);
          n = pPlayer->m_ObjID.m_wIndex;
          v6 = CGuildBattleController::Instance();
          v6->JoinGuild(n, v15->m_dwSerial, pPlayer->m_dwObjSerial);
        }
      }
    }
  }
}

void CPlayer::Guild_Self_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildRoomSystem *Instance; // rax
  CGuildRoomSystem *v4; // rax
  CGuildBattleController *v5; // rax
  CGuildMasterEffect *v6; // rax
  CPlayer::CashChangeStateFlag *v7; // rax
  char *CharNameA; // rax
  int v9; // eax
  __int64 v10; // [rsp+0h] [rbp-E8h] BYREF
  char v11; // [rsp+40h] [rbp-A8h]
  CPlayer *pkPlayer; // [rsp+48h] [rbp-A0h]
  CGuild *v13; // [rsp+50h] [rbp-98h]
  int v14; // [rsp+58h] [rbp-90h]
  unsigned int dwMemberSerial; // [rsp+5Ch] [rbp-8Ch]
  int v16; // [rsp+60h] [rbp-88h]
  int v17; // [rsp+64h] [rbp-84h]
  char *m_sData; // [rsp+68h] [rbp-80h]
  char v19; // [rsp+70h] [rbp-78h]
  _qry_case_disjointguild v20; // [rsp+88h] [rbp-60h] BYREF
  CPlayer::CashChangeStateFlag v21{0}; // [rsp+94h] [rbp-54h] BYREF
  CUserDB *m_pUserDB; // [rsp+98h] [rbp-50h]
  int n; // [rsp+A0h] [rbp-48h]
  CGuild *m_pGuild; // [rsp+A8h] [rbp-40h]
  CUserDB *v25; // [rsp+B0h] [rbp-38h]
  int m_wIndex; // [rsp+B8h] [rbp-30h]
  CGuild *v27; // [rsp+C0h] [rbp-28h]
  unsigned __int8 Grade; // [rsp+C8h] [rbp-20h]
  char *pszFileName; // [rsp+D0h] [rbp-18h]

  v11 = 0;
  pkPlayer = 0LL;
  v13 = 0LL;
  v14 = -1;
  dwMemberSerial = -1;
  v16 = -1;
  v17 = -1;
  m_sData = pData->m_sData;
  v14 = *(_DWORD *)&pData->m_sData[8];
  dwMemberSerial = *(_DWORD *)pData->m_sData;
  v16 = *(_DWORD *)&pData->m_sData[12];
  v17 = *(_DWORD *)&pData->m_sData[16];
  v19 = 0;
  pkPlayer = &g_Player[v14];
  if ( pkPlayer->m_bLive && pkPlayer->m_dwObjSerial == dwMemberSerial )
  {
    pkPlayer->m_Param.m_bGuildLock = 0;
    v19 = 1;
  }
  if ( !v19 )
    pkPlayer = 0LL;
  v19 = 0;
  v13 = &g_Guild[v16];
  if ( v13->IsFill() && v13->m_dwSerial == v17 )
    v19 = 1;
  if ( v19 )
  {
    if ( !pkPlayer )
      goto LABEL_22;
    if ( !pkPlayer->m_pUserDB )
      goto LABEL_22;
    if ( !pkPlayer->m_Param.m_pGuild )
      goto LABEL_22;
    m_pUserDB = pkPlayer->m_pUserDB;
    n = pkPlayer->m_ObjID.m_wIndex;
    m_pGuild = pkPlayer->m_Param.m_pGuild;
    Instance = CGuildRoomSystem::GetInstance();
    if (!Instance->IsGuildRoomMemberIn(m_pGuild->m_dwSerial, n, m_pUserDB->m_dwSerial))
      goto LABEL_22;
    v25 = pkPlayer->m_pUserDB;
    m_wIndex = pkPlayer->m_ObjID.m_wIndex;
    v27 = pkPlayer->m_Param.m_pGuild;
    v4 = CGuildRoomSystem::GetInstance();
    if (v4->SetPlayerOut(v27->m_dwSerial, m_wIndex, v25->m_dwSerial))
    {
      pkPlayer->SendMsg_GuildSelfLeaveResult( 0xFFu);
    }
    else
    {
LABEL_22:
      v5 = CGuildBattleController::Instance();
      v5->LeaveGuild(pkPlayer);
      if ( pkPlayer && pkPlayer->m_Param.m_byClassInGuild == 2 )
      {
        Grade = v13->GetGrade();
        v6 = CGuildMasterEffect::GetInstance();
        v6->out_player(pkPlayer, Grade);
      }
      v13->SendMsg_LeaveMember( dwMemberSerial, 1, 0);
      v13->PopMember( dwMemberSerial);
      if ( pkPlayer )
      {
        pkPlayer->m_Param.m_pGuild = 0LL;
        pkPlayer->SendMsg_GuildSelfLeaveResult( 0);
        pkPlayer->UpdateVisualVer(v21);
        pkPlayer->SendMsg_GuildJoinOtherInform();
        pkPlayer->SetLastAttBuff( 0);
        pszFileName = v13->m_szHistoryFileName;
        CharNameA = pkPlayer->m_Param.GetCharNameA();
        CGuild::s_MgrHistory.leave_member(
          CharNameA,
          pkPlayer->m_dwObjSerial,
          1,
          v13->m_nMemberNum,
          pszFileName,
          0);
      }
      if ( v13->GetMemberNum() <= 0 )
      {
        v20.in_guildserial = v13->m_dwSerial;
        v20.tmp_guildindex = v13->m_nIndex;
        v9 = v20.size();
        g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x16u, (char *)&v20, v9);
      }
    }
  }
  else if ( pkPlayer )
  {
    pkPlayer->SendMsg_GuildSelfLeaveResult( 0xFFu);
  }
}

void CPlayer::Guild_Force_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildBattleController *v3; // rax
  CGuildRoomSystem *Instance; // rax
  CGuildRoomSystem *v5; // rax
  CPlayer::CashChangeStateFlag *v6; // rax
  char *CharNameA; // rax
  __int64 v8; // [rsp+0h] [rbp-C8h] BYREF
  CGuild *v9; // [rsp+40h] [rbp-88h]
  unsigned int dwMemberSerial; // [rsp+48h] [rbp-80h]
  int v11; // [rsp+4Ch] [rbp-7Ch]
  int v12; // [rsp+50h] [rbp-78h]
  char *m_sData; // [rsp+58h] [rbp-70h]
  char v14; // [rsp+60h] [rbp-68h]
  _guild_member_info *MemberFromSerial; // [rsp+68h] [rbp-60h]
  CPlayer *pPlayer; // [rsp+70h] [rbp-58h]
  CPlayer::CashChangeStateFlag v17{0}; // [rsp+78h] [rbp-50h] BYREF
  CUserDB *m_pUserDB; // [rsp+80h] [rbp-48h]
  int n; // [rsp+88h] [rbp-40h]
  CGuild *m_pGuild; // [rsp+90h] [rbp-38h]
  CUserDB *v21; // [rsp+98h] [rbp-30h]
  int m_wIndex; // [rsp+A0h] [rbp-28h]
  CGuild *v23; // [rsp+A8h] [rbp-20h]
  char *pszFileName; // [rsp+B0h] [rbp-18h]
  int nMemNum; // [rsp+B8h] [rbp-10h]

  v9 = 0LL;
  dwMemberSerial = -1;
  v11 = -1;
  v12 = -1;
  m_sData = pData->m_sData;
  dwMemberSerial = *(_DWORD *)pData->m_sData;
  v11 = *(_DWORD *)&pData->m_sData[4];
  v12 = *(_DWORD *)&pData->m_sData[8];
  v14 = 0;
  v9 = &g_Guild[v11];
  if ( v9->IsFill() && v9->m_dwSerial == v12 )
    v14 = 1;
  if ( v14 )
  {
    MemberFromSerial = v9->GetMemberFromSerial( dwMemberSerial);
    if ( MemberFromSerial )
    {
      if ( MemberFromSerial->pPlayer )
      {
        v3 = CGuildBattleController::Instance();
        v3->LeaveGuild(MemberFromSerial->pPlayer);
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer->m_Param.m_pGuild )
        {
          m_pUserDB = pPlayer->m_pUserDB;
          n = pPlayer->m_ObjID.m_wIndex;
          m_pGuild = pPlayer->m_Param.m_pGuild;
          Instance = CGuildRoomSystem::GetInstance();
          if (Instance->IsGuildRoomMemberIn(m_pGuild->m_dwSerial, n, m_pUserDB->m_dwSerial))
          {
            v21 = pPlayer->m_pUserDB;
            m_wIndex = pPlayer->m_ObjID.m_wIndex;
            v23 = pPlayer->m_Param.m_pGuild;
            v5 = CGuildRoomSystem::GetInstance();
            v5->SetPlayerOut(v23->m_dwSerial, m_wIndex, v21->m_dwSerial);
          }
        }
        MemberFromSerial->pPlayer->m_Param.m_bGuildLock = 0;
        MemberFromSerial->pPlayer->m_Param.m_pGuild = 0LL;
        MemberFromSerial->pPlayer->m_Param.m_pGuildMemPtr = 0LL;
        MemberFromSerial->pPlayer->UpdateVisualVer(v17);
        MemberFromSerial->pPlayer->SendMsg_GuildJoinOtherInform();
        MemberFromSerial->pPlayer->SetLastAttBuff( 0);
        if ( m_sData[24] )
          MemberFromSerial->pPlayer->SendMsg_GuildForceLeaveBoradori();
        pszFileName = v9->m_szHistoryFileName;
        nMemNum = v9->m_nMemberNum - 1;
        CharNameA = MemberFromSerial->pPlayer->m_Param.GetCharNameA();
        CGuild::s_MgrHistory.leave_member(
          CharNameA,
          MemberFromSerial->dwSerial,
          0,
          nMemNum,
          pszFileName,
          m_sData[24]);
      }
      v9->SendMsg_LeaveMember( dwMemberSerial, 0, m_sData[24]);
      v9->PopMember( dwMemberSerial);
    }
  }
}

void CPlayer::Guild_Push_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int Dalant; // eax
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  int v5; // [rsp+50h] [rbp-B8h]
  int v6; // [rsp+54h] [rbp-B4h]
  unsigned int dwIOerSerial; // [rsp+58h] [rbp-B0h]
  char Destination[28]; // [rsp+68h] [rbp-A0h] BYREF
  unsigned int dwPush; // [rsp+84h] [rbp-84h]
  unsigned int dwPopDalant; // [rsp+88h] [rbp-80h]
  long double dTotalGold; // [rsp+90h] [rbp-78h]
  long double dTotalDalant; // [rsp+98h] [rbp-70h]
  unsigned __int8 pbyDate[20]; // [rsp+A4h] [rbp-64h] BYREF
  char *m_sData; // [rsp+B8h] [rbp-50h]
  CGuild *v15; // [rsp+C0h] [rbp-48h]
  _guild_member_info *MemberFromSerial; // [rsp+C8h] [rbp-40h]
  CPlayer *pPlayer; // [rsp+D0h] [rbp-38h]
  char *pszFileName; // [rsp+E0h] [rbp-28h]
  unsigned int dwLeftGold; // [rsp+E8h] [rbp-20h]

  v5 = -1;
  v6 = -1;
  dwIOerSerial = -1;
  dwPush = 0;
  dwPopDalant = 0;
  dTotalGold = 0.0;
  dTotalDalant = 0.0;
  m_sData = pData->m_sData;
  v5 = *(_DWORD *)&pData->m_sData[24];
  v6 = *(_DWORD *)&pData->m_sData[28];
  dwIOerSerial = *(_DWORD *)pData->m_sData;
  dwPopDalant = *(_DWORD *)&pData->m_sData[36];
  dwPush = *(_DWORD *)&pData->m_sData[32];
  dTotalDalant = *(long double *)&pData->m_sData[56];
  dTotalGold = *(long double *)&pData->m_sData[48];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(pbyDate, m_sData + 40, 4uLL);
  v15 = &g_Guild[v5];
  if ( v15->m_dwSerial == v6 )
  {
    v15->m_bIOWait = 0;
    if ( pData->m_byResult )
    {
      MemberFromSerial = v15->GetMemberFromSerial( dwIOerSerial);
      if ( MemberFromSerial )
      {
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer )
        {
          pPlayer->AddGold( dwPush, 1);
          pPlayer->AddDalant( dwPopDalant, 1);
          pszFileName = pPlayer->m_szItemHistoryFileName;
          dwLeftGold = pPlayer->m_Param.GetGold();
          Dalant = pPlayer->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.guild_pop_money_rollback(
            pPlayer->m_ObjID.m_wIndex,
            v15->m_aszName,
            dwPopDalant,
            dwPush,
            Dalant,
            dwLeftGold,
            pszFileName);
        }
      }
    }
    else
    {
      v15->m_byMoneyOutputKind = 0;
      
        v15->IOMoney(
        Destination,
        dwIOerSerial,
        (double)(int)dwPopDalant,
        (double)(int)dwPush,
        dTotalDalant,
        dTotalGold,
        pbyDate,
        1);
    }
  }
}

void CPlayer::Guild_Pop_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int Dalant; // eax
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  __int64 dTotalDalant; // [rsp+28h] [rbp-E0h]
  __int64 dTotalGold; // [rsp+30h] [rbp-D8h]
  __int64 bInPut; // [rsp+40h] [rbp-C8h]
  __int64 v8; // [rsp+48h] [rbp-C0h]
  int v9; // [rsp+50h] [rbp-B8h]
  unsigned int v10; // [rsp+54h] [rbp-B4h]
  unsigned int dwIOerSerial; // [rsp+58h] [rbp-B0h]
  char Destination[28]; // [rsp+68h] [rbp-A0h] BYREF
  unsigned int dwPopGold; // [rsp+84h] [rbp-84h]
  unsigned int dwPush; // [rsp+88h] [rbp-80h]
  long double v15; // [rsp+90h] [rbp-78h]
  long double v16; // [rsp+98h] [rbp-70h]
  unsigned __int8 pbyDate[20]; // [rsp+A4h] [rbp-64h] BYREF
  char *m_sData; // [rsp+B8h] [rbp-50h]
  CGuild *v19; // [rsp+C0h] [rbp-48h]
  _guild_member_info *MemberFromSerial; // [rsp+C8h] [rbp-40h]
  CPlayer *pPlayer; // [rsp+D0h] [rbp-38h]
  char *pszFileName; // [rsp+E0h] [rbp-28h]
  unsigned int dwLeftGold; // [rsp+E8h] [rbp-20h]

  v9 = -1;
  v10 = -1;
  dwIOerSerial = -1;
  dwPopGold = 0;
  dwPush = 0;
  v15 = 0.0;
  v16 = 0.0;
  m_sData = pData->m_sData;
  v9 = *(_DWORD *)&pData->m_sData[24];
  v10 = *(_DWORD *)&pData->m_sData[28];
  dwIOerSerial = *(_DWORD *)pData->m_sData;
  dwPush = *(_DWORD *)&pData->m_sData[36];
  dwPopGold = *(_DWORD *)&pData->m_sData[32];
  v16 = *(double *)&pData->m_sData[56];
  v15 = *(double *)&pData->m_sData[48];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(pbyDate, m_sData + 40, 4uLL);
  v19 = &g_Guild[v9];
  if ( v19->m_dwSerial == v10 )
  {
    v19->m_bIOWait = 0;
	    if ( m_sData[64] )
	    {
	      const unsigned __int8 qryRet = static_cast<unsigned __int8>(m_sData[64]);
	      const unsigned int ioerSerialForLog = *reinterpret_cast<unsigned int *>(m_sData);
	      const unsigned int subGold = dwPopGold;
	      const unsigned int subDalant = dwPush;
	      g_Main.m_logSystemError.Write(
	        
	        "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
	        "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%u) SubG(%u) %s(%u)\r\n"
	        "\t\t_qry_case_outputgmoney Ret(%u) Fail!",
	        v10,
	        v16,
	        v15,
	        subDalant,
	        subGold,
	        Destination,
	        ioerSerialForLog,
	        qryRet);
	    }
    else if ( !pData->m_byResult )
    {
      
        v19->IOMoney(
        Destination,
        dwIOerSerial,
        -0.0 - (double)(int)dwPush,
        -0.0 - (double)(int)dwPopGold,
        v16,
        v15,
        pbyDate,
        0);
      MemberFromSerial = v19->GetMemberFromSerial( dwIOerSerial);
      if ( MemberFromSerial )
      {
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer )
        {
          pPlayer->AddDalant( dwPush, 1);
          pPlayer->AddGold( dwPopGold, 1);
          pszFileName = pPlayer->m_szItemHistoryFileName;
          dwLeftGold = pPlayer->m_Param.GetGold();
          Dalant = pPlayer->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.guild_pop_money(
            pPlayer->m_ObjID.m_wIndex,
            v19->m_aszName,
            dwPush,
            dwPopGold,
            Dalant,
            dwLeftGold,
            pszFileName);
          pPlayer->SendMsg_AlterMoneyInform( 0);
        }
        else
        {
          if (dwPush)
            g_Main.Push_ChargeItem(dwIOerSerial, 0xFFFFFFFF, dwPush, 0xFFFFFFFu, 1u);
          if (dwPopGold)
            g_Main.Push_ChargeItem(dwIOerSerial, 0xFFFFFFFF, dwPopGold, 0xFFFFFFFu, 2u);
          g_Main.m_logSystemError.Write(
            "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
            "\t\tPush Charge Money BECAUSE Poper Connection Closed : Poper Serial(%u) Pop Dalant(%u) Pop Gold(%u)\r\n",
            dwIOerSerial,
            dwPush,
            dwPopGold);
        }
      }
    }
  }
}

void CPlayer::Guild_Buy_Emblem_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-F8h] BYREF
  __int64 dTotalDalant; // [rsp+28h] [rbp-D0h]
  unsigned __int8 *pbyDate; // [rsp+38h] [rbp-C0h]
  bool bInPut[16]; // [rsp+40h] [rbp-B8h]
  int v7; // [rsp+50h] [rbp-A8h]
  unsigned int v8; // [rsp+54h] [rbp-A4h]
  unsigned int dwEmblemBack; // [rsp+58h] [rbp-A0h]
  unsigned int dwEmblemMark; // [rsp+5Ch] [rbp-9Ch]
  unsigned int dwIOerSerial; // [rsp+60h] [rbp-98h]
  char Destination[28]; // [rsp+78h] [rbp-80h] BYREF
  int v13; // [rsp+94h] [rbp-64h]
  int v14; // [rsp+98h] [rbp-60h]
  long double dTotalGold; // [rsp+A0h] [rbp-58h]
  long double v16; // [rsp+A8h] [rbp-50h]
  unsigned __int8 v17[20]; // [rsp+B4h] [rbp-44h] BYREF
  char *m_sData; // [rsp+C8h] [rbp-30h]
  CGuild *v19; // [rsp+D0h] [rbp-28h]

  v7 = -1;
  v8 = -1;
  dwEmblemBack = -1;
  dwEmblemMark = -1;
  dwIOerSerial = -1;
  v13 = 0;
  v14 = 0;
  dTotalGold = 0.0;
  v16 = 0.0;
  memset(v17, 0, 4);
  m_sData = pData->m_sData;
  v7 = *(_DWORD *)&pData->m_sData[24];
  v8 = *(_DWORD *)pData->m_sData;
  dwEmblemBack = *(_DWORD *)&pData->m_sData[4];
  dwEmblemMark = *(_DWORD *)&pData->m_sData[8];
  dwIOerSerial = *(_DWORD *)&pData->m_sData[16];
  v14 = *(_DWORD *)&pData->m_sData[12];
  v16 = *(double *)&pData->m_sData[56];
  dTotalGold = *(double *)&pData->m_sData[48];
  memcpy_0(v17, &pData->m_sData[20], 4uLL);
  strcpy_0(Destination, m_sData + 28);
  v19 = &g_Guild[v7];
  if ( v19->m_dwSerial == v8 )
  {
    v19->m_bIOWait = 0;
	    if ( m_sData[64] )
	    {
	      const unsigned __int8 qryRet = static_cast<unsigned __int8>(m_sData[64]);
	      const unsigned int ioerSerialForLog = *((_DWORD *)m_sData + 4);
	      const int subDalant = v14;
	      g_Main.m_logSystemError.Write(
	        
	        "CPlayer::Guild_Buy_Emblem_Complete(...) : \r\n"
	        "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%d) %s(%u)\r\n"
	        "\t\tqry_case_buyemblem Ret(%u) Fail!",
	        v8,
	        v16,
	        dTotalGold,
	        subDalant,
	        m_sData + 28,
	        ioerSerialForLog,
	        qryRet);
	    }
    else if ( !pData->m_byResult )
    {
      v19->m_byMoneyOutputKind = 1;
      v19->IOMoney( Destination, dwIOerSerial, (double)v14, 0.0, v16, dTotalGold, v17, 0);
      v19->UpdateEmblem( dwEmblemBack, dwEmblemMark);
    }
  }
}

void CPlayer::Guild_Disjoint_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-48h] BYREF
  int v4; // [rsp+20h] [rbp-28h]
  int v5; // [rsp+24h] [rbp-24h]
  char *m_sData; // [rsp+28h] [rbp-20h]
  CGuild *v7; // [rsp+30h] [rbp-18h]

  v4 = -1;
  v5 = -1;
  m_sData = pData->m_sData;
  v4 = *(_DWORD *)&pData->m_sData[4];
  v5 = *(_DWORD *)pData->m_sData;
  v7 = &g_Guild[v4];
  if ( v7 && v7->m_dwSerial == v5 && !pData->m_byResult )
  {
    if ( v7->m_nApplierNum > 0 )
      v7->SendMsg_GuildDisjointInform();
    v7->Release();
  }
}

void CPlayer::Guild_Update_GuildMater_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-68h] BYREF
  int v4; // [rsp+30h] [rbp-38h]
  int v5; // [rsp+34h] [rbp-34h]
  unsigned int in_guild_prev_masterSerial; // [rsp+38h] [rbp-30h]
  unsigned __int8 v7; // [rsp+3Ch] [rbp-2Ch]
  unsigned int in_guild_new_masterSerial; // [rsp+40h] [rbp-28h]
  unsigned __int8 in_guild_new_masterPrevGrade; // [rsp+44h] [rbp-24h]
  char *m_sData; // [rsp+48h] [rbp-20h]
  CGuild *v11; // [rsp+50h] [rbp-18h]

  v4 = -1;
  v5 = -1;
  m_sData = pData->m_sData;
  v4 = *(_DWORD *)&pData->m_sData[4];
  v5 = *(_DWORD *)pData->m_sData;
  in_guild_prev_masterSerial = *(_DWORD *)&pData->m_sData[8];
  v7 = pData->m_sData[12];
  in_guild_new_masterSerial = *(_DWORD *)&pData->m_sData[16];
  in_guild_new_masterPrevGrade = pData->m_sData[20];
  v11 = &g_Guild[v4];
  if ( v11 && v11->m_dwSerial == v5 && !pData->m_byResult )
    
      v11->DB_Update_GuildMaster_Complete(
      in_guild_prev_masterSerial,
      v7,
      in_guild_new_masterSerial,
      in_guild_new_masterPrevGrade);
}

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

void CPlayer::pc_GuildBattleBlock(bool bBlock)
{
  this->m_bBlockGuildBattleMsg = bBlock;
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
    const unsigned __int64 maxHp = static_cast<unsigned __int64>(pTar->GetMaxHP());
    const unsigned __int64 curHp = static_cast<unsigned __int64>(pTar->GetHP());
    if (maxHp > 0)
    {
      this->m_TargetObject.wHPRate =
        static_cast<unsigned __int16>((10000u * curHp) / maxHp);
    }
    else
    {
      this->m_TargetObject.wHPRate = 0;
    }
  }

  this->SendMsg_SetTargetObjectResult(static_cast<char>(result), bForce);
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
      if (!targetPlayer->m_bCorpse)
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

void CPlayer::SendMsg_AnimusInvenChange(char byErrCode)
{
  unsigned __int8 pbyType[2] = {13, 5};
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
        if (wReplaceSerial != 0xFFFF)
        {
          replaceItem = targetStorage->GetPtrFromSerial(wReplaceSerial);
          if (!replaceItem || replaceItem->m_byTableCode != 24)
          {
            resultCode = 2;
            goto SEND_ANIMUS_CHANGE_RESULT;
          }
          if (replaceItem->m_bLock)
          {
            resultCode = 11;
            goto SEND_ANIMUS_CHANGE_RESULT;
          }
          if (replaceItem->m_wItemIndex != sourceItem->m_wItemIndex)
          {
            resultCode = 3;
            goto SEND_ANIMUS_CHANGE_RESULT;
          }
          if (replaceItem == this->m_pRecalledAnimusItem)
          {
            resultCode = 6;
            goto SEND_ANIMUS_CHANGE_RESULT;
          }
        }
        if (wReplaceSerial == 0xFFFF && targetStorage->GetIndexEmptyCon() == 255)
        {
          resultCode = 5;
        }
        else if (wReplaceSerial == 0xFFFF && !pItem->byStorageCode)
        {
          record = g_Main.m_tblItemData[24].GetRecord(sourceItem->m_wItemIndex);
          if (!record)
          {
            resultCode = 8;
            goto SEND_ANIMUS_CHANGE_RESULT;
          }

          bool canDuplicate = false;
          if (*reinterpret_cast<int *>(&record[3].m_strCode[60]) == 1)
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
              void *historyEffect = this->m_Param.m_ppHistoryEffect[index];
              if (!historyEffect)
              {
                break;
              }
              if (*reinterpret_cast<int *>(reinterpret_cast<char *>(historyEffect) + 1440))
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
            goto SEND_ANIMUS_CHANGE_RESULT;
          }

          for (int index = 0; index < targetStorage->m_nUsedNum; ++index)
          {
            _STORAGE_LIST::_db_con *item = &targetStorage->m_pStorageList[index];
            if (item->m_bLoad && item->m_wItemIndex == sourceItem->m_wItemIndex)
            {
              resultCode = 4;
              goto SEND_ANIMUS_CHANGE_RESULT;
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

SEND_ANIMUS_CHANGE_RESULT:
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
      this->SendMsg_AnimusInvenChange(0xFFu);
      return;
    }

    if (replaceItem)
    {
      _STORAGE_LIST::_db_con replaceBackup;
      std::memcpy(&replaceBackup, replaceItem, sizeof(replaceBackup));
      if (!this->Emb_AddStorage(sourceStorage->m_nListCode, &replaceBackup, true, false))
      {
        this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
        this->SendMsg_AnimusInvenChange(0xFFu);
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
        this->SendMsg_AnimusInvenChange(0xFFu);
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
        this->SendMsg_AnimusInvenChange(0xFFu);
        return;
      }
    }
    else if (!this->Emb_AddStorage(targetStorage->m_nListCode, &sourceBackup, true, false))
    {
      this->Emb_AddStorage(sourceStorage->m_nListCode, &sourceBackup, true, false);
      this->SendMsg_AnimusInvenChange(0xFFu);
      return;
    }
    this->Emb_EquipLink();
  }
  this->SendMsg_AnimusInvenChange(resultCode);
}

void CPlayer::SendMsg_BuddyDelResult(char byRetCode, unsigned int dwSerial)
{
  char msg[5]{};
  msg[0] = byRetCode;
  std::memcpy(&msg[1], &dwSerial, sizeof(dwSerial));

  unsigned __int8 type[2] = {31, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 5u);
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
        else if (*reinterpret_cast<int *>(&animusItemRecord[3].m_strCode[60]) == 1)
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

            adjustedHP = static_cast<unsigned __int16>(LOWORD(animusItem->m_dwLv));
            adjustedFP = static_cast<unsigned __int16>(HIWORD(animusItem->m_dwLv));

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

            if (!resultCode && !FindEmptyAnimusSlot())
            {
              resultCode = 5;
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

    newAnimus = CreateAnimusObject(this, animusItem);
    if (newAnimus)
    {
      this->m_pRecalledAnimusItem = animusItem;
      this->m_pRecalledAnimusChar = newAnimus;
      this->m_wTimeFreeRecallSerial = static_cast<unsigned __int16>(-1);
      animusItem->lock(true);
    }
    else
    {
      resultCode = 5;
    }
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
    this->SendMsg_AnimusReturnResult(ret, 0xFFFFu, 0);
  }
  else
  {
    this->_AnimusReturn(0);
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

void CPlayer::pc_GuildEstablishRequest(char *pwszGuildName)
{
  unsigned __int8 resultCode = 0;
  int selectedGuildIndex = -1;
  unsigned __int8 memberCount = 0;
  int memberIndices[8] = {};
  unsigned int memberSerials[8] = {};
  unsigned int memberPvpPoints[8] = {};
  unsigned __int8 memberLevels[8] = {};
  unsigned __int8 memberGrades[8] = {};
  char memberNames[136] = {};

  std::memset(memberIndices, 0xFF, sizeof(memberIndices));
  std::memset(memberSerials, 0xFF, sizeof(memberSerials));

  CPartyPlayer *party = this->m_pPartyMgr;
  CPartyPlayer **partyMembers = party ? party->GetPtrPartyMember() : nullptr;
  CGuild *reservedGuild = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    resultCode = 106;
    goto SEND_RESULT;
  }

  for (char *cursor = pwszGuildName; *cursor; ++cursor)
  {
    if (*cursor == ' ' || *cursor == '\'')
    {
      resultCode = 15;
      goto SEND_RESULT;
    }
  }

  if (!CTSingleton<CNationSettingManager>::Instance()->IsNormalString(pwszGuildName))
  {
    resultCode = 15;
    goto SEND_RESULT;
  }

  if (!IsSQLValidString(pwszGuildName))
  {
    g_Main.m_logSystemError.Write(
      "CPlayer::pc_GuildEstablishRequest() : %u(%s) ::IsSQLValidString(pwszGuildName(%s)) Invalid!",
      this->m_dwObjSerial,
      this->m_Param.GetCharNameA(),
      pwszGuildName);
    resultCode = 15;
    goto SEND_RESULT;
  }

  if (this->IsPunished(1u, true))
  {
    return;
  }

  if (!party || !party->IsPartyMode())
  {
    resultCode = 1;
    goto SEND_RESULT;
  }

  if (!party->IsPartyBoss())
  {
    resultCode = 2;
    goto SEND_RESULT;
  }

  if (g_Main.IsReleaseServiceMode() && this->m_byUserDgr)
  {
    resultCode = 13;
    goto SEND_RESULT;
  }

  if (this->m_Param.GetDalant() < g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant())
  {
    resultCode = 12;
    goto SEND_RESULT;
  }

  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    if (!g_Guild[guildIndex].IsFill() && !g_Guild[guildIndex].m_bDBWait)
    {
      reservedGuild = &g_Guild[guildIndex];
      break;
    }
  }

  if (!reservedGuild)
  {
    resultCode = 10;
    goto SEND_RESULT;
  }

  if (!partyMembers)
  {
    resultCode = 1;
    goto SEND_RESULT;
  }

  for (int partySlot = 0; partySlot < 8 && partyMembers[partySlot]; ++partySlot)
  {
    CPlayer *member = &g_Player[partyMembers[partySlot]->m_wZoneIndex];
    if (!member->m_bLive)
    {
      resultCode = 9;
      goto SEND_RESULT;
    }

    if (member->IsPunished(1u, true))
    {
      resultCode = 70;
      goto SEND_RESULT;
    }

    if (member->m_Param.m_pGuild || member->m_Param.m_bGuildLock)
    {
      resultCode = 7;
      goto SEND_RESULT;
    }

    if (member->m_Param.m_pApplyGuild)
    {
      resultCode = 8;
      goto SEND_RESULT;
    }

    if (!member->m_Param.m_pClassHistory[0])
    {
      resultCode = 11;
      goto SEND_RESULT;
    }

    if (member->m_Param.GetRaceCode() != this->m_Param.GetRaceCode())
    {
      resultCode = 70;
      goto SEND_RESULT;
    }

    memberIndices[partySlot] = member->m_id.wIndex;
    memberSerials[partySlot] = member->m_id.dwSerial;
    memberPvpPoints[partySlot] = static_cast<unsigned int>(static_cast<int>(member->m_Param.GetPvPPoint()));
    memberLevels[partySlot] = static_cast<unsigned __int8>(member->m_Param.GetLevel());
    memberGrades[partySlot] = member->m_Param.m_byPvPGrade;
    strcpy_0(&memberNames[17 * partySlot], member->m_Param.GetCharNameW());
    ++memberCount;
  }

  if (g_Main.IsReleaseServiceMode())
  {
    if (this->m_byUserDgr)
    {
      resultCode = 13;
      goto SEND_RESULT;
    }

    if (memberCount < 8u)
    {
      resultCode = 9;
      goto SEND_RESULT;
    }
  }
  else if (!this->m_byUserDgr && memberCount < 8u)
  {
    resultCode = 9;
    goto SEND_RESULT;
  }

  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    if ((g_Guild[guildIndex].IsFill() || g_Guild[guildIndex].m_bDBWait)
        && !strcmp_0(pwszGuildName, g_Guild[guildIndex].m_wszName))
    {
      resultCode = 4;
      goto SEND_RESULT;
    }
  }

  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    if (!g_Guild[guildIndex].IsFill() && !g_Guild[guildIndex].m_bDBWait)
    {
      g_Guild[guildIndex].m_nIndex = guildIndex;
      g_Guild[guildIndex].m_bDBWait = true;
      strcpy_0(g_Guild[guildIndex].m_wszName, pwszGuildName);
      selectedGuildIndex = guildIndex;
      break;
    }
  }

  if (selectedGuildIndex == -1)
  {
    resultCode = 10;
    goto SEND_RESULT;
  }

SEND_RESULT:
  if (resultCode)
  {
    this->SendMsg_GuildEstablishFail(static_cast<char>(resultCode));
    return;
  }

  _qry_case_insertguild_local query;
  query.in_guildindex = static_cast<unsigned int>(selectedGuildIndex);
  query.tmp_Esterindex = this->m_id.wIndex;
  query.tmp_Esterserial = this->m_id.dwSerial;
  query.in_guildRace = static_cast<unsigned __int8>(this->m_Param.GetRaceCode());
  query.in_membernum = memberCount;
  strcpy_0(query.in_w_guildName, pwszGuildName);
  std::memcpy(query.in_memberindex, memberIndices, sizeof(query.in_memberindex));
  std::memcpy(query.in_memberserial, memberSerials, sizeof(query.in_memberserial));
  std::memcpy(query.tmp_pvp, memberPvpPoints, sizeof(query.tmp_pvp));
  std::memcpy(query.tmp_lv, memberLevels, sizeof(query.tmp_lv));
  std::memcpy(query.tmp_grade, memberGrades, sizeof(query.tmp_grade));
  std::memcpy(query.tmp_w_membername, memberNames, sizeof(query.tmp_w_membername));

  if (!g_Main.PushDQSData(
        0xFFFFFFFFu,
        nullptr,
        0x0Fu,
        reinterpret_cast<char *>(&query),
        static_cast<int>(query.size())))
  {
    return;
  }

  for (int partySlot = 0; partySlot < memberCount; ++partySlot)
  {
    CPlayer *member = &g_Player[partyMembers[partySlot]->m_wZoneIndex];
    member->m_Param.m_bGuildLock = true;
  }

  const unsigned int consumeDalant = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
  this->SubDalant(consumeDalant);

  const int level = this->m_Param.GetLevel();
  if (IsEconomyFeeLevel(level))
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(this->m_Param.GetRaceCode()),
      level,
      consumeDalant);
  }
}

void CPlayer::pc_GuildDownLoadRequest()
{
  if (!this->m_Param.m_pGuild)
  {
    return;
  }

  this->m_bGuildDownload = true;
  if (this->m_Param.m_pGuildMemPtr)
  {
    this->m_Param.m_pGuild->SendMsg_DownPacket(1u, this->m_Param.m_pGuildMemPtr);
  }
}

void CPlayer::pc_GuildJoinApplyRequest(char *pwszGuildName)
{
  unsigned __int8 result = 0;
  CGuild *applyGuild = GetGuildPtrFromName(g_Guild, MAX_GUILD, pwszGuildName);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else
  {
    if (this->IsPunished(1u, true))
    {
      return;
    }

    if (!applyGuild)
    {
      result = static_cast<unsigned __int8>(-56);
    }
    else if (this->m_Param.m_bGuildLock)
    {
      result = static_cast<unsigned __int8>(-51);
    }
    else if (this->m_Param.m_pGuild)
    {
      result = 22;
    }
    else if (this->m_Param.m_pApplyGuild)
    {
      result = 23;
    }
    else if (applyGuild->m_nMemberNum >= 50)
    {
      result = 24;
    }
    else if (applyGuild->m_nApplierNum >= 32)
    {
      result = 25;
    }
    else if (this->m_Param.GetRaceCode() != static_cast<int>(applyGuild->m_byRace))
    {
      result = 70;
    }
    else if (g_Main.IsReleaseServiceMode() && this->m_byUserDgr)
    {
      result = static_cast<unsigned __int8>(-1);
    }
  }

  if (!result)
  {
    applyGuild->PushApplier(this);
    this->m_Param.m_pApplyGuild = applyGuild;
  }

  this->SendMsg_GuildJoinApplyResult(static_cast<char>(result), applyGuild);
}

void CPlayer::pc_GuildJoinApplyCancelRequest()
{
  unsigned __int8 result = 0;
  CGuild *applyGuild = this->m_Param.m_pApplyGuild;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else if (this->m_Param.m_bGuildLock)
  {
    result = static_cast<unsigned __int8>(-51);
  }
  else if (!applyGuild || !applyGuild->GetApplierFromSerial(this->m_dwObjSerial))
  {
    result = static_cast<unsigned __int8>(-52);
  }

  if (!result)
  {
    applyGuild->PopApplier(this->m_dwObjSerial, 2u);
    this->m_Param.m_pApplyGuild = nullptr;
  }

  this->SendMsg_GuildJoinApplyCancelResult(static_cast<char>(result));
}

void CPlayer::pc_GuildJoinAcceptRequest(unsigned int dwApplierSerial, bool bAccept)
{
  unsigned __int8 result = 0;
  CGuild *guild = this->m_Param.m_pGuild;
  _guild_applier_info *applierInfo = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(this->m_id.wIndex) == 99)
  {
    result = 106;
  }
  else if (!guild)
  {
    result = static_cast<unsigned __int8>(-54);
  }
  else
  {
    applierInfo = guild->GetApplierFromSerial(dwApplierSerial);
    if (!applierInfo)
    {
      result = static_cast<unsigned __int8>(-52);
    }
    else if (!applierInfo->pPlayer)
    {
      result = 26;
    }
    else if (this->m_Param.m_pGuildMemPtr->byClassInGuild != 1
      && this->m_Param.m_pGuildMemPtr->byClassInGuild != 2)
    {
      result = static_cast<unsigned __int8>(-53);
    }
    else if (static_cast<int>(guild->GetMemberNum()) + guild->m_nTempMemberNum >= 50)
    {
      result = static_cast<unsigned __int8>(-50);
    }
    else if (applierInfo->pPlayer->m_Param.GetRaceCode() != static_cast<int>(guild->m_byRace))
    {
      result = 70;
    }
    else if (guild->m_bRankWait)
    {
      result = 71;
    }
  }

  if (!result)
  {
    if (bAccept)
    {
      g_Main.m_logGuild.Write(
        "[%s] Join accept: approver=%s applicant=%s",
        guild->m_aszName,
        this->m_Param.GetCharNameA(),
        applierInfo->pPlayer->m_Param.GetCharNameA());

      _qry_case_joinacguild_local query{};
      query.in_guildindex = guild->m_nIndex;
      query.in_guildserial = guild->m_dwSerial;
      query.in_applierindex = applierInfo->pPlayer->m_ObjID.m_wIndex;
      query.in_applierserial = applierInfo->pPlayer->m_dwObjSerial;
      query.in_accepterserial = this->m_dwObjSerial;
      query.in_Grade = 0;
      query.in_MemberNum = static_cast<int>(guild->GetMemberNum()) + 1;

      if (g_Main.PushDQSData(0xFFFFFFFFu, &this->m_id, 0x10u, reinterpret_cast<char *>(&query), static_cast<int>(query.size())))
      {
        applierInfo->pPlayer->m_Param.m_bGuildLock = true;
        ++guild->m_nTempMemberNum;
      }
      else
      {
        result = static_cast<unsigned __int8>(-1);
      }
    }
    else
    {
      applierInfo->pPlayer->SendMsg_GuildJoinApplyRejectInform();
      applierInfo->pPlayer->m_Param.m_pApplyGuild = nullptr;
      guild->PopApplier(dwApplierSerial, 1u);
    }
  }

  if (result)
  {
    this->SendMsg_GuildJoinAcceptFail(static_cast<char>(result), dwApplierSerial);
  }
}

void CPlayer::pc_GuildPushMoneyRequest(unsigned int dwPushDalant, unsigned int dwPushGold)
{
  unsigned __int8 result = 0;
  CGuild *guild = this->m_Param.m_pGuild;

  if (!guild || !guild->GetMemberFromSerial(this->m_dwObjSerial))
  {
    result = static_cast<unsigned __int8>(-54);
  }
  else if (guild->m_bIOWait)
  {
    result = 103;
  }
  else if (dwPushDalant > 0xB2D05E00u || dwPushGold > 0xB2D05E00u)
  {
    result = 101;
  }
  else if (this->m_Param.GetDalant() < dwPushDalant || this->m_Param.GetGold() < dwPushGold)
  {
    result = 102;
  }
  else
  {
    const double totalDalant = static_cast<double>(guild->GetTotalDalant()) + static_cast<double>(dwPushDalant);
    const double totalGold = static_cast<double>(guild->GetTotalGold()) + static_cast<double>(dwPushGold);
    if (totalDalant > 1000000000.0 || totalGold > 500000.0)
    {
      result = 104;
    }
  }

  if (!result)
  {
    _qry_case_inputgmoney_local query{};
    query.in_pusherserial = this->m_id.dwSerial;
    query.tmp_guildindex = guild->m_nIndex;
    query.in_guildserial = guild->m_dwSerial;
    query.dwAddGold = dwPushGold;
    query.dwAddDalant = dwPushDalant;
    query.in_date[0] = GetCurrentMonth();
    query.in_date[1] = GetCurrentDay();
    query.in_date[2] = GetCurrentHour();
    query.in_date[3] = GetCurrentMin();
    strcpy_0(query.in_w_pushername, this->m_Param.GetCharNameW());

    if (g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x13u, reinterpret_cast<char *>(&query), static_cast<int>(query.size())))
    {
      guild->m_bIOWait = true;
      this->SubDalant(dwPushDalant);
      this->SubGold(dwPushGold);
      this->SendMsg_AlterMoneyInform(0);
    }
    else
    {
      result = static_cast<unsigned __int8>(-1);
    }
  }

  this->SendMsg_GuildPushMoneyResult(static_cast<char>(result));
}

void CPlayer::pc_GuildRoomRentRequest(_guildroom_rent_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 0;
  const unsigned __int8 roomType = pProtocol ? pProtocol->byRoomType : 0;

  if (roomType >= 2u)
  {
    retCode = 7;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    retCode = 6;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  if (this->m_Param.m_byClassInGuild != 1 && this->m_Param.m_byClassInGuild != 2)
  {
    retCode = 2;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (guild->m_bIOWait)
  {
    retCode = 11;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (static_cast<long double>(CGuildRoomInfo::sm_RoomInfo[roomType].dwCost) > guild->GetTotalDalant())
  {
    retCode = 8;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  if (guild->GetGrade() < CGuildRoomInfo::sm_RoomInfo[roomType].byRestrict)
  {
    retCode = 9;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (roomSystem->IsRoomRented(pProtocol->dwGuildSerial))
  {
    retCode = 3;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  const unsigned __int8 raceCode = static_cast<unsigned __int8>(this->m_pUserDB->m_AvatorData.dbAvator.m_byRaceSexCode >> 1);
  subRetCode = static_cast<unsigned __int8>(
    roomSystem->RentRoom(raceCode, roomType, guild->m_nIndex, guild->m_dwSerial, nullptr, false));
  if (subRetCode)
  {
    retCode = 5;
    this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
    return;
  }

  retCode = 10;

  _qry_case_outputgmoney query{};
  query.in_poperserial = 0;
  query.tmp_guildindex = guild->m_nIndex;
  query.in_guildserial = guild->m_dwSerial;
  query.dwSubDalant = CGuildRoomInfo::sm_RoomInfo[roomType].dwCost;
  query.dwSubGold = 0;
  std::memset(&query.out_totalgold, 0, 17);
  query.in_date[0] = static_cast<unsigned __int8>(GetCurrentMonth());
  query.in_date[1] = static_cast<unsigned __int8>(GetCurrentDay());
  query.in_date[2] = static_cast<unsigned __int8>(GetCurrentHour());
  query.in_date[3] = static_cast<unsigned __int8>(GetCurrentMin());
  strcpy_0(query.in_w_popername, "GuildRoom Rent");
  g_Main.PushDQSData(
    0xFFFFFFFFu,
    nullptr,
    0x14u,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));

  CMapData *roomMap = roomSystem->GetMapData(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()), roomType);
  if (roomMap)
  {
    CMapItemStoreList *destStoreList = CItemStoreManager::Instance()->GetMapItemStoreListBySerial(roomMap->m_nMapIndex);
    CMapItemStoreList *emptyInstanceStore = CItemStoreManager::Instance()->GetEmptyInstanceItemStore();
    if (destStoreList && emptyInstanceStore && emptyInstanceStore->CopyItemStoreData(destStoreList))
    {
      emptyInstanceStore->SetTypeNSerial(1u, static_cast<int>(guild->m_dwSerial));
    }
  }

  this->SendMsg_GuildRoomRentResult(retCode, subRetCode, roomType);
  guild->SendMsg_GuildRoomRented(static_cast<char>(roomType));
}

void CPlayer::pc_GuildRoomEnterRequest(_guildroom_enter_request_clzo *pProtocol)
{
  unsigned __int8 retCode = 0;
  unsigned __int8 subRetCode = 3;
  unsigned __int8 mapIndex = 0;
  unsigned __int16 mapLayer = 0;
  float startPos[3] = {0.0f, 0.0f, 0.0f};
  int restTime[5]{};

  if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    retCode = 5;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (this->IsRidingUnit())
  {
    retCode = 6;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (this->m_byStandType == 1)
  {
    retCode = 7;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    retCode = 1;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (!roomSystem->IsRoomRented(guild->m_dwSerial))
  {
    retCode = 2;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  subRetCode = static_cast<unsigned __int8>(
    roomSystem->RoomIn(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial));
  if (subRetCode)
  {
    retCode = 3;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CMapData *mapDummy = nullptr;
  unsigned __int8 roomType = 2;
  if (!roomSystem->GetMapPos(guild->m_dwSerial, startPos, mapDummy, &mapLayer, &roomType))
  {
    roomSystem->RoomOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial);
    retCode = 4;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  CMapData *roomMap = roomSystem->GetMapData(static_cast<unsigned __int8>(this->m_Param.GetRaceCode()), roomType);
  if (!roomMap)
  {
    roomSystem->RoomOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial);
    retCode = 4;
    this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
    return;
  }

  retCode = 10;
  this->OutOfMap(roomMap, mapLayer, 3u, startPos);
  mapIndex = static_cast<unsigned __int8>(roomMap->m_pMapSet->m_dwIndex);
  roomSystem->GetRestTime(guild->m_dwSerial, restTime);
  this->SendMsg_GuildRoomEnterResult(retCode, subRetCode, mapIndex, mapLayer, startPos, restTime[0]);
}

void CPlayer::pc_GuildRoomOutRequest(_guildroom_out_request_clzo *pProtocol)
{
  if (this->GetCurSecNum() == static_cast<unsigned int>(-1) || this->m_bMapLoading)
  {
    this->SendMsg_GuildRoomOutResult(4u, 0, 0, nullptr);
    return;
  }

  if (this->IsRidingUnit())
  {
    this->SendMsg_GuildRoomOutResult(5u, 0, 0, nullptr);
    return;
  }

  if (this->m_byStandType == 1)
  {
    this->SendMsg_GuildRoomOutResult(6u, 0, 0, nullptr);
    return;
  }

  if (!this->m_Param.m_pGuild || !pProtocol || pProtocol->dwGuildSerial != this->m_Param.m_pGuild->m_dwSerial)
  {
    this->SendMsg_GuildRoomOutResult(1u, 0, 0, nullptr);
    return;
  }

  CGuild *guild = this->m_Param.m_pGuild;
  CGuildRoomSystem *roomSystem = CGuildRoomSystem::GetInstance();
  if (!roomSystem->IsRoomRented(guild->m_dwSerial)
    || !roomSystem->SetPlayerOut(guild->m_dwSerial, this->m_ObjID.m_wIndex, this->m_pUserDB->m_dwSerial))
  {
    this->SendMsg_GuildRoomOutResult(2u, 0, 0, nullptr);
  }
}

void CPlayer::pc_GuildSetHonorRequest(_guild_honor_set_request_clzo *pData)
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial =
    CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);

  if (charSerial == raceBossSerial)
  {
    const unsigned __int8 result =
      CHonorGuild::Instance()->SetNextHonorGuild(static_cast<unsigned __int8>(raceCode), pData);
    this->SendMsg_GuildSetHonorResult(static_cast<char>(result));
  }
  else
  {
    this->SendMsg_GuildSetHonorResult(1u);
  }
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

void CPlayer::pc_GuildHonorListRequest(unsigned __int8 byUI)
{
  const int raceCode = this->m_Param.GetRaceCode();
  CHonorGuild *honorGuild = CHonorGuild::Instance();
  if (raceCode < 0 || raceCode >= 3)
  {
    return;
  }

  _guild_honor_list_result_zocl *currentList = honorGuild->m_pCurrHonorGuild[raceCode];
  if (!currentList)
  {
    return;
  }

  currentList->byUI = byUI;
  unsigned __int8 type[2] = {27, 112};
  const unsigned __int16 len = static_cast<unsigned __int16>(currentList->size());
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(currentList), len);
}

void CPlayer::pc_AnimusCommandRequest(unsigned __int8 byCommandCode)
{
  if (!this->m_pRecalledAnimusItem || !this->m_pRecalledAnimusChar)
  {
    return;
  }

  if (this->m_pRecalledAnimusChar->m_dwAIMode != byCommandCode)
  {
    this->m_pRecalledAnimusChar->m_dwAIMode = byCommandCode;
  }
}

void CPlayer::pc_AnimusTargetRequest(
  unsigned __int8 byObjectID,
  unsigned __int16 wObjectIndex,
  unsigned int dwObjectSerial)
{
unsigned __int8 result = 0;
  CCharacter *target = reinterpret_cast<CCharacter *>(g_Main.GetObjectA(0, byObjectID, wObjectIndex));
  if (!this->m_pRecalledAnimusItem || !this->m_pRecalledAnimusChar)
  {
    result = 7;
  }
  else if (!target || !target->m_bLive || target->m_bCorpse)
  {
    result = 8;
  }
  else if (GetSqrt(this->m_pRecalledAnimusChar->m_fCurPos, target->m_fCurPos) > 400.0f)
  {
    result = 8;
  }

  if (!result)
  {
    this->m_pRecalledAnimusChar->m_pTarget = target;
    this->m_pRecalledAnimusChar->m_pNextTarget = target;
    this->m_pRecalledAnimusChar->MasterAttack_MasterInform(target);
  }

  this->SendMsg_AnimusTargetResult(static_cast<char>(result));
}

void CPlayer::pc_GuildQueryInfoRequest(unsigned int dwGuildSerial)
{
  for (int guildIndex = 0; guildIndex < MAX_GUILD; ++guildIndex)
  {
    CGuild &guild = g_Guild[guildIndex];
    if (!guild.IsFill() || guild.m_dwSerial != dwGuildSerial)
    {
      continue;
    }

    guild.MakeQueryInfoPacket();
    unsigned __int8 type[2] = {27, 34};
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      type,
      reinterpret_cast<char *>(guild.m_QueryPacket_Info),
      0x2Bu);
    return;
  }
}

void CPlayer::pc_GuildListRequest(unsigned __int8 byPage)
{

  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
    static_cast<unsigned __int8>(raceCode),
    0);

  if (charSerial != raceBossSerial)
  {
    return;
  }

  if (raceCode < 0 || raceCode >= 3 || byPage >= 75u)
  {
    return;
  }

  CGuildList &guildList = CGuild::s_GuildList;
  if (!guildList.m_pGuildList[raceCode])
  {
    return;
  }

  unsigned __int8 resolvedPage = byPage;
  if (resolvedPage > guildList.m_byMaxPage[raceCode])
  {
    resolvedPage = guildList.m_byMaxPage[raceCode];
  }

  _guild_list_result_zocl result{};
  result.byMaxPage = static_cast<char>(guildList.m_byMaxPage[raceCode] + 1);
  result.byPage = static_cast<char>(resolvedPage);
  result.byListCnt = static_cast<char>(guildList.m_pGuildList[raceCode][resolvedPage].byListCnt);
  std::memcpy(
    result.GuildList,
    guildList.m_pGuildList[raceCode][resolvedPage].GuildList,
    sizeof(result.GuildList));

  unsigned __int8 type[2] = {27, 116};
  unsigned __int8 listCount = static_cast<unsigned __int8>(result.byListCnt);
  if (listCount > 4u)
  {
    listCount = 0;
  }
  const unsigned __int16 len = static_cast<unsigned __int16>(143 - 35 * (4 - listCount));
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&result),
    len);
}

void CPlayer::pc_GuildNextHonorListRequest()
{
  const unsigned int charSerial = this->m_Param.GetCharSerial();
  const int raceCode = this->m_Param.GetRaceCode();
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(
    static_cast<unsigned __int8>(raceCode),
    0);

  if (charSerial != raceBossSerial || raceCode < 0 || raceCode >= 3)
  {
    return;
  }

  CHonorGuild *honorGuild = CHonorGuild::Instance();
  _guild_honor_list_result_zocl *listToSend = honorGuild->m_bNext[raceCode]
    ? honorGuild->m_pNextHonorGuild[raceCode]
    : honorGuild->m_pCurrHonorGuild[raceCode];

  if (!listToSend)
  {
    return;
  }

  unsigned __int8 type[2] = {27, 119};
  const unsigned __int16 len = static_cast<unsigned __int16>(listToSend->size());
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(listToSend), len);
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

void CPlayer::pc_UnitReturnRequest()
{
  unsigned __int8 result = 0;
  bool chargePullFee = true;

  if (this->m_Param.GetRaceCode())
  {
    result = 1;
  }
  else if (this->m_pParkingUnit)
  {
    if (this->m_pParkingUnit->m_dwOwnerSerial == this->m_dwObjSerial)
    {
      if (GetSqrt(this->m_fCurPos, this->m_pParkingUnit->m_fCurPos) <= 540.0f)
      {
        if (IsBeNearStore(this, 4))
        {
          chargePullFee = false;
        }
      }
      else
      {
        result = 20;
      }
    }
    else
    {
      result = 2;
    }
  }
  else
  {
    result = 2;
  }

  unsigned int pullFee = 0;
  if (!result)
  {
    this->m_pParkingUnit->Destroy(0);
    if (chargePullFee)
    {
      pullFee = 1000;
    }
    this->_UpdateUnitDebt(this->m_pUsingUnit->bySlotIndex, pullFee);
    this->_LockUnitKey(this->m_pUsingUnit->bySlotIndex, false);
    this->m_pUsingUnit = nullptr;
    this->m_pParkingUnit = nullptr;
  }

  char msg[9] = {};
  msg[0] = static_cast<char>(result);
  memcpy_0(msg + 1, &pullFee, sizeof(pullFee));
  const unsigned int dalant = this->m_Param.GetDalant();
  memcpy_0(msg + 5, &dalant, sizeof(dalant));
  unsigned __int8 type[2] = {23, 16};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 9u);
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

void CPlayer::pc_UnitBulletReplaceRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byPackIndex,
  unsigned __int8 byBulletPart)
{
  if (!this->m_pUserDB)
  {
    return;
  }

  unsigned __int8 result = 0;
  _UNIT_DB_BASE::_LIST *unitData = &this->m_Param.m_UnitDB.m_List[bySlotIndex];
  if (unitData->byFrame == 0xFF)
  {
    result = 5;
  }
  else if (unitData->dwSpare[byPackIndex] == 0xFFFFFFFFu)
  {
    result = 25;
  }

  if (!result)
  {
    unitData->dwBullet[byBulletPart] = unitData->dwSpare[byPackIndex];
    unitData->dwSpare[byPackIndex] = 0xFFFFFFFFu;
    this->m_pUserDB->Update_UnitData(bySlotIndex, unitData);
  }

  char msg[1] = {static_cast<char>(result)};
  unsigned __int8 type[2] = {23, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 1u);
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

void CPlayer::pc_UnitFrameBuyRequest(unsigned __int8 byFrameCode, int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  unsigned __int8 emptyUnitSlot = static_cast<unsigned __int8>(-1);
  unsigned int consumeMoney[12] = {};
  unsigned __int8 defaultParts[6];
  std::memset(defaultParts, 0xFF, sizeof(defaultParts));

  _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(byFrameCode));
  _UnitKeyItem_fld *unitKeyRecord = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else
  {
    const unsigned int taxRate = eGetTexRate(0) + 10000;
    if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
    {
      resultCode = 21;
    }
    else if (!frameRecord)
    {
      resultCode = 9;
    }
    else if (m_Param.GetRaceCode() != 0)
    {
      resultCode = 1;
    }
    else if (m_pUsingUnit)
    {
      resultCode = 2;
    }
    else
    {
      for (int slotIndex = 0; slotIndex < 4; ++slotIndex)
      {
        if (m_Param.m_UnitDB.m_List[slotIndex].byFrame == 0xFF)
        {
          emptyUnitSlot = static_cast<unsigned __int8>(slotIndex);
          break;
        }
      }

      if (emptyUnitSlot == 0xFF)
      {
        resultCode = 3;
      }
      else if (!m_Param.m_dbInven.GetNumEmptyCon())
      {
        resultCode = 4;
      }
      else
      {
        unitKeyRecord = GetUnitKeyMatchFrame(byFrameCode);
        if (!unitKeyRecord)
        {
          resultCode = 6;
        }
        else
        {
          const int moneyCode = frameRecord->m_nMoney;
          if (moneyCode < 0 || moneyCode >= 7)
          {
            resultCode = 9;
          }
          else
          {
            const unsigned long long rawCost =
              static_cast<unsigned long long>(frameRecord->m_nStdPrice) * static_cast<unsigned long long>(taxRate);
            consumeMoney[moneyCode] = static_cast<unsigned int>(rawCost / 10000u);
            if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
            {
              resultCode = 7;
            }
            else
            {
              const char *defaultPartCodes[6] = {
                frameRecord->m_strDFHead,
                frameRecord->m_strDFUpper,
                frameRecord->m_strDFLower,
                frameRecord->m_strDFArms,
                frameRecord->m_strDFShoulder,
                frameRecord->m_strDFBack,
              };

              for (int partCode = 0; partCode < 6; ++partCode)
              {
                if (strncmp(defaultPartCodes[partCode], "-1", 2u) != 0)
                {
                  _base_fld *partRecord =
                    g_Main.m_tblUnitPart[partCode].GetRecord(const_cast<char *>(defaultPartCodes[partCode]));
                  if (!partRecord)
                  {
                    resultCode = 100;
                    break;
                  }
                  defaultParts[partCode] = static_cast<unsigned __int8>(partRecord->m_dwIndex);
                }
              }
            }
          }
        }
      }
    }
  }

  unsigned __int16 keySerial = static_cast<unsigned __int16>(-1);
  unsigned __int16 keyIndex = static_cast<unsigned __int16>(-1);

  if (!resultCode)
  {
    _UNIT_DB_BASE::_LIST *slotData = &m_Param.m_UnitDB.m_List[emptyUnitSlot];
    slotData->DelUnit();
    slotData->byFrame = byFrameCode;
    std::memcpy(slotData->byPart, defaultParts, sizeof(slotData->byPart));
    slotData->dwGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
    m_pUserDB->Update_UnitData(emptyUnitSlot, slotData);

    keySerial = m_Param.GetNewItemSerial();
    keyIndex = static_cast<unsigned __int16>(unitKeyRecord->m_dwIndex);

    _STORAGE_LIST::_storage_con keyItem;
    keyItem.m_byTableCode = 19;
    keyItem.m_wItemIndex = keyIndex;
    keyItem.m_dwDur = 0;
    keyItem.m_dwLv = emptyUnitSlot;
    keyItem.m_wSerial = keySerial;

    if (!Emb_AddStorage(0, &keyItem, false, true))
    {
      this->SendMsg_UnitFrameBuyResult(static_cast<char>(0xFF), static_cast<char>(byFrameCode), static_cast<char>(emptyUnitSlot), keyIndex, keySerial, consumeMoney);
      return;
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);

    if (!m_byUserDgr)
    {
      if (consumeMoney[1])
      {
        eAddGold(0, consumeMoney[1]);
      }
      if (consumeMoney[0])
      {
        eAddDalant(0, consumeMoney[0]);
      }
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1])
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0])
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitFrameBuyResult(static_cast<char>(resultCode), static_cast<char>(byFrameCode), static_cast<char>(emptyUnitSlot), keyIndex, keySerial, consumeMoney);
}

void CPlayer::pc_UnitSellRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));

  unsigned int totalDebt = 0;
  unsigned int sellMoney = 0;
  int dalantDelta = 0;

  const unsigned int sellRate = 10000u - eGetTexRate(0);
  const unsigned int debtRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (!frameRecord)
  {
    resultCode = 9;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit)
  {
    resultCode = 2;
  }
  else if (frameCode == 0xFF)
  {
    resultCode = 5;
  }
  else
  {
    const unsigned long long rawDebt =
      static_cast<unsigned long long>(unitData->nPullingFee) * static_cast<unsigned long long>(debtRate);
    totalDebt = static_cast<unsigned int>(rawDebt / 10000u);

    if (totalDebt > m_Param.GetDalant())
    {
      resultCode = 10;
    }
    else
    {
      const float gaugeRate =
        static_cast<float>(unitData->dwGauge) / static_cast<float>(frameRecord->m_nUnit_HP);
      const double baseSell =
        static_cast<double>(frameRecord->m_nStdPrice) * static_cast<double>(gaugeRate) / 2.0;
      const unsigned long long roundedSell =
        static_cast<unsigned long long>(sellRate * static_cast<unsigned long long>(static_cast<unsigned int>(baseSell)) + 5000u);
      sellMoney = static_cast<unsigned int>(roundedSell / 10000u);

      dalantDelta = static_cast<int>(sellMoney) - static_cast<int>(totalDebt);
      if (dalantDelta > 0)
      {
        if (!CanAddMoneyForMaxLimMoney(static_cast<unsigned int>(dalantDelta), m_Param.GetDalant()))
        {
          resultCode = 30;
        }
      }
      else if (dalantDelta < 0)
      {
        if (static_cast<unsigned int>(-dalantDelta) > m_Param.GetDalant())
        {
          resultCode = 10;
        }
      }
    }
  }

  unsigned __int16 removedKeySerial = static_cast<unsigned __int16>(-1);
  if (!resultCode)
  {
    unitData->byFrame = static_cast<unsigned __int8>(-1);
    removedKeySerial = DeleteUnitKey(this, bySlotIndex);

    const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
    dalantDelta = static_cast<int>(static_cast<double>(dalantDelta) * playerPenalty);

    AlterDalant(static_cast<double>(dalantDelta));
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddDalant(0, dalantDelta);
    }
  }

  this->SendMsg_UnitSellResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), removedKeySerial, static_cast<int>(sellMoney), totalDebt, m_Param.GetDalant(), m_Param.GetGold());
}

void CPlayer::pc_UnitPartTuningRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byTuningNum,
  _tuning_data *pTuningData,
  int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  static constexpr int kBulletSlotPerPart[6] = {-1, -1, -1, 0, 1, -1};

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  int consumeMoney[7] = {};

  const unsigned int buyTaxRate = eGetTexRate(0) + 10000u;
  const unsigned int sellTaxRate = 10000u - eGetTexRate(0);

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit)
  {
    resultCode = 2;
  }
  else if (frameCode == 0xFF)
  {
    resultCode = 5;
  }
  else
  {
    _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));
    if (!frameRecord)
    {
      resultCode = 5;
    }
    else if (!frameRecord->m_bRepair)
    {
      resultCode = 38;
    }
    else if (!unitData->dwGauge)
    {
      resultCode = 36;
    }
    else
    {
      for (int tuningIndex = 0; tuningIndex < byTuningNum; ++tuningIndex)
      {
        const unsigned __int8 partCode = pTuningData[tuningIndex].byPartCode;
        const unsigned __int8 partIndex = pTuningData[tuningIndex].byPartIndex;

        _UnitPart_fld *newPartRecord =
          reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[partCode].GetRecord(partIndex));
        if (!newPartRecord)
        {
          resultCode = 8;
          break;
        }

        if (newPartRecord->m_strDefFrame[frameCode] == '0')
        {
          resultCode = 6;
          break;
        }

        if (unitData->byPart[partCode] == partIndex)
        {
          resultCode = 29;
          break;
        }

        const int level = m_Param.GetLevel();
        if (newPartRecord->m_nLevelLim > level)
        {
          resultCode = 12;
          break;
        }
        if (newPartRecord->m_nUpLevelLim != -1 && newPartRecord->m_nUpLevelLim < level)
        {
          resultCode = 12;
          break;
        }

        const int masteryCodeList[2] = {newPartRecord->m_nExpertID1, newPartRecord->m_nExpertID2};
        const int masteryLimitList[2] = {newPartRecord->m_nExpertLim1, newPartRecord->m_nExpertLim2};
        for (int masterySlot = 0; masterySlot < 2; ++masterySlot)
        {
          const int masteryCode = masteryCodeList[masterySlot];
          if (masteryCode != -1)
          {
            if (masteryCode >= 6)
            {
              resultCode = 13;
              break;
            }
            if (m_pmMst.GetEquipMastery(static_cast<unsigned __int8>(masteryCode))
                < masteryLimitList[masterySlot])
            {
              resultCode = 13;
              break;
            }
          }
        }
        if (resultCode)
        {
          break;
        }

        if (newPartRecord->m_nMoney < 0 || newPartRecord->m_nMoney >= 7)
        {
          resultCode = 8;
          break;
        }

        const unsigned long long partCostRaw =
          static_cast<unsigned long long>(newPartRecord->m_nStdPrice)
          * static_cast<unsigned long long>(buyTaxRate);
        consumeMoney[newPartRecord->m_nMoney] += static_cast<int>(partCostRaw / 10000u);

        if (partCode == 5)
        {
          for (int spareSlot = 0; spareSlot < 8; ++spareSlot)
          {
            if (unitData->dwSpare[spareSlot] != 0xFFFFFFFFu)
            {
              _UnitBullet_fld *spareBullet =
                reinterpret_cast<_UnitBullet_fld *>(
                  g_Main.m_tblUnitBullet.GetRecord(static_cast<unsigned __int16>(unitData->dwSpare[spareSlot])));
              if (spareBullet)
              {
                const int spareMoneyCode = spareBullet->m_nMoney;
                if (spareMoneyCode >= 0 && spareMoneyCode < 7)
                {
                  const unsigned long long spareRefundRaw =
                    static_cast<unsigned long long>(sellTaxRate)
                    * static_cast<unsigned long long>(spareBullet->m_nStdPrice / 2)
                    + 5000u;
                  consumeMoney[spareMoneyCode] -= static_cast<int>(spareRefundRaw / 10000u);
                }
              }
            }
          }
        }
      }

      if (!resultCode)
      {
        for (int moneyCode = 0; moneyCode < 7; ++moneyCode)
        {
          if (consumeMoney[moneyCode] > static_cast<int>(GetMoney(static_cast<unsigned __int8>(moneyCode))))
          {
            resultCode = 7;
            break;
          }
        }
      }
    }
  }

  if (!resultCode)
  {
    for (int tuningIndex = 0; tuningIndex < byTuningNum; ++tuningIndex)
    {
      const unsigned __int8 partCode = pTuningData[tuningIndex].byPartCode;
      const unsigned __int8 partIndex = pTuningData[tuningIndex].byPartIndex;

      unitData->byPart[partCode] = partIndex;
      if (partCode == 5)
      {
        std::memset(unitData->dwSpare, 0xFF, sizeof(unitData->dwSpare));
      }

      if (partCode == 3 || partCode == 4)
      {
        const int bulletSlot = kBulletSlotPerPart[partCode];
        if (bulletSlot != -1)
        {
          unsigned int &bulletData = unitData->dwBullet[bulletSlot];
          if (HIWORD(bulletData) && HIWORD(bulletData) != 0xFFFF)
          {
            _base_fld *bulletRecord = g_Main.m_tblUnitBullet.GetRecord(static_cast<unsigned __int16>(bulletData));
            _base_fld *partRecord = g_Main.m_tblUnitPart[partCode].GetRecord(partIndex);
            if (bulletRecord
                && partRecord
                && *reinterpret_cast<int *>(&bulletRecord[2].m_strCode[60])
                     != *reinterpret_cast<int *>(&partRecord[3].m_strCode[60]))
            {
              bulletData = 0xFFFFFFFFu;
            }
          }
        }
      }
    }

    SubDalant(static_cast<unsigned int>(consumeMoney[0]));
    SubGold(static_cast<unsigned int>(consumeMoney[1]));
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddGold(0, consumeMoney[1]);
      eAddDalant(0, consumeMoney[0]);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1] > 0)
      {
        const unsigned int amount = 2000u * static_cast<unsigned int>(consumeMoney[1]);
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0] > 0)
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, static_cast<unsigned int>(consumeMoney[0]));
      }
    }
  }

  this->SendMsg_UnitPartTuningResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), consumeMoney);
}

void CPlayer::pc_UnitFrameRepairRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;
  _UnitFrame_fld *frameRecord = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));

  unsigned int consumeDalant = 0;
  unsigned int newGauge = 0;
  const unsigned int taxRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else
  {
    _UnitFrame_fld *repairableFrame = reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(frameCode));
    if (!repairableFrame)
    {
      resultCode = 5;
    }
    else if (!repairableFrame->m_bRepair)
    {
      resultCode = 37;
    }
    else if (m_Param.GetRaceCode() != 0)
    {
      resultCode = 1;
    }
    else if (m_pUsingUnit)
    {
      resultCode = 2;
    }
    else if (!frameRecord)
    {
      resultCode = 9;
    }
    else if (frameCode == 0xFF)
    {
      resultCode = 5;
    }
    else
    {
      newGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
      const int gaugeDiff = static_cast<int>(newGauge) - static_cast<int>(unitData->dwGauge);
      if (gaugeDiff <= 0)
      {
        resultCode = 14;
      }
      else
      {
        if (frameRecord->m_nRepPrice > 0)
        {
          consumeDalant = static_cast<unsigned int>(frameRecord->m_nRepPrice);
        }

        int brokenBonus = 0;
        for (int partCode = 0; partCode < 6; ++partCode)
        {
          _UnitPart_fld *partRecord =
            reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[partCode].GetRecord(unitData->byPart[partCode]));
          if (!partRecord)
          {
            continue;
          }

          if (partRecord->m_nRepPrice > 0)
          {
            consumeDalant += static_cast<unsigned int>(partRecord->m_nRepPrice);
          }
          if (!unitData->dwGauge)
          {
            brokenBonus += partRecord->m_nDesrepPrice;
          }
        }

        const float scaledCost =
          static_cast<float>(consumeDalant)
          * static_cast<float>(gaugeDiff)
          * m_fUnitPv_RepPr;
        const unsigned long long taxedCost =
          static_cast<unsigned long long>(taxRate) * static_cast<unsigned long long>(static_cast<unsigned int>(scaledCost));
        consumeDalant = static_cast<unsigned int>(brokenBonus) + static_cast<unsigned int>(taxedCost / 10000u);

        if (consumeDalant > m_Param.GetDalant())
        {
          resultCode = 7;
        }
      }
    }
  }

  if (!resultCode)
  {
    unitData->dwGauge = newGauge;
    SubDalant(consumeDalant);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddDalant(0, consumeDalant);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateUnitRepairingChargesData(level, consumeDalant);
    }
  }

  this->SendMsg_UnitFrameRepairResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), newGauge, consumeDalant);
}

void CPlayer::pc_UnitBulletFillRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int16 *pwBulletIndex,
  int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  static constexpr unsigned __int8 kWeaponPartPerBullet[2] = {3, 4};

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;

  _UnitBullet_fld *bulletRecords[2] = {
    reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(pwBulletIndex[0])),
    reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(pwBulletIndex[1])),
  };

  unsigned int consumeMoney[11] = {};
  const unsigned int taxRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (m_pUsingUnit)
  {
    resultCode = 2;
  }
  else if (frameCode == 0xFF)
  {
    resultCode = 5;
  }
  else
  {
    for (int bulletSlot = 0; bulletSlot < 2; ++bulletSlot)
    {
      _UnitBullet_fld *bulletRecord = bulletRecords[bulletSlot];
      if (!bulletRecord)
      {
        continue;
      }

      const unsigned __int8 partCode = kWeaponPartPerBullet[bulletSlot];
      _base_fld *partRecord = g_Main.m_tblUnitPart[partCode].GetRecord(unitData->byPart[partCode]);
      if (!partRecord)
      {
        resultCode = 26;
        break;
      }

      if (*reinterpret_cast<int *>(&partRecord[3].m_strCode[60]) != bulletRecord->m_nWPType)
      {
        resultCode = 26;
        break;
      }
    }

    if (!resultCode)
    {
      for (int bulletSlot = 0; bulletSlot < 2; ++bulletSlot)
      {
        _UnitBullet_fld *bulletRecord = bulletRecords[bulletSlot];
        if (bulletRecord)
        {
          const int moneyCode = bulletRecord->m_nMoney;
          if (moneyCode >= 0 && moneyCode < 7)
          {
            consumeMoney[moneyCode] += static_cast<unsigned int>(bulletRecord->m_nStdPrice);
          }
        }
      }

      for (int moneyCode = 0; moneyCode < 7; ++moneyCode)
      {
        const unsigned long long taxed =
          static_cast<unsigned long long>(consumeMoney[moneyCode]) * static_cast<unsigned long long>(taxRate);
        consumeMoney[moneyCode] = static_cast<unsigned int>(taxed / 10000u);
        if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
        {
          resultCode = 7;
          break;
        }
      }
    }
  }

  if (!resultCode)
  {
    for (int bulletSlot = 0; bulletSlot < 2; ++bulletSlot)
    {
      _UnitBullet_fld *bulletRecord = bulletRecords[bulletSlot];
      if (bulletRecord)
      {
        unsigned int &slotData = unitData->dwBullet[bulletSlot];
        reinterpret_cast<unsigned __int16 &>(slotData) = pwBulletIndex[bulletSlot];
        reinterpret_cast<unsigned __int16 *>(&slotData)[1] = static_cast<unsigned __int16>(bulletRecord->m_nDurUnit);
      }
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddGold(0, consumeMoney[1]);
      eAddDalant(0, consumeMoney[0]);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1])
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0])
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitBulletFillResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), pwBulletIndex, consumeMoney);
}

void CPlayer::pc_UnitPackFillRequest(
  unsigned __int8 bySlotIndex,
  unsigned __int8 byFillNum,
  _unit_pack_fill_request_clzo::__list *pList,
  int bUseNPCLinkIntem)
{
  if (!m_pUserDB)
  {
    return;
  }

  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const unsigned __int8 frameCode = unitData->byFrame;

  _UnitPart_fld *backPartRecord =
    reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[5].GetRecord(unitData->byPart[5]));

  _UnitBullet_fld *bulletPerSpare[8] = {};
  unsigned int consumeMoney[11] = {};
  const unsigned int taxRate = eGetTexRate(0) + 10000u;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 34;
  }
  else if (!bUseNPCLinkIntem && !IsBeNearStore(this, 4))
  {
    resultCode = 21;
  }
  else if (m_Param.GetRaceCode() != 0)
  {
    resultCode = 1;
  }
  else if (frameCode == 0xFF)
  {
    resultCode = 5;
  }
  else if (!backPartRecord)
  {
    resultCode = 17;
  }
  else
  {
    for (int fillIndex = 0; fillIndex < byFillNum; ++fillIndex)
    {
      if (pList[fillIndex].bySpareIndex >= backPartRecord->m_nBackSlt)
      {
        resultCode = 18;
        break;
      }

      _UnitBullet_fld *bulletRecord =
        reinterpret_cast<_UnitBullet_fld *>(g_Main.m_tblUnitBullet.GetRecord(pList[fillIndex].wBulletIndex));
      bulletPerSpare[pList[fillIndex].bySpareIndex] = bulletRecord;
      if (!bulletRecord)
      {
        resultCode = 18;
        break;
      }
    }

    if (!resultCode)
    {
      if (m_pUsingUnit)
      {
        resultCode = 2;
      }
      else
      {
        for (int spareSlot = 0; spareSlot < 8; ++spareSlot)
        {
          _UnitBullet_fld *bulletRecord = bulletPerSpare[spareSlot];
          if (bulletRecord)
          {
            const int moneyCode = bulletRecord->m_nMoney;
            if (moneyCode >= 0 && moneyCode < 7)
            {
              consumeMoney[moneyCode] += static_cast<unsigned int>(bulletRecord->m_nStdPrice);
            }
          }
        }

        for (int moneyCode = 0; moneyCode < 7; ++moneyCode)
        {
          const unsigned long long taxed =
            static_cast<unsigned long long>(consumeMoney[moneyCode]) * static_cast<unsigned long long>(taxRate);
          consumeMoney[moneyCode] = static_cast<unsigned int>(taxed / 10000u);
          if (consumeMoney[moneyCode] > GetMoney(static_cast<unsigned __int8>(moneyCode)))
          {
            resultCode = 7;
            break;
          }
        }
      }
    }
  }

  if (!resultCode)
  {
    for (int spareSlot = 0; spareSlot < 8; ++spareSlot)
    {
      _UnitBullet_fld *bulletRecord = bulletPerSpare[spareSlot];
      if (bulletRecord)
      {
        unsigned int &spareData = unitData->dwSpare[spareSlot];
        reinterpret_cast<unsigned __int16 &>(spareData) = static_cast<unsigned __int16>(bulletRecord->m_dwIndex);
        reinterpret_cast<unsigned __int16 *>(&spareData)[1] = static_cast<unsigned __int16>(bulletRecord->m_nDurUnit);
      }
    }

    SubDalant(consumeMoney[0]);
    SubGold(consumeMoney[1]);
    m_pUserDB->Update_UnitData(bySlotIndex, unitData);

    if (!m_byUserDgr)
    {
      eAddGold(0, consumeMoney[1]);
      eAddDalant(0, consumeMoney[0]);
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      if (consumeMoney[1])
      {
        const unsigned int amount = 2000u * consumeMoney[1];
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, amount);
      }
      if (consumeMoney[0])
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeMoney[0]);
      }
    }
  }

  this->SendMsg_UnitPackFillResult(resultCode, bySlotIndex, byFillNum, pList, consumeMoney);
}

void CPlayer::pc_UnitDeliveryRequest(
  unsigned __int8 bySlotIndex,
  CItemStore *pStore,
  bool bPayFee,
  float *pfNewPos,
  int bUseNPCLinkIntem)
{
  unsigned __int8 resultCode = 0;
  _UNIT_DB_BASE::_LIST *unitData = &m_Param.m_UnitDB.m_List[bySlotIndex];
  const int raceCode = m_Param.GetRaceCode();
  unsigned int consumeDalant = 0;
  CParkingUnit *parkingUnit = nullptr;
  unsigned __int8 transDistCode = 0;

  if (!unitData->dwGauge)
  {
    resultCode = 35;
  }
  else
  {
    const unsigned int taxRate = eGetTexRate(raceCode) + 10000u;

    if (m_bObserver)
    {
      resultCode = 27;
    }
    else if (raceCode != 0)
    {
      resultCode = 1;
    }
    else if (unitData->byFrame == 0xFF)
    {
      resultCode = 5;
    }
    else if (m_pCurMap->m_pMapSet->m_nMapType >= 2u)
    {
      resultCode = 27;
    }
    else if (m_pUsingUnit)
    {
      resultCode = 2;
    }
    else
    {
      if (pStore)
      {
        if (pStore->m_pRec->m_nStore_trade != 4)
        {
          resultCode = 23;
        }
        else if (!bUseNPCLinkIntem && GetSqrt(m_fCurPos, pStore->GetStorePos()) > 540.0f)
        {
          resultCode = 21;
        }
      }
      else
      {
        transDistCode = 1;
      }

      if (!resultCode && bPayFee)
      {
        consumeDalant = static_cast<unsigned int>(unitData->nPullingFee) + 10000u;
        if (transDistCode == 1)
        {
          consumeDalant += 1000u;
        }

        const unsigned long long taxed =
          static_cast<unsigned long long>(taxRate) * static_cast<unsigned long long>(consumeDalant);
        consumeDalant = static_cast<unsigned int>(taxed / 10000u);
        if (consumeDalant > m_Param.GetDalant())
        {
          resultCode = 11;
        }
      }

      if (!resultCode)
      {
        if (GetSqrt(m_fCurPos, pfNewPos) > 40.0f)
        {
          resultCode = 24;
        }
        else
        {
          parkingUnit = FindEmptyParkingUnitSlot();
          if (!parkingUnit)
          {
            resultCode = 19;
          }
        }
      }
    }
  }

  unsigned int parkingSerial = static_cast<unsigned int>(-1);
  if (!resultCode)
  {
    _object_create_setdata createData{};
    createData.m_pMap = m_pCurMap;
    createData.m_nLayerIndex = m_wMapLayerIndex;
    createData.m_pRecordSet = g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame);
    std::memcpy(createData.m_fStartPos, pfNewPos, sizeof(createData.m_fStartPos));

    if (!createData.m_pRecordSet || !parkingUnit->Create(&createData))
    {
      resultCode = 19;
    }
    else
    {
      parkingUnit->m_dwObjSerial = GenerateTransientObjSerial();
      parkingUnit->m_pOwner = this;
      parkingUnit->m_dwOwnerSerial = m_dwObjSerial;
      parkingUnit->m_byFrame = unitData->byFrame;
      std::memcpy(parkingUnit->m_byPartCode, unitData->byPart, sizeof(parkingUnit->m_byPartCode));
      parkingUnit->m_byCreateType = 0;
      parkingUnit->m_byTransDistCode = transDistCode;
      parkingUnit->m_dwParkingStartTime = GetLoopTime();

      unsigned int maxGauge = 10000u;
      _UnitFrame_fld *frameRecord =
        reinterpret_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(unitData->byFrame));
      if (frameRecord && frameRecord->m_nUnit_HP > 0)
      {
        maxGauge = static_cast<unsigned int>(frameRecord->m_nUnit_HP);
      }
      parkingUnit->m_wHPRate = static_cast<unsigned __int16>(10000u * unitData->dwGauge / maxGauge);

      parkingSerial = parkingUnit->m_dwObjSerial;
      _LockUnitKey(bySlotIndex, true);

      _UnitPart_fld *backPartRecord =
        reinterpret_cast<_UnitPart_fld *>(g_Main.m_tblUnitPart[5].GetRecord(unitData->byPart[5]));
      if (backPartRecord)
      {
        unitData->wBooster = static_cast<unsigned __int16>(backPartRecord->m_nBstCha);
      }

      m_pUsingUnit = unitData;
      m_pParkingUnit = parkingUnit;
      m_dwUnitViewOverTime = static_cast<unsigned int>(-1);

      SubDalant(consumeDalant);
      if (!m_byUserDgr)
      {
        eAddDalant(raceCode, consumeDalant);
      }

      const int level = m_Param.GetLevel();
      if (level == 30 || level == 40 || level == 50 || level == 60)
      {
        CMoneySupplyMgr::Instance()->UpdateBuyUnitData(level, consumeDalant);
      }

    }
  }

  this->SendMsg_UnitDeliveryResult(static_cast<char>(resultCode), static_cast<char>(bySlotIndex), parkingSerial, consumeDalant);
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

void CPlayer::pc_GuildCancelSuggestRequest(unsigned int dwMatterVoteSynKey)
{
unsigned __int8 resultCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    resultCode = static_cast<unsigned __int8>(-54);
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  else if (guild->m_dwSuggesterSerial != m_dwObjSerial)
  {
    resultCode = static_cast<unsigned __int8>(-53);
  }

  if (!resultCode)
  {
    guild->InitVote();
  }

  this->SendMsg_CancelSuggestResult(static_cast<char>(resultCode));
}

void CPlayer::pc_GuildVoteRequest(unsigned int dwMatterVoteSynKey, unsigned __int8 byVoteCode)
{
  unsigned __int8 resultCode = 0;
  CGuild *guild = m_Param.m_pGuild;
  if (!guild)
  {
    resultCode = static_cast<unsigned __int8>(-54);
  }
  else if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 106;
  }
  else if (!guild->m_bNowProcessSgtMter)
  {
    resultCode = 62;
  }
  else if (guild->m_SuggestedMatter.dwMatterVoteSynKey != dwMatterVoteSynKey)
  {
    resultCode = 63;
  }
  else if (!m_Param.m_pGuildMemPtr)
  {
    resultCode = static_cast<unsigned __int8>(-53);
  }
  else if (m_Param.m_pGuildMemPtr->bVote)
  {
    resultCode = 66;
  }

  if (!resultCode)
  {
    m_Param.m_pGuildMemPtr->bVote = true;
    if (byVoteCode < 2)
    {
      ++guild->m_SuggestedMatter.byVoteState[byVoteCode];
    }
  }

  this->SendMsg_VoteResult(dwMatterVoteSynKey, resultCode);
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

void CPlayer::pc_SkillRequest(unsigned __int8 bySkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  unsigned __int8 sfLevel = 1;
  bool hadStealth = GetStealth(true);

  _skill_fld *skillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[0].GetRecord(bySkillIndex));
  CCharacter *target = GetLiveTargetInSameMap(this, pidDst);
  unsigned __int16 delPoint[3]{};
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};

  if (!skillFld)
  {
    byErrCode = 1;
  }
  else if (!target)
  {
    byErrCode = 2;
  }
  else if (skillFld->m_nTempEffectType >= 150)
  {
    byErrCode = 21;
  }
  else if (skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
  {
    byErrCode = 8;
  }
  else if (!IsEffectableDst(skillFld->m_strActableDst, target))
  {
    byErrCode = 5;
  }
  else if (skillFld->m_nContEffectType != -1 && !target->IsRecvableContEffect())
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

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 0u, skillFld->m_dwIndex, skillFld->m_nMastIndex))
  {
    byErrCode = 9;
  }
  if (!byErrCode && !IsSFUsableGauge(0u, static_cast<unsigned __int16>(skillFld->m_dwIndex), delPoint))
  {
    byErrCode = 6;
  }
  if (!byErrCode && !IsSFUsableSFMastery(3u, skillFld->m_nMastIndex))
  {
    byErrCode = 3;
  }
  if (!byErrCode && !IsSFActableByClass(0u, reinterpret_cast<_base_fld *>(skillFld)))
  {
    byErrCode = 16;
  }
  if (!byErrCode && !GetUseConsumeItem(skillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode)
  {
    sfLevel = m_pmMst.GetSkillLv(static_cast<unsigned __int8>(skillFld->m_dwIndex));
    if (sfLevel == 0)
    {
      sfLevel = 1;
    }

    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistSkill(target, 0u, skillFld, sfLevel, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;

    if (success)
    {
      ApplyGaugeConsumeAndSendRecover(this, delPoint);
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(skillFld->m_fActDelay + addDelay));
    }
  }

  if ((byErrCode == 0 || byErrCode == 100) && hadStealth)
  {
    BreakStealth();
  }
  this->SendMsg_SkillResult(byErrCode, pidDst, bySkillIndex, sfLevel);
}

void CPlayer::pc_ClassSkillRequest(unsigned __int16 wSkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial)
{
  unsigned __int8 byErrCode = 0;
  bool hadStealth = GetStealth(true);

  _skill_fld *skillFld = reinterpret_cast<_skill_fld *>(g_Main.m_tblEffectData[2].GetRecord(wSkillIndex));
  CCharacter *target = GetLiveTargetInSameMap(this, pidDst);
  unsigned __int16 delPoint[3]{};
  _STORAGE_LIST::_db_con *consumeItems[3]{};
  int consumeCounts[3]{};
  bool consumeOverlap[3]{};

  if (!skillFld)
  {
    byErrCode = 1;
  }
  else if (!target)
  {
    byErrCode = 2;
  }
  else if (skillFld->m_nTempEffectType >= 150)
  {
    byErrCode = 21;
  }
  else if (skillFld->m_nTempEffectType == -1 && skillFld->m_nContEffectType == -1)
  {
    byErrCode = 8;
  }
  else if (!IsEffectableDst(skillFld->m_strActableDst, target))
  {
    byErrCode = 5;
  }
  else if (skillFld->m_nContEffectType != -1 && !target->IsRecvableContEffect())
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

  if (!byErrCode && !m_bSFDelayNotCheck
      && !_ATTACK_DELAY_CHECKER::IsDelay(&m_AttDelayChker, 2u, skillFld->m_dwIndex, skillFld->m_nMastIndex))
  {
    byErrCode = 9;
  }
  if (!byErrCode && !IsSFUsableGauge(2u, static_cast<unsigned __int16>(skillFld->m_dwIndex), delPoint))
  {
    byErrCode = 6;
  }
  if (!byErrCode && !IsSFActableByClass(2u, reinterpret_cast<_base_fld *>(skillFld)))
  {
    byErrCode = 16;
  }
  if (!byErrCode && !GetUseConsumeItem(skillFld->m_ConsumeItemList, pConsumeSerial, consumeItems, consumeCounts, consumeOverlap))
  {
    byErrCode = 32;
  }

  if (!byErrCode)
  {
    bool upMastery = false;
    unsigned __int8 assistErr = 0;
    const bool success = AssistSkill(target, 2u, skillFld, 1, &assistErr, &upMastery) != 0;
    byErrCode = assistErr;
    if (success)
    {
      ApplyGaugeConsumeAndSendRecover(this, delPoint);
      DeleteUseConsumeItem(consumeItems, consumeCounts, consumeOverlap);
      const float addDelay = m_EP.GetEff_Plus(12);
      _ATTACK_DELAY_CHECKER::SetDelay(
        &m_AttDelayChker,
        static_cast<unsigned int>(skillFld->m_fActDelay + addDelay));
    }
  }

  if ((byErrCode == 0 || byErrCode == 100) && hadStealth)
  {
    BreakStealth();
  }
  this->SendMsg_ClassSkillResult(byErrCode, pidDst, wSkillIndex);
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

void CPlayer::pc_GuildOfferSuggestRequest(
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *pwszComment,
  unsigned int dwMatterObj1,
  unsigned int dwMatterObj2,
  unsigned int dwMatterObj3)
{
  unsigned __int8 byRetCode = 0;
  CGuild *guild = m_Param.m_pGuild;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    byRetCode = 106;
  }
  else if (!guild)
  {
    byRetCode = static_cast<unsigned __int8>(-54);
  }
  else if (guild->m_bNowProcessSgtMter)
  {
    byRetCode = 61;
  }
  else if (guild->m_bRankWait)
  {
    byRetCode = 71;
  }
  else if (!guild->GetMemberFromSerial(dwMatterDst))
  {
    byRetCode = 63;
  }
  else if (guild->m_MasterData.IsFill() && guild->m_MasterData.dwSerial == dwMatterDst)
  {
    byRetCode = 67;
  }
  else if (!guild->m_bPossibleElectMaster)
  {
    byRetCode = static_cast<unsigned __int8>(-78);
  }

  if (!byRetCode
      && !guild->RegSuggestedMatter(
        m_dwObjSerial,
        byMatterType,
        dwMatterDst,
        pwszComment,
        dwMatterObj1,
        dwMatterObj2,
        dwMatterObj3))
  {
    byRetCode = 64;
  }

  this->SendMsg_OfferSuggestResult(static_cast<char>(byRetCode));
}

void CPlayer::pc_UnitTakeRequest()
{
  unsigned __int8 byRetCode = 0;
  BreakCloakBooster();

  if (m_bInGuildBattle && m_bTakeGravityStone)
  {
    byRetCode = 31;
  }
  else
  {
    bool canUseUnit = m_bFreeSFByClass;
    if (!canUseUnit)
    {
      for (int index = 0; index < 4; ++index)
      {
        void *historyEffect = m_Param.m_ppHistoryEffect[index];
        if (!historyEffect)
        {
          break;
        }
        if (*reinterpret_cast<int *>(reinterpret_cast<char *>(historyEffect) + 1436))
        {
          canUseUnit = true;
          break;
        }
      }
    }

    if (!canUseUnit)
    {
      byRetCode = 28;
    }
    else if (m_pSiegeItem)
    {
      byRetCode = 28;
    }
    else if (!m_pParkingUnit || !m_pUsingUnit)
    {
      byRetCode = 2;
    }
    else if (m_pParkingUnit->m_dwOwnerSerial != m_dwObjSerial)
    {
      byRetCode = 2;
    }
    else if (GetSqrt(m_fCurPos, m_pParkingUnit->m_fCurPos) > 50.0)
    {
      byRetCode = 20;
    }
    else if (m_bAfterEffect)
    {
      byRetCode = 32;
    }
    else if (m_pParkingUnit->GetHP() <= 0)
    {
      byRetCode = 35;
    }
  }

  if (!byRetCode)
  {
    float newPos[3] = {
      m_pParkingUnit->m_fCurPos[0],
      m_pParkingUnit->m_fCurPos[1],
      m_pParkingUnit->m_fCurPos[2],
    };
    m_pCurMap->GetRandPosInRange(m_pParkingUnit->m_fCurPos, 10, newPos);
    std::memcpy(m_fOldPos, m_fCurPos, sizeof(m_fOldPos));
    std::memcpy(m_fCurPos, newPos, sizeof(m_fCurPos));

    m_pParkingUnit->Destroy(1);
    m_pParkingUnit = nullptr;
    m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
    CalcDefTol();
    SetHaveEffect(0);
    SetShapeAllBuffer();

    SendMsg_AlterUnitHPInform(static_cast<char>(m_pUsingUnit->bySlotIndex), m_pUsingUnit->dwGauge);
  }

  this->SendMsg_UnitTakeResult(static_cast<char>(byRetCode));
}

void CPlayer::pc_UnitLeaveRequest(float *pfNewPos)
{
  unsigned __int8 byRetCode = 0;
  CParkingUnit *parkingUnit = nullptr;

  if (m_Param.GetRaceCode() != 0)
  {
    byRetCode = 1;
  }
  else if (!m_pUsingUnit || m_pParkingUnit)
  {
    byRetCode = 22;
  }
  else if (GetSqrt(m_fCurPos, pfNewPos) > 40.0)
  {
    byRetCode = 24;
  }
  else
  {
    parkingUnit = CreateParkingUnitObject(this, m_pUsingUnit->bySlotIndex, m_fCurPos);
    if (!parkingUnit)
    {
      byRetCode = 19;
    }
  }

  if (!byRetCode && parkingUnit)
  {
    parkingUnit->m_byCreateType = 1;
    m_pParkingUnit = parkingUnit;
    m_dwUnitViewOverTime = static_cast<unsigned int>(-1);
    CalcDefTol();
    SetHaveEffect(0);
    SetShapeAllBuffer();
  }

  this->SendMsg_UnitLeaveResult(static_cast<char>(byRetCode));
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
    unsigned int *packedStatus = &animus->m_dwLv;
    if (LOWORD(animus->m_dwLv) < animusFld->m_nMaxHP)
    {
      *reinterpret_cast<unsigned __int16 *>(packedStatus) += LOWORD(animusFld->m_nHPRecUnit);
      if (*reinterpret_cast<unsigned __int16 *>(packedStatus) > animusFld->m_nMaxHP)
      {
        *reinterpret_cast<unsigned __int16 *>(packedStatus) = static_cast<unsigned __int16>(animusFld->m_nMaxHP);
      }
    }

    if (HIWORD(animus->m_dwLv) < animusFld->m_nMaxFP)
    {
      reinterpret_cast<unsigned __int16 *>(packedStatus)[1] += LOWORD(animusFld->m_nFPRecUnit);
      if (reinterpret_cast<unsigned __int16 *>(packedStatus)[1] > animusFld->m_nMaxFP)
      {
        reinterpret_cast<unsigned __int16 *>(packedStatus)[1] = static_cast<unsigned __int16>(animusFld->m_nMaxFP);
      }
    }

    if (previousLv != animus->m_dwLv && m_pUserDB)
    {
      m_pUserDB->Update_ItemUpgrade(4u, static_cast<unsigned __int8>(animusIndex), animus->m_dwLv, false);
    }
  }
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
      ForcePullUnit(false);
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






