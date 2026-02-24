#include "pch.h"

#include "WorldServerUtil.h"

#include "CMonsterHierarchy.h"
#include "CMonster.h"
#include "ObjectCreateSetData.h"
#include "GlobalObjects.h"
#include "CRecordData.h"
#include "monster_fld.h"

void CMonsterHierarchy::Init()
{

  m_dwTotalCount = 0;
  m_pParentMon = nullptr;
  m_dwParentSerial = static_cast<unsigned int>(-1);
  m_byChildMonSetNum = 0;
  for (unsigned int j = 0; j < 3; ++j)
  {
    for (unsigned int k = 0; k < 0xA; ++k)
    {
      m_pChildMon[j][k] = nullptr;
    }
    m_dwMonCount[j] = 0;
  }
  m_dwChildRecallTime = 0;
}

void CMonsterHierarchy::OnlyOnceInit(CMonster *pThis)
{
  Init();
  m_pThisMon = pThis;
}

CMonster *CMonsterHierarchy::GetParent()
{
  return m_pParentMon;
}

unsigned __int8 CMonsterHierarchy::ChildKindCount()
{
  return m_byChildMonSetNum;
}

__int64 CMonsterHierarchy::GetChildCount(unsigned int nKindIndex)
{
  if (nKindIndex > 2)
  {
    return 0;
  }
  return m_dwMonCount[nKindIndex];
}

CMonster *CMonsterHierarchy::GetChild(int nKind, unsigned int nIndex)
{
  if (nKind < 0)
  {
    return nullptr;
  }

  unsigned int kindCount = ChildKindCount() <= 3 ? ChildKindCount() : 3;
  if (static_cast<unsigned int>(nKind) >= kindCount || (nIndex & 0x80000000u) != 0)
  {
    return nullptr;
  }

  unsigned int childCount = GetChildCount(static_cast<unsigned int>(nKind)) <= 0xA
                              ? static_cast<unsigned int>(GetChildCount(static_cast<unsigned int>(nKind)))
                              : 10u;
  if (nIndex >= childCount)
  {
    return nullptr;
  }

  return m_pChildMon[nKind][nIndex];
}

__int64 CMonsterHierarchy::SearchChildMon(CMonster *pMon)
{
  if (pMon)
  {
    for (unsigned int kind = 0; kind < 3; ++kind)
    {
      for (unsigned int index = 0; index < 0xA; ++index)
      {
        if (m_pChildMon[kind][index] == pMon)
        {
          return 1;
        }
      }
    }
  }

  return 0;
}

__int64 CMonsterHierarchy::PushChildMon(unsigned int nKind, CMonster *pMon)
{
  if (SearchChildMon(pMon))
  {
    return 1;
  }

  if (nKind <= 2)
  {
    for (unsigned int index = 0; index < 0xA; ++index)
    {
      if (!m_pChildMon[nKind][index])
      {
        m_pChildMon[nKind][index] = pMon;
        ++m_dwMonCount[nKind];
        ++m_dwTotalCount;
        return 1;
      }
    }
  }

  return 0;
}

__int64 CMonsterHierarchy::PopChildMon(CMonster *pMon)
{
  if (pMon)
  {
    for (unsigned int kind = 0; kind < 3; ++kind)
    {
      for (unsigned int index = 0; index < 0xA; ++index)
      {
        if (m_pChildMon[kind][index] == pMon)
        {
          m_pChildMon[kind][index]->m_MonHierarcy.SetParent(nullptr);
          m_pChildMon[kind][index] = nullptr;
          --m_dwMonCount[kind];
          --m_dwTotalCount;
          return 1;
        }
      }
    }
  }
  return 0;
}

void CMonsterHierarchy::PopChildMonAll()
{
  for (unsigned int kind = 0; kind < 3; ++kind)
  {
    for (unsigned int index = 0; index < 0xA; ++index)
    {
      if (m_pChildMon[kind][index])
      {
        m_pChildMon[kind][index]->m_MonHierarcy.SetParent(nullptr);
        m_pChildMon[kind][index] = nullptr;
        --m_dwMonCount[kind];
        --m_dwTotalCount;
      }
    }
  }
}

__int64 CMonsterHierarchy::SetParent(CMonster *pMon)
{
  m_pParentMon = pMon;
  return 1;
}

void CMonsterHierarchy::OnChildRegenLoop()
{
  if (!m_pThisMon || !m_pThisMon->m_pMonRec)
  {
    return;
  }

  _monster_fld *monsterRecord = m_pThisMon->m_pMonRec;
  if (!ChildKindCount())
  {
    return;
  }

  if (m_pParentMon || GetLoopTime() - m_dwChildRecallTime <= monsterRecord->m_nGuardRecallTimeMS)
  {
    return;
  }

  m_dwChildRecallTime = GetLoopTime();
  const int kindCount = m_byChildMonSetNum >= 3u ? 3 : m_byChildMonSetNum;
  for (int kind = 0; kind < kindCount; ++kind)
  {
    if (monsterRecord->m_Child[kind].nChildMonNum <= 0)
    {
      break;
    }

    const unsigned int missingCount = monsterRecord->m_Child[kind].nChildMonNum - m_dwMonCount[kind];
    for (unsigned int index = 0; index < missingCount; ++index)
    {
      float newPos[3]{};
      if (m_pThisMon->m_pCurMap->GetRandPosVirtualDumExcludeStdRange(m_pThisMon->m_fCurPos, 100, 0, newPos))
      {
        CMonster *spawned = CreateRepMonster(
          m_pThisMon->m_pCurMap,
          m_pThisMon->m_wMapLayerIndex,
          newPos,
          monsterRecord->m_Child[kind].strChildMon,
          m_pThisMon,
          m_pThisMon->m_bRobExp,
          false,
          false,
          false,
          false);

        if (spawned)
        {
          if (!PushChildMon(static_cast<unsigned int>(kind), spawned))
          {
            spawned->m_MonHierarcy.SetParent(nullptr);
          }
        }
      }
    }
  }
}

void CMonsterHierarchy::OnChildMonsterCreate(_monster_create_setdata *pData)
{

  Init();
  _monster_fld *recordSet = reinterpret_cast<_monster_fld *>(pData->m_pRecordSet);
  m_pParentMon = pData->pParent;
  if (recordSet && !m_pParentMon)
  {
    for (int j = 0; j < 3 && recordSet->m_Child[j].nChildMonNum > 0; ++j)
    {
      _base_fld *record = g_Main.m_tblMonster.GetRecord(recordSet->m_Child[j].strChildMon);
      if (!record)
      {
        recordSet->m_Child[j].nChildMonNum = 0;
        return;
      }
      if (reinterpret_cast<_monster_fld *>(record)->m_bExpDown == 1)
      {
        recordSet->m_Child[j].nChildMonNum = 0;
        return;
      }
      ++m_byChildMonSetNum;
    }
  }
}

void CMonsterHierarchy::OnChildMonsterDestroy()
{

  if (m_pParentMon)
  {
    m_pParentMon->m_MonHierarcy.PopChildMon(m_pThisMon);
  }

  for (unsigned int j = 0; j < 3; ++j)
  {
    for (unsigned int k = 0; k < 0xA; ++k)
    {
      if (m_pChildMon[j][k])
      {
        m_pChildMon[j][k]->Command_ChildMonDestroy(0x1388u);
      }
    }
  }

  Init();
}
