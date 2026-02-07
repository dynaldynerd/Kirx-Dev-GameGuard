#include "pch.h"

#include "wrac_packets.h"

unsigned __int16 _open_world_request_wrac::size() const
{
  return static_cast<unsigned __int16>(sizeof(_open_world_request_wrac));
}

unsigned __int16 _cashdb_setting_request_wrac::size() const
{
  return 0;
}

unsigned __int16 _uilock_user_refresh_info_request_wrac::size() const
{
  return static_cast<unsigned __int16>(sizeof(_uilock_user_refresh_info_request_wrac));
}

unsigned __int16 _logout_account_request_wrac::size() const
{
  return static_cast<unsigned __int16>(sizeof(_logout_account_request_wrac));
}
