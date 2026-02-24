#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _select_supply
{
  char m_itmSelect[10][8];
  int m_nSelectNumber;
};

struct _fix_supply
{
  char m_itmFix[8];
  int m_nFixNumber;
};

struct _PcRoom_fld : _base_fld
{
  int m_bUse;
  char m_strEventName[64];
  _select_supply m_SelectSupply[5];
  _fix_supply m_FixSupply[10];
};
#pragma pack(pop)
