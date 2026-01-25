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
