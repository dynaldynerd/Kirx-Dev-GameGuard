#pragma once

#include "IdaCompat.h"

struct _qry_case_selfleave
{
  unsigned int in_leaverserial;
  int in_MemberNum;
  unsigned int tmp_leaverindex;
  unsigned int tmp_guildindex;
  unsigned int tmp_guildserial;

  int size() const
  {
    return static_cast<int>(sizeof(*this));
  }
};
