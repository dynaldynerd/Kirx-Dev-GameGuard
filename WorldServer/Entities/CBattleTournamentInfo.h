#pragma once

#include "IdaCompat.h"

/* 1808 */
class __cppobj CBattleTournamentInfo
{
public:
  CBattleTournamentInfo();
  ~CBattleTournamentInfo();
  void Init();
  void SetLoad(bool bLoad);
  char SetWinnerInfo(unsigned int dwSerial, char *pwszCharName, unsigned __int8 byGrade);
  unsigned __int8 GetWinnerGrade(unsigned int dwSerial, char *pwszCharName);

  bool m_bLoad;
  int m_nCurNum;
  TournamentWinner m_WinnerInfo[48];
};

