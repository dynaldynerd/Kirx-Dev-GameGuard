#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _ore_cutting_result_zocl
{
  struct _list
  {
    unsigned __int16 wResIndex;
    unsigned __int8 byAddAmount;
  };

  unsigned __int8 byErrCode;
  unsigned __int8 byLeftNum;
  unsigned int dwLeftDalant;
  unsigned int dwConsumDalant;
  unsigned __int8 byCuttingNum;
  _list ResList[100];

  _ore_cutting_result_zocl();
  unsigned __int16 size();
};
#pragma pack(pop)

inline _ore_cutting_result_zocl::_ore_cutting_result_zocl()
{
  byCuttingNum = 0;
}

inline unsigned __int16 _ore_cutting_result_zocl::size()
{
  if (byCuttingNum > 0x64u)
  {
    byCuttingNum = 0;
  }
  return static_cast<unsigned __int16>(311 - 3LL * (100 - byCuttingNum));
}
