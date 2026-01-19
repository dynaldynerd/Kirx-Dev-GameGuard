#pragma once

#include "IdaCompat.h"

#include "CUnmannedTraderRequestLimiter.h"
#include "CRaceBuffHolyQuestResultInfo.h"
#include "CRaceBuffInfoByHolyQuestList.h"

class __cppobj CRaceBuffByHolyQuestProcedure
{
public:
  using REQUEST_TYPE = CUnmannedTraderRequestLimiter::REQUEST_TYPE;

  REQUEST_TYPE m_eState;
  unsigned int m_uiProccessIndex;
  CRaceBuffHolyQuestResultInfo m_kBuffHolyQestResultInfo;
  CRaceBuffInfoByHolyQuestList m_kBuffInfo;
};
