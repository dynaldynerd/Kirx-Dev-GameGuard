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
class  CPathFinder
{
public:
  int mMaxDepth;
  _PATH_NODE mPathNode[12];
  float mStart[3];
  _PATH_NODE mStackPathNode[20];
  int mStackPoint;

  CPathFinder();
  __int64 AddPath(float *const a2, int a3);
  void CompletePath(int a2);
  void PushPathStack(int a2);
  void PopPathStack(int a2);
  __int64 GetPathCnt(int a2);
  unsigned __int16 GetFrontLineId(int a2);
  unsigned __int16 GetBackLineId(int a2);
  __int64 GetPathDirection(int a2);
  void SetFrontLineId(int a2, unsigned __int16 a3);
  void SetBackLineId(int a2, unsigned __int16 a3);
  void SetPathDirection(int a2, int a3);
};

