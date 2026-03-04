#pragma once

#include "IdaCompat.h"
#include "CIndexList.h"

class CPlayer;

/* 1735 */
class  CRealMoveRequestDelayChecker
{
public:
  CRealMoveRequestDelayChecker();
  ~CRealMoveRequestDelayChecker();

  CIndexList m_kNodeInxOrderList;
  std::vector<unsigned long> m_vecDelayList;
  unsigned __int16 m_wTotalMissCount;
  bool m_bPrevRet;
  unsigned __int16 m_wContinueMissCount;
  unsigned __int16 m_wSingleMissCount;
  unsigned __int16 m_wContinueValiedCount;
  unsigned __int16 m_wTotalContinueValiedCount;

  bool Init(unsigned int listCount);
  bool Check(CPlayer *pkUser);
  void Reset();
  bool GetCurInx(unsigned int *pInx);
  bool CheckDelay(unsigned int dwCurTime, unsigned int dwIndex);
  void IncNodeIndex();
};


