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
