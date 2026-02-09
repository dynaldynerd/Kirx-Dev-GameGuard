#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _qry_sheet_delete
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byRaceCode;
  unsigned int dwAvatorSerial;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _qry_sheet_delete::size() const
{
  return 8LL;
}
