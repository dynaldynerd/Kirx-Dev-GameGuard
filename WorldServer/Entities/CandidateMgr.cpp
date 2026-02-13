#include "pch.h"

#include "CandidateMgr.h"

#include "CMainThread.h"
#include "CPlayer.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRFWorldDatabase.h"
#include "CUserRankingProcess.h"
#include "ClassOrderProcessor.h"
#include "GlobalObjects.h"
#include "PatriarchElectProcessor.h"
#include "qry_case_insert_candidate.h"
#include "qry_case_request_refund.h"
#include "update_candidate_wincount_packing.h"

#include <new>

namespace
{
#pragma pack(push, 1)
struct _qry_case_discharge_patriarch
{
  unsigned __int8 byRace;
  unsigned int dwAvatorSerial;
  unsigned int dwElectSerial;

  _qry_case_discharge_patriarch(
    unsigned __int8 race,
    unsigned int avatorSerial,
    unsigned int electSerial)
    : byRace(race), dwAvatorSerial(avatorSerial), dwElectSerial(electSerial)
  {
  }

  int size() const
  {
    return sizeof(*this);
  }
};
#pragma pack(pop)
}

CandidateMgr *CandidateMgr::Instance()
{
  static CandidateMgr s_instance;
  return &s_instance;
}

CandidateMgr::_candidate_info::_candidate_info()
{
  _Init();
}

void CandidateMgr::_candidate_info::_Init()
{
  bLoad = false;
  bUpdateClassType = false;
  eStatus = candidate_normal;
  byRace = 0xFF;
  byLevel = 0xFF;
  dwRank = 0xFFFFFFFFu;
  dPvpPoint = 0.0;
  dwGuildSerial = 0xFFFFFFFFu;
  dwAvatorSerial = 0xFFFFFFFFu;
  memset_0(wszGuildName, 0, sizeof(wszGuildName));
  memset_0(wszName, 0, sizeof(wszName));
  dwScore = 0;
  dwWinCnt = 0;
  eClassType = normal_user;
  bValidChar = true;
}

bool CandidateMgr::Initialize(int maxCount)
{
  char filePath[256]{};

  const unsigned int sysTime = GetKorLocalTime();
  sprintf_s(filePath, sizeof(filePath), "..\\ZoneServerLog\\SystemLog\\Patriarch\\CandidateMgr_%d.log", sysTime);
  m_kSysLog.SetWriteLogFile(filePath, true, false, true, true);

  const unsigned int voteTime = GetKorLocalTime();
  sprintf_s(filePath, sizeof(filePath), "..\\ZoneServerLog\\ServiceLog\\Patriarch\\VoteResult_%d.log", voteTime);
  m_kVoteResultLog.SetWriteLogFile(filePath, true, false, true, true);

  m_nMaxNum = maxCount;
  for (int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    m_kCandidate[raceIndex] = new (std::nothrow) _candidate_info[m_nMaxNum];
    if (!m_kCandidate[raceIndex])
    {
      return false;
    }

    m_kCandidate_old[raceIndex] = new (std::nothrow) _candidate_info[m_nMaxNum];
    if (!m_kCandidate_old[raceIndex])
    {
      return false;
    }

    m_kPatriarchGroup[raceIndex] = new (std::nothrow) _candidate_info[9];
    if (!m_kPatriarchGroup[raceIndex])
    {
      return false;
    }
  }

  return true;
}

void CandidateMgr::InitCandidate()
{
  for (int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    m_nCandidateCnt_1st[raceIndex] = 0;
    m_nCandidateCnt_2st[raceIndex] = 0;

    for (int candidateIndex = 0; candidateIndex < 500; ++candidateIndex)
    {
      m_pkCandidateLink_1st[raceIndex][candidateIndex] = nullptr;
      m_kCandidate[raceIndex][candidateIndex]._Init();
    }

    for (int rankIndex = 0; rankIndex < 8; ++rankIndex)
    {
      m_pkCandidateLink_2st[raceIndex][rankIndex] = nullptr;
    }

    for (int leaderIndex = 0; leaderIndex < 9; ++leaderIndex)
    {
      m_pkLeader[raceIndex][leaderIndex] = nullptr;
    }
  }
}

CandidateMgr::_candidate_info *CandidateMgr::GetEmpty(unsigned __int8 byRace)
{
  for (int index = 0; index < m_nMaxNum; ++index)
  {
    if (!m_kCandidate[byRace][index].bLoad)
    {
      return &m_kCandidate[byRace][index];
    }
  }

  return nullptr;
}

CandidateMgr::_candidate_info *CandidateMgr::GetEmptyPatriarchGroup(unsigned __int8 byRace)
{
  for (int index = 0; index < 9; ++index)
  {
    if (!m_kPatriarchGroup[byRace][index].bLoad)
      return &m_kPatriarchGroup[byRace][index];
  }

  return nullptr;
}

bool CandidateMgr::AppointPatriarchGroup(CPlayer *pOne, _candidate_info::ClassType eClassType)
{
  _candidate_info *candidate = GetEmptyPatriarchGroup(pOne->m_Param.GetRaceCode());
  if (candidate == nullptr)
    return false;

  candidate->bLoad = true;
  candidate->byRace = pOne->m_Param.GetRaceCode();
  candidate->byLevel = pOne->m_Param.GetLevel();
  candidate->dwRank = pOne->m_Param.GetPvpRank();
  candidate->dPvpPoint = pOne->m_Param.GetPvPPoint();
  candidate->byGrade = pOne->m_Param.m_byPvPGrade;
  candidate->dwAvatorSerial = pOne->m_Param.GetCharSerial();
  candidate->eClassType = eClassType;
  candidate->eStatus = _candidate_info::candidate_appoint;
  strcpy_s(candidate->wszName, sizeof(candidate->wszName), pOne->m_Param.GetCharNameW());

  if (pOne->m_Param.m_pGuild != nullptr)
  {
    candidate->dwGuildSerial = pOne->m_Param.m_pGuild->m_dwSerial;
    strcpy_s(candidate->wszGuildName, sizeof(candidate->wszGuildName), pOne->m_Param.m_pGuild->m_wszName);
  }
  else
  {
    candidate->dwGuildSerial = 0xFFFFFFFFu;
    memset_0(candidate->wszGuildName, 0, sizeof(candidate->wszGuildName));
  }

  const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetElectSerial();
  _qry_case_insert_candidate query(candidate->byRace, pOne->m_id.wIndex, electSerial, candidate->dwAvatorSerial);
  g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x7Du, reinterpret_cast<char *>(&query), query.size());
  return true;
}

bool CandidateMgr::DischargePatriarchGroup(unsigned __int8 byRace, _candidate_info::ClassType eClassType)
{
  _candidate_info *candidate = GetPatriarchGroup(byRace, eClassType);
  if (candidate == nullptr)
    return false;

  candidate->eStatus = _candidate_info::candidate_discharge;
  ClassOrderProcessor::Instance()->UpdatePacket(byRace, static_cast<unsigned __int8>(eClassType));

  const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetCurrPatriarchElectSerial();
  _qry_case_discharge_patriarch query(candidate->byRace, candidate->dwAvatorSerial, electSerial);
  g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x7Eu, reinterpret_cast<char *>(&query), query.size());
  return true;
}

bool CandidateMgr::Regist(CPlayer *pOne)
{
  const unsigned __int8 raceCode = pOne->m_Param.GetRaceCode();
  _candidate_info *const empty = GetEmpty(raceCode);
  if (empty == nullptr)
  {
    return false;
  }

  empty->bLoad = true;
  empty->byRace = raceCode;
  empty->dwAvatorSerial = pOne->m_Param.GetCharSerial();
  empty->byGrade = pOne->m_Param.m_byPvPGrade;
  empty->dwRank = pOne->m_Param.GetPvpRank();
  empty->dPvpPoint = pOne->m_Param.GetPvPPoint();
  empty->byLevel = pOne->m_Param.GetLevel();
  empty->eStatus = _candidate_info::candidate_1st;
  strcpy_s(empty->wszName, sizeof(empty->wszName), pOne->m_Param.GetCharNameW());

  if (pOne->m_Param.m_pGuild != nullptr)
  {
    empty->dwGuildSerial = pOne->m_Param.m_pGuild->m_dwSerial;
    strcpy_s(empty->wszGuildName, sizeof(empty->wszGuildName), pOne->m_Param.m_pGuild->m_wszName);
  }
  else
  {
    empty->dwGuildSerial = static_cast<unsigned int>(-1);
    memset_0(empty->wszGuildName, 0, sizeof(empty->wszGuildName));
  }

  m_pkCandidateLink_1st[empty->byRace][m_nCandidateCnt_1st[empty->byRace]++] = empty;

  const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetElectSerial();
  _qry_case_insert_candidate query(
    empty->byRace,
    pOne->m_id.wIndex,
    electSerial,
    empty->dwAvatorSerial);
  g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x75u, reinterpret_cast<char *>(&query), query.size());

  return true;
}

bool CandidateMgr::Regist(unsigned __int8 byRace, const _PVP_RANK_DATA *pData)
{
  _candidate_info *const empty = GetEmpty(byRace);
  if (empty == nullptr)
  {
    return false;
  }

  empty->bLoad = true;
  empty->byRace = byRace;
  empty->dwAvatorSerial = pData->dwAvatorSerial;
  empty->dwRank = pData->byRank;
  empty->dPvpPoint = pData->dPvpPoint;
  empty->byLevel = pData->byLv;
  empty->eStatus = _candidate_info::candidate_1st;
  empty->dwGuildSerial = pData->dwGuildSerial;
  empty->byGrade = pData->byGrade;
  strcpy_s(empty->wszName, sizeof(empty->wszName), pData->wszName);
  strcpy_s(empty->wszGuildName, sizeof(empty->wszGuildName), pData->wszGuildName);

  m_pkCandidateLink_1st[empty->byRace][m_nCandidateCnt_1st[empty->byRace]++] = empty;

  const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetElectSerial();
  _qry_case_insert_candidate query(
    empty->byRace,
    0xFFFFu,
    electSerial,
    empty->dwAvatorSerial);
  g_Main.PushDQSData(0xFFFFFFFFu, nullptr, 0x75u, reinterpret_cast<char *>(&query), query.size());

  return true;
}

int CandidateMgr::GetCandidateCnt_1st(unsigned __int8 byRace)
{
  return m_nCandidateCnt_1st[byRace];
}

int CandidateMgr::GetCandidateCnt_2st(unsigned __int8 byRace)
{
  return m_nCandidateCnt_2st[byRace];
}

const CandidateMgr::_candidate_info *CandidateMgr::GetCandidate(unsigned __int8 byRace, unsigned int dwIdx)
{
  if (byRace > 3u || dwIdx > static_cast<unsigned int>(m_nMaxNum))
  {
    return nullptr;
  }

  return &m_kCandidate[byRace][dwIdx];
}

const CandidateMgr::_candidate_info *CandidateMgr::GetLeader(unsigned __int8 byRace, unsigned int nIdx)
{
  if (byRace >= 3u)
  {
    return nullptr;
  }
  if (nIdx < 9u)
  {
    return m_pkLeader[byRace][nIdx];
  }
  return nullptr;
}

int CandidateMgr::GetMaxNum()
{
  return m_nMaxNum;
}

unsigned int CandidateMgr::GetWinCnt(unsigned __int8 byRace, unsigned int dwAvatorSerial)
{
  for (int index = 0; index < m_nMaxNum; ++index)
  {
    if (m_kCandidate[byRace][index].dwAvatorSerial == dwAvatorSerial)
    {
      return m_kCandidate[byRace][index].dwWinCnt;
    }
  }

  return 0;
}

bool CandidateMgr::IsRegistedAvator_1(unsigned __int8 byRace, unsigned int dwAvatorSerial)
{
  for (int index = 0; index < m_nCandidateCnt_1st[byRace]; ++index)
  {
    _candidate_info *const candidate = m_pkCandidateLink_1st[byRace][index];
    if (candidate != nullptr
        && candidate->eStatus == _candidate_info::candidate_1st
        && candidate->dwAvatorSerial == dwAvatorSerial)
    {
      return true;
    }
  }

  return false;
}

bool CandidateMgr::IsRegistedAvator_2(unsigned __int8 byRace, unsigned int dwAvatorSerial)
{
  for (int index = 0; index < m_nCandidateCnt_2st[byRace]; ++index)
  {
    _candidate_info *candidate = m_pkCandidateLink_2st[static_cast<unsigned __int64>(byRace)][index];
    if (candidate != nullptr
        && candidate->eStatus == _candidate_info::candidate_2st
        && candidate->dwAvatorSerial == dwAvatorSerial)
    {
      return true;
    }
  }

  return false;
}

void CandidateMgr::AddScore(unsigned __int8 byRace, char *wszName, unsigned __int8 byScore)
{
  for (int index = 0; index < m_nCandidateCnt_2st[byRace]; ++index)
  {
    _candidate_info *candidate = m_pkCandidateLink_2st[static_cast<unsigned __int64>(byRace)][index];
    if (candidate != nullptr && !strcmp_0(candidate->wszName, wszName))
    {
      candidate->dwScore += byScore;
      return;
    }
  }
}

CandidateMgr::_candidate_info *CandidateMgr::GetCandidateBySerial(unsigned __int8 byRace, unsigned int dwASerial)
{
  for (int index = 0; index < m_nCandidateCnt_1st[byRace]; ++index)
  {
    if (m_kCandidate[byRace][index].bLoad && m_kCandidate[byRace][index].dwAvatorSerial == dwASerial)
    {
      return &m_kCandidate[byRace][index];
    }
  }

  for (int index = 0; index < m_nCandidateCnt_1st[byRace]; ++index)
  {
    m_kSysLog.Write(
      "GetCandidateBySerial() Return NULL : %d : Serial=%d",
      index,
      m_kCandidate[byRace][index].dwAvatorSerial);
  }

  return nullptr;
}

CandidateMgr::_candidate_info *CandidateMgr::GetPatriarchGroupBySerial(
  unsigned __int8 byRace,
  unsigned int dwASerial)
{
  if (byRace >= 3)
  {
    return nullptr;
  }

  for (int index = 0; index < 9; ++index)
  {
    if (m_kPatriarchGroup[byRace][index].bLoad
        && m_kPatriarchGroup[byRace][index].dwAvatorSerial == dwASerial)
    {
      return &m_kPatriarchGroup[byRace][index];
    }
  }

  return nullptr;
}

CandidateMgr::_candidate_info *CandidateMgr::GetPatriarchGroup(
  unsigned __int8 byRace,
  _candidate_info::ClassType eType)
{
  if (byRace >= 3)
  {
    return nullptr;
  }
  if (eType >= _candidate_info::patriarch_group_num)
  {
    return nullptr;
  }

  for (int j = 0; j < 9; ++j)
  {
    if (m_kPatriarchGroup[byRace][j].bLoad && m_kPatriarchGroup[byRace][j].eClassType == eType)
    {
      return &m_kPatriarchGroup[byRace][j];
    }
  }

  return nullptr;
}

const CandidateMgr::_candidate_info *CandidateMgr::GetCandidate_2st(
  unsigned __int8 byRace,
  unsigned int nIdx)
{
  if (byRace >= 3)
  {
    return nullptr;
  }
  if (nIdx < 8)
  {
    return m_pkCandidateLink_2st[static_cast<unsigned __int64>(byRace)][nIdx];
  }
  return nullptr;
}

__int64 CandidateMgr::__SortByPvpPoint()
{
  for (int race = 0; race < 3; ++race)
  {
    for (int lhs = 1; lhs < m_nCandidateCnt_1st[race]; ++lhs)
    {
      for (int rhs = 1; rhs < m_nCandidateCnt_1st[race]; ++rhs)
      {
        if (m_pkCandidateLink_1st[race][lhs]->dPvpPoint
            <= m_pkCandidateLink_1st[race][rhs]->dPvpPoint)
        {
          if (m_pkCandidateLink_1st[race][lhs]->dPvpPoint == m_pkCandidateLink_1st[race][rhs]->dPvpPoint
              && m_pkCandidateLink_1st[race][lhs]->byGrade > m_pkCandidateLink_1st[race][rhs]->byGrade)
          {
            _candidate_info *swap = m_pkCandidateLink_1st[race][lhs];
            m_pkCandidateLink_1st[race][lhs] = m_pkCandidateLink_1st[race][rhs];
            m_pkCandidateLink_1st[race][rhs] = swap;
          }
        }
        else
        {
          _candidate_info *swap = m_pkCandidateLink_1st[race][lhs];
          m_pkCandidateLink_1st[race][lhs] = m_pkCandidateLink_1st[race][rhs];
          m_pkCandidateLink_1st[race][rhs] = swap;
        }
      }
    }
  }

  return 0;
}

__int64 CandidateMgr::__SortByRank()
{
  for (int race = 0; race < 3; ++race)
  {
    for (int lhs = 1; lhs < m_nCandidateCnt_1st[race]; ++lhs)
    {
      for (int rhs = 1; rhs < m_nCandidateCnt_1st[race]; ++rhs)
      {
        if (m_pkCandidateLink_1st[race][lhs]->dwRank >= m_pkCandidateLink_1st[race][rhs]->dwRank)
        {
          if (m_pkCandidateLink_1st[race][lhs]->dwRank == m_pkCandidateLink_1st[race][rhs]->dwRank
              && m_pkCandidateLink_1st[race][lhs]->dPvpPoint > m_pkCandidateLink_1st[race][rhs]->dPvpPoint)
          {
            _candidate_info *swap = m_pkCandidateLink_1st[race][lhs];
            m_pkCandidateLink_1st[race][lhs] = m_pkCandidateLink_1st[race][rhs];
            m_pkCandidateLink_1st[race][rhs] = swap;
          }
        }
        else
        {
          _candidate_info *swap = m_pkCandidateLink_1st[race][lhs];
          m_pkCandidateLink_1st[race][lhs] = m_pkCandidateLink_1st[race][rhs];
          m_pkCandidateLink_1st[race][rhs] = swap;
        }
      }
    }
  }

  return 0;
}

__int64 CandidateMgr::__SortByScore()
{
  for (int race = 0; race < 3; ++race)
  {
    for (int lhs = 0; lhs < m_nCandidateCnt_2st[race]; ++lhs)
    {
      for (int rhs = 0; rhs < m_nCandidateCnt_2st[race]; ++rhs)
      {
        if (m_pkCandidateLink_2st[race][lhs]->dwScore <= m_pkCandidateLink_2st[race][rhs]->dwScore)
        {
          if (m_pkCandidateLink_2st[race][lhs]->dwScore == m_pkCandidateLink_2st[race][rhs]->dwScore
              && m_pkCandidateLink_2st[race][lhs]->dPvpPoint > m_pkCandidateLink_2st[race][rhs]->dPvpPoint)
          {
            _candidate_info *swap = m_pkCandidateLink_2st[race][lhs];
            m_pkCandidateLink_2st[race][lhs] = m_pkCandidateLink_2st[race][rhs];
            m_pkCandidateLink_2st[race][rhs] = swap;
          }
        }
        else
        {
          _candidate_info *swap = m_pkCandidateLink_2st[race][lhs];
          m_pkCandidateLink_2st[race][lhs] = m_pkCandidateLink_2st[race][rhs];
          m_pkCandidateLink_2st[race][rhs] = swap;
        }
      }
    }
  }

  return 0;
}

void CandidateMgr::ChangeState_1to2()
{
  __SortByPvpPoint();
  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < m_nCandidateCnt_1st[race]; ++index)
    {
      if (m_pkCandidateLink_1st[race][index]
          && m_pkCandidateLink_1st[race][index]->eStatus == _candidate_info::candidate_1st)
      {
        if (m_pkCandidateLink_1st[race][index]->bValidChar)
        {
          if (m_nCandidateCnt_2st[race] >= 8)
          {
            m_pkCandidateLink_1st[race][index]->bRefund = true;
          }
          else
          {
            m_pkCandidateLink_1st[race][index]->eStatus = _candidate_info::candidate_2st;
            m_pkCandidateLink_2st[static_cast<unsigned __int64>(race)][m_nCandidateCnt_2st[race]++] =
              m_pkCandidateLink_1st[race][index];
          }
        }
        else
        {
          m_pkCandidateLink_1st[race][index]->bRefund = true;
        }
      }
    }
  }

  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x72u, nullptr, 0);
}

void CandidateMgr::__AddWinner(unsigned __int8 byRace, unsigned __int8 byNum)
{
  int addedCount = 0;

  for (int rankIndex = 0; addedCount < byNum && rankIndex < 5; ++rankIndex)
  {
    const _PVP_RANK_DATA *rankData =
      CPvpUserAndGuildRankingSystem::Instance()->GetCurrentPvpRankData(byRace, static_cast<unsigned __int8>(rankIndex));
    if (rankData == nullptr || rankData->wszName[0] == '\0'
        || IsRegistedAvator_2(byRace, rankData->dwAvatorSerial))
    {
      continue;
    }

    _candidate_info *empty = GetEmpty(byRace);
    empty->bLoad = true;
    empty->bUpdateClassType = true;
    empty->byRace = byRace;
    empty->dwAvatorSerial = rankData->dwAvatorSerial;
    empty->dwGuildSerial = rankData->dwGuildSerial;
    empty->dwRank = rankData->byRank;
    empty->byLevel = rankData->byLv;
    empty->dPvpPoint = rankData->dPvpPoint;
    empty->byGrade = rankData->byGrade;
    strcpy_s(empty->wszName, sizeof(empty->wszName), rankData->wszName);
    strcpy_s(empty->wszGuildName, sizeof(empty->wszGuildName), rankData->wszGuildName);
    empty->eClassType = static_cast<_candidate_info::ClassType>(addedCount + 5 - byNum);
    m_pkLeader[byRace][empty->eClassType] = empty;

    ++m_nCandidateCnt_1st[empty->byRace];
    ++addedCount;

    m_kSysLog.Write(
      "RANK_AUTO ADD : Serial=%d, Name=%s, CurCandidateCnd=%d",
      empty->dwAvatorSerial,
      empty->wszName,
      m_nCandidateCnt_1st[empty->byRace]);

    const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetElectSerial();
    _qry_case_insert_candidate query(empty->byRace, 0xFFFFu, electSerial, empty->dwAvatorSerial);
    const int querySize = static_cast<int>(query.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x75u, reinterpret_cast<char *>(&query), querySize);
  }
}

void CandidateMgr::FinalDecision()
{
  __SortByScore();

  for (int race = 0; race < 3; ++race)
  {
    _candidate_info::ClassType classType = _candidate_info::patriarch;
    m_kVoteResultLog.Write("================================================ [Race:%d]", race);

    for (int candidateIndex = 0; candidateIndex < 8 && classType < 5; ++candidateIndex)
    {
      _candidate_info *candidate = m_pkCandidateLink_2st[race][candidateIndex];
      if (candidate != nullptr && candidate->bValidChar)
      {
        candidate->eClassType = classType;
        candidate->bUpdateClassType = true;
        m_pkLeader[race][classType] = candidate;
        m_kVoteResultLog.Write(
          "RACE(%d) IDX(%d) Serial(%d) Name(%s) Score(%d)",
          race,
          candidateIndex,
          candidate->dwAvatorSerial,
          candidate->wszName,
          candidate->dwScore);
        classType = static_cast<_candidate_info::ClassType>(classType + 1);
      }
    }

    PatriarchElectProcessor *electProcessor = PatriarchElectProcessor::Instance();
    m_kVoteResultLog.Write(
      "RACE(%d) TOTAL_VOTE(%d) NON_VOTE(%d) HIGH_GRADE(%d)",
      race,
      electProcessor->m_dwTotalVoteCnt[race],
      electProcessor->m_dwNonvoteCnt[race],
      electProcessor->m_dwHighGradeNum[race]);
    m_kVoteResultLog.Write("------------------------------------------------ [Race:%d]", race);

    if (classType < 5)
    {
      __AddWinner(static_cast<unsigned __int8>(race), static_cast<unsigned __int8>(5 - classType));
    }
  }

  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x74u, nullptr, 0);
}

void CandidateMgr::ApplyPatriarchGroup()
{
  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < 9; ++index)
    {
      m_kPatriarchGroup[race][index]._Init();
      const _candidate_info *leader = GetLeader(static_cast<unsigned __int8>(race), index);
      if (leader != nullptr)
      {
        memcpy_0(&m_kPatriarchGroup[race][index], leader, sizeof(m_kPatriarchGroup[race][index]));
      }
    }
  }
}

__int64 CandidateMgr::Update_RegistCandidate_2st()
{
  char source[240];
  char buffer[10244];
  memset_0(source, 0, 200);
  memset_0(buffer, 0, 10240);

  PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
  const unsigned int electSerial = processor->GetElectSerial();
  sprintf_s(
    buffer,
    sizeof(buffer),
    "update [dbo].[tbl_patriarch_candidate] set state = %d where eSerial = %d and (",
    2,
    electSerial);

  int count = 0;
  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < m_nCandidateCnt_1st[race]; ++index)
    {
      if (m_kCandidate[race][index].eStatus == _candidate_info::candidate_2st)
      {
        sprintf_s(source, 0xC8u, "ASerial=%d or ", m_kCandidate[race][index].dwAvatorSerial);
        strcat_s(buffer, sizeof(buffer), source);
        ++count;
      }
    }
  }

  if (count <= 0)
  {
    return 0;
  }

  const size_t length = strlen_0(buffer);
  if (length >= 4)
  {
    buffer[length - 4] = ')';
    buffer[length - 3] = '\0';
  }

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

__int64 CandidateMgr::Update_Score()
{
  unsigned int result = 0;

  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < m_nCandidateCnt_1st[race]; ++index)
    {
      _candidate_info *candidate = &m_kCandidate[race][index];
      _candidate_info *candidateOld = &m_kCandidate_old[race][index];
      if (candidate->bLoad && candidate->dwScore != candidateOld->dwScore)
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        const unsigned int electSerial = processor->GetElectSerial();

        char buffer[260]{};
        sprintf_s(
          buffer,
          sizeof(buffer),
          "{ CALL pUpdate_Patriarch_Score ( %d, %d, %d, %d ) }",
          candidate->dwScore,
          electSerial,
          candidate->dwAvatorSerial,
          candidate->byRace);

        if (g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true))
        {
          memcpy_0(candidateOld, candidate, sizeof(_candidate_info));
        }
        else
        {
          result = 24;
        }
      }
    }
  }

  return result;
}

__int64 CandidateMgr::Update_ClassType()
{
  unsigned int result = 0;

  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < 8; ++index)
    {
      const _candidate_info *candidate = GetCandidate_2st(race, index);
      if (candidate && candidate->bLoad && candidate->bUpdateClassType && candidate->eClassType < 5)
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        const unsigned int electSerial = processor->GetElectSerial();

        char buffer[260]{};
        sprintf_s(
          buffer,
          sizeof(buffer),
          "{ CALL pUpdate_Patriarch_ClassType ( %d, %d, %d ) }",
          static_cast<unsigned __int8>(candidate->eClassType),
          electSerial,
          candidate->dwAvatorSerial);
        if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true))
        {
          result = 24;
        }
      }
    }
  }

  return result;
}

__int64 CandidateMgr::Update_Refund()
{
  for (int race = 0; race < 3; ++race)
  {
    for (int index = 0; index < m_nCandidateCnt_1st[race]; ++index)
    {
      _candidate_info *candidate = &m_kCandidate[race][index];
      _candidate_info *candidateOld = &m_kCandidate_old[race][index];
      if (candidate->bLoad && candidate->bRefund != candidateOld->bRefund)
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        const unsigned int electSerial = processor->GetElectSerial();

        char buffer[260]{};
        sprintf_s(
          buffer,
          sizeof(buffer),
          "{ CALL pUpdate_Patriarch_Refund ( %d, %d, %d, %d ) }",
          static_cast<unsigned __int8>(candidate->bRefund),
          electSerial,
          candidate->dwAvatorSerial,
          candidate->byRace);

        if (g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true))
        {
          memcpy_0(candidateOld, candidate, sizeof(_candidate_info));
        }
      }
    }
  }

  return 0;
}

__int64 CandidateMgr::Update_DischargePatriarch(char *p)
{
  if (!GetPatriarchGroupBySerial(static_cast<unsigned __int8>(*p), *reinterpret_cast<unsigned int *>(p + 4)))
  {
    return 0;
  }

  char buffer[272]{};
  sprintf_s(
    buffer,
    sizeof(buffer),
    "{ CALL pUpdate_Patriarch_Status ( %d, %d, %d, %d ) }",
    4,
    *reinterpret_cast<unsigned int *>(p + 8),
    *reinterpret_cast<unsigned int *>(p + 4),
    static_cast<unsigned __int8>(*p));

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

__int64 CandidateMgr::CheckDBValidCharacter(unsigned __int8 byProc)
{
  unsigned int dwDbSerial[4]{};
  dwDbSerial[0] = 0xFFFFFFFFu;

  for (int race = 0; race < 3; ++race)
  {
    int maxCount = 0;
    if (byProc == 1)
    {
      maxCount = m_nCandidateCnt_1st[race];
    }
    else if (byProc > 1 && byProc <= 3)
    {
      maxCount = m_nCandidateCnt_2st[race];
    }

    for (int index = 0; index < maxCount; ++index)
    {
      _candidate_info *candidate = nullptr;
      if (byProc == 1)
      {
        candidate = m_pkCandidateLink_1st[race][index];
      }
      else if (byProc > 1 && byProc <= 3)
      {
        candidate = m_pkCandidateLink_2st[static_cast<unsigned __int64>(race)][index];
      }

      if (candidate && candidate->bLoad)
      {
        const unsigned __int8 result =
          g_Main.m_pWorldDB->Select_IsValidChar(candidate->dwAvatorSerial, dwDbSerial);
        if (result == 1)
        {
          PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
          const unsigned int electSerial = processor->GetElectSerial();
          m_kSysLog.Write(
            "FAILED DB_RET(%s_%d):%d(Race:%d, Process:%d)",
            "CandidateMgr::CheckDBValidCharacter()",
            electSerial,
            candidate->dwAvatorSerial,
            race,
            byProc);
        }
        else if (result == 2)
        {
          candidate->bValidChar = false;
        }
      }
    }
  }

  return 0;
}

__int64 CandidateMgr::Update_VoteTime(unsigned int dwSerial)
{
  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);

  char buffer[280]{};
  sprintf_s(
    buffer,
    sizeof(buffer),
    "update [dbo].[tbl_general] set tmRaceBossVote = '%04d-%02d-%02d %02d:%02d:%02d.%03d' where serial = %d",
    systemTime.wYear,
    systemTime.wMonth,
    systemTime.wDay,
    systemTime.wHour,
    systemTime.wMinute,
    systemTime.wSecond,
    systemTime.wMilliseconds,
    dwSerial);

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

__int64 CandidateMgr::Insert_Candidate(char *p)
{
  _candidate_info *candidate = nullptr;
  char *data = p + 16;

  if (p[14] == 117)
  {
    candidate = GetCandidateBySerial(static_cast<unsigned __int8>(*data), *reinterpret_cast<unsigned int *>(data + 8));
  }
  else if (p[14] == 125)
  {
    candidate = GetPatriarchGroupBySerial(static_cast<unsigned __int8>(*data), *reinterpret_cast<unsigned int *>(data + 8));
  }

  if (!candidate)
  {
    m_kSysLog.Write("CANDIDATE RANK AUTO ERROR : Serial=%d", *reinterpret_cast<unsigned int *>(data + 8));
    return 24;
  }

  const int selectResult =
    g_Main.m_pWorldDB->Select_PatriarchWinCnt(static_cast<unsigned __int8>(*data),
      *reinterpret_cast<unsigned int *>(data + 8),
      &candidate->dwWinCnt);
  if (selectResult == 1)
  {
    return 24;
  }

  char buffer[1040]{};
  sprintf_s(
    buffer,
    sizeof(buffer),
    "{ CALL pInsert_Patriarch_Candidate_070313 ( %d, %d, %d, %d, %d, %f, %d, '%s', %d, '%s', %d, %d ) }",
    *reinterpret_cast<unsigned int *>(data + 4),
    candidate->byRace,
    candidate->byLevel,
    candidate->dwRank,
    candidate->byGrade,
    static_cast<double>(candidate->dPvpPoint),
    candidate->dwAvatorSerial,
    candidate->wszName,
    candidate->dwGuildSerial,
    candidate->wszGuildName,
    static_cast<unsigned __int8>(candidate->eClassType),
    static_cast<unsigned __int8>(candidate->eStatus));

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

void CandidateMgr::CompleteInsertCandidate(unsigned __int8 byRet, char *p)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(p);
  if (byRet)
  {
    if (*reinterpret_cast<unsigned __int16 *>(data + 2) != 0xFFFF)
    {
      CPlayer *player = &g_Player[*reinterpret_cast<unsigned __int16 *>(data + 2)];
      if (player->m_bOper)
      {
        const unsigned int charSerial = player->m_Param.GetCharSerial();
        if (charSerial == *reinterpret_cast<unsigned int *>(data + 8))
        {
          _qry_case_request_refund refund(
            *data,
            *reinterpret_cast<unsigned __int16 *>(data + 2),
            *reinterpret_cast<unsigned int *>(data + 8),
            0x989680uLL);
          const int size = static_cast<int>(refund.size());
          g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x7Cu, reinterpret_cast<char *>(&refund), size);
        }
      }
    }

    m_kSysLog.Write(
      "FAILED DB_RET(%s_%d):%d(Race:%d)",
      "Candidate Insert",
      *reinterpret_cast<unsigned int *>(data + 4),
      *reinterpret_cast<unsigned int *>(data + 8),
      *data);

    _candidate_info *candidate = GetCandidateBySerial(*data, *reinterpret_cast<unsigned int *>(data + 8));
    if (candidate)
    {
      candidate->_Init();
    }
    return;
  }

  _candidate_info *candidate = GetCandidateBySerial(*data, *reinterpret_cast<unsigned int *>(data + 8));
  if (candidate && candidate->dwWinCnt)
  {
    PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
    if (processor->GetProcessorType() == ElectProcessor::_eCandidateRegister)
    {
      _update_candidate_wincount_packing info;
      info.byRace = candidate->byRace;
      info.dwWinCnt = candidate->dwWinCnt;
      strcpy_0(info.wszdName, candidate->wszName);
      processor->Doit(_eReqUpdateWinCount, nullptr, reinterpret_cast<char *>(&info));
    }
  }

  m_kSysLog.Write(
    "CompleteInsertCandidate(%d):Serial:%d(Race:%d)",
    *reinterpret_cast<unsigned int *>(data + 4),
    *reinterpret_cast<unsigned int *>(data + 8),
    *data);
}
