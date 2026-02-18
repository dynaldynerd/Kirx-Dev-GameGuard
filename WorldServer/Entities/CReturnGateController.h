#pragma once

#include "IdaCompat.h"
#include "CMyTimer.h"
#include "CNetIndexList.h"

class CReturnGate;
class CPlayer;

class  CReturnGateController
{
public:
  CReturnGateController();
  ~CReturnGateController();

  CMyTimer *m_pkTimer;
  CNetIndexList *m_pkEmptyInxList;
  CNetIndexList *m_pkUseInxList;
  unsigned int m_uiGateTotCnt;
  CReturnGate **m_ppkGatePool;

  void CleanUp();
  static CReturnGateController *Instance();
  bool Init(unsigned int gateCount);
  void OnLoop();
  void UpdateClose();
  CReturnGate *GetGate(unsigned int uiInx);
  void Close(CReturnGate *pkGate);
  int ProcessEnter(unsigned int uiGateInx, CPlayer *pkObj);
  void SendEnterResult(char iResult, CPlayer *pkObj);
  char Enter(unsigned int uiGateInx, CPlayer *pkObj);
};
