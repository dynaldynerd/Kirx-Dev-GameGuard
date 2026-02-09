#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct __cppobj __declspec(align(2)) _qry_case_alive_char
{
  unsigned __int8 in_byCase;
  unsigned int in_dwSerial;
  char in_w_szName[17];
  unsigned __int8 in_bySlot;
  _REGED out_AliveAvator;

  _qry_case_alive_char();
  ~_qry_case_alive_char();
  __int64 size() const;
};
#pragma pack(pop)

inline _qry_case_alive_char::_qry_case_alive_char() = default;
inline _qry_case_alive_char::~_qry_case_alive_char() = default;

inline __int64 _qry_case_alive_char::size() const
{
  return 296LL;
}
