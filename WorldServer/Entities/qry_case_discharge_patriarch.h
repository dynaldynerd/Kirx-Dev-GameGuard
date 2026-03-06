#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _qry_case_discharge_patriarch
{
  unsigned __int8 byRace;
  unsigned int dwAvatorSerial;
  unsigned int dwElectSerial;

  _qry_case_discharge_patriarch() = default;
  _qry_case_discharge_patriarch(
    unsigned __int8 race,
    unsigned int avatorSerial,
    unsigned int electSerial)
    : byRace(race), dwAvatorSerial(avatorSerial), dwElectSerial(electSerial)
  {
  }

  int size() const
  {
    return sizeof(*this);
  }
};
#pragma pack(pop)

static_assert(sizeof(_qry_case_discharge_patriarch) == 9);
