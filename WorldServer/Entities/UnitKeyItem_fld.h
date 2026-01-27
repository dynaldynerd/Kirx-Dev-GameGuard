#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct __cppobj _UnitKeyItem_fld : _base_fld
{
  int m_bExist;
  char m_strModel[64];
  int m_nIconIDX;
  char m_strName[64];
  char m_strCivil[64];
  int m_nFRAType;
  int m_bSell;
  int m_bExchange;
  int m_bGround;
  int m_bStoragePossible;
  int m_bUseableNormalAcc;
  char m_strTooltipIndex[64];
};

#pragma pack(pop)
