#pragma once

#include "IdaCompat.h"

struct  _qry_case_insert_candidate
{
  _qry_case_insert_candidate() = default;
  _qry_case_insert_candidate(
    unsigned __int8 byRaceIn,
    unsigned __int16 wIndexIn,
    unsigned int dwElectSerialIn,
    unsigned int dwAvatorSerialIn)
    : byRace(byRaceIn),
      wIndex(wIndexIn),
      dwElectSerial(dwElectSerialIn),
      dwAvatorSerial(dwAvatorSerialIn)
  {
  }

  int size() const
  {
    return 12;
  }

  unsigned __int8 byRace;
  unsigned __int16 wIndex;
  unsigned int dwElectSerial;
  unsigned int dwAvatorSerial;
};
