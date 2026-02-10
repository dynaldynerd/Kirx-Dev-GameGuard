#pragma once

#include "IdaCompat.h"

class CMyTimer;

class __cppobj CUnmannedTraderLazyCleaner
{
public:
  CUnmannedTraderLazyCleaner();
  ~CUnmannedTraderLazyCleaner();
  bool Init();
  void Loop();
  unsigned __int8 UpdateClear(bool *p);
  unsigned __int8 ProcUpdate(unsigned __int8 byState, const _SYSTEMTIME *pCurTime, bool *pbRemain);
  void CompleteUpdateClear(char *p);

  CMyTimer *m_pkTimer;
  bool m_bClearProcess;
  unsigned int m_uiRetryCnt;
};
