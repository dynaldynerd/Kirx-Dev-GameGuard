#pragma once

#include "IdaCompat.h"

#include "CGameObject.h"

class CPlayer;

class CGravityStone : public CGameObject
{
public:
  explicit CGravityStone(unsigned __int16 wInx);

  unsigned int m_dwTakeLimitTime;
  CPlayer *m_pkOwner;
};

