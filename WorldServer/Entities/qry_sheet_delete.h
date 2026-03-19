#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _qry_sheet_delete
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byRaceCode;
  unsigned int dwAvatorSerial;

  int size() const;
};
#pragma pack(pop)

inline int _qry_sheet_delete::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(8LL);
}
