#pragma once

#include "IdaCompat.h"

class CPlayer;

/* 1783 */
class __cppobj CPotionParam
{
public:
  void SetPotionActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime, unsigned int dwActDelay);

  _ContPotionData m_ContCommonPotionData[2];
  _ContPotionData m_StoneOfMovePotionData;
  unsigned int m_dwNextUseTime[38];
  CPlayer *m_pMaster;
};

