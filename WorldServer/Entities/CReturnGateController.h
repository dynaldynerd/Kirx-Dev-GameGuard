#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CReturnGate;

class __cppobj CReturnGateController
{
public:
  CMyTimer *m_pkTimer;
  CNetIndexList *m_pkEmptyInxList;
  CNetIndexList *m_pkUseInxList;
  unsigned int m_uiGateTotCnt;
  CReturnGate **m_ppkGatePool;
};
