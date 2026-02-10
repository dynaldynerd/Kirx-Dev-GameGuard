#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CNetIndexList.h"

class __cppobj __declspec(align(8)) CMgrGuildHistory
{
public:
  CMgrGuildHistory();
  ~CMgrGuildHistory();

  static void IOThread(void *pv);
  void WriteFile(const char *pszFileName, const char *pszLog);
  void push_money(
    const char *pszIOerName,
    unsigned int dwIOerSerial,
    int nPushDalant,
    int nPushGold,
    long double dTotalDalant,
    long double dTotalGold,
    const char *pszFileName);

  struct __LOG_DATA
  {
    char szFileName[128];
    int nLen;
    char sData[300];
  };

  char m_szStdPath[128];
  unsigned int m_dwLastLocalDate;
  unsigned int m_dwLastLocalHour;
  CMyTimer m_tmrUpdateTime;
  char m_szCurTime[32];
  __LOG_DATA m_LogData[2532];
  CNetIndexList m_listLogData;
  CNetIndexList m_listLogDataEmpty;
  bool m_bIOThread;
};
