#include "pch.h"

#include "CGuildRanking.h"

#include "CLogFile.h"
#include "GlobalObjects.h"

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
