#pragma once

#include "IdaCompat.h"
#include "CNetIndexList.h"
#include "starting_vote_inform_zocl.h"

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

    _started_vote_inform_zocl();
    __int64 size();
  };

  CVoteSystem();
  ~CVoteSystem();

  char StartVote(char *pwszContent, unsigned __int8 byLimGrade, unsigned __int8 byRaceCode);
  char StartVote(
    unsigned __int8 byRaceCode,
    unsigned __int8 byPunishType,
    char *pwszContent,
    char *pwszName,
    unsigned int dwSerial);
  void CompleteSelectCharSerial(char *pData);
  char ActVote(unsigned int dwAvatorSerial, unsigned __int8 byPoint);
  void Loop();
  char StopVote();
  void SendMsg_StartedVoteInform(unsigned int n, unsigned int dwAvatorSerial, bool bPunish);
  void ProcessPunishment();

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
