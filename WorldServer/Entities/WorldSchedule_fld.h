#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _WorldSchedule_fld : _base_fld
{
  int m_nHour;
  int m_nMin;
  int m_nEventCode;
  int m_nEventInfo1;
  int m_nEventInfo2;
  int m_nEventInfo3;
};
#pragma pack(pop)
