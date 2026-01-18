#pragma once

#include "IdaCompat.h"

/* 1783 */
struct __cppobj CPotionParam
{
  _ContPotionData m_ContCommonPotionData[2];
  _ContPotionData m_StoneOfMovePotionData;
  unsigned int m_dwNextUseTime[38];
  CPlayer *m_pMaster;
};
