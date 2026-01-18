#pragma once

#include "IdaCompat.h"
#include "CCharacter.h"

/* 1746 */
class __cppobj CTrap : CCharacter
{
  int m_nHP;
  CPlayer *m_pMaster;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwMasterSerial;
  char m_wszMasterName[17];
  char m_aszMasterName[17];
  long double m_dMasterPvPPoint;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bBreakTransparBuffer;
  unsigned int m_dwLastDestroyTime;
  int m_nTrapMaxAttackPnt;
};

