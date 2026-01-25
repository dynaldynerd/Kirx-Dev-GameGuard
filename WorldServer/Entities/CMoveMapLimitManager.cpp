#include "pch.h"

#include "CMoveMapLimitManager.h"

CMoveMapLimitManager *CMoveMapLimitManager::Instance()
{
  static CMoveMapLimitManager s_instance;
  return &s_instance;
}

bool CMoveMapLimitManager::Init()
{
  return true;
}
