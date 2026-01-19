#pragma once

#include "IdaCompat.h"
#include "CAttack.h"

class CPlayer;

class __cppobj CPlayerAttack : public CAttack
{
public:
  CPlayer *m_pAttPlayer;
};
