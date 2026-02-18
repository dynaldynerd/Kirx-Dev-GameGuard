#pragma once

#include "IdaCompat.h"

class CPlayer;
class CMapData;

/* 1637 */
class  CLootingMgr
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
    double GetScore(bool bBonus);
  };

  bool m_bFirst;
  unsigned __int8 m_byUserNode;
  _list m_AtterList[64];

  CLootingMgr();
  void Init(unsigned __int8 nUserNode);
  void PushDamage(CPlayer *pAtter, unsigned __int16 wDamage);
  CPlayer *GetLooter(CMapData *pMap, float *pPos, CPlayer *pSkipObject);
};

