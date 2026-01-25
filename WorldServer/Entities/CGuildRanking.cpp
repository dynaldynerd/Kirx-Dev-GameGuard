#include "pch.h"

#include "CGuildRanking.h"

#include <cstring>

bool CGuildRanking::Init()
{
  unsigned int zero = 0;
  m_vecAllGuildSerial.assign(500, zero);

  unsigned char zeroByte = 0;
  m_vecAllGuildGrade.assign(500, zeroByte);

  _guild_member_refresh_data *initPtr = nullptr;
  m_vecGuildMemberRefresh.assign(500, initPtr);

  for (size_t j = 0; j < m_vecGuildMemberRefresh.size(); ++j)
  {
    _guild_member_refresh_data *entry = new _guild_member_refresh_data();
    m_vecGuildMemberRefresh[j] = entry;
    if (entry == nullptr)
    {
      return false;
    }
    std::memset(entry, 0, sizeof(*entry));
  }
  return true;
}

void CGuildRanking::SetLogger(CLogFile *pkLogger)
{
  m_pkLogger = pkLogger;
}
