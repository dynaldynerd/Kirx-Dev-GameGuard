#include "pch.h"

#include "CMonsterHierarchy.h"
#include "CMonster.h"

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
          CMonsterHierarchy::SetParent(&m_pChildMon[kind][index]->m_MonHierarcy, nullptr);
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

