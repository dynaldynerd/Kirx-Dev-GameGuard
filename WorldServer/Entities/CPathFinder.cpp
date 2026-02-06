#include "pch.h"

#include "CPathFinder.h"
#include "R3EngineGlobals.h"

CPathFinder::CPathFinder()
{
  memset_0(this, 0, sizeof(CPathFinder));
}

__int64 CPathFinder::AddPath(float *const a2, int a3)
{
  _PATH_NODE &node = mPathNode[a3];
  const int pathCount = node.PathCnt;
  if (pathCount + 1 >= mMaxDepth)
  {
    return 0LL;
  }

  node.Path[pathCount][0] = a2[0];
  node.Path[pathCount][1] = a2[1];
  node.Path[pathCount][2] = a2[2];

  float dx = 0.0f;
  float dy = 0.0f;
  float dz = 0.0f;
  if (pathCount)
  {
    dx = a2[0] - node.Path[pathCount - 1][0];
    dy = a2[1] - node.Path[pathCount - 1][1];
    dz = a2[2] - node.Path[pathCount - 1][2];
  }
  else
  {
    dx = a2[0] - mStart[0];
    dy = a2[1] - mStart[1];
    dz = a2[2] - mStart[2];
  }

  node.PathCnt = pathCount + 1;
  node.TotalLeng = sqrtf_0((dx * dx) + (dy * dy) + (dz * dz)) + node.TotalLeng;
  return 1LL;
}

void CPathFinder::CompletePath(int a2)
{
  mPathNode[a2].IsFind = 1;
}

void CPathFinder::PushPathStack(int a2)
{
  static char s_pathfinder_stack_overflow[] = "pathfinder stack overflow";

  memcpy_0(
    &mStackPathNode[mStackPoint],
    &mPathNode[a2],
    sizeof(mStackPathNode[mStackPoint]));
  if (++mStackPoint >= 20)
  {
    Error(s_pathfinder_stack_overflow, byte_140883769);
  }
}

void CPathFinder::PopPathStack(int a2)
{
  if (--mStackPoint < 0)
  {
    Error(aPathfinderStac, byte_140883769);
  }
  memcpy_0(&mPathNode[a2], &mStackPathNode[mStackPoint], sizeof(mPathNode[a2]));
}

__int64 CPathFinder::GetPathCnt(int a2)
{
  return static_cast<unsigned int>(mPathNode[a2].PathCnt);
}

unsigned __int16 CPathFinder::GetFrontLineId(int a2)
{
  return mPathNode[a2].FrontLinetId;
}

unsigned __int16 CPathFinder::GetBackLineId(int a2)
{
  return mPathNode[a2].BackLineId;
}

__int64 CPathFinder::GetPathDirection(int a2)
{
  return static_cast<unsigned int>(mPathNode[a2].WhatDirection);
}

void CPathFinder::SetFrontLineId(int a2, unsigned __int16 a3)
{
  mPathNode[a2].FrontLinetId = a3;
}

void CPathFinder::SetBackLineId(int a2, unsigned __int16 a3)
{
  mPathNode[a2].BackLineId = a3;
}

void CPathFinder::SetPathDirection(int a2, int a3)
{
  mPathNode[a2].WhatDirection = a3;
}

