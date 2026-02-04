#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _CashShop_fld : _base_fld
{
  char m_strCsItemCode[64];
  int m_nCsPrice;
  int m_bView;
  char m_strItemPart[64];
  int m_nCsDiscount;
  int m_nCsEvent[8];
};
#pragma pack(pop)
