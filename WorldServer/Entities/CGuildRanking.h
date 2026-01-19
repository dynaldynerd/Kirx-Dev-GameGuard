#pragma once

#include "IdaCompat.h"

#include <vector>

class CLogFile;
struct _guild_member_refresh_data;

class __cppobj __declspec(align(8)) CGuildRanking
{
public:
  CLogFile *m_pkLogger;
  unsigned int m_dwCurProcIndex;
  unsigned int m_dwNextProcIndex;
  unsigned int m_dwAllGuildCount;
  std::vector<unsigned long> m_vecAllGuildSerial;
  std::vector<unsigned char> m_vecAllGuildGrade;
  std::vector<_guild_member_refresh_data *> m_vecGuildMemberRefresh;
  int m_nRankInGuildJobOffset;
};
