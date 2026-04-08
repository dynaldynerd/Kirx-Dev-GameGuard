#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _NameTxt_fld : _base_fld
{
  char m_NameTag[9][64];
};
#pragma pack(pop)
