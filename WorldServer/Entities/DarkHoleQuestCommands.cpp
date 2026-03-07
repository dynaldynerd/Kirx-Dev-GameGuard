#include "pch.h"

#include "DarkHoleQuestCommands.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <new>

#include "CDarkHoleDungeonQuestSetup.h"
#include "CMainThread.h"
#include "CMapData.h"
#include "CMapOperation.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "strFILE.h"

namespace
{
  static const char kDHReactAreaNoValue[] = "-1";
  static const char kDHReactAreaDummyToken[] = "dummy:";
  static const char kDHReactAreaBlockToken[] = "block:";
  static const char kDHReactObjectMonsterToken[] = "monster:";
  static const char kDHReactObjectGroupToken[] = "m_group:";
  static const char kDHReactObjectItemToken[] = "item:";
  static const char kDHQuestCommandMembershipParty[] = "membership_party";
  static const char kDHQuestCommandMemberNum[] = "member_num";
  static const char kDHQuestCommandUseMap[] = "use_map";
  static const char kDHQuestCommandStartMission[] = "start_mission";
  static const char kDHQuestCommandLimitLvMin[] = "limit_lv_min";
  static const char kDHQuestCommandLimitLvMax[] = "limit_lv_max";
  static const char kDHQuestCommandDummyBlock[] = "dummy_block";
  static const char kDHQuestCommandMonsterGroup[] = "monster_group";
  static const char kDHQuestCommandRewardItem[] = "reward_item";
  static const char kDHQuestCommandRewardExp[] = "reward_exp";
  static const char kDHQuestCommandRewardPvp[] = "reward_pvp";
  static const char kDHQuestCommandRewardDalant[] = "reward_dalant";
  static const char kDHQuestCommandDescription[] = "description";
  static const char kDHMissionCommandArea[] = "area";
  static const char kDHMissionCommandStartPos[] = "start_pos";
  static const char kDHMissionCommandGatePos[] = "gate_pos";
  static const char kDHMissionCommandJobOrder[] = "job_order";
  static const char kDHMissionCommandLimTimeSec[] = "lim_time_sec";
  static const char kDHMissionCommandResultType[] = "result_type";
  static const char kDHMissionCommandResultContents[] = "result_contents";
  static const char kDHMissionCommandDescription[] = "description";
  static const char kDHMissionCommandCompleteMsg[] = "complete_msg";
  static const char kDHMissionCommandLootItem[] = "loot_item";
  static const char kDHMissionCommandAddMonster[] = "add_monster";
  static const char kDHMissionCommandRespawnMonster[] = "respawn_monster";
  static const char kDHMissionCommandRespawnMonsterOption[] = "respawn_monster_option";
  static const char kDHMissionCommandChangeMonster[] = "change_monster";
  static const char kDHMissionCommandIf[] = "if";
  static const char kDHMissionCommandInner[] = "inner";
  static const char kDHMissionCommandRespond[] = "respond";
  static const char kDHMissionCommandAddTime[] = "add_time";
  static const char kDHJobCommandType[] = "type";
  static const char kDHJobCommandContents[] = "contents";
  static const char kDHJobCommandCount[] = "count";
  static const char kDHJobCommandReactType[] = "react_type";
  static const char kDHJobCommandReactContents[] = "react_contents";
  static const char kDHJobCommandDescription[] = "description";
  static const char kDHRewardScopeOne[] = "one";
  static const char kDHJobOrderAnd[] = "AND";
  static const char kDHJobOrderOr[] = "OR";
  static const char kDHMissionResultComplete[] = "COMPLETE";
  static const char kDHMissionResultPortal[] = "PORTAL";
  static const char kDHMissionResultNext[] = "NEXT";
  static const char kDHRespawnTermToken[] = "term:";
  static const char kDHRespawnLimToken[] = "lim:";
  static const char kDHIfChangeMonster[] = "change_monster";
  static const char kDHIfDescription[] = "description";
  static const char kDHIfCompleteMsg[] = "complete_msg";
  static const char kDHInnerMsgToken[] = "msg:";
  static const char kDHInnerRespawnToken[] = "respawn_monster:";
  static const char kDHRespondHuntToken[] = "hunt:";
  static const char kDHJobTypeTake[] = "TAKE";
  static const char kDHJobTypeHunt[] = "HUNT";
  static const char kDHJobContentsAny[] = "ANY";
  static const char kDHJobReactPortal[] = "PORTAL";
}

QUEST_COMMAND quest_command[] = {
  {kDHQuestCommandMembershipParty, qc_MembershipParty},
  {kDHQuestCommandMemberNum, qc_MemberNum},
  {kDHQuestCommandUseMap, qc_UseMap},
  {kDHQuestCommandStartMission, qc_StartMission},
  {kDHQuestCommandLimitLvMin, qc_LimitLvMin},
  {kDHQuestCommandLimitLvMax, qc_LimitLvMax},
  {kDHQuestCommandDummyBlock, qc_DummyBlock},
  {kDHQuestCommandMonsterGroup, qc_monsterGroup},
  {kDHQuestCommandRewardItem, qc_RewardItem},
  {kDHQuestCommandRewardExp, qc_RewardExp},
  {kDHQuestCommandRewardPvp, qc_RewardPvp},
  {kDHQuestCommandRewardDalant, qc_Dalant},
  {kDHQuestCommandDescription, qc_Description},
  {nullptr, nullptr},
};

MISSION_COMMAND mission_command[] = {
  {kDHMissionCommandArea, mc_Area},
  {kDHMissionCommandStartPos, mc_StartPos},
  {kDHMissionCommandGatePos, mc_GatePos},
  {kDHMissionCommandJobOrder, mc_JobOrder},
  {kDHMissionCommandLimTimeSec, mc_LimTimeSec},
  {kDHMissionCommandResultType, mc_ResultType},
  {kDHMissionCommandResultContents, mc_ResultContents},
  {kDHMissionCommandDescription, mc_Description},
  {kDHMissionCommandCompleteMsg, mc_CompleteMsg},
  {kDHMissionCommandLootItem, mc_LootItem},
  {kDHMissionCommandAddMonster, mc_AddMonster},
  {kDHMissionCommandRespawnMonster, mc_RespawnMonster},
  {kDHMissionCommandRespawnMonsterOption, mc_RespawnMonsterOption},
  {kDHMissionCommandChangeMonster, mc_ChangeMonster},
  {kDHMissionCommandIf, mc_If},
  {kDHMissionCommandInner, mc_Inner},
  {kDHMissionCommandRespond, mc_respond},
  {kDHMissionCommandAddTime, mc_AddTime},
  {nullptr, nullptr},
};

JOB_COMMAND job_command[] = {
  {kDHJobCommandType, jc_Type},
  {kDHJobCommandContents, jc_Contents},
  {kDHJobCommandCount, jc_Count},
  {kDHJobCommandReactType, jc_ReactType},
  {kDHJobCommandReactContents, jc_ReactContents},
  {kDHJobCommandDescription, jc_Description},
  {nullptr, nullptr},
};

bool (__fastcall *SearchQuestCommandFn(char *pszCommand))(strFILE *, CDarkHoleDungeonQuestSetup *, char *)
{

  int index = 0;
  while (true)
  {
    QUEST_COMMAND *entry = &quest_command[index++];
    if (!entry->pszCommand)
      break;
    if (!strcmp_0(entry->pszCommand, pszCommand))
      return entry->fn;
  }
  return nullptr;
}

bool (__fastcall *SearchMissionCommandFn(char *pszCommand))(strFILE *, CDarkHoleDungeonQuestSetup *, char *)
{

  int index = 0;
  while (true)
  {
    MISSION_COMMAND *entry = &mission_command[index++];
    if (!entry->pszCommand)
      break;
    if (!strcmp_0(entry->pszCommand, pszCommand))
      return entry->fn;
  }
  return nullptr;
}

bool (__fastcall *SearchJobCommandFn(char *pszCommand))(strFILE *, CDarkHoleDungeonQuestSetup *, char *)
{

  int index = 0;
  while (true)
  {
    JOB_COMMAND *entry = &job_command[index++];
    if (!entry->pszCommand)
      break;
    if (!strcmp_0(entry->pszCommand, pszCommand))
      return entry->fn;
  }
  return nullptr;
}

bool __fastcall _false(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup)
{

  if (fstr)
    pSetup->m_ErrorInfo.nLine = fstr->read_line_count();
  return false;
}

bool __fastcall GetReactObject(
  strFILE *fstr,
  CDarkHoleDungeonQuestSetup *pSetup,
  _react_obj *poutReactObject,
  bool bReadCnt,
  unsigned int dwDesiredApply,
  char *pszoutErrMsg,
  char *pszoutEventCode)
{

  char poutszWord[160]{};
  char szRecordCode[136]{};
  char groupName[136]{};
  char psItemCode[136]{};
  int pnoutVal[7]{};

  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (!strcmp_0(poutszWord, kDHReactObjectMonsterToken))
  {
    if ((dwDesiredApply & 2) == 0)
      return _false(fstr, pSetup);
    poutReactObject->ObjDefType = react_kind_monster;
    if (!fstr->word(szRecordCode))
      return _false(fstr, pSetup);
    _monster_fld *record = (_monster_fld *)g_Main.m_tblMonster.GetRecord(szRecordCode);
    if (!record)
      return _false(fstr, pSetup);
    poutReactObject->obj.monster.pMonsterFld = record;
  }
  else if (!strcmp_0(poutszWord, kDHReactObjectGroupToken))
  {
    if ((dwDesiredApply & 1) == 0)
      return _false(fstr, pSetup);
    poutReactObject->ObjDefType = react_kind_mgrp;
    if (!fstr->word(groupName))
      return _false(fstr, pSetup);
    __monster_group *group = nullptr;
    for (int j = 0; j < pSetup->m_pCurLoadQuest->nMonsterGroupNum; ++j)
    {
      __monster_group *candidate = pSetup->m_pCurLoadQuest->pMonsterGroup[j];
      if (!strcmp_0(candidate->pszGroupName, groupName))
      {
        group = candidate;
        break;
      }
    }
    if (!group)
      return _false(fstr, pSetup);
    poutReactObject->obj.mon_grp.pMonGrp = group;
  }
  else
  {
    if (strcmp_0(poutszWord, kDHReactObjectItemToken))
    {
      if (pszoutEventCode)
      {
        strcpy_0(pszoutEventCode, poutszWord);
        return false;
      }
      return _false(fstr, pSetup);
    }
    if ((dwDesiredApply & 1) == 0)
      return _false(fstr, pSetup);
    poutReactObject->ObjDefType = react_kind_item;
    if (!fstr->word(psItemCode))
      return _false(fstr, pSetup);
    poutReactObject->obj.item.byItemTableCode = GetItemTableCode(psItemCode);
    if (poutReactObject->obj.item.byItemTableCode >= 37)
      return _false(fstr, pSetup);
    _base_fld *record =
      g_Main.m_tblItemData[poutReactObject->obj.item.byItemTableCode].GetRecord(psItemCode);
    if (!record)
      return _false(fstr, pSetup);
    poutReactObject->obj.item.pItemFld = record;
  }

  if (bReadCnt)
  {
    if (fstr->word(pnoutVal))
    {
      poutReactObject->wNum = static_cast<unsigned __int16>(pnoutVal[0]);
      return true;
    }
    return _false(fstr, pSetup);
  }
  return true;
}

bool __fastcall GetReactArea(
  strFILE *fstr,
  CDarkHoleDungeonQuestSetup *pSetup,
  _react_area *poutReactArea,
  unsigned int dwDesiredApply,
  char *pszoutErrMsg,
  char *pszoutEventCode)
{
char poutszWord[160]{};
  char targetName[136]{};
  if (!fstr->word(poutszWord) || !fstr->word(targetName))
    return _false(fstr, pSetup);
  if (!strcmp_0(targetName, kDHReactAreaNoValue))
  {
    if ((dwDesiredApply & 4) != 0)
    {
      poutReactArea->AreaDefType = at_var;
      return true;
    }
    return _false(fstr, pSetup);
  }
  if (strcmp_0(poutszWord, kDHReactAreaDummyToken))
  {
    if (!strcmp_0(poutszWord, kDHReactAreaBlockToken) && (dwDesiredApply & 2) != 0)
    {
      __dummy_block *block = nullptr;
      for (int j = 0; j < pSetup->m_pCurLoadQuest->nDummyBlockNum; ++j)
      {
        __dummy_block *candidate = pSetup->m_pCurLoadQuest->pDummyBlock[j];
        if (!strcmp_0(candidate->pszBlockName, targetName))
        {
          block = candidate;
          break;
        }
      }
      if (block)
      {
        poutReactArea->AreaDefType = at_block;
        poutReactArea->obj.block.pBlk = block;
        return true;
      }
    }
    return _false(fstr, pSetup);
  }
  if ((dwDesiredApply & 1) == 0)
    return _false(fstr, pSetup);
  _dummy_position *dummy = pSetup->m_pCurLoadQuest->pUseMap->GetDummyPostion(targetName);
  if (!dummy)
    return _false(fstr, pSetup);
  if (pSetup->m_pCurLoadMission->pAreaDummy)
  {
    unsigned int lineIndex = pSetup->m_pCurLoadMission->pAreaDummy->m_wLineIndex;
    CExtDummy *dummyTable = &pSetup->m_pCurLoadQuest->pUseMap->m_Dummy;
    if (!dummyTable->IsInBBox(lineIndex, dummy->m_fCenterPos))
      return _false(fstr, pSetup);
  }
  poutReactArea->AreaDefType = at_dummy;
  poutReactArea->obj.dummy.pPos = dummy;
  return true;
}

bool __fastcall qc_MembershipParty(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  int pnoutVal[9]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadQuest->bPartyOnly = pnoutVal[0];
  return true;
}

bool __fastcall qc_MemberNum(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  int pnoutVal[9]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadQuest->dwMaxMember = pnoutVal[0];
  return true;
}

bool __fastcall qc_UseMap(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[136]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  CMapData *map = g_MapOper.GetMap(poutszWord);
  if (!map || map->m_pMapSet->m_nMapType != 1)
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadQuest->pUseMap = map;
  return true;
}

bool __fastcall qc_StartMission(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord) || strlen_0(poutszWord) > 32)
    return _false(fstr, pSetup);
  strcpy_0(pSetup->m_pCurLoadQuest->szStartMissionBuffer, poutszWord);
  return true;
}

bool __fastcall qc_LimitLvMin(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadQuest->nLimitLvMin = atoi(poutszWord);
  return true;
}

bool __fastcall qc_LimitLvMax(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadQuest->nLimitLvMax = atoi(poutszWord);
  return true;
}

bool __fastcall qc_DummyBlock(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[160]{};
  char pszDummyCode[136]{};
  if (!fstr->word(poutszWord) || !fstr->word(pszDummyCode))
    return _false(fstr, pSetup);
  __dummy_block *block = nullptr;
  for (int j = 0; j < pSetup->m_pCurLoadQuest->nDummyBlockNum; ++j)
  {
    __dummy_block *candidate = pSetup->m_pCurLoadQuest->pDummyBlock[j];
    if (!strcmp_0(candidate->pszBlockName, poutszWord))
    {
      block = candidate;
      break;
    }
  }
  if (!block)
  {
    __dummy_block *newBlock = (__dummy_block *)operator new(272);
    if (newBlock)
    {
      new (newBlock) __dummy_block();
      block = newBlock;
    }
    pSetup->m_pCurLoadQuest->pDummyBlock[pSetup->m_pCurLoadQuest->nDummyBlockNum] = block;
    block = pSetup->m_pCurLoadQuest->pDummyBlock[pSetup->m_pCurLoadQuest->nDummyBlockNum];
    size_t nameLen = strlen_0(poutszWord);
    block->pszBlockName = (char *)operator new[](nameLen + 1);
    strcpy_0(block->pszBlockName, poutszWord);
    ++pSetup->m_pCurLoadQuest->nDummyBlockNum;
  }
  _dummy_position *dummy = pSetup->m_pCurLoadQuest->pUseMap->GetDummyPostion(pszDummyCode);
  if (!dummy)
    return _false(fstr, pSetup);
  block->pSubDummy[block->nSubDummyNum++] = dummy;
  return true;
}

bool __fastcall qc_monsterGroup(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[160]{};
  char szRecordCode[136]{};
  if (!fstr->word(poutszWord) || !fstr->word(szRecordCode))
    return _false(fstr, pSetup);
  __monster_group *group = nullptr;
  for (int j = 0; j < pSetup->m_pCurLoadQuest->nMonsterGroupNum; ++j)
  {
    __monster_group *candidate = pSetup->m_pCurLoadQuest->pMonsterGroup[j];
    if (!strcmp_0(candidate->pszGroupName, poutszWord))
    {
      group = candidate;
      break;
    }
  }
  if (!group)
  {
    __monster_group *newGroup = (__monster_group *)operator new(272);
    if (newGroup)
    {
      new (newGroup) __monster_group();
      group = newGroup;
    }
    pSetup->m_pCurLoadQuest->pMonsterGroup[pSetup->m_pCurLoadQuest->nMonsterGroupNum] = group;
    group = pSetup->m_pCurLoadQuest->pMonsterGroup[pSetup->m_pCurLoadQuest->nMonsterGroupNum];
    size_t nameLen = strlen_0(poutszWord);
    group->pszGroupName = (char *)operator new[](nameLen + 1);
    strcpy_0(group->pszGroupName, poutszWord);
    ++pSetup->m_pCurLoadQuest->nMonsterGroupNum;
  }
  _base_fld *record = g_Main.m_tblMonster.GetRecord(szRecordCode);
  if (!record)
    return _false(fstr, pSetup);
  group->pSubMonster[group->nSubMonsterNum++] = (_monster_fld *)record;
  return true;
}

bool __fastcall qc_RewardItem(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  const bool isOne = strcmp_0(poutszWord, kDHRewardScopeOne) == 0;
  char psItemCode[136]{};
  if (!fstr->word(psItemCode))
    return _false(fstr, pSetup);
  _dh_reward_sub_setup *reward = isOne ? &pSetup->m_pCurLoadQuest->RewardOne : &pSetup->m_pCurLoadQuest->RewardOther;
  unsigned __int8 itemTableCode = GetItemTableCode(psItemCode);
  if (itemTableCode >= 37)
    return _false(fstr, pSetup);
  _base_fld *record = g_Main.m_tblItemData[itemTableCode].GetRecord(psItemCode);
  if (!record)
    return _false(fstr, pSetup);
  unsigned __int8 itemKindCode = GetItemKindCode(itemTableCode);
  unsigned int maxParamFromExp = 268435455;
  if (itemKindCode)
  {
    if (itemKindCode == 1)
    {
      maxParamFromExp = GetMaxParamFromExp(record->m_dwIndex, 0LL);
    }
    else
    {
      return _false(fstr, pSetup);
    }
  }
  else
  {
    unsigned __int8 socketNum = GetDefItemUpgSocketNum(itemTableCode, record->m_dwIndex);
    maxParamFromExp = GetBitAfterSetLimSocket(socketNum);
  }
  unsigned int itemDurPoint = GetItemDurPoint(itemTableCode, record->m_dwIndex);
  int pnoutVal[8]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  if (IsOverLapItem(itemTableCode))
  {
    if (pnoutVal[0] > 99)
      pnoutVal[0] = 99;
    itemDurPoint = pnoutVal[0];
  }
  float pfoutVal[4]{};
  if (!fstr->word(pfoutVal) || pfoutVal[0] < 0.0f || pfoutVal[0] > 1.0f)
    return _false(fstr, pSetup);
  const int nItemNum = reward->nItemNum;
  if (static_cast<float>(pfoutVal[0] * TSVR_ADD_DARKHOLE_REWARD_RATE) <= 1.0f)
    reward->m_dwGivePercent[nItemNum] = static_cast<int>(static_cast<float>(pfoutVal[0] * 2147450900.0));
  else
    reward->m_dwGivePercent[nItemNum] = 2147450879;
  _STORAGE_LIST::_db_con *newItem = (_STORAGE_LIST::_db_con *)operator new(50);
  if (newItem)
  {
    new (newItem) _STORAGE_LIST::_db_con();
  }
  reward->Item[nItemNum] = newItem;
  _STORAGE_LIST::_db_con *item = reward->Item[nItemNum];
  item->m_byTableCode = itemTableCode;
  item->m_wItemIndex = record->m_dwIndex;
  item->m_dwDur = itemDurPoint;
  item->m_dwLv = maxParamFromExp;
  ++reward->nItemNum;
  return true;
}

bool __fastcall qc_RewardExp(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  const bool isOne = strcmp_0(poutszWord, kDHRewardScopeOne) == 0;
  long double pdoutVal[2]{};
  if (!fstr->word(pdoutVal))
    return _false(fstr, pSetup);
  _dh_reward_sub_setup *reward = isOne ? &pSetup->m_pCurLoadQuest->RewardOne : &pSetup->m_pCurLoadQuest->RewardOther;
  if (pdoutVal[0] < 0.0)
    return _false(fstr, pSetup);
  reward->dExp = pdoutVal[0];
  return true;
}

bool __fastcall qc_RewardPvp(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  const bool isOne = strcmp_0(poutszWord, kDHRewardScopeOne) == 0;
  int pnoutVal[5]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  _dh_reward_sub_setup *reward = isOne ? &pSetup->m_pCurLoadQuest->RewardOne : &pSetup->m_pCurLoadQuest->RewardOther;
  if (pnoutVal[0] < 0)
    return _false(fstr, pSetup);
  reward->dwPvp = pnoutVal[0];
  return true;
}

bool __fastcall qc_Dalant(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  const bool isOne = strcmp_0(poutszWord, kDHRewardScopeOne) == 0;
  int pnoutVal[5]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  _dh_reward_sub_setup *reward = isOne ? &pSetup->m_pCurLoadQuest->RewardOne : &pSetup->m_pCurLoadQuest->RewardOther;
  if (pnoutVal[0] < 0)
    return _false(fstr, pSetup);
  reward->dwDalant = pnoutVal[0];
  return true;
}

bool __fastcall qc_Description(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord) || strlen_0(poutszWord) > 16)
    return _false(fstr, pSetup);
  strcpy_0(pSetup->m_pCurLoadQuest->szDescirptCode, poutszWord);
  return true;
}

bool __fastcall mc_Area(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[136]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  _dummy_position *record =
    reinterpret_cast<_dummy_position *>(pSetup->m_pCurLoadQuest->pUseMap->m_tbQuestDumPos.GetRecord(poutszWord));
  if (!record)
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadMission->pAreaDummy = record;
  return true;
}

bool __fastcall mc_StartPos(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[136]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  _dummy_position *record =
    reinterpret_cast<_dummy_position *>(pSetup->m_pCurLoadQuest->pUseMap->m_tbPortalDumPos.GetRecord(poutszWord));
  if (!record)
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadMission->pStartDummy = record;
  return true;
}

bool __fastcall mc_GatePos(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[136]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (!pSetup->m_pCurLoadQuest->pUseMap->m_tbPortalDumPos.GetRecord(poutszWord))
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadMission->facMissionPotal.set(poutszWord);
  return true;
}

bool __fastcall mc_JobOrder(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (!strcmp_0(poutszWord, kDHJobOrderAnd))
  {
    pSetup->m_pCurLoadMission->byJobOrder = 0;
  }
  else
  {
    if (strcmp_0(poutszWord, kDHJobOrderOr))
      return _false(fstr, pSetup);
    pSetup->m_pCurLoadMission->byJobOrder = 1;
  }
  return true;
}

bool __fastcall mc_LimTimeSec(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  int pnoutVal[9]{};
  if (!fstr->word(pnoutVal) || pnoutVal[0] < 0)
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadMission->dwLimTimeMSec = 1000 * pnoutVal[0];
  return true;
}

bool __fastcall mc_ResultType(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (!strcmp_0(poutszWord, kDHMissionResultComplete))
  {
    pSetup->m_pCurLoadMission->byResultType = 0;
  }
  else if (!strcmp_0(poutszWord, kDHMissionResultPortal))
  {
    pSetup->m_pCurLoadMission->byResultType = 1;
  }
  else
  {
    if (strcmp_0(poutszWord, kDHMissionResultNext))
      return _false(fstr, pSetup);
    pSetup->m_pCurLoadMission->byResultType = 2;
  }
  return true;
}

bool __fastcall mc_ResultContents(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord) || strlen_0(poutszWord) > 32)
    return _false(fstr, pSetup);
  pSetup->m_pCurLoadMission->pszNextMissionTitle = (char *)operator new[](33);
  strcpy_0(pSetup->m_pCurLoadMission->pszNextMissionTitle, poutszWord);
  return true;
}

bool __fastcall mc_Description(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord) || strlen_0(poutszWord) > 16)
    return _false(fstr, pSetup);
  strcpy_0(pSetup->m_pCurLoadMission->szDescirptCode, poutszWord);
  return true;
}

bool __fastcall mc_CompleteMsg(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord) || strlen_0(poutszWord) > 16)
    return _false(fstr, pSetup);
  strcpy_0(pSetup->m_pCurLoadMission->szCompleteMsg, poutszWord);
  return true;
}

bool __fastcall mc_LootItem(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  const int itemTableCode = GetItemTableCode(poutszWord);
  if (itemTableCode == -1)
    return _false(fstr, pSetup);
  _base_fld *record = g_Main.m_tblItemData[itemTableCode].GetRecord(poutszWord);
  if (!record)
    return _false(fstr, pSetup);
  int pnoutVal[9]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  if (pnoutVal[0] <= 0)
    return _false(fstr, pSetup);
  _react_obj poutReactObject;
    if (!GetReactObject(fstr, pSetup, &poutReactObject, 0, 6u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  const int nLootItemNum = pSetup->m_pCurLoadMission->nLootItemNum;
  __add_loot_item *item = (__add_loot_item *)operator new(56);
  if (item)
  {
    new (item) __add_loot_item();
  }
  pSetup->m_pCurLoadMission->pLootItem[nLootItemNum] = item;
  pSetup->m_pCurLoadMission->pLootItem[nLootItemNum]->byItemTableCode = itemTableCode;
  pSetup->m_pCurLoadMission->pLootItem[nLootItemNum]->pItemFld = record;
  pSetup->m_pCurLoadMission->pLootItem[nLootItemNum]->dwDur = pnoutVal[0];
  pSetup->m_pCurLoadMission->pLootItem[nLootItemNum]->ReactObj.copy(&poutReactObject);
  ++pSetup->m_pCurLoadMission->nLootItemNum;
  return true;
}

bool __fastcall mc_AddMonster(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  _react_obj poutReactObject;
    if (!GetReactObject(fstr, pSetup, &poutReactObject, 1, 6u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  _react_area poutReactArea;
    if (!GetReactArea(fstr, pSetup, &poutReactArea, 3u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  const int nAddMonsterNum = pSetup->m_pCurLoadMission->nAddMonsterNum;
  if (nAddMonsterNum >= 32)
    return _false(fstr, pSetup);
  __add_monster *monster = (__add_monster *)operator new(48);
  if (monster)
  {
    new (monster) __add_monster();
  }
  pSetup->m_pCurLoadMission->pAddMonster[nAddMonsterNum] = monster;
  pSetup->m_pCurLoadMission->pAddMonster[nAddMonsterNum]->ReactObj.copy(&poutReactObject);
  pSetup->m_pCurLoadMission->pAddMonster[nAddMonsterNum]->ReactArea.copy(&poutReactArea);
  ++pSetup->m_pCurLoadMission->nAddMonsterNum;
  return true;
}

bool __fastcall mc_RespawnMonster(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  _react_obj poutReactObject;
    if (!GetReactObject(fstr, pSetup, &poutReactObject, 1, 6u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  if (!poutReactObject.wNum)
    return _false(fstr, pSetup);
  if (poutReactObject.wNum > 128)
    return _false(fstr, pSetup);
  _react_area poutReactArea;
    if (!GetReactArea(fstr, pSetup, &poutReactArea, 3u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  char poutszWord[148]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (strcmp_0(poutszWord, kDHRespawnTermToken))
    return _false(fstr, pSetup);
  int pnoutVal[11]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  char Str1[148]{};
  if (!fstr->word(Str1))
    return _false(fstr, pSetup);
  if (strcmp_0(Str1, kDHRespawnLimToken))
    return _false(fstr, pSetup);
  int limVal[4]{};
  if (!fstr->word(limVal))
    return _false(fstr, pSetup);
  const int nRespawnMonsterNum = pSetup->m_pCurLoadMission->nRespawnMonsterNum;
  if (nRespawnMonsterNum >= 32)
    return _false(fstr, pSetup);
  __respawn_monster *respawn = (__respawn_monster *)operator new(72);
  if (respawn)
  {
    new (respawn) __respawn_monster();
  }
  pSetup->m_pCurLoadMission->pRespawnMonster[nRespawnMonsterNum] = respawn;
  pSetup->m_pCurLoadMission->pRespawnMonster[nRespawnMonsterNum]->ReactObj.copy(&poutReactObject);
  pSetup->m_pCurLoadMission->pRespawnMonster[nRespawnMonsterNum]->ReactArea.copy(&poutReactArea);
  pSetup->m_pCurLoadMission->pRespawnMonster[nRespawnMonsterNum]->dwTermMSec = 1000 * pnoutVal[0];
  pSetup->m_pCurLoadMission->pRespawnMonster[nRespawnMonsterNum]->nLim = limVal[0];
  ++pSetup->m_pCurLoadMission->nRespawnMonsterNum;
  return true;
}

bool __fastcall mc_RespawnMonsterOption(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[160]{};
  char nextWord[160]{};
  char Source[132]{};
  if (!fstr->word(poutszWord)
    || !fstr->word(nextWord)
    || !fstr->word(Source)
    || !pSetup->m_pCurLoadMission->nRespawnMonsterNum)
  {
    return _false(fstr, pSetup);
  }
  const int index = pSetup->m_pCurLoadMission->nRespawnMonsterNum - 1;
  __respawn_monster *respawn = pSetup->m_pCurLoadMission->pRespawnMonster[index];
  if (!respawn->pszDefineCode)
  {
    respawn->pszDefineCode = (char *)operator new[](33);
  }
  strcpy_0(respawn->pszDefineCode, Source);
  respawn->bCallEvent = true;
  return true;
}

bool __fastcall mc_ChangeMonster(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[160]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  char szRecordCode[160]{};
  if (!fstr->word(szRecordCode))
    return _false(fstr, pSetup);
  char Str[136]{};
  if (!fstr->word(Str))
    return _false(fstr, pSetup);
  _monster_fld *recordA = (_monster_fld *)g_Main.m_tblMonster.GetRecord(poutszWord);
  if (!recordA)
    return _false(fstr, pSetup);
  _monster_fld *recordB = (_monster_fld *)g_Main.m_tblMonster.GetRecord(szRecordCode);
  if (!recordB)
    return _false(fstr, pSetup);
  int len = static_cast<int>(strlen_0(Str));
  if (Str[len] == '%')
    Str[len] = '\0';
  const int prob = atoi(Str);
  const int nChangeMonsterNum = pSetup->m_pCurLoadMission->nChangeMonsterNum;
  __change_monster *change = (__change_monster *)operator new(40uLL);
  if (change)
  {
    new (change) __change_monster();
  }
  pSetup->m_pCurLoadMission->pChangeMonster[nChangeMonsterNum] = change;
  pSetup->m_pCurLoadMission->pChangeMonster[nChangeMonsterNum]->pMonsterFldA = recordA;
  pSetup->m_pCurLoadMission->pChangeMonster[nChangeMonsterNum]->pMonsterFldB = recordB;
  pSetup->m_pCurLoadMission->pChangeMonster[nChangeMonsterNum]->nProb = prob;
  ++pSetup->m_pCurLoadMission->nChangeMonsterNum;
  return true;
}

bool __fastcall mc_If(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[160]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (strcmp_0(poutszWord, kDHIfChangeMonster))
    return _false(fstr, pSetup);
  char Str1[132]{};
  if (!fstr->word(Str1))
    return _false(fstr, pSetup);
  const int index = pSetup->m_pCurLoadMission->nChangeMonsterNum - 1;
  if (index == -1)
    return _false(fstr, pSetup);
  __change_monster *change = pSetup->m_pCurLoadMission->pChangeMonster[index];
  if (strcmp_0(Str1, kDHIfDescription))
  {
    char Source[144]{};
    if (!strcmp_0(Str1, kDHIfCompleteMsg)
      && fstr->word(Source)
      && strlen_0(Source) <= 16
      && !change->pszifCompleteMsg)
    {
      change->pszifCompleteMsg = (char *)operator new[](17uLL);
      strcpy_0(change->pszifCompleteMsg, Source);
      return true;
    }
    return _false(fstr, pSetup);
  }
  char Str[160]{};
  if (!fstr->word(Str) || strlen_0(Str) > 16 || change->pszIfMissionDescirptCode)
    return _false(fstr, pSetup);
  change->pszIfMissionDescirptCode = (char *)operator new[](17uLL);
  strcpy_0(change->pszIfMissionDescirptCode, Str);
  return true;
}

bool __fastcall mc_Inner(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  _react_area poutReactArea;
    if (!GetReactArea(fstr, pSetup, &poutReactArea, 3u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  bool hasEventObj = false;
  _react_obj poutReactObject;
    char Str1[132]{};
  bool hasMsgOrRespawn = false;
  if (GetReactObject(fstr, pSetup, &poutReactObject, 1, 7u, pszoutErrMsg, Str1))
  {
    hasEventObj = true;
  }
  else
  {
    if (strcmp_0(Str1, kDHInnerMsgToken) && strcmp_0(Str1, kDHInnerRespawnToken))
      return _false(fstr, pSetup);
    hasMsgOrRespawn = true;
  }

  char msgCode[32]{};
  char respawnCode[72]{};
  if (hasMsgOrRespawn)
  {
    if (!strcmp_0(Str1, kDHInnerMsgToken))
    {
      char poutszWord[160]{};
      if (!fstr->word(poutszWord))
        return _false(fstr, pSetup);
      memcpy_0(msgCode, poutszWord, 16);
      msgCode[16] = '\0';
    }
    else
    {
      char Src[152]{};
      if (strcmp_0(Str1, kDHInnerRespawnToken) || !fstr->word(Src))
        return _false(fstr, pSetup);
      memcpy_0(respawnCode, Src, 32);
      respawnCode[32] = '\0';
    }
  }

  _react_area afterArea;
    if (hasEventObj && !GetReactArea(fstr, pSetup, &afterArea, 3u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  const int nInnerCheckNum = pSetup->m_pCurLoadMission->nInnerCheckNum;
  __inner_check *inner = (__inner_check *)operator new(80);
  if (inner)
  {
    new (inner) __inner_check();
  }
  pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum] = inner;
  pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->ReactArea_Evt.copy(&poutReactArea);
  pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->ReactArea_Aft.copy(&afterArea);
  pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->ReactObj.copy(&poutReactObject);
  if (msgCode[0])
  {
    pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->pszMsg = (char *)operator new[](17);
    strcpy_0(pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->pszMsg, msgCode);
  }
  if (respawnCode[0])
  {
    pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->pszRespawnCode = (char *)operator new[](33);
    strcpy_0(pSetup->m_pCurLoadMission->pInnerCheck[nInnerCheckNum]->pszRespawnCode, respawnCode);
  }
  ++pSetup->m_pCurLoadMission->nInnerCheckNum;
  return true;
}

bool __fastcall mc_respond(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[160]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  char szRecordCode[132]{};
  if (!fstr->word(szRecordCode))
    return _false(fstr, pSetup);
  unsigned __int8 actTableCode = static_cast<unsigned __int8>(-1);
  _base_fld *record = nullptr;
  if (strcmp_0(poutszWord, kDHRespondHuntToken))
    return _false(fstr, pSetup);
  record = g_Main.m_tblMonster.GetRecord(szRecordCode);
  if (!record)
    return _false(fstr, pSetup);
  _react_obj poutReactObject;
    char Str1[132]{};
  bool hasMsgOnly = false;
  bool hasEventObj = false;
  if (GetReactObject(fstr, pSetup, &poutReactObject, 1, 7u, pszoutErrMsg, Str1))
  {
    hasEventObj = true;
  }
  else
  {
    if (strcmp_0(Str1, kDHInnerMsgToken))
      return _false(fstr, pSetup);
    hasMsgOnly = true;
  }
  char msgCode[56]{};
  if (hasMsgOnly && !strcmp_0(Str1, kDHInnerMsgToken))
  {
    char Src[152]{};
    if (fstr->word(Src))
    {
      memcpy_0(msgCode, Src, 16);
      msgCode[16] = '\0';
    }
    else
    {
      return _false(fstr, pSetup);
    }
  }

  _react_area poutReactArea;
    if (hasEventObj && !GetReactArea(fstr, pSetup, &poutReactArea, 7u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  const int nRespondCheckNum = pSetup->m_pCurLoadMission->nRespondCheckNum;
  __respond_check *respond = (__respond_check *)operator new(72);
  if (respond)
  {
    new (respond) __respond_check();
  }
  pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum] = respond;
  pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum]->byActTableCode = actTableCode;
  pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum]->pActObjFld = record;
  pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum]->ReactObj.copy(&poutReactObject);
  pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum]->ReactArea.copy(&poutReactArea);
  if (msgCode[0])
  {
    pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum]->pszMsg = (char *)operator new[](16);
    strcpy_0(pSetup->m_pCurLoadMission->pRespondCheck[nRespondCheckNum]->pszMsg, msgCode);
  }
  ++pSetup->m_pCurLoadMission->nRespondCheckNum;
  return true;
}

bool __fastcall mc_AddTime(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  _react_obj poutReactObject;
    if (!GetReactObject(fstr, pSetup, &poutReactObject, 0, 7u, pszoutErrMsg, nullptr))
    return _false(fstr, pSetup);
  char poutszWord[148]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  int pnoutVal[11]{};
  if (!fstr->word(pnoutVal) || !fstr->word(poutszWord))
    return _false(fstr, pSetup);
  char Str1[132]{};
  if (!fstr->word(Str1))
    return _false(fstr, pSetup);
  const int nAddSecCheckNum = pSetup->m_pCurLoadMission->nAddSecCheckNum;
  __add_time *addTime = (__add_time *)operator new(48);
  if (addTime)
  {
    new (addTime) __add_time();
  }
  pSetup->m_pCurLoadMission->pAddSecCheck[nAddSecCheckNum] = addTime;
  pSetup->m_pCurLoadMission->pAddSecCheck[nAddSecCheckNum]->EventObj.copy(&poutReactObject);
  pSetup->m_pCurLoadMission->pAddSecCheck[nAddSecCheckNum]->dwAddSec = pnoutVal[0];
  if (strcmp_0(Str1, kDHReactAreaNoValue))
  {
    pSetup->m_pCurLoadMission->pAddSecCheck[nAddSecCheckNum]->pszMsg = (char *)operator new[](16);
    strcpy_0(pSetup->m_pCurLoadMission->pAddSecCheck[nAddSecCheckNum]->pszMsg, Str1);
  }
  ++pSetup->m_pCurLoadMission->nAddSecCheckNum;
  return true;
}

bool __fastcall jc_Type(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  if (!strcmp_0(poutszWord, kDHJobTypeTake))
  {
    pSetup->m_pCurLoadJob->eventType = dh_event_take;
  }
  else
  {
    if (strcmp_0(poutszWord, kDHJobTypeHunt))
      return _false(fstr, pSetup);
    pSetup->m_pCurLoadJob->eventType = dh_event_hunt;
  }
  return true;
}

bool __fastcall jc_Contents(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord))
    return _false(fstr, pSetup);
  EM_DH_EVENT eventType = pSetup->m_pCurLoadJob->eventType;
  if (eventType)
  {
    if (eventType != dh_event_hunt)
      return true;
    if (!strcmp_0(poutszWord, kDHJobContentsAny))
    {
      pSetup->m_pCurLoadJob->JobSetup.pEventFld = nullptr;
      return true;
    }
    _base_fld *record = g_Main.m_tblMonster.GetRecord(poutszWord);
    if (record)
    {
      pSetup->m_pCurLoadJob->JobSetup.pEventFld = record;
      return true;
    }
    return _false(fstr, pSetup);
  }
  if (!strcmp_0(poutszWord, kDHJobContentsAny))
  {
    pSetup->m_pCurLoadJob->JobSetup.pEventFld = nullptr;
    pSetup->m_pCurLoadJob->JobSetup.byTable = static_cast<unsigned __int8>(-1);
  }
  else
  {
    unsigned __int8 itemTableCode = GetItemTableCode(poutszWord);
    if (itemTableCode >= 37)
      return _false(fstr, pSetup);
    _base_fld *record = g_Main.m_tblItemData[itemTableCode].GetRecord(poutszWord);
    if (!record)
      return _false(fstr, pSetup);
    pSetup->m_pCurLoadJob->JobSetup.pEventFld = record;
    pSetup->m_pCurLoadJob->JobSetup.byTable = itemTableCode;
  }
  return true;
}

bool __fastcall jc_Count(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  int pnoutVal[4]{};
  if (!fstr->word(pnoutVal))
    return _false(fstr, pSetup);
  if (pnoutVal[0] < 1)
  {
    bool fail = true;
    if (pnoutVal[0] == -1)
      fail = pSetup->m_pCurLoadJob->eventType != dh_event_hunt;
    if (fail)
      return _false(fstr, pSetup);
  }
  pSetup->m_pCurLoadJob->JobSetup.nEventCount = pnoutVal[0];
  return true;
}

bool __fastcall jc_ReactType(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord) || strcmp_0(poutszWord, kDHJobReactPortal))
    return _false(fstr, pSetup);
  const int nReactNum = pSetup->m_pCurLoadJob->nReactNum;
  _react_sub_setup *react = (_react_sub_setup *)operator new(16);
  if (react)
  {
    new (react) _react_sub_setup();
  }
  pSetup->m_pCurLoadJob->ReactSetup[nReactNum] = react;
  pSetup->m_pCurLoadJob->ReactSetup[nReactNum]->byReactType = 0;
  ++pSetup->m_pCurLoadJob->nReactNum;
  return true;
}

bool __fastcall jc_ReactContents(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[132]{};
  if (!fstr->word(poutszWord) || pSetup->m_pCurLoadJob->nReactNum <= 0)
    return _false(fstr, pSetup);
  const int index = pSetup->m_pCurLoadJob->nReactNum - 1;
  if (!pSetup->m_pCurLoadJob->ReactSetup[index]->byReactType)
  {
    _dummy_position *record =
      reinterpret_cast<_dummy_position *>(pSetup->m_pCurLoadQuest->pUseMap->m_tbPortalDumPos.GetRecord(poutszWord));
    if (record)
    {
      if (!pSetup->m_pCurLoadMission->pAreaDummy)
      {
        pSetup->m_pCurLoadJob->ReactSetup[index]->pPortalDummy = record;
        return true;
      }
      unsigned int lineIndex = pSetup->m_pCurLoadMission->pAreaDummy->m_wLineIndex;
      CExtDummy *dummyTable = &pSetup->m_pCurLoadQuest->pUseMap->m_Dummy;
      if (dummyTable->IsInBBox(lineIndex, record->m_fCenterPos))
      {
        pSetup->m_pCurLoadJob->ReactSetup[index]->pPortalDummy = record;
        return true;
      }
    }
    return _false(fstr, pSetup);
  }
  return true;
}

bool __fastcall jc_Description(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg)
{

  char poutszWord[144]{};
  if (!fstr->word(poutszWord) || strlen_0(poutszWord) > 16)
    return _false(fstr, pSetup);
  strcpy_0(pSetup->m_pCurLoadJob->szDescirptCode, poutszWord);
  return true;
}








