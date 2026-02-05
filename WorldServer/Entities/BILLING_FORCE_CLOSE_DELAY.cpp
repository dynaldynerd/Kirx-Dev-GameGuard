#include "pch.h"

#include "BILLING_FORCE_CLOSE_DELAY.h"
#include "CUserDB.h"
#include "GlobalObjects.h"

_BILLING_FORCE_CLOSE_DELAY::_BILLING_FORCE_CLOSE_DELAY() = default;

_BILLING_FORCE_CLOSE_DELAY::~_BILLING_FORCE_CLOSE_DELAY() = default;

void _BILLING_FORCE_CLOSE_DELAY::Process(unsigned int dwIndex, unsigned int dwSerial)
{
  CPlayer *player = &g_Player[dwIndex];
  if (player->m_bLive && player->m_dwObjSerial == dwSerial)
  {
    player->m_pUserDB->ForceCloseCommand(6u, 0, 1, "Billing");
  }
}
