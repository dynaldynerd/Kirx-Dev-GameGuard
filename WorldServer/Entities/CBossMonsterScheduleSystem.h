#pragma once

#include "IdaCompat.h"
#include "USTaskPool.h"
#include "USThread.h"

class CMapOperation;
class CMapData;
struct INI_Section;
struct BossSchedule;
struct BossSchedule_Map;
struct BossSchedule_TBL;
struct ScheduleMSG;
struct _mon_active;
struct _mon_block;

class __cppobj CBossMonsterScheduleSystem
  : public US::CWinThread<US::ThreadParamInterface<CBossMonsterScheduleSystem, US::AbstractThreadPool>>
{
public:
  static CBossMonsterScheduleSystem *Instance();
  static CBossMonsterScheduleSystem *m_pInstance;

  CBossMonsterScheduleSystem();
  ~CBossMonsterScheduleSystem() override;

  bool Init(CMapOperation *mapOper);
  bool CreateTaskPool();
  bool CreateWorkerThread();
  BossSchedule_TBL *MakeTBL(CMapOperation *pMapOper);
  BossSchedule_Map *MakeMap(int nIndex, CMapData *pMap);
  BossSchedule *MakeSchedule(
    BossSchedule_Map *pMapSchedule,
    _mon_active *pMonAct,
    _mon_block *pBlock,
    int nActIndex,
    int nBlockIndex);
  BossSchedule *LoadSchedule(BossSchedule_Map *pMapSchedule, INI_Section *pSection);
  void Savechedule(BossSchedule_Map *pMapSchedule, BossSchedule *pSchedule);
  void AnalysisMsg(ScheduleMSG *pMSG);
  void RespawnMonster();
  __int64 WorkProc() override;

  CMapOperation *m_pMapOper;
  BossSchedule_TBL *m_pCurTBL;
  US::CDynamicTaskPool<ScheduleMSG, US::CCircularFIFO<unsigned long, US::CriticalSection, 0>> m_MSG_POOL;
  bool m_bRespawnMonster;
};

