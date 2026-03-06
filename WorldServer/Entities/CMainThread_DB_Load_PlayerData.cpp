#include "pch.h"

#include "CMainThread.h"

#include "CCheckSumCharacAccountTrunkData.h"

#include <cstring>
#include <cstdio>
#include <new>
#include <mmsystem.h>

namespace
{
_EMBELLKEY EmbellKeyFromDb(int key)
{
  _EMBELLKEY decoded{};
  decoded.bySlotIndex = static_cast<unsigned __int8>(key & 0xFF);
  decoded.byTableCode = static_cast<unsigned __int8>((key >> 8) & 0xFF);
  decoded.wItemIndex = static_cast<unsigned __int16>((key >> 16) & 0xFFFF);
  return decoded;
}

_COMBINEKEY CombineKeyFromDb(int key)
{
  _COMBINEKEY decoded{};
  memcpy_0(&decoded, &key, sizeof(key));
  return decoded;
}
} // namespace

unsigned __int8 CMainThread::_db_Check_NpcData(unsigned int dwSerial, _AVATOR_DATA *pAvatorData)
{
  if (!this->m_bCheckSumActive)
  {
    return 0;
  }

  const unsigned __int8 race = pAvatorData->dbAvator.m_byRaceSexCode / 2;
  CCheckSumCharacAccountTrunkData srcValue(
    dwSerial,
    pAvatorData->dbAvator.m_dwAccountSerial,
    race);
  CCheckSumCharacAccountTrunkData dbValue(
    dwSerial,
    pAvatorData->dbAvator.m_dwAccountSerial,
    race);

  srcValue.Encode(pAvatorData);
  int result = dbValue.Load(this->m_pWorldDB, &srcValue);
  if (result < 0)
  {
    return 24;
  }

  result = dbValue.CheckDiff(this->m_pWorldDB, pAvatorData->dbAvator.m_wszAvatorName, &srcValue);
  if (result < 0)
  {
    return 24;
  }
  if (result > 0)
  {
    return 40;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Base(unsigned int dwSerial, _AVATOR_DATA *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-138h] BYREF
  _worlddb_character_base_info pCharacterData; // [rsp+30h] [rbp-108h] BYREF
  unsigned __int8 v8; // [rsp+114h] [rbp-24h]
  int j; // [rsp+118h] [rbp-20h]

  v3 = &v6;
  memset_0(&pCharacterData, 0, sizeof(pCharacterData));
  v8 = this->m_pWorldDB->Select_CharacterBaseInfo(dwSerial, &pCharacterData);
  if ( v8 == 1 )
    return 24;
  if ( v8 == 2 )
    return 24;
  strcpy_0(pCon->dbAvator.m_wszAvatorName, pCharacterData.wszName);
  pCon->dbAvator.m_dwRecordNum = dwSerial;
  pCon->dbAvator.m_byRaceSexCode = pCharacterData.byRace;
  strcpy_0(pCon->dbAvator.m_szClassCode, pCharacterData.szClassCode);
  pCon->dbAvator.m_bySlotIndex = pCharacterData.bySlotIndex;
  pCon->dbAvator.m_byLevel = pCharacterData.byLevel;
  pCon->dbAvator.m_dwDalant = pCharacterData.dwDalant;
  pCon->dbAvator.m_dwGold = pCharacterData.dwGold;
  pCon->dbAvator.m_dwBaseShape = pCharacterData.dwBaseShape;
  pCon->dbAvator.m_dwLastConnTime = pCharacterData.dwLastConnTime;
  for ( j = 0; j < 8; ++j )
  {
    pCon->dbAvator.m_EquipKey[j].LoadDBKey(pCharacterData.shEKArray[j]);
    pCon->dbAvator.m_dwFixEquipLv[j] = pCharacterData.dwEUArray[j];
    pCon->dbAvator.m_lnUID[j] = pCharacterData.lnUIDArray[j];
    pCon->dbAvator.m_dwET[j] = pCharacterData.dwETArray[j];
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Buddy(unsigned int dwSerial, _BUDDY_DB_BASE *pBuddy)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-508h] BYREF
  _worlddb_buddy_info pBuddyData; // [rsp+30h] [rbp-4D8h] BYREF
  unsigned __int8 v8; // [rsp+4E4h] [rbp-24h]
  char v9; // [rsp+4E5h] [rbp-23h]
  int j; // [rsp+4E8h] [rbp-20h]

  v3 = &v6;
  memset_0(&pBuddyData, 0, sizeof(pBuddyData));
  v8 = this->m_pWorldDB->Select_Buddy(dwSerial, &pBuddyData);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_Buddy(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_Buddy(dwSerial, &pBuddyData) )
    return 24;
LABEL_11:
  v9 = 0;
  for ( j = 0; j < 50; ++j )
  {
    pBuddy->m_List[j].dwSerial = pBuddyData.BuddyData[j].dwSerial;
    if ( pBuddyData.BuddyData[j].dwSerial != -1 )
      strcpy_0(pBuddy->m_List[j].wszName, pBuddyData.BuddyData[j].wszName);
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_CryMsg(unsigned int dwSerial, _CRYMSG_DB_BASE *pBossCry)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-2E8h] BYREF
  _worlddb_crymsg_info pCryMsg; // [rsp+30h] [rbp-2B8h] BYREF
  unsigned __int8 v8; // [rsp+2C4h] [rbp-24h]
  int j; // [rsp+2C8h] [rbp-20h]

  v3 = &v6;
  memset_0(&pCryMsg, 0, sizeof(pCryMsg));
  v8 = this->m_pWorldDB->Select_BossCryMsg(dwSerial, &pCryMsg);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_BossCryRecord(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_BossCryMsg(dwSerial, &pCryMsg) )
    return 24;
LABEL_11:
  for ( j = 0; j < 10; ++j )
    strcpy_0(pBossCry->m_List[j].wszCryMsg, pCryMsg.CryMsgData[j].wszCryMsg);
  return 0;
}

unsigned __int8 CMainThread::_db_Load_General(
  unsigned int dwSerial,
  unsigned __int8 byRaceCode,
  _AVATOR_DATA *pCon)
{
  char parseBuffer[160];
  _worlddb_character_general_info characterData;
  char leftResListBuffer[212];
  int leftResLength = 0;
  int leftResOffset = 0;

  memset(parseBuffer, 0, 128);
  memset_0(parseBuffer, 0, 0x80uLL);
  memset_0(&characterData, 0, sizeof(characterData));

  const unsigned __int8 dbResult = this->m_pWorldDB->Select_CharacterGeneralInfo(dwSerial, &characterData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
    return 37;

  pCon->dbAvator.m_dwHP = characterData.dwHP;
  pCon->dbAvator.m_dwFP = characterData.dwFP;
  pCon->dbAvator.m_dwSP = characterData.dwSP;
  pCon->dbAvator.m_dwDP = characterData.dwDP;
  pCon->dbAvator.m_dExp = characterData.dExp;
  pCon->dbAvator.m_dLossExp = characterData.dLoseExp;
  pCon->dbAvator.m_byBagNum = characterData.byBagNum;
  pCon->dbAvator.m_byMapCode = characterData.byMapCode;
  pCon->dbAvator.m_fStartPos[0] = characterData.fStartPos[0];
  pCon->dbAvator.m_fStartPos[1] = characterData.fStartPos[1];
  pCon->dbAvator.m_fStartPos[2] = characterData.fStartPos[2];
  pCon->dbAvator.m_dwTotalPlayMin = characterData.dwTotalPlayMin;

  strcpy_0(leftResListBuffer, characterData.szLeftResList);
  pCon->dbCutting.Init();
  if ( leftResListBuffer[0] != 42 )
  {
    leftResLength = static_cast<int>(strlen_0(leftResListBuffer));
    if ( !(leftResLength % 5) )
    {
      pCon->dbCutting.m_byLeftNum = static_cast<unsigned __int8>(leftResLength / 5);
      leftResOffset = 0;
      for ( int leftResIndex = 0; leftResIndex < pCon->dbCutting.m_byLeftNum; ++leftResIndex )
      {
        memcpy_0(parseBuffer, &leftResListBuffer[leftResOffset], 2uLL);
        parseBuffer[2] = 0;
        leftResOffset += 2;
        pCon->dbCutting.m_List[leftResIndex].Key.byTableCode = 18;
        pCon->dbCutting.m_List[leftResIndex].Key.wItemIndex = static_cast<unsigned __int16>(atoi(parseBuffer));
        memcpy_0(parseBuffer, &leftResListBuffer[leftResOffset], 3uLL);
        parseBuffer[3] = 0;
        leftResOffset += 3;
        pCon->dbCutting.m_List[leftResIndex].dwDur = static_cast<unsigned int>(atoi(parseBuffer));
      }
      pCon->dbCutting.m_bOldDataLoad = 1;
    }
  }

  for ( int index = 0; index < 7; ++index )
  {
    _EMBELLKEY embellKey = EmbellKeyFromDb(characterData.lEK[index]);
    pCon->dbEquip.m_EmbellishList[index].Key.LoadDBKey(embellKey);
    pCon->dbEquip.m_EmbellishList[index].wAmount = characterData.wED[index];
    pCon->dbEquip.m_EmbellishList[index].dwT = characterData.dwET[index];
    pCon->dbEquip.m_EmbellishList[index].lnUID = characterData.lnUID_E[index];
  }

  for ( int index = 0; index < 88; ++index )
  {
    pCon->dbForce.m_List[index].Key.dwKey = characterData.lF[index];
    pCon->dbForce.m_List[index].lnUID = characterData.lnUID_F[index];
  }

  if ( byRaceCode == 1 )
  {
    for ( int index = 0; index < 4; ++index )
    {
      pCon->dbAnimus.m_List[index].Key.LoadDBKey(characterData.byAK[index]);
      pCon->dbAnimus.m_List[index].dwExp = characterData.dwAD[index];
      pCon->dbAnimus.m_List[index].dwParam = characterData.dwAP[index];
      pCon->dbAnimus.m_List[index].lnUID = characterData.lnUID_A[index];
    }
  }

  for ( int index = 0; index < 2; ++index )
  {
    sprintf_s(parseBuffer, sizeof(parseBuffer), "WM%d", index);
    pCon->dbStat.m_dwDamWpCnt[index] = characterData.dwWM[index];
  }

  for ( int index = 0; index < 24; ++index )
  {
    sprintf_s(parseBuffer, sizeof(parseBuffer), "FM%d", index);
    pCon->dbStat.m_dwForceCum[index] = characterData.dwFM[index];
  }

  for ( int index = 0; index < 48; ++index )
  {
    sprintf_s(parseBuffer, sizeof(parseBuffer), "SM%d", index);
    pCon->dbStat.m_dwSkillCum[index] = characterData.dwSM[index];
  }

  for ( int index = 0; index < 3; ++index )
  {
    sprintf_s(parseBuffer, sizeof(parseBuffer), "MI%d", index);
    pCon->dbStat.m_dwMakeCum[index] = characterData.dwMI[index];
  }

  pCon->dbStat.m_dwSpecialCum = characterData.dwSR;
  pCon->dbStat.m_dwDefenceCnt = characterData.dwDM;
  pCon->dbStat.m_dwShieldCnt = characterData.dwPM;

  for ( int index = 0; index < 3; ++index )
    pCon->dbAvator.m_zClassHistory[index] = characterData.zClassHistory[index];

  pCon->dbAvator.m_dwClassInitCnt = characterData.dwClassInitCnt;
  pCon->dbAvator.m_byLastClassGrade = characterData.byLastClassGrade;
  pCon->dbAvator.m_dPvPPoint = characterData.dPvPPoint;
  pCon->dbAvator.m_dPvPCashBag = characterData.dPvPCashBag;
  memcpy_0(
    pCon->dbAvator.m_szBindMapCode,
    characterData.szBindMapCode,
    sizeof(pCon->dbAvator.m_szBindMapCode));
  pCon->dbAvator.m_szBindMapCode[11] = 0;
  memcpy_0(
    pCon->dbAvator.m_szBindDummy,
    characterData.szBindDummy,
    sizeof(pCon->dbAvator.m_szBindDummy));
  pCon->dbAvator.m_szBindDummy[11] = 0;
  pCon->dbAvator.m_dwGuildSerial = characterData.dwGuildSerial;
  pCon->dbAvator.m_byClassInGuild = characterData.byGuildGrade;
  pCon->dbAvator.m_dwRadarDelayTime = characterData.dwRadarDelayTime;
  pCon->dbAvator.m_dwTakeLastMentalTicket = characterData.dwTakeLastMentalTicket;
  pCon->dbAvator.m_dwTakeLastCriTicket = characterData.dwTakeLastCriTicket;
  pCon->dbAvator.m_byMaxLevel = characterData.byMaxLevel;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Inven(
  unsigned int dwSerial,
  unsigned __int8 byBagNum,
  _INVEN_DB_BASE *pCon)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-CC8h] BYREF
  _worlddb_inven_info pInvenData; // [rsp+30h] [rbp-C98h] BYREF
  unsigned __int8 v9; // [rsp+CB4h] [rbp-14h]
  int j; // [rsp+CB8h] [rbp-10h]

  v4 = &v7;
  memset_0(&pInvenData, 0, sizeof(pInvenData));
  v9 = this->m_pWorldDB->Select_Inven(dwSerial, byBagNum, &pInvenData);
  if ( v9 == 1 )
    return 24;
  if ( v9 == 2 )
    return 37;
  for ( j = 0; j < 100; ++j )
  {
    if ( j >= 20 * static_cast<int>(byBagNum) )
    {
      pCon->m_List[j].Key.SetRelease();
    }
    else
    {
      pCon->m_List[j].Key.LoadDBKey(pInvenData.invenKey[j].lK);
      pCon->m_List[j].dwDur = pInvenData.invenKey[j].dwD;
      pCon->m_List[j].dwUpt = pInvenData.invenKey[j].dwU;
      pCon->m_List[j].dwT = pInvenData.invenKey[j].dwT;
      pCon->m_List[j].lnUID = pInvenData.invenKey[j].lnUID;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_ItemCombineEx(unsigned int dwSerial, _ITEMCOMBINE_DB_BASE *pCombineEx)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-188h] BYREF
  _worlddb_itemcombineex_info pdbItemCombineExInfo; // [rsp+30h] [rbp-158h] BYREF
  unsigned __int8 v8; // [rsp+174h] [rbp-14h]
  int j; // [rsp+178h] [rbp-10h]

  v3 = &v6;
  memset_0(&pdbItemCombineExInfo, 0, sizeof(pdbItemCombineExInfo));
  v8 = this->m_pWorldDB->Select_ItemCombineEx(dwSerial, &pdbItemCombineExInfo);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_ItemCombineEx(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_ItemCombineEx(dwSerial, &pdbItemCombineExInfo) )
    return 24;
LABEL_11:
  pCombineEx->Init();
  pCombineEx->m_bIsResult = pdbItemCombineExInfo.bIsResult;
  if ( !pCombineEx->m_bIsResult )
    return 0;
  pCombineEx->m_dwCheckKey = pdbItemCombineExInfo.dwCheckKey;
  pCombineEx->m_byDlgType = pdbItemCombineExInfo.byDlgType;
  pCombineEx->m_dwDalant = pdbItemCombineExInfo.dwDalant;
  pCombineEx->m_byItemListNum = pdbItemCombineExInfo.byItemListCount;
  pCombineEx->m_bySelectItemCount = pdbItemCombineExInfo.byItemSelectCount;
  for ( j = 0; j < 24; ++j )
  {
    if ( j >= pdbItemCombineExInfo.byItemListCount )
    {
      pCombineEx->m_List[j].Key.SetRelease();
    }
    else
    {
      _COMBINEKEY combineKey = CombineKeyFromDb(pdbItemCombineExInfo.invenKey[j].lK);
      pCombineEx->m_List[j].Key.LoadDBKey(combineKey);
      pCombineEx->m_List[j].dwDur = pdbItemCombineExInfo.invenKey[j].dwD;
      pCombineEx->m_List[j].dwUpt = pdbItemCombineExInfo.invenKey[j].dwU;
    }
  }
  pCombineEx->m_dwResultEffectType = pdbItemCombineExInfo.dwResultEffectType;
  pCombineEx->m_dwResultEffectMsgCode = pdbItemCombineExInfo.dwResultEffectMsgCode;
  return 0;
}

unsigned __int8 CMainThread::_db_load_losebattlecount(
        unsigned int dwSerial,
        _AVATOR_DATA *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-48h] BYREF
  int v7; // [rsp+20h] [rbp-28h]
  unsigned int *dwCount; // [rsp+28h] [rbp-20h]
  unsigned int *v9; // [rsp+30h] [rbp-18h]
  unsigned int *v10; // [rsp+38h] [rbp-10h]

  v3 = &v6;
  dwCount = pCon->dbAvator.m_dwRaceBattleRecord;
  v7 = this->m_pWorldDB->Select_WinBattleCount(
         pCon->dbAvator.m_byRaceSexCode / 2,
         dwSerial,
         pCon->dbAvator.m_dwRaceBattleRecord);
  if ( v7 == 1 )
    return 24;
  v9 = &pCon->dbAvator.m_dwRaceBattleRecord[1];
  v7 = this->m_pWorldDB->Select_FailBattleCount(
         pCon->dbAvator.m_byRaceSexCode / 2,
         dwSerial,
         &pCon->dbAvator.m_dwRaceBattleRecord[1]);
  if ( v7 == 1 )
    return 24;
  v10 = &pCon->dbAvator.m_dwRaceBattleRecord[2];
  v7 = this->m_pWorldDB->Select_LoseBattleCount(
         pCon->dbAvator.m_byRaceSexCode / 2,
         dwSerial,
         &pCon->dbAvator.m_dwRaceBattleRecord[2]);
  if ( v7 == 1 )
    return 24;
  else
    return 0;
}

unsigned __int8 CMainThread::_db_Load_MacroData(
        unsigned int dwSerial,
        _AIOC_A_MACRODATA *pMacro)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v7; // [rsp+20h] [rbp-18h]
  int j; // [rsp+24h] [rbp-14h]

  v3 = &v6;
  v7 = 0;
  memset_0(pMacro, 0, sizeof(_AIOC_A_MACRODATA));
  v7 = this->m_pWorldDB->Select_MacroData(dwSerial, pMacro);
  if ( v7 == 2 )
  {
    if ( !this->m_pWorldDB->Insert_MacroData( dwSerial) )
      return 24;
    for ( j = 0; j < 1; ++j )
    {
      pMacro->mcr_Potion[j].Potion[0] = -1;
      pMacro->mcr_Potion[j].Potion[1] = -1;
      pMacro->mcr_Potion[j].Potion[2] = -1;
      pMacro->mcr_Potion[j].PotionValue[0] = 0;
      pMacro->mcr_Potion[j].PotionValue[1] = 0;
      pMacro->mcr_Potion[j].PotionValue[2] = 0;
    }
    for ( j = 0; j < 3; ++j )
    {
      pMacro->mcr_Action[j].Action[0] = -1;
      pMacro->mcr_Action[j].Action[1] = -1;
      pMacro->mcr_Action[j].Action[2] = -1;
      pMacro->mcr_Action[j].Action[3] = -1;
      pMacro->mcr_Action[j].Action[4] = -1;
      pMacro->mcr_Action[j].Action[5] = -1;
      pMacro->mcr_Action[j].Action[6] = -1;
      pMacro->mcr_Action[j].Action[7] = -1;
      pMacro->mcr_Action[j].Action[8] = -1;
      pMacro->mcr_Action[j].Action[9] = -1;
    }
    for ( j = 0; j < 2; ++j )
    {
      memset_0(&pMacro->mcr_Chat[j], 0, 0x100uLL);
      memset_0(pMacro->mcr_Chat[j].Chat[1], 0, sizeof(pMacro->mcr_Chat[j].Chat[1]));
      memset_0(pMacro->mcr_Chat[j].Chat[2], 0, sizeof(pMacro->mcr_Chat[j].Chat[2]));
      memset_0(pMacro->mcr_Chat[j].Chat[3], 0, sizeof(pMacro->mcr_Chat[j].Chat[3]));
      memset_0(pMacro->mcr_Chat[j].Chat[4], 0, sizeof(pMacro->mcr_Chat[j].Chat[4]));
    }
  }
  else if ( v7 == 1 )
  {
    return 24;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_NpcQuest_History(
        unsigned int dwSerial,
        _QUEST_DB_BASE *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-4B8h] BYREF
  _worlddb_npc_quest_complete_history pNpcQHis; // [rsp+30h] [rbp-488h] BYREF
  unsigned __int8 v8; // [rsp+494h] [rbp-24h]
  int j; // [rsp+498h] [rbp-20h]

  v3 = &v6;
  v8 = this->m_pWorldDB->Select_NpcQuest_History(dwSerial, &pNpcQHis);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_NpcQuest_History(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_NpcQuest_History(dwSerial, &pNpcQHis) )
    return 24;
LABEL_11:
  for ( j = 0; j < 70; ++j )
  {
    strcpy_0(pCon->m_History[j].szQuestCode, pNpcQHis.List[j].szQuestCode);
    pCon->m_History[j].byLevel = pNpcQHis.List[j].byLevel;
    pCon->m_History[j].dwEventEndTime = pNpcQHis.List[j].dwEventEndTime;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_OreCutting(
        unsigned int dwSerial,
        _CUTTING_DB_BASE *pDbCutting)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-E8h] BYREF
  _worlddb_ore_cutting pOreCutting; // [rsp+30h] [rbp-B8h] BYREF
  char v8; // [rsp+D4h] [rbp-14h]
  int j; // [rsp+D8h] [rbp-10h]

  v3 = &v6;
  v8 = this->m_pWorldDB->Select_OreCutting(dwSerial, &pOreCutting);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_21;
  if ( !this->m_pWorldDB->Insert_OreCutting(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_OreCutting(dwSerial, &pOreCutting) )
    return 24;
LABEL_21:
  if ( pDbCutting->m_bOldDataLoad )
    return 0;
  pDbCutting->Init();
  for ( j = 0; j < 20; ++j )
  {
    if ( pOreCutting.List[j].nKey != -1 )
    {
      pDbCutting->m_List[j].Key.LoadDBKey(pOreCutting.List[j].nKey);
      pDbCutting->m_List[j].dwDur = pOreCutting.List[j].dwDur;
      ++pDbCutting->m_byLeftNum;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PcBangFavor(
        unsigned int dwSerial,
        _PCBANG_FAVOR_ITEM_DB_BASE *pDbPcBangFavor)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-1D8h] BYREF
  _worlddb_pcbang_favor_item pPcBangFavorItem; // [rsp+30h] [rbp-1A8h] BYREF
  char v8; // [rsp+1C4h] [rbp-14h]
  int j; // [rsp+1C8h] [rbp-10h]

  v3 = &v6;
  v8 = this->m_pWorldDB->Select_PcBangFavorItem(dwSerial, &pPcBangFavorItem);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_PcBangFavorItem(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_PcBangFavorItem(dwSerial, &pPcBangFavorItem) )
    return 24;
LABEL_11:
  for ( j = 0; j < 50; ++j )
    pDbPcBangFavor->lnUID[j] = pPcBangFavorItem.lnUID[j];
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PotionDelay(
        unsigned int dwSerial,
        _POTION_NEXT_USE_TIME_DB_BASE *pDbPotionDelay)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-E8h] BYREF
  _worlddb_potion_delay_info pPotionDelayInfo; // [rsp+30h] [rbp-B8h] BYREF
  char v8; // [rsp+D4h] [rbp-14h]
  DWORD Time; // [rsp+D8h] [rbp-10h]
  int j; // [rsp+DCh] [rbp-Ch]

  v3 = &v6;
  v8 = this->m_pWorldDB->Select_PotionDelay(dwSerial, &pPotionDelayInfo);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_PotionDelay(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_PotionDelay(dwSerial, &pPotionDelayInfo) )
    return 24;
LABEL_11:
  Time = timeGetTime();
  for ( j = 0; j < 38; ++j )
    pDbPotionDelay->dwPotionNextUseTime[j] = Time + pPotionDelayInfo.dwPotionDelay[j];
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PrimiumPlayTime(
        unsigned int dwAccSerial,
        _PCBANG_PLAY_TIME *kData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-38h] BYREF
  char v7; // [rsp+20h] [rbp-18h]

  v3 = &v6;
  v7 = this->m_pWorldDB->Select_PrimiumPlayTime(dwAccSerial, kData);
  if ( v7 == 1 )
    return 24;
  if ( v7 == 2 )
  {
    if ( !this->m_pWorldDB->Insert_PrimiumPlayTime(dwAccSerial) )
      return 24;
    kData->bForcedClose = 1;
    kData->dwContPlayTime = 0;
    kData->dwLastConnTime = 0;
  }
  kData->dwAccSerial = dwAccSerial;
  return 0;
}

unsigned __int8 CMainThread::_db_load_punishment(
        unsigned int dwSerial,
        _AVATOR_DATA *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-38h] BYREF
  int v7; // [rsp+20h] [rbp-18h]

  v3 = &v6;
  v7 = this->m_pWorldDB->Select_Punishment(
         dwSerial,
         pCon->dbAvator.m_dwElectSerial,
         pCon->dbAvator.m_dwPunishment);
  if ( v7 == 1 )
    return 24;
  else
    return 0;
}

unsigned __int8 CMainThread::_db_Load_PvpOrderView(
        unsigned int dwSerial,
        _PVP_ORDER_VIEW_DB_BASE *kData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-A8h] BYREF
  _pvporderview_info kInfo; // [rsp+30h] [rbp-78h] BYREF
  char v8; // [rsp+94h] [rbp-14h]
  int j; // [rsp+98h] [rbp-10h]

  v3 = &v6;
  memset_0(&kInfo, 0, sizeof(kInfo));
  v8 = this->m_pWorldDB->Select_PvpOrderViewInfo(dwSerial, &kInfo);
  if ( v8 == 1 )
    return 24;
  if ( v8 == 2 )
  {
    if ( this->m_pWorldDB->Insert_PvpOrderViewInfo(dwSerial) )
      return 0;
    else
      return 24;
  }
  else
  {
    kData->tUpdatedate = kInfo.tUpdatedate;
    kData->nDeath = kInfo.nDeath;
    kData->nKill = kInfo.nKill;
    kData->dTodayStacked = kInfo.dTodayStacked;
    kData->dPvpPoint = kInfo.dPvpPoint;
    kData->dPvpTempCash = kInfo.dPvpTempCash;
    kData->dPvpCash = kInfo.dPvpCash;
    for ( j = 0; j < 10; ++j )
      kData->dwKillerSerial[j] = kInfo.dwKillerSerial[j];
    kData->byContHaveCash = kInfo.byContHaveCash;
    kData->byContLoseCash = kInfo.byContLoseCash;
    kData->bRaceWarRecvr = kInfo.bRaceWarRecvr;
    return 0;
  }
}

unsigned __int8 CMainThread::_db_Load_PvpPointLimitData(
        unsigned int dwSerial,
        _PVPPOINT_LIMIT_DB_BASE *kData)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-68h] BYREF
  _pvppointlimit_info kInfo; // [rsp+28h] [rbp-40h] BYREF
  unsigned __int8 v8; // [rsp+54h] [rbp-14h]

  v3 = &v6;
  memset_0(&kInfo, 0, sizeof(kInfo));
  v8 = this->m_pWorldDB->Select_PvpPointLimitInfo(dwSerial, &kInfo);
  if ( v8 == 1 )
    return 24;
  if ( v8 == 2 )
  {
    if ( this->m_pWorldDB->Insert_PvpPointLimitInfoRecord(dwSerial) )
      return 0;
    else
      return 24;
  }
  else
  {
    kData->tUpdatedate = kInfo.tUpdatedate;
    kData->bUseUp = kInfo.bUseUp;
    kData->byLimitRate = kInfo.byLimitRate;
    kData->dOriginalPoint = kInfo.dOriginalPoint;
    kData->dLimitPoint = kInfo.dLimitPoint;
    kData->dUsePoint = kInfo.dUsePoint;
    return 0;
  }
}

unsigned __int8 CMainThread::_db_Load_Quest( unsigned int dwSerial, _QUEST_DB_BASE *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-238h] BYREF
  _worlddb_quest_array questData; // [rsp+30h] [rbp-208h] BYREF
  unsigned __int8 v8; // [rsp+214h] [rbp-24h]
  int j; // [rsp+218h] [rbp-20h]

  v3 = &v6;
  memset_0(&questData, 0, sizeof(questData));
  v8 = this->m_pWorldDB->Select_Quest(dwSerial, &questData);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_Quest(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_Quest(dwSerial, &questData) )
    return 24;
LABEL_11:
  for ( j = 0; j < 30; ++j )
  {
    pCon->m_List[j].byQuestType = questData.questList[j].byQuestType;
    pCon->m_List[j].wIndex = questData.questList[j].wIndex;
    pCon->m_List[j].dwPassSec = questData.questList[j].dwPassSec;
    pCon->m_List[j].wNum[0] = questData.questList[j].wNum[0];
    pCon->m_List[j].wNum[1] = questData.questList[j].wNum[1];
    pCon->m_List[j].wNum[2] = questData.questList[j].wNum[2];
  }
  return 0;
}

unsigned __int8 CMainThread::_db_load_raceboss( unsigned int dwSerial, _AVATOR_DATA *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-48h] BYREF
  int v7; // [rsp+20h] [rbp-28h]
  bool *bOverlapVote; // [rsp+28h] [rbp-20h]
  unsigned int *dwCnt; // [rsp+30h] [rbp-18h]

  v3 = &v6;
  bOverlapVote = &pCon->dbAvator.m_bOverlapVote;
  v7 = this->m_pWorldDB->Select_PatriarchVoted(
         pCon->dbAvator.m_byRaceSexCode / 2,
         dwSerial,
         &pCon->dbAvator.m_bOverlapVote);
  if ( v7 == 1 )
    return 24;
  dwCnt = &pCon->dbAvator.m_dwGivebackCount;
  v7 = this->m_pWorldDB->Select_PatriarchRefundCount(
         pCon->dbAvator.m_byRaceSexCode / 2,
         dwSerial,
         &pCon->dbAvator.m_dwGivebackCount);
  if ( v7 )
    pCon->dbAvator.m_dwGivebackCount = 0;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_SFDelayData(
        unsigned int dwSerial,
        _worlddb_sf_delay_info *pDbSFDelayInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-38h] BYREF
  unsigned __int8 v7; // [rsp+20h] [rbp-18h]

  v3 = &v6;
  v7 = this->m_pWorldDB->Select_SFDelayInfo(dwSerial, pDbSFDelayInfo);
  if ( v7 == 1 )
    return 24;
  if ( v7 != 2 || this->m_pWorldDB->Insert_SFDelayInfo(dwSerial, pDbSFDelayInfo) )
    return 0;
  return 24;
}

unsigned __int8 CMainThread::_db_Load_Start_NpcQuest_History(
  unsigned int dwSerial,
  unsigned __int8 byRaceCode,
  _QUEST_DB_BASE *pCon)
{
  _worlddb_start_npc_quest_complete_history pdwCount{};

  const unsigned __int64 entryCount = this->m_dwStartNPCQuestCnt[byRaceCode];
  pCon->m_StartHistory = new (std::nothrow) _QUEST_DB_BASE::_START_NPC_QUEST_HISTORY[entryCount];
  pdwCount.List = new (std::nothrow) _worlddb_start_npc_quest_complete_history::__list[entryCount];

  if (!pCon->m_StartHistory || !pdwCount.List)
  {
    return 24;
  }

  unsigned __int8 started =
    this->m_pWorldDB->Select_Start_NpcQuest_History_Count(dwSerial, &pdwCount.dwListCnt);
  if (started == 1)
  {
    return 24;
  }
  if (started == 2)
  {
    pdwCount.dwListCnt = 0;
    pCon->dwListCnt = 0;
  }
  pCon->dwListCnt = pdwCount.dwListCnt;

  started = this->m_pWorldDB->Select_Start_NpcQuest_History(dwSerial, &pdwCount, pdwCount.dwListCnt);
  if (started == 1)
  {
    return 24;
  }
  if (started != 2)
  {
    for (unsigned int j = 0; j < pdwCount.dwListCnt; ++j)
    {
      strcpy_0(pCon->m_StartHistory[j].szQuestCode, pdwCount.List[j].szQuestCode);
      pCon->m_StartHistory[j].byLevel = pdwCount.List[j].byLevel;
      pCon->m_StartHistory[j].nEndTime = pdwCount.List[j].nEndTime;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Supplement(
        unsigned int dwSerial,
        _SUPPLEMENT_DB_BASE *pDbSupplement)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  size_t v6; // rax
  __int64 v7; // [rsp+0h] [rbp-118h] BYREF
  _worlddb_character_supplement_info pSupplement; // [rsp+28h] [rbp-F0h] BYREF
  char v9; // [rsp+64h] [rbp-B4h]
  unsigned __int64 dwScanerCnt; // [rsp+68h] [rbp-B0h]
  char Buffer[88]; // [rsp+80h] [rbp-98h] BYREF
  char String[28]; // [rsp+D8h] [rbp-40h] BYREF
  int j; // [rsp+F4h] [rbp-24h]

  v3 = &v7;
  v9 = this->m_pWorldDB->Select_Supplement(dwSerial, &pSupplement);
  if ( v9 == 1 )
    return 24;
  if ( v9 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_Supplement(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_Supplement(dwSerial, &pSupplement) )
    return 24;
LABEL_11:
  pDbSupplement->dPvpPointLeak = pSupplement.dPvpPointLeak;
  pDbSupplement->bLastAttBuff = pSupplement.bLastAttBuff;
  pDbSupplement->dwBufPotionEndTime = pSupplement.dwBufPotionEndTime;
  pDbSupplement->dwRaceBuffClear = pSupplement.dwRaceBuffClear;
  if ( this->m_pWorldDB->Select_Supplement_Ex(dwSerial, &pSupplement) )
    return 24;
  pDbSupplement->byVoted = pSupplement.byVoted;
  pDbSupplement->dwAccumPlayTime = pSupplement.dwAccumPlayTime;
  pDbSupplement->dwLastResetDate = pSupplement.dwLastResetDate;
  pDbSupplement->VoteEnable = pSupplement.VoteEnable;
  dwScanerCnt = pSupplement.dwScanerCnt;
  if ( pSupplement.dwScanerCnt )
  {
    memset(Buffer, 0, 64);
    memset(String, 0, 10);
    sprintf_s(Buffer, 0x40uLL, "%I64u", dwScanerCnt);
    memcpy_0(String, Buffer, 9uLL);
    pDbSupplement->dwScanerGetDate = atoi(String);
    memset_0(String, 0, 0xAuLL);
    v6 = strlen_0(Buffer);
    memcpy_0(String, &Buffer[9], v6 - 9);
    pDbSupplement->wScanerCnt = atoi(String);
  }
  else
  {
    pDbSupplement->wScanerCnt = 0;
    pDbSupplement->dwScanerGetDate = 199001010;
  }
  if ( this->m_pWorldDB->Select_Supplement_ActPoint(dwSerial, &pSupplement) )
    return 24;
  for ( j = 0; j < 3; ++j )
    pDbSupplement->dwActionPoint[j] = pSupplement.dwActionPoint[j];
  return 0;
}

unsigned __int8 CMainThread::_db_Load_TimeLimitInfo(
        unsigned int dwAccSerial,
        _TIMELIMITINFO_DB_BASE *pDbTimeLimitInfo)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-58h] BYREF
  _worlddb_time_limit_info pTimeLimiInfo; // [rsp+28h] [rbp-30h] BYREF
  char v8; // [rsp+44h] [rbp-14h]

  v3 = &v6;
  v8 = this->m_pWorldDB->Select_PlayerTimeLimitInfo(dwAccSerial, &pTimeLimiInfo);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_PlayerTimeLimitInfo(dwAccSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_PlayerTimeLimitInfo(dwAccSerial, &pTimeLimiInfo) )
    return 24;
LABEL_11:
  pDbTimeLimitInfo->dwAccSerial = dwAccSerial;
  pDbTimeLimitInfo->dwFatigue = pTimeLimiInfo.dwFatigue;
  pDbTimeLimitInfo->byTLStatus = pTimeLimiInfo.byTLStatus;
  pDbTimeLimitInfo->dwLastLogoutTime = pTimeLimiInfo.dwLastLogoutTime;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Trade(
        unsigned __int8 byRace,
        unsigned int dwSerial,
        _TRADE_DB_BASE *pTrade)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-6B8h] BYREF
  _worlddb_trade_info pTradeData; // [rsp+40h] [rbp-678h] BYREF
  unsigned __int8 v9; // [rsp+694h] [rbp-24h]
  int j; // [rsp+698h] [rbp-20h]

  v4 = &v7;
  memset_0(&pTradeData, 0, sizeof(pTradeData));
  v9 = this->m_pWorldDB->Select_Trade(0, dwSerial, byRace, &pTradeData);
  if ( v9 == 1 )
    return 24;
  if ( v9 == 2 )
    return 0;
  for ( j = 0; j < 20; ++j )
  {
    pTrade->m_List[j].byState = pTradeData.list[j].byState;
    pTrade->m_List[j].dwRegistSerial = pTradeData.list[j].dwRegistSerial;
    pTrade->m_List[j].byInvenIndex = pTradeData.list[j].byInvenIndex;
    pTrade->m_List[j].dwPrice = pTradeData.list[j].dwPrice;
    pTrade->m_List[j].tStartTime = pTradeData.list[j].tStartTime;
    pTrade->m_List[j].bySellTurm = pTradeData.list[j].bySellTurm;
    pTrade->m_List[j].dwBuyerSerial = pTradeData.list[j].dwBuyerSerial;
    pTrade->m_List[j].dwTax = pTradeData.list[j].dwTax;
    pTrade->m_List[j].tResultTime = pTradeData.list[j].tResultTime;
    strcpy_s(pTrade->m_List[j].wszBuyerName, 0x11uLL, pTradeData.list[j].wszBuyerName);
    strcpy_s(pTrade->m_List[j].szBuyerAccount, 0xDuLL, pTradeData.list[j].szBuyerAccount);
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Trunk(
        unsigned int dwSerial,
        unsigned int dwAccountSerial,
        unsigned __int8 byRace,
        _TRUNK_DB_BASE *pTrunk)
{
  __int64 *v5; // rdi
  __int64 i; // rcx
  __int64 v8; // [rsp+0h] [rbp-1678h] BYREF
  _worlddb_trunk_info pTrunkData; // [rsp+30h] [rbp-1648h] BYREF
  unsigned __int8 v10; // [rsp+1654h] [rbp-24h]
  int j; // [rsp+1658h] [rbp-20h]
  unsigned __int8 v12; // [rsp+165Ch] [rbp-1Ch]

  v5 = &v8;
  memset_0(&pTrunkData, 0, sizeof(pTrunkData));
  v10 = this->m_pWorldDB->Select_AccountTrunk(dwAccountSerial, byRace, &pTrunkData);
  if ( v10 == 1 )
    return 24;
  if ( v10 == 2 )
  {
    if ( !this->m_pWorldDB->Insert_AccountTrunk(dwAccountSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_AccountTrunk(dwAccountSerial, byRace, &pTrunkData) )
      return 24;
  }
  pTrunk->bySlotNum = pTrunkData.bySlotNum;
  strcpy_0(pTrunk->wszPasswd, pTrunkData.wszPasswd);
  pTrunk->dDalant = pTrunkData.dDalant;
  pTrunk->dGold = pTrunkData.dGold;
  pTrunk->byHintIndex = pTrunkData.byHintIndex;
  strcpy_0(pTrunk->wszHintAnswer, pTrunkData.wszHintAnswer);
  for ( j = 0; j < 100; ++j )
  {
    if ( j >= pTrunkData.bySlotNum )
    {
      pTrunk->m_List[j].Key.SetRelease();
    }
    else
    {
      pTrunk->m_List[j].Key.LoadDBKey(pTrunkData.trunkKey[j].lK);
      pTrunk->m_List[j].dwDur = pTrunkData.trunkKey[j].dwD;
      pTrunk->m_List[j].dwUpt = pTrunkData.trunkKey[j].dwU;
      pTrunk->m_List[j].byRace = pTrunkData.trunkKey[j].byRace;
      pTrunk->m_List[j].dwT = pTrunkData.trunkKey[j].dwT;
      pTrunk->m_List[j].lnUID = pTrunkData.trunkKey[j].lnUID;
    }
  }
  v12 = this->m_pWorldDB->Select_AccountTrunkExtend(dwAccountSerial, &pTrunkData);
  if ( v12 == 1 )
    return 24;
  if ( v12 != 2 )
    goto LABEL_24;
  if ( !this->m_pWorldDB->Insert_AccountTrunkExtend(dwAccountSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_AccountTrunkExtend(dwAccountSerial, &pTrunkData) )
    return 24;
LABEL_24:
  pTrunk->byExtSlotNum = pTrunkData.byExtSlotNum;
  for ( j = 0; j < 40; ++j )
  {
    if ( j >= pTrunkData.byExtSlotNum )
    {
      pTrunk->m_ExtList[j].Key.SetRelease();
    }
    else
    {
      pTrunk->m_ExtList[j].Key.LoadDBKey(pTrunkData.ExttrunkKey[j].lK);
      pTrunk->m_ExtList[j].dwDur = pTrunkData.ExttrunkKey[j].dwD;
      pTrunk->m_ExtList[j].dwUpt = pTrunkData.ExttrunkKey[j].dwU;
      pTrunk->m_ExtList[j].byRace = pTrunkData.ExttrunkKey[j].byRace;
      pTrunk->m_ExtList[j].dwT = pTrunkData.ExttrunkKey[j].dwT;
      pTrunk->m_ExtList[j].lnUID = pTrunkData.ExttrunkKey[j].lnUID;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_UI(
        unsigned int dwSerial,
        _LINK_DB_BASE *pLink,
        _SFCONT_DB_BASE *pSfcont)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  __int64 v7; // [rsp+0h] [rbp-138h] BYREF
  _worlddb_userinterface_info pUserinterfaceInfo; // [rsp+30h] [rbp-108h] BYREF
  unsigned __int8 v9; // [rsp+114h] [rbp-24h]
  int j; // [rsp+118h] [rbp-20h]

  v4 = &v7;
  memset_0(&pUserinterfaceInfo, 0, sizeof(pUserinterfaceInfo));
  v9 = this->m_pWorldDB->Select_UserInterface(dwSerial, &pUserinterfaceInfo);
  if ( v9 == 1 )
    return 24;
  if ( v9 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_UserInterface(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_UserInterface(dwSerial, &pUserinterfaceInfo) )
    return 24;
LABEL_11:
  for ( j = 0; j < 50; ++j )
    pLink->m_LinkList[j].Key.wEffectCode = pUserinterfaceInfo.wLinkBoard[j];
  for ( j = 0; j < 8; ++j )
  {
    pSfcont->m_List[0][j].dwKey = pUserinterfaceInfo.dwDamageCForce[j];
    pSfcont->m_List[1][j].dwKey = pUserinterfaceInfo.dwHelpCForce[j];
  }
  memcpy_0(pLink->m_dwSkill, pUserinterfaceInfo.dwSkill, sizeof(pLink->m_dwSkill));
  memcpy_0(pLink->m_dwForce, pUserinterfaceInfo.dwForce, sizeof(pLink->m_dwForce));
  memcpy_0(pLink->m_dwCharacter, pUserinterfaceInfo.dwCharacter, sizeof(pLink->m_dwCharacter));
  memcpy_0(pLink->m_dwAnimus, pUserinterfaceInfo.dwAnimus, sizeof(pLink->m_dwAnimus));
  memcpy_0(pLink->m_dwInvenBag, pUserinterfaceInfo.dwInvenBag, sizeof(pLink->m_dwInvenBag));
  pLink->m_dwInven = pUserinterfaceInfo.dwInven;
  pLink->m_byLinkBoardLock = pUserinterfaceInfo.byLinkBoardLock;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Unit( unsigned int dwSerial, _UNIT_DB_BASE *pCon)
{
  __int64 *v3; // rdi
  __int64 i; // rcx
  __int64 v6; // [rsp+0h] [rbp-168h] BYREF
  _worlddb_unit_info_array pUnitInfo; // [rsp+30h] [rbp-138h] BYREF
  unsigned __int8 v8; // [rsp+144h] [rbp-24h]
  int j; // [rsp+148h] [rbp-20h]
  int k; // [rsp+14Ch] [rbp-1Ch]

  v3 = &v6;
  memset_0(&pUnitInfo, 0, sizeof(pUnitInfo));
  v8 = this->m_pWorldDB->Select_Unit(dwSerial, &pUnitInfo);
  if ( v8 == 1 )
    return 24;
  if ( v8 != 2 )
    goto LABEL_11;
  if ( !this->m_pWorldDB->Insert_Unit(dwSerial) )
    return 24;
  if ( this->m_pWorldDB->Select_Unit(dwSerial, &pUnitInfo) )
    return 24;
LABEL_11:
  for ( j = 0; j < 4; ++j )
  {
    pCon->m_List[j].byFrame = pUnitInfo.UnitInfo[j].byFrame;
    if ( pCon->m_List[j].byFrame != 255 )
    {
      pCon->m_List[j].byPart[0] = pUnitInfo.UnitInfo[j].byPart[0];
      pCon->m_List[j].byPart[1] = pUnitInfo.UnitInfo[j].byPart[1];
      pCon->m_List[j].byPart[2] = pUnitInfo.UnitInfo[j].byPart[2];
      pCon->m_List[j].byPart[3] = pUnitInfo.UnitInfo[j].byPart[3];
      pCon->m_List[j].byPart[4] = pUnitInfo.UnitInfo[j].byPart[4];
      pCon->m_List[j].byPart[5] = pUnitInfo.UnitInfo[j].byPart[5];
      pCon->m_List[j].dwGauge = pUnitInfo.UnitInfo[j].dwGauge;
      pCon->m_List[j].dwBullet[0] = pUnitInfo.UnitInfo[j].dwBullet[0];
      pCon->m_List[j].dwBullet[1] = pUnitInfo.UnitInfo[j].dwBullet[1];
      pCon->m_List[j].nPullingFee = pUnitInfo.UnitInfo[j].nPullingFee;
      pCon->m_List[j].dwCutTime = pUnitInfo.UnitInfo[j].dwCutTime;
      for ( k = 0; k < 8; ++k )
        pCon->m_List[j].dwSpare[k] = pUnitInfo.UnitInfo[j].dwSpare[k];
    }
  }
  return 0;
}
