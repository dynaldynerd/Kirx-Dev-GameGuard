#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CReturnGate;
class CPlayer;

class __cppobj CReturnGateController
{
public:
  CMyTimer *m_pkTimer;
  CNetIndexList *m_pkEmptyInxList;
  CNetIndexList *m_pkUseInxList;
  unsigned int m_uiGateTotCnt;
  CReturnGate **m_ppkGatePool;

  static CReturnGateController *Instance();
  bool Init(unsigned int gateCount);
  CReturnGate *GetGate(unsigned int uiInx);
  void Close(CReturnGate *pkGate);
  int ProcessEnter(unsigned int uiGateInx, CPlayer *pkObj);
  void SendEnterResult(char iResult, CPlayer *pkObj);
  char Enter(unsigned int uiGateInx, CPlayer *pkObj);
};
