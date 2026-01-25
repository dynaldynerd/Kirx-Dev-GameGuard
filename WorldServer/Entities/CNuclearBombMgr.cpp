#include "pch.h"

#include "CNuclearBombMgr.h"

CNuclearBombMgr *CNuclearBombMgr::Instance()
{
  static CNuclearBombMgr s_instance;
  return &s_instance;
}

bool CNuclearBombMgr::MissileInit()
{
  return true;
}

