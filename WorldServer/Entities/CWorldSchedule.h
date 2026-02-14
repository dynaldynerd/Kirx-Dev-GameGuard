#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"
#include "CMyTimer.h"

struct _WorldSchedule_fld;

class __cppobj CWorldSchedule
{
public:
  CWorldSchedule();
  ~CWorldSchedule();

  bool Init();
  void Loop();
  void CheckSch();
  bool DataCheck();
  int CalcScheduleCursor(int nHour, int nMin);
  void ChangeSchCursor(_WorldSchedule_fld *pFld, int nPassMin);
  void PassOneStep();

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
