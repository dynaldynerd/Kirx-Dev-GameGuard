#pragma once

#include "IdaCompat.h"

class CMonster;

/* 1640 */
class __cppobj __declspec(align(8)) CMonsterHierarchy
{
  unsigned int m_dwTotalCount;
  CMonster *m_pThisMon;
  CMonster *m_pParentMon;
  unsigned int m_dwParentSerial;
  CMonster *m_pChildMon[3][10];
  unsigned int m_dwMonCount[3];
  unsigned __int8 m_byChildMonSetNum;
  unsigned int m_dwChildRecallTime;

public:
  virtual ~CMonsterHierarchy() = default;
  CMonster *GetParent();
  unsigned __int8 ChildKindCount();
  __int64 GetChildCount(unsigned int nKindIndex);
  CMonster *GetChild(int nKind, unsigned int nIndex);
  __int64 PopChildMon(CMonster *pMon);
  __int64 SetParent(CMonster *pMon);
};

