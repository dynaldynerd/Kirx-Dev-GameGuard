#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CRecallRequest;

class __cppobj CRecallEffectController
{
public:
  unsigned int m_uiInfoTotCnt;
  CRecallRequest **m_ppkReqeust;
  CMyTimer *m_pkTimer;
  CNetIndexList *m_pkEmptyInxList;
  CNetIndexList *m_pkUseInxList;
};
