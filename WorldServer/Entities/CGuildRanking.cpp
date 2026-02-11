#include "pch.h"

#include "CGuildRanking.h"

#include "CLogFile.h"
#include "GlobalObjects.h"
#include "worlddb_guild_info.h"
#include "worlddb_guild_member_info.h"
#include "worlddb_guild_money_io_info.h"
#include "worlddb_rankinguild_info.h"

#include <algorithm>
#include <cstring>

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
  if (m_vecAllGuildSerial.empty())
  {
    szData[9] = 3;
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
  unsigned __int8 updated = g_Main.m_pWorldDB->Update_RankInGuild_Step1(*reinterpret_cast<unsigned int *>(szData + 12));
  if (updated == 2)
  {
    szData[9] = 4;
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
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step2(*reinterpret_cast<unsigned int *>(szData + 12)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep3(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step3(*reinterpret_cast<unsigned int *>(szData + 12)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep4(char *szData)
{
  if (g_Main.m_pWorldDB->Update_RankInGuild_Step4(*reinterpret_cast<unsigned int *>(szData + 12)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 CGuildRanking::UpdateRankinGuildStep5(char *szData)
{
  if (m_vecAllGuildSerial.size() > m_dwCurProcIndex
      && m_vecGuildMemberRefresh.size() > m_dwCurProcIndex
      && m_dwAllGuildCount > m_dwCurProcIndex)
  {
    unsigned int guildSerial = m_vecAllGuildSerial[m_dwCurProcIndex];
    if (*reinterpret_cast<unsigned int *>(szData + 12) == guildSerial)
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

    szData[9] = 6;
    return 0;
  }

  szData[9] = 5;
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

    szData[9] = 8;
    return 0;
  }

  szData[9] = 7;
  return 0;
}
