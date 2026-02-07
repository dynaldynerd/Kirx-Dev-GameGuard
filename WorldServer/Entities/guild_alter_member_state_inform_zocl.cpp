#include "pch.h"

#include "guild_alter_member_state_inform_zocl.h"

_guild_alter_member_state_inform_zocl::_guild_alter_member_state_inform_zocl()
{
  byAlterMemberNum = 0;
}

__int64 _guild_alter_member_state_inform_zocl::size()
{
  if (byAlterMemberNum > 0x32u)
  {
    byAlterMemberNum = 0;
  }
  return 451 - 9LL * (50 - byAlterMemberNum);
}
