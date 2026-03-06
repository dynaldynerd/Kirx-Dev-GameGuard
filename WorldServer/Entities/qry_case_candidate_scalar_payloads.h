#pragma once

#include "IdaCompat.h"

struct _qry_case_check_invalid_character
{
  unsigned __int8 byProc;

  __int64 size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_check_invalid_character) == 1);

struct _qry_case_update_vote_time
{
  unsigned int dwSerial;

  __int64 size() const
  {
    return sizeof(*this);
  }
};

static_assert(sizeof(_qry_case_update_vote_time) == 4);
