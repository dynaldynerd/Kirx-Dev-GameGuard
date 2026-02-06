#include "pch.h"

#include "CMainThread.h"
#include "CBossMonsterScheduleSystem.h"

#include <cstdio>
#include <cstring>

ScheduleMSG::ScheduleMSG()
{
  Init();
}

void ScheduleMSG::Init()
{
  memset_0(this, 0, sizeof(ScheduleMSG));
  m_byKey = 3;
}

bool BossSchedule::Make_LiveCountString(char *strBuff, int nBuffSize)
{
  if (!strBuff)
  {
    return false;
  }
  sprintf_s(strBuff, nBuffSize, "%d", m_LiveCount);
  return true;
}

void BossSchedule::Save_LiveCount(unsigned __int16 wCount)
{
  if (!m_pParent)
  {
    return;
  }
  if (!m_pParent->m_pSystem)
  {
    return;
  }

  ScheduleMSG *pTask = m_pParent->m_pSystem->m_MSG_POOL.PopEmpty();
  if (!pTask)
  {
    return;
  }

  pTask->Init();
  m_LiveCount = wCount;
  pTask->m_byKey = 2;
  strcpy_s(pTask->m_strSection, sizeof(pTask->m_strSection), m_strSection);
  pTask->m_wIniFileIndex = static_cast<unsigned __int16>(m_pParent->m_nIndex);
  if (Make_LiveCountString(pTask->m_strValue, 64))
  {
    m_pParent->m_pSystem->m_MSG_POOL.PushProc(pTask);
  }
}
