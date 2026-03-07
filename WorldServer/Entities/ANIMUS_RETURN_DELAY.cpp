#include "pch.h"

#include "ANIMUS_RETURN_DELAY.h"

#include "CPlayer.h"
#include "GlobalObjects.h"

_ANIMUS_RETURN_DELAY::_ANIMUS_RETURN_DELAY()
  : _DELAY_PROCESS(2532u, 5000u)
{
}

_ANIMUS_RETURN_DELAY::~_ANIMUS_RETURN_DELAY() = default;

void _ANIMUS_RETURN_DELAY::Process(unsigned int dwIndex, unsigned int dwSerial)
{
  CPlayer *player = &g_Player[dwIndex];
  if (player->m_bLive && player->m_dwObjSerial == dwSerial)
  {
    player->_AnimusReturn(2u);
  }
}
