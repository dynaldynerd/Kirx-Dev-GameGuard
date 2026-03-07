#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _ItemLooting_fld : _base_fld
{
  int m_nLootRate;
  int m_nLootTime;
  int m_nOperationCount;
  int m_nLootListCount;
  char m_itmLootCodeKey[200][8];
};

#pragma pack(pop)

static_assert(sizeof(_ItemLooting_fld) == 1684, "_ItemLooting_fld size must match IDA (1684)");

