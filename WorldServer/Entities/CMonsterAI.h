#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

/* 1673 */
class  CMonsterAI : public Us_HFSM
{
public:
  SF_Timer m_SFCheckTime[4];
  CMonster *m_pAsistMonster;
  CPathMgr m_PathFinder;
  unsigned int m_dwBattleModeTime;
  int m_nCurPathFindFailCount;

  CMonsterAI();
  virtual ~CMonsterAI();
  void Init();
  bool SetMyData(UsStateTBL *pStateTBL, void *pObject);
  SF_Timer *GetTimer(unsigned int nIndex);
  CPathMgr *GetPathFinder();
  unsigned int GetBattleModeTime();
  void SetBattleModeTime(unsigned int dwTempBattleModeTime);
};

