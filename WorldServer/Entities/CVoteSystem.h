#pragma once

#include "IdaCompat.h"
#include "CNetIndexList.h"

class __cppobj CVoteSystem
{
public:
  struct __unaligned __declspec(align(1)) _started_vote_inform_zocl
  {
    int nVoteSerial;
    unsigned __int8 byLimGrade;
    unsigned __int16 wLeftSec;
    bool bActed;
    unsigned __int16 wPoint[3];
    bool bHurry;
    unsigned __int16 wContentSize;
    char wszContent[1280];
  };

  bool m_bActive;
  unsigned __int8 m_byRaceCode;
  int m_nSerial;
  bool m_bHurry;
  unsigned int m_dwPoint[3];
  CNetIndexList m_listVote;
  unsigned int m_dwStartVoteTime;
  unsigned int m_dwLastBroadcastTime;
  unsigned __int8 m_byLimGrade;
  _started_vote_inform_zocl m_SendStarted;
  bool m_bPunishment;
  unsigned __int8 m_byPunishType;
  unsigned int m_dwAvatorSerial;
  char m_wszCharName[17];
};
