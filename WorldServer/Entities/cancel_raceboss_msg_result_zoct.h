#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _cancel_raceboss_msg_result_zoct
{
  unsigned __int8 byRet;
  unsigned __int8 byRaceCode;
  int nID;

  unsigned __int16 size() const;
};

inline unsigned __int16 _cancel_raceboss_msg_result_zoct::size() const
{
  return 6;
}
