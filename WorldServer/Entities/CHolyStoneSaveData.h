#pragma once

#include "IdaCompat.h"

class CHolyStoneSaveData
{
public:
  int m_nSceneCode;
  unsigned int m_dwPassTimeInScene;
  int m_nStartStoneHP;
  int m_nHolyMasterRace;
  int m_nDestroyStoneRace;
  unsigned __int8 m_byNumOfTime;
  unsigned int m_dwCumPlayerNum;
  unsigned int m_dwCumCount;
  unsigned __int16 m_wStartYear;
  unsigned __int8 m_byStartMonth;
  unsigned __int8 m_byStartDay;
  unsigned __int8 m_byStartHour;
  unsigned __int8 m_byStartMin;
  unsigned int m_dwDestroyerSerial;
  int m_eDestroyerState;
  int m_nStoneHP_Buffer[3];
  unsigned int m_dwTerm[2];
  unsigned int m_dwOreRemainAmount;
  unsigned int m_dwOreTotalAmount;
  unsigned int m_dwDestroyerGuildSerial;
  unsigned __int8 m_byOreTransferCount;
  unsigned int m_dwOreTransferAmount;
};
