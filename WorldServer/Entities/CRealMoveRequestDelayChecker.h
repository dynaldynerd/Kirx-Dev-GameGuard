#pragma once

#include "IdaCompat.h"

/* 1735 */
class __cppobj __declspec(align(8)) CRealMoveRequestDelayChecker
{
public:
  CIndexList m_kNodeInxOrderList;
  std::vector<unsigned long> m_vecDelayList;
  unsigned __int16 m_wTotalMissCount;
  bool m_bPrevRet;
  unsigned __int16 m_wContinueMissCount;
  unsigned __int16 m_wSingleMissCount;
  unsigned __int16 m_wContinueValiedCount;
  unsigned __int16 m_wTotalContinueValiedCount;

  bool Init(unsigned int listCount);
};

