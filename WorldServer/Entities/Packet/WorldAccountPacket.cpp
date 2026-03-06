#include "pch.h"
#include "ZoneAccountPacket.h"

unsigned __int16 _character_disconnect_result_wrac::size() const
{
  return 3;
}

unsigned __int16 _enter_lobby_report_wrac::size() const
{
  return 8;
}

__int64 _enter_world_request_wrac::size() const
{
  return 18LL;
}

__int64 _select_avator_report_wrac::size() const
{
  return 30;
}

unsigned __int16 _start_world_request_wrac::size() const
{
  return 6;
}

unsigned __int16 _trans_account_report_wrac::size() const
{
  return 9;
}
