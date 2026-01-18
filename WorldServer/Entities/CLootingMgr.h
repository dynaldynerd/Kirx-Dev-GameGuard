#pragma once

#include "IdaCompat.h"

struct CPlayer;

/* 1637 */
struct __cppobj CLootingMgr
{
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
