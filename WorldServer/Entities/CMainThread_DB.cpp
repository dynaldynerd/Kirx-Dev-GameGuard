#include "pch.h"

#include "CMainThread.h"

#include <cstring>

#include "CCryptor.h"
#include "CCheckSumGuildData.h"
#include "CGoldenBoxItemMgr.h"
#include "CMgrAvatorItemHistory.h"
#include "CMapOperation.h"
#include "CRFWorldDatabase.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "economy_history_data.h"
#include "qry_case_cash_limsale.h"
#include "WorldServerUtil.h"

namespace
{
constexpr char kNonMakeName[3][17] = {"GM", "ADMIN", "OPERATOR"};
}

unsigned __int8 CMainThread::db_Reged_Avator(
  unsigned int dwAccountSerial,
  _REGED *pRegedList,
  _NOT_ARRANGED_AVATOR_DB *pArrangedList,
  const char *pszIP)
{
_worlddb_character_base_info_array characterData{};
  unsigned __int8 result = m_pWorldDB->Select_CharacterBaseInfoBySerial(
    dwAccountSerial,
    &characterData);
  if (result == 1)
  {
    return 24;
  }

  for (int j = 0; j < characterData.wCharacterCount; ++j)
  {
    unsigned __int8 slotIndex = characterData.CharacterInfo[j].bySlotIndex;
    if (slotIndex > 3)
    {
      return 24;
    }

    if (pRegedList[slotIndex].m_bySlotIndex != 255)
    {
      int emptyIndex = -1;
      for (int k = 0; k < 3; ++k)
      {
        if (pRegedList[k].m_bySlotIndex == 255)
        {
          emptyIndex = k;
          break;
        }
      }
      if (emptyIndex == -1)
      {
        continue;
      }
      slotIndex = static_cast<unsigned __int8>(emptyIndex);
    }

    pRegedList[slotIndex].m_bySlotIndex = slotIndex;
    pRegedList[slotIndex].m_dwRecordNum = characterData.CharacterInfo[j].dwSerial;
    strcpy_0(pRegedList[slotIndex].m_wszAvatorName, characterData.CharacterInfo[j].wszName);
    pRegedList[slotIndex].m_byRaceSexCode = characterData.CharacterInfo[j].byRace;
    strcpy_0(pRegedList[slotIndex].m_szClassCode, characterData.CharacterInfo[j].szClassCode);
    pRegedList[slotIndex].m_byLevel = characterData.CharacterInfo[j].byLevel;
    pRegedList[slotIndex].m_dwDalant = characterData.CharacterInfo[j].dwDalant;
    pRegedList[slotIndex].m_dwGold = characterData.CharacterInfo[j].dwGold;
    pRegedList[slotIndex].m_dwBaseShape = characterData.CharacterInfo[j].dwBaseShape;
    pRegedList[slotIndex].m_dwLastConnTime = characterData.CharacterInfo[j].dwLastConnTime;
    for (int m = 0; m < 8; ++m)
    {
      pRegedList[slotIndex].m_EquipKey[m].LoadDBKey(characterData.CharacterInfo[j].shEKArray[m]);
      pRegedList[slotIndex].m_dwFixEquipLv[m] = characterData.CharacterInfo[j].dwEUArray[m];
      pRegedList[slotIndex].m_lnUID[m] = characterData.CharacterInfo[j].lnUIDArray[m];
      pRegedList[slotIndex].m_dwET[m] = characterData.CharacterInfo[j].dwETArray[m];
    }
  }

  _worlddb_arrange_char_info arrangeData{};
  memset_0(&arrangeData, 0, sizeof(arrangeData));
  result = m_pWorldDB->Select_NotArrangeCharacter(dwAccountSerial, &arrangeData);
  if (result == 1)
  {
    return 0;
  }

  for (int j = 0; j < arrangeData.byCount; ++j)
  {
    memcpy_0(&pArrangedList[j], &arrangeData.ArrangeChar[j], sizeof(_NOT_ARRANGED_AVATOR_DB));
  }

  return 0;
}

unsigned __int8 CMainThread::db_Insert_Avator(
  unsigned int dwAccountSerial,
  char *pszAccount,
  _REGED_AVATOR_DB *pCharDB,
  unsigned int *pdwAvatorSerial)
{
  unsigned int dwSerial[7]{};
  dwSerial[0] = static_cast<unsigned int>(-1);
  *pdwAvatorSerial = static_cast<unsigned int>(-1);

  if (!m_pWorldDB->Select_Equal_Name(pCharDB->m_wszAvatorName))
  {
    return 25;
  }
  if (!m_pWorldDB->Select_Equal_DeleteName_NoArranged(pCharDB->m_wszAvatorName))
  {
    return 25;
  }

  CMapData *startMap = g_MapOper.GetStartMap(pCharDB->m_byRaceSexCode / 2);
  if (!m_pWorldDB->Insert_CharacterData(
        pCharDB->m_wszAvatorName,
        pCharDB->m_szClassCode,
        dwAccountSerial,
        pszAccount,
        pCharDB->m_bySlotIndex,
        pCharDB->m_byRaceSexCode,
        pCharDB->m_dwBaseShape,
        startMap->m_nMapIndex,
        dwSerial))
  {
    return 24;
  }
  if (!m_pWorldDB->Insert_MacroData(dwSerial[0]))
  {
    return 24;
  }
  *pdwAvatorSerial = dwSerial[0];
  return 0;
}

unsigned __int8 CMainThread::db_Delete_Avator(unsigned int dwSerial, unsigned __int8 byRaceCode)
{
if (m_pWorldDB->Delete_CharacterData(dwSerial))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Log_AvatorLevel(
  unsigned int dwSerial,
  unsigned int dwAccountSerial,
  unsigned __int8 byLevel)
{
  if (m_pWorldDB->Insert_Level_Log(dwAccountSerial, byLevel, dwSerial))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Log_UserNum(unsigned int dwRace, unsigned int dwUserNum)
{
  if (m_pWorldDB->Insert_UserNum_Log(dwRace, dwUserNum))
  {
    return 0;
  }
  return 24;
}

bool CMainThread::db_Insert_Economy_History(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData)
{
  return m_pWorldDB->Insert_Economy_History(dwDate, pEconomyData);
}

unsigned __int8 CMainThread::db_Select_Economy_History(
  _economy_history_data *pCurData,
  unsigned int *pnCurMgrValue,
  unsigned int *pnNextMgrValue,
  _economy_history_data *pHisData,
  int *pHistoryNum,
  unsigned int dwDate)
{
  _worlddb_economy_history_info pEconomyData{};
  if (!m_pWorldDB->Select_Exist_Economy(dwDate, &pEconomyData))
  {
    for (int j = 0; j < 3; ++j)
    {
      pCurData->dTradeDalant[j] = pEconomyData.dTradeDalant[j];
      pCurData->dTradeGold[j] = pEconomyData.dTradeGold[j];
      for (int k = 0; k < 3; ++k)
      {
        pCurData->dOreMineCount[j][k] = pEconomyData.dMineOre[j][k];
        pCurData->dOreCutCount[j][k] = pEconomyData.dCutOre[j][k];
      }
    }
    *pnCurMgrValue = pEconomyData.dwManageValue;
    *pnNextMgrValue = pEconomyData.dwManageValue;
  }

  _worlddb_economy_history_info_array history{};
  const unsigned __int8 result = m_pWorldDB->Select_Economy_History(&history, dwDate);
  if (result == 1)
  {
    return 24;
  }

  int slot = 12;
  for (int m = 0; m < history.wRowCount; ++m)
  {
    if (--slot < 0)
    {
      break;
    }
    for (int n = 0; n < 3; ++n)
    {
      pHisData[slot].dTradeDalant[n] = history.EconomyData[m].dTradeDalant[n];
      pHisData[slot].dTradeGold[n] = history.EconomyData[m].dTradeGold[n];
      for (int ii = 0; ii < 3; ++ii)
      {
        pHisData[slot].dOreMineCount[n][ii] = history.EconomyData[m].dMineOre[n][ii];
        pHisData[slot].dOreCutCount[n][ii] = history.EconomyData[m].dCutOre[n][ii];
      }
    }
  }

  *pHistoryNum = history.wRowCount;
  return 0;
}

unsigned __int8 CMainThread::db_Insert_CharacSelect_Log(
  unsigned int dwAccountSerial,
  char *szAccount,
  unsigned int dwCharacSerial,
  char *pwszCharacName,
  unsigned __int16 dwYear,
  unsigned __int8 byMonth,
  unsigned __int8 byDay,
  unsigned __int8 byHour,
  unsigned __int8 byMin,
  unsigned __int8 bySec)
{
  char buffer[144]{};
  sprintf_s(buffer, 0x80uLL, "tbl_characterselect_log_%04d%02d", dwYear, byMonth);
  if (!m_pWorldDB->TableExist(buffer)
      && !m_pWorldDB->CreateCharacterSelectLogTable(buffer)
      && !m_pWorldDB->CreateCharacterSelectLogTable(buffer))
  {
    return 24;
  }

  if (m_pWorldDB->InsertCharacterSelectLog(
        dwAccountSerial,
        szAccount,
        dwCharacSerial,
        pwszCharacName,
        dwYear,
        byMonth,
        byDay,
        byHour,
        byMin,
        bySec))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Update_PvpInfo(
  unsigned int dwSerial,
  unsigned __int8 byGrade,
  __int16 *pwKillPoint,
  long double dPvPPoint)
{
  if (m_pWorldDB->Update_PvpPointInfo(dwSerial, pwKillPoint, dPvPPoint))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Add_PvpPoint(unsigned int dwSerial, unsigned int dwAddPoint, unsigned int dwAddCashBag)
{
  if (m_pWorldDB->Add_PvpPoint(dwSerial, dwAddPoint, dwAddCashBag))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Insert_Item(
  unsigned int dwSerial,
  unsigned int dwItemCodeK,
  unsigned int dwItemCodeD,
  unsigned int dwItemCodeU,
  unsigned __int8 byType)
{
  if (m_pWorldDB->Insert_ItemChargeInGame(
        dwSerial,
        dwItemCodeK,
        dwItemCodeD,
        dwItemCodeU,
        byType))
  {
    return 0;
  }
  return 24;
}

char CMainThread::db_Insert_guild(
  unsigned int *dwSerial,
  char *pwszGuildName,
  unsigned __int8 byRace,
  unsigned int *dwGuildSerial)
{
  if (!m_pWorldDB->Insert_Guild(pwszGuildName, byRace))
  {
    return 24;
  }
  if (!m_pWorldDB->Select_GuildSerial(pwszGuildName, dwGuildSerial))
  {
    return 24;
  }

  CCheckSumGuildData checksum(*dwGuildSerial);
  checksum.Encode(0.0, 0.0);
  if (!checksum.Insert(m_pWorldDB))
  {
    return 24;
  }
  if (!m_pWorldDB->Insert_WeeklyGuildPvpPointSum(*dwGuildSerial))
  {
    return 24;
  }
  for (int j = 0; j < 8 && dwSerial[j] != static_cast<unsigned int>(-1); ++j)
  {
    if (!m_pWorldDB->Update_UserGuildData(dwSerial[j], *dwGuildSerial, 0))
    {
      return 24;
    }
  }
  return 0;
}

unsigned __int8 CMainThread::db_update_guildmember_add(
  unsigned int dwGuildSerial,
  unsigned int dwCharSerial,
  unsigned __int8 byClass,
  unsigned int dwLevel)
{
  if (static_cast<int>(dwLevel) > 50)
  {
    return 24;
  }
  if (!m_pWorldDB->Check_GuildMemberCount(dwCharSerial))
  {
    return 24;
  }
  if (!m_pWorldDB->Update_UserGuildData(dwGuildSerial, dwCharSerial, byClass))
  {
    return 24;
  }
  if (m_pWorldDB->Update_GuildMemberCount(dwCharSerial, static_cast<int>(dwLevel)))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_update_guildmember_del(
  unsigned int dwGuildSerial,
  unsigned int dwCharSerial,
  unsigned int dwGuildIndex)
{
  if (static_cast<int>(dwGuildIndex) < 0)
  {
    return 24;
  }
  if (!m_pWorldDB->Update_UserGuildData(dwGuildSerial, 0xFFFFFFFF, 0xFFu))
  {
    return 24;
  }
  if (m_pWorldDB->Update_GuildMemberCount(dwCharSerial, static_cast<int>(dwGuildIndex)))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_input_guild_money(
  unsigned int dwGuildSerial,
  unsigned int dwCharSerial,
  unsigned int dwAccountSerial,
  unsigned int dwParam,
  long double *dCurTotalDalant,
  long double *dCurTotalGold,
  unsigned __int8 *pbyDate,
  const char *pszPurpose)
{
  if (!m_pWorldDB->Update_InputGuildMoney(dwCharSerial, static_cast<int>(dwAccountSerial), dwParam))
  {
    return 24;
  }

  _worlddb_guild_info::__guild_info guildData{};
  if (!m_pWorldDB->Select_GuildData(dwCharSerial, &guildData))
  {
    return 24;
  }

  *dCurTotalDalant = guildData.dDalant;
  *dCurTotalGold = guildData.dGold;

  CCheckSumGuildData checksum(dwCharSerial);
  checksum.Encode(guildData.dDalant, guildData.dGold);
  if (!checksum.Update(m_pWorldDB))
  {
    return 24;
  }

  char buffer[28]{};
  sprintf_s(buffer, 9uLL, "%02d%02d%02d%02d", pbyDate[0], pbyDate[1], pbyDate[2], pbyDate[3]);
  if (!m_pWorldDB->Insert_GuildMoneyHistory(
        dwCharSerial,
        static_cast<double>(dwAccountSerial),
        static_cast<double>(dwParam),
        *dCurTotalDalant,
        *dCurTotalGold,
        buffer,
        dwGuildSerial,
        pszPurpose))
  {
    return 24;
  }

  unsigned __int8 check = g_Main.check_min_max_guild_money(dwCharSerial, dCurTotalDalant, dCurTotalGold);
  if (check)
  {
    return check;
  }
  return 0;
}

unsigned __int8 CMainThread::db_output_guild_money(
  unsigned int dwGuildSerial,
  unsigned int dwCharSerial,
  unsigned int dwAccountSerial,
  unsigned int dwParam,
  long double *dCurTotalDalant,
  long double *dCurTotalGold,
  unsigned __int8 *pbyDate,
  const char *pszPurpose,
  unsigned __int8 *pbyRetCode)
{
  unsigned __int8 check = check_min_max_guild_money(dwCharSerial, dCurTotalDalant, dCurTotalGold);
  if (check)
  {
    return check;
  }

  if ((*dCurTotalDalant == 0.0 && dwAccountSerial)
      || (*dCurTotalGold == 0.0 && dwParam)
      || static_cast<double>(dwAccountSerial) > *dCurTotalDalant
      || static_cast<double>(dwParam) > *dCurTotalGold)
  {
    *pbyRetCode = 1;
    return 0;
  }

  *dCurTotalDalant = *dCurTotalDalant - static_cast<double>(dwAccountSerial);
  *dCurTotalGold = *dCurTotalGold - static_cast<double>(dwParam);

  char buffer[48]{};
  sprintf_s(buffer, 9uLL, "%02d%02d%02d%02d", pbyDate[0], pbyDate[1], pbyDate[2], pbyDate[3]);

  m_pWorldDB->SetAutoCommitMode(false);
  if (!m_pWorldDB->Update_SetGuildMoney(dwCharSerial, *dCurTotalDalant, *dCurTotalGold))
  {
    m_pWorldDB->RollbackTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
    return 24;
  }

  CCheckSumGuildData checksum(dwCharSerial);
  checksum.Encode(*dCurTotalDalant, *dCurTotalGold);
  if (!checksum.Update(m_pWorldDB))
  {
    m_pWorldDB->RollbackTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
    return 24;
  }

  if (!m_pWorldDB->Insert_GuildMoneyHistory(
        dwCharSerial,
        -static_cast<double>(dwAccountSerial),
        -static_cast<double>(dwParam),
        *dCurTotalDalant,
        *dCurTotalGold,
        buffer,
        dwGuildSerial,
        pszPurpose))
  {
    m_pWorldDB->RollbackTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
    return 24;
  }

  m_pWorldDB->CommitTransaction();
  m_pWorldDB->SetAutoCommitMode(true);
  return 0;
}

unsigned __int8 CMainThread::db_buy_emblem(
  unsigned int dwGuildSerial,
  unsigned int dwCharSerial,
  unsigned int dwGuildIndex,
  unsigned int dwEmblemBack,
  unsigned int dwEmblemMark,
  long double *dCurTotalDalant,
  long double *dCurTotalGold,
  unsigned __int8 *pbyDate,
  char *pwszGuildName,
  unsigned __int8 *pbyRetCode)
{
  const int emblemDalant = static_cast<int>(dwCharSerial);
  const unsigned int suggestorSerial = dwEmblemMark;
  const unsigned int emblemBack = dwGuildIndex;
  const unsigned int emblemMark = dwEmblemBack;

  unsigned __int8 check = check_min_max_guild_money(dwGuildSerial, dCurTotalDalant, dCurTotalGold);
  if (check)
  {
    return check;
  }

  if ((*dCurTotalDalant == 0.0 && emblemDalant) || (-static_cast<double>(emblemDalant) > *dCurTotalDalant))
  {
    *pbyRetCode = 1;
    return 0;
  }

  *dCurTotalDalant = *dCurTotalDalant + static_cast<double>(emblemDalant);

  char buffer[48]{};
  sprintf_s(buffer, 9uLL, "%02d%02d%02d%02d", pbyDate[0], pbyDate[1], pbyDate[2], pbyDate[3]);

  m_pWorldDB->SetAutoCommitMode(false);
  if (!m_pWorldDB->Update_GuildEmblem(
        dwGuildSerial,
        *dCurTotalDalant,
        emblemBack,
        emblemMark))
  {
    m_pWorldDB->RollbackTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
    return 24;
  }

  CCheckSumGuildData checksum(dwGuildSerial);
  checksum.Encode(*dCurTotalDalant, *dCurTotalGold);
  if (!checksum.Update(m_pWorldDB))
  {
    m_pWorldDB->RollbackTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
    return 24;
  }

  if (!m_pWorldDB->Insert_GuildMoneyHistory(
        dwGuildSerial,
        static_cast<double>(emblemDalant),
        0.0,
        *dCurTotalDalant,
        *dCurTotalGold,
        buffer,
        suggestorSerial,
        pwszGuildName))
  {
    m_pWorldDB->RollbackTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
    return 24;
  }

  m_pWorldDB->CommitTransaction();
  m_pWorldDB->SetAutoCommitMode(true);
  return 0;
}

unsigned __int8 CMainThread::db_disjoint_guild(unsigned int dwGuildSerial)
{
  if (m_pWorldDB->Delete_Guild(dwGuildSerial))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_char_set_alive(
  unsigned int dwSerial,
  unsigned __int8 byRaceCode,
  unsigned int dwDalant,
  char *szReviveMapCode,
  unsigned __int8 byReviveStat,
  _REGED *pReged)
{
  unsigned __int8 charNum[16]{};
  unsigned __int8 result = m_pWorldDB->Select_CharNumInWorld(dwSerial, charNum);
  if (result == 1)
  {
    return 24;
  }
  if (charNum[0] >= 3u)
  {
    return 58;
  }
  if (!m_pWorldDB->Select_ArrangeInfo(dwDalant))
  {
    return 57;
  }

  char destName[44]{};
  strcpy_0(destName, szReviveMapCode);
  if (destName[0] == '*' || destName[0] == '!')
  {
    return 47;
  }

  if (byRaceCode)
  {
    if (!m_pWorldDB->Select_Equal_Name(destName))
    {
      return 25;
    }
    if (!m_pWorldDB->Select_Equal_DeleteName_NoArranged(destName))
    {
      return 25;
    }
  }
  else if (!m_pWorldDB->Select_Equal_Name(destName))
  {
    return 25;
  }

  for (int j = 0; j < 3; ++j)
  {
    if (!strcmp_0(destName, kNonMakeName[j]))
    {
      return 47;
    }
  }

  if (!m_pWorldDB->Update_SetActive(dwDalant, destName, byReviveStat))
  {
    return 24;
  }
  m_pWorldDB->Update_CharSlot(dwDalant);

  _worlddb_character_base_info baseInfo{};
  m_pWorldDB->Select_CharacterBaseInfo(dwDalant, &baseInfo);
  pReged->m_bySlotIndex = byReviveStat;
  pReged->m_dwRecordNum = dwDalant;
  strcpy_0(pReged->m_wszAvatorName, baseInfo.wszName);
  pReged->m_byRaceSexCode = baseInfo.byRace;
  strcpy_0(pReged->m_szClassCode, baseInfo.szClassCode);
  pReged->m_byLevel = baseInfo.byLevel;
  pReged->m_dwDalant = baseInfo.dwDalant;
  pReged->m_dwGold = baseInfo.dwGold;
  pReged->m_dwBaseShape = baseInfo.dwBaseShape;
  pReged->m_dwLastConnTime = baseInfo.dwLastConnTime;
  for (int k = 0; k < 8; ++k)
  {
    pReged->m_EquipKey[k].LoadDBKey(baseInfo.shEKArray[k]);
    pReged->m_dwFixEquipLv[k] = baseInfo.dwEUArray[k];
    pReged->m_lnUID[k] = baseInfo.lnUIDArray[k];
    pReged->m_dwET[k] = baseInfo.dwETArray[k];
  }
  return 0;
}

unsigned __int8 CMainThread::db_Insert_ChangeClass_AfterInitClass(
  unsigned int dwCharacSerial,
  unsigned __int8 byType,
  char *szPrevClassCode,
  char *szNextClassCode,
  int nClassInitCnt,
  unsigned __int8 byLastClassGrade,
  unsigned __int16 dwYear,
  unsigned __int8 byMonth,
  unsigned __int8 byDay,
  unsigned __int8 byHour,
  unsigned __int8 byMin,
  unsigned __int8 bySec)
{
  if (m_pWorldDB->InsertChangeClassLogAfterInitClass(
        dwCharacSerial,
        byType,
        szPrevClassCode,
        szNextClassCode,
        nClassInitCnt,
        byLastClassGrade,
        dwYear,
        byMonth,
        byDay,
        byHour,
        byMin,
        bySec))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_sendwebracebosssms(_qry_case_sendwebracebosssms *pSheet)
{
  if (!m_pWorldDB->Select_ChracterSerialRace(
        pSheet->szCharacterName,
        &pSheet->dwCharactSerial,
        &pSheet->byRace))
  {
    return 24;
  }
  pSheet->byRace = static_cast<unsigned __int8>(pSheet->byRace >> 1);
  return 0;
}

unsigned __int8 CMainThread::db_GM_GreetingMsg(_qry_case_gm_greetingmsg *pSheet)
{
  if (m_pWorldDB->Update_GmGreet(pSheet))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_RACE_GreetingMsg(_qry_case_race_greetingmsg *pSheet)
{
  if (m_pWorldDB->Update_RaceGreet(pSheet))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_GUILD_GreetingMsg(_qry_case_guild_greetingmsg *pSheet)
{
  if (m_pWorldDB->Update_GuildGreet(pSheet))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_Load_PostStorage(char *pData)
{
  _post_storage_list listData{};
  unsigned __int8 result = m_pWorldDB->Select_PostStorageList(*reinterpret_cast<unsigned int *>(pData), &listData);
  pData[4] = static_cast<char>(result);
  if (pData[4] != 1)
  {
    for (int j = 0; j < static_cast<int>(listData.dwCount); ++j)
    {
      *reinterpret_cast<unsigned int *>(&pData[80 * j + 16]) = listData.List[j].dwSerial;
      pData[80 * j + 20] = static_cast<char>(listData.List[j].byIndex);
      pData[80 * j + 21] = static_cast<char>(listData.List[j].byState);
      strcpy_s(&pData[80 * j + 22], 0x11uLL, listData.List[j].wszSendName);
      strcpy_s(&pData[80 * j + 39], 0x15uLL, listData.List[j].wszTitle);
      reinterpret_cast<_INVENKEY *>(&pData[80 * j + 60])->LoadDBKey(listData.List[j].nK);
      *reinterpret_cast<unsigned long long *>(&pData[80 * j + 64]) = listData.List[j].dwDur;
      *reinterpret_cast<unsigned int *>(&pData[80 * j + 72]) = listData.List[j].dwUpt;
      *reinterpret_cast<unsigned int *>(&pData[80 * j + 88]) = listData.List[j].dwGold;
      *reinterpret_cast<unsigned long long *>(&pData[80 * j + 80]) = listData.List[j].lnUID;
      *reinterpret_cast<unsigned __int16 *>(&pData[80 * j + 92]) = listData.List[j].wStorageIndex;
    }
    *reinterpret_cast<unsigned int *>(pData + 8) = listData.dwCount;
  }
  return 0;
}

unsigned __int8 CMainThread::db_Load_ReturnPost(char *pData)
{
  _return_post_list retData{};
  unsigned __int8 result = m_pWorldDB->Select_ReturnPost(
    *reinterpret_cast<unsigned int *>(pData + 4),
    *reinterpret_cast<unsigned int *>(pData),
    &retData);
  pData[8] = static_cast<char>(result);
  if (!pData[8])
  {
    for (int j = 0; j < static_cast<int>(retData.dwCount); ++j)
    {
      *reinterpret_cast<unsigned int *>(&pData[280 * j + 16]) = retData.List[j].dwSerial;
      pData[280 * j + 20] = static_cast<char>(retData.List[j].byState);
      strcpy_s(&pData[280 * j + 21], 0x11uLL, retData.List[j].wszRecvName);
      strcpy_s(&pData[280 * j + 38], 0x15uLL, retData.List[j].wszTitle);
      strcpy_s(&pData[280 * j + 59], 0xC9uLL, retData.List[j].wszContent);
      reinterpret_cast<_INVENKEY *>(&pData[280 * j + 260])->LoadDBKey(retData.List[j].nK);
      *reinterpret_cast<unsigned long long *>(&pData[280 * j + 264]) = retData.List[j].dwDur;
      *reinterpret_cast<unsigned int *>(&pData[280 * j + 272]) = retData.List[j].dwUpt;
      *reinterpret_cast<unsigned int *>(&pData[280 * j + 288]) = retData.List[j].dwGold;
      pData[280 * j + 292] = static_cast<char>(retData.List[j].byErr);
      *reinterpret_cast<unsigned long long *>(&pData[280 * j + 280]) = retData.List[j].lnUID;
    }
    *reinterpret_cast<unsigned int *>(pData + 12) = retData.dwCount;
  }
  return 0;
}

unsigned __int8 CMainThread::db_Load_Content(char *pData)
{
  unsigned __int8 result = m_pWorldDB->Select_PostContent(
    *reinterpret_cast<unsigned int *>(pData),
    pData + 16,
    201);
  pData[4] = static_cast<char>(result);
  return 0;
}

unsigned __int8 CMainThread::db_update_guildmaster(_qry_case_update_guildmaster *pSheet)
{
  if (!m_pWorldDB->Update_GuildMaster(
        pSheet->in_guildserial,
        pSheet->in_guild_new_masterSerial,
        pSheet->in_guild_new_masterPrevGrade))
  {
    return 24;
  }
  if (pSheet->in_guild_prev_masterSerial != static_cast<unsigned int>(-1)
      && pSheet->in_guildserial != static_cast<unsigned int>(-1)
      && pSheet->in_guild_new_masterSerial != static_cast<unsigned int>(-1)
      && !m_pWorldDB->Update_UserGuildData(
        pSheet->in_guild_prev_masterSerial,
        pSheet->in_guildserial,
        0))
  {
    return 24;
  }
  if (pSheet->in_guild_new_masterSerial == static_cast<unsigned int>(-1)
      || pSheet->in_guildserial == static_cast<unsigned int>(-1)
      || m_pWorldDB->Update_UserGuildData(
        pSheet->in_guild_new_masterSerial,
        pSheet->in_guildserial,
        2u))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::db_input_guild_money_atradetax(
  unsigned int dwGuildSerial,
  unsigned int dwCharSerial,
  unsigned int dwAccountSerial,
  long double *dCurTotalDalant,
  long double *dCurTotalGold,
  unsigned __int8 *pbyDate)
{
  if (!m_pWorldDB->Update_InputGuildMoney(dwCharSerial, static_cast<int>(dwAccountSerial), 0))
  {
    return 24;
  }

  _worlddb_guild_info::__guild_info guildData{};
  if (!m_pWorldDB->Select_GuildData(dwCharSerial, &guildData))
  {
    return 24;
  }

  *dCurTotalDalant = guildData.dDalant;
  *dCurTotalGold = guildData.dGold;

  CCheckSumGuildData checksum(dwCharSerial);
  checksum.Encode(guildData.dDalant, guildData.dGold);
  if (!checksum.Update(m_pWorldDB))
  {
    return 24;
  }

  char buffer[40]{};
  sprintf_s(buffer, 9uLL, "%02d%02d%02d%02d", pbyDate[0], pbyDate[1], pbyDate[2], pbyDate[3]);
  if (!m_pWorldDB->Insert_GuildMoneyHistory(
        dwCharSerial,
        static_cast<double>(dwAccountSerial),
        0.0,
        *dCurTotalDalant,
        *dCurTotalGold,
        buffer,
        dwGuildSerial,
        nullptr))
  {
    return 24;
  }
  return 0;
}

unsigned __int8 CMainThread::check_min_max_guild_money(
  unsigned int dwGuildSerial,
  long double *pdDalant,
  long double *pdGold)
{
  _worlddb_guild_info::__guild_info guildData{};
  if (!m_pWorldDB->Select_GuildData(dwGuildSerial, &guildData))
  {
    return 24;
  }

  bool changed = false;
  if (guildData.dDalant >= 0.0 || guildData.dGold >= 0.0)
  {
    if (guildData.dDalant >= 0.0)
    {
      if (guildData.dGold < 0.0)
      {
        guildData.dGold = 0.0;
        changed = true;
      }
    }
    else
    {
      guildData.dDalant = 0.0;
      changed = true;
    }
  }
  else
  {
    guildData.dDalant = 0.0;
    guildData.dGold = 0.0;
    changed = true;
  }

  if (guildData.dDalant <= 1000000000.0 || guildData.dGold <= 500000.0)
  {
    if (guildData.dDalant <= 1000000000.0)
    {
      if (guildData.dGold > 500000.0)
      {
        guildData.dGold = 500000.0;
        changed = true;
      }
    }
    else
    {
      guildData.dDalant = 1000000000.0;
      changed = true;
    }
  }
  else
  {
    guildData.dDalant = 1000000000.0;
    guildData.dGold = 500000.0;
    changed = true;
  }

  if (changed)
  {
    m_pWorldDB->SetAutoCommitMode(false);
    if (!m_pWorldDB->Update_SetGuildMoney(dwGuildSerial, guildData.dDalant, guildData.dGold))
    {
      m_pWorldDB->RollbackTransaction();
      m_pWorldDB->SetAutoCommitMode(true);
      return 24;
    }

    CCheckSumGuildData checksum(dwGuildSerial);
    checksum.Encode(guildData.dDalant, 0.0);
    if (!checksum.Update(m_pWorldDB))
    {
      m_pWorldDB->RollbackTransaction();
      m_pWorldDB->SetAutoCommitMode(true);
      return 24;
    }

    m_pWorldDB->CommitTransaction();
    m_pWorldDB->SetAutoCommitMode(true);
  }

  if (pdDalant)
  {
    *pdDalant = guildData.dDalant;
  }
  if (pdGold)
  {
    *pdGold = guildData.dGold;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_load_event_classrefine(
  unsigned int dwAvatorSerial,
  unsigned __int8 *byRefinedCnt,
  unsigned int *dwRefineDate)
{
  unsigned int result = m_pWorldDB->Select_RFEvent_ClassRefine(dwAvatorSerial, byRefinedCnt, dwRefineDate);
  if (result == 2)
  {
    if (!m_pWorldDB->Insert_RFEvent_ClassRefine(dwAvatorSerial))
    {
      *byRefinedCnt = static_cast<unsigned __int8>(-1);
      return 24;
    }
    *byRefinedCnt = 0;
    *dwRefineDate = 0;
  }
  else if (result == 1)
  {
    *byRefinedCnt = static_cast<unsigned __int8>(-1);
    return 24;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_update_event_classrefine(
  unsigned __int16 wIndex,
  unsigned int dwSerial,
  unsigned __int8 byRefinedCnt,
  unsigned int dwRefineDate)
{
if (m_pWorldDB->Update_RFEvent_ClassRefine(dwSerial, byRefinedCnt, dwRefineDate))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::_db_Update_SFDelayData(unsigned int dwSerial, _AVATOR_DATA *pData)
{
  return m_pWorldDB->Update_SFDelayInfo(dwSerial, &pData->dbSFDelay);
}

unsigned __int8 CMainThread::_db_init_classrefine_count()
{
  if (m_pWorldDB->ExecUpdateQuery(
        "update [dbo].[tbl_event] set ClassRefineCnt=0,ClassRefineDate=0",
        true))
  {
    return 0;
  }
  return 24;
}

unsigned __int8 CMainThread::_db_GuildRoom_Insert(_qry_case_guildroom_insert *pSheet)
{
  if (m_pWorldDB->Insert_GuidRoom(pSheet->dwGuildSerial, pSheet->byRoomType, pSheet->byRace))
  {
    return 0;
  }
  return 24;
}

bool CMainThread::_db_GuildRoom_Update(_qry_case_guildroom_update *pSheet)
{
  return m_pWorldDB->Update_GuildRoom(pSheet->dwGuildSerial);
}

unsigned __int8 CMainThread::_db_Load_PatriarchComm(char *pData)
{
  _patriarch_comm_list outList{};
  unsigned __int8 result = m_pWorldDB->Select_PatriarchComm(
    *reinterpret_cast<unsigned int *>(pData),
    &outList);
  pData[4] = static_cast<char>(result);
  if (pData[4] == 1)
  {
    return 24;
  }
  if (pData[4] == 2)
  {
    return 0;
  }
  if (!pData[4] && outList.dwCount)
  {
    CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pData));
    if (!player || !player->m_bOper)
    {
      return 0;
    }
    for (unsigned int j = 0; j < outList.dwCount; ++j)
    {
      unsigned int currentDalant = player->m_Param.GetDalant();
      if (!CanAddMoneyForMaxLimMoney(outList.List[j].dwDalant, currentDalant))
      {
        player->SendMsg_InformTaxIncome(1u, outList.List[j].dwDalant, outList.List[j].pszDepDate);
        return 0;
      }
      player->AlterDalant(static_cast<double>(static_cast<int>(outList.List[j].dwDalant)));
      if (!m_pWorldDB->Delete_PatriarchComm(*reinterpret_cast<unsigned int *>(pData), outList.List[j].pszDepDate))
      {
        player->AlterDalant(-static_cast<double>(outList.List[j].dwDalant));
        return 0;
      }
      player->SendMsg_ExchangeMoneyResult(0);
      player->SendMsg_InformTaxIncome(0, outList.List[j].dwDalant, outList.List[j].pszDepDate);
      unsigned int leftDalant = player->m_Param.GetDalant();
      const char *charName = player->m_Param.GetCharNameW();
      CPlayer::s_MgrItemHistory.patriarch_push_money(
        const_cast<char *>(charName),
        outList.List[j].dwDalant,
        leftDalant,
        player->m_szItemHistoryFileName);
    }
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Load_Cash_LimSale(qry_case_cash_limsale *pSheet)
{
  _worlddb_cash_limited_sale cashLimited{};
  unsigned __int8 result = m_pWorldDB->Select_CashLimSale(&cashLimited);
  if (result == 1)
  {
    return 24;
  }
  if (result == 2)
  {
    if (!m_pWorldDB->Insert_CashLimSale())
    {
      return 24;
    }
    if (m_pWorldDB->Select_CashLimSale(&cashLimited))
    {
      return 24;
    }
  }

  pSheet->NewSale.byDck = cashLimited.byDck;
  pSheet->NewSale.byLimited_sale_num = cashLimited.byLimited_sale_num;
  pSheet->OldSale.byDck = cashLimited.byDck;
  pSheet->OldSale.byLimited_sale_num = cashLimited.byLimited_sale_num;
  for (int j = 0; j < cashLimited.byLimited_sale_num; ++j)
  {
    pSheet->NewSale.List[j].nLimcode = cashLimited.List[j].nLimcode;
    pSheet->NewSale.List[j].nLimcount = cashLimited.List[j].nLimcount;
    pSheet->OldSale.List[j].nLimcode = cashLimited.List[j].nLimcode;
    pSheet->OldSale.List[j].nLimcount = cashLimited.List[j].nLimcount;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Update_Cash_LimSale(_db_cash_limited_sale *pNewData, _db_cash_limited_sale *pOldData)
{
  char source[160]{};
  char buffer[2052]{};
  sprintf_s(buffer, 0x800uLL, "UPDATE tbl_Cash_LimSale Set ");
  int baseLen = static_cast<int>(strlen_0(buffer));
  if (pNewData->byDck != pOldData->byDck)
  {
    sprintf(source, "DCK=%d,", pNewData->byDck);
    strcat_s(buffer, 0x800uLL, source);
  }
  if (pNewData->byLimited_sale_num != pOldData->byLimited_sale_num)
  {
    sprintf(source, "LimSaleNum=%d,", pNewData->byLimited_sale_num);
    strcat_s(buffer, 0x800uLL, source);
  }
  for (int j = 0; j < pNewData->byLimited_sale_num; ++j)
  {
    if (pNewData->List[j].nLimcode != pOldData->List[j].nLimcode)
    {
      sprintf(source, "Code_K%d=%d,", j, pNewData->List[j].nLimcode);
      strcat_s(buffer, 0x800uLL, source);
    }
    if (pNewData->List[j].nLimcount != pOldData->List[j].nLimcount)
    {
      sprintf(source, "Num%d=%d,", j, pNewData->List[j].nLimcount);
      strcat_s(buffer, 0x800uLL, source);
    }
  }

  if (strlen_0(buffer) <= static_cast<size_t>(baseLen))
  {
    memset_0(buffer, 0, baseLen);
  }
  else
  {
    sprintf_s(source, 0x80uLL, "WHERE [index] = 1");
    buffer[strlen_0(buffer) - 1] = ' ';
    strcat_s(buffer, 0x800uLL, source);
    m_pWorldDB->ExecUpdateQuery(buffer, true);
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Update_Set_Limit_Run()
{
  unsigned __int8 hash[72]{};
  char out[84]{};

  CCryptor *cryptor = CTSingleton<CCryptor>::Instance();
  if (!cryptor->MakeHash(g_cbHashVerify, 0x20uLL, hash, 0x20uLL))
  {
    return 82;
  }

  MakeBinaryStr(hash, 0x20uLL, out, 0x43uLL);
  unsigned __int8 result = m_pWorldDB->Select_Limit_Run_Record();
  if (result)
  {
    if (result == 2)
    {
      if (!m_pWorldDB->Insert_Set_Limit_Run(hash, 32))
      {
        return 24;
      }
    }
    else if (result == 1)
    {
      return 24;
    }
  }
  else if (!m_pWorldDB->Update_Set_Limit_Run(hash, 32))
  {
    return 24;
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Update_GoldBoxItem(
  int nDBSerial,
  _db_golden_box_item *pNewData,
  _db_golden_box_item *pOldData)
{
  char source[160]{};
  char buffer[2052]{};

  sprintf_s(buffer, 0x800uLL, "UPDATE tbl_GoldBoxItem Set ");
  int baseLen = static_cast<int>(strlen_0(buffer));
  if (pNewData->bydck != pOldData->bydck)
  {
    sprintf(source, "DCK=%d,", pNewData->bydck);
    strcat_s(buffer, 0x800uLL, source);
  }
  if (pNewData->dwStarterBoxCnt != pOldData->dwStarterBoxCnt)
  {
    sprintf(source, "StarterBoxMax=%d,", pNewData->dwStarterBoxCnt);
    strcat_s(buffer, 0x800uLL, source);
  }

  for (int j = 0;; ++j)
  {
    CGoldenBoxItemMgr *mgr = CGoldenBoxItemMgr::Instance();
    unsigned __int8 loopCount = mgr->GetLoopCount();
    if (j >= loopCount)
    {
      break;
    }
    if (pNewData->nBoxcode[j] != pOldData->nBoxcode[j])
    {
      sprintf(source, "BoxItemK_%d=%ld,", j + 1, pNewData->nBoxcode[j]);
      strcat_s(buffer, 0x800uLL, source);
    }
    if (pNewData->wBoxMax[j] != pOldData->wBoxMax[j])
    {
      sprintf(source, "BoxItemMax_%d=%d,", j + 1, pNewData->wBoxMax[j]);
      strcat_s(buffer, 0x800uLL, source);
    }
    if (pNewData->bygolden_item_num[j] != pOldData->bygolden_item_num[j])
    {
      sprintf(source, "LimItemNum_%d=%d,", j + 1, pNewData->bygolden_item_num[j]);
      strcat_s(buffer, 0x800uLL, source);
    }
    for (int k = 0; k < pNewData->bygolden_item_num[j]; ++k)
    {
      if (pNewData->List[j][k].ncode != pOldData->List[j][k].ncode)
      {
        sprintf(source, "K%d_%d=%d,", j + 1, k, pNewData->List[j][k].ncode);
        strcat_s(buffer, 0x800uLL, source);
      }
      if (pNewData->List[j][k].wcount != pOldData->List[j][k].wcount)
      {
        sprintf(source, "N%d_%d=%d,", j + 1, k, pNewData->List[j][k].wcount);
        strcat_s(buffer, 0x800uLL, source);
      }
    }
  }

  if (strlen_0(buffer) <= static_cast<size_t>(baseLen))
  {
    memset_0(buffer, 0, baseLen);
  }
  else
  {
    sprintf_s(source, 0x80uLL, "WHERE serial=%d", nDBSerial);
    buffer[strlen_0(buffer) - 1] = ' ';
    strcat_s(buffer, 0x800uLL, source);
    m_pWorldDB->ExecUpdateQuery(buffer, true);
  }
  return 0;
}

unsigned __int8 CMainThread::_db_Select_RegeAvator_For_Lobby_Logout(char *pData)
{
  _rege_char_data regeCharData{};
  unsigned __int8 result = m_pWorldDB->Select_RegeAvator_For_Lobby_Logout(
    *reinterpret_cast<unsigned int *>(pData),
    &regeCharData);
  pData[68] = static_cast<char>(result);
  if (pData[68])
  {
    return 24;
  }

  *reinterpret_cast<int *>(pData + 72) = regeCharData.nCharNum;
  for (int j = 0; j < *reinterpret_cast<int *>(pData + 72); ++j)
  {
    pData[40 * j + 76] = static_cast<char>(regeCharData.RegeList[j].bySlotIndex);
    *reinterpret_cast<unsigned int *>(&pData[40 * j + 80]) = regeCharData.RegeList[j].dwCharSerial;
    *reinterpret_cast<int *>(&pData[40 * j + 104]) = regeCharData.RegeList[j].nLevel;
    *reinterpret_cast<unsigned int *>(&pData[40 * j + 108]) = regeCharData.RegeList[j].dwDalant;
    *reinterpret_cast<unsigned int *>(&pData[40 * j + 112]) = regeCharData.RegeList[j].dwGold;
    strcpy_s(&pData[40 * j + 84], 0x11uLL, regeCharData.RegeList[j].szCharName);
  }
  return 0;
}

char CMainThread::_db_Update_Inven(
  unsigned int dwSerial,
  _AVATOR_DATA *pNewData,
  _AVATOR_DATA *pOldData,
  char *pSzQuery)
{
  char source[136]{};
  char *buffer = pSzQuery;
  sprintf(pSzQuery, "UPDATE tbl_inven SET ");
  unsigned int baseLen = static_cast<unsigned int>(strlen_0(buffer));
  unsigned __int8 maxSlots = static_cast<unsigned __int8>(20 * pNewData->dbAvator.m_byBagNum);

  for (int j = 0; j < maxSlots; ++j)
  {
    if (pNewData->dbInven.m_List[j].Key.IsFilled())
    {
      if (pOldData->dbInven.m_List[j].Key.IsFilled())
      {
        int newKey = pNewData->dbInven.m_List[j].Key.CovDBKey();
        int oldKey = pOldData->dbInven.m_List[j].Key.CovDBKey();
        if (newKey != oldKey)
        {
          sprintf(source, "K%d=%d,", j, newKey);
          strcat_0(buffer, source);
        }
        if (pNewData->dbInven.m_List[j].dwDur != pOldData->dbInven.m_List[j].dwDur)
        {
          sprintf(source, "D%d=%I64d,", j, pNewData->dbInven.m_List[j].dwDur);
          strcat_0(buffer, source);
        }
        if (pNewData->dbInven.m_List[j].dwUpt != pOldData->dbInven.m_List[j].dwUpt)
        {
          sprintf(source, "U%d=%d,", j, pNewData->dbInven.m_List[j].dwUpt);
          strcat_0(buffer, source);
        }
        if (pNewData->dbInven.m_List[j].byCsMethod)
        {
          unsigned int newTime = 0;
          unsigned int oldTime = 0;
          if (pNewData->dbInven.m_List[j].byCsMethod == 1)
          {
            newTime = pNewData->dbInven.m_List[j].dwT - g_Main.m_tmDbUpdate;
            oldTime = pOldData->dbInven.m_List[j].dwT - g_Main.m_tmDbUpdate;
            sprintf(source, "T%d=%d,", j, newTime);
            strcat_0(buffer, source);
          }
          else if (pNewData->dbInven.m_List[j].byCsMethod == 2)
          {
            newTime = pNewData->dbInven.m_List[j].dwT;
            oldTime = pOldData->dbInven.m_List[j].dwT;
            if (newTime != oldTime)
            {
              sprintf(source, "T%d=%d,", j, newTime);
              strcat_0(buffer, source);
            }
          }
        }
        if (pNewData->dbInven.m_List[j].lnUID != pOldData->dbInven.m_List[j].lnUID)
        {
          sprintf(source, "S%d=%I64d,", j, pNewData->dbInven.m_List[j].lnUID);
          strcat_0(buffer, source);
        }
      }
      else
      {
        unsigned int newTime = 0;
        if (pNewData->dbInven.m_List[j].byCsMethod == 1)
        {
          newTime = pNewData->dbInven.m_List[j].dwT - g_Main.m_tmDbUpdate;
        }
        else if (pNewData->dbInven.m_List[j].byCsMethod == 2)
        {
          newTime = pNewData->dbInven.m_List[j].dwT;
        }
        int newKey = pNewData->dbInven.m_List[j].Key.CovDBKey();
        sprintf(
          source,
          "K%d=%d,D%d=%I64d,U%d=%d,T%d=%d,S%d=%I64d,",
          j,
          newKey,
          j,
          pNewData->dbInven.m_List[j].dwDur,
          j,
          pNewData->dbInven.m_List[j].dwUpt,
          j,
          newTime,
          j,
          pNewData->dbInven.m_List[j].lnUID);
        strcat_0(buffer, source);
      }
    }
    else if (pOldData->dbInven.m_List[j].Key.IsFilled())
    {
      int newKey = pNewData->dbInven.m_List[j].Key.CovDBKey();
      sprintf(source, "K%d=%d,", j, newKey);
      strcat_0(buffer, source);
    }
  }

  if (strlen_0(buffer) <= baseLen)
  {
    memset_0(buffer, 0, baseLen);
  }
  else
  {
    sprintf(source, "WHERE Serial=%d", dwSerial);
    buffer[strlen_0(buffer) - 1] = ' ';
    strcat_0(buffer, source);
  }
  return 1;
}

unsigned __int8 CMainThread::_db_Update_Data_For_Post_Send(char *pSheet)
{
  unsigned int *serial = reinterpret_cast<unsigned int *>(pSheet);
  if (m_pWorldDB->Update_Gold(serial[0], serial[1]))
  {
    char query[2048]{};
    if (_db_Update_Inven(
          serial[0],
          reinterpret_cast<_AVATOR_DATA *>(serial[2]),
          reinterpret_cast<_AVATOR_DATA *>(serial[3]),
          query))
    {
      if (m_pWorldDB->ExecUpdateQuery(query, true))
      {
        return 0;
      }
      return 24;
    }
    m_logSystemError.Write( "_db_Update_Inven..failed ..", serial[0]);
    return 24;
  }
  m_logSystemError.Write( "_db_Update_Glod(sr:%d) ..failed ..", serial[0]);
  return 24;
}

unsigned __int8 CMainThread::_db_Update_Data_For_Trade(char *pSheet)
{
  for (int i = 0; i < 2; ++i)
  {
    const char *entry = pSheet + 32 * i;
    unsigned int serial = *reinterpret_cast<const unsigned int *>(entry);
    unsigned int dalant = *reinterpret_cast<const unsigned int *>(entry + 4);
    unsigned int gold = *reinterpret_cast<const unsigned int *>(entry + 8);
    _AVATOR_DATA *newData = *reinterpret_cast<_AVATOR_DATA *const *>(entry + 16);
    _AVATOR_DATA *oldData = *reinterpret_cast<_AVATOR_DATA *const *>(entry + 24);

    if (!m_pWorldDB->Update_Dalant(serial, dalant))
    {
      m_logSystemError.Write( "Update_Dalant(sr:%d) ..failed ..", serial);
      return 24;
    }
    if (!m_pWorldDB->Update_Gold(serial, gold))
    {
      m_logSystemError.Write( "_db_Update_Glod(sr:%d) ..failed ..", serial);
      return 24;
    }

    char query[2048]{};
    if (!_db_Update_Inven(serial, newData, oldData, query))
    {
      m_logSystemError.Write( "_db_Update_Inven..failed ..", serial);
      return 24;
    }
    if (!m_pWorldDB->ExecUpdateQuery(query, true))
    {
      return 24;
    }
  }
  return 0;
}

bool CMainThread::_db_Update_MacroData(
  unsigned int dwSerial,
  _AIOC_A_MACRODATA *pMacro,
  _AIOC_A_MACRODATA *pOldMacro)
{
  if (!pMacro)
  {
    return 0;
  }
  if (pOldMacro)
  {
    int changed = 1;
    for (int j = 0; j < 1; ++j)
    {
      for (int k = 0; k < 3; ++k)
      {
        if (pMacro->mcr_Potion[j].Potion[k] != pOldMacro->mcr_Potion[j].Potion[k]
            || pMacro->mcr_Potion[j].PotionValue[k] != pOldMacro->mcr_Potion[j].PotionValue[k])
        {
          changed = 0;
          break;
        }
      }
    }
    for (int j = 0; j < 3 && changed == 1; ++j)
    {
      for (int k = 0; k < 10; ++k)
      {
        if (pMacro->mcr_Action[j].Action[k] != pOldMacro->mcr_Action[j].Action[k])
        {
          changed = 0;
          break;
        }
      }
    }
    for (int j = 0; j < 2 && changed == 1; ++j)
    {
      for (int k = 0; k < 5; ++k)
      {
        if (strcmp_0(pMacro->mcr_Chat[j].Chat[k], pOldMacro->mcr_Chat[j].Chat[k]))
        {
          changed = 0;
          break;
        }
      }
    }
    if (changed == 1)
    {
      return 1;
    }
  }
  return m_pWorldDB->Update_MacroData(dwSerial, pMacro);
}
