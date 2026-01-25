#include "pch.h"

#include "CRecallEffectController.h"

CRecallEffectController *CRecallEffectController::Instance()
{
  static CRecallEffectController s_instance;
  return &s_instance;
}

bool CRecallEffectController::Init(unsigned int infoCount)
{
  m_uiInfoTotCnt = infoCount;
  return true;
}
