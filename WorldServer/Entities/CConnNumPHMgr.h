#pragma once

#include "IdaCompat.h"

/* 1273 */
class __cppobj CConnNumPHMgr
{
  struct __cnt_per_h
  {
    int nUserCumPerMin;
    unsigned int dwCounting;
    int nMaxUserNum;
  };

  __cnt_per_h m_cph;
  _USER_NUM_SHEET m_LastResult;
  int m_nLastHour;
  unsigned int m_dwCheckLastTime;
};

