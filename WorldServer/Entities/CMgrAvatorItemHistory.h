#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"
#include "StorageList.h"

class __cppobj CMgrAvatorItemHistory
{
public:
  struct __LOG_DATA
  {
    char szFileName[64];
    int nLen;
  };

  struct __cppobj __LOG_DATA_10K : public __LOG_DATA
  {
    char sData[10000];
  };

  struct __cppobj __LOG_DATA_1K : public __LOG_DATA
  {
    char sData[1000];
  };

  struct __cppobj __LOG_DATA_200 : public __LOG_DATA
  {
    char sData[200];
  };

  CMgrAvatorItemHistory();
  static void IOThread(void *pv);
  void WriteFile(const char *pszFileName, const char *pszLog);
  void consume_del_item(int n, _STORAGE_LIST::_db_con *pItem, const char *pszFileName);
  void reward_add_item(int n, char *pszClause, _STORAGE_LIST::_db_con *pItem, char *pszFileName);
  void personal_amine_itemlog(
    const char *szLogDesc,
    unsigned __int8 byPos,
    unsigned __int8 byTblCode,
    unsigned __int16 wItemIndex,
    unsigned int dwDur,
    const char *szFileName);
  void personal_amine_uninstall(
    unsigned __int8 byType,
    const unsigned int *pdwMineCnt,
    int nMaxOreNum,
    _STORAGE_LIST::_db_con *pItem,
    const char *szFileName);
  void lenditem_del_from_inven(
    unsigned __int8 byTblCode,
    unsigned __int16 wItemIndex,
    unsigned __int64 lnUID,
    char *pFN);
  void time_jade_effect_log(
    char *pszItemName,
    _STORAGE_LIST::_db_con *pItem,
    bool bAdd,
    char *pszFileName);
  void exp_prof_log(int count, char *szFile);
  void mastery_change_jade(
    int nMstCode,
    unsigned int dwOldCum,
    unsigned int dwNewCum,
    int nLv,
    float fVal,
    char *szFileName,
    int nWpType);

  char m_szStdPath[128];
  unsigned int m_dwLastLocalDate;
  unsigned int m_dwLastLocalHour;
  CMyTimer m_tmrUpdateTime;
  char m_szCurDate[32];
  char m_szCurTime[32];
  __LOG_DATA_10K m_LogData_10K[254];
  CNetIndexList m_listLogData_10K;
  CNetIndexList m_listLogDataEmpty_10K;
  __LOG_DATA_1K m_LogData_1K[254];
  CNetIndexList m_listLogData_1K;
  CNetIndexList m_listLogDataEmpty_1K;
  __LOG_DATA_200 m_LogData_200[2532];
  CNetIndexList m_listLogData_200;
  CNetIndexList m_listLogDataEmpty_200;
  bool m_bIOThread;
  CFrameRate m_FrameRate;
};
