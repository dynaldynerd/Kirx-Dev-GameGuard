#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"

struct _QUEST_DB_BASE;

class  CMgrAvatorQuestHistory
{
public:
  CMgrAvatorQuestHistory();
  ~CMgrAvatorQuestHistory();
  static void __cdecl IOThread(void *pv);
  void OnLoop();
  int GetTotalWaitSize();
  void WriteFile(char *pszFileName, char *pszLog);
  void GetNewFileName(unsigned int dwAvatorSerial, char *pszFileName);
  void init_quest(char *pszAvatorName, _QUEST_DB_BASE *pQuestDB, char *pszFileName);
  void insert_quest(unsigned int nSlot, char *pszQuestCode, char *pszFileName);
  void complete_quest(unsigned int nSlot, char *pszQuestCode, char *pszFileName);
  void fail_quest(unsigned int nSlot, char *pszQuestCode, char *pszFailCode, char *pszFileName);
  void char_copy(char *pszDstName, unsigned int dwDstSerial, char *pszFileName);

  struct __LOG_DATA
  {
    char szFileName[64];
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
  CFrameRate m_FrameRate;
};
