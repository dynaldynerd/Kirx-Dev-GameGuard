#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"

class CPartyPlayer;
struct _MASTERY_PARAM;

class __cppobj CMgrAvatorLvHistory
{
public:
  struct __LOG_DATA
  {
    char szFileName[64];
    int nLen;
  };

  struct __cppobj __LOG_DATA_2K : public __LOG_DATA
  {
    char sData[2000];
  };

  struct __cppobj __LOG_DATA_1K : public __LOG_DATA
  {
    char sData[1000];
  };

  struct __cppobj __LOG_DATA_200 : public __LOG_DATA
  {
    char sData[200];
  };

  void alter_pvp(int n, long double dPvpVariation, CPartyPlayer *pParty, char *pszFileName);
  void GetNewFileName(unsigned int dwAvatorSerial, char *pszFileName);
  void start_mastery(
    int n,
    char *pszAvatorName,
    unsigned int dwLv,
    long double dExp,
    unsigned int dwExpRate,
    int nGrade,
    int *pnMaxPoint,
    _MASTERY_PARAM *pData,
    char *pszFileName);
  void update_mastery(
    int n,
    unsigned __int8 byUserDgr,
    unsigned int dwLv,
    long double dExp,
    unsigned int dwExpRate,
    int nGrade,
    int *pnMaxPoint,
    _MASTERY_PARAM *pData,
    unsigned int *pdwAlter,
    char *pszFileName,
    unsigned __int8 byLogType,
    char *pszTitle);
  void WriteFile(char *pszFileName, char *pszLog);

  char m_szStdPath[128];
  unsigned int m_dwLastLocalDate;
  unsigned int m_dwLastLocalHour;
  CMyTimer m_tmrUpdateTime;
  char m_szCurDate[32];
  char m_szCurTime[32];
  __LOG_DATA_2K m_LogData_2K[254];
  CNetIndexList m_listLogData_2K;
  CNetIndexList m_listLogDataEmpty_2K;
  __LOG_DATA_1K m_LogData_1K[254];
  CNetIndexList m_listLogData_1K;
  CNetIndexList m_listLogDataEmpty_1K;
  __LOG_DATA_200 m_LogData_200[2532];
  CNetIndexList m_listLogData_200;
  CNetIndexList m_listLogDataEmpty_200;
  bool m_bIOThread;
  CFrameRate m_FrameRate;
};
