#include "pch.h"

#include "MOVE_LOBBY_DELAY.h"

#include "CNetProcess.h"
#include "CNetworkEX.h"
#include "GlobalObjects.h"
#include "moveout_user_result_zone.h"

_MOVE_LOBBY_DELAY::_MOVE_LOBBY_DELAY()
  : _DELAY_PROCESS(0x9E4u, 0x2710u)
{
}

_MOVE_LOBBY_DELAY::~_MOVE_LOBBY_DELAY() = default;

void _MOVE_LOBBY_DELAY::Process(unsigned int dwIndex, unsigned int dwSerial)
{
  CUserDB *user = &g_UserDB[dwIndex];
  if (user->m_idWorld.dwSerial == dwSerial)
  {
    _moveout_user_result_zone msg{};
    msg.byRetCode = 0;
    unsigned __int8 type[2] = {1, 7};
    const unsigned __int16 len = msg.size();
    g_Network.m_pProcess[0]->LoadSendMsg(dwIndex, type, reinterpret_cast<char *>(&msg), len);
  }
}
