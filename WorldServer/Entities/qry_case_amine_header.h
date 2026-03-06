#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _qry_case_amine_header
{
  unsigned __int8 bySubQryCase;
};
#pragma pack(pop)

static_assert(sizeof(_qry_case_amine_header) == 1);
