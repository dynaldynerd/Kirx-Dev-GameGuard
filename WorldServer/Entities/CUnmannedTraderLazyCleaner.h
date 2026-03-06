#pragma once

#include "IdaCompat.h"

class CMyTimer;
struct _qry_case_unmandtrader_lazyclean_flags;

class  CUnmannedTraderLazyCleaner
{
public:
  CUnmannedTraderLazyCleaner();
  ~CUnmannedTraderLazyCleaner();
  bool Init();
  void Loop();
  unsigned __int8 UpdateClear(_qry_case_unmandtrader_lazyclean_flags *pFlags);
  unsigned __int8 ProcUpdate(unsigned __int8 byState, const _SYSTEMTIME *pCurTime, bool *pbRemain);
  void CompleteUpdateClear(_qry_case_unmandtrader_lazyclean_flags *pFlags);

  CMyTimer *m_pkTimer;
  bool m_bClearProcess;
  unsigned int m_uiRetryCnt;
};
