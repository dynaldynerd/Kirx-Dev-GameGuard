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

    _list();
    void Init();
  };

  bool m_bFirst;
  unsigned __int8 m_byUserNode;
  _list m_AtterList[64];

  CLootingMgr();
  void Init(unsigned __int8 nUserNode);
};

