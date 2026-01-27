#pragma once

#include "IdaCompat.h"

/* 1416 */
struct _PATH_NODE
{
  float TotalLeng;
  float Path[17][3];
  int WhatDirection;
  unsigned short FrontLinetId;
  unsigned short BackLineId;
  int PathCnt;
  int IsFind;
};

/* 1417 */
class __cppobj CPathFinder
{
  int mMaxDepth;
  _PATH_NODE mPathNode[12];
  float mStart[3];
  _PATH_NODE mStackPathNode[20];
  int mStackPoint;
};

