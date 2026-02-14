#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _guild_battle_suggest_request_result_zocl
{
  unsigned __int8 byRet;
  char wszDestGuildName[17];

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _guild_battle_suggest_request_result_zocl::size() const
{
  return byRet ? 1 : 18;
}
