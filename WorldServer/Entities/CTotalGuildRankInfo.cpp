#include "pch.h"

#include "CTotalGuildRankInfo.h"

#include <cstring>

#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "Packet/ZoneClientPacket.h"
#include "WorldServerUtil.h"

__int64 _total_guild_rank_result_zocl::size()
{
  if (byCnt < 0xBu)
  {
    return 403;
  }
  return 0;
}

CTotalGuildRankRecord::CTotalGuildRankRecord()
{
  Clear();
}

CTotalGuildRankRecord::~CTotalGuildRankRecord()
{
  Clear();
}

CTotalGuildRankInfo::CTotalGuildRankInfo()
  : m_bInit(false),
    m_bNoData(true),
    m_ppkInfo(nullptr),
    m_pkSendList(nullptr)
{
  Clear();
}

CTotalGuildRankInfo::~CTotalGuildRankInfo()
{
  if (m_ppkInfo)
  {
    for (int j = 0; j < 500; ++j)
    {
      delete m_ppkInfo[j];
      m_ppkInfo[j] = nullptr;
    }
    operator delete[](m_ppkInfo);
    m_ppkInfo = nullptr;
  }

  if (m_pkSendList)
  {
    operator delete[](m_pkSendList);
    m_pkSendList = nullptr;
  }
}

__int64 CTotalGuildRankInfo::Find(unsigned __int8 byRace, unsigned int dwGuildSerial)
{
  if (byRace >= 3 || !m_dwRecordCnt[byRace] || !m_ppkRaceStartPos[byRace])
  {
    return -1;
  }

  CTotalGuildRankRecord **records = m_ppkRaceStartPos[byRace];
  for (unsigned int j = 0; j < m_dwRecordCnt[byRace]; ++j)
  {
    if (records[j]->m_dwSerial == dwGuildSerial)
    {
      return j;
    }
  }

  return -2;
}

char CTotalGuildRankInfo::Load(_total_guild_rank_info *pkInfo)
{
  if (!m_bInit || !pkInfo)
  {
    return 0;
  }
  if (!pkInfo->wCount)
  {
    return 1;
  }
  if (pkInfo->wRaceCnt[3])
  {
    return 0;
  }

  unsigned __int16 offset = 0;
  for (int j = 0; j < 3; ++j)
  {
    m_dwRecordCnt[j] = pkInfo->wRaceCnt[j];
    m_ppkRaceStartPos[j] = &m_ppkInfo[offset];
    offset += static_cast<unsigned __int16>(m_dwRecordCnt[j]);
  }

  for (int k = 0; k < pkInfo->wCount; ++k)
  {
    m_ppkInfo[k]->m_dwInx = k;
    m_ppkInfo[k]->m_wRank = pkInfo->list[k].wRank;
    m_ppkInfo[k]->m_dwSerial = pkInfo->list[k].dwSerial;
    strcpy_0(m_ppkInfo[k]->m_wszGuildName, pkInfo->list[k].wszGuildName);
    m_ppkInfo[k]->m_byGrade = pkInfo->list[k].byGrade;
    strcpy_0(m_ppkInfo[k]->m_wszMasterName, pkInfo->list[k].wszMasterName);
  }

  for (int m = 0; m < 3; ++m)
  {
    ++m_pkSendList[m].dwVer;
    m_pkSendList[m].byRace = static_cast<unsigned __int8>(m);
    m_pkSendList[m].byCnt = m_dwRecordCnt[m] <= 10 ? m_dwRecordCnt[m] : 10;
    for (int n = 0; n < m_pkSendList[m].byCnt; ++n)
    {
      m_pkSendList[m].list[n].byRank = m_ppkRaceStartPos[m][n]->m_wRank;
      strcpy_0(m_pkSendList[m].list[n].wszGuildName, m_ppkRaceStartPos[m][n]->m_wszGuildName);
      m_pkSendList[m].list[n].byGrade = m_ppkRaceStartPos[m][n]->m_byGrade;
      strcpy_0(m_pkSendList[m].list[n].wszMasterName, m_ppkRaceStartPos[m][n]->m_wszMasterName);
    }
  }

  m_bNoData = false;
  return 1;
}

void CTotalGuildRankInfo::SetNoDataFlag()
{
  m_bNoData = true;
}

void CTotalGuildRankInfo::Send(
  unsigned int dwVer,
  unsigned int n,
  unsigned __int8 byTabRace,
  unsigned __int8 bySelfRace,
  unsigned int dwGuildSerial)
{
  if (m_bInit && byTabRace < 3u && bySelfRace < 3u)
  {
    if (m_bNoData)
    {
      __trace(
        "CTotalGuildRankInfo::Send( %u, %d, %u, %u, %u ) : No Data!",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial);
    }
    else if (dwVer == m_pkSendList[byTabRace].dwVer)
    {
      __trace(
        "CTotalGuildRankInfo::Send( %u, %d, %u, %u, %u ) : %u == m_pkSendList[%u].dwVer(%u)",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial,
        dwVer,
        byTabRace,
        m_pkSendList[byTabRace].dwVer);
    }
    else
    {
      const int found = static_cast<int>(Find(bySelfRace, dwGuildSerial));
      __trace(
        "CTotalGuildRankInfo::Send( %u, %d, %u, %u, %u ) : Find( byRace, dwGuildSerial ) -> %d!",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial,
        found);

      if (found < 0)
      {
        m_pkSendList[byTabRace].list[10].byRank = 0;
        memset_0(
          m_pkSendList[byTabRace].list[10].wszGuildName,
          0,
          sizeof(m_pkSendList[byTabRace].list[10].wszGuildName));
        m_pkSendList[byTabRace].list[10].byGrade = 0;
        memset_0(
          m_pkSendList[byTabRace].list[10].wszMasterName,
          0,
          sizeof(m_pkSendList[byTabRace].list[10].wszMasterName));
        m_pkSendList[byTabRace].byExistSelfRankInfo = 0;
      }
      else
      {
        m_pkSendList[byTabRace].list[10].byRank = m_ppkRaceStartPos[bySelfRace][found]->m_wRank;
        strcpy_0(
          m_pkSendList[byTabRace].list[10].wszGuildName,
          m_ppkRaceStartPos[bySelfRace][found]->m_wszGuildName);
        m_pkSendList[byTabRace].list[10].byGrade = m_ppkRaceStartPos[bySelfRace][found]->m_byGrade;
        strcpy_0(
          m_pkSendList[byTabRace].list[10].wszMasterName,
          m_ppkRaceStartPos[bySelfRace][found]->m_wszMasterName);
        m_pkSendList[byTabRace].byExistSelfRankInfo = 1;
      }

      unsigned __int8 type[36]{};
      type[0] = 13;
      type[1] = 38;
      const unsigned __int16 len = static_cast<unsigned __int16>(m_pkSendList[byTabRace].size());
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&m_pkSendList[byTabRace]), len);
      __trace(
        "CTotalGuildRankInfo::Send( %u, %d, %u, %u, %u ) : Send",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial);
    }
  }
}

bool CTotalGuildRankInfo::Update(_total_guild_rank_info *pkInfo)
{
  if (!m_bInit)
  {
    return false;
  }
  Clear();
  return Load(pkInfo) != 0;
}

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
