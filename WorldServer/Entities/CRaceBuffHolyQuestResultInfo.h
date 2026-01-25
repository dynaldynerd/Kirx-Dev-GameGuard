#pragma once

#include "IdaCompat.h"

class __cppobj CRaceBuffHolyQuestResultInfo
{
public:
  bool Load();
  bool LoadINI();
  bool SaveINI();
  bool SaveINISubProcSaveNum(const char *szItem, unsigned int value);

  bool m_bSetBuff;
  unsigned __int8 m_byCurWinRace;
  unsigned __int8 m_byCurFailRace;
  unsigned __int8 m_byCurLoseRace;
  unsigned __int8 m_byOldWinRace;
  unsigned __int8 m_byOldFailRace;
  unsigned __int8 m_byOldLoseRace;
  unsigned int m_uiContinueWinCnt;
  unsigned int m_uiContinueFailCnt;
  unsigned int m_uiContinueLoseCnt;
};
