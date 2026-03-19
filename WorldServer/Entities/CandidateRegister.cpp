#include "pch.h"

#include "CandidateRegister.h"

#include <algorithm>
#include <cstdio>
#include <cstring>

#include "CandidateMgr.h"
#include "CMgrAvatorItemHistory.h"
#include "CMoneySupplyMgr.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "KorLocalTime.h"
#include "PatriarchElectProcessor.h"
#include "update_candidate_wincount_packing.h"

static_assert(sizeof(_pt_result_fcandidacy_list_zocl::__candi_info) == 47, "Candidacy packet entry size mismatch");
static_assert(sizeof(_pt_result_fcandidacy_list_zocl) == 23501, "Candidate list packet size mismatch");

_pt_result_fcandidacy_list_zocl::_pt_result_fcandidacy_list_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

int _pt_result_fcandidacy_list_zocl::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned __int16>(1 + static_cast<unsigned int>(byCnt) * sizeof(__candi_info)));
}

CandidateRegister::CandidateRegister()
  : ElectProcessor(_eCandidateRegister)
{
  memset(m_kSend, 0, sizeof(m_kSend));
  m_byPtType[0] = 56;
  m_byPtType[1] = 2;
}

CandidateRegister::~CandidateRegister() = default;

bool CandidateRegister::Initialize()
{
  char logPath[256]{};
  const unsigned int localTime = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(
    logPath,
    sizeof(logPath),
    "..\\ZoneServerLog\\SystemLog\\Patriarch\\CandidateRegister_%d.log",
    localTime);
  _kSysLog.SetWriteLogFile(logPath, true, false, true, true);

  ElectProcessor::Initialize();
  m_bInitCandidate = false;
  std::memset(m_kSend, 0, sizeof(m_kSend));
  return true;
}

int CandidateRegister::Doit(Cmd eCmd, CPlayer *pOne, char *pdata)
{
  if (eCmd == _eRequestInitCandidate)
  {
    _InitCandidate();
    return 0;
  }

  switch (eCmd)
  {
    case _eReqUpdateWinCount:
    {
      _update_candidate_wincount_packing *updateInfo =
        reinterpret_cast<_update_candidate_wincount_packing *>(pdata);
      _UpdatePacketWin(updateInfo->byRace, updateInfo->wszdName, updateInfo->dwWinCnt);
      return 0;
    }

    case _eRequestCandidateList:
      if (m_bInitCandidate)
      {
        const unsigned __int8 raceCode = static_cast<unsigned char>(pOne->m_Param.GetRaceCode());
        return _SendList(pOne->m_ObjID.m_wIndex, raceCode);
      }
      return 2;

    case _eRegCandidate:
      if (m_bInitCandidate)
      {
        return _Regist(pOne, pdata);
      }
      return 2;

    default:
      return 255;
  }
}

int CandidateRegister::_CheckPlayerInfo(CPlayer *pOne)
{
  if (pOne->m_Param.m_byPvPGrade < 3)
  {
    return 4;
  }

  const unsigned int avatorSerial = pOne->m_Param.GetCharSerial();
  const int raceCode = static_cast<int>(pOne->m_Param.GetRaceCode());
  if (CandidateMgr::Instance()->IsRegistedAvator_1(static_cast<unsigned __int8>(raceCode), avatorSerial))
  {
    return 3;
  }

  if (pOne->m_Param.GetDalant() >= 10000000)
  {
    return 0;
  }

  return 5;
}

bool CandidateRegister::_AddToPacket(CPlayer *pOne, unsigned int dwWinCnt)
{
  const unsigned __int8 raceCode = static_cast<unsigned char>(pOne->m_Param.GetRaceCode());
  const int currentCount = m_kSend[raceCode].byCnt;
  if (currentCount >= CandidateMgr::Instance()->GetMaxNum())
  {
    return false;
  }

  _pt_result_fcandidacy_list_zocl::__candi_info &candidate = m_kSend[raceCode].Candidacy[m_kSend[raceCode].byCnt];
  candidate.byGrade = pOne->m_Param.m_byPvPGrade;
  candidate.dPvpPoint = pOne->m_Param.GetPvPPoint();
  candidate.dwWinCnt = dwWinCnt;
  strcpy_s(candidate.wszAvatorName, sizeof(candidate.wszAvatorName), pOne->m_Param.GetCharNameW());
  if (pOne->m_Param.m_pGuild != nullptr)
  {
    strcpy_s(candidate.wszGuildName, sizeof(candidate.wszGuildName), pOne->m_Param.m_pGuild->m_wszName);
  }

  ++m_kSend[raceCode].byCnt;
  _SortCandidacyByPvpPoint(raceCode);
  return true;
}

void CandidateRegister::_SortCandidacyByPvpPoint(unsigned __int8 byRace)
{
  if (byRace >= 3u)
  {
    return;
  }

  for (int left = 0; left < m_kSend[byRace].byCnt; ++left)
  {
    for (int right = 0; right < m_kSend[byRace].byCnt; ++right)
    {
      if (m_kSend[byRace].Candidacy[left].dPvpPoint > m_kSend[byRace].Candidacy[right].dPvpPoint)
      {
        _pt_result_fcandidacy_list_zocl::__candi_info temp{};
        std::memcpy(&temp, &m_kSend[byRace].Candidacy[left], sizeof(temp));
        std::memcpy(
          &m_kSend[byRace].Candidacy[left],
          &m_kSend[byRace].Candidacy[right],
          sizeof(m_kSend[byRace].Candidacy[left]));
        std::memcpy(&m_kSend[byRace].Candidacy[right], &temp, sizeof(m_kSend[byRace].Candidacy[right]));
      }
    }
  }
}

int CandidateRegister::_SendList(unsigned __int16 wSock, unsigned __int8 byRace)
{
  const unsigned __int16 length = m_kSend[byRace].size();
  g_Network.m_pProcess[0]->LoadSendMsg(wSock, m_byPtType, reinterpret_cast<char *>(&m_kSend[byRace]), length);
  return 0;
}

int CandidateRegister::_Regist(CPlayer *pOne, char *pdata)
{
if (!_bEnable)
  {
    return 2;
  }

  const unsigned int result = _CheckPlayerInfo(pOne);
  if (result != 0)
  {
    return static_cast<int>(result);
  }

  const unsigned __int8 raceCode = static_cast<unsigned char>(pOne->m_Param.GetRaceCode());
  if (!CandidateMgr::Instance()->Regist(pOne))
  {
    return 18;
  }

  const unsigned int avatorSerial = pOne->m_Param.GetCharSerial();
  const unsigned int winCount = CandidateMgr::Instance()->GetWinCnt(raceCode, avatorSerial);
  if (!_AddToPacket(pOne, winCount))
  {
    return 18;
  }

  PatriarchElectProcessor::Instance()->SendMsg_ResultCode(pOne->m_id.wIndex, 1u);
  pOne->SubDalant(10000000u);
  pOne->SendMsg_AlterMoneyInform(0);

  const unsigned __int16 packetLength = m_kSend[raceCode].size();
  g_Network.m_pProcess[0]->LoadSendMsg(
    pOne->m_ObjID.m_wIndex,
    m_byPtType,
    reinterpret_cast<char *>(&m_kSend[raceCode]),
    packetLength);

  CPlayer::s_MgrItemHistory.raceboss_candidate(-10000000, pOne->m_Param.GetCharSerial(), pOne->m_szItemHistoryFileName);

  const int level = pOne->m_Param.GetLevel();
  if (level == 30 || level == 40 || level == 50 || level == 60)
  {
    CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(
      static_cast<unsigned __int8>(pOne->m_Param.GetRaceCode()),
      level,
      10000000u);
  }

  return 0;
}

void CandidateRegister::_InitCandidate()
{
  for (int race = 0; race < 3; ++race)
  {
    if (CandidateMgr::Instance()->GetCandidateCnt_1st(static_cast<unsigned __int8>(race)) == 0)
    {
      unsigned int avatorSerial =
        CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(race), 0);
      unsigned int rank = CPvpUserAndGuildRankingSystem::Instance()->FindRank(static_cast<unsigned __int8>(race), avatorSerial);
      if (avatorSerial == 0 || rank == static_cast<unsigned int>(-1))
      {
        rank = 1;
      }

      const _PVP_RANK_DATA *const rankData =
        CPvpUserAndGuildRankingSystem::Instance()->GetCurrentPvpRankData(static_cast<unsigned __int8>(race), rank - 1);
      if (rankData != nullptr && rankData->dwAvatorSerial != 0)
      {
        CandidateMgr::Instance()->Regist(static_cast<unsigned __int8>(race), rankData);
      }
    }

    m_kSend[race].byCnt = 0;
    for (unsigned int index = 0; index < 500; ++index)
    {
      const CandidateMgr::_candidate_info *const candidate =
        CandidateMgr::Instance()->GetCandidate(static_cast<unsigned __int8>(race), index);
      if (candidate != nullptr
          && candidate->bLoad
          && candidate->eStatus == CandidateMgr::_candidate_info::candidate_1st)
      {
        _pt_result_fcandidacy_list_zocl::__candi_info &packetCandidate =
          m_kSend[race].Candidacy[m_kSend[race].byCnt];
        packetCandidate.byGrade = candidate->byGrade;
        packetCandidate.dPvpPoint = candidate->dPvpPoint;
        packetCandidate.dwWinCnt = candidate->dwWinCnt;
        strcpy_s(packetCandidate.wszAvatorName, sizeof(packetCandidate.wszAvatorName), candidate->wszName);
        if (candidate->dwGuildSerial != static_cast<unsigned int>(-1))
        {
          strcpy_s(packetCandidate.wszGuildName, sizeof(packetCandidate.wszGuildName), candidate->wszGuildName);
        }
        ++m_kSend[race].byCnt;
      }
    }

    _SortCandidacyByPvpPoint(static_cast<unsigned __int8>(race));
  }

  m_bInitCandidate = true;
}

void CandidateRegister::_UpdatePacketWin(unsigned __int8 byRace, char *wszName, unsigned int dwWinCnt)
{
  if (byRace >= 3u)
  {
    return;
  }

  for (int index = 0; index < m_kSend[byRace].byCnt; ++index)
  {
    if (!std::strcmp(m_kSend[byRace].Candidacy[index].wszAvatorName, wszName))
    {
      m_kSend[byRace].Candidacy[index].dwWinCnt = dwWinCnt;
      return;
    }
  }
}
