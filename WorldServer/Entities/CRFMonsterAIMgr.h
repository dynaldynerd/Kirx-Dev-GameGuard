#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

class __cppobj CRFMonsterAIMgr
{
public:
  CRFMonsterAIMgr();
  ~CRFMonsterAIMgr();

  static CRFMonsterAIMgr *Instance();
  static void Destory();

  UsPoint<UsStateTBL> *GetStateTBL(UsPoint<UsStateTBL> *result, int nIndex);

  static CRFMonsterAIMgr *ms_Instance;

  UsPoint<UsStateTBL> m_spStateTBLPoolPtr[1];
};
