#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CFrameRate.h"
#include "CNetIndexList.h"

class __cppobj CMgrAvatorQuestHistory
{
public:
  CMgrAvatorQuestHistory();
  ~CMgrAvatorQuestHistory();
  static void __cdecl IOThread(void *pv);

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
