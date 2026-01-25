#include "pch.h"

#include "CMonsterEventSet.h"

CMonsterEventSet g_MonsterEventSet;

bool CMonsterEventSet::LoadEventSet(char *errBuffer)
{
  if (errBuffer != nullptr)
  {
    errBuffer[0] = '\0';
  }
  return true;
}

bool CMonsterEventSet::LoadEventSetLooting()
{
  return true;
}
