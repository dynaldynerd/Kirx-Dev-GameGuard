#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"
#include "CMyTimer.h"

class __cppobj CWorldSchedule
{
public:
  static bool Init(CWorldSchedule *pSch);
  bool DataCheck();
  int CalcScheduleCursor(int nHour, int nMin);

  bool m_bOper;
  CRecordData m_tblSch;
  int m_nMaxSchNum;
  int m_nSchCursor;
  int m_nCurHour;
  int m_nCurMin;
  int m_nCurMilSec;
  unsigned int m_dwLastCheckTime;
  CMyTimer m_tmrCheck;
};
