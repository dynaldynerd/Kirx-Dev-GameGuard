#include "pch.h"

#include "CReturnGateController.h"

CReturnGateController *CReturnGateController::Instance()
{
  static CReturnGateController s_instance;
  return &s_instance;
}

bool CReturnGateController::Init(unsigned int gateCount)
{
  m_uiGateTotCnt = gateCount;
  return true;
}
