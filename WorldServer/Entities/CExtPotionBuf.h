#pragma once

#include "IdaCompat.h"

class CPlayer;

/* 1784 */
class  CExtPotionBuf
{
public:
  CExtPotionBuf();
  ~CExtPotionBuf();
  void SetExtPotionBufUse(bool bUse);
  void SetExtPotionEndTime(unsigned int dwEndTime);
  void CalcRemainTime(unsigned __int16 wInx, bool bUse);
  bool IsExtPotionUse();
  void CheckPotionTime(CPlayer *pOne);
  void SendMsg_RemainBufUseTime(char bUse, unsigned __int16 wIndex, char nEndDay, char nEndHour, char nEndMin);
  void SednMsg_RemovePotionContEffect(unsigned __int16 wPotionInx, unsigned __int16 wIndex);
  void UseBuffPotion(CPlayer *pOne);

private:
  bool m_bExtPotionBufUse;
  bool m_bDayChange;
  unsigned int m_dwEndPotionTime;
};

