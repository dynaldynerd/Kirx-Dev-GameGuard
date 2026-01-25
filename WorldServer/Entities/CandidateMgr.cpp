#include "pch.h"

#include "CandidateMgr.h"

CandidateMgr *CandidateMgr::Instance()
{
  static CandidateMgr s_instance;
  return &s_instance;
}

bool CandidateMgr::Initialize(int maxCount)
{
  (void)maxCount;
  return true;
}
