#include "pch.h"

#include "CLogTypeDBTaskManager.h"

CLogTypeDBTaskManager *CLogTypeDBTaskManager::Instance()
{
  static CLogTypeDBTaskManager s_instance;
  return &s_instance;
}

bool CLogTypeDBTaskManager::Init()
{
  return true;
}
