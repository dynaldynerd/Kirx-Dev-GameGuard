#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _AnimusItem_fld : _base_fld
{
  char m_strModel[64];
  int m_nIconIDX;
  char m_strCivil[64];
  char m_strName[64];
  int m_nAnimusClass;
  int m_nAnimusType;
  int m_nKindClt;
  int m_nFixPart;
  int m_nMoney;
  int m_nStdPrice;
  int m_nStdPoint;
  int m_nGoldPoint;
  int m_nKillPoint;
  int m_nProcPoint;
  int m_nStoragePrice;
  int m_bSell;
  int m_bExchange;
  int m_bGround;
  int m_bStoragePossible;
  int m_bUseableNormalAcc;
  char m_strTooltipIndex[64];
};
#pragma pack(pop)
