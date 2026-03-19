#pragma once

#include "IdaCompat.h"

class CMonster;
struct _monster_create_setdata;

/* 1640 */
class  CMonsterHierarchy
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
  CMonsterHierarchy();
  virtual ~CMonsterHierarchy();

  void Init();
  CMonster *GetParent();
  unsigned __int8 ChildKindCount();
  unsigned int GetChildCount(unsigned int nKindIndex);
  CMonster *GetChild(int nKind, unsigned int nIndex);
  void OnlyOnceInit(CMonster *pThis);
  int SearchChildMon(CMonster *pMon);
  int PushChildMon(unsigned int nKind, CMonster *pMon);
  int PopChildMon(CMonster *pMon);
  void PopChildMonAll();
  int SetParent(CMonster *pMon);
  void OnChildRegenLoop();
  void OnChildMonsterCreate(_monster_create_setdata *pData);
  void OnChildMonsterDestroy();
};


