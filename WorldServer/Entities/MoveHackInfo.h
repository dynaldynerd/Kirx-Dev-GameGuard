#pragma once

#include "IdaCompat.h"

struct MoveHackInfo
{
  unsigned int m_dwLastMoveTime;
  unsigned int m_dwWarningEndTime;
  float m_fLastSpeed;
  int m_nCountMove;
  int m_nCountWarning;
};
