#pragma once

#include "IdaCompat.h"
#include "CNetIndexList.h"
#include "starting_vote_inform_zocl.h"
#include "Packet/ZoneClientPacket.h"

struct _qry_case_select_charserial;

class  CVoteSystem
{
public:
  CVoteSystem();
  ~CVoteSystem();

  bool StartVote(char *pwszContent, unsigned __int8 byLimGrade, unsigned __int8 byRaceCode);
  bool StartVote(
    unsigned __int8 byRaceCode,
    unsigned __int8 byPunishType,
    char *pwszContent,
    char *pwszName,
    unsigned int dwSerial);
  void CompleteSelectCharSerial(const _qry_case_select_charserial *query);
  bool ActVote(unsigned int dwAvatorSerial, unsigned __int8 byPoint);
  void Loop();
  bool StopVote();
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
