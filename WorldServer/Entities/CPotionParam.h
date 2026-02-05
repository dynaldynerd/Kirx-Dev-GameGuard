#pragma once

#include "IdaCompat.h"

class CPlayer;

/* 1783 */
class __cppobj CPotionParam
{
public:
  _ContPotionData m_ContCommonPotionData[2];
  _ContPotionData m_StoneOfMovePotionData;
  unsigned int m_dwNextUseTime[38];
  CPlayer *m_pMaster;
};

