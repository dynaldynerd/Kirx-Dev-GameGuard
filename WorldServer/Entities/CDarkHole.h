#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"

class CDarkHoleChannel;

/* 1600 */
class __cppobj __declspec(align(4)) CDarkHole : CGameObject
{
  CDarkHoleChannel *m_pChannel;
  unsigned int m_dwChannelSerial;
  unsigned int m_dwOpenerSerial;
  char m_wszOpenerName[17];
  char m_aszOpenerName[17];
  unsigned int m_dwNextCloseTime;
  unsigned int m_dwNextHurryTime;
  bool m_bHurry;
};

