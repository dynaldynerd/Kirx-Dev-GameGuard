#pragma once

#include "IdaCompat.h"

struct  _qry_case_request_refund
{
  unsigned __int8 byRace;
  unsigned __int16 wIndex;
  unsigned int dwAvatorSerial;
  unsigned __int64 dwRefund;

  _qry_case_request_refund();
  _qry_case_request_refund(
    unsigned __int8 byR,
    unsigned __int16 wIdx,
    unsigned int dwS,
    unsigned __int64 dwRef);
  int size() const;
};

inline _qry_case_request_refund::_qry_case_request_refund()
{
  std::memset(this, 0, sizeof(*this));
}

inline _qry_case_request_refund::_qry_case_request_refund(
  unsigned __int8 byR,
  unsigned __int16 wIdx,
  unsigned int dwS,
  unsigned __int64 dwRef)
  : byRace(byR),
    wIndex(wIdx),
    dwAvatorSerial(dwS),
    dwRefund(dwRef)
{
}

inline int _qry_case_request_refund::size() const
{
  return 16;
}
