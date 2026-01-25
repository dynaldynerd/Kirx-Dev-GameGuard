#include "pch.h"

#include "CRaceBossMsgController.h"

CRaceBossMsgController *CRaceBossMsgController::Instance()
{
  static CRaceBossMsgController s_instance;
  return &s_instance;
}

void CRaceBossMsgController::Init()
{
}
