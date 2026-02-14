#pragma once

#include "IdaCompat.h"
#include "CFrameRate.h"
#include "CLogFile.h"
#include "CNetIndexList.h"

struct _SYN_DATA;

class __cppobj CProcessThread
{
public:
  struct _THREAD_CONFIG
  {
    _THREAD_CONFIG();

    char szThreadID[128];
    bool bProgramExitWhenLackData;
    char szLogFileName[128];
    int nSleepCount;
  };

  struct __unaligned __declspec(align(4)) _SYN_HEADER
  {
    unsigned int m_dwQryIndex;
    unsigned int m_dwQrySerial;
    unsigned __int8 m_byResult;
    unsigned __int8 m_byQryCase;
  };

  struct __unaligned _SYN_DATA : _SYN_HEADER
  {
    bool m_bUse;
    bool m_bProcess;
    char *m_psData;
  };

  CProcessThread();
  virtual ~CProcessThread();
  virtual void CrashThread();
  virtual unsigned char WorkProcess(_SYN_DATA *pSynData);
  virtual void CompleteProcess(_SYN_DATA *pSynData);

  _SYN_DATA *m_pSynData;
  CNetIndexList m_listData;
  CNetIndexList m_listDataComplete;
  CNetIndexList m_listDataEmpty;
  unsigned int m_dwSynDataNum;
  unsigned int m_dwSynDataSize;
  unsigned int m_dwWorkCount;
  unsigned int m_dwLoopCount;
  bool m_bThread;
  CLogFile m_logSystem;
  CFrameRate m_FrameRate;
  _THREAD_CONFIG m_Config;
};
