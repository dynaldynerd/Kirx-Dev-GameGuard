#pragma once

#include "IdaCompat.h"
#include "base_fld.h"
#include "class_fld.h"

#pragma pack(push, 1)

struct _MasteryLimit_fld : _base_fld
{
  int m_nLevel;
  _mastery_lim_data m_MasteryLim;
};

#pragma pack(pop)

static_assert(sizeof(_MasteryLimit_fld) == 0xE8, "_MasteryLimit_fld size must match IDA (0xE8)");
