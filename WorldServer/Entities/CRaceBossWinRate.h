#pragma once

#include "IdaCompat.h"

class __cppobj CRaceBossWinRate
{
public:
  CRaceBossWinRate();
  static CRaceBossWinRate *Instance();
  void UpdateTotalCnt();
  void UpdateWinCnt(unsigned __int8 byRace);
  void Notify();

  static CRaceBossWinRate *m_Inst;
  unsigned __int8 m_byTotalBattleCnt;
  unsigned __int8 m_byWinCnt[3];
  unsigned int m_dwAccBattleCnt[3];
  unsigned int m_dwAccWinCnt[3];
};
