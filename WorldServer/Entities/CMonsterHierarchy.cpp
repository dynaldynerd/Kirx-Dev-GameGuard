#include "pch.h"

#include "CMonsterHierarchy.h"
#include "CMonster.h"
#include "ObjectCreateSetData.h"
#include "GlobalObjects.h"
#include "CRecordData.h"

void CMonsterHierarchy::Init()
{
  unsigned int stackFill = 0;
  unsigned int *fillPtr = &stackFill;
  for (int fillCount = 4; fillCount; --fillCount)
  {
    *fillPtr++ = 0xCCCCCCCC;
  }

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

__int64 CMonsterHierarchy::SetParent(CMonster *pMon)
{
  m_pParentMon = pMon;
  return 1;
}

void CMonsterHierarchy::OnChildMonsterCreate(_monster_create_setdata *pData)
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 16; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  Init();
  _base_fld *recordSet = pData->m_pRecordSet;
  m_pParentMon = pData->pParent;
  if (!m_pParentMon)
  {
    for (int j = 0; j < 3 && *reinterpret_cast<int *>(&recordSet[j + 27].m_strCode[24]) > 0; ++j)
    {
      _base_fld *record = g_Main.m_tblMonster.GetRecord(&recordSet[j + 26].m_strCode[28]);
      if (!record)
      {
        *reinterpret_cast<int *>(&recordSet[j + 27].m_strCode[24]) = 0;
        return;
      }
      if (record[4].m_dwIndex == 1)
      {
        *reinterpret_cast<int *>(&recordSet[j + 27].m_strCode[24]) = 0;
        return;
      }
      ++m_byChildMonSetNum;
    }
  }
}

void CMonsterHierarchy::OnChildMonsterDestroy()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 12; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

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
