#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _exp_fld : _base_fld
{
  char m_strBellato[64];
  char m_strCora[64];
  char m_strAccretia[64];
};
#pragma pack(pop)
