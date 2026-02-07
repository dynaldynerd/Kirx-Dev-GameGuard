#pragma once

#include "IdaCompat.h"

class CMapData;

struct __cppobj __declspec(align(8)) __TEMP_WAIT_TOWER
{
  unsigned int dwMasterSerial;
  unsigned __int8 byItemIndex;
  CMapData *pMap;
  float fPos[3];
  unsigned int dwPushTime;
  bool bComplete;

  __TEMP_WAIT_TOWER();
};

inline __TEMP_WAIT_TOWER::__TEMP_WAIT_TOWER()
  : dwMasterSerial(static_cast<unsigned int>(-1))
{
}
