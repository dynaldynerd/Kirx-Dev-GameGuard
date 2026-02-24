#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _grade_fld : _base_fld
{
  float m_fRanking;
  int m_nGrade;
};
#pragma pack(pop)
