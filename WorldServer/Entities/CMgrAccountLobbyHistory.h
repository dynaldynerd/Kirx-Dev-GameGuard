#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"

struct _qry_case_lobby_logout;
struct _REGED;
struct _REGED_AVATOR_DB;
struct _AVATOR_DATA;

class  CMgrAccountLobbyHistory
{
public:
  struct __LOG_DATA
  {
    char szFileName[64];
    int nLen;
  };

  struct  __LOG_DATA_10K : public __LOG_DATA
  {
    char sData[10000];
  };

  struct  __LOG_DATA_1K : public __LOG_DATA
  {
    char sData[1000];
  };

  struct  __LOG_DATA_200 : public __LOG_DATA
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
  void GetNewFileName(unsigned int dwAccountSerial, char *pszFileName);
  void enter_lobby(
    unsigned int dwAccountSerial,
    char *pAccountID,
    unsigned __int8 byUserDgr,
    unsigned int dwIpAddress,
    bool bFirst,
    char *pszFileName);
  void lobby_disconnect(_qry_case_lobby_logout *pRegeData, char *pszFileName);
  void reged_char_request(char *pszFileName);
  void add_char_request(char *pszFileName);
  void del_char_request(unsigned __int8 bySlotIndex, unsigned int dwAvatorSerial, char *pszFileName);
  void sel_char_request(unsigned __int8 bySlotIndex, unsigned int dwAvatorSerial, char *pszFileName);
  void recovery_char_complete(unsigned __int8 byRetCode, _REGED *pAvator, char *pszFileName);
  void reged_char_complete(unsigned __int8 byRetCode, int nCharNum, _REGED *pRegedList, char *pszFileName);
  void add_char_complete(unsigned __int8 byRetCode, _REGED_AVATOR_DB *pInsertData, char *pszFileName);
  void tutorial_process_report_recv(char *pszFileName);
  void del_char_complete(unsigned __int8 byRetCode, char *pszFileName);
  void sel_char_complete(
    unsigned __int8 byRetCode,
    _AVATOR_DATA *pAvator,
    unsigned int dwAddDalant,
    unsigned int dwAddGold,
    char *pszFileName);
  void player_create(bool bFirstStart, _AVATOR_DATA *pAvator, char *pszFileName);
  void player_create_complete_money(_AVATOR_DATA *pAvator, char *pszFileName);
  void player_money_fix(
    unsigned int dwOldDalant,
    unsigned int dwOldGold,
    _AVATOR_DATA *pAvator,
    char *pszFileName);
  void WriteFile(char *pszFileName, char *pszLog);
  void OnLoop();
  static void IOThread(char *pv);
};
