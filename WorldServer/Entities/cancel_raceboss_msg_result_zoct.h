#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _cancel_raceboss_msg_result_zoct
{
  unsigned __int8 byRet;
  unsigned __int8 byRaceCode;
  int nID;

  int size() const;
};
#pragma pack(pop)

inline int _cancel_raceboss_msg_result_zoct::size() const
{
  return 6;
}
