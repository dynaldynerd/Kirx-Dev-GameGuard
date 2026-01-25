#include "pch.h"

#include "CExchangeEvent.h"

CExchangeEvent *CExchangeEvent::Instance()
{
  static CExchangeEvent s_instance;
  return &s_instance;
}

bool CExchangeEvent::Initialzie()
{
  return true;
}
