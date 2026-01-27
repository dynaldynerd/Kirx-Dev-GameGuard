#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)

struct __cppobj _MakeToolItem_fld : _base_fld
{
  int m_bExist;
  char m_strModel[64];
  int m_nIconIDX;
  char m_strName[64];
  int m_nKindClt;
  int m_nFixPart;
  char m_strCivil[64];
  int m_nLevelLim;
  int m_nUpLevelLim;
  int m_nMoney;
  int m_nStdPrice;
  int m_nStdPoint;
  int m_nGoldPoint;
  int m_nKillPoint;
  int m_nProcPoint;
  int m_nStoragePrice;
  int m_bAbr;
  int m_nDurUnit;
  int m_nToolClass;
  int m_bRepair;
  int m_nEffState;
  int m_bSell;
  int m_bExchange;
  int m_bGround;
  int m_bStoragePossible;
  int m_bUseableNormalAcc;
  int m_bUpgrade;
  char m_strTooltipIndex[64];
  int m_bIsTime;
};

#pragma pack(pop)
