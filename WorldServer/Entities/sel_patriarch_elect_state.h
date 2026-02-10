#pragma once

#include "IdaCompat.h"

struct __cppobj _sel_patriarch_elect_state
{
  char szWorldName[33];
  unsigned int dwSerial;
  unsigned __int8 byProcType;
  unsigned int dwVoteCnt[3];
  unsigned int dwNonVoteCnt[3];
  unsigned int dwHighGradeNum[3];
  unsigned int dwResetServerToken;

  _sel_patriarch_elect_state();
};

inline _sel_patriarch_elect_state::_sel_patriarch_elect_state()
{
  memset_0(this, 0, sizeof(*this));
}
