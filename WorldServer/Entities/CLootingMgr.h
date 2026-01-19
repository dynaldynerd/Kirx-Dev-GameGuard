#pragma once

#include "IdaCompat.h"

class CPlayer;

/* 1637 */
class __cppobj CLootingMgr
{
public:
  struct _list
  {
    CPlayer *pAtter;
    unsigned int dwAtterSerial;
    unsigned int dwAttCount;
    unsigned int dwDamage;
    unsigned int dwLastAttTime;
  };

  bool m_bFirst;
  unsigned __int8 m_byUserNode;
  _list m_AtterList[64];
};

