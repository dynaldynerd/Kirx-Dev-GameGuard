#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct  _OreItem_fld : _base_fld
{
  int m_bExist;
  char m_strModel[64];
  int m_nIconIDX;
  char m_strName[64];
  int m_nOre_List;
  int m_nOre_Level;
  unsigned int m_dwOreProbability;
  int m_nKindClt;
  int m_nMoney;
  int m_nStdPrice;
  int m_nStdPoint;
  int m_nGoldPoint;
  int m_nKillPoint;
  int m_nProcPoint;
  int m_nProcessPoint;
  int m_nStoragePrice;
  int m_bSell;
  int m_bExchange;
  int m_bGround;
  int m_bStoragePossible;
  int m_bUseableNormalAcc;
  int m_nmin_C_random;
  int m_nmax_C_random;
  int m_nProcessPrice;
  char m_strTooltipIndex[64];
  int m_bIsTime;
};

#pragma pack(pop)
