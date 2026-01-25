#pragma once

#include "IdaCompat.h"

class CMapOperation;

class __cppobj CBossMonsterScheduleSystem
{
public:
  static CBossMonsterScheduleSystem *Instance();
  bool Init(CMapOperation *mapOper);
};

