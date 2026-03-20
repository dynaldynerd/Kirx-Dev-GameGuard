#pragma once

#include "IdaCompat.h"
#include "CMainThread.h"

#pragma pack(push, 1)
struct  _qry_case_alive_char
{
  unsigned __int8 in_byCase;
  unsigned int in_dwSerial;
  char in_w_szName[17];
  unsigned __int8 in_bySlot;
  _REGED out_AliveAvator;

  _qry_case_alive_char();
  ~_qry_case_alive_char();
  int size() const;
};
#pragma pack(pop)

inline _qry_case_alive_char::_qry_case_alive_char() = default;
inline _qry_case_alive_char::~_qry_case_alive_char() = default;

inline int _qry_case_alive_char::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(296LL);
}

