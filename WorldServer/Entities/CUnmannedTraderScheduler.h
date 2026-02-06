#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include "CMyTimer.h"
#include "CUnmannedTraderSchedule.h"
#include <vector>

class __cppobj CUnmannedTraderScheduler
{
public:
  static CUnmannedTraderScheduler *Instance();

  CUnmannedTraderScheduler();
  bool Init();
  void SetLogger(CLogFile *pkLogger);

  static CUnmannedTraderScheduler *ms_Instance;

  int m_iOldDay;
  bool m_bLoad;
  CMyTimer *m_pkTimer;
  CLogFile *m_pkLogger;
  std::vector<CUnmannedTraderSchedule>::iterator m_iterSchedule;
  std::vector<CUnmannedTraderSchedule> m_veckSchdule;
};
