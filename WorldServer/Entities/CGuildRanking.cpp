#include "pch.h"

#include "CGuildRanking.h"

#include "CLogFile.h"
#include "CCheckSumGuildData.h"
#include "GlobalObjects.h"
#include "qry_case_rank_racerank_guildrank.h"
#include "WorldServerUtil.h"
#include "worlddb_guild_info.h"
#include "worlddb_guild_member_info.h"
#include "worlddb_guild_money_io_info.h"
#include "worlddb_rankinguild_info.h"

#include <algorithm>
#include <cstring>

CGuildRanking::CGuildRanking()
{
  m_dwCurProcIndex = 0;
  m_dwNextProcIndex = 0;
  m_dwAllGuildCount = 0;
  m_nRankInGuildJobOffset = 0;
}

CGuildRanking::~CGuildRanking()
{
  for (size_t index = 0; index < m_vecGuildMemberRefresh.size(); ++index)
  {
    delete m_vecGuildMemberRefresh[index];
    m_vecGuildMemberRefresh[index] = nullptr;
  }
}

bool CGuildRanking::Init()
{
  unsigned int zero = 0;
  m_vecAllGuildSerial.assign(MAX_GUILD, zero);

  unsigned char zeroByte = 0;
  m_vecAllGuildGrade.assign(MAX_GUILD, zeroByte);

  _guild_member_refresh_data *initPtr = nullptr;
  m_vecGuildMemberRefresh.assign(MAX_GUILD, initPtr);

  for (size_t j = 0; j < m_vecGuildMemberRefresh.size(); ++j)
  {
    _guild_member_refresh_data *entry =
      reinterpret_cast<_guild_member_refresh_data *>(operator new(sizeof(_guild_member_refresh_data)));
    m_vecGuildMemberRefresh[j] = entry;
    if (entry == nullptr)
    {
      return false;
    }
  }
  return true;
}

char CGuildRanking::Load()
{
  _worlddb_guild_info info{};
  if (!UpdateRankAndGrade())
  {
    m_pkLogger->Write("bool CGuildRanking::Load() : UpdateRankAndGrade(..) Fail!\r\n");
    return 0;
  }

  info.wGuildCount = 0;
  memset(info.GuildData, 0, sizeof(info.GuildData));
  if (!LoadAllGuildData(&info))
  {
    m_pkLogger->Write("bool CGuildRanking::Load() : UpdateRankAndGrade(..) Fail!\r\n");
    return 0;
  }

  if (!SetLoadAllGuildInfo(&info))
  {
    m_pkLogger->Write("bool CGuildRanking::Load() : SetLoadAllGuildInfo(..) Fail!\r\n");
    return 0;
  }

  return 1;
}

char CGuildRanking::UpdateRankAndGrade()
{
  char today[32]{};
  GetTodayStr(today);
  return CheckAndCreateTodayGuildRankTable(today);
}

char CGuildRanking::CheckAndCreateTodayGuildRankTable(char *szDate)
{
  char buffer[56]{};
  g_Main.m_logLoadingError.Write("Guild Rank Table Make Start!!");
  sprintf_s(buffer, 32uLL, "tbl_GuildRank%s", szDate);

  if (!g_Main.m_pWorldDB->TableExist(buffer))
  {
    if (!g_Main.m_pWorldDB->Update_GuildRank(szDate))
    {
      MyMessageBox("DatabaseInit", "create guild-rank-table fail");
      return 0;
    }

    if (!g_Main.m_pWorldDB->Update_GuildGrade())
    {
      MyMessageBox("DatabaseInit", "update guild-grade fail");
      return 0;
    }
  }

  g_Main.m_logLoadingError.Write("Guild Rank Table Make Complete!!");
  return 1;
}

char CGuildRanking::LoadAllGuildData(_worlddb_guild_info *pkInfo)
{
  memset_0(pkInfo, 0, sizeof(_worlddb_guild_info));
  if (!g_Main.m_pWorldDB->Select_AllGuildData(pkInfo))
  {
    MyMessageBox("DatabaseInit", "Select_AllGuildData() fail");
    return 0;
  }

  g_Main.m_logLoadingError.Write("Guild List(%d) Load Complete!!", pkInfo->wGuildCount);
  for (unsigned __int16 j = 0; j < static_cast<unsigned __int16>(pkInfo->wGuildCount); ++j)
  {
    if (!CheckGuildCheckSum(
          pkInfo->GuildData[j].dwGuildSerial,
          pkInfo->GuildData[j].wszGuildName,
          &pkInfo->GuildData[j].dDalant,
          &pkInfo->GuildData[j].dGold))
    {
      return 0;
    }
  }

  return 1;
}

char CGuildRanking::SetLoadAllGuildInfo(_worlddb_guild_info *pkInfo)
{
  unsigned __int16 memberCount = 0;
  _guild_member_info members[50];
  int ioMoneyHistoryCount = 0;
  _io_money_data ioInfo[100]{};

  for (unsigned __int16 j = 0; j < static_cast<unsigned __int16>(pkInfo->wGuildCount); ++j)
  {
    memberCount = 0;
    memset_0(members, 0, sizeof(members));
    if (!LoadMemberInfo(
          pkInfo->GuildData[j].dwMasterSerial,
          pkInfo->GuildData[j].dwGuildSerial,
          members,
          &memberCount))
    {
      continue;
    }

    ioMoneyHistoryCount = 0;
    memset_0(ioInfo, 0, sizeof(ioInfo));
    if (!LoadGuildMoneyIOInfo(pkInfo->GuildData[j].dwGuildSerial, ioInfo, &ioMoneyHistoryCount))
    {
      continue;
    }

    if (g_Main.check_min_max_guild_money(pkInfo->GuildData[j].dwGuildSerial, nullptr, nullptr))
    {
      g_Main.m_logLoadingError.Write(
        "CGuildRanking::SetLoadAllGuildInfo(...) : %d > g_Main.check_min_max_guild_money(...) Fail!",
        pkInfo->GuildData[j].dwGuildSerial);
      MyMessageBox(
        "Error",
        "CGuildRanking::SetLoadAllGuildInfo(...) : \r\n%d > g_Main.check_min_max_guild_money(...) Fail!",
        pkInfo->GuildData[j].dwGuildSerial);
      return 0;
    }

    CheckMaxGuildMoney(
      pkInfo->GuildData[j].dwGuildSerial,
      pkInfo->GuildData[j].wszGuildName,
      &pkInfo->GuildData[j].dDalant,
      &pkInfo->GuildData[j].dGold);

    g_Guild[j].SetGuild(
      pkInfo->GuildData[j].dwGuildSerial,
      pkInfo->GuildData[j].byGuildGrade,
      pkInfo->GuildData[j].byRace,
      pkInfo->GuildData[j].wszGuildName,
      pkInfo->GuildData[j].wszGreetingMsg,
      pkInfo->GuildData[j].dwEmblemBack,
      pkInfo->GuildData[j].dwEmblemMark,
      memberCount,
      members,
      pkInfo->GuildData[j].dDalant,
      pkInfo->GuildData[j].dGold,
      pkInfo->GuildData[j].dwMasterSerial,
      pkInfo->GuildData[j].byMasterPrevGrade,
      ioMoneyHistoryCount,
      ioInfo,
      pkInfo->GuildData[j].dwTotWin,
      pkInfo->GuildData[j].dwTotDraw,
      pkInfo->GuildData[j].dwTotLose);
  }

  g_Main.m_logLoadingError.Write("Guild List(%d) Setting Complete!!", pkInfo->wGuildCount);
  return 1;
}

char CGuildRanking::LoadMemberInfo(
  unsigned int dwMasterSerial,
  unsigned int dwGuildSerial,
  _guild_member_info *pkMemberInfo,
  unsigned __int16 *pwMemberCnt)
{
  _worlddb_guild_member_info memberInfo{};
  if (!g_Main.m_pWorldDB->Select_GuildMemberData(50u, dwGuildSerial, &memberInfo))
  {
    return 0;
  }

  *pwMemberCnt = memberInfo.wMemberCount;
  for (int j = 0; j < memberInfo.wMemberCount; ++j)
  {
    pkMemberInfo[j].dwSerial = memberInfo.MemberData[j].dwSerial;
    pkMemberInfo[j].byClassInGuild = memberInfo.MemberData[j].byClassInGuild;
    pkMemberInfo[j].byLv = memberInfo.MemberData[j].byLv;
    pkMemberInfo[j].dwPvpPoint = memberInfo.MemberData[j].dwPvpPoint;
    pkMemberInfo[j].byRank = static_cast<unsigned __int8>(memberInfo.MemberData[j].wRank - 1);
    strcpy_0(pkMemberInfo[j].wszName, memberInfo.MemberData[j].wszName);

    if (dwMasterSerial == pkMemberInfo[j].dwSerial
        && pkMemberInfo[j].dwSerial != static_cast<unsigned int>(-1)
        && pkMemberInfo[j].byClassInGuild != 2)
    {
      g_Main.m_pWorldDB->Update_UserGuildData(pkMemberInfo[j].dwSerial, dwGuildSerial, 2u);
      pkMemberInfo[j].byClassInGuild = 2;
      g_Main.m_logLoadingError.Write(
        "guild_master_data_error and modify Data(guild:%u, user:%u:, ClassInGuild:%d )",
        dwGuildSerial,
        pkMemberInfo[j].dwSerial,
        2);
    }

    if (pkMemberInfo[j].byClassInGuild == 2
        && dwMasterSerial != pkMemberInfo[j].dwSerial
        && dwMasterSerial != static_cast<unsigned int>(-1))
    {
      g_Main.m_pWorldDB->Update_UserGuildData(pkMemberInfo[j].dwSerial, dwGuildSerial, 0);
      pkMemberInfo[j].byClassInGuild = 0;
      g_Main.m_logLoadingError.Write(
        "guild_master_data_error and modify Data(guild:%u, user:%u:, ClassInGuild:%d )",
        dwGuildSerial,
        pkMemberInfo[j].dwSerial,
        0);
    }
  }

  if (!g_Main.m_pWorldDB->Update_GuildMemberCount(dwGuildSerial, memberInfo.wMemberCount))
  {
    g_Main.m_logLoadingError.Write(
      "Update_GuildMemberCount guild(%u) cnt(%u) Fail!",
      dwGuildSerial,
      memberInfo.wMemberCount);
  }
  return 1;
}

char CGuildRanking::LoadGuildMoneyIOInfo(
  unsigned int dwGuildSerial,
  _io_money_data *pkIOInfo,
  int *pnIOMonHisNum)
{
  _worlddb_guild_money_io_info ioData{};
  memset_0(&ioData, 0, sizeof(ioData));
  if (!g_Main.m_pWorldDB->Select_GuildMoneyIOData(dwGuildSerial, &ioData))
  {
    return 0;
  }

  for (int j = 0; j < ioData.wRecordCount; ++j)
  {
    const int sourceIndex = ioData.wRecordCount - (j + 1);
    pkIOInfo[j].dwIOerSerial = ioData.IOData[sourceIndex].dwIOerSerial;
    pkIOInfo[j].dIODalant = ioData.IOData[sourceIndex].dIODalant;
    pkIOInfo[j].dIOGold = ioData.IOData[sourceIndex].dIOGold;
    pkIOInfo[j].dLeftDalant = ioData.IOData[sourceIndex].dLeftDalant;
    pkIOInfo[j].dLeftGold = ioData.IOData[sourceIndex].dLeftGold;
    memcpy_0(pkIOInfo[j].byDate, ioData.IOData[sourceIndex].byDate, sizeof(pkIOInfo[j].byDate));
    strcpy_0(pkIOInfo[j].wszIOerName, ioData.IOData[sourceIndex].wszIOerName);
  }

  *pnIOMonHisNum = ioData.wRecordCount;
  return 1;
}

char CGuildRanking::CheckGuildCheckSum(
  unsigned int dwSerial,
  char *wszGuildName,
  long double *dDalant,
  long double *dGold)
{
  CCheckSumGuildData guildChecksum(dwSerial);
  CCheckSumGuildData sourceValue(dwSerial);
  int result = 0;

  if (!g_Main.m_bCheckSumActive)
  {
    return 1;
  }

  sourceValue.Encode(*dDalant, *dGold);
  result = static_cast<int>(guildChecksum.Load(g_Main.m_pWorldDB, &sourceValue));
  if (result < 0)
  {
    g_Main.m_logLoadingError.Write("Guild(%u, %s) CheckSum Load Fail!", dwSerial, wszGuildName);
    return 0;
  }

  if (result == 0)
  {
    result = static_cast<int>(guildChecksum.CheckDiff(g_Main.m_pWorldDB, wszGuildName, &sourceValue));
    if (result < 0)
    {
      g_Main.m_logLoadingError.Write("Guild(%u, %s) CheckSum Insert Fail!", dwSerial, wszGuildName);
      return 0;
    }
  }

  if (result > 0)
  {
    const long double correctedDalant = guildChecksum.GetDalant();
    const long double correctedGold = guildChecksum.GetGold();
    g_Main.m_logLoadingError.Write(
      "Guild(%u, %s) Diff Value  dalant(%f) -> %f, Gold(%f) -> %f",
      dwSerial,
      wszGuildName,
      static_cast<double>(correctedDalant),
      static_cast<double>(*dDalant),
      static_cast<double>(correctedGold),
      static_cast<double>(*dGold));

    *dDalant = correctedDalant;
    *dGold = correctedGold;
    if (!g_Main.m_pWorldDB->UpdateGuildMoney(dwSerial, *dDalant, *dGold))
    {
      return 0;
    }
  }

  return 1;
}

void CGuildRanking::CheckMaxGuildMoney(
  unsigned int dwGuildSerial,
  char *wszGuildName,
  long double *pdDalant,
  long double *pdGold)
{
  if (*pdDalant > 1000000000.0)
  {
    g_Main.m_logLoadingError.Write(
      "%d : %s > Guild DALANT (%.0f) (ADJUST %d)",
      dwGuildSerial,
      wszGuildName,
      static_cast<double>(*pdDalant),
      1000000000);
    *pdDalant = 1000000000.0;
  }

  if (*pdGold > static_cast<double>(MAX_GOLD))
  {
    g_Main.m_logLoadingError.Write(
      "%d : %s > Guild GOLD (%.0f) (ADJUST %d)",
      dwGuildSerial,
      wszGuildName,
      static_cast<double>(*pdGold),
      MAX_GOLD);
    *pdGold = static_cast<double>(MAX_GOLD);
  }
}

void CGuildRanking::SetLogger(CLogFile *pkLogger)
{
  m_pkLogger = pkLogger;
}

void CGuildRanking::ApplyGuildGrade()
{
  CGuild *guild = nullptr;
  for (unsigned int j = 0; j < m_dwAllGuildCount; ++j)
  {
    unsigned int guildSerial = m_vecAllGuildSerial[j];
    guild = GetGuildDataFromSerial(g_Guild, MAX_GUILD, guildSerial);
    if (guild)
    {
      unsigned __int8 storedGrade = m_vecAllGuildGrade[j];
      unsigned __int8 currentGrade = guild->GetGrade();
      if (storedGrade != currentGrade)
      {
        guild->UpdateGrade(storedGrade);
      }
    }
  }
}

bool CGuildRanking::ApplyRankInGuild()
{
  bool applied = false;

  for (int j = m_nRankInGuildJobOffset; j < MAX_GUILD; ++j)
  {
    CGuild *guild = &g_Guild[j];
    if (guild->IsFill())
    {
      for (int k = 0; k < MAX_GUILD; ++k)
      {
        _guild_member_refresh_data *refresh = m_vecGuildMemberRefresh[k];
        if (refresh->dwGuildSerial == guild->m_dwSerial)
        {
          guild->RefreshGuildMemberData(refresh);
          guild->SendMsg_AlterMemberState();
          guild->SendMsg_AlterMemberGrade();
          applied = true;
          m_nRankInGuildJobOffset = j + 1;
          m_pkLogger->Write(
            "CGuildRanking::ApplyRankInGuild() : ApplyRankInGuild Success! Serial(%u)",
            guild->m_dwSerial);
          break;
        }
      }
      if (applied)
      {
        break;
      }
    }
  }

  return applied == false;
}

void CGuildRanking::ClearApplyRankInGuildJobOffset()
{
  m_nRankInGuildJobOffset = 0;
}

void CGuildRanking::ClearGuildSerial()
{
  m_dwCurProcIndex = 0;
  m_dwNextProcIndex = 0;
  m_dwAllGuildCount = 0;

  std::fill(m_vecAllGuildSerial.begin(), m_vecAllGuildSerial.end(), 0u);
}

void CGuildRanking::ClearGuildGrade()
{
  std::fill(m_vecAllGuildGrade.begin(), m_vecAllGuildGrade.end(), static_cast<unsigned __int8>(0));
}

unsigned int CGuildRanking::GetRankInGuildJobOffset()
{
  return static_cast<unsigned int>(m_nRankInGuildJobOffset);
}

void CGuildRanking::ClearRefreshData()
{
  if (m_vecGuildMemberRefresh.empty())
  {
    return;
  }

  for (size_t j = 0; j < m_vecGuildMemberRefresh.size(); ++j)
  {
    _guild_member_refresh_data *entry = m_vecGuildMemberRefresh[j];
    if (entry)
    {
      memset_0(entry, 0, sizeof(*entry));
    }
  }
}

unsigned int CGuildRanking::GetNextGuildSerial()
{
  if (!m_dwAllGuildCount || m_dwCurProcIndex == static_cast<unsigned int>(-1) || m_vecAllGuildSerial.empty())
  {
    return static_cast<unsigned int>(-1);
  }

  if (m_vecAllGuildSerial.size() > m_dwAllGuildCount)
  {
    if (m_dwAllGuildCount > m_dwCurProcIndex)
    {
      for (unsigned int j = m_dwNextProcIndex; j < m_dwAllGuildCount; ++j)
      {
        unsigned int guildSerial = m_vecAllGuildSerial[j];
        if (guildSerial && guildSerial != static_cast<unsigned int>(-1))
        {
          m_dwCurProcIndex = j;
          m_dwNextProcIndex = j + 1;
          return guildSerial;
        }
      }

      m_dwCurProcIndex = static_cast<unsigned int>(-1);
      m_dwNextProcIndex = static_cast<unsigned int>(-1);
      return static_cast<unsigned int>(-1);
    }

    m_dwCurProcIndex = static_cast<unsigned int>(-1);
    m_dwNextProcIndex = static_cast<unsigned int>(-1);
    return static_cast<unsigned int>(-1);
  }

  m_dwAllGuildCount = static_cast<unsigned int>(m_vecAllGuildSerial.size());
  return static_cast<unsigned int>(-1);
}

unsigned __int8 CGuildRanking::UpdateGuildRankStep1(char *szData)
{
  if (g_Main.m_pWorldDB->Update_GuildRank_Step1(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateGuildRankStep2(char *szData)
{
  if (g_Main.m_pWorldDB->Update_GuildRank_Step2(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateGuildRankStep3(char *szData)
{
  if (g_Main.m_pWorldDB->Update_GuildRank_Step3(szData))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateGuildRankStep4(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (m_vecAllGuildSerial.empty())
  {
    query->scProcRet = 3;
    return 0;
  }

  unsigned int *serials = reinterpret_cast<unsigned int *>(m_vecAllGuildSerial.data());
  if (g_Main.m_pWorldDB->SelectAllGuildSerial(&m_dwAllGuildCount, serials))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep1(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  unsigned __int8 updated = g_Main.m_pWorldDB->Update_RankInGuild_Step1(query->dwParam1);
  if (updated == 2)
  {
    query->scProcRet = 4;
    return 0;
  }

  if (updated == 1)
  {
    return 24;
  }

  return 0;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep2(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step2(query->dwParam1))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep3(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step3(query->dwParam1))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep4(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step4(query->dwParam1))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep5(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (m_vecAllGuildSerial.size() > m_dwCurProcIndex
      && m_vecGuildMemberRefresh.size() > m_dwCurProcIndex
      && m_dwAllGuildCount > m_dwCurProcIndex)
  {
    unsigned int guildSerial = m_vecAllGuildSerial[m_dwCurProcIndex];
    if (query->dwParam1 == guildSerial)
    {
      _worlddb_rankinguild_info rankData{};
      rankData.wRecordCount = 0;
      memset_0(rankData.MemberData, 0, sizeof(rankData.MemberData));
      g_Main.m_pWorldDB->Update_RankInGuild_Step5(guildSerial, &rankData);

      _guild_member_refresh_data *refresh = m_vecGuildMemberRefresh[m_dwCurProcIndex];
      refresh->dwGuildSerial = guildSerial;
      refresh->wMemberCount = rankData.wRecordCount;
      for (int j = 0; j < rankData.wRecordCount; ++j)
      {
        refresh->rMemberData[j].dwSerial = rankData.MemberData[j].dwSerial;
        refresh->rMemberData[j].byGrade = rankData.MemberData[j].byGrade;
        refresh->rMemberData[j].byLv = rankData.MemberData[j].byLv;
        refresh->rMemberData[j].dwPvpPoint = rankData.MemberData[j].dwPvpPoint;
      }
      return 0;
    }

    query->scProcRet = 6;
    return 0;
  }

  query->scProcRet = 5;
  return 0;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep6(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step6())
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateAndSelectGuildGrade(char *szData)
{
  auto *query = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(szData);
  if (g_Main.m_pWorldDB->Update_GuildGrade() || g_Main.m_pWorldDB->Update_GuildGrade())
  {
    unsigned __int8 *grades = m_vecAllGuildGrade.data();
    unsigned int *serials = reinterpret_cast<unsigned int *>(m_vecAllGuildSerial.data());
    if (g_Main.m_pWorldDB->SelectAllGuildSerialGrade(&m_dwAllGuildCount, serials, grades))
    {
      return 0;
    }

    grades = m_vecAllGuildGrade.data();
    serials = reinterpret_cast<unsigned int *>(m_vecAllGuildSerial.data());
    if (g_Main.m_pWorldDB->SelectAllGuildSerialGrade(&m_dwAllGuildCount, serials, grades))
    {
      return 0;
    }

    query->scProcRet = 8;
    return 0;
  }

  query->scProcRet = 7;
  return 0;
}
