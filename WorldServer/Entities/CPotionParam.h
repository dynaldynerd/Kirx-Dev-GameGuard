#pragma once

#include "IdaCompat.h"
#include "ContPotionData.h"

class CPlayer;

/* 1783 */
class  CPotionParam
{
public:
  CPotionParam();
  ~CPotionParam();

  void Init(CPlayer *pMaster);
  void SetPotionActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime, unsigned int dwActDelay);
  bool IsUsableActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime);

  _ContPotionData m_ContCommonPotionData[2];
  _ContPotionData m_StoneOfMovePotionData;
  unsigned int m_dwNextUseTime[38];
  CPlayer *m_pMaster;
};

