#include "pch.h"

#include "param_cash.h"

_param_cash::_param_cash(unsigned int dwAccount, unsigned int dwAvator, unsigned __int16 wSock)
{
  in_dwAccountSerial = dwAccount;
  in_dwAvatorSerial = dwAvator;
  in_wSockIndex = wSock;
  in_bAdjustDiscount = false;
  in_bOneN_One = false;
  in_bSetDiscount = false;
  in_bLimited_Sale = false;
}

_param_cash::~_param_cash()
{
  // this is not a stub
}

_param_cash_select::_param_cash_select(unsigned int dwAc, unsigned int dwAv, unsigned __int16 wSock)
  : _param_cash(dwAc, dwAv, wSock)
{
}

_param_cash_select::~_param_cash_select()
{
  // this is not a stub
}

int _param_cash_select::size()
{
  return 44;
}

int _param_cash_total_selling::size()
{
  return 4;
}

_param_cash_update::__item::__item()
{
  std::memset(this, 0, sizeof(_param_cash_update::__item));
}

_param_cash_update::_param_cash_update(unsigned int dwAc, unsigned int dwAv, unsigned __int16 wSock)
  : _param_cash(dwAc, dwAv, wSock)
{
  in_nNum10 = 0;
  out_nCashAmount = 0;
  for (int i = 0; i < 20; ++i)
  {
    new (&in_item[i]) __item();
  }
}

_param_cash_update::~_param_cash_update()
{
  // this is not a stub
}

int _param_cash_update::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(1392 - (static_cast<__int64>(20 - in_nNum10) << 6));
}

_param_cash_rollback::_param_cash_rollback(unsigned int dwAc, unsigned int dwAv, unsigned __int16 wSock)
  : _param_cash(dwAc, dwAv, wSock)
{
  in_byNum = 0;
}

_param_cash_rollback::~_param_cash_rollback()
{
  // this is not a stub
}

int _param_cash_rollback::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(1072 - 48LL * (20 - in_byNum));
}

_param_cashitem_dblog::_param_cashitem_dblog(unsigned int dwAv)
  : _param_cash(0xFFFFFFFFu, dwAv, 0xFFFFu)
{
  nBuyNum = 0;
  std::memset(data, 0, sizeof(data));
}

_param_cashitem_dblog::~_param_cashitem_dblog()
{
  // this is not a stub
}

int _param_cashitem_dblog::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(356 - 16LL * (20 - nBuyNum));
}
