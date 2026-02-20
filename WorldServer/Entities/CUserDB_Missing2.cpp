#include "pch.h"

#include "WorldServerUtil.h"

#include "CUserDB.h"
#include "CPlayer.h"
#include "CLogFile.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "RFEvent_ClassRefine.h"
#include "log_sheet_lv.h"
#include "qry_case_contsave.h"

#include <cstring>
#include <cstdio>
#include <mmsystem.h>

static const char aAe[] = "FirstSettingData (%s) Item[%d] Invalid Table Code (%s)";
static const char aAe_0[] = "FirstSettingData (%s) Item[%d] Unit Item Not Allowed (%s)";
static const char aAe_1[] = "FirstSettingData (%s) Item[%d] Record Not Found (%s)";
static const char aDDatavalidchec_21[] = "%d > DataValidCheckRevise Àå";
static const char aDDatavalidchec_22[] = "%d > DataValidCheckRevise ";
static const char aDDatavalidchec_23[] = "%d > DataValidCheckRevise Àå";
static const char aDDatavalidchec_24[] = "%d > DataValidCheckRevise Æ";
static const char aDDatavalidchec_25[] = "%d > DataValidCheckRevise ";
static const char aDDDatavalidche[] = "%d > %d DataValidCheckRevise À";
static const char aDDDatavalidche_0[] = "%d > %d DataValidCheckRevise Äù";
static const char aDDDatavalidche_1[] = "%d > %d DataValidCheckRevise Äù";
static const char aDDatavalidchec_26[] = "%d > DataValidCheckRevise ÀÌÆåÆ® ÀÎµ";
static const char aDDatavalidchec_27[] = "%d > DataValidCheckRevise ";

void CUserDB::WriteLog_Level(unsigned __int8 byLv)
{
  if ( !this->m_bField )
  {
    return;
  }

  _log_sheet_lv pQryData{};
  pQryData.dwAvatorSerial = this->m_dwSerial;
  pQryData.byLv = byLv;
  pQryData.dwTotalPlayMin = this->m_AvatorData.dbAvator.m_dwTotalPlayMin;

  int nSize = static_cast<int>(pQryData.size());
  g_Main.PushDQSData(
    this->m_dwAccountSerial,
    &this->m_idWorld,
    7u,
    reinterpret_cast<char *>(&pQryData),
    nSize);
}

char CUserDB::Update_Level(unsigned __int8 lv, long double exp)
{
  if ( this->m_AvatorData.dbAvator.m_byMaxLevel )
  {
    if ( lv > this->m_AvatorData.dbAvator.m_byMaxLevel && (!this->m_byUserDgr || this->m_byUserDgr == 1) )
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_Level(): pRecv->byLv (%d) > max level (%d) => failed ",
        this->m_aszAvatorName,
        lv,
        this->m_AvatorData.dbAvator.m_byMaxLevel);
      return 0;
    }
  }
  else if ( lv > 0x32u )
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_Level(): pRecv->byLv (%d) => failed ",
      this->m_aszAvatorName,
      lv);
    return 0;
  }

  if ( lv > this->m_AvatorData.dbAvator.m_byLevel || this->m_byUserDgr )
  {
    this->m_AvatorData.dbAvator.m_byLevel = lv;
    this->m_AvatorData.dbAvator.m_dExp = exp;
    if ( !this->m_byUserDgr )
    {
      this->WriteLog_Level(lv);
    }
    this->m_bDataUpdate = 1;
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_Level(): OLD(%d) >= NEW(%d) => failed ",
    this->m_aszAvatorName,
    this->m_AvatorData.dbAvator.m_byLevel,
    lv);
  return 0;
}

#if 0 // duplicate implementations exist in CUserDB.cpp
void CUserDB::Update_MaxLevel(unsigned __int8 byMaxLevel)
{
  this->m_AvatorData.dbAvator.m_byMaxLevel = byMaxLevel;
}

char CUserDB::Update_QuestUpdate(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData, bool bUpdate)
{
  if (bySlotIndex < 0x1Eu)
  {
    if (this->m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_QuestUpdate(NOTHING) : slot : %d",
        this->m_aszAvatorName,
        bySlotIndex);
      return 0;
    }

    memcpy_0(
      &this->m_AvatorData.dbQuest.m_List[bySlotIndex],
      pSlotData,
      sizeof(this->m_AvatorData.dbQuest.m_List[bySlotIndex]));
    if (bUpdate)
    {
      this->m_bDataUpdate = 1;
    }
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_QuestUpdate(SlotIndex OVER) : slot : %d",
    this->m_aszAvatorName,
    bySlotIndex);
  return 0;
}

char CUserDB::Update_QuestDelete(unsigned __int8 bySlotIndex)
{
  if (bySlotIndex < 0x1Eu)
  {
    if (this->m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
    {
      g_Main.m_logSystemError.Write(
        "%s : Update_QuestDelete(EXIST) : slot : %d",
        this->m_aszAvatorName,
        bySlotIndex);
      return 0;
    }

    this->m_AvatorData.dbQuest.m_List[bySlotIndex].Init();
    this->m_bDataUpdate = 1;
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_QuestDelete(SlotIndex OVER) : slot : %d",
    this->m_aszAvatorName,
    bySlotIndex);
  return 0;
}

char CUserDB::Update_QuestInsert(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pSlotData)
{
  if (bySlotIndex < 0x1Eu)
  {
    if (this->m_AvatorData.dbQuest.m_List[bySlotIndex].byQuestType == 0xFF)
    {
      memcpy_0(
        &this->m_AvatorData.dbQuest.m_List[bySlotIndex],
        pSlotData,
        sizeof(this->m_AvatorData.dbQuest.m_List[bySlotIndex]));
      this->m_bDataUpdate = 1;
      return 1;
    }

    g_Main.m_logSystemError.Write(
      "%s : Update_QuestInsert(EXIST) : slot : %d",
      this->m_aszAvatorName,
      bySlotIndex);
    return 0;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_QuestInsert(SlotIndex OVER) : slot : %d",
    this->m_aszAvatorName,
    bySlotIndex);
  return 0;
}

char CUserDB::Update_NPCQuestHistory(unsigned __int8 byIndex, _QUEST_DB_BASE::_NPC_QUEST_HISTORY *pHisData)
{
  if (byIndex < 0x46u)
  {
    memcpy_0(
      &this->m_AvatorData.dbQuest.m_History[byIndex],
      pHisData,
      sizeof(this->m_AvatorData.dbQuest.m_History[byIndex]));
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "%s : Update_NPCQuestHistory(Index OVER) : %d",
    this->m_aszAvatorName,
    byIndex);
  return 0;
}
#endif

char CUserDB::Update_TakeLastMentalTicket(unsigned int dwMentalTicket)
{
  m_AvatorData.dbAvator.m_dwTakeLastMentalTicket = dwMentalTicket;
  m_bDataUpdate = true;
  return 1;
}

char CUserDB::Update_TrunkSlotNum(unsigned __int8 bySlotNum)
{
  this->m_AvatorData.dbTrunk.bySlotNum = bySlotNum;
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_ExtTrunkSlotNum(unsigned __int8 byExtSlotNum)
{
  this->m_AvatorData.dbTrunk.byExtSlotNum = byExtSlotNum;
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_ItemUpgrade(unsigned __int8 storage, unsigned __int8 slot, unsigned int upg, bool bUpdate)
{
  if (!IsStorageRange(storage, slot))
  {
    g_Main.m_logSystemError.Write(
      "%s : Update_Upgrade(CODE) : scode : %d, icode : %d  ",
      this->m_aszAvatorName,
      storage,
      slot);
    return 0;
  }

  if (storage)
  {
    switch (storage)
    {
      case 1:
      {
        _EQUIPKEY *key = &this->m_AvatorData.dbAvator.m_EquipKey[slot];
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(EQUIP, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        this->m_AvatorData.dbAvator.m_dwFixEquipLv[slot] = upg;
        break;
      }
      case 4:
      {
        _ANIMUSKEY *key = &this->m_AvatorData.dbAnimus.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(ANIMUS, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        *reinterpret_cast<unsigned int *>(&key[9].byItemIndex) = upg;
        break;
      }
      case 5:
      {
        _INVENKEY *key = &this->m_AvatorData.dbTrunk.m_List[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(TRUNK, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        memcpy_0(&key[3], &upg, sizeof(upg));
        break;
      }
      case 7:
      {
        _INVENKEY *key = &this->m_AvatorData.dbTrunk.m_ExtList[slot].Key;
        if (!key->IsFilled())
        {
          g_Main.m_logSystemError.Write("%s:Update_Upgrade(EXT_TRUNK, Idx:%d)", this->m_aszAvatorName, slot);
          return 0;
        }
        memcpy_0(&key[3], &upg, sizeof(upg));
        break;
      }
      default:
        g_Main.m_logSystemError.Write(
          "%s:Update_Upgrade(NOT EQUAL(%d), Idx:%d)",
          this->m_aszAvatorName,
          storage,
          slot);
        return 0;
    }
  }
  else
  {
    _INVENKEY *key = &this->m_AvatorData.dbInven.m_List[slot].Key;
    if (!key->IsFilled())
    {
      g_Main.m_logSystemError.Write("%s:Update_Upgrade(INVEN, Idx:%d)", this->m_aszAvatorName, slot);
      return 0;
    }
    memcpy_0(&key[3], &upg, sizeof(upg));
  }

  if (bUpdate)
  {
    this->m_bDataUpdate = 1;
  }
  return 1;
}

bool CUserDB::FirstSettingData()
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v4; // [rsp+0h] [rbp-E8h] BYREF
  unsigned int *Record; // [rsp+30h] [rbp-B8h]
  unsigned int *v6; // [rsp+38h] [rbp-B0h]
  __int64 v7; // [rsp+40h] [rbp-A8h]
  int j; // [rsp+48h] [rbp-A0h]
  unsigned int dwNewCum; // [rsp+4Ch] [rbp-9Ch]
  int k; // [rsp+50h] [rbp-98h]
  int v11; // [rsp+54h] [rbp-94h]
  char *Str1; // [rsp+58h] [rbp-90h]
  int nTableCode; // [rsp+60h] [rbp-88h]
  unsigned __int16 *v14; // [rsp+68h] [rbp-80h]
  unsigned __int64 dwExp; // [rsp+70h] [rbp-78h]
  unsigned __int8 ItemKindCode; // [rsp+78h] [rbp-70h]
  _STORAGE_LIST::_db_con pItem; // [rsp+88h] [rbp-60h] BYREF
  _SKILL_IDX_PER_MASTERY *v18; // [rsp+C8h] [rbp-20h]
  _SKILL_IDX_PER_MASTERY *v19; // [rsp+D0h] [rbp-18h]
  if ( !this->m_bActive )
    return 0;
  Record = (unsigned int *)g_Main.m_tblClass.GetRecord( this->m_AvatorData.dbAvator.m_szClassCode);
  if ( !Record )
    return 0;
  v7 = 0LL;
  v6 = Record + 370;
  for ( j = 0; j < 2; ++j )
  {
    if ( v6[j] )
      this->Update_Stat( j, v6[j], 1);
  }
  if ( Record[372] )
    this->Update_Stat( 0x4Fu, Record[372], 1);
  if ( Record[373] )
    this->Update_Stat( 2u, Record[373], 1);
  if ( Record[374] )
    this->Update_Stat( 3u, Record[374], 1);
  v6 = Record + 375;
  for ( j = 0; j < 3; ++j )
  {
    if ( v6[j] )
      this->Update_Stat( j + 76, v6[j], 1);
  }
  for ( j = 0; j < 8; ++j )
  {
    if ( Record[j + 378] )
    {
      v18 = CPlayer::s_SkillIndexPerMastery;
      dwNewCum = (int)Record[j + 378] / CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum;
      v19 = CPlayer::s_SkillIndexPerMastery;
      if ( (int)Record[j + 378] % CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum )
        ++dwNewCum;
      for ( k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k )
      {
        v11 = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
        if ( (int)dwNewCum > 0 )
          this->Update_Stat( v11 + 4, dwNewCum, 1);
      }
    }
  }
  v6 = Record + 386;
  for ( j = 0; j < 24; ++j )
  {
    if ( v6[j] )
      this->Update_Stat( j + 52, v6[j], 1);
  }
  for ( j = 0; j < 9; ++j )
  {
    Str1 = (char *)&Record[17 * j + 411];
    if ( !strncmp(Str1, "-1", 2uLL) )
      continue;
    nTableCode = GetItemTableCode(Str1);
    if ( nTableCode == -1 )
    {
      g_Main.m_logSystemError.Write( aAe, this->m_AvatorData.dbAvator.m_szClassCode, (unsigned int)j, Str1);
      continue;
    }
    if ( nTableCode == 19 )
    {
      g_Main.m_logSystemError.Write( aAe_0, this->m_AvatorData.dbAvator.m_szClassCode, (unsigned int)j, Str1);
      continue;
    }
    v14 = (unsigned __int16 *)g_Main.m_tblItemData[nTableCode].GetRecord( Str1);
    if ( !v14 )
    {
      g_Main.m_logSystemError.Write( aAe_1, this->m_AvatorData.dbAvator.m_szClassCode, (unsigned int)j, Str1);
      continue;
    }
    dwExp = *((unsigned int *)Str1 + 16);
    ItemKindCode = GetItemKindCode(nTableCode);
    if ( !ItemKindCode || ItemKindCode == 1 )
    {
      unsigned int dwLv = 0;
      if ( ItemKindCode == 1 )
      {
        dwLv = GetMaxParamFromExp(*reinterpret_cast<unsigned int *>(v14), static_cast<unsigned int>(dwExp));
      }
      else
      {
        dwLv = 0xFFFFFFF;
      }

      pItem.Init();
      pItem.m_byTableCode = nTableCode;
      pItem.m_wItemIndex = *v14;
      pItem.m_dwDur = static_cast<unsigned int>(dwExp);
      pItem.m_dwLv = dwLv;
      this->Update_ItemAdd( 0, j, &pItem, 1);
      continue;
    }
  }
  this->Update_Level( 1u, 0.0);
  this->m_AvatorData.dbAvator.m_fStartPos[1] = -65535.0f;
  return 1;
}

char CUserDB::DataValidCheckRevise(_AVATOR_DATA *pData, bool *pDataUpdated)
{
  __int64 *v2; // rdi
  __int64 i; // rcx
  int RecordNum; // eax
  unsigned int byTableCode; // ecx
  unsigned __int8 Index; // al
  unsigned __int8 v7; // al
  int v8; // eax
  int v9; // eax
  __int64 v11; // [rsp+0h] [rbp-128h] BYREF
  __int64 v12; // [rsp+20h] [rbp-108h]
  int j; // [rsp+30h] [rbp-F8h]
  _INVENKEY *p_Key; // [rsp+38h] [rbp-F0h]
  char v15; // [rsp+40h] [rbp-E8h]
  int k; // [rsp+44h] [rbp-E4h]
  _UNIT_DB_BASE::_LIST *v17; // [rsp+48h] [rbp-E0h]
  _EQUIPKEY *v18; // [rsp+50h] [rbp-D8h]
  unsigned char v19[48]; // [rsp+68h] [rbp-C0h] BYREF
  _EMBELLKEY *v20; // [rsp+98h] [rbp-90h]
  _FORCEKEY *v21; // [rsp+A0h] [rbp-88h]
  _ANIMUSKEY *v22; // [rsp+A8h] [rbp-80h]
  _UNIT_DB_BASE::_LIST *v23; // [rsp+B0h] [rbp-78h]
  char v24; // [rsp+B8h] [rbp-70h]
  int m; // [rsp+BCh] [rbp-6Ch]
  _INVENKEY *v26; // [rsp+C0h] [rbp-68h]
  _QUEST_DB_BASE::_LIST *v27; // [rsp+C8h] [rbp-60h]
  _base_fld *Record; // [rsp+D0h] [rbp-58h]
  int n; // [rsp+D8h] [rbp-50h]
  char *v30; // [rsp+E0h] [rbp-48h]
  _INVENKEY *v31; // [rsp+E8h] [rbp-40h]
  _worlddb_sf_delay_info::_eff_list *v32; // [rsp+F0h] [rbp-38h]
  _worlddb_sf_delay_info::_mas_list *v33; // [rsp+F8h] [rbp-30h]
  int wItemIndex; // [rsp+108h] [rbp-20h]
  int wIndex; // [rsp+10Ch] [rbp-1Ch]
  int v36; // [rsp+110h] [rbp-18h]
  if ( pDataUpdated )
    *pDataUpdated = 0;
  if ( pData->dbAvator.m_byBagNum > 5u )
  {
    CUserDB::s_logAvatorDB.Write(
      "%d > DataValidCheckRevise BAG (%d) (ADJUST %d)",
      pData->dbAvator.m_dwRecordNum,
      pData->dbAvator.m_byBagNum,
      5);
    pData->dbAvator.m_byBagNum = 5;
    if ( pDataUpdated )
      *pDataUpdated = 1;
  }
  if ( pData->dbAvator.m_dwDalant > 0x77359400 )
  {
    CUserDB::s_logAvatorDB.Write(
      "%d > DataValidCheckRevise DALANT (%u) (ADJUST %d)",
      pData->dbAvator.m_dwRecordNum,
      pData->dbAvator.m_dwDalant,
      2000000000);
    pData->dbAvator.m_dwDalant = 2000000000;
    if ( pDataUpdated )
      *pDataUpdated = 1;
  }
  if ( pData->dbAvator.m_dwGold > 0x7A120 )
  {
    CUserDB::s_logAvatorDB.Write(
      "%d > DataValidCheckRevise GOLD (%u) (ADJUST %d)",
      pData->dbAvator.m_dwRecordNum,
      pData->dbAvator.m_dwGold,
      500000);
    pData->dbAvator.m_dwGold = 500000;
    if ( pDataUpdated )
      *pDataUpdated = 1;
  }
  if ( !pData->dbAvator.m_byLevel )
  {
    if ( pData->dbAvator.m_dwDalant )
    {
      v12 = 0LL;
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise 0 Lv DALANT (%u) (ADJUST 0)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbAvator.m_dwDalant);
      pData->dbAvator.m_dwDalant = 0;
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  if ( !pData->dbAvator.m_byLevel )
  {
    if ( pData->dbAvator.m_dwGold )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise 0 Lv GOLD (%u) (ADJUST 0)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbAvator.m_dwGold);
      pData->dbAvator.m_dwGold = 0;
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  if ( pData->dbTrunk.bySlotNum )
  {
    if ( pData->dbTrunk.dDalant > 1000000000.0 )
    {
      v12 = 1000000000;
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise Trunk DALANT (%f) (ADJUST %d)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbTrunk.dDalant,
        v12);
      pData->dbTrunk.dDalant = 1000000000.0;
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
    if ( pData->dbTrunk.dGold > 500000.0 )
    {
      v12 = 500000;
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise Trunk GOLD (%f) (ADJUST %d)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbTrunk.dGold,
        v12);
      pData->dbTrunk.dGold = 500000.0;
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  for ( j = 0; j < 20 * pData->dbAvator.m_byBagNum; ++j )
  {
    _INVEN_DB_BASE::_LIST *invenEntry = &pData->dbInven.m_List[j];
    p_Key = &invenEntry->Key;
    if ( !p_Key->IsFilled() )
      continue;
    if ( p_Key->byTableCode >= 0x25u )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise INVEN.. TABLE (%d) (DEL)",
        pData->dbAvator.m_dwRecordNum,
        p_Key->byTableCode);
      p_Key->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
      continue;
    }
    wItemIndex = p_Key->wItemIndex;
    RecordNum = g_Main.m_tblItemData[p_Key->byTableCode].GetRecordNum();
    if ( wItemIndex >= RecordNum )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise INVEN.. INDEX (%d : %d) (DEL)",
        pData->dbAvator.m_dwRecordNum,
        p_Key->byTableCode,
        p_Key->wItemIndex);
      p_Key->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
      continue;
    }
    if ( IsOverLapItem(p_Key->byTableCode) )
    {
      if ( !invenEntry->dwDur )
      {
        CUserDB::s_logAvatorDB.Write(
          "%d > DataValidCheckRevise INVEN.. AMOUNT: 0, (%d : %d) (DEL)",
          pData->dbAvator.m_dwRecordNum,
          p_Key->byTableCode,
          p_Key->wItemIndex);
        p_Key->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
        continue;
      }
      if ( invenEntry->dwDur > 0x63uLL )
      {
        CUserDB::s_logAvatorDB.Write(
          "%d > DataValidCheckRevise INVEN.. AMOUNT: %d, (%d : %d) (ADJUST)",
          pData->dbAvator.m_dwRecordNum,
          static_cast<unsigned int>(invenEntry->dwDur),
          p_Key->byTableCode,
          p_Key->wItemIndex);
        invenEntry->dwDur = 99;
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
    if ( p_Key->byTableCode != 19 )
      goto LABEL_208;
    if ( (int)pData->dbAvator.m_byRaceSexCode >> 1 )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise INVEN.. UNIT KEY (race %d) (ADJUST)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbAvator.m_byRaceSexCode / 2);
      p_Key->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
      continue;
    }
    v15 = 0;
    for ( k = 0; k < 4; ++k )
    {
      v17 = &pData->dbUnit.m_List[k];
      if ( pData->dbUnit.m_List[k].byFrame != 255 && invenEntry->dwUpt == static_cast<unsigned int>(k) )
      {
        v15 = 1;
        break;
      }
    }
    if ( v15 )
    {
LABEL_208:
      if ( !IsSaveItem(p_Key->byTableCode) )
      {
        CUserDB::s_logAvatorDB.Write( "%d > NO SAVE ITEM (INVEN) (DEL)", pData->dbAvator.m_dwRecordNum);
        p_Key->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
    else
    {
      CUserDB::s_logAvatorDB.Write( "%d > NOTHING UNIT (KEY DEL)", pData->dbAvator.m_dwRecordNum);
      p_Key->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  for ( j = 0; j < 8; ++j )
  {
    v18 = &pData->dbAvator.m_EquipKey[j];
    if ( v18->IsFilled() && !g_Main.m_tblItemData[j].GetRecord( v18->zItemIndex) )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise EQUIP.. INDEX (%d_%d) (DEL)",
        pData->dbAvator.m_dwRecordNum,
        j,
        v18->zItemIndex);
      v18->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  memset_0(v19, 0, 0x25uLL);
  for ( j = 0; j < 7; ++j )
  {
    v20 = &pData->dbEquip.m_EmbellishList[j].Key;
    if ( v20->IsFilled() )
    {
      if ( v20->byTableCode == 10 || v20->byTableCode == 8 || v20->byTableCode == 9 )
      {
        if ( g_Main.m_tblItemData[v20->byTableCode].GetRecord( v20->wItemIndex) )
        {
          ++v19[v20->byTableCode];
        }
        else
        {
          CUserDB::s_logAvatorDB.Write(
            "%d > DataValidCheckRevise EMBELL.. TABLE (%d) (DEL)",
            pData->dbAvator.m_dwRecordNum,
            v20->byTableCode);
          byTableCode = v20->byTableCode;
          v12 = v20->wItemIndex;
          CUserDB::s_logAvatorDB.Write( aDDatavalidchec_21, pData->dbAvator.m_dwRecordNum, byTableCode, v12);
          v20->SetRelease();
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
      else
      {
        CUserDB::s_logAvatorDB.Write(
          "%d > DataValidCheckRevise EMBELL.. TABLE (%d) (DEL)",
          pData->dbAvator.m_dwRecordNum,
          v20->byTableCode);
        v20->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
  }
  for ( j = 0; j < 37; ++j )
  {
    if ( j == 10 )
    {
      if ( v19[10] > 3u )
      {
        v12 = (unsigned __int8)v19[j];
        CUserDB::s_logAvatorDB.Write(
          aDDatavalidchec_22,
          pData->dbAvator.m_dwRecordNum,
          (unsigned int)j,
          v12);
      }
    }
    else if ( (unsigned __int8)v19[j] > 2u )
    {
      v12 = (unsigned __int8)v19[j];
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_23, pData->dbAvator.m_dwRecordNum, (unsigned int)j, v12);
    }
  }
  for ( j = 0; j < 88; ++j )
  {
    v21 = &pData->dbForce.m_List[j].Key;
    if ( v21->IsFilled() )
    {
      Index = v21->GetIndex();
      if ( !g_Main.m_tblItemData[15].GetRecord( Index) )
      {
        v7 = v21->GetIndex();
        CUserDB::s_logAvatorDB.Write( aDDatavalidchec_24, pData->dbAvator.m_dwRecordNum, v7);
        v21->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
  }
  for ( j = 0; j < 4; ++j )
  {
    v22 = &pData->dbAnimus.m_List[j].Key;
    if ( v22->IsFilled() && !g_Main.m_tblItemData[24].GetRecord( v22->byItemIndex) )
    {
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_25, pData->dbAvator.m_dwRecordNum, v22->byItemIndex);
      v22->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  if ( !((int)pData->dbAvator.m_byRaceSexCode >> 1) )
  {
    for ( j = 0; j < 4; ++j )
    {
      v23 = &pData->dbUnit.m_List[j];
      if ( pData->dbUnit.m_List[j].byFrame != 255 )
      {
        v24 = 0;
        for ( m = 0; m < 20 * pData->dbAvator.m_byBagNum; ++m )
        {
          _INVEN_DB_BASE::_LIST *unitInvenEntry = &pData->dbInven.m_List[m];
          v26 = &unitInvenEntry->Key;
          if ( v26->IsFilled() && v26->byTableCode == 19 && unitInvenEntry->dwUpt == v23->bySlotIndex )
          {
            v24 = 1;
            break;
          }
        }
        if ( !v24 )
        {
          v23->byFrame = -1;
          CUserDB::s_logAvatorDB.Write( aDDDatavalidche, pData->dbAvator.m_dwRecordNum, (unsigned int)j);
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
    }
  }
  for ( j = 0; j < 30; ++j )
  {
    v27 = &pData->dbQuest.m_List[j];
    if ( v27->byQuestType != 255 )
    {
      if ( v27->byQuestType < 3u )
      {
        wIndex = v27->wIndex;
        v8 = CQuestMgr::s_tblQuest->GetRecordNum();
        if ( wIndex < v8 )
        {
          Record = CQuestMgr::s_tblQuest->GetRecord( v27->wIndex);
          for ( n = 0; n < 3; ++n )
          {
            if ( v27->wNum[n] != 0xFFFF )
            {
              v30 = &Record[4 * n + 1].m_strCode[32];
              if ( *reinterpret_cast<int *>(v30) == -1 )
              {
                v27->wNum[n] = -1;
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
              else if ( v27->wNum[n] >= *((int *)v30 + 49) )
              {
                v27->wNum[n] = -1;
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
            }
          }
        }
        else
        {
          v12 = v27->wIndex;
          CUserDB::s_logAvatorDB.Write(
            aDDDatavalidche_1,
            pData->dbAvator.m_dwRecordNum,
            (unsigned int)j,
            v12);
          v27->byQuestType = -1;
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
      else
      {
        v12 = v27->byQuestType;
        CUserDB::s_logAvatorDB.Write( aDDDatavalidche_0, pData->dbAvator.m_dwRecordNum, (unsigned int)j, v12);
        v27->byQuestType = -1;
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
  }
  j = 0;
  while ( 2 )
  {
    if ( j < pData->dbTrunk.bySlotNum )
    {
      _TRUNK_DB_BASE::_LIST *trunkEntry = &pData->dbTrunk.m_List[j];
      v31 = &trunkEntry->Key;
      if ( v31->IsFilled() )
      {
        if ( v31->byTableCode < 0x25u )
        {
          v36 = v31->wItemIndex;
          v9 = g_Main.m_tblItemData[v31->byTableCode].GetRecordNum();
          if ( v36 < v9 )
          {
            if ( IsOverLapItem(v31->byTableCode) )
            {
              if ( !trunkEntry->dwDur )
              {
                CUserDB::s_logAvatorDB.Write(
                  "%d > DataValidCheckRevise TRUNK.. AMOUNT: 0, (%d : %d) (DEL)",
                  pData->dbAvator.m_dwRecordNum,
                  v31->byTableCode,
                  v31->wItemIndex);
                v31->SetRelease();
                if ( pDataUpdated )
                  *pDataUpdated = 1;
                goto LABEL_166;
              }
              if ( trunkEntry->dwDur > 0x63uLL )
              {
                CUserDB::s_logAvatorDB.Write(
                  "%d > DataValidCheckRevise TRUNK.. AMOUNT: %d, (%d : %d) (ADJUST)",
                  pData->dbAvator.m_dwRecordNum,
                  static_cast<unsigned int>(trunkEntry->dwDur),
                  v31->byTableCode,
                  v31->wItemIndex);
                trunkEntry->dwDur = 99;
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
            }
            if ( trunkEntry->byRace < 3u )
            {
              if ( !IsSaveItem(v31->byTableCode) )
              {
                CUserDB::s_logAvatorDB.Write(
                  "%d > NO SAVE ITEM (TRUNK) (DEL)",
                  pData->dbAvator.m_dwRecordNum);
                v31->SetRelease();
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
            }
            else
            {
              CUserDB::s_logAvatorDB.Write(
                "%d > DataValidCheckRevise TRUNK.. RACE (%d) (DEL)",
                pData->dbAvator.m_dwRecordNum,
                trunkEntry->byRace);
              v31->SetRelease();
              if ( pDataUpdated )
                *pDataUpdated = 1;
            }
            goto LABEL_166;
          }
          CUserDB::s_logAvatorDB.Write(
            "%d > DataValidCheckRevise TRUNK.. INDEX (%d : %d) (DEL)",
            pData->dbAvator.m_dwRecordNum,
            v31->byTableCode,
            v31->wItemIndex);
          v31->SetRelease();
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
        else
        {
          CUserDB::s_logAvatorDB.Write(
            "%d > DataValidCheckRevise TRUNK.. TABLE (%d) (DEL)",
            pData->dbAvator.m_dwRecordNum,
            v31->byTableCode);
          v31->SetRelease();
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
LABEL_166:
      ++j;
      continue;
    }
    break;
  }
  for ( j = 0; j < 10; ++j )
  {
    v32 = &pData->dbSFDelay.EFF[j];
    if ( v32->byEffectCode != 255
      && (v32->byEffectCode >= 4u
       || !g_Main.m_tblEffectData[v32->byEffectCode].GetRecord( v32->wEffectIndex)) )
    {
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_26, v32->byEffectCode, v32->wEffectIndex);
      v32->byEffectCode = -1;
    }
    v33 = &pData->dbSFDelay.MAS[j];
    if ( v33->byEffectCode != 255 && v33->byEffectCode >= 4u )
    {
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_27, v33->byEffectCode, v33->byMastery);
      v33->byEffectCode = -1;
    }
  }
  return 1;
}

char CUserDB::UpdateContUserSave(bool bDirect)
{
  if ( this->m_bDBWaitState )
    return 0;

  DWORD timeNow = timeGetTime();
  if ( !bDirect )
  {
    if ( timeNow - this->m_dwTermContSaveTime < 0x493E0 )
      return 0;
    this->m_dwTermContSaveTime = timeNow;
    if ( !this->m_bDataUpdate && timeNow - this->m_dwLastContSaveTime < 0x927C0 )
      return 0;
  }

  _qry_case_contsave pQryData{};
  if ( this->m_AvatorData.m_byHSKTime <= 2u && this->m_AvatorData.m_byCristalBattleDBInfo == 1 )
  {
    this->m_AvatorData.m_byCristalBattleDBInfo = 3;
    this->m_AvatorData.m_byHSKTime = static_cast<unsigned __int8>(-1);
  }

  pQryData.dwAvatorSerial = this->m_dwSerial;
  memcpy_0(&pQryData.NewData, &this->m_AvatorData, sizeof(pQryData.NewData));
  memcpy_0(&pQryData.OldData, &this->m_AvatorData_bk, sizeof(pQryData.OldData));
  pQryData.bUpdateRefineCnt = 0;

  unsigned int wIndex = this->m_idWorld.wIndex;
  if ( g_Main.m_pRFEvent_ClassRefine->IsDbUpdate(wIndex) )
  {
    _event_participant_classrefine *state =
      g_Main.m_pRFEvent_ClassRefine->GetPlayerState(wIndex, this->m_idWorld.dwSerial);
    if ( state )
    {
      pQryData.bUpdateRefineCnt = 1;
      pQryData.byRefinedCnt = state->nCurRefineCnt;
      pQryData.dwRefineDate = state->dwRefineDate;
    }
  }

  int nSize = static_cast<int>(pQryData.size());
  _DB_QRY_SYN_DATA *pushData = g_Main.PushDQSData(
    this->m_dwAccountSerial,
    &this->m_idWorld,
    0xCu,
    reinterpret_cast<char *>(&pQryData),
    nSize);

  if ( pushData )
  {
    this->m_bDBWaitState = 1;
    this->m_pDBPushData = pushData;
    this->m_AvatorData.PostUpdateInit();
    this->m_bDataUpdate = 0;
    this->m_dwLastContSaveTime = timeNow;
    return 1;
  }

  return 0;
}

#if 0 // duplicate implementation exists in CUserDB.cpp
CUserDB *SearchAvatorWithName(CUserDB *pList, int nMax, char *pwszName)
{
  if (!pList || !pwszName)
  {
    return nullptr;
  }

  const unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(pwszName));
  for (int j = 0; j < nMax; ++j)
  {
    CUserDB *user = &pList[j];
    if (user->m_bActive && user->m_bField && user->m_byNameLen == nameLen &&
        !strncmp(user->m_wszAvatorName, pwszName, nameLen))
    {
      return user;
    }
  }
  return nullptr;
}
#endif

char CUserDB::Setting_Class(char *pszClassCode)
{
  if (!g_Main.m_tblClass.GetRecord(pszClassCode))
  {
    return 0;
  }

  strcpy_0(this->m_AvatorData.dbAvator.m_szClassCode, pszClassCode);
  this->m_bDataUpdate = 1;
  return 1;
}

char CUserDB::Update_PlayTime(unsigned int dwTotalTimeMin)
{
  this->m_AvatorData.dbAvator.m_dwTotalPlayMin = dwTotalTimeMin;
  return 1;
}

void CUserDB::TotalPlayMinCheck()
{
  if (this->m_dwSerial != static_cast<unsigned int>(-1) && this->m_tmrCheckPlayMin.CountingTimer())
  {
    Update_PlayTime(this->m_AvatorData.dbAvator.m_dwTotalPlayMin + 1);
  }
}

void CUserDB::OnLoop_Static()
{
  CUserDB::s_MgrLobbyHistory.OnLoop();
}
