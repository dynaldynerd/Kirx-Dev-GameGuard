#include "pch.h"

#include "guild_vote_process_inform_zocl.h"

_guild_vote_process_inform_zocl::_guild_vote_process_inform_zocl()
{
  byCommentLen = 0;
}

__int64 _guild_vote_process_inform_zocl::size()
{
  if (byCommentLen >= 64)
  {
    byCommentLen = 0;
  }
  return 116LL - (65 - byCommentLen);
}
