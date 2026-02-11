#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"
#include "CPostData.h"

class __cppobj CPostSystemManager
{
public:
  static CPostSystemManager *Instace();
  bool Init();
  bool Load();
  bool InitLogger();
  char UpdateDisappearOwnerRecord();
  char InsertDefaultPSRecord();
  char PostRegistryLoad();
  unsigned __int8 UpdateRegist(char *pData);
  unsigned __int8 PostSend(char *pData);
  unsigned __int8 PostReceiverCheck(char *pData);
  void CompleteRegist(char *pData);
  void CompleteSend(char *pData);
  void CompletePostReceiverCheck(char *pData);
  void Log(const char *fmt, ...);
  void Log(const wchar_t *fmt, ...);

  unsigned __int8 m_byRegiNum;
  unsigned __int8 m_byProcNum;
  CMyTimer m_tmrRegiTime;
  CMyTimer m_tmrProcTime;
  CNetIndexList m_listEmpty;
  CNetIndexList m_listRegist;
  CNetIndexList m_listProc;
  CLogFile *m_pkLogger;
  CPostData *m_PostData;
  __int64 m_tNextWrite;
  int m_nPostProcCountPerDay;
  int m_nPostReturnCountPerDay;
};
