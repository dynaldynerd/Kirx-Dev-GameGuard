#include "pch.h"

#include "qry_logout.h"

_qry_case_lobby_logout::_qry_case_lobby_logout()
{
  init();
}

void _qry_case_lobby_logout::init()
{
  std::memset(this, 0, sizeof(_qry_case_lobby_logout));
  for (int j = 0; j < 3; ++j)
  {
    RegeList[j].bySlotIndex = static_cast<unsigned __int8>(-1);
    RegeList[j].dwCharSerial = static_cast<unsigned int>(-1);
  }
}

__int64 _qry_case_lobby_logout::size()
{
  return 196;
}

_qry_sheet_logout::_qry_sheet_logout() = default;

_qry_sheet_logout::~_qry_sheet_logout() = default;

__int64 _qry_sheet_logout::size()
{
  return 74444;
}
