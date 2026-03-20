#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct  _raceboss_msg_confirm_zowb
{
  int nCountIndex;
  int nWorldCode;
  unsigned __int8 byRaceCode;
  char wszMasterName[17];
  char wszMsg[255];

  int size() const;
};
#pragma pack(pop)

inline int _raceboss_msg_confirm_zowb::size() const
{
  return 281;
}
