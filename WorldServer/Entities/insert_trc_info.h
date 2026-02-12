#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _insert_trc_info
{
  unsigned __int8 byRace;
  unsigned int dwGSerial;
  char szGuildName[17];
  unsigned __int8 byMatterType;
  unsigned int dwMatterDst;
  char wszMatterDst[64];
  unsigned int dwNext;
  unsigned __int8 byCurrTax;

  __int64 size() const;
};
#pragma pack(pop)

inline __int64 _insert_trc_info::size() const
{
  return 96;
}
