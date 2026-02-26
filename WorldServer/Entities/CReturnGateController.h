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
  CReturnGate *GetEmpty();
  bool IsExistOwner(CPlayer *pkObj);
  CReturnGate *GetGate(unsigned int uiInx);
  bool Open(CPlayer *pkOwner);
  void Close(CReturnGate *pkGate);
  int ProcessEnter(unsigned int uiGateInx, CPlayer *pkObj);
  void SendEnterResult(char iResult, CPlayer *pkObj);
  char Enter(unsigned int uiGateInx, CPlayer *pkObj);
};
