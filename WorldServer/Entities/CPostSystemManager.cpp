#include "pch.h"

#include "CPostSystemManager.h"

CPostSystemManager *CPostSystemManager::Instace()
{
  static CPostSystemManager s_instance;
  return &s_instance;
}

bool CPostSystemManager::Init()
{
  return true;
}
