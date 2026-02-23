#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _ItemExist_fld : _base_fld
{
  int m_bExist;
};

#pragma pack(pop)

static_assert(sizeof(_ItemExist_fld) == 0x48, "_ItemExist_fld size must match IDA layout (0x48)");

