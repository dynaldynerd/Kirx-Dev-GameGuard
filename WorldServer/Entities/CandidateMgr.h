#pragma once

#include "IdaCompat.h"
#include "CLogFile.h"

class CPlayer;
struct _PVP_RANK_DATA;
struct _qry_case_discharge_patriarch;
struct _qry_case_insert_candidate;

class  CandidateMgr
{
public:
  CandidateMgr();
  ~CandidateMgr();

  struct  _candidate_info
  {
    enum Status : int
    {
      candidate_normal = 0,
      candidate_1st = 1,
      candidate_2st = 2,
      candidate_appoint = 3,
      candidate_discharge = 4,
      candidate_delete = 5,
      candidate_type_num = 6,
    };

    enum ClassType : int
    {
      patriarch = 0,
      attack_leader_a = 2,
      defence_leader_a = 3,
      support_leader_a = 4,
      attack_leader_b = 6,
      defence_leader_b = 7,
      support_leader_b = 8,
      patriarch_group_num = 9,
      normal_user = 255,
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

  static CandidateMgr *Instance();
  bool Initialize(int maxCount);
  char LoadDatabase();
  bool LoadLeaderPreVersion(unsigned __int8 byRace);
  char LoadPatriarchGroup();
  void Release();
  void InitCandidate();
  _candidate_info *GetEmpty(unsigned __int8 byRace);
  bool Regist(CPlayer *pOne);
  bool Regist(unsigned __int8 byRace, const _PVP_RANK_DATA *pData);
  int GetCandidateCnt_1st(unsigned __int8 byRace);
  int GetCandidateCnt_2st(unsigned __int8 byRace);
  const _candidate_info *GetCandidate(unsigned __int8 byRace, unsigned int dwIdx);
  const _candidate_info *GetLeader(unsigned __int8 byRace, unsigned int nIdx);
  int GetMaxNum();
  unsigned int GetWinCnt(unsigned __int8 byRace, unsigned int dwAvatorSerial);
  bool IsRegistedAvator_1(unsigned __int8 byRace, unsigned int dwAvatorSerial);
  bool IsRegistedAvator_2(unsigned __int8 byRace, unsigned int dwAvatorSerial);
  void AddScore(unsigned __int8 byRace, char *wszName, unsigned __int8 byScore);
  _candidate_info *GetCandidateBySerial(unsigned __int8 byRace, unsigned int dwASerial);
  _candidate_info *GetPatriarchGroup(unsigned __int8 byRace, _candidate_info::ClassType eType);
  _candidate_info *GetPatriarchGroupBySerial(unsigned __int8 byRace, unsigned int dwASerial);
  _candidate_info *GetEmptyPatriarchGroup(unsigned __int8 byRace);
  bool AppointPatriarchGroup(CPlayer *pOne, _candidate_info::ClassType eClassType);
  bool DischargePatriarchGroup(unsigned __int8 byRace, _candidate_info::ClassType eClassType);
  const _candidate_info *GetCandidate_2st(unsigned __int8 byRace, unsigned int nIdx);
  void FinalDecision();
  void ApplyPatriarchGroup();
  void ChangeState_1to2();
  __int64 Update_RegistCandidate_2st();
  __int64 Update_Score();
  __int64 Update_ClassType();
  __int64 Insert_Candidate(unsigned __int8 byQryCase, _qry_case_insert_candidate *p);
  __int64 Update_Refund();
  __int64 Update_DischargePatriarch(_qry_case_discharge_patriarch *p);
  __int64 CheckDBValidCharacter(unsigned __int8 byProc);
  __int64 Update_VoteTime(unsigned int dwSerial);
  void CompleteInsertCandidate(unsigned __int8 byRet, _qry_case_insert_candidate *p);
  __int64 __SortByPvpPoint();
  __int64 __SortByRank();
  __int64 __SortByScore();
  void __AddWinner(unsigned __int8 byRace, unsigned __int8 byNum);

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
