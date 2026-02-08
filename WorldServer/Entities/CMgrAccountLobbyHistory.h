#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"

struct _qry_case_lobby_logout;
struct _REGED;

class __cppobj CMgrAccountLobbyHistory
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

  CMgrAccountLobbyHistory();
  ~CMgrAccountLobbyHistory();
  void enter_lobby(
    unsigned int dwAccountSerial,
    char *pAccountID,
    unsigned __int8 byUserDgr,
    unsigned int dwIpAddress,
    bool bFirst,
    char *pszFileName);
  void lobby_disconnect(_qry_case_lobby_logout *pRegeData, char *pszFileName);
  void recovery_char_complete(unsigned __int8 byRetCode, _REGED *pAvator, char *pszFileName);
  void WriteFile(char *pszFileName, char *pszLog);
  void OnLoop();
  static void IOThread(char *pv);
};
