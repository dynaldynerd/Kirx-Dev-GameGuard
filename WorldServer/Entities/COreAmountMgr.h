#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"
#include <Windows.h>

/* 5395 */
class __cppobj COreAmountMgr
{
  int m_bChangeRemainRate;
  unsigned int m_dwTotalOreSet;
  unsigned int m_dwMinOreSet;
  float m_fMultipleRate[7][3];
  unsigned int m_dwTotalOreAmount;
  unsigned int m_dwRemainOreAmount;
  unsigned int m_uiProccessIndex;
  unsigned __int8 m_byDepositRate;
  _FILETIME m_ftWrite;
  unsigned int m_dwOreTransferAmount;
  unsigned __int8 m_byOreTransferCount;
  bool m_bCheckExhOreLog;
  CLogFile m_logOreAmount;
};
