#pragma once

#include "CRFNewDatabase.h"
#include "unmannedtrader_page_info.h"

/* 1133 */
class __cppobj CRFWorldDatabase : public CRFNewDatabase
{
public:
  CRFWorldDatabase();
  virtual ~CRFWorldDatabase();

  bool Insert_RenamePotionLog(unsigned int dwSerial, char *wszOldName, char *wszNewName);
  bool Insert_SettlementOwnerLog(
    unsigned __int8 byNth,
    unsigned __int8 byRace,
    unsigned int dwGuildSerial,
    const char *wszGuildName,
    unsigned __int16 wRank,
    unsigned __int8 byGrade,
    long double dKillPvpPoint,
    long double dGuildBattlePvpPoint,
    unsigned int dwSumLv);
  unsigned __int8 Select_UnmannedTraderSearchGroupTotalRowCount(
    unsigned __int8 byType,
    unsigned __int8 byRace,
    unsigned __int8 byClass1,
    unsigned __int8 byClass2,
    unsigned __int8 byClass3,
    unsigned int *dwCount);
  unsigned __int8 Select_UnmannedTraderSearchPageInfo(
    unsigned __int8 byType,
    unsigned __int8 byRace,
    unsigned __int8 byClass1,
    unsigned __int8 byClass2,
    unsigned __int8 byClass3,
    unsigned int dwMaxRowCount,
    unsigned int dwExcludeRowCount,
    const char *szSortQuery,
    _unmannedtrader_page_info *pkInfo);
};

