#include "pch.h"

#include "Voter.h"

#include "CandidateMgr.h"
#include "CNetProcess.h"
#include "PatriarchElectProcessor.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "qry_case_candidate_scalar_payloads.h"

#include <cstring>
#include <cstdio>

Voter::Voter()
  : ElectProcessor(_eVoter)
{
}

bool Voter::Initialize()
{
  char logPath[256]{};
  std::snprintf(logPath, sizeof(logPath), "..\\ZoneServerLog\\SystemLog\\Patriarch\\Voter_%llu.log", GetKorLocalTime());
  _kSysLog.SetWriteLogFile(logPath, true, false, true, true);

  PatriarchElectProcessor::Instance()->PushDQSCheckInvalidChar();
  ElectProcessor::Initialize();
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x78u, nullptr, false);
  return true;
}

int Voter::Doit(Cmd cmd, CPlayer *player, char *payload)
{
  switch (cmd)
  {
  case _eMakeVotePaper:
    _MakeVotePaper();
    _SendVotePaperAll();
    return 0;
  case _eSendVotePaper:
    return _SendVotePaper(player);
  case _eVote:
    return _Vote(player, payload);
  case _eVoteScore:
    _SendVoteScore(player);
    return 0;
  default:
    return 255;
  }
}

void Voter::_MakeVotePaper()
{
  memset_0(_kCandidateInfo, 0, sizeof(_kCandidateInfo));
  memset_0(_kVoteScoreInfo, 0, sizeof(_kVoteScoreInfo));

  for (int race = 0; race < 3; ++race)
  {
    const int totalVoteCount = PatriarchElectProcessor::Instance()->m_dwTotalVoteCnt[race];
    const int nonVoteCount = PatriarchElectProcessor::Instance()->m_dwNonvoteCnt[race];
    const int totalCount = totalVoteCount + nonVoteCount;

    _kVoteScoreInfo[race].byRace = static_cast<unsigned __int8>(race);
    if (nonVoteCount != 0)
    {
      _kVoteScoreInfo[race].byNonvoteRate =
        static_cast<unsigned __int8>(static_cast<int>((static_cast<float>(nonVoteCount) / static_cast<float>(totalCount)) * 100.0f));
    }
    else
    {
      _kVoteScoreInfo[race].byNonvoteRate = 0;
    }

    if (totalVoteCount != 0)
    {
      _kVoteScoreInfo[race].byVoteRate =
        static_cast<unsigned __int8>(static_cast<int>((static_cast<float>(totalVoteCount) / static_cast<float>(totalCount)) * 100.0f));
    }
    else
    {
      _kVoteScoreInfo[race].byVoteRate = 0;
    }

    int packetIndex = 0;
    for (int index = 0; index < CandidateMgr::Instance()->GetCandidateCnt_2st(static_cast<unsigned __int8>(race)); ++index)
    {
      const CandidateMgr::_candidate_info *candidate =
        CandidateMgr::Instance()->GetCandidate_2st(static_cast<unsigned __int8>(race), index);
      if (candidate == nullptr)
      {
        continue;
      }

      if (candidate->eStatus != CandidateMgr::_candidate_info::candidate_2st)
      {
        _kSysLog.Write("Invalid value >> CandidateMgr::Instance()->GetCandidate_2st(%d, %d)", race, index);
        continue;
      }

      if (!candidate->bValidChar)
      {
        continue;
      }

      _kCandidateInfo[race].body[packetIndex].byRank = static_cast<unsigned __int8>(candidate->dwRank);
      _kCandidateInfo[race].body[packetIndex].dwWinCnt = candidate->dwWinCnt;
      strcpy_s(
        _kCandidateInfo[race].body[packetIndex].wszAvatorName,
        sizeof(_kCandidateInfo[race].body[packetIndex].wszAvatorName),
        candidate->wszName);
      strcpy_s(
        _kCandidateInfo[race].body[packetIndex].wszGuildName,
        sizeof(_kCandidateInfo[race].body[packetIndex].wszGuildName),
        candidate->wszGuildName);
      ++_kCandidateInfo[race].byCnt;

      _kVoteScoreInfo[race].body[packetIndex].byRank = static_cast<unsigned __int8>(candidate->dwRank);
      if (candidate->dwScore != 0)
      {
        _kVoteScoreInfo[race].body[packetIndex].byScoreRate =
          static_cast<unsigned __int8>(
            static_cast<int>((static_cast<float>(static_cast<int>(candidate->dwScore)) / static_cast<float>(totalCount)) * 100.0f));
      }
      else
      {
        _kVoteScoreInfo[race].body[packetIndex].byScoreRate = 0;
      }
      strcpy_s(
        _kVoteScoreInfo[race].body[packetIndex].wszAvatorName,
        sizeof(_kVoteScoreInfo[race].body[packetIndex].wszAvatorName),
        candidate->wszName);
      ++_kVoteScoreInfo[race].byCnt;
      ++packetIndex;
    }
  }
}

int Voter::_SendVotePaper(CPlayer *player)
{
  if (player == nullptr || !player->m_bOper)
  {
    return 0;
  }

  const int raceCode = player->m_Param.GetRaceCode();
  if (_kCandidateInfo[raceCode].byCnt < 2u)
  {
    PatriarchElectProcessor::Instance()->SendMsg_ResultCode(player->m_id.wIndex, 8u);
    return 9;
  }

  if (player->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote)
  {
    return 10;
  }
  if (!(player->m_bOper && player->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime >= g_Main.m_dwCheatSetPlayTime))
  {
    return 11;
  }
  if (!(player->m_bOper && player->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable))
  {
    return 11;
  }
  if (!(player->m_bOper && player->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt >= g_Main.m_dwCheatSetScanerCnt))
  {
    return 11;
  }
  if (!(player->m_bOper && player->m_pUserDB->m_AvatorData.dbAvator.m_byLevel >= g_Main.m_dwCheatSetLevel))
  {
    return 11;
  }
  if (!(player->m_bOper && player->m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade >= 2u))
  {
    return 11;
  }

  const unsigned int avatorSerial = player->m_Param.GetCharSerial();
  if (CandidateMgr::Instance()->IsRegistedAvator_2(static_cast<unsigned __int8>(raceCode), avatorSerial))
  {
    return 11;
  }

  unsigned __int8 packetType[2]{56, 5};
  const unsigned __int16 packetLength = static_cast<unsigned __int16>(_kCandidateInfo[raceCode].size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    packetType,
    reinterpret_cast<char *>(&_kCandidateInfo[raceCode]),
    packetLength);
  return 0;
}

int Voter::_Vote(CPlayer *player, char *payload)
{
  const bool isValidPlayerAndUser = (player != nullptr && player->m_pUserDB != nullptr);
  bool abstention = false;
  const unsigned __int8 raceCode = player->m_Param.GetRaceCode();

  if (_kCandidateInfo[raceCode].byCnt < 2u)
  {
    return 8;
  }

  if (isValidPlayerAndUser)
  {
    if (player->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote)
    {
      return 10;
    }

    const unsigned __int8 voteScore = (player->m_Param.m_byPvPGrade < 4u) ? 1u : 2u;
    char *candidateName = payload + 1;
    if (!strcmp_0(player->m_Param.GetCharNameW(), candidateName))
    {
      PatriarchElectProcessor::Instance()->m_dwNonvoteCnt[raceCode] += voteScore;
      abstention = true;
    }
    else
    {
      if (!IsRegistedVotePaper(raceCode, candidateName))
      {
        return 19;
      }

      PatriarchElectProcessor::Instance()->m_dwTotalVoteCnt[raceCode] += voteScore;
      if (player->m_Param.m_byPvPGrade >= 4u)
      {
        ++PatriarchElectProcessor::Instance()->m_dwHighGradeNum[raceCode];
      }
      CandidateMgr::Instance()->AddScore(raceCode, candidateName, voteScore);
    }

    player->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote = 1;
    player->PushDQSUpdateVoteAvilable();

    _qry_case_update_vote_time voteTimeQuery{};
    voteTimeQuery.dwSerial = player->m_Param.GetCharSerial();
    g_Main.PushDQSData(
      0xFFFFFFFF,
      nullptr,
      0x8Bu,
      reinterpret_cast<char *>(&voteTimeQuery),
      static_cast<int>(voteTimeQuery.size()));

    if (!PatriarchElectProcessor::Instance()->GetTimeCheck())
    {
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x73u, nullptr, 0);
      g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x78u, nullptr, 0);
    }

    _SetVoteScoreInfo(raceCode, payload + 1, abstention);
    _SendVoteScoreAll(raceCode);
    return 0;
  }

  const char *playerName = (player != nullptr) ? player->m_Param.GetCharNameW() : "NULL";
  const int rank = static_cast<unsigned __int8>(payload[0]);
  _kSysLog.Write(
    "_Vote() Invalid player pointer : Candidate(%s) Rank(%d) : %s",
    payload + 1,
    rank,
    playerName);
  return 0;
}

void Voter::_SendVoteScore(CPlayer *player)
{
  if (_kCandidateInfo[player->m_Param.GetRaceCode()].byCnt >= 2u)
  {
    unsigned __int8 packetType[2]{56, 6};
    _pt_notify_vote_score_zocl *scoreInfo = &_kVoteScoreInfo[player->m_Param.GetRaceCode()];
    const unsigned __int16 packetLength = static_cast<unsigned __int16>(scoreInfo->size());
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      packetType,
      reinterpret_cast<char *>(scoreInfo),
      packetLength);
  }
}

void Voter::_SetVoteScoreInfo(unsigned __int8 raceCode, char *candidateName, bool abstention)
{
const int totalVoteCount = PatriarchElectProcessor::Instance()->m_dwTotalVoteCnt[raceCode];
  const int nonVoteCount = PatriarchElectProcessor::Instance()->m_dwNonvoteCnt[raceCode];
  const int totalCount = totalVoteCount + nonVoteCount;

  if (nonVoteCount != 0)
  {
    _kVoteScoreInfo[raceCode].byNonvoteRate =
      static_cast<unsigned __int8>(static_cast<int>((static_cast<float>(nonVoteCount) / static_cast<float>(totalCount)) * 100.0f));
  }
  else
  {
    _kVoteScoreInfo[raceCode].byNonvoteRate = 0;
  }

  if (totalVoteCount != 0)
  {
    _kVoteScoreInfo[raceCode].byVoteRate =
      static_cast<unsigned __int8>(static_cast<int>((static_cast<float>(totalVoteCount) / static_cast<float>(totalCount)) * 100.0f));
  }
  else
  {
    _kVoteScoreInfo[raceCode].byVoteRate = 0;
  }

  if (!abstention)
  {
    for (int index = 0; index < CandidateMgr::Instance()->GetCandidateCnt_2st(raceCode); ++index)
    {
      const CandidateMgr::_candidate_info *candidate = CandidateMgr::Instance()->GetCandidate_2st(raceCode, index);
      if (candidate != nullptr
          && candidate->eStatus == CandidateMgr::_candidate_info::candidate_2st
          && candidate->bValidChar)
      {
        if (candidate->dwScore != 0)
        {
          _kVoteScoreInfo[raceCode].body[index].byScoreRate =
            static_cast<unsigned __int8>(
              static_cast<int>((static_cast<float>(static_cast<int>(candidate->dwScore)) / static_cast<float>(totalVoteCount)) * 100.0f));
        }
        else
        {
          _kVoteScoreInfo[raceCode].body[index].byScoreRate = 0;
        }
      }
    }
  }
}

void Voter::_SendVotePaperAll()
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *online = &g_Player[index];
    if (!online->m_bOper)
    {
      continue;
    }

    const int raceCode = online->m_Param.GetRaceCode();
    if (_kCandidateInfo[raceCode].byCnt >= 2u)
    {
      if (!online->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote
          && online->m_bOper
          && online->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime >= g_Main.m_dwCheatSetPlayTime
          && online->m_bOper
          && online->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable
          && online->m_bOper
          && online->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt >= g_Main.m_dwCheatSetScanerCnt
          && online->m_bOper
          && online->m_pUserDB->m_AvatorData.dbAvator.m_byLevel >= g_Main.m_dwCheatSetLevel
          && online->m_bOper
          && online->m_pUserDB->m_AvatorData.dbAvator.m_byLastClassGrade >= 2u)
      {
        const unsigned int avatorSerial = online->m_Param.GetCharSerial();
        if (!CandidateMgr::Instance()->IsRegistedAvator_2(static_cast<unsigned __int8>(raceCode), avatorSerial))
        {
          unsigned __int8 packetType[2]{56, 5};
          const unsigned __int16 packetLength =
            static_cast<unsigned __int16>(_kCandidateInfo[raceCode].size());
          g_Network.m_pProcess[0]->LoadSendMsg(
            online->m_ObjID.m_wIndex,
            packetType,
            reinterpret_cast<char *>(&_kCandidateInfo[raceCode]),
            packetLength);
        }
      }
    }
    else
    {
      PatriarchElectProcessor::Instance()->SendMsg_ResultCode(online->m_id.wIndex, 8u);
    }
  }
}

void Voter::_SendVoteScoreAll(unsigned __int8 raceCode)
{
  _pt_notify_vote_score_zocl *scoreInfo = &_kVoteScoreInfo[raceCode];
  unsigned __int8 packetType[2]{56, 6};

  for (int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *online = &g_Player[clientIndex];
    if (online->m_bOper && static_cast<unsigned __int8>(online->m_Param.GetRaceCode()) == raceCode)
    {
      const unsigned __int16 packetLength = static_cast<unsigned __int16>(scoreInfo->size());
      g_Network.m_pProcess[0]->LoadSendMsg(
        static_cast<unsigned int>(clientIndex),
        packetType,
        reinterpret_cast<char *>(scoreInfo),
        packetLength);
    }
  }
}

bool Voter::IsRegistedVotePaper(unsigned __int8 raceCode, char *candidateName)
{
  for (int index = 0; index < _kCandidateInfo[raceCode].byCnt; ++index)
  {
    if (!strcmp_0(_kCandidateInfo[raceCode].body[index].wszAvatorName, candidateName))
    {
      return true;
    }
  }
  return false;
}

