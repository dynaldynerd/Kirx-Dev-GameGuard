#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _OreCutting_fld : _base_fld
{
  char m_strResource_Item[64];
  int m_nCutting_probability;
};

#pragma pack(pop)

static_assert(sizeof(_OreCutting_fld) == 0x88, "_OreCutting_fld size must match IDA (0x88)");
