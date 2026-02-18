#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CNetIndexList.h"

class CGuild;

class  __declspec(align(8)) CMgrGuildHistory
{
public:
  CMgrGuildHistory();
  ~CMgrGuildHistory();

  static void IOThread(void *pv);
  void WriteFile(const char *pszFileName, const char *pszLog);
  void GetNewFileName(unsigned int dwGuildSerial, char *pszFileName);
  void start_guild(CGuild *pGuild, char *pszFileName);
  void join_member(
    char *pszJoinerName,
    unsigned int dwJoinerSerial,
    char *pszOKerName,
    unsigned int dwOKSerial,
    int nMemNum,
    char *pszFileName);
  void leave_member(
    char *pszLeaverName,
    unsigned int dwLeaverSerial,
    bool bSelf,
    int nMemNum,
    char *pszFileName,
    bool bPunish);
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
