#include "pch.h"

#include "WorldServerUtil.h"

#include "CUserDB.h"
#include "CPlayer.h"
#include "CLogFile.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "class_fld.h"
#include "GlobalObjects.h"

#include <cstring>

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

bool CUserDB::FirstSettingData()
{
  _class_fld *classRecord; // [rsp+30h] [rbp-B8h]
  int j; // [rsp+48h] [rbp-A0h]
  unsigned int dwNewCum; // [rsp+4Ch] [rbp-9Ch]
  int k; // [rsp+50h] [rbp-98h]
  int skillStatIndex; // [rsp+54h] [rbp-94h]
  char *Str1; // [rsp+58h] [rbp-90h]
  int nTableCode; // [rsp+60h] [rbp-88h]
  unsigned __int16 *itemRecord; // [rsp+68h] [rbp-80h]
  unsigned __int64 dwExp; // [rsp+70h] [rbp-78h]
  unsigned __int8 ItemKindCode; // [rsp+78h] [rbp-70h]
  _STORAGE_LIST::_db_con pItem; // [rsp+88h] [rbp-60h] BYREF
  if ( !this->m_bActive )
    return 0;
  classRecord = (_class_fld *)g_Main.m_tblClass.GetRecord( this->m_AvatorData.dbAvator.m_szClassCode);
  if ( !classRecord )
    return 0;
  for ( j = 0; j < 2; ++j )
  {
    if ( classRecord->m_MasteryLim.m_nBnsMMastery[j] )
      this->Update_Stat( j, classRecord->m_MasteryLim.m_nBnsMMastery[j], 1);
  }
  if ( classRecord->m_MasteryLim.m_nBnsSMastery )
    this->Update_Stat( 0x4Fu, classRecord->m_MasteryLim.m_nBnsSMastery, 1);
  if ( classRecord->m_MasteryLim.m_nBnsDefMastery )
    this->Update_Stat( 2u, classRecord->m_MasteryLim.m_nBnsDefMastery, 1);
  if ( classRecord->m_MasteryLim.m_nBnsPryMastery )
    this->Update_Stat( 3u, classRecord->m_MasteryLim.m_nBnsPryMastery, 1);
  for ( j = 0; j < 3; ++j )
  {
    if ( classRecord->m_MasteryLim.m_nBnsMakeMastery[j] )
      this->Update_Stat( j + 76, classRecord->m_MasteryLim.m_nBnsMakeMastery[j], 1);
  }
  for ( j = 0; j < 8; ++j )
  {
    if ( classRecord->m_MasteryLim.m_nBnsSkillMastery[j] )
    {
      dwNewCum = classRecord->m_MasteryLim.m_nBnsSkillMastery[j] / CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum;
      if ( classRecord->m_MasteryLim.m_nBnsSkillMastery[j] % CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum )
        ++dwNewCum;
      for ( k = 0; k < CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndexNum; ++k )
      {
        skillStatIndex = CPlayer::s_SkillIndexPerMastery[j].m_nSkillIndex[k];
        if ( (int)dwNewCum > 0 )
          this->Update_Stat( skillStatIndex + 4, dwNewCum, 1);
      }
    }
  }
  for ( j = 0; j < 24; ++j )
  {
    if ( classRecord->m_MasteryLim.m_nBnsForceMastery[j] )
      this->Update_Stat( j + 52, classRecord->m_MasteryLim.m_nBnsForceMastery[j], 1);
  }
  for ( j = 0; j < 9; ++j )
  {
    Str1 = classRecord->m_DefaultItem[j].strDefaultItem;
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
    itemRecord = (unsigned __int16 *)g_Main.m_tblItemData[nTableCode].GetRecord( Str1);
    if ( !itemRecord )
    {
      g_Main.m_logSystemError.Write( aAe_1, this->m_AvatorData.dbAvator.m_szClassCode, (unsigned int)j, Str1);
      continue;
    }
    dwExp = classRecord->m_DefaultItem[j].nAmount;
    ItemKindCode = GetItemKindCode(nTableCode);
    if ( !ItemKindCode || ItemKindCode == 1 )
    {
      unsigned int dwLv = 0;
      if ( ItemKindCode == 1 )
      {
        dwLv = GetMaxParamFromExp(*reinterpret_cast<unsigned int *>(itemRecord), static_cast<unsigned int>(dwExp));
      }
      else
      {
        dwLv = 0xFFFFFFF;
      }

      pItem.Init();
      pItem.m_byTableCode = nTableCode;
      pItem.m_wItemIndex = *itemRecord;
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
  int recordCount; // eax
  unsigned int tableCode; // ecx
  unsigned __int8 itemIndex; // al
  unsigned __int8 forceItemIndex; // al
  int questRecordCount; // eax
  int trunkRecordCount; // eax
  __int64 logAdjustValue; // [rsp+20h] [rbp-108h]
  int j; // [rsp+30h] [rbp-F8h]
  _INVENKEY *invenKey; // [rsp+38h] [rbp-F0h]
  char hasLinkedUnit; // [rsp+40h] [rbp-E8h]
  int k; // [rsp+44h] [rbp-E4h]
  _UNIT_DB_BASE::_LIST *unitEntry; // [rsp+48h] [rbp-E0h]
  _EQUIPKEY *equipKey; // [rsp+50h] [rbp-D8h]
  unsigned char embellishCountByTableCode[48]; // [rsp+68h] [rbp-C0h] BYREF
  _EMBELLKEY *embellishKey; // [rsp+98h] [rbp-90h]
  _FORCEKEY *forceKey; // [rsp+A0h] [rbp-88h]
  _ANIMUSKEY *animusKey; // [rsp+A8h] [rbp-80h]
  _UNIT_DB_BASE::_LIST *unitSlot; // [rsp+B0h] [rbp-78h]
  char hasUnitKeyInInven; // [rsp+B8h] [rbp-70h]
  int m; // [rsp+BCh] [rbp-6Ch]
  _INVENKEY *unitInvenKey; // [rsp+C0h] [rbp-68h]
  _QUEST_DB_BASE::_LIST *questEntry; // [rsp+C8h] [rbp-60h]
  _Quest_fld *questRecord; // [rsp+D0h] [rbp-58h]
  int n; // [rsp+D8h] [rbp-50h]
  _INVENKEY *trunkKey; // [rsp+E8h] [rbp-40h]
  _worlddb_sf_delay_info::_eff_list *effectDelayEntry; // [rsp+F0h] [rbp-38h]
  _worlddb_sf_delay_info::_mas_list *masteryDelayEntry; // [rsp+F8h] [rbp-30h]
  int wItemIndex; // [rsp+108h] [rbp-20h]
  int wIndex; // [rsp+10Ch] [rbp-1Ch]
  int trunkItemIndex; // [rsp+110h] [rbp-18h]
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
      logAdjustValue = 0LL;
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
      logAdjustValue = 1000000000;
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise Trunk DALANT (%f) (ADJUST %d)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbTrunk.dDalant,
        logAdjustValue);
      pData->dbTrunk.dDalant = 1000000000.0;
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
    if ( pData->dbTrunk.dGold > 500000.0 )
    {
      logAdjustValue = 500000;
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise Trunk GOLD (%f) (ADJUST %d)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbTrunk.dGold,
        logAdjustValue);
      pData->dbTrunk.dGold = 500000.0;
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  for ( j = 0; j < 20 * pData->dbAvator.m_byBagNum; ++j )
  {
    _INVEN_DB_BASE::_LIST *invenEntry = &pData->dbInven.m_List[j];
    invenKey = &invenEntry->Key;
    if ( !invenKey->IsFilled() )
      continue;
    if ( invenKey->byTableCode >= 0x25u )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise INVEN.. TABLE (%d) (DEL)",
        pData->dbAvator.m_dwRecordNum,
        invenKey->byTableCode);
      invenKey->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
      continue;
    }
    wItemIndex = invenKey->wItemIndex;
    recordCount = g_Main.m_tblItemData[invenKey->byTableCode].GetRecordNum();
    if ( wItemIndex >= recordCount )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise INVEN.. INDEX (%d : %d) (DEL)",
        pData->dbAvator.m_dwRecordNum,
        invenKey->byTableCode,
        invenKey->wItemIndex);
      invenKey->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
      continue;
    }
    if ( IsOverLapItem(invenKey->byTableCode) )
    {
      if ( !invenEntry->dwDur )
      {
        CUserDB::s_logAvatorDB.Write(
          "%d > DataValidCheckRevise INVEN.. AMOUNT: 0, (%d : %d) (DEL)",
          pData->dbAvator.m_dwRecordNum,
          invenKey->byTableCode,
          invenKey->wItemIndex);
        invenKey->SetRelease();
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
          invenKey->byTableCode,
          invenKey->wItemIndex);
        invenEntry->dwDur = 99;
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
    if ( invenKey->byTableCode != 19 )
      goto LABEL_208;
    if ( (int)pData->dbAvator.m_byRaceSexCode >> 1 )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise INVEN.. UNIT KEY (race %d) (ADJUST)",
        pData->dbAvator.m_dwRecordNum,
        pData->dbAvator.m_byRaceSexCode / 2);
      invenKey->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
      continue;
    }
    hasLinkedUnit = 0;
    for ( k = 0; k < 4; ++k )
    {
      unitEntry = &pData->dbUnit.m_List[k];
      if ( pData->dbUnit.m_List[k].byFrame != 255 && invenEntry->dwUpt == static_cast<unsigned int>(k) )
      {
        hasLinkedUnit = 1;
        break;
      }
    }
    if ( hasLinkedUnit )
    {
LABEL_208:
      if ( !IsSaveItem(invenKey->byTableCode) )
      {
        CUserDB::s_logAvatorDB.Write( "%d > NO SAVE ITEM (INVEN) (DEL)", pData->dbAvator.m_dwRecordNum);
        invenKey->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
    else
    {
      CUserDB::s_logAvatorDB.Write( "%d > NOTHING UNIT (KEY DEL)", pData->dbAvator.m_dwRecordNum);
      invenKey->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  for ( j = 0; j < 8; ++j )
  {
    equipKey = &pData->dbAvator.m_EquipKey[j];
    if ( equipKey->IsFilled() && !g_Main.m_tblItemData[j].GetRecord( equipKey->zItemIndex) )
    {
      CUserDB::s_logAvatorDB.Write(
        "%d > DataValidCheckRevise EQUIP.. INDEX (%d_%d) (DEL)",
        pData->dbAvator.m_dwRecordNum,
        j,
        equipKey->zItemIndex);
      equipKey->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  memset_0(embellishCountByTableCode, 0, 0x25uLL);
  for ( j = 0; j < 7; ++j )
  {
    embellishKey = &pData->dbEquip.m_EmbellishList[j].Key;
    if ( embellishKey->IsFilled() )
    {
      if ( embellishKey->byTableCode == 10 || embellishKey->byTableCode == 8 || embellishKey->byTableCode == 9 )
      {
        if ( g_Main.m_tblItemData[embellishKey->byTableCode].GetRecord( embellishKey->wItemIndex) )
        {
          ++embellishCountByTableCode[embellishKey->byTableCode];
        }
        else
        {
          CUserDB::s_logAvatorDB.Write(
            "%d > DataValidCheckRevise EMBELL.. TABLE (%d) (DEL)",
            pData->dbAvator.m_dwRecordNum,
            embellishKey->byTableCode);
          tableCode = embellishKey->byTableCode;
          logAdjustValue = embellishKey->wItemIndex;
          CUserDB::s_logAvatorDB.Write( aDDatavalidchec_21, pData->dbAvator.m_dwRecordNum, tableCode, logAdjustValue);
          embellishKey->SetRelease();
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
      else
      {
        CUserDB::s_logAvatorDB.Write(
          "%d > DataValidCheckRevise EMBELL.. TABLE (%d) (DEL)",
          pData->dbAvator.m_dwRecordNum,
          embellishKey->byTableCode);
        embellishKey->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
  }
  for ( j = 0; j < 37; ++j )
  {
    if ( j == 10 )
    {
      if ( embellishCountByTableCode[10] > 3u )
      {
        logAdjustValue = (unsigned __int8)embellishCountByTableCode[j];
        CUserDB::s_logAvatorDB.Write(
          aDDatavalidchec_22,
          pData->dbAvator.m_dwRecordNum,
          (unsigned int)j,
          logAdjustValue);
      }
    }
    else if ( (unsigned __int8)embellishCountByTableCode[j] > 2u )
    {
      logAdjustValue = (unsigned __int8)embellishCountByTableCode[j];
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_23, pData->dbAvator.m_dwRecordNum, (unsigned int)j, logAdjustValue);
    }
  }
  for ( j = 0; j < 88; ++j )
  {
    forceKey = &pData->dbForce.m_List[j].Key;
    if ( forceKey->IsFilled() )
    {
      itemIndex = forceKey->GetIndex();
      if ( !g_Main.m_tblItemData[15].GetRecord( itemIndex) )
      {
        forceItemIndex = forceKey->GetIndex();
        CUserDB::s_logAvatorDB.Write( aDDatavalidchec_24, pData->dbAvator.m_dwRecordNum, forceItemIndex);
        forceKey->SetRelease();
        if ( pDataUpdated )
          *pDataUpdated = 1;
      }
    }
  }
  for ( j = 0; j < 4; ++j )
  {
    animusKey = &pData->dbAnimus.m_List[j].Key;
    if ( animusKey->IsFilled() && !g_Main.m_tblItemData[24].GetRecord( animusKey->byItemIndex) )
    {
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_25, pData->dbAvator.m_dwRecordNum, animusKey->byItemIndex);
      animusKey->SetRelease();
      if ( pDataUpdated )
        *pDataUpdated = 1;
    }
  }
  if ( !((int)pData->dbAvator.m_byRaceSexCode >> 1) )
  {
    for ( j = 0; j < 4; ++j )
    {
      unitSlot = &pData->dbUnit.m_List[j];
      if ( pData->dbUnit.m_List[j].byFrame != 255 )
      {
        hasUnitKeyInInven = 0;
        for ( m = 0; m < 20 * pData->dbAvator.m_byBagNum; ++m )
        {
          _INVEN_DB_BASE::_LIST *unitInvenEntry = &pData->dbInven.m_List[m];
          unitInvenKey = &unitInvenEntry->Key;
          if ( unitInvenKey->IsFilled() && unitInvenKey->byTableCode == 19 && unitInvenEntry->dwUpt == unitSlot->bySlotIndex )
          {
            hasUnitKeyInInven = 1;
            break;
          }
        }
        if ( !hasUnitKeyInInven )
        {
          unitSlot->byFrame = -1;
          CUserDB::s_logAvatorDB.Write( aDDDatavalidche, pData->dbAvator.m_dwRecordNum, (unsigned int)j);
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
    }
  }
  for ( j = 0; j < 30; ++j )
  {
    questEntry = &pData->dbQuest.m_List[j];
    if ( questEntry->byQuestType != 255 )
    {
      if ( questEntry->byQuestType < 3u )
      {
        wIndex = questEntry->wIndex;
        questRecordCount = CQuestMgr::s_tblQuest->GetRecordNum();
        if ( wIndex < questRecordCount )
        {
          questRecord = (_Quest_fld *)CQuestMgr::s_tblQuest->GetRecord( questEntry->wIndex);
          for ( n = 0; n < 3; ++n )
          {
            if ( questEntry->wNum[n] != 0xFFFF )
            {
              const _action_node *actionNode = &questRecord->m_ActionNode[n];
              if ( actionNode->m_nActType == -1 )
              {
                questEntry->wNum[n] = -1;
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
              else if ( questEntry->wNum[n] >= actionNode->m_nReqAct )
              {
                questEntry->wNum[n] = -1;
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
            }
          }
        }
        else
        {
          logAdjustValue = questEntry->wIndex;
          CUserDB::s_logAvatorDB.Write(
            aDDDatavalidche_1,
            pData->dbAvator.m_dwRecordNum,
            (unsigned int)j,
            logAdjustValue);
          questEntry->byQuestType = -1;
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
      }
      else
      {
        logAdjustValue = questEntry->byQuestType;
        CUserDB::s_logAvatorDB.Write( aDDDatavalidche_0, pData->dbAvator.m_dwRecordNum, (unsigned int)j, logAdjustValue);
        questEntry->byQuestType = -1;
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
      trunkKey = &trunkEntry->Key;
      if ( trunkKey->IsFilled() )
      {
        if ( trunkKey->byTableCode < 0x25u )
        {
          trunkItemIndex = trunkKey->wItemIndex;
          trunkRecordCount = g_Main.m_tblItemData[trunkKey->byTableCode].GetRecordNum();
          if ( trunkItemIndex < trunkRecordCount )
          {
            if ( IsOverLapItem(trunkKey->byTableCode) )
            {
              if ( !trunkEntry->dwDur )
              {
                CUserDB::s_logAvatorDB.Write(
                  "%d > DataValidCheckRevise TRUNK.. AMOUNT: 0, (%d : %d) (DEL)",
                  pData->dbAvator.m_dwRecordNum,
                  trunkKey->byTableCode,
                  trunkKey->wItemIndex);
                trunkKey->SetRelease();
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
                  trunkKey->byTableCode,
                  trunkKey->wItemIndex);
                trunkEntry->dwDur = 99;
                if ( pDataUpdated )
                  *pDataUpdated = 1;
              }
            }
            if ( trunkEntry->byRace < 3u )
            {
              if ( !IsSaveItem(trunkKey->byTableCode) )
              {
                CUserDB::s_logAvatorDB.Write(
                  "%d > NO SAVE ITEM (TRUNK) (DEL)",
                  pData->dbAvator.m_dwRecordNum);
                trunkKey->SetRelease();
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
              trunkKey->SetRelease();
              if ( pDataUpdated )
                *pDataUpdated = 1;
            }
            goto LABEL_166;
          }
          CUserDB::s_logAvatorDB.Write(
            "%d > DataValidCheckRevise TRUNK.. INDEX (%d : %d) (DEL)",
            pData->dbAvator.m_dwRecordNum,
            trunkKey->byTableCode,
            trunkKey->wItemIndex);
          trunkKey->SetRelease();
          if ( pDataUpdated )
            *pDataUpdated = 1;
        }
        else
        {
          CUserDB::s_logAvatorDB.Write(
            "%d > DataValidCheckRevise TRUNK.. TABLE (%d) (DEL)",
            pData->dbAvator.m_dwRecordNum,
            trunkKey->byTableCode);
          trunkKey->SetRelease();
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
    effectDelayEntry = &pData->dbSFDelay.EFF[j];
    if ( effectDelayEntry->byEffectCode != 255
      && (effectDelayEntry->byEffectCode >= 4u
       || !g_Main.m_tblEffectData[effectDelayEntry->byEffectCode].GetRecord( effectDelayEntry->wEffectIndex)) )
    {
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_26, effectDelayEntry->byEffectCode, effectDelayEntry->wEffectIndex);
      effectDelayEntry->byEffectCode = -1;
    }
    masteryDelayEntry = &pData->dbSFDelay.MAS[j];
    if ( masteryDelayEntry->byEffectCode != 255 && masteryDelayEntry->byEffectCode >= 4u )
    {
      CUserDB::s_logAvatorDB.Write( aDDatavalidchec_27, masteryDelayEntry->byEffectCode, masteryDelayEntry->byMastery);
      masteryDelayEntry->byEffectCode = -1;
    }
  }
  return 1;
}
