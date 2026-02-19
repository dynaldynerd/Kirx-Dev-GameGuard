#pragma once

#include "IdaCompat.h"

class CMapData;

/* 1472 */
struct _region_data
{
  char szRegionData[32];
  CMapData *pMap;
  unsigned __int16 wDummyLineIndex;
};

