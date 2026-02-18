#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _cancel_raceboss_msg_result_zoct
{
  unsigned __int8 byRet;
  unsigned __int8 byRaceCode;
  int nID;

  unsigned __int16 size() const;
};
#pragma pack(pop)

inline unsigned __int16 _cancel_raceboss_msg_result_zoct::size() const
{
  return 6;
}
