#include "pch.h"

#include "CRaceBuffByHolyQuestProcedure.h"

#include "GlobalObjects.h"

CRaceBuffByHolyQuestProcedure::CRaceBuffByHolyQuestProcedure()
  : m_eState(REQUEST_TYPE::RT_WAIT),
    m_uiProccessIndex(0),
    m_kBuffHolyQestResultInfo(),
    m_kBuffInfo()
{
}

CRaceBuffByHolyQuestProcedure::~CRaceBuffByHolyQuestProcedure() = default;

bool CRaceBuffByHolyQuestProcedure::Init()
{
  return m_kBuffHolyQestResultInfo.Load() && m_kBuffInfo.Init();
}

bool CRaceBuffByHolyQuestProcedure::Request(int iType)
{
  if (m_eState != REQUEST_TYPE::RT_WAIT)
  {
    return false;
  }

  if (iType == 1)
  {
    return RequestSubProcCancelRaceBuff();
  }
  if (iType == 2)
  {
    return RequestSubProcSetRaceBattleResult();
  }
  if (iType == 3)
  {
    return RequestSubProcSetRaceBuff();
  }
  return false;
}

void CRaceBuffByHolyQuestProcedure::Loop()
{
  if (m_eState == REQUEST_TYPE::RT_SET_RACE_BUFF)
  {
    LoopSubProcSetRaceBuff();
  }
}

bool CRaceBuffByHolyQuestProcedure::CreateComplete(CPlayer *pkPlayer)
{
  if (!pkPlayer || !pkPlayer->m_bOper)
  {
    return false;
  }

  const bool haveMentalTicket = pkPlayer->IsHaveMentalTicket();
  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&pkPlayer->m_Param));
  const int resultType = m_kBuffHolyQestResultInfo.GetResultType(race, haveMentalTicket);
  if (resultType < 0)
  {
    return false;
  }
  if (pkPlayer->IsUseReleaseRaceBuffPotion())
  {
    return false;
  }

  const unsigned int continueCnt = m_kBuffHolyQestResultInfo.GetContinueCnt(resultType);
  return m_kBuffInfo.CreateComplete(continueCnt, resultType, pkPlayer);
}

int CRaceBuffByHolyQuestProcedure::CancelPlayerRaceBuff(
  CPlayer *pkPlayer,
  CRaceBuffInfoByHolyQuestfGroup::RESULT_TYPE eReleaseType,
  unsigned int uiReleaseLv)
{
  if (!pkPlayer || !pkPlayer->m_bOper)
  {
    return -1;
  }

  const bool haveMentalTicket = pkPlayer->IsHaveMentalTicket();
  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&pkPlayer->m_Param));
  const int resultType = m_kBuffHolyQestResultInfo.GetResultType(race, haveMentalTicket);
  if (resultType < 0)
  {
    return -2;
  }
  if (static_cast<int>(eReleaseType) != resultType)
  {
    return -3;
  }

  const unsigned int continueCnt = m_kBuffHolyQestResultInfo.GetContinueCnt(resultType);
  if (continueCnt > uiReleaseLv)
  {
    return -4;
  }

  if (m_kBuffInfo.Release(continueCnt, resultType, pkPlayer))
  {
    return 0;
  }
  return -5;
}

int CRaceBuffByHolyQuestProcedure::GetRaceBuffLevel(CPlayer *pOne)
{
  const bool haveMentalTicket = pOne->IsHaveMentalTicket();
  const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&pOne->m_Param));
  const int resultType = m_kBuffHolyQestResultInfo.GetResultType(race, haveMentalTicket);
  if (resultType >= 0)
  {
    return static_cast<int>(m_kBuffHolyQestResultInfo.GetContinueCnt(resultType));
  }
  return -1;
}

bool CRaceBuffByHolyQuestProcedure::RequestSubProcCancelRaceBuff()
{
  for (int j = 0; j < MAX_PLAYER; ++j)
  {
    if (g_Player[j].m_bOper)
    {
      const bool haveMentalTicket = g_Player[j].IsHaveMentalTicket();
      const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&g_Player[j].m_Param));
      const int resultType = m_kBuffHolyQestResultInfo.GetResultType(race, haveMentalTicket);
      if (resultType >= 0)
      {
        const unsigned int continueCnt = m_kBuffHolyQestResultInfo.GetContinueCnt(resultType);
        m_kBuffInfo.Release(continueCnt, resultType, &g_Player[j]);
      }
    }
  }

  m_kBuffHolyQestResultInfo.ClearResult();
  return true;
}

bool CRaceBuffByHolyQuestProcedure::RequestSubProcSetRaceBattleResult()
{
  const int destroyRace =
    (g_HolySys.GetDestroyStoneRace() == -1) ? 0xFF : g_HolySys.GetDestroyStoneRace();
  const int holyMasterRace =
    (g_HolySys.GetHolyMasterRace() == -1) ? 0xFF : g_HolySys.GetHolyMasterRace();
  const unsigned int maxThCnt = m_kBuffInfo.GetMaxThCnt();
  m_kBuffHolyQestResultInfo.SetResult(
    static_cast<unsigned __int8>(holyMasterRace),
    static_cast<unsigned __int8>(destroyRace),
    maxThCnt);
  return true;
}

bool CRaceBuffByHolyQuestProcedure::RequestSubProcSetRaceBuff()
{
  if (!m_kBuffHolyQestResultInfo.IsChaos())
  {
    m_uiProccessIndex = 0;
    m_eState = REQUEST_TYPE::RT_SET_RACE_BUFF;
    m_kBuffHolyQestResultInfo.SetBuffFlag();
  }
  return true;
}

void CRaceBuffByHolyQuestProcedure::LoopSubProcSetRaceBuff()
{
  const unsigned int lastIndex = m_uiProccessIndex + 50;

  unsigned int j = m_uiProccessIndex;
  for (; j < lastIndex && j < MAX_PLAYER; ++j)
  {
    if (g_Player[j].m_bOper)
    {
      const bool haveMentalTicket = g_Player[j].IsHaveMentalTicket();
      const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&g_Player[j].m_Param));
      const int resultType = m_kBuffHolyQestResultInfo.GetResultType(race, haveMentalTicket);
      if (resultType >= 0)
      {
        const unsigned int continueCnt = m_kBuffHolyQestResultInfo.GetContinueCnt(resultType);
        m_kBuffInfo.Apply(continueCnt, resultType, &g_Player[j]);
      }
    }
  }

  m_uiProccessIndex = j;
  if (m_uiProccessIndex >= MAX_PLAYER)
  {
    m_uiProccessIndex = 0;
    m_eState = REQUEST_TYPE::RT_WAIT;
  }
}
