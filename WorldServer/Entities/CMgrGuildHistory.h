#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CNetIndexList.h"

class CGuild;
struct _suggested_matter;

class  CMgrGuildHistory
{
public:
  CMgrGuildHistory();
  ~CMgrGuildHistory();

  static void IOThread(void *pv);
  void OnLoop();
  int GetTotalWaitSize();
  void WriteFile(const char *pszFileName, const char *pszLog);
  void GetNewFileName(unsigned int dwGuildSerial, char *pszFileName);
  void load_guild(CGuild *pGuild, char *pszFileName);
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
  void pop_money(
    char *pszIOerName,
    unsigned int dwIOerSerial,
    int nPopDalant,
    int nPopGold,
    long double dTotalDalant,
    long double dTotalGold,
    char *pszFileName);
  void suggest_vote(
    char *pszSugerName,
    unsigned int dwSugerSerial,
    _suggested_matter *pMatter,
    char *pszFileName);
  void suggest_complete(
    char *pszSugerName,
    unsigned int dwSugerSerial,
    _suggested_matter *pMatter,
    bool bPass,
    char *pszFileName);
  void suggest_cancel(
    char *pszSugerName,
    unsigned int dwSugerSerial,
    _suggested_matter *pMatter,
    char *pszFileName);
  void change_atrade_taxrate(
    char *pszSugerName,
    unsigned int dwSugerSerial,
    unsigned __int8 byCurTax,
    unsigned __int8 byNextTax,
    char *pszFileName);

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

