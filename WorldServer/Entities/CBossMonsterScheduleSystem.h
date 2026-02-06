#pragma once

#include "IdaCompat.h"
#include "USTaskPool.h"

class CMapOperation;
struct BossSchedule_TBL;
struct ScheduleMSG;

class __cppobj CBossMonsterScheduleSystem
{
public:
  static CBossMonsterScheduleSystem *Instance();
  bool Init(CMapOperation *mapOper);

  CMapOperation *m_pMapOper;
  BossSchedule_TBL *m_pCurTBL;
  US::CDynamicTaskPool<ScheduleMSG, US::CCircularFIFO<unsigned long, US::CriticalSection, 0>> m_MSG_POOL;
  bool m_bRespawnMonster;
};

