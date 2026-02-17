#include "pch.h"

#include "CLootingMgr.h"
#include "CPlayer.h"

#include <mmsystem.h>

CLootingMgr::_list::_list()
{
  Init();
}

void CLootingMgr::_list::Init()
{
  memset_0(this, 0, sizeof(CLootingMgr::_list));
}

double CLootingMgr::_list::GetScore(bool bBonus)
{
  const double score = static_cast<double>(dwDamage * dwAttCount);
  if (bBonus)
  {
    return score * 1.200000047683716;
  }
  return score;
}

CLootingMgr::CLootingMgr()
{
  m_bFirst = false;
  m_byUserNode = 0;
  for (int j = 0; j < 64; ++j)
  {
    m_AtterList[j].Init();
  }
}

void CLootingMgr::Init(unsigned __int8 nUserNode)
{

  m_byUserNode = nUserNode;
  for (int j = 0; j < m_byUserNode; ++j)
  {
    m_AtterList[j].Init();
  }
  m_bFirst = true;
}

void CLootingMgr::PushDamage(CPlayer *pAtter, unsigned __int16 wDamage)
{
  for (int j = 0; j < m_byUserNode; ++j)
  {
    _list *entry = &m_AtterList[j];
    if (entry->pAtter == pAtter)
    {
      if (entry->dwAtterSerial == pAtter->m_dwObjSerial)
      {
        const unsigned int nextAttCount = entry->dwAttCount + 1;
        if (entry->dwAttCount < nextAttCount)
        {
          entry->dwAttCount = nextAttCount;
        }

        const unsigned int nextDamage = entry->dwDamage + wDamage;
        if (entry->dwDamage <= nextDamage)
        {
          entry->dwDamage = nextDamage;
        }
        entry->dwLastAttTime = timeGetTime();
        return;
      }

      entry->Init();
      break;
    }
  }

  _list *targetEntry = nullptr;
  for (int j = 0; j < m_byUserNode; ++j)
  {
    _list *entry = &m_AtterList[j];
    if (!entry->pAtter)
    {
      targetEntry = entry;
      break;
    }
  }

  if (!targetEntry)
  {
    unsigned int oldestTime = m_AtterList[0].dwLastAttTime;
    targetEntry = &m_AtterList[0];
    for (int j = 1; j < m_byUserNode; ++j)
    {
      _list *entry = &m_AtterList[j];
      if (oldestTime > entry->dwLastAttTime)
      {
        oldestTime = entry->dwLastAttTime;
        targetEntry = entry;
      }
    }

    targetEntry->Init();
    if (targetEntry == &m_AtterList[0])
    {
      m_bFirst = false;
    }
  }

  targetEntry->pAtter = pAtter;
  targetEntry->dwAtterSerial = pAtter->m_dwObjSerial;

  const unsigned int nextAttCount = targetEntry->dwAttCount + 1;
  if (targetEntry->dwAttCount < nextAttCount)
  {
    targetEntry->dwAttCount = nextAttCount;
  }

  const unsigned int nextDamage = targetEntry->dwDamage + wDamage;
  if (targetEntry->dwDamage <= nextDamage)
  {
    targetEntry->dwDamage = nextDamage;
  }
  targetEntry->dwLastAttTime = timeGetTime();
}

CPlayer *CLootingMgr::GetLooter(CMapData *pMap, float *pPos, CPlayer *pSkipObject)
{
  (void)pMap;
  (void)pPos;

  for (int j = 0; j < m_byUserNode; ++j)
  {
    double maxScore = 0.0;
    _list *bestEntry = nullptr;

    for (int k = 0; k < m_byUserNode; ++k)
    {
      _list *entry = &m_AtterList[k];
      if (!entry->pAtter)
      {
        continue;
      }

      const bool bonus = (k == 0) ? m_bFirst : false;
      const double score = entry->GetScore(bonus);
      if (score > maxScore)
      {
        maxScore = score;
        bestEntry = entry;
      }
    }

    if (!bestEntry)
    {
      return nullptr;
    }

    if (bestEntry->pAtter == pSkipObject)
    {
      return nullptr;
    }

    if (bestEntry->pAtter->m_bLive && bestEntry->pAtter->m_dwObjSerial == bestEntry->dwAtterSerial)
    {
      return bestEntry->pAtter;
    }

    bestEntry->pAtter = nullptr;
  }

  return nullptr;
}
