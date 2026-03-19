#include "pch.h"

#include "wrac_packets.h"

int _open_world_request_wrac::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned __int16>(sizeof(_open_world_request_wrac)));
}

int _cashdb_setting_request_wrac::size() const
{
  return 0;
}

int _uilock_user_refresh_info_request_wrac::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned __int16>(sizeof(_uilock_user_refresh_info_request_wrac)));
}

int _logout_account_request_wrac::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(static_cast<unsigned __int16>(sizeof(_logout_account_request_wrac)));
}
