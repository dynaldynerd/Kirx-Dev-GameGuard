#include "pch.h"

#include "CMonsterAI.h"

#include "CPathMgr.h"

CMonsterAI::CMonsterAI()
  : Us_HFSM()
{
  for (int i = 0; i < 4; ++i)
  {
    m_SFCheckTime[i].m_dwLastCheckTime = static_cast<unsigned int>(-1);
    m_SFCheckTime[i].m_dwGapCheckTime = static_cast<unsigned int>(-1);
  }
  m_pAsistMonster = nullptr;
  m_nCurPathFindFailCount = 0;
}

CMonsterAI::~CMonsterAI() = default;

void CMonsterAI::Init()
{
  Us_HFSM::Init();
  m_PathFinder.Init();
  m_pAsistMonster = nullptr;
  m_nCurPathFindFailCount = 0;
}

int CMonsterAI::SetMyData(UsStateTBL *pStateTBL, void *pObject)
{
  return Us_HFSM::SetMyData(pStateTBL, pObject);
}

SF_Timer *CMonsterAI::GetTimer(unsigned int nIndex)
{
  if (nIndex >= 4)
  {
    return nullptr;
  }
  return &m_SFCheckTime[nIndex];
}

CPathMgr *CMonsterAI::GetPathFinder()
{
  return &m_PathFinder;
}

unsigned int CMonsterAI::GetBattleModeTime()
{
  return m_dwBattleModeTime;
}

void CMonsterAI::SetBattleModeTime(unsigned int dwTempBattleModeTime)
{
  m_dwBattleModeTime = dwTempBattleModeTime;
}

