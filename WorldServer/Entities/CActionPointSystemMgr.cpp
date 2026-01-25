#include "pch.h"

#include "CActionPointSystemMgr.h"

CActionPointSystemMgr *CActionPointSystemMgr::Instance()
{
  static CActionPointSystemMgr s_instance;
  return &s_instance;
}

bool CActionPointSystemMgr::Initialize()
{
  return true;
}
