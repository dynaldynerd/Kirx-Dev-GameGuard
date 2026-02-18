#pragma once

#include "IdaCompat.h"

class  CRaceBuffHolyQuestResultInfo
{
public:
  CRaceBuffHolyQuestResultInfo() = default;
  ~CRaceBuffHolyQuestResultInfo() = default;

  bool Load();
  bool LoadINI();
  bool SaveINI();
  bool SaveINISubProcSaveNum(const char *szItem, unsigned int value);
  void ClearResult();
  void SetBuffFlag();
  void SetResult(unsigned __int8 byWinRace, unsigned __int8 byLoseRace, unsigned int uiThMax);
  bool SetResultSubProcSetRace(
    unsigned __int8 byWinRace,
    unsigned __int8 byLoseRace,
    unsigned int uiThMax);
  bool FindFailRace(unsigned __int8 byWinRace, unsigned __int8 byLoseRace, unsigned __int8 *pbyFailRace);
  bool IsValidResult();
  bool IsChaos();
  int GetResultType(unsigned __int8 byRace, bool bGetScanner);
  unsigned int GetContinueCnt(unsigned int iType);

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
