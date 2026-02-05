#include "pch.h"

#include "CHonorGuild.h"

#include <cstring>

_guild_honor_list_result_zocl::_guild_honor_list_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

int _guild_honor_list_result_zocl::size() const
{
  return sizeof(*this);
}

CHonorGuild *CHonorGuild::Instance()
{
  static CHonorGuild s_instance;
  return &s_instance;
}

bool CHonorGuild::Init()
{
  for (int j = 0; j < 3; ++j)
  {
    m_pCurrHonorGuild[j] = new _guild_honor_list_result_zocl();
    if (m_pCurrHonorGuild[j] == nullptr)
    {
      return false;
    }

    m_pNextHonorGuild[j] = new _guild_honor_list_result_zocl();
    if (m_pNextHonorGuild[j] == nullptr)
    {
      return false;
    }

    m_bNext[j] = false;
    m_bSendInform[j] = false;
  }
  return true;
}

unsigned __int8 CHonorGuild::FindHonorGuildRank(unsigned __int8 byRace, unsigned int dwGuildSerial)
{
  if (byRace >= 3u)
  {
    return static_cast<unsigned __int8>(-1);
  }

  for (int j = 0; j < m_pCurrHonorGuild[byRace]->byListNum; ++j)
  {
    if (m_pCurrHonorGuild[byRace]->GuildList[j].dwGuildSerial == dwGuildSerial)
    {
      return static_cast<unsigned __int8>(j);
    }
  }
  return static_cast<unsigned __int8>(-1);
}

char CHonorGuild::CheckHonorGuild(unsigned __int8 byRace, unsigned int dwSerial)
{
  for (int j = 0; j < m_pCurrHonorGuild[byRace]->byListNum; ++j)
  {
    if (m_pCurrHonorGuild[byRace]->GuildList[j].dwGuildSerial == dwSerial)
    {
      return 1;
    }
  }
  return 0;
}
