#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

/* 1673 */
struct __cppobj CMonsterAI : Us_HFSM
{
  SF_Timer m_SFCheckTime[4];
  CMonster *m_pAsistMonster;
  CPathMgr m_PathFinder;
  unsigned int m_dwBattleModeTime;
  int m_nCurPathFindFailCount;
};
