#include "pch.h"

#include "CWeeklyGuildRankInfo.h"
#include "CMainThread.h"

#include <cstring>

#include "CGuild.h"
#include "CNetworkEX.h"
#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "pvppoint_guild_rank_info.h"
#include "weeklyguildrank_owner_info.h"

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

bool CWeeklyGuildRankOwnerInfo::IsEmpty()
{
  return m_dwSerial == 0;
}

CWeeklyGuildRankRecord::CWeeklyGuildRankRecord()
{
  Clear();
}

CWeeklyGuildRankRecord::~CWeeklyGuildRankRecord()
{
  // this is not a stub
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

__int64 CWeeklyGuildRankInfo::_weekly_guild_rank_result_zocl::size()
{
  if (byCnt < 0xBu)
  {
    return 260;
  }
  return 0;
}

CWeeklyGuildRankInfo::CWeeklyGuildRankInfo()
  : m_bInit(false),
    m_NoDataPrev(true),
    m_bNoDataToday(true),
    m_ppkInfo(nullptr),
    m_pkSendList(nullptr)
{
  Clear();
}

CWeeklyGuildRankInfo::~CWeeklyGuildRankInfo()
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

__int64 CWeeklyGuildRankInfo::Find(unsigned __int8 byRace, unsigned int dwGuildSerial)
{
  if (byRace >= 3 || !m_dwRecordCnt[byRace] || !m_ppkRaceStartPos[byRace])
  {
    return -1;
  }

  CWeeklyGuildRankRecord **records = m_ppkRaceStartPos[byRace];
  for (unsigned int j = 0; j < m_dwRecordCnt[byRace]; ++j)
  {
    if (records[j]->m_dwSerial == dwGuildSerial)
    {
      return j;
    }
  }

  return -2;
}

char CWeeklyGuildRankInfo::CheckEmpty(_pvppoint_guild_rank_info *pkInfo)
{
  char isEmpty = 1;
  for (int j = 0; j < pkInfo->wCount; ++j)
  {
    if (pkInfo->list[j].dKillPvpPoint != 0.0 && pkInfo->list[j].dGuildBattlePvpPoint != 0.0)
    {
      return 0;
    }
    if (pkInfo->list[j].wRank != 1)
    {
      isEmpty = 0;
    }
  }
  return isEmpty;
}

char CWeeklyGuildRankInfo::Load(_pvppoint_guild_rank_info *pkInfo, bool *bNoData)
{
  if (!m_bInit || !pkInfo)
  {
    return 0;
  }

  for (int j = 0; j < 3; ++j)
  {
    ++m_pkSendList[j].dwVer;
  }

  if (!pkInfo->wCount)
  {
    return 1;
  }
  if (pkInfo->wRaceCnt[3])
  {
    return 0;
  }

  if (CheckEmpty(pkInfo))
  {
    *bNoData = true;
    return 1;
  }

  unsigned __int16 offset = 0;
  for (int k = 0; k < 3; ++k)
  {
    m_dwRecordCnt[k] = pkInfo->wRaceCnt[k];
    m_ppkRaceStartPos[k] = &m_ppkInfo[offset];
    offset += static_cast<unsigned __int16>(m_dwRecordCnt[k]);
  }

  for (int m = 0; m < pkInfo->wCount; ++m)
  {
    m_ppkInfo[m]->m_dwInx = m;
    m_ppkInfo[m]->m_wRank = pkInfo->list[m].wRank;
    m_ppkInfo[m]->m_dwSerial = pkInfo->list[m].dwSerial;
    strcpy_0(m_ppkInfo[m]->m_wszGuildName, pkInfo->list[m].wszGuildName);
    m_ppkInfo[m]->m_byGrade = pkInfo->list[m].byGrade;
    m_ppkInfo[m]->m_dKillPvpPoint = pkInfo->list[m].dKillPvpPoint;
    m_ppkInfo[m]->m_dGuildBattlePvpPoint = pkInfo->list[m].dGuildBattlePvpPoint;
  }

  for (int n = 0; n < 3; ++n)
  {
    m_pkSendList[n].byRace = static_cast<unsigned __int8>(n);
    m_pkSendList[n].byCnt = m_dwRecordCnt[n] <= 10 ? m_dwRecordCnt[n] : 10;
    for (int ii = 0; ii < m_pkSendList[n].byCnt; ++ii)
    {
      m_pkSendList[n].list[ii].byRank = m_ppkRaceStartPos[n][ii]->m_wRank;
      strcpy_0(m_pkSendList[n].list[ii].wszGuildName, m_ppkRaceStartPos[n][ii]->m_wszGuildName);
      m_pkSendList[n].list[ii].byGrade = m_ppkRaceStartPos[n][ii]->m_byGrade;
      m_pkSendList[n].list[ii].dwPvpPoint =
        static_cast<int>(m_ppkRaceStartPos[n][ii]->m_dKillPvpPoint + m_ppkRaceStartPos[n][ii]->m_dGuildBattlePvpPoint);
    }
  }

  *bNoData = false;
  return 1;
}

bool CWeeklyGuildRankInfo::LoadToday(_pvppoint_guild_rank_info *pkInfo)
{
  return Load(pkInfo, &m_bNoDataToday) != 0;
}

bool CWeeklyGuildRankInfo::LoadPrev(_pvppoint_guild_rank_info *pkInfo)
{
  return Load(pkInfo, &m_NoDataPrev) != 0;
}

char CWeeklyGuildRankInfo::LoadOwner(_weeklyguildrank_owner_info *pkInfo)
{
  if (pkInfo && !pkInfo->wRaceCnt[3])
  {
    for (int j = 0; j < 3; ++j)
    {
      const int baseIndex = 2 * j;
      for (int k = 0; k < 2; ++k)
      {
        const int infoIndex = baseIndex + k;
        m_kOwnerInfo[j][k].m_dwSerial = pkInfo->list[infoIndex].dwSerial;
        strcpy_0(m_kOwnerInfo[j][k].m_wszGuildName, pkInfo->list[infoIndex].wszGuildName);
        m_kOwnerInfo[j][k].m_byRace = pkInfo->list[infoIndex].byRace;
        m_kOwnerInfo[j][k].m_wRank = pkInfo->list[infoIndex].wRank;
        m_kOwnerInfo[j][k].m_byGrade = pkInfo->list[infoIndex].byGrade;
        m_kOwnerInfo[j][k].m_dKillPvpPoint = pkInfo->list[infoIndex].dKillPvpPoint;
        m_kOwnerInfo[j][k].m_dGuildBattlePvpPoint = pkInfo->list[infoIndex].dGuildBattlePvpPoint;
        m_kOwnerInfo[j][k].m_dwSumLv = pkInfo->list[infoIndex].dwSumLv;
      }

      g_Main.m_kEtcNotifyInfo.UpdateSettlementOwner(
        j,
        m_kOwnerInfo[j][0].m_dwSerial,
        m_kOwnerInfo[j][1].m_dwSerial);
    }

    CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
    system->Log("CWeeklyGuildRankInfo::LoadOwner() : Success! ");
    return 1;
  }

  CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
  system->Log("CWeeklyGuildRankInfo::LoadOwner() : ( NULL == pkInfo || 0 < pkInfo->wRaceCnt[RACE_NUM] ) Fail!");
  return 0;
}

bool CWeeklyGuildRankInfo::Update(_pvppoint_guild_rank_info *pkInfo)
{
  if (!m_bInit)
  {
    return false;
  }

  m_NoDataPrev = true;
  m_bNoDataToday = true;
  ClearRank();
  return Load(pkInfo, &m_bNoDataToday) != 0;
}

bool CWeeklyGuildRankInfo::UpdateOwner(_weeklyguildrank_owner_info *pkInfo)
{
  ClearOwner();
  return LoadOwner(pkInfo) != 0;
}

CGuild *CWeeklyGuildRankInfo::GetOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth)
{
  if (m_bInit && byNth < 2u && byRace < 3u)
  {
    if (m_kOwnerInfo[byRace][byNth].IsEmpty())
    {
      CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
      system->Log(
        "CWeeklyGuildRankInfo::GetOwnerGuild(%u, %u) : m_kOwnerInfo[%u][%u] Empty!",
        byRace,
        byNth,
        byRace,
        byNth);
      return nullptr;
    }

    if (SETTLEMENT_AREA_MANAGE_OWNER_LIMIT_GRADE <= static_cast<int>(m_kOwnerInfo[byRace][byNth].m_byGrade))
    {
      CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
      const long double sumPoint =
        m_kOwnerInfo[byRace][byNth].m_dKillPvpPoint + m_kOwnerInfo[byRace][byNth].m_dGuildBattlePvpPoint;
      system->Log(
        "CWeeklyGuildRankInfo::GetOwnerGuild(%u, %u) : m_kOwnerInfo[%u][%u] Info : %u(%s) R(%u) G(%u) SumPvpPoint(%f) KillPvpPoint(%f) SumLv(%u) GuildBattlePvpPoint(%f)",
        byRace,
        byNth,
        byRace,
        byNth,
        m_kOwnerInfo[byRace][byNth].m_dwSerial,
        m_kOwnerInfo[byRace][byNth].m_wszGuildName,
        m_kOwnerInfo[byRace][byNth].m_byRace,
        m_kOwnerInfo[byRace][byNth].m_byGrade,
        static_cast<double>(sumPoint),
        static_cast<double>(m_kOwnerInfo[byRace][byNth].m_dKillPvpPoint),
        m_kOwnerInfo[byRace][byNth].m_dwSumLv,
        static_cast<double>(m_kOwnerInfo[byRace][byNth].m_dGuildBattlePvpPoint));
      return GetGuildDataFromSerial(g_Guild, MAX_GUILD, m_kOwnerInfo[byRace][byNth].m_dwSerial);
    }

    CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
    system->Log(
      "CWeeklyGuildRankInfo::GetOwnerGuild(%u, %u) : m_kOwnerInfo[%u][%u] Empty!",
      byRace,
      byNth,
      byRace,
      byNth);
    return nullptr;
  }

  CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
  system->Log(
    "CWeeklyGuildRankInfo::GetOwnerGuild(%u, %u) : m_bInit(%d) Invalid Internal Process!",
    byRace,
    byNth,
    m_bInit);
  return nullptr;
}

CGuild *CWeeklyGuildRankInfo::GetPrevOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth)
{
  if (m_NoDataPrev)
  {
    CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
    system->Log("CWeeklyGuildRankInfo::GetPrevOwnerGuild(%u, %u) : No Prev Owner Data!", byRace, byNth);
    return nullptr;
  }

  CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
  system->Log("CWeeklyGuildRankInfo::GetPrevOwnerGuild(%u, %u) : ", byRace, byNth);
  return GetOwnerGuild(byRace, byNth);
}

CGuild *CWeeklyGuildRankInfo::GetCurOwnerGuild(unsigned __int8 byRace, unsigned __int8 byNth)
{
  if (m_bNoDataToday)
  {
    CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
    system->Log("CWeeklyGuildRankInfo::GetCurOwnerGuild(%u, %u) : No Today Owner Data!", byRace, byNth);
    return nullptr;
  }

  CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
  system->Log("CWeeklyGuildRankInfo::GetCurOwnerGuild(%u, %u) : ", byRace, byNth);
  return GetOwnerGuild(byRace, byNth);
}

bool CWeeklyGuildRankInfo::IsNoDataPrev()
{
  return m_NoDataPrev;
}

bool CWeeklyGuildRankInfo::IsNoDataToday()
{
  return m_bNoDataToday;
}

void CWeeklyGuildRankInfo::SetNoDataFlagToday()
{
  m_bNoDataToday = true;
}

void CWeeklyGuildRankInfo::Send(
  unsigned int dwVer,
  unsigned int n,
  unsigned __int8 byTabRace,
  unsigned __int8 bySelfRace,
  unsigned int dwGuildSerial)
{
  if (m_bInit && byTabRace < 3u && bySelfRace < 3u)
  {
    if (dwVer == m_pkSendList[byTabRace].dwVer)
    {
      __trace(
        "CWeeklyGuildRankInfo::Send( %u, %d, %u, %u, %u ) : %u == m_pkSendList[%u].dwVer(%u)",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial,
        dwVer,
        byTabRace,
        m_pkSendList[byTabRace].dwVer);
    }
    else if (!m_bNoDataToday && m_pkSendList[byTabRace].byCnt)
    {
      const int found = static_cast<int>(Find(bySelfRace, dwGuildSerial));
      __trace(
        "CWeeklyGuildRankInfo::Send( %u, %d, %u, %u, %u ) : Find( byRace, dwGuildSerial ) -> %d!",
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
        m_pkSendList[byTabRace].list[10].dwPvpPoint = 0;
        m_pkSendList[byTabRace].byExistSelfRankInfo = 0;
      }
      else
      {
        m_pkSendList[byTabRace].list[10].byRank = m_ppkRaceStartPos[bySelfRace][found]->m_wRank;
        strcpy_0(
          m_pkSendList[byTabRace].list[10].wszGuildName,
          m_ppkRaceStartPos[bySelfRace][found]->m_wszGuildName);
        m_pkSendList[byTabRace].list[10].byGrade = m_ppkRaceStartPos[bySelfRace][found]->m_byGrade;
        m_pkSendList[byTabRace].list[10].dwPvpPoint =
          static_cast<int>(m_ppkRaceStartPos[byTabRace][found]->m_dKillPvpPoint +
                           m_ppkRaceStartPos[bySelfRace][found]->m_dGuildBattlePvpPoint);
        m_pkSendList[byTabRace].byExistSelfRankInfo = 1;
      }

      unsigned __int8 type[36]{};
      type[0] = 13;
      type[1] = 40;
      const unsigned __int16 len = static_cast<unsigned __int16>(m_pkSendList[byTabRace].size());
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&m_pkSendList[byTabRace]), len);
      __trace(
        "CWeeklyGuildRankInfo::Send( %u, %d, %u, %u, %u ) : Send",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial);
    }
    else
    {
      __trace(
        "CWeeklyGuildRankInfo::Send( %u, %d, %u, %u, %u ) : No Data!",
        dwVer,
        n,
        byTabRace,
        bySelfRace,
        dwGuildSerial);

      unsigned __int8 msg[5]{};
      *reinterpret_cast<unsigned int *>(msg) = m_pkSendList[byTabRace].dwVer;
      msg[4] = byTabRace;
      unsigned __int8 type[16]{};
      type[0] = 13;
      type[1] = 41;
      g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(msg), 5u);
    }
  }
}
