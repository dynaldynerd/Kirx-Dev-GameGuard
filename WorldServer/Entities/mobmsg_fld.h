#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct  _mobmsg_fld : _base_fld
{
  int m_nUsingNum;
  char m_strMobMessage[90][2560];
};
#pragma pack(pop)
