#include "pch.h"
#include "ZoneAccountPacket.h"

int _character_disconnect_result_wrac::size() const
{
  return 3;
}

int _enter_lobby_report_wrac::size() const
{
  return 8;
}

int _enter_world_request_wrac::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(18LL);
}

int _select_avator_report_wrac::size() const
{
  return 30;
}

int _start_world_request_wrac::size() const
{
  return 6;
}

int _trans_account_report_wrac::size() const
{
  return 9;
}
