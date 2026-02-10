#include "pch.h"

#include "CPotionParam.h"

void CPotionParam::SetPotionActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime, unsigned int dwActDelay)
{
  if (byPotionClass < 0x26u)
  {
    m_dwNextUseTime[byPotionClass] = dwActDelay + dwCurrTime;
  }
}
