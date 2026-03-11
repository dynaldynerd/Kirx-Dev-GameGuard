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
  std::memcpy(&decoded, &key, sizeof(key));
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
  _worlddb_character_base_info characterData;
  unsigned __int8 dbResult;

  std::memset(&characterData, 0, sizeof(characterData));
  dbResult = this->m_pWorldDB->Select_CharacterBaseInfo(dwSerial, &characterData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
    return 24;
  std::strcpy(pCon->dbAvator.m_wszAvatorName, characterData.wszName);
  pCon->dbAvator.m_dwRecordNum = dwSerial;
  pCon->dbAvator.m_byRaceSexCode = characterData.byRace;
  std::strcpy(pCon->dbAvator.m_szClassCode, characterData.szClassCode);
  pCon->dbAvator.m_bySlotIndex = characterData.bySlotIndex;
  pCon->dbAvator.m_byLevel = characterData.byLevel;
  pCon->dbAvator.m_dwDalant = characterData.dwDalant;
  pCon->dbAvator.m_dwGold = characterData.dwGold;
  pCon->dbAvator.m_dwBaseShape = characterData.dwBaseShape;
  pCon->dbAvator.m_dwLastConnTime = characterData.dwLastConnTime;
  for ( int equipIndex = 0; equipIndex < 8; ++equipIndex )
  {
    pCon->dbAvator.m_EquipKey[equipIndex].LoadDBKey(characterData.shEKArray[equipIndex]);
    pCon->dbAvator.m_dwFixEquipLv[equipIndex] = characterData.dwEUArray[equipIndex];
    pCon->dbAvator.m_lnUID[equipIndex] = characterData.lnUIDArray[equipIndex];
    pCon->dbAvator.m_dwET[equipIndex] = characterData.dwETArray[equipIndex];
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Buddy(unsigned int dwSerial, _BUDDY_DB_BASE *pBuddy)
{
  _worlddb_buddy_info buddyData;
  unsigned __int8 dbResult;

  std::memset(&buddyData, 0, sizeof(buddyData));
  dbResult = this->m_pWorldDB->Select_Buddy(dwSerial, &buddyData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_Buddy(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_Buddy(dwSerial, &buddyData) )
      return 24;
  }
  for ( int buddyIndex = 0; buddyIndex < 50; ++buddyIndex )
  {
    pBuddy->m_List[buddyIndex].dwSerial = buddyData.BuddyData[buddyIndex].dwSerial;
    if ( buddyData.BuddyData[buddyIndex].dwSerial != -1 )
      std::strcpy(pBuddy->m_List[buddyIndex].wszName, buddyData.BuddyData[buddyIndex].wszName);
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_CryMsg(unsigned int dwSerial, _CRYMSG_DB_BASE *pBossCry)
{
  _worlddb_crymsg_info cryMessageData;
  unsigned __int8 dbResult;

  std::memset(&cryMessageData, 0, sizeof(cryMessageData));
  dbResult = this->m_pWorldDB->Select_BossCryMsg(dwSerial, &cryMessageData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_BossCryRecord(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_BossCryMsg(dwSerial, &cryMessageData) )
      return 24;
  }
  for ( int cryIndex = 0; cryIndex < 10; ++cryIndex )
    std::strcpy(pBossCry->m_List[cryIndex].wszCryMsg, cryMessageData.CryMsgData[cryIndex].wszCryMsg);
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
  std::memset(parseBuffer, 0, 128);
  std::memset(&characterData, 0, sizeof(characterData));

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

  std::strcpy(leftResListBuffer, characterData.szLeftResList);
  pCon->dbCutting.Init();
  if ( leftResListBuffer[0] != '*' )
  {
    leftResLength = static_cast<int>(std::strlen(leftResListBuffer));
    if ( !(leftResLength % 5) )
    {
      pCon->dbCutting.m_byLeftNum = static_cast<unsigned __int8>(leftResLength / 5);
      leftResOffset = 0;
      for ( int leftResIndex = 0; leftResIndex < pCon->dbCutting.m_byLeftNum; ++leftResIndex )
      {
        std::memcpy(parseBuffer, &leftResListBuffer[leftResOffset], 2uLL);
        parseBuffer[2] = 0;
        leftResOffset += 2;
        pCon->dbCutting.m_List[leftResIndex].Key.byTableCode = 18;
        pCon->dbCutting.m_List[leftResIndex].Key.wItemIndex = static_cast<unsigned __int16>(atoi(parseBuffer));
        std::memcpy(parseBuffer, &leftResListBuffer[leftResOffset], 3uLL);
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
  std::memcpy(
    pCon->dbAvator.m_szBindMapCode,
    characterData.szBindMapCode,
    sizeof(pCon->dbAvator.m_szBindMapCode));
  pCon->dbAvator.m_szBindMapCode[11] = 0;
  std::memcpy(
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
  _worlddb_inven_info invenData;
  unsigned __int8 dbResult;

  std::memset(&invenData, 0, sizeof(invenData));
  dbResult = this->m_pWorldDB->Select_Inven(dwSerial, byBagNum, &invenData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
    return 37;
  for ( int itemIndex = 0; itemIndex < 100; ++itemIndex )
  {
    if ( itemIndex >= 20 * static_cast<int>(byBagNum) )
    {
      pCon->m_List[itemIndex].Key.SetRelease();
    }
    else
    {
      pCon->m_List[itemIndex].Key.LoadDBKey(invenData.invenKey[itemIndex].lK);
      pCon->m_List[itemIndex].dwDur = invenData.invenKey[itemIndex].dwD;
      pCon->m_List[itemIndex].dwUpt = invenData.invenKey[itemIndex].dwU;
      pCon->m_List[itemIndex].dwT = invenData.invenKey[itemIndex].dwT;
      pCon->m_List[itemIndex].lnUID = invenData.invenKey[itemIndex].lnUID;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_ItemCombineEx(unsigned int dwSerial, _ITEMCOMBINE_DB_BASE *pCombineEx)
{
  _worlddb_itemcombineex_info itemCombineExInfo;
  unsigned __int8 dbResult;

  std::memset(&itemCombineExInfo, 0, sizeof(itemCombineExInfo));
  dbResult = this->m_pWorldDB->Select_ItemCombineEx(dwSerial, &itemCombineExInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_ItemCombineEx(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_ItemCombineEx(dwSerial, &itemCombineExInfo) )
      return 24;
  }
  pCombineEx->Init();
  pCombineEx->m_bIsResult = itemCombineExInfo.bIsResult;
  if ( !pCombineEx->m_bIsResult )
    return 0;
  pCombineEx->m_dwCheckKey = itemCombineExInfo.dwCheckKey;
  pCombineEx->m_byDlgType = itemCombineExInfo.byDlgType;
  pCombineEx->m_dwDalant = itemCombineExInfo.dwDalant;
  pCombineEx->m_byItemListNum = itemCombineExInfo.byItemListCount;
  pCombineEx->m_bySelectItemCount = itemCombineExInfo.byItemSelectCount;
  for ( int itemIndex = 0; itemIndex < 24; ++itemIndex )
  {
    if ( itemIndex >= itemCombineExInfo.byItemListCount )
    {
      pCombineEx->m_List[itemIndex].Key.SetRelease();
    }
    else
    {
      _COMBINEKEY combineKey = CombineKeyFromDb(itemCombineExInfo.invenKey[itemIndex].lK);
      pCombineEx->m_List[itemIndex].Key.LoadDBKey(combineKey);
      pCombineEx->m_List[itemIndex].dwDur = itemCombineExInfo.invenKey[itemIndex].dwD;
      pCombineEx->m_List[itemIndex].dwUpt = itemCombineExInfo.invenKey[itemIndex].dwU;
    }
  }
  pCombineEx->m_dwResultEffectType = itemCombineExInfo.dwResultEffectType;
  pCombineEx->m_dwResultEffectMsgCode = itemCombineExInfo.dwResultEffectMsgCode;
  return 0;
}

unsigned __int8 CMainThread::_db_load_losebattlecount(
        unsigned int dwSerial,
        _AVATOR_DATA *pCon)
{
  const unsigned __int8 race = pCon->dbAvator.m_byRaceSexCode / 2;
  int selectResult = this->m_pWorldDB->Select_WinBattleCount(
    race,
    dwSerial,
    pCon->dbAvator.m_dwRaceBattleRecord);
  if ( selectResult == 1 )
    return 24;
  selectResult = this->m_pWorldDB->Select_FailBattleCount(
    race,
    dwSerial,
    &pCon->dbAvator.m_dwRaceBattleRecord[1]);
  if ( selectResult == 1 )
    return 24;
  selectResult = this->m_pWorldDB->Select_LoseBattleCount(
    race,
    dwSerial,
    &pCon->dbAvator.m_dwRaceBattleRecord[2]);
  if ( selectResult == 1 )
    return 24;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_MacroData(
        unsigned int dwSerial,
        _AIOC_A_MACRODATA *pMacro)
{
  unsigned __int8 dbResult = 0;

  std::memset(pMacro, 0, sizeof(_AIOC_A_MACRODATA));
  dbResult = this->m_pWorldDB->Select_MacroData(dwSerial, pMacro);
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_MacroData( dwSerial) )
      return 24;
    for ( int potionIndex = 0; potionIndex < 1; ++potionIndex )
    {
      pMacro->mcr_Potion[potionIndex].Potion[0] = -1;
      pMacro->mcr_Potion[potionIndex].Potion[1] = -1;
      pMacro->mcr_Potion[potionIndex].Potion[2] = -1;
      pMacro->mcr_Potion[potionIndex].PotionValue[0] = 0;
      pMacro->mcr_Potion[potionIndex].PotionValue[1] = 0;
      pMacro->mcr_Potion[potionIndex].PotionValue[2] = 0;
    }
    for ( int actionIndex = 0; actionIndex < 3; ++actionIndex )
    {
      pMacro->mcr_Action[actionIndex].Action[0] = -1;
      pMacro->mcr_Action[actionIndex].Action[1] = -1;
      pMacro->mcr_Action[actionIndex].Action[2] = -1;
      pMacro->mcr_Action[actionIndex].Action[3] = -1;
      pMacro->mcr_Action[actionIndex].Action[4] = -1;
      pMacro->mcr_Action[actionIndex].Action[5] = -1;
      pMacro->mcr_Action[actionIndex].Action[6] = -1;
      pMacro->mcr_Action[actionIndex].Action[7] = -1;
      pMacro->mcr_Action[actionIndex].Action[8] = -1;
      pMacro->mcr_Action[actionIndex].Action[9] = -1;
    }
    for ( int chatIndex = 0; chatIndex < 2; ++chatIndex )
    {
      std::memset(&pMacro->mcr_Chat[chatIndex], 0, 256);
      std::memset(pMacro->mcr_Chat[chatIndex].Chat[1], 0, sizeof(pMacro->mcr_Chat[chatIndex].Chat[1]));
      std::memset(pMacro->mcr_Chat[chatIndex].Chat[2], 0, sizeof(pMacro->mcr_Chat[chatIndex].Chat[2]));
      std::memset(pMacro->mcr_Chat[chatIndex].Chat[3], 0, sizeof(pMacro->mcr_Chat[chatIndex].Chat[3]));
      std::memset(pMacro->mcr_Chat[chatIndex].Chat[4], 0, sizeof(pMacro->mcr_Chat[chatIndex].Chat[4]));
    }
  }
  else if ( dbResult == 1 )
  {
    return 24;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_NpcQuest_History(
        unsigned int dwSerial,
        _QUEST_DB_BASE *pCon)
{
  _worlddb_npc_quest_complete_history npcQuestHistory;
  unsigned __int8 dbResult;

  dbResult = this->m_pWorldDB->Select_NpcQuest_History(dwSerial, &npcQuestHistory);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_NpcQuest_History(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_NpcQuest_History(dwSerial, &npcQuestHistory) )
      return 24;
  }
  for ( int historyIndex = 0; historyIndex < 70; ++historyIndex )
  {
    std::strcpy(pCon->m_History[historyIndex].szQuestCode, npcQuestHistory.List[historyIndex].szQuestCode);
    pCon->m_History[historyIndex].byLevel = npcQuestHistory.List[historyIndex].byLevel;
    pCon->m_History[historyIndex].dwEventEndTime = npcQuestHistory.List[historyIndex].dwEventEndTime;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_OreCutting(
        unsigned int dwSerial,
        _CUTTING_DB_BASE *pDbCutting)
{
  _worlddb_ore_cutting oreCuttingData;
  unsigned __int8 dbResult;

  dbResult = this->m_pWorldDB->Select_OreCutting(dwSerial, &oreCuttingData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_OreCutting(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_OreCutting(dwSerial, &oreCuttingData) )
      return 24;
  }
  if ( pDbCutting->m_bOldDataLoad )
    return 0;
  pDbCutting->Init();
  for ( int cutIndex = 0; cutIndex < 20; ++cutIndex )
  {
    if ( oreCuttingData.List[cutIndex].nKey != -1 )
    {
      pDbCutting->m_List[cutIndex].Key.LoadDBKey(oreCuttingData.List[cutIndex].nKey);
      pDbCutting->m_List[cutIndex].dwDur = oreCuttingData.List[cutIndex].dwDur;
      ++pDbCutting->m_byLeftNum;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PcBangFavor(
        unsigned int dwSerial,
        _PCBANG_FAVOR_ITEM_DB_BASE *pDbPcBangFavor)
{
  _worlddb_pcbang_favor_item favorItemData;
  unsigned __int8 dbResult;

  dbResult = this->m_pWorldDB->Select_PcBangFavorItem(dwSerial, &favorItemData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_PcBangFavorItem(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_PcBangFavorItem(dwSerial, &favorItemData) )
      return 24;
  }
  for ( int itemIndex = 0; itemIndex < 50; ++itemIndex )
    pDbPcBangFavor->lnUID[itemIndex] = favorItemData.lnUID[itemIndex];
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PotionDelay(
        unsigned int dwSerial,
        _POTION_NEXT_USE_TIME_DB_BASE *pDbPotionDelay)
{
  _worlddb_potion_delay_info potionDelayInfo;
  unsigned __int8 dbResult;
  DWORD currentTime;

  dbResult = this->m_pWorldDB->Select_PotionDelay(dwSerial, &potionDelayInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_PotionDelay(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_PotionDelay(dwSerial, &potionDelayInfo) )
      return 24;
  }
  currentTime = timeGetTime();
  for ( int delayIndex = 0; delayIndex < 38; ++delayIndex )
    pDbPotionDelay->dwPotionNextUseTime[delayIndex] = currentTime + potionDelayInfo.dwPotionDelay[delayIndex];
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PrimiumPlayTime(
        unsigned int dwAccSerial,
        _PCBANG_PLAY_TIME *kData)
{
  unsigned __int8 dbResult = this->m_pWorldDB->Select_PrimiumPlayTime(dwAccSerial, kData);

  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
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
  const int selectResult = this->m_pWorldDB->Select_Punishment(
    dwSerial,
    pCon->dbAvator.m_dwElectSerial,
    pCon->dbAvator.m_dwPunishment);
  if ( selectResult == 1 )
    return 24;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_PvpOrderView(
        unsigned int dwSerial,
        _PVP_ORDER_VIEW_DB_BASE *kData)
{
  _pvporderview_info orderViewInfo;
  unsigned __int8 dbResult;

  std::memset(&orderViewInfo, 0, sizeof(orderViewInfo));
  dbResult = this->m_pWorldDB->Select_PvpOrderViewInfo(dwSerial, &orderViewInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( this->m_pWorldDB->Insert_PvpOrderViewInfo(dwSerial) )
      return 0;
    else
      return 24;
  }
  else
  {
    kData->tUpdatedate = orderViewInfo.tUpdatedate;
    kData->nDeath = orderViewInfo.nDeath;
    kData->nKill = orderViewInfo.nKill;
    kData->dTodayStacked = orderViewInfo.dTodayStacked;
    kData->dPvpPoint = orderViewInfo.dPvpPoint;
    kData->dPvpTempCash = orderViewInfo.dPvpTempCash;
    kData->dPvpCash = orderViewInfo.dPvpCash;
    for ( int killerIndex = 0; killerIndex < 10; ++killerIndex )
      kData->dwKillerSerial[killerIndex] = orderViewInfo.dwKillerSerial[killerIndex];
    kData->byContHaveCash = orderViewInfo.byContHaveCash;
    kData->byContLoseCash = orderViewInfo.byContLoseCash;
    kData->bRaceWarRecvr = orderViewInfo.bRaceWarRecvr;
    return 0;
  }
}

unsigned __int8 CMainThread::_db_Load_PvpPointLimitData(
        unsigned int dwSerial,
        _PVPPOINT_LIMIT_DB_BASE *kData)
{
  _pvppointlimit_info limitInfo;
  unsigned __int8 dbResult;

  std::memset(&limitInfo, 0, sizeof(limitInfo));
  dbResult = this->m_pWorldDB->Select_PvpPointLimitInfo(dwSerial, &limitInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( this->m_pWorldDB->Insert_PvpPointLimitInfoRecord(dwSerial) )
      return 0;
    else
      return 24;
  }
  else
  {
    kData->tUpdatedate = limitInfo.tUpdatedate;
    kData->bUseUp = limitInfo.bUseUp;
    kData->byLimitRate = limitInfo.byLimitRate;
    kData->dOriginalPoint = limitInfo.dOriginalPoint;
    kData->dLimitPoint = limitInfo.dLimitPoint;
    kData->dUsePoint = limitInfo.dUsePoint;
    return 0;
  }
}

unsigned __int8 CMainThread::_db_Load_Quest( unsigned int dwSerial, _QUEST_DB_BASE *pCon)
{
  _worlddb_quest_array questData;
  unsigned __int8 dbResult;

  std::memset(&questData, 0, sizeof(questData));
  dbResult = this->m_pWorldDB->Select_Quest(dwSerial, &questData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_Quest(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_Quest(dwSerial, &questData) )
      return 24;
  }
  for ( int questIndex = 0; questIndex < 30; ++questIndex )
  {
    pCon->m_List[questIndex].byQuestType = questData.questList[questIndex].byQuestType;
    pCon->m_List[questIndex].wIndex = questData.questList[questIndex].wIndex;
    pCon->m_List[questIndex].dwPassSec = questData.questList[questIndex].dwPassSec;
    pCon->m_List[questIndex].wNum[0] = questData.questList[questIndex].wNum[0];
    pCon->m_List[questIndex].wNum[1] = questData.questList[questIndex].wNum[1];
    pCon->m_List[questIndex].wNum[2] = questData.questList[questIndex].wNum[2];
  }
  return 0;
}

unsigned __int8 CMainThread::_db_load_raceboss( unsigned int dwSerial, _AVATOR_DATA *pCon)
{
  const unsigned __int8 race = pCon->dbAvator.m_byRaceSexCode / 2;
  int selectResult = this->m_pWorldDB->Select_PatriarchVoted(
    race,
    dwSerial,
    &pCon->dbAvator.m_bOverlapVote);
  if ( selectResult == 1 )
    return 24;
  selectResult = this->m_pWorldDB->Select_PatriarchRefundCount(
    race,
    dwSerial,
    &pCon->dbAvator.m_dwGivebackCount);
  if ( selectResult )
    pCon->dbAvator.m_dwGivebackCount = 0;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_SFDelayData(
        unsigned int dwSerial,
        _worlddb_sf_delay_info *pDbSFDelayInfo)
{
  const unsigned __int8 dbResult = this->m_pWorldDB->Select_SFDelayInfo(dwSerial, pDbSFDelayInfo);

  if ( dbResult == 1 )
    return 24;
  if ( dbResult != 2 || this->m_pWorldDB->Insert_SFDelayInfo(dwSerial, pDbSFDelayInfo) )
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
      std::strcpy(pCon->m_StartHistory[j].szQuestCode, pdwCount.List[j].szQuestCode);
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
  _worlddb_character_supplement_info supplementInfo;
  unsigned __int8 dbResult;
  unsigned __int64 scannerCounter;
  char scannerBuffer[88];
  char scannerCountString[28];

  dbResult = this->m_pWorldDB->Select_Supplement(dwSerial, &supplementInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_Supplement(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_Supplement(dwSerial, &supplementInfo) )
      return 24;
  }
  pDbSupplement->dPvpPointLeak = supplementInfo.dPvpPointLeak;
  pDbSupplement->bLastAttBuff = supplementInfo.bLastAttBuff;
  pDbSupplement->dwBufPotionEndTime = supplementInfo.dwBufPotionEndTime;
  pDbSupplement->dwRaceBuffClear = supplementInfo.dwRaceBuffClear;
  if ( this->m_pWorldDB->Select_Supplement_Ex(dwSerial, &supplementInfo) )
    return 24;
  pDbSupplement->byVoted = supplementInfo.byVoted;
  pDbSupplement->dwAccumPlayTime = supplementInfo.dwAccumPlayTime;
  pDbSupplement->dwLastResetDate = supplementInfo.dwLastResetDate;
  pDbSupplement->VoteEnable = supplementInfo.VoteEnable;
  scannerCounter = supplementInfo.dwScanerCnt;
  if ( supplementInfo.dwScanerCnt )
  {
    memset(scannerBuffer, 0, 64);
    memset(scannerCountString, 0, 10);
    sprintf_s(scannerBuffer, 64, "%I64u", scannerCounter);
    std::memcpy(scannerCountString, scannerBuffer, 9uLL);
    pDbSupplement->dwScanerGetDate = atoi(scannerCountString);
    std::memset(scannerCountString, 0, 10);
    const size_t scannerBufferLength = std::strlen(scannerBuffer);
    std::memcpy(scannerCountString, &scannerBuffer[9], scannerBufferLength - 9);
    pDbSupplement->wScanerCnt = atoi(scannerCountString);
  }
  else
  {
    pDbSupplement->wScanerCnt = 0;
    pDbSupplement->dwScanerGetDate = 199001010;
  }
  if ( this->m_pWorldDB->Select_Supplement_ActPoint(dwSerial, &supplementInfo) )
    return 24;
  for ( int actionPointIndex = 0; actionPointIndex < 3; ++actionPointIndex )
    pDbSupplement->dwActionPoint[actionPointIndex] = supplementInfo.dwActionPoint[actionPointIndex];
  return 0;
}

unsigned __int8 CMainThread::_db_Load_TimeLimitInfo(
        unsigned int dwAccSerial,
        _TIMELIMITINFO_DB_BASE *pDbTimeLimitInfo)
{
  _worlddb_time_limit_info timeLimitInfo;
  unsigned __int8 dbResult;

  dbResult = this->m_pWorldDB->Select_PlayerTimeLimitInfo(dwAccSerial, &timeLimitInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_PlayerTimeLimitInfo(dwAccSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_PlayerTimeLimitInfo(dwAccSerial, &timeLimitInfo) )
      return 24;
  }
  pDbTimeLimitInfo->dwAccSerial = dwAccSerial;
  pDbTimeLimitInfo->dwFatigue = timeLimitInfo.dwFatigue;
  pDbTimeLimitInfo->byTLStatus = timeLimitInfo.byTLStatus;
  pDbTimeLimitInfo->dwLastLogoutTime = timeLimitInfo.dwLastLogoutTime;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Trade(
        unsigned __int8 byRace,
        unsigned int dwSerial,
        _TRADE_DB_BASE *pTrade)
{
  _worlddb_trade_info tradeData;
  unsigned __int8 dbResult;

  std::memset(&tradeData, 0, sizeof(tradeData));
  dbResult = this->m_pWorldDB->Select_Trade(0, dwSerial, byRace, &tradeData);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
    return 0;
  for ( int tradeIndex = 0; tradeIndex < 20; ++tradeIndex )
  {
    pTrade->m_List[tradeIndex].byState = tradeData.list[tradeIndex].byState;
    pTrade->m_List[tradeIndex].dwRegistSerial = tradeData.list[tradeIndex].dwRegistSerial;
    pTrade->m_List[tradeIndex].byInvenIndex = tradeData.list[tradeIndex].byInvenIndex;
    pTrade->m_List[tradeIndex].dwPrice = tradeData.list[tradeIndex].dwPrice;
    pTrade->m_List[tradeIndex].tStartTime = tradeData.list[tradeIndex].tStartTime;
    pTrade->m_List[tradeIndex].bySellTurm = tradeData.list[tradeIndex].bySellTurm;
    pTrade->m_List[tradeIndex].dwBuyerSerial = tradeData.list[tradeIndex].dwBuyerSerial;
    pTrade->m_List[tradeIndex].dwTax = tradeData.list[tradeIndex].dwTax;
    pTrade->m_List[tradeIndex].tResultTime = tradeData.list[tradeIndex].tResultTime;
    strcpy_s(pTrade->m_List[tradeIndex].wszBuyerName, 17, tradeData.list[tradeIndex].wszBuyerName);
    strcpy_s(pTrade->m_List[tradeIndex].szBuyerAccount, 13, tradeData.list[tradeIndex].szBuyerAccount);
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Trunk(
        unsigned int dwSerial,
        unsigned int dwAccountSerial,
        unsigned __int8 byRace,
        _TRUNK_DB_BASE *pTrunk)
{
  _worlddb_trunk_info trunkData;
  unsigned __int8 trunkResult;
  unsigned __int8 trunkExtendResult;

  std::memset(&trunkData, 0, sizeof(trunkData));
  trunkResult = this->m_pWorldDB->Select_AccountTrunk(dwAccountSerial, byRace, &trunkData);
  if ( trunkResult == 1 )
    return 24;
  if ( trunkResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_AccountTrunk(dwAccountSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_AccountTrunk(dwAccountSerial, byRace, &trunkData) )
      return 24;
  }
  pTrunk->bySlotNum = trunkData.bySlotNum;
  std::strcpy(pTrunk->wszPasswd, trunkData.wszPasswd);
  pTrunk->dDalant = trunkData.dDalant;
  pTrunk->dGold = trunkData.dGold;
  pTrunk->byHintIndex = trunkData.byHintIndex;
  std::strcpy(pTrunk->wszHintAnswer, trunkData.wszHintAnswer);
  for ( int slotIndex = 0; slotIndex < 100; ++slotIndex )
  {
    if ( slotIndex >= trunkData.bySlotNum )
    {
      pTrunk->m_List[slotIndex].Key.SetRelease();
    }
    else
    {
      pTrunk->m_List[slotIndex].Key.LoadDBKey(trunkData.trunkKey[slotIndex].lK);
      pTrunk->m_List[slotIndex].dwDur = trunkData.trunkKey[slotIndex].dwD;
      pTrunk->m_List[slotIndex].dwUpt = trunkData.trunkKey[slotIndex].dwU;
      pTrunk->m_List[slotIndex].byRace = trunkData.trunkKey[slotIndex].byRace;
      pTrunk->m_List[slotIndex].dwT = trunkData.trunkKey[slotIndex].dwT;
      pTrunk->m_List[slotIndex].lnUID = trunkData.trunkKey[slotIndex].lnUID;
    }
  }
  trunkExtendResult = this->m_pWorldDB->Select_AccountTrunkExtend(dwAccountSerial, &trunkData);
  if ( trunkExtendResult == 1 )
    return 24;
  if ( trunkExtendResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_AccountTrunkExtend(dwAccountSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_AccountTrunkExtend(dwAccountSerial, &trunkData) )
      return 24;
  }
  pTrunk->byExtSlotNum = trunkData.byExtSlotNum;
  for ( int extSlotIndex = 0; extSlotIndex < 40; ++extSlotIndex )
  {
    if ( extSlotIndex >= trunkData.byExtSlotNum )
    {
      pTrunk->m_ExtList[extSlotIndex].Key.SetRelease();
    }
    else
    {
      pTrunk->m_ExtList[extSlotIndex].Key.LoadDBKey(trunkData.ExttrunkKey[extSlotIndex].lK);
      pTrunk->m_ExtList[extSlotIndex].dwDur = trunkData.ExttrunkKey[extSlotIndex].dwD;
      pTrunk->m_ExtList[extSlotIndex].dwUpt = trunkData.ExttrunkKey[extSlotIndex].dwU;
      pTrunk->m_ExtList[extSlotIndex].byRace = trunkData.ExttrunkKey[extSlotIndex].byRace;
      pTrunk->m_ExtList[extSlotIndex].dwT = trunkData.ExttrunkKey[extSlotIndex].dwT;
      pTrunk->m_ExtList[extSlotIndex].lnUID = trunkData.ExttrunkKey[extSlotIndex].lnUID;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_UI(
        unsigned int dwSerial,
        _LINK_DB_BASE *pLink,
        _SFCONT_DB_BASE *pSfcont)
{
  _worlddb_userinterface_info userInterfaceInfo;
  unsigned __int8 dbResult;

  std::memset(&userInterfaceInfo, 0, sizeof(userInterfaceInfo));
  dbResult = this->m_pWorldDB->Select_UserInterface(dwSerial, &userInterfaceInfo);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_UserInterface(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_UserInterface(dwSerial, &userInterfaceInfo) )
      return 24;
  }
  for ( int linkIndex = 0; linkIndex < 50; ++linkIndex )
    pLink->m_LinkList[linkIndex].Key.wEffectCode = userInterfaceInfo.wLinkBoard[linkIndex];
  for ( int forceIndex = 0; forceIndex < 8; ++forceIndex )
  {
    pSfcont->m_List[0][forceIndex].dwKey = userInterfaceInfo.dwDamageCForce[forceIndex];
    pSfcont->m_List[1][forceIndex].dwKey = userInterfaceInfo.dwHelpCForce[forceIndex];
  }
  std::memcpy(pLink->m_dwSkill, userInterfaceInfo.dwSkill, sizeof(pLink->m_dwSkill));
  std::memcpy(pLink->m_dwForce, userInterfaceInfo.dwForce, sizeof(pLink->m_dwForce));
  std::memcpy(pLink->m_dwCharacter, userInterfaceInfo.dwCharacter, sizeof(pLink->m_dwCharacter));
  std::memcpy(pLink->m_dwAnimus, userInterfaceInfo.dwAnimus, sizeof(pLink->m_dwAnimus));
  std::memcpy(pLink->m_dwInvenBag, userInterfaceInfo.dwInvenBag, sizeof(pLink->m_dwInvenBag));
  pLink->m_dwInven = userInterfaceInfo.dwInven;
  pLink->m_byLinkBoardLock = userInterfaceInfo.byLinkBoardLock;
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Unit( unsigned int dwSerial, _UNIT_DB_BASE *pCon)
{
  _worlddb_unit_info_array unitInfoArray;
  unsigned __int8 dbResult;

  std::memset(&unitInfoArray, 0, sizeof(unitInfoArray));
  dbResult = this->m_pWorldDB->Select_Unit(dwSerial, &unitInfoArray);
  if ( dbResult == 1 )
    return 24;
  if ( dbResult == 2 )
  {
    if ( !this->m_pWorldDB->Insert_Unit(dwSerial) )
      return 24;
    if ( this->m_pWorldDB->Select_Unit(dwSerial, &unitInfoArray) )
      return 24;
  }
  for ( int unitIndex = 0; unitIndex < 4; ++unitIndex )
  {
    pCon->m_List[unitIndex].byFrame = unitInfoArray.UnitInfo[unitIndex].byFrame;
    if ( pCon->m_List[unitIndex].byFrame != 255 )
    {
      pCon->m_List[unitIndex].byPart[0] = unitInfoArray.UnitInfo[unitIndex].byPart[0];
      pCon->m_List[unitIndex].byPart[1] = unitInfoArray.UnitInfo[unitIndex].byPart[1];
      pCon->m_List[unitIndex].byPart[2] = unitInfoArray.UnitInfo[unitIndex].byPart[2];
      pCon->m_List[unitIndex].byPart[3] = unitInfoArray.UnitInfo[unitIndex].byPart[3];
      pCon->m_List[unitIndex].byPart[4] = unitInfoArray.UnitInfo[unitIndex].byPart[4];
      pCon->m_List[unitIndex].byPart[5] = unitInfoArray.UnitInfo[unitIndex].byPart[5];
      pCon->m_List[unitIndex].dwGauge = unitInfoArray.UnitInfo[unitIndex].dwGauge;
      pCon->m_List[unitIndex].dwBullet[0] = unitInfoArray.UnitInfo[unitIndex].dwBullet[0];
      pCon->m_List[unitIndex].dwBullet[1] = unitInfoArray.UnitInfo[unitIndex].dwBullet[1];
      pCon->m_List[unitIndex].nPullingFee = unitInfoArray.UnitInfo[unitIndex].nPullingFee;
      pCon->m_List[unitIndex].dwCutTime = unitInfoArray.UnitInfo[unitIndex].dwCutTime;
      for ( int spareIndex = 0; spareIndex < 8; ++spareIndex )
        pCon->m_List[unitIndex].dwSpare[spareIndex] = unitInfoArray.UnitInfo[unitIndex].dwSpare[spareIndex];
    }
  }
  return 0;
}
