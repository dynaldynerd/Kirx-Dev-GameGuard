#include "pch.h"

#include "CUnmannedTraderController.h"

CUnmannedTraderController *CUnmannedTraderController::Instance()
{
  static CUnmannedTraderController s_instance;
  return &s_instance;
}

bool CUnmannedTraderController::Init()
{
  return true;
}
