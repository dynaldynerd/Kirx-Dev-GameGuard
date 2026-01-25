#include "pch.h"

#include "CBossMonsterScheduleSystem.h"

CBossMonsterScheduleSystem *CBossMonsterScheduleSystem::Instance()
{
  static CBossMonsterScheduleSystem s_instance;
  return &s_instance;
}

bool CBossMonsterScheduleSystem::Init(CMapOperation *mapOper)
{
  (void)mapOper;
  return true;
}
