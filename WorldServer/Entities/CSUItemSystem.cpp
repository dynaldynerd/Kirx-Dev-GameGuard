#include "pch.h"

#include "CSUItemSystem.h"

CSUItemSystem *CSUItemSystem::Instance()
{
  static CSUItemSystem s_instance;
  return &s_instance;
}

bool CSUItemSystem::SUItemSystem_Init()
{
  return true;
}
