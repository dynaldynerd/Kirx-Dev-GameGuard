#include "pch.h"

#include "CWeeklyGuildRankInfo.h"
#include "CMainThread.h"

#include <cstring>

int SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE = 3;

CWeeklyGuildRankOwnerInfo::CWeeklyGuildRankOwnerInfo()
{
  Clear();
}

void CWeeklyGuildRankOwnerInfo::Clear()
{
  m_dwSerial = 0;
  std::memset(m_wszGuildName, 0, sizeof(m_wszGuildName));
  m_byRace = 0;
  m_wRank = 0;
  m_byGrade = 0;
  m_dKillPvpPoint = 0.0;
  m_dGuildBattlePvpPoint = 0.0;
  m_dwSumLv = 0;
}

CWeeklyGuildRankRecord::CWeeklyGuildRankRecord()
{
  Clear();
}

void CWeeklyGuildRankRecord::Clear()
{
  m_dwInx = 0;
  m_wRank = 0;
  m_dwSerial = 0;
  std::memset(m_wszGuildName, 0, sizeof(m_wszGuildName));
  m_byGrade = 0;
  m_dKillPvpPoint = 0.0;
  m_dGuildBattlePvpPoint = 0.0;
}

bool CWeeklyGuildRankInfo::Init()
{
  m_ppkInfo = new CWeeklyGuildRankRecord *[500];
  if (m_ppkInfo == nullptr)
  {
    return false;
  }

  for (int j = 0; j < 500; ++j)
  {
    m_ppkInfo[j] = new CWeeklyGuildRankRecord();
    if (m_ppkInfo[j] == nullptr)
    {
      return false;
    }
  }

  m_pkSendList = new _weekly_guild_rank_result_zocl[3];
  if (m_pkSendList == nullptr)
  {
    return false;
  }
  for (int k = 0; k < 3; ++k)
  {
    m_pkSendList[k].dwVer = 0;
  }

  m_bInit = true;
  Clear();

  if (g_Main.IsTestServer())
  {
    SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE = 1;
  }
  return true;
}

void CWeeklyGuildRankInfo::Clear()
{
  m_NoDataPrev = true;
  m_bNoDataToday = true;
  ClearRank();
  ClearOwner();
}

void CWeeklyGuildRankInfo::ClearRank()
{
  m_dwRecordCnt[0] = 0;
  m_dwRecordCnt[1] = 0;
  m_dwRecordCnt[2] = 0;
  m_ppkRaceStartPos[0] = nullptr;
  m_ppkRaceStartPos[1] = nullptr;
  m_ppkRaceStartPos[2] = nullptr;
  m_dwMaxCnt = 0;

  if (m_ppkInfo != nullptr)
  {
    for (int j = 0; j < 500; ++j)
    {
      m_ppkInfo[j]->Clear();
    }
  }

  if (m_pkSendList != nullptr)
  {
    for (int k = 0; k < 3; ++k)
    {
      m_pkSendList[k].byExistSelfRankInfo = 0;
      m_pkSendList[k].byCnt = 0;
      m_pkSendList[k].byRace = static_cast<unsigned __int8>(-1);
      std::memset(m_pkSendList[k].list, 0, sizeof(m_pkSendList[k].list));
    }
  }
}

void CWeeklyGuildRankInfo::ClearOwner()
{
  for (unsigned __int8 j = 0; j < 3; ++j)
  {
    for (int k = 0; k < 2; ++k)
    {
      m_kOwnerInfo[j][k].Clear();
    }
  }
}
