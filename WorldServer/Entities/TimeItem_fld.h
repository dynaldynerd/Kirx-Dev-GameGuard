#pragma once

#include "base_fld.h"

#pragma pack(push, 1)
struct  _TimeItem_fld : _base_fld
{
  char m_strLendItemCode[64];
  int m_nCheckType;
  int m_nUseTime;
};
#pragma pack(pop)
