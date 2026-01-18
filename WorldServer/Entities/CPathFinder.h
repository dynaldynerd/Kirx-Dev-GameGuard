#pragma once

#include "IdaCompat.h"

/* 1417 */
struct __cppobj CPathFinder
{
  int mMaxDepth;
  _PATH_NODE mPathNode[12];
  float mStart[3];
  _PATH_NODE mStackPathNode[20];
  int mStackPoint;
};
