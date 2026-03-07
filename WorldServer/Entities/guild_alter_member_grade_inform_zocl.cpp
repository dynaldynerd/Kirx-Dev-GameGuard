#include "pch.h"

#include "guild_alter_member_grade_inform_zocl.h"

_guild_alter_member_grade_inform_zocl::_guild_alter_member_grade_inform_zocl()
{
  byAlterMemberNum = 0;
}

__int64 _guild_alter_member_grade_inform_zocl::size()
{
  if (byAlterMemberNum > 50)
  {
    byAlterMemberNum = 0;
  }
  return 301 - 6LL * (50 - byAlterMemberNum);
}
