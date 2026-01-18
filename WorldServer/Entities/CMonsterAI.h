#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

/* 1673 */
class __cppobj CMonsterAI : public Us_HFSM
{
public:
  SF_Timer m_SFCheckTime[4];
  CMonster *m_pAsistMonster;
  CPathMgr m_PathFinder;
  unsigned int m_dwBattleModeTime;
  int m_nCurPathFindFailCount;
};

