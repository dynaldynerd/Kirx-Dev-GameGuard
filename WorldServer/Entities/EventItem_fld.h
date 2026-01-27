#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct __cppobj _EventItem_fld : _base_fld
{
  int m_nIconIDX;
  char m_strName[64];
  char m_strTooltipIndex[64];
};

#pragma pack(pop)
