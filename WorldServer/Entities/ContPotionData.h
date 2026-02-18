#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _ContPotionData
{
  unsigned int m_dwPotionEffectIndex;
  unsigned int m_dwStartSec;
  unsigned __int16 m_wDurCapSec;
  unsigned int m_dwID;
  static unsigned int ms_dwCount;

  _ContPotionData();

  void Init();
  void Set(unsigned int dwPotionIndex, unsigned int dwStartTime, unsigned __int16 wDurCapSec);
  bool IsLive();
  unsigned int GetEffectIndex();
};
#pragma pack(pop)
