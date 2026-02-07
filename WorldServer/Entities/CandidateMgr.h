#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

class __cppobj CandidateMgr
{
public:
  static CandidateMgr *Instance();
  bool Initialize(int maxCount);

  struct __cppobj _candidate_info
  {
    enum Status : int
    {
      candidate_normal = 0x0,
      candidate_1st = 0x1,
      candidate_2st = 0x2,
      candidate_appoint = 0x3,
      candidate_discharge = 0x4,
      candidate_delete = 0x5,
      candidate_type_num = 0x6,
    };

    enum ClassType : int
    {
      patriarch = 0x0,
      attack_leader_a = 0x2,
      defence_leader_a = 0x3,
      support_leader_a = 0x4,
      attack_leader_b = 0x6,
      defence_leader_b = 0x7,
      support_leader_b = 0x8,
      patriarch_group_num = 0x9,
      normal_user = 0xFF,
    };

    bool bLoad;
    bool bUpdateClassType;
    bool bRefund;
    Status eStatus;
    ClassType eClassType;
    unsigned __int8 byRace;
    unsigned __int8 byLevel;
    unsigned int dwRank;
    unsigned int dwAvatorSerial;
    unsigned int dwGuildSerial;
    char wszName[17];
    char wszGuildName[17];
    long double dPvpPoint;
    unsigned int dwWinCnt;
    unsigned int dwScore;
    bool bValidChar;
    unsigned __int8 byGrade;

    _candidate_info();
    void _Init();
  };

  int m_nMaxNum;
  _candidate_info *m_kCandidate[3];
  _candidate_info *m_kCandidate_old[3];
  _candidate_info *m_kPatriarchGroup[3];
  int m_nCandidateCnt_1st[3];
  _candidate_info *m_pkCandidateLink_1st[3][500];
  int m_nCandidateCnt_2st[3];
  _candidate_info *m_pkCandidateLink_2st[3][8];
  _candidate_info *m_pkLeader[3][9];
  CLogFile m_kSysLog;
  CLogFile m_kVoteResultLog;
};
