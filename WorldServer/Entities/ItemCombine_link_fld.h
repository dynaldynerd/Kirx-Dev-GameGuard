#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct _ItemCombine_link_fld : _base_fld
{
  struct _list
  {
    char m_itmPdCode[64];
  };

  _list m_list[100];
};

#pragma pack(pop)

static_assert(sizeof(_ItemCombine_link_fld) == 6468);
