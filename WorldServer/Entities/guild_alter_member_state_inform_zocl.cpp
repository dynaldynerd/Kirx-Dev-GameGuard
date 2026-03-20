#include "pch.h"

#include "guild_alter_member_state_inform_zocl.h"

_guild_alter_member_state_inform_zocl::_guild_alter_member_state_inform_zocl()
{
  byAlterMemberNum = 0;
}

int _guild_alter_member_state_inform_zocl::size()
{
  if (byAlterMemberNum > 50)
  {
    byAlterMemberNum = 0;
  }
  // narrowing cast for thunk return parity
  return static_cast<int>(451 - 9LL * (50 - byAlterMemberNum));
}
