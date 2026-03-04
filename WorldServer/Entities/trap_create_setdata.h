#pragma once

#include "ObjectCreateSetData.h"

class CPlayer;

struct _trap_create_setdata : _character_create_setdata
{
  int nHP;
  CPlayer *pMaster;
  int nTrapMaxAttackPnt;

  _trap_create_setdata()
  {
    nHP = 0;
    pMaster = nullptr;
    nTrapMaxAttackPnt = 0;
  }
};
