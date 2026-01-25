#include "pch.h"

#include "CTotalGuildRankInfo.h"

#include <cstring>

void CTotalGuildRankRecord::Clear()
{
  m_dwInx = 0;
  m_wRank = 0;
  m_dwSerial = 0;
  std::memset(m_wszGuildName, 0, sizeof(m_wszGuildName));
  m_byGrade = 0;
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
}

void CTotalGuildRankInfo::Clear()
{
  m_bNoData = true;
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
      if (m_ppkInfo[j] != nullptr)
      {
        m_ppkInfo[j]->Clear();
      }
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

bool CTotalGuildRankInfo::Init()
{
  m_ppkInfo = reinterpret_cast<CTotalGuildRankRecord **>(operator new[](0xFA0));
  if (m_ppkInfo == nullptr)
  {
    return false;
  }
  for (int j = 0; j < 500; ++j)
  {
    CTotalGuildRankRecord *record = new CTotalGuildRankRecord();
    m_ppkInfo[j] = record;
    if (record == nullptr)
    {
      return false;
    }
  }
  m_pkSendList = reinterpret_cast<_total_guild_rank_result_zocl *>(operator new[](0x4B9));
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
  return true;
}
