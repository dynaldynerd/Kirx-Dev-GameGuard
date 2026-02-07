#include "pch.h"

#include "CandidateMgr.h"

#include <new>

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
