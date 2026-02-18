#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _mon_active_fld : _base_fld
{
  unsigned int m_dwRegenTime;
  unsigned int m_dwRegenLimNum;
  unsigned int m_dwRegenProp;
  unsigned int m_dwRegenMinNum;
  unsigned int m_dwStdKill;
  unsigned int m_dwRegenMaxNum;
};
#pragma pack(pop)
