#pragma once

#include "IdaCompat.h"

class __cppobj CUnmannedTraderRequestLimiter
{
public:
  enum class REQUEST_TYPE : int
  {
    RT_WAIT = 0,
    RT_CANCEL_RACE_BUFF = 1,
    RT_SET_RACE_BATTLE_RESULT = 2,
    RT_SET_RACE_BUFF = 3,
    RT_TYPE_MAX = 4,
  };

  CUnmannedTraderRequestLimiter();
  void ClearRequset();
  bool IsEmpty();
  void SetRequest(REQUEST_TYPE iRequest);

  REQUEST_TYPE m_eState;
};
