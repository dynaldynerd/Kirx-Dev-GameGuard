#pragma once

#include "IdaCompat.h"

/* 1273 */
class __cppobj CConnNumPHMgr
{
public:
  struct __cnt_per_h
  {
    int nUserCumPerMin;
    unsigned int dwCounting;
    int nMaxUserNum;

    void Init();
    int GetAverageUserNumPerHour();
  };

  void Init();
  _USER_NUM_SHEET *Check(int nLoginNum);
  int GetCurHour();

  __cnt_per_h m_cph;
  _USER_NUM_SHEET m_LastResult;
  int m_nLastHour;
  unsigned int m_dwCheckLastTime;
};

