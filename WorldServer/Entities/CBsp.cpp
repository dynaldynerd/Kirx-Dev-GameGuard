#include "pch.h"
#include "CBsp.h"
#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "CEntity.h"
#include "CParticle.h"
#include "BspReadEntities.h"
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <intrin.h>

namespace
{
    using GetVertexFromCompressFn = void (*)(float *, char *, _BSP_READ_M_GROUP *);

    void GetVertexFromBVertexWrap(float *dst, char *src, _BSP_READ_M_GROUP *mg)
    {
        GetVertexFromBVertex(dst, src, mg);
    }

    void GetVertexFromWVertexWrap(float *dst, char *src, _BSP_READ_M_GROUP *mg)
    {
        GetVertexFromWVertex(dst, reinterpret_cast<short *>(src), mg);
    }

    void GetVertexFromFVertexWrap(float *dst, char *src, _BSP_READ_M_GROUP *mg)
    {
        GetVertexFromFVertex(dst, reinterpret_cast<float *>(src), mg);
    }

    GetVertexFromCompressFn GetVertexFromCompress[5] = {
        nullptr,
        GetVertexFromBVertexWrap, // 1
        GetVertexFromWVertexWrap, // 2
        nullptr,
        GetVertexFromFVertexWrap  // 4
    };

    inline void VecSub(const float *a, const float *b, float *out)
    {
        out[0] = a[0] - b[0];
        out[1] = a[1] - b[1];
        out[2] = a[2] - b[2];
    }

    inline float VecLen(const float *v)
    {
        return sqrtf_0((v[0] * v[0]) + (v[1] * v[1]) + (v[2] * v[2]));
    }

    inline void VecNormalize(float *v)
    {
        float len = VecLen(v);
        if (len != 0.0f)
        {
            v[0] /= len;
            v[1] /= len;
            v[2] /= len;
        }
    }

    inline float VecDot(const float *a, const float *b)
    {
        return (a[0] * b[0]) + (a[1] * b[1]) + (a[2] * b[2]);
    }

    inline unsigned char LoByte(unsigned int value)
    {
        return static_cast<unsigned char>(value & 0xFFu);
    }

    inline unsigned char HiByte(unsigned int value)
    {
        return static_cast<unsigned char>((value >> 8) & 0xFFu);
    }

    inline bool CheckEdgeNoEpsilon(const float *a, const float *b, const float *v, const float *normal)
    {
        float vv[3];
        VecSub(a, b, vv);
        float n[3];
        n[0] = (normal[1] * vv[2]) - (normal[2] * vv[1]);
        n[1] = (normal[2] * vv[0]) - (normal[0] * vv[2]);
        n[2] = (normal[0] * vv[1]) - (normal[1] * vv[0]);
        float dp = (n[0] * (v[0] - b[0])) + (n[1] * (v[1] - b[1])) + (n[2] * (v[2] - b[2]));
        return dp <= 0.0f;
    }
}

void CBsp::GetFaceFrontPoint(float (*a2)[3], int a3)
{
  const int idx = 3 * a3;
  float *normal = reinterpret_cast<float *>(mCFVNormal);
  float *vertex = reinterpret_cast<float *>(mCFVertex);
  (*a2)[0] = (normal[idx] * 2.0f) + vertex[idx];
  (*a2)[1] = (normal[idx + 1] * 2.0f) + vertex[idx + 1];
  (*a2)[2] = (normal[idx + 2] * 2.0f) + vertex[idx + 2];
}

int CBsp::IsExistSelfPoint(int a2, int a3)
{
  int index = 0;
  _PATH_NODE &node = mPathFinder.mPathNode[a2];
  if (node.WhatDirection == -1)
  {
    const int startVertex = mCFLine[a3].start_v;
    const int pathCount = node.PathCnt;
    if (pathCount > 0)
    {
      for (int i = 0; i < pathCount; ++i)
      {
        const float *point = node.Path[i];
        if ((mCFVertex[startVertex][0] + 2.0f) > point[0] && point[0] > (mCFVertex[startVertex][0] - 2.0f)
          && (mCFVertex[startVertex][1] + 2.0f) > point[1] && point[1] > (mCFVertex[startVertex][1] - 2.0f)
          && (mCFVertex[startVertex][2] + 2.0f) > point[2] && point[2] > (mCFVertex[startVertex][2] - 2.0f))
        {
          return 1;
        }
        ++index;
      }
      return 0;
    }
  }
  else
  {
    const int endVertex = mCFLine[a3].end_v;
    const int pathCount = node.PathCnt;
    if (pathCount > 0)
    {
      for (int i = 0; i < pathCount; ++i)
      {
        const float *point = node.Path[i];
        if ((mCFVertex[endVertex][0] + 2.0f) > point[0] && point[0] > (mCFVertex[endVertex][0] - 2.0f)
          && (mCFVertex[endVertex][1] + 2.0f) > point[1] && point[1] > (mCFVertex[endVertex][1] - 2.0f)
          && (mCFVertex[endVertex][2] + 2.0f) > point[2] && point[2] > (mCFVertex[endVertex][2] - 2.0f))
        {
          return 1;
        }
        ++index;
      }
      return 0;
    }
  }
  return 0;
}

int CBsp::GetPath(float *const a2, float *const a3)
{
  if (mFindPathCnt >= 10)
  {
    return 0;
  }
  if (mPathFinder.GetPathCnt(mFindPathCnt) + 1 >= mPathFinder.mMaxDepth)
  {
    return 0;
  }

  float get[3]{};
  float getTemp[3]{};
  float get2[3]{};

  int type = GetPathCrossPoint(
    a2,
    a3,
    &get,
    mPathFinder.GetFrontLineId(mFindPathCnt),
    mPathFinder.GetBackLineId(mFindPathCnt));
  const int backupFace = mColFaceId;

  if (type == 1)
  {
    if (IsExistSelfPoint(mFindPathCnt, backupFace))
    {
      type = 2;
    }
  }

  switch (type)
  {
  case 1:
    mPathFinder.AddPath(get, mFindPathCnt);
    mPathFinder.PushPathStack(mFindPathCnt);
    mPathFinder.SetFrontLineId(mFindPathCnt, backupFace);
    mPathFinder.SetBackLineId(mFindPathCnt, mCFLine[backupFace].back);
    mPathFinder.SetPathDirection(mFindPathCnt, -1);
    GetFaceFrontPoint(&getTemp, mCFLine[backupFace].start_v);
    getTemp[1] = a2[1];

    type = GetPathCrossPoint(
      get,
      getTemp,
      &get2,
      mPathFinder.GetFrontLineId(mFindPathCnt),
      mPathFinder.GetBackLineId(mFindPathCnt));
    if (type && mColFaceId != -1)
    {
      mPathFinder.AddPath(get2, mFindPathCnt);
      mPathFinder.SetFrontLineId(mFindPathCnt, mColFaceId);
      mPathFinder.SetBackLineId(mFindPathCnt, mCFLine[mColFaceId].back);
      GetFaceFrontPoint(&getTemp, mCFLine[mColFaceId].start_v);
      getTemp[1] = a2[1];
    }
    mPathFinder.AddPath(getTemp, mFindPathCnt);
    GetPath(getTemp, a3);

    ++mFindPathCnt;
    mPathFinder.PopPathStack(mFindPathCnt);
    if (mFindPathCnt >= 10)
    {
      --mFindPathCnt;
      return 0;
    }

    mPathFinder.SetFrontLineId(mFindPathCnt, mCFLine[backupFace].front);
    mPathFinder.SetBackLineId(mFindPathCnt, backupFace);
    mPathFinder.SetPathDirection(mFindPathCnt, 1);
    GetFaceFrontPoint(&getTemp, mCFLine[backupFace].end_v);
    getTemp[1] = a2[1];

    type = GetPathCrossPoint(
      get,
      getTemp,
      &get2,
      mPathFinder.GetFrontLineId(mFindPathCnt),
      mPathFinder.GetBackLineId(mFindPathCnt));
    if (type && mColFaceId != -1)
    {
      mPathFinder.AddPath(get2, mFindPathCnt);
      mPathFinder.SetBackLineId(mFindPathCnt, mColFaceId);
      mPathFinder.SetFrontLineId(mFindPathCnt, mCFLine[mColFaceId].front);
      GetFaceFrontPoint(&getTemp, mCFLine[mColFaceId].end_v);
      getTemp[1] = a2[1];
    }
    mPathFinder.AddPath(getTemp, mFindPathCnt);
    GetPath(getTemp, a3);
    break;
  case 2:
    if (mPathFinder.GetPathDirection(mFindPathCnt) == -1)
    {
      const int backLine = mPathFinder.GetBackLineId(mFindPathCnt);
      mPathFinder.SetFrontLineId(mFindPathCnt, backLine);
      mPathFinder.SetBackLineId(mFindPathCnt, mCFLine[backLine].back);
      GetFaceFrontPoint(&getTemp, mCFLine[backLine].start_v);
      getTemp[1] = a2[1];

      type = GetPathCrossPoint(
        a2,
        getTemp,
        &get,
        mPathFinder.GetFrontLineId(mFindPathCnt),
        mPathFinder.GetBackLineId(mFindPathCnt));
      if (type && mColFaceId != -1)
      {
        mPathFinder.AddPath(get, mFindPathCnt);
        mPathFinder.SetFrontLineId(mFindPathCnt, mColFaceId);
        mPathFinder.SetBackLineId(mFindPathCnt, mCFLine[mColFaceId].back);
        GetFaceFrontPoint(&getTemp, mCFLine[mColFaceId].start_v);
        getTemp[1] = a2[1];
      }
      mPathFinder.AddPath(getTemp, mFindPathCnt);
      GetPath(getTemp, a3);
    }
    else
    {
      const int backLine = mPathFinder.GetFrontLineId(mFindPathCnt);
      mPathFinder.SetBackLineId(mFindPathCnt, backLine);
      mPathFinder.SetFrontLineId(mFindPathCnt, mCFLine[backLine].front);
      GetFaceFrontPoint(&getTemp, mCFLine[backLine].end_v);
      getTemp[1] = a2[1];

      type = GetPathCrossPoint(
        a2,
        getTemp,
        &get,
        mPathFinder.GetFrontLineId(mFindPathCnt),
        mPathFinder.GetBackLineId(mFindPathCnt));
      if (type && mColFaceId != -1)
      {
        mPathFinder.AddPath(get, mFindPathCnt);
        mPathFinder.SetBackLineId(mFindPathCnt, mColFaceId);
        mPathFinder.SetFrontLineId(mFindPathCnt, mCFLine[mColFaceId].front);
        GetFaceFrontPoint(&getTemp, mCFLine[mColFaceId].end_v);
        getTemp[1] = a2[1];
      }
      mPathFinder.AddPath(getTemp, mFindPathCnt);
      GetPath(getTemp, a3);
    }
    break;
  case 0:
    if (mPathFinder.AddPath(a3, mFindPathCnt))
    {
      mPathFinder.CompletePath(mFindPathCnt);
    }
    break;
  case 3:
    if (mPathFinder.AddPath(get, mFindPathCnt))
    {
      mPathFinder.CompletePath(mFindPathCnt);
    }
    break;
  default:
    break;
  }

  return 0;
}

int CBsp::GetFinalPath(float *a2, float *const a3, float (*const a4)[3])
{
  _PATH_NODE *node = reinterpret_cast<_PATH_NODE *>(a2);
  float temp[3]{};
  if (node->PathCnt == 1)
  {
    (*a4)[0] = node->Path[0][0];
    (*a4)[1] = node->Path[0][1];
    (*a4)[2] = node->Path[0][2];
    return 1;
  }

  int index[16]{};
  int indexCount = 0;
  int id = -1;

  index[indexCount++] = node->PathCnt - 1;
  for (int i = node->PathCnt - 2; i > 0; --i)
  {
    id = -2;
    if (!GetPathCrossPoint(a3, node->Path[i], &temp, 0, 0))
    {
      id = -1;
      index[indexCount++] = i;
      break;
    }
    for (int j = i - 2; j > 0; --j)
    {
      if (!GetPathCrossPoint(node->Path[j], node->Path[i], &temp, 0, 0))
      {
        id = j;
      }
    }
    if (id == -2)
    {
      index[indexCount++] = i;
      continue;
    }

    index[indexCount++] = i;
    index[indexCount++] = id;
    i = id + 1;
  }
  if (id != -1)
  {
    index[indexCount++] = 0;
  }

  for (int i = 0; i < indexCount; ++i)
  {
    const int pointIndex = index[indexCount - i - 1];
    a4[i][0] = node->Path[pointIndex][0];
    a4[i][1] = node->Path[pointIndex][1];
    a4[i][2] = node->Path[pointIndex][2];
  }
  return indexCount;
}

int CBsp::GetPathFind(float *const a2, float *const a3, float (*const a4)[3], unsigned int *a5, int a6)
{
  float minDist = FLOAT_1000000_0;
  int minIndex = -1;
  bool tooLong = false;

  float target[3] = {a3[0], a2[1], a3[2]};
  float tempAt[3] = {0.0f, 0.0f, 0.0f};
  float tempVec[3] = {target[0] - a2[0], target[1] - a2[1], target[2] - a2[2]};

  const float dist = sqrtf_0((tempVec[0] * tempVec[0]) + (tempVec[1] * tempVec[1]) + (tempVec[2] * tempVec[2]));
  if (dist > 200.0f)
  {
    tooLong = true;
    tempAt[0] = target[0];
    tempAt[1] = target[1];
    tempAt[2] = target[2];
    Normalize(tempVec);
    tempVec[0] *= 200.0f;
    tempVec[1] *= 200.0f;
    tempVec[2] *= 200.0f;
    target[0] = a2[0] + tempVec[0];
    target[1] = a2[1] + tempVec[1];
    target[2] = a2[2] + tempVec[2];
    Normalize(tempVec);
    tempVec[0] *= 210.0f;
    tempVec[1] *= 210.0f;
    tempVec[2] *= 210.0f;
    tempAt[0] = a2[0] + tempVec[0];
    tempAt[1] = a2[1];
    tempAt[2] = a2[2] + tempVec[2];
  }

  memset_0(mPathFinder.mPathNode, 0, sizeof(_PATH_NODE) * 10);
  mFindPathCnt = 0;
  mPathFinder.mStackPoint = 0;
  mPathFinder.mStart[0] = a2[0];
  mPathFinder.mStart[1] = a2[1];
  mPathFinder.mStart[2] = a2[2];
  mPathFinder.mMaxDepth = a6;
  GetPath(a2, target);
  ++mFindPathCnt;

  for (unsigned int i = 0; i < mFindPathCnt; ++i)
  {
    if (!mPathFinder.mPathNode[i].IsFind)
    {
      continue;
    }
    if (minDist > mPathFinder.mPathNode[i].TotalLeng)
    {
      minDist = mPathFinder.mPathNode[i].TotalLeng;
      minIndex = static_cast<int>(i);
    }
  }

  if (minIndex == -1)
  {
    float canGo[3]{};
    if (GetPathCrossPoint(a2, target, &canGo, 0, 0))
    {
      (*a4)[0] = canGo[0];
      (*a4)[1] = canGo[1];
      (*a4)[2] = canGo[2];
    }
    else
    {
      (*a4)[0] = target[0];
      (*a4)[1] = target[1];
      (*a4)[2] = target[2];
    }
    *a5 = 1;
    return 0;
  }

  const unsigned int finalPath = static_cast<unsigned int>(
    GetFinalPath(reinterpret_cast<float *>(&mPathFinder.mPathNode[minIndex].TotalLeng), a2, a4));
  *a5 = finalPath;
  if (tooLong && finalPath == 1 && target[0] == (*a4)[0] && target[1] == (*a4)[1] && target[2] == (*a4)[2])
  {
    *a5 = 2;
    (*a4)[3] = tempAt[0];
    (*a4)[4] = tempAt[1];
    (*a4)[5] = tempAt[2];
    return 2;
  }
  return 1;
}

float CBsp::GetFirstYpos(float *const a2, float *const a3, float *const a4)
{
    __int16 v5[4] = {};
    __int16 v6[4] = {};

    v6[0] = static_cast<int>(*a3);
    v6[1] = static_cast<int>(a3[1]);
    v6[2] = static_cast<int>(a3[2]);
    v5[0] = static_cast<int>(*a4);
    v5[1] = static_cast<int>(a4[1]);
    v5[2] = static_cast<int>(a4[2]);
    return GetFirstYpos(a2, v6, v5);
}

float CBsp::GetFirstYpos(float *const a2, int a3)
{
    float best;
    if (a3 == -1)
        best = 32000.0f;
    else
        best = -32000.0f;

    float start[3] = {a2[0], 30000.0f, a2[2]};
    float end[3] = {a2[0], -30000.0f, a2[2]};
    __int16 leafList[32000];
    int leafCount = 0;

    GetLeafList(start, end, &leafCount, leafList, 0x7D00u);

    if (leafCount > 0)
    {
        for (int i = 0; i < leafCount; ++i)
        {
            _BSP_LEAF &leaf = mLeaf[leafList[i]];
            if (!leaf.face_num)
                continue;

            for (unsigned int j = 0; j < leaf.face_num; ++j)
            {
                unsigned int faceIdx = mCFaceId[leaf.face_start_id + j];
                _BSP_C_FACE &face = mCFace[faceIdx];
                if (face.Attr & 0x40)
                    continue;

                float cross[3];
                if (!GetPlaneCrossPoint(start, end, cross, face.Normal, face.Normal[3]))
                    continue;

                int inside = 0;
                int vNum = face.VNum;
                unsigned int vStart = face.VStartId;
                if (vNum)
                {
                    for (int k = 0; k < vNum; ++k)
                    {
                        unsigned int v0 = mCVertexId[vStart + k];
                        unsigned int v1 = mCVertexId[vStart + (k + 1) % vNum];
                        if (CheckEdgeNoEpsilon(mCVertex[v0], mCVertex[v1], cross, face.Normal))
                            ++inside;
                    }
                }

                if (inside == vNum)
                {
                    float y = cross[1];
                    if (a3 == -1)
                    {
                        if (best > y)
                        {
                            best = y;
                            mNowCFaceId = static_cast<int>(faceIdx);
                        }
                    }
                    else
                    {
                        if (y > best)
                        {
                            best = y;
                            mNowCFaceId = static_cast<int>(faceIdx);
                        }
                    }
                }
            }
        }
    }

    if (std::fabs(best) == 32000.0f)
        return 65535.0f;
    return best;
}

float CBsp::GetFirstYpos(float *const a2, __int16 *const a3, __int16 *const a4)
{
    float best = -65535.0f;
    float start[3] = {a2[0], static_cast<float>(a4[1]), a2[2]};
    float end[3] = {a2[0], static_cast<float>(a3[1]), a2[2]};
    __int16 leafList[32000];
    int leafCount = 0;

    GetLeafList(start, end, &leafCount, leafList, 0x7D00u);
    if (leafCount > 0)
    {
        for (int i = 0; i < leafCount; ++i)
        {
            _BSP_LEAF &leaf = mLeaf[leafList[i]];
            if (!leaf.face_num)
                continue;

            for (unsigned int j = 0; j < leaf.face_num; ++j)
            {
                unsigned int faceIdx = mCFaceId[leaf.face_start_id + j];
                _BSP_C_FACE &face = mCFace[faceIdx];
                if (face.Attr & 0x40)
                    continue;

                float cross[3];
                if (!GetPlaneCrossPoint(start, end, cross, face.Normal, face.Normal[3]))
                    continue;

                int inside = 0;
                int vNum = face.VNum;
                unsigned int vStart = face.VStartId;
                if (vNum)
                {
                    for (int k = 0; k < vNum; ++k)
                    {
                        unsigned int v0 = mCVertexId[vStart + k];
                        unsigned int v1 = mCVertexId[vStart + (k + 1) % vNum];
                        if (CheckEdgeNoEpsilon(mCVertex[v0], mCVertex[v1], cross, face.Normal))
                            ++inside;
                    }
                }

                if (inside == vNum)
                {
                    float y = cross[1];
                    if (start[1] > y && y > end[1] && y > best)
                    {
                        best = y;
                        mNowCFaceId = static_cast<int>(faceIdx);
                    }
                }
            }
        }
    }
    return best;
}

__int16 CBsp::GetLeafNum(float *const a2)
{
    mTempCamera[0] = a2[0];
    mTempCamera[1] = a2[1];
    mTempCamera[2] = a2[2];
    mTempSearchOk = -1;
    SearchNode(1);
    return mTempSearchOk;
}

void CBsp::SearchNode(__int16 a2)
{
    if (!a2)
    {
        return;
    }

    _BSP_NODE *nodes = mNode;
    const float side = DotProduct(mCNNormal[nodes[a2].f_normal_id], mTempCamera) - nodes[a2].d;
    if (side < 0.0f)
    {
        const __int16 back = nodes[a2].back;
        if (back >= 0)
        {
            SearchNode(back);
        }
        else
        {
            mTempSearchOk = static_cast<__int16>(-1 - back);
        }

        if (mTempSearchOk <= 0)
        {
            const __int16 front = nodes[a2].front;
            if (front >= 0)
            {
                SearchNode(front);
            }
            else
            {
                mTempSearchOk = static_cast<__int16>(-1 - front);
            }
        }
    }
    else
    {
        const __int16 front = nodes[a2].front;
        if (front >= 0)
        {
            SearchNode(front);
        }
        else
        {
            mTempSearchOk = static_cast<__int16>(-1 - front);
        }

        if (mTempSearchOk <= 0)
        {
            const __int16 back = nodes[a2].back;
            if (back >= 0)
            {
                SearchNode(back);
            }
            else
            {
                mTempSearchOk = static_cast<__int16>(-1 - back);
            }
        }
    }
}

float CBsp::GetYposInLeaf(float *const a2, float *const a3, float a4, float a5, int a6)
{
    const _BSP_LEAF &leaf = mLeaf[a6];
    float bestY = FLOAT_N32000_0;
    if (!leaf.face_num)
    {
        return bestY;
    }

    unsigned int *faceIds = &mCFaceId[leaf.face_start_id];
    for (unsigned int i = 0; i < leaf.face_num; ++i)
    {
        const unsigned int faceIndex = faceIds[i];
        _BSP_C_FACE &face = mCFace[faceIndex];
        if (face.Attr & 0x40)
        {
            continue;
        }

        float cross[4];
        if (!GetPlaneCrossPoint(a2, a3, cross, face.Normal, face.Normal[3]))
        {
            continue;
        }

        int insideCount = 0;
        const int vNum = face.VNum;
        const unsigned int vStart = face.VStartId;
        if (vNum > 0)
        {
            for (int j = 0; j < vNum; ++j)
            {
                const unsigned int v0 = mCVertexId[vStart + j];
                const unsigned int v1 = mCVertexId[vStart + (j + 1) % vNum];
                const float *vertex0 = mCVertex[v0];
                const float *vertex1 = mCVertex[v1];
                float edge[3] = {vertex0[0] - vertex1[0], vertex0[1] - vertex1[1], vertex0[2] - vertex1[2]};
                float edgeNormal[4];
                sub_1404E2FB0(face.Normal, edge, edgeNormal);
                const float dx = cross[0] - vertex1[0];
                const float dy = cross[1] - vertex1[1];
                const float dz = cross[2] - vertex1[2];
                if ((dx * edgeNormal[0] + dy * edgeNormal[1] + dz * edgeNormal[2]) <= 0.0f)
                {
                    ++insideCount;
                }
            }
        }

        if (insideCount == vNum)
        {
            const float y = cross[1];
            if (y > bestY && y < (a4 + a5) && y > (a5 - a4))
            {
                bestY = y;
                mNowCFaceId = static_cast<int>(faceIndex);
            }
        }
    }

    return bestY;
}

float CBsp::GetBestYposInLeaf(float *const a2, float *const a3, float a4, float a5, int a6)
{
    const _BSP_LEAF &leaf = mLeaf[a6];
    float bestY = FLOAT_N32000_0;
    unsigned int bestLevel = 10000;
    if (!leaf.face_num)
    {
        return bestY;
    }

    unsigned int *faceIds = &mCFaceId[leaf.face_start_id];
    for (unsigned int i = 0; i < leaf.face_num; ++i)
    {
        const unsigned int faceIndex = faceIds[i];
        _BSP_C_FACE &face = mCFace[faceIndex];
        if (face.Attr & 0x40)
        {
            continue;
        }

        float cross[4];
        if (!GetPlaneCrossPoint(a2, a3, cross, face.Normal, face.Normal[3]))
        {
            continue;
        }

        int insideCount = 0;
        const int vNum = face.VNum;
        const unsigned int vStart = face.VStartId;
        if (vNum > 0)
        {
            for (int j = 0; j < vNum; ++j)
            {
                const unsigned int v0 = mCVertexId[vStart + j];
                const unsigned int v1 = mCVertexId[vStart + (j + 1) % vNum];
                const float *vertex0 = mCVertex[v0];
                const float *vertex1 = mCVertex[v1];
                float edge[3] = {vertex0[0] - vertex1[0], vertex0[1] - vertex1[1], vertex0[2] - vertex1[2]};
                float edgeNormal[4];
                sub_1404E2FB0(face.Normal, edge, edgeNormal);
                const float dx = cross[0] - vertex1[0];
                const float dy = cross[1] - vertex1[1];
                const float dz = cross[2] - vertex1[2];
                if ((dx * edgeNormal[0] + dy * edgeNormal[1] + dz * edgeNormal[2]) <= 0.0f)
                {
                    ++insideCount;
                }
            }
        }

        if (insideCount == vNum)
        {
            const float y = cross[1];
            const unsigned int level = sub_1404E46E0(a5, a4, y);
            if (bestLevel == level)
            {
                if (y > bestY)
                {
                    bestLevel = level;
                    bestY = y;
                    mNowCFaceId = static_cast<int>(faceIndex);
                }
            }
            else if (bestLevel > level)
            {
                bestLevel = level;
                bestY = y;
                mNowCFaceId = static_cast<int>(faceIndex);
            }
        }
    }

    return bestY;
}

float CBsp::GetYposInLeafUseEdgeNormal(float *const a2, float *const a3, float a4, float a5, int a6)
{
    if (!mCNEdgeNormal)
    {
        Warning(aGetyposinleafu, byte_140883769);
        return 0.0f;
    }

    const _BSP_LEAF &leaf = mLeaf[a6];
    float bestY = FLOAT_N32000_0;
    if (!leaf.face_num)
    {
        return bestY;
    }

    unsigned int *faceIds = &mCFaceId[leaf.face_start_id];
    for (unsigned int i = 0; i < leaf.face_num; ++i)
    {
        const unsigned int faceIndex = faceIds[i];
        _BSP_C_FACE &face = mCFace[faceIndex];
        if (face.Attr & 0x40)
        {
            continue;
        }

        float cross[3];
        if (!GetPlaneCrossPoint(a2, a3, cross, face.Normal, face.Normal[3]))
        {
            continue;
        }

        const int vNum = face.VNum;
        int insideCount = 0;
        if (vNum > 0)
        {
            float *edgeNormals = reinterpret_cast<float *>(mCNEdgeNormal) + (faceIndex * 16);
            for (int j = 0; j < vNum; ++j)
            {
                if ((cross[0] * edgeNormals[0] + cross[1] * edgeNormals[1] + cross[2] * edgeNormals[2]
                     - edgeNormals[3])
                    <= -0.01f)
                {
                    break;
                }
                ++insideCount;
                edgeNormals += 4;
            }
        }

        if (insideCount == vNum && cross[1] > bestY && (a4 + a5) > cross[1] && cross[1] > (a5 - a4))
        {
            bestY = cross[1];
            mNowCFaceId = static_cast<int>(faceIndex);
        }
    }

    return bestY;
}

__int64 CBsp::GetLightFromPoint(float *const a2, int a3)
{
    float start[3] = {a2[0], a2[1] + static_cast<float>(a3), a2[2]};
    float end[3] = {a2[0], a2[1] - static_cast<float>(a3), a2[2]};
    __int16 leafList[1256];
    int leafCount = 0;

    GetLeafList(start, end, &leafCount, leafList, 0x4E7u);

    float bestDist = 100000000.0f;
    int bestFace = -1;

    if (leafCount <= 0)
        return 4286611584LL;

    for (int i = 0; i < leafCount; ++i)
    {
        _BSP_LEAF &leaf = mLeaf[leafList[i]];
        if (!leaf.face_num)
            continue;

        for (unsigned int j = 0; j < leaf.face_num; ++j)
        {
            unsigned int faceIdx = mCFaceId[leaf.face_start_id + j];
            _BSP_C_FACE &face = mCFace[faceIdx];
            if (face.Attr & 0x40)
                continue;

            float cross[3];
            if (!GetPlaneCrossPoint(start, end, cross, face.Normal, face.Normal[3]))
                continue;

            int inside = 0;
            int vNum = face.VNum;
            unsigned int vStart = face.VStartId;
            if (vNum)
            {
                for (int k = 0; k < vNum; ++k)
                {
                    unsigned int v0 = mCVertexId[vStart + k];
                    unsigned int v1 = mCVertexId[vStart + (k + 1) % vNum];
                    if (CheckEdgeNoEpsilon(mCVertex[v0], mCVertex[v1], cross, face.Normal))
                        ++inside;
                }
            }

            if (inside == vNum)
            {
                float dx = cross[0] - start[0];
                float dy = cross[1] - start[1];
                float dz = cross[2] - start[2];
                float dist = (dx * dx) + (dy * dy) + (dz * dz);
                if (dist < bestDist)
                {
                    bestDist = dist;
                    bestFace = static_cast<int>(faceIdx);
                }
            }
        }
    }

    if (bestFace == -1)
        return 4286611584LL;

    int lgtId = mMatGroup[mCFace[bestFace].MatGIndex].LgtId;
    unsigned int color = 0;
    if (lgtId == -1)
    {
        color = static_cast<unsigned int>(GetColorFromPoint(bestFace, a2));
    }
    else
    {
        float uv[2] = {};
        GetLightMapUVFromPoint(uv, bestFace, a2);
        color = GetLightMapColor(uv, lgtId);
    }

    unsigned int c0 = static_cast<unsigned int>(static_cast<unsigned char>(color));
    unsigned int c1 = static_cast<unsigned int>(static_cast<unsigned char>(color >> 8));
    unsigned int c2 = static_cast<unsigned int>(static_cast<unsigned char>(color >> 16));

    unsigned int r = static_cast<unsigned int>(static_cast<float>(c0) * 0.64999998f + 25.5f);
    unsigned int g = static_cast<unsigned int>(static_cast<float>(c1) * 0.64999998f + 25.5f);
    unsigned int b = static_cast<unsigned int>(static_cast<float>(c2) * 0.64999998f + 25.5f);

    unsigned int packed = (((b | 0xFFFFFF00u) << 8) | g) << 8;
    packed |= r;
    return static_cast<__int64>(packed);
}

__int64 CBsp::GetColorFromPoint(int a2, float *const a3)
{
    _BSP_C_FACE *face = &mCFace[a2];
    int v0 = 0;
    int v1 = 0;
    int v2 = 0;
    int r0 = 0;
    int r1 = 0;
    int r2 = 0;

    int start_v = static_cast<int>(face->VStartId);
    for (int i = 0; i < static_cast<int>(face->VNum) - 2; ++i)
    {
        v0 = start_v + 0;
        v1 = start_v + i + 1;
        v2 = start_v + i + 2;
        if (CheckEdgeNoEpsilon(mCVertex[mCVertexId[v0]], mCVertex[mCVertexId[v1]], a3, face->Normal) &&
            CheckEdgeNoEpsilon(mCVertex[mCVertexId[v1]], mCVertex[mCVertexId[v2]], a3, face->Normal) &&
            CheckEdgeNoEpsilon(mCVertex[mCVertexId[v2]], mCVertex[mCVertexId[v0]], a3, face->Normal))
        {
            break;
        }
    }

    float temp[3];
    VecSub(mCVertex[mCVertexId[v0]], a3, temp);
    float dist0 = VecLen(temp);
    VecSub(mCVertex[mCVertexId[v1]], a3, temp);
    float dist1 = VecLen(temp);
    VecSub(mCVertex[mCVertexId[v2]], a3, temp);
    float dist2 = VecLen(temp);

    if (dist0 > dist1)
    {
        if (dist0 > dist2)
        {
            r0 = v0;
            r1 = v1;
            r2 = v2;
        }
        else
        {
            r0 = v2;
            r1 = v0;
            r2 = v1;
        }
    }
    else
    {
        if (dist1 > dist2)
        {
            r0 = v1;
            r1 = v2;
            r2 = v0;
        }
        else
        {
            r0 = v2;
            r1 = v0;
            r2 = v1;
        }
    }

    float a[3];
    float b[3];
    VecSub(mCVertex[mCVertexId[r1]], mCVertex[mCVertexId[r0]], a);
    float leng0 = VecLen(a);
    VecSub(mCVertex[mCVertexId[r2]], mCVertex[mCVertexId[r0]], b);
    float leng1 = VecLen(b);
    VecSub(a3, mCVertex[mCVertexId[r0]], temp);
    dist2 = VecLen(temp);

    VecNormalize(a);
    VecNormalize(b);
    VecNormalize(temp);

    dist0 = VecDot(a, temp);
    dist0 = sqrtf_0(1.0f - (dist0 * dist0));
    dist1 = VecDot(b, temp);
    dist1 = sqrtf_0(1.0f - (dist1 * dist1));
    dist0 *= leng0;
    dist1 *= leng1;

    dist0 = dist0 / (dist1 + dist0);
    VecSub(mCVertex[mCVertexId[r2]], mCVertex[mCVertexId[r1]], a);
    a[0] *= dist0;
    a[1] *= dist0;
    a[2] *= dist0;
    a[0] += mCVertex[mCVertexId[r1]][0];
    a[1] += mCVertex[mCVertexId[r1]][1];
    a[2] += mCVertex[mCVertexId[r1]][2];

    VecSub(a, mCVertex[mCVertexId[r0]], b);
    dist1 = VecLen(b);
    dist1 = dist2 / dist1;

    float red[3];
    float green[3];
    float blue[3];
    red[0] = static_cast<float>((mVertexColor[mCVertexId[r0]] >> 16) & 0xFF) / 255.0f;
    red[1] = static_cast<float>((mVertexColor[mCVertexId[r1]] >> 16) & 0xFF) / 255.0f;
    red[2] = static_cast<float>((mVertexColor[mCVertexId[r2]] >> 16) & 0xFF) / 255.0f;
    green[0] = static_cast<float>((mVertexColor[mCVertexId[r0]] >> 8) & 0xFF) / 255.0f;
    green[1] = static_cast<float>((mVertexColor[mCVertexId[r1]] >> 8) & 0xFF) / 255.0f;
    green[2] = static_cast<float>((mVertexColor[mCVertexId[r2]] >> 8) & 0xFF) / 255.0f;
    blue[0] = static_cast<float>(mVertexColor[mCVertexId[r0]] & 0xFF) / 255.0f;
    blue[1] = static_cast<float>(mVertexColor[mCVertexId[r1]] & 0xFF) / 255.0f;
    blue[2] = static_cast<float>(mVertexColor[mCVertexId[r2]] & 0xFF) / 255.0f;

    float rr = (red[2] - red[1]) * dist0 + red[1];
    rr = (rr - red[0]) * dist1 + red[0];
    float gg = (green[2] - green[1]) * dist0 + green[1];
    gg = (gg - green[0]) * dist1 + green[0];
    float bb = (blue[2] - blue[1]) * dist0 + blue[1];
    bb = (bb - blue[0]) * dist1 + blue[0];

    unsigned int out = 0xFF000000u | (static_cast<unsigned int>(rr * 255.0f) << 16)
                     | (static_cast<unsigned int>(gg * 255.0f) << 8)
                     | (static_cast<unsigned int>(bb * 255.0f));
    return static_cast<__int64>(out);
}

void CBsp::GetLightMapUVFromPoint(float *const a2, int a3, float *const a4)
{
    _BSP_C_FACE *face = &mCFace[a3];
    int v0 = 0;
    int v1 = 0;
    int v2 = 0;
    int r0 = 0;
    int r1 = 0;
    int r2 = 0;

    int start_v = static_cast<int>(face->VStartId);
    for (int i = 0; i < static_cast<int>(face->VNum) - 2; ++i)
    {
        v0 = start_v + 0;
        v1 = start_v + i + 1;
        v2 = start_v + i + 2;
        if (CheckEdgeNoEpsilon(mCVertex[mCVertexId[v0]], mCVertex[mCVertexId[v1]], a4, face->Normal) &&
            CheckEdgeNoEpsilon(mCVertex[mCVertexId[v1]], mCVertex[mCVertexId[v2]], a4, face->Normal) &&
            CheckEdgeNoEpsilon(mCVertex[mCVertexId[v2]], mCVertex[mCVertexId[v0]], a4, face->Normal))
        {
            break;
        }
    }

    float temp[3];
    VecSub(mCVertex[mCVertexId[v0]], a4, temp);
    float dist0 = VecLen(temp);
    VecSub(mCVertex[mCVertexId[v1]], a4, temp);
    float dist1 = VecLen(temp);
    VecSub(mCVertex[mCVertexId[v2]], a4, temp);
    float dist2 = VecLen(temp);

    if (dist0 > dist1)
    {
        if (dist0 > dist2)
        {
            r0 = v0;
            r1 = v1;
            r2 = v2;
        }
        else
        {
            r0 = v2;
            r1 = v0;
            r2 = v1;
        }
    }
    else
    {
        if (dist1 > dist2)
        {
            r0 = v1;
            r1 = v2;
            r2 = v0;
        }
        else
        {
            r0 = v2;
            r1 = v0;
            r2 = v1;
        }
    }

    float a[3];
    float b[3];
    VecSub(mCVertex[mCVertexId[r1]], mCVertex[mCVertexId[r0]], a);
    float leng0 = VecLen(a);
    VecSub(mCVertex[mCVertexId[r2]], mCVertex[mCVertexId[r0]], b);
    float leng1 = VecLen(b);
    VecSub(a4, mCVertex[mCVertexId[r0]], temp);
    dist2 = VecLen(temp);

    VecNormalize(a);
    VecNormalize(b);
    VecNormalize(temp);

    dist0 = VecDot(a, temp);
    dist0 = sqrtf_0(1.0f - (dist0 * dist0));
    dist1 = VecDot(b, temp);
    dist1 = sqrtf_0(1.0f - (dist1 * dist1));
    dist0 *= leng0;
    dist1 *= leng1;

    dist0 = dist0 / (dist1 + dist0);
    VecSub(mCVertex[mCVertexId[r2]], mCVertex[mCVertexId[r1]], a);
    a[0] *= dist0;
    a[1] *= dist0;
    a[2] *= dist0;
    a[0] += mCVertex[mCVertexId[r1]][0];
    a[1] += mCVertex[mCVertexId[r1]][1];
    a[2] += mCVertex[mCVertexId[r1]][2];

    VecSub(a, mCVertex[mCVertexId[r0]], b);
    dist1 = VecLen(b);
    dist1 = dist2 / dist1;

    float l_uv[3][2];
    l_uv[0][0] = static_cast<float>(mLgtUV[r0][0]) / 32767.0f;
    l_uv[0][1] = static_cast<float>(mLgtUV[r0][1]) / 32767.0f;
    l_uv[1][0] = static_cast<float>(mLgtUV[r1][0]) / 32767.0f;
    l_uv[1][1] = static_cast<float>(mLgtUV[r1][1]) / 32767.0f;
    l_uv[2][0] = static_cast<float>(mLgtUV[r2][0]) / 32767.0f;
    l_uv[2][1] = static_cast<float>(mLgtUV[r2][1]) / 32767.0f;

    a2[0] = (l_uv[2][0] - l_uv[1][0]) * dist0 + l_uv[1][0];
    a2[1] = (l_uv[2][1] - l_uv[1][1]) * dist0 + l_uv[1][1];

    a2[0] = (a2[0] - l_uv[0][0]) * dist1 + l_uv[0][0];
    a2[1] = (a2[1] - l_uv[0][1]) * dist1 + l_uv[0][1];
}

void CBsp::GetLeafList(float *const a2, float *const a3, int *const a4, __int16 *a5, unsigned int a6)
{
    if (!a4) return;
    int count = 0;
    this->WalkNodeForLeafList(1, a2, a3, a5, &count);
    *a4 = count;
}

void CBsp::WalkNodeForLeafList(short nNodeId, float *const a3, float *const a4, short *a5, int *a6)
{
    if (nNodeId == 0) return;
    _BSP_NODE &node = this->mNode[nNodeId];
    float *normal = this->mCNNormal[node.f_normal_id];
    
    float v11 = DotProduct(normal, a3) - node.d;
    float v12 = DotProduct(normal, a4) - node.d;
    
    if ((v11 > 0.0f || v12 <= 0.0f) && (v12 > 0.0f || v11 <= 0.0f))
    {
        this->SubLeafList(v11, &node, a3, a4, a5, a6);
    }
    else
    {
        float v13 = v11 / (v11 - v12);
        float intersection[3];
        intersection[0] = (a4[0] - a3[0]) * v13 + a3[0];
        intersection[1] = (a4[1] - a3[1]) * v13 + a3[1];
        intersection[2] = (a4[2] - a3[2]) * v13 + a3[2];
        
        this->SubLeafList(v11, &node, a3, intersection, a5, a6);
        this->SubLeafList(v12, &node, intersection, a4, a5, a6);
    }
}

void CBsp::SubLeafList(float dist, _BSP_NODE *pNode, float *const a4, float *const a5, short *a6, int *a7)
{
    short next = (dist >= 0.0f) ? pNode->front : pNode->back;
    if (next < 0)
    {
        a6[(*a7)++] = -1 - next;
    }
    else
    {
        this->WalkNodeForLeafList(next, a4, a5, a6, a7);
    }
}

__int64 CBsp::IsCollisionFace(float *const a2, float *const a3, float (*a4)[3], float (*a5)[4])
{
    float *v5 = a3;
    float *v6 = a2;
    float *v61 = reinterpret_cast<float *>(a4);
    float *v63 = reinterpret_cast<float *>(a5);
    int v51 = 0;
    __int16 v66[32000];

    GetLeafList(a2, a3, &v51, v66, 0x7D00u);

    float v8 = 1.0e8f;
    int v50 = 0;
    int v53 = 0;
    if (v51 <= 0)
        return 0LL;

    __int16 *v10 = v66;
    unsigned int v11 = static_cast<unsigned int>(v51);
    do
    {
        _BSP_LEAF *mLeaf = this->mLeaf;
        int v14 = *v10;
        unsigned short face_num = mLeaf[v14].face_num;
        int face_start_id = static_cast<int>(mLeaf[v14].face_start_id);
        if (face_num)
        {
            int v13 = 0;
            int v17 = face_start_id;
            do
            {
                _BSP_C_FACE *mCFace = this->mCFace;
                int v55 = this->mCFaceId[v17];
                if ((mCFace[v55].Attr & 0x40) == 0)
                {
                    float v59[4];
                    if ((unsigned int)GetPlaneCrossPoint(v6, v5, v59, mCFace[v55].Normal, mCFace[v55].Normal[3]))
                    {
                        int v22 = mCFace[v55].VNum;
                        int v23 = static_cast<int>(mCFace[v55].VStartId);
                        float v24 = v59[2];
                        float v25 = v59[1];
                        float v26 = v59[0];
                        int v27 = 0;
                        int v28 = 0;
                        if (v22)
                        {
                            unsigned int *mCVertexId = this->mCVertexId;
                            float *mCVertex = reinterpret_cast<float *>(this->mCVertex);
                            float *v35 = mCFace[v55].Normal;
                            do
                            {
                                ++v28;
                                unsigned int v36 = mCVertexId[v23 + v28 - 1];
                                unsigned int vNext = mCVertexId[v23 + v28 % v22];
                                float v37 = mCVertex[3 * vNext];
                                float v38 = mCVertex[3 * vNext + 1];
                                float v39 = mCVertex[3 * vNext + 2];
                                float v40 = mCVertex[3 * v36 + 1] - v38;
                                float v60[3];
                                v60[0] = mCVertex[3 * v36] - v37;
                                float v41 = mCVertex[3 * v36 + 2];
                                v60[1] = v40;
                                v60[2] = v41 - v39;
                                float v58[4];
                                sub_1404E2FB0(v35, v60, v58);
                                if ((float)((float)((float)(v25 - v38) * v58[1]) + (float)((float)(v26 - v37) * v58[0]))
                                        + (float)((float)(v24 - v39) * v58[2])
                                    <= 0.0f)
                                    ++v27;
                            } while (v28 < v22);
                        }
                        if (v27 == v22)
                        {
                            float dist = (float)((float)((float)(v25 - v6[1]) * (float)(v25 - v6[1]))
                                                  + (float)((float)(v26 - *v6) * (float)(v26 - *v6)))
                                        + (float)((float)(v24 - v6[2]) * (float)(v24 - v6[2]));
                            if (v8 > dist)
                            {
                                v8 = dist;
                                v50 = 1;
                                v61[0] = v26;
                                v61[1] = v25;
                                v61[2] = v24;
                                v53 = v55;
                            }
                        }
                    }
                }
                ++v13;
                ++v17;
            } while (v13 < face_num);
        }
        ++v10;
        --v11;
    } while (v11);

    if (!v50)
        return 0LL;

    v63[0] = this->mCFace[v53].Normal[0];
    v63[1] = this->mCFace[v53].Normal[1];
    v63[2] = this->mCFace[v53].Normal[2];
    v63[3] = this->mCFace[v53].Normal[3];
    return 1LL;
}

void CBsp::LoadBsp(char *a2)
{
    if (!IsInitR3Engine())
        Error(aR3engineAe, byte_140883769);

    SetMergeFileManager(nullptr);

    FILE *fp = fopen(a2, "rb");
    if (!fp)
        Error(a2, aAaiai_0);

    unsigned int v5 = 0;
    mNowCFaceId = 0;
    mTotalAllocSize = 0;
    ResetTotalVertexBufferInfo();

    fread(&mBSPHeader, 0x2ACu, 1, fp);
    if (mBSPHeader.version != 39)
        Error(aBspAai, aAa);

    mLeafNum = mBSPHeader.Leaf.size / 25;
    mNodeNum = mBSPHeader.Node.size / 0x18;
    mCVertexNum = mBSPHeader.BVertex.size / 3 + mBSPHeader.WVertex.size / 6 + mBSPHeader.FVertex.size / 0xC;
    mCFaceNum = mBSPHeader.Face.size / 6;
    mObjectNum = mBSPHeader.Object.size / 0x58;

    mMatGroupNum = mBSPHeader.ReadMatGroup.size / 42;

    int v10 = 12 * mCVertexNum;
    unsigned int size = mBSPHeader.VertexId.size;
    int v12 = 24 * mCFaceNum;

    if (IsServerMode())
    {
        mCNEdgeNormal = reinterpret_cast<float (*)[4]>(Dmalloc(mCFaceNum << 6));
        mMapEntitiesListNum = 0;
        mEntityIDNum = 0;
        mEntityListNum = 0;
        mLeafEntityListNum = 0;
    }

    unsigned int v13 = v10 + size;
    unsigned int v14 = mBSPHeader.ReadMatGroup.size / 42;
    unsigned int v15 =
        v12 + v13 + mBSPHeader.CPlanes.size + mBSPHeader.CFaceId.size + mBSPHeader.Node.size + mBSPHeader.Track.size +
        mBSPHeader.Leaf.size + mBSPHeader.LgtUV.size + mBSPHeader.MatListInLeaf.size + mBSPHeader.VertexColor.size +
        361 * (mBSPHeader.Object.size / 0x58) +
        2 * (mObjectNum + 29 * mMapEntitiesListNum +
             43 * v14);

    mStaticAllocSize = v15;
    mTotalAllocSize += v15;
    mStaticAlloc = static_cast<unsigned char *>(Dmalloc(v15));

    mCVertex = reinterpret_cast<float (*)[3]>(mStaticAlloc);
    mCVertexId = reinterpret_cast<unsigned int *>(&mStaticAlloc[v10]);
    mCFace = reinterpret_cast<_BSP_C_FACE *>(&mStaticAlloc[v13]);

    unsigned int v17 = v12 + v13;
    mCNNormal = reinterpret_cast<float (*)[3]>(&mStaticAlloc[v17]);
    fread(mCNNormal, mBSPHeader.CPlanes.size, 1, fp);

    unsigned int v19 = mBSPHeader.CPlanes.size + v17;
    mCFaceId = reinterpret_cast<unsigned int *>(&mStaticAlloc[v19]);
    fread(mCFaceId, mBSPHeader.CFaceId.size, 1, fp);

    unsigned int v21 = mBSPHeader.CFaceId.size + v19;
    mNode = reinterpret_cast<_BSP_NODE *>(&mStaticAlloc[v21]);
    fread(mNode, mBSPHeader.Node.size, 1, fp);

    unsigned int v23 = mBSPHeader.Node.size + v21;
    mLeaf = reinterpret_cast<_BSP_LEAF *>(&mStaticAlloc[v23]);
    fread(mLeaf, mBSPHeader.Leaf.size, 1, fp);

    unsigned int v25 = mBSPHeader.Leaf.size + v23;
    MatListInLeafId = reinterpret_cast<unsigned short *>(&mStaticAlloc[v25]);
    fread(MatListInLeafId, mBSPHeader.MatListInLeaf.size, 1, fp);

    unsigned int v27 = mBSPHeader.MatListInLeaf.size + v25;
    auto *v28 = static_cast<unsigned int *>(Dmalloc(mBSPHeader.Object.size));
    mObject = reinterpret_cast<_ANI_OBJECT *>(&mStaticAlloc[v27]);
    fread(v28, mBSPHeader.Object.size, 1, fp);

    unsigned int v29 = 361 * (mBSPHeader.Object.size / 0x58) + v27;
    unsigned char *v30 = &mStaticAlloc[v29];
    fread(v30, mBSPHeader.Track.size, 1, fp);

    unsigned int v31 = mBSPHeader.Track.size + v29;
    mEventObjectID = reinterpret_cast<unsigned short *>(&mStaticAlloc[v31]);
    fread(mEventObjectID, mBSPHeader.EventObjectID.size, 1, fp);

    unsigned int v33 = v31 + 2 * mObjectNum;
    int v34 = 0;
    unsigned int *p_size = &mBSPHeader.ReadSpare[0].size;
    __int64 v36 = 35;
    do
    {
        v34 += *p_size;
        p_size += 2;
        --v36;
    } while (v36);
    fseek(fp, v34, 1);

    ConvAniObject(static_cast<int>(mBSPHeader.Object.size / 0x58), v30, reinterpret_cast<_READ_ANI_OBJECT *>(v28), mObject);
    Dfree(v28);

    mMatGroup = reinterpret_cast<_BSP_MAT_GROUP *>(&mStaticAlloc[v33]);
    unsigned int v38 = mBSPHeader.ReadMatGroup.size / 42;
    unsigned int v39 = 86 * v38 + v33;
    mLgtUV = reinterpret_cast<__int16 (*)[2]>(&mStaticAlloc[v39]);
    mVertexColor = reinterpret_cast<unsigned int *>(&mStaticAlloc[mBSPHeader.LgtUV.size + v39]);

    ReadDynamicDataFillVertexBuffer(fp);
    fclose(fp);

    mMatGroupCacheSize = static_cast<int>((mBSPHeader.ReadMatGroup.size / 336) + 1);
    mMatGroupCache = static_cast<unsigned char *>(Dmalloc(mMatGroupCacheSize));
    mTotalAllocSize += mMatGroupCacheSize;

    mAlpha.InitAlpha(this);

    mMapEntityMFM.InitMergeFile(byte_184A790F0);
    SetMergeFileManager(&mMapEntityMFM);

    unsigned int v42 = 0;
    int *mEnvIDPtr = reinterpret_cast<int *>(mEnvID);
    do
    {
        int v46 = unk_184A7999C[v42];
        mEnvIDPtr[0] = v46;
        if (v46)
        {
            char *v47 = &byte_184A79924[128 * v42];
            if (IsParticle(v47))
                mEnvIDPtr[0] |= 0x1000u;
            if (_bittest(reinterpret_cast<const LONG *>(mEnvIDPtr), 0xCu))
            {
                CParticle *v48 = reinterpret_cast<CParticle *>(operator new(0x490ull));
                CParticle *v49 = v48 ? new (v48) CParticle() : nullptr;
                ___u21.mEnvEntity[v42] = reinterpret_cast<CEntity *>(v49);
                if (v49 && v49->LoadParticleSPT(v47, 0))
                {
                    v49->InitParticle();
                    v49->SetParticleState(1u);
                }
                else
                {
                    CParticle *v50 = reinterpret_cast<CParticle *>(___u21.mEnvEntity[v42]);
                    if (___u21.mEnvEntity[v42])
                    {
                        v50->~CParticle();
                        operator delete(v50);
                    }
                    mEnvIDPtr[0] = 0;
                    unk_184A7999C[v42] = 0;
                }
            }
            else
            {
                CEntity *v51 = reinterpret_cast<CEntity *>(operator new(0xF4ull));
                CEntity *v52 = v51 ? new (v51) CEntity() : nullptr;
                ___u21.mEnvEntity[v42] = v52;
                if (v52 && !v52->LoadEntity(v47, 0))
                {
                    CEntity *v53 = ___u21.mEnvEntity[v42];
                    if (v53)
                    {
                        v53->~CEntity();
                        operator delete(v53);
                    }
                }
            }
        }
        ++v42;
        ++mEnvIDPtr;
    } while (v42 < 2);

    if (mObjectNum)
    {
        unsigned int v54 = 0;
        do
        {
            if (mObject[v54].parent >= mObjectNum)
                mObject[v54].parent = 0;
            ++v5;
            ++v54;
        } while (v5 < mObjectNum);
    }
}

void CBsp::LoadExtBsp(char *a2)
{
    mIsLoadEBP = 0;
    FILE *fp = fopen(a2, "rb");
    if (!fp)
        return;

    fread(&mExtBSPHeader, 0x184u, 1, fp);
    if (mExtBSPHeader.version != 20)
    {
        Warning(aEbpAai, aAa);
        return;
    }

    unsigned int cfVertexSize = mExtBSPHeader.CFVertex.size;
    unsigned int entityListSize = mExtBSPHeader.EntityList.size;
    mCFLineNum = mExtBSPHeader.CFLine.size >> 4;
    mCFVertexNum = cfVertexSize / 0xC;
    mCFLineIdNum = mExtBSPHeader.CFLineId.size >> 1;
    mLeafSoundEntityListNum = mExtBSPHeader.LeafSoundEntityList.size >> 2;
    mEntityListNum = entityListSize / 0x54;
    mEntityIDNum = mExtBSPHeader.EntityID.size >> 1;
    mLeafEntityListNum = mExtBSPHeader.LeafEntityList.size / 6;
    mSoundEntityIDNum = mExtBSPHeader.SoundEntityID.size >> 1;
    mMapEntitiesListNum = mExtBSPHeader.MapEntitiesList.size / 0x26;
    mSoundEntityListNum = mExtBSPHeader.SoundEntityList.size >> 6;
    mSoundEntitiesListNum = mExtBSPHeader.SoundEntitiesList.size / 0x38;

    if (mLeafSoundEntityListNum && mLeafSoundEntityListNum != mLeafNum)
    {
        fclose(fp);
        Error(a2, aEbpAaiaiBsp);
    }
    if (mLeafEntityListNum && mLeafEntityListNum != mLeafNum)
    {
        fclose(fp);
        Error(a2, aEbpAaiaiBsp);
    }

    unsigned int allocSize = mExtBSPHeader.CFLineId.size + mExtBSPHeader.CFLeaf.size + mExtBSPHeader.CFLine.size +
                             mExtBSPHeader.CFVertex.size +
                             2 * (mEntityIDNum + mSoundEntityIDNum + 29 * mMapEntitiesListNum +
                                  3 * (mLeafEntityListNum + 2 * (mCFVertexNum + 7 * mEntityListNum)) +
                                  2 * (mLeafSoundEntityListNum +
                                       4 * (mCFLineNum + 9 * mSoundEntitiesListNum + 4 * mSoundEntityListNum)));

    mTotalAllocSize += allocSize;
    mExtBspStaticAllocSize = allocSize;
    mExtBspStaticAlloc = static_cast<unsigned char *>(Dmalloc(allocSize));

    unsigned int offset = 0;
    mCFVertex = reinterpret_cast<float (*)[3]>(mExtBspStaticAlloc + offset);
    fread(mCFVertex, mExtBSPHeader.CFVertex.size, 1, fp);
    offset += mExtBSPHeader.CFVertex.size;

    mCFLine = reinterpret_cast<_TOOL_COL_LINE *>(mExtBspStaticAlloc + offset);
    fread(mCFLine, mExtBSPHeader.CFLine.size, 1, fp);
    offset += mExtBSPHeader.CFLine.size;

    mCFLineId = reinterpret_cast<unsigned __int16 *>(mExtBspStaticAlloc + offset);
    fread(mCFLineId, mExtBSPHeader.CFLineId.size, 1, fp);
    offset += mExtBSPHeader.CFLineId.size;

    mCFLeaf = reinterpret_cast<_TOOL_COL_LEAF *>(mExtBspStaticAlloc + offset);
    fread(mCFLeaf, mExtBSPHeader.CFLeaf.size, 1, fp);
    offset += mExtBSPHeader.CFLeaf.size;

    mCFVNormal = reinterpret_cast<float (*)[3]>(mExtBspStaticAlloc + offset);
    offset += 12 * mCFVertexNum;

    mCFNormal = reinterpret_cast<float (*)[4]>(mExtBspStaticAlloc + offset);
    offset += 16 * mCFLineNum;

    SetCFNormal();

    mEntityList = reinterpret_cast<_ENTITY_LIST *>(mExtBspStaticAlloc + offset);
    fread(mEntityList, mExtBSPHeader.EntityList.size, 1, fp);
    offset += mExtBSPHeader.EntityList.size;

    mEntityID = reinterpret_cast<unsigned __int16 *>(mExtBspStaticAlloc + offset);
    fread(mEntityID, mExtBSPHeader.EntityID.size, 1, fp);
    offset += mExtBSPHeader.EntityID.size;

    mLeafEntityList = reinterpret_cast<_LEAF_ENTITIES_LIST_INFO *>(mExtBspStaticAlloc + offset);
    fread(mLeafEntityList, mExtBSPHeader.LeafEntityList.size, 1, fp);
    offset += mExtBSPHeader.LeafEntityList.size;

    mSoundEntityID = reinterpret_cast<unsigned __int16 *>(mExtBspStaticAlloc + offset);
    fread(mSoundEntityID, mExtBSPHeader.SoundEntityID.size, 1, fp);
    offset += mExtBSPHeader.SoundEntityID.size;

    mLeafSoundEntityList = reinterpret_cast<_LEAF_SOUND_ENTITIES_LIST_INFO *>(mExtBspStaticAlloc + offset);
    fread(mLeafSoundEntityList, mExtBSPHeader.LeafSoundEntityList.size, 1, fp);
    offset += mExtBSPHeader.LeafSoundEntityList.size;

    mMapEntitiesList = reinterpret_cast<_MAP_ENTITIES_LIST *>(mExtBspStaticAlloc + offset);
    memset_0(mMapEntitiesList, 0, 58 * mMapEntitiesListNum);
    offset += 58 * mMapEntitiesListNum;

    mSoundEntityList = reinterpret_cast<_SOUND_ENTITY_LIST *>(mExtBspStaticAlloc + offset);
    mSoundEntitiesList = reinterpret_cast<_SOUND_ENTITIES_LIST *>(mExtBspStaticAlloc + offset + 64 * mSoundEntityListNum);

    ReadDynamicDataExtBsp(fp);
    fclose(fp);

    mEntityCacheSize = mExtBSPHeader.MapEntitiesList.size / 0x130 + 1;
    mEntityCache = static_cast<unsigned char *>(Dmalloc(mEntityCacheSize));
    mTotalAllocSize += mEntityCacheSize;

    mSoundEntityCacheSize = (mSoundEntitiesListNum >> 3) + 1;
    mSoundEntityCache = static_cast<unsigned char *>(Dmalloc(mSoundEntityCacheSize));
    memset_0(mSoundEntityCache, 0, mSoundEntityCacheSize);
    mTotalAllocSize += mSoundEntityCacheSize;

    mIsLoadEBP = 1;
}

bool CBsp::EdgeTest(float *pPos, unsigned int nLineId)
{
    _TOOL_COL_LINE &line = this->mCFLine[nLineId];
    float *v = (float*)this->mCFVertex;
    float *n = this->mCFNormal[nLineId];

    float p1[3] = { v[3 * line.start_v], v[3 * line.start_v + 1], v[3 * line.start_v + 2] };
    float p2[3] = { v[3 * line.start_v], v[3 * line.start_v + 1] + line.height, v[3 * line.start_v + 2] };

    if (!CheckEdgeEpsilon(p1, p2, pPos, n)) return false;

    float p3[3] = { v[3 * line.end_v], v[3 * line.end_v + 1] + line.height, v[3 * line.end_v + 2] };
    if (!CheckEdgeEpsilon(p2, p3, pPos, n)) return false;

    float p4[3] = { v[3 * line.end_v], v[3 * line.end_v + 1], v[3 * line.end_v + 2] };
    if (!CheckEdgeEpsilon(p3, p4, pPos, n)) return false;

    return CheckEdgeEpsilon(p4, p1, pPos, n);
}

bool CBsp::CanYouGoThere(float *const a2, float *const a3, float (*a4)[3]) { 
    return !this->mIsLoaded || this->GetPathCrossPoint(a2, a3, a4, 0, 0) == 0;
}

int CBsp::GetPathCrossPoint(float *const a2, float *const a3, float (*a4)[3], int a5, int a6)
{
    float v6 = 16.0f;
    float v7 = 100000.0f;
    mColFaceId = -1;
    float *v8 = reinterpret_cast<float *>(a4);
    float *v64 = reinterpret_cast<float *>(a4);
    int v9 = 0;
    a2[1] = a2[1] + 16.0f;
    float v10 = a3[1];
    float *v11 = a3;
    float *v63 = a3;
    float *v12 = a2;
    float *v56 = a2;
    int v52 = 0;
    a3[1] = v10 + 16.0f;
    int v14 = 0;
    int v15 = 0;
    dword_184A7B2F0 = 0;
    int v53 = 0;
    __int16 v66[32000];
    int v65[10000];
    int v54 = 0;
    int v55 = 0;
    int v21 = 0;
    int v22 = 0;
    float v57 = 0.0f;
    float v58 = 0.0f;
    float v59 = 0.0f;
    float v60 = 0.0f;
    float v61 = 0.0f;
    float v62 = 0.0f;

    GetLeafList(a2, a3, &v53, v66, 0x7D00u);
    if (mCFLineNum < 2)
    {
        v12[1] = v12[1] - 16.0f;
        v11[1] = v11[1] - 16.0f;
        return 0;
    }

    int v17 = v53;
    int v18 = a6;
    int v19 = a5;
    int v20 = 0;
    __int16 *v23 = v66;
    float *mCFNormal = nullptr;
    float v27 = 0.0f;
    float v28 = 0.0f;
    float v29 = 0.0f;
    float v30 = 0.0f;
    float v31 = 0.0f;
    float v32 = 0.0f;
    float v33 = 0.0f;
    float v34 = 0.0f;
    float v35 = 0.0f;
    float v36 = 0.0f;
    float v37 = 0.0f;
    bool v38 = false;
    float v39 = 0.0f;
    float v41 = 0.0f;
    float v42 = 0.0f;
    float v43 = 0.0f;
    float v44 = 0.0f;
    float dx = 0.0f;
    float dy = 0.0f;
    float dz = 0.0f;
    if (v53 <= 0)
        goto LABEL_52;
    do
    {
        int v24 = 0;
        if (!mCFLeaf[*v23].line_num)
            goto LABEL_50;
        do
        {
            unsigned int v25 = mCFLineId[v24 + mCFLeaf[*v23].start_id];
            if (v19 == static_cast<int>(v25))
            {
                v9 = v52;
                goto LABEL_48;
            }
            if (v18 == static_cast<int>(v25))
            {
                v9 = v52;
                goto LABEL_48;
            }
            mCFNormal = reinterpret_cast<float *>(this->mCFNormal);
            v27 = *v12;
            v28 = v12[1];
            v29 = v12[2];
            v30 = mCFNormal[4 * v25];
            v31 = mCFNormal[4 * v25 + 2];
            v32 = mCFNormal[4 * v25 + 3];
            v33 = *v63;
            v34 = v63[1];
            v35 = v63[2];
            v36 = (float)((float)((float)(v27 * v30) + (float)(mCFNormal[4 * v25 + 1] * v28)) + (float)(v29 * v31)) - v32;
            v37 = (float)((float)((float)(v33 * v30) + (float)(mCFNormal[4 * v25 + 1] * v34)) + (float)(v35 * v31)) - v32;
            if (v36 > -0.01f && v36 < 0.01f)
                v55 = ++v21;
            if (v36 > 0.0f && v37 > 0.0f)
            {
                v9 = v52;
                goto LABEL_48;
            }
            if (v36 < -0.01f && v37 < -0.01f)
            {
                v9 = v52;
                goto LABEL_48;
            }
            if (v36 <= -0.01f || v37 > 0.0f)
            {
                if (v36 >= 0.0f || v37 <= 0.0f)
                {
                    if (v36 > -0.01f && v37 > -0.01f)
                    {
                        v9 = v52;
                        goto LABEL_48;
                    }
                    v38 = v36 < -0.01f && v37 > -0.01f;
                }
                else
                {
                    v38 = true;
                }
            }
            else
            {
                v38 = false;
            }
            v39 = v36 / (float)(v36 - v37);
            v60 = (float)((float)(v33 - v27) * v39) + v27;
            v61 = (float)((float)(v34 - v28) * v39) + v28;
            v62 = (float)((float)(v35 - v29) * v39) + v29;
            if (EdgeTest(&v60, v25))
            {
                int v40 = 0;
                if (v22 <= 0)
                    goto LABEL_31;
                while (v65[v40] != static_cast<int>(v25))
                {
                    if (++v40 >= v22)
                        goto LABEL_31;
                }
                v9 = v52;
                goto LABEL_AFTER_EDGE;
LABEL_31:
                v41 = v60;
                v42 = v61;
                v43 = v62;
                v65[v22++] = static_cast<int>(v25);
                if (static_cast<int>(v25) >= 10000)
                    v22 = 9999;
                dx = v42 - v56[1];
                dy = v41 - v56[0];
                dz = v43 - v56[2];
                v44 = sqrtf_0((dx * dx) + (dy * dy) + (dz * dz));
                if (v7 <= v44)
                {
                    v9 = v52;
                    if (v38)
                        ++v14;
                    else
                        ++v15;
                }
                else
                {
                    if (v38)
                    {
                        v21 = v55;
                        v12 = v56;
                        v18 = a6;
                        v7 = v44;
                        v19 = a5;
                        v9 = 0;
                        ++v14;
                        v52 = 0;
                        goto LABEL_48;
                    }
                    this->mColFaceId = v25;
                    v9 = 1;
                    v57 = v41;
                    v58 = v42;
                    v59 = v43;
                    v7 = v44;
                    v52 = 1;
                    ++v15;
                }
LABEL_AFTER_EDGE:
                v21 = v55;
                v12 = v56;
                v18 = a6;
                v19 = a5;
            }
            else
            {
                v9 = v52;
                v21 = v55;
                v12 = v56;
                v18 = a6;
                v19 = a5;
            }
LABEL_48:
            ++v24;
        } while (v24 < this->mCFLeaf[*v23].line_num);
        v17 = v53;
        v20 = v54;
LABEL_50:
        ++v20;
        ++v23;
        v54 = v20;
    } while (v20 < v17);
    v6 = 16.0f;
    v11 = v63;
    v8 = v64;
LABEL_52:
    int v45 = dword_184A7B2F0;
    if (v14 - v15 == 1)
        v45 = 1;
    v12[1] = v12[1] - v6;
    float v46 = v11[1];
    dword_184A7B2F0 = v45;
    v11[1] = v46 - v6;
    if (v9)
    {
        sub_14050C650(&v57, v12, v11);
        float v47 = v57;
        float v48 = v58;
        float v49 = v59;
        v8[0] = v57;
        v8[1] = v48;
        v8[2] = v49;
        if (v15 - v14 > 0 || (v15 && !v14))
            return 3;
        int v50 = a6;
        if (!a5)
        {
            if (a6)
                goto LABEL_63;
            return 1;
        }
LABEL_63:
        float *v51 = reinterpret_cast<float *>(this->mCFNormal);
        float vPlaneA = (float)((float)((float)(v48 * v51[4 * a5 + 1]) + (float)(v47 * v51[4 * a5]))
                              + (float)(v49 * v51[4 * a5 + 2]))
                       - v51[4 * a5 + 3];
        float vPlaneB = (float)((float)((float)(v48 * v51[4 * v50 + 1]) + (float)(v47 * v51[4 * v50]))
                              + (float)(v49 * v51[4 * v50 + 2]))
                       - v51[4 * v50 + 3];
        if (vPlaneA >= 0.0f || vPlaneB >= 0.0f)
            return 1;
        return 2;
    }
    else
    {
        if ((v19 || v18) && v14)
            return 2;
        return 0;
    }
}

void CBsp::CalcEntitiesMainColor()
{
    unsigned int i = 0;
    if (mMapEntitiesListNum)
    {
        __int64 idx = 0;
        do
        {
            _MAP_ENTITIES_LIST *mapList = mMapEntitiesList;
            _ENTITY_LIST *entityList = mEntityList;
            __int64 id = mapList[idx].ID;
            if (entityList[id].IsFileExist)
            {
                if (entityList[id].IsParticle)
                {
                    mapList[idx].mMapColor = -8355712;
                }
                else
                {
                    mapList[idx].mMapColor = GetLightFromPoint(mapList[idx].Pos, 30);
                    if (mapList[idx].mMapColor == -8355712)
                        mapList[idx].mMapColor = GetLightFromPoint(mapList[idx].Pos, 60);
                }
            }
            ++i;
            ++idx;
        } while (i < mMapEntitiesListNum);
    }
}

void CBsp::SetCFNormal()
{
    unsigned int i = 0;
    if (mCFLineNum)
    {
        __int64 idx = 0;
        do
        {
            _TOOL_COL_LINE &line = mCFLine[idx];
            float *v = reinterpret_cast<float *>(mCFVertex);
            float p0[3] = {v[3 * line.start_v], v[3 * line.start_v + 1], v[3 * line.start_v + 2]};
            float p1[3] = {v[3 * line.end_v], v[3 * line.end_v + 1] + line.height, v[3 * line.end_v + 2]};
            float p2[3] = {v[3 * line.end_v], v[3 * line.end_v + 1], v[3 * line.end_v + 2]};
            GetNormal(mCFNormal[i++], p0, p1, p2);
            ++idx;
        } while (i < mCFLineNum);
    }

    memset_0(mCFVNormal, 0, 12LL * mCFVertexNum);
    unsigned int v6 = 1;
    unsigned int v7 = 1;
    if (mCFLineNum > 1)
    {
        __int64 v8 = 1;
        do
        {
            ++v7;
            __int64 start_v = mCFLine[v8++].start_v;
            mCFVNormal[start_v][0] = mCFNormal[v8 - 1][0] + mCFVNormal[start_v][0];
            mCFVNormal[start_v][1] = mCFNormal[v8 - 1][1] + mCFVNormal[start_v][1];
            mCFVNormal[start_v][2] = mCFNormal[v8 - 1][2] + mCFVNormal[start_v][2];
            __int64 end_v = mCFLine[v8 - 1].end_v;
            mCFVNormal[end_v][0] = mCFNormal[v8 - 1][0] + mCFVNormal[end_v][0];
            mCFVNormal[end_v][1] = mCFNormal[v8 - 1][1] + mCFVNormal[end_v][1];
            mCFVNormal[end_v][2] = mCFNormal[v8 - 1][2] + mCFVNormal[end_v][2];
        } while (v7 < mCFLineNum);
    }

    if (mCFVertexNum > 1)
    {
        __int64 v12 = 3;
        do
        {
            float *norm = reinterpret_cast<float *>(mCFVNormal);
            float nx = norm[v12];
            float ny = norm[v12 + 1];
            float nz = norm[v12 + 2];
            float len = sqrtf_0((nx * nx) + (ny * ny) + (nz * nz));
            ++v6;
            v12 += 3;
            norm[v12 - 3] = nx / len;
            norm[v12 - 2] = ny / len;
            norm[v12 - 1] = nz / len;
        } while (v6 < mCFVertexNum);
    }
}

void CBsp::OnlyStoreCollisionStructure(_BSP_READ_M_GROUP *pRM, char (*pBV)[3], short (*pWV)[3], float (*pFV)[3], unsigned int *pVI, _BSP_READ_FACE *pRF, unsigned int *pFI)
{
    unsigned int baseCharIndex = mBSPHeader.BVertex.size / 3;
    unsigned int baseWordIndex = baseCharIndex + mBSPHeader.WVertex.size / 6;
    unsigned int static_v_cnt = 0;
    _R3MATERIAL *mat = GetMainMaterial();

    int matGroupCount = static_cast<int>(mBSPHeader.ReadMatGroup.size / 0x2A);
    for (int i = 0; i < matGroupCount; ++i)
    {
        _BSP_READ_M_GROUP &mGroup = pRM[i];
        float *vertex = nullptr;
        int function_id = 4;
        if (mGroup.attr & 0x8000)
        {
            vertex = reinterpret_cast<float *>(pBV);
            function_id = 1;
        }
        else if (mGroup.attr & 0x4000)
        {
            vertex = reinterpret_cast<float *>(pWV);
            function_id = 2;
        }
        else
        {
            vertex = reinterpret_cast<float *>(pFV);
            function_id = 4;
        }

        float w_matrix[16] = {};
        if (mGroup.object_id)
        {
            mObject[mGroup.object_id - 1].now_frame = 0.0f;
            float t = R3GetTime() * 30.0f;
            GetObjectMatrix(reinterpret_cast<float (*)[4]>(w_matrix), mGroup.object_id, mObject, t);
        }

        for (unsigned int j = 0; j < mGroup.face_num; ++j)
        {
            unsigned int faceIdx = pFI[j + mGroup.face_start_id];
            unsigned int baseIndex = 0;
            if (function_id == 2)
                baseIndex = baseCharIndex;
            else if (function_id == 3)
                baseIndex = baseWordIndex;

            mCFace[faceIdx].Attr = 0;
            mCFace[faceIdx].VNum = pRF[faceIdx].v_num;
            mCFace[faceIdx].VStartId = static_v_cnt;
            mCFace[faceIdx].MatGIndex = i;

            for (unsigned int k = 0; k < pRF[faceIdx].v_num; ++k)
            {
                unsigned int vId = pRF[faceIdx].v_start_id + k;
                float pos[3];
                GetVertexFromCompress[function_id](pos, reinterpret_cast<char *>(vertex) + 3 * function_id * pVI[vId], &mGroup);
                unsigned int c_v_id = baseIndex + pVI[vId];

                if (mGroup.object_id)
                {
                    mCVertex[c_v_id][0] = (pos[0] * w_matrix[0]) + (pos[1] * w_matrix[4]) + (pos[2] * w_matrix[8]) + w_matrix[12];
                    mCVertex[c_v_id][1] = (pos[0] * w_matrix[1]) + (pos[1] * w_matrix[5]) + (pos[2] * w_matrix[9]) + w_matrix[13];
                    mCVertex[c_v_id][2] = (pos[0] * w_matrix[2]) + (pos[1] * w_matrix[6]) + (pos[2] * w_matrix[10]) + w_matrix[14];
                }
                else
                {
                    mCVertex[c_v_id][0] = pos[0];
                    mCVertex[c_v_id][1] = pos[1];
                    mCVertex[c_v_id][2] = pos[2];
                }

                mCVertexId[static_v_cnt++] = c_v_id;
            }

            unsigned int vStart = pRF[faceIdx].v_start_id;
            GetNormal(
                mCFace[faceIdx].Normal,
                mCVertex[baseIndex + pVI[vStart]],
                mCVertex[baseIndex + pVI[vStart + 1]],
                mCVertex[baseIndex + pVI[vStart + 2]]);

            float n0 = mCFace[faceIdx].Normal[0];
            float n1 = mCFace[faceIdx].Normal[1];
            float n2 = mCFace[faceIdx].Normal[2];
            int n3bits = *reinterpret_cast<int *>(&mCFace[faceIdx].Normal[3]);
            if (std::isnan(n0) || std::isnan(n1) || std::isnan(n2) || n3bits == -4194304)
            {
                mCFace[faceIdx].Normal[0] = 0.0f;
                mCFace[faceIdx].Normal[1] = 0.0f;
                mCFace[faceIdx].Normal[2] = 1.0f;
                mCFace[faceIdx].Normal[3] = 0.0f;
                mCFace[faceIdx].Attr |= 0x40u;
            }

            if (mGroup.mtl_id != -1 && mat)
            {
                if (_bittest(reinterpret_cast<const LONG *>(&mat[mGroup.mtl_id].m_Layer[0].m_dwFlag), 0x11u))
                    mCFace[faceIdx].Attr |= 0x40u;
            }
        }
    }
}

void CBsp::ReadDynamicDataFillVertexBuffer(FILE *Stream)
{
    int read_alloc_size = 0;
    read_alloc_size += mBSPHeader.BVertex.size;
    read_alloc_size += mBSPHeader.WVertex.size;
    read_alloc_size += mBSPHeader.FVertex.size;
    read_alloc_size += mBSPHeader.VertexColor.size;
    read_alloc_size += mBSPHeader.UV.size;
    read_alloc_size += mBSPHeader.LgtUV.size;
    read_alloc_size += mBSPHeader.Face.size;
    read_alloc_size += mBSPHeader.FaceId.size;
    read_alloc_size += mBSPHeader.VertexId.size;
    read_alloc_size += mBSPHeader.ReadMatGroup.size;

    unsigned char *read_alloc_buf = static_cast<unsigned char *>(Dmalloc(read_alloc_size));
    unsigned char *ptr = read_alloc_buf;

    char (*bvertex)[3] = reinterpret_cast<char (*)[3]>(ptr);
    fread(bvertex, mBSPHeader.BVertex.size, 1, Stream);
    ptr += mBSPHeader.BVertex.size;

    short (*wvertex)[3] = reinterpret_cast<short (*)[3]>(ptr);
    fread(wvertex, mBSPHeader.WVertex.size, 1, Stream);
    ptr += mBSPHeader.WVertex.size;

    float (*fvertex)[3] = reinterpret_cast<float (*)[3]>(ptr);
    fread(fvertex, mBSPHeader.FVertex.size, 1, Stream);
    ptr += mBSPHeader.FVertex.size;

    int v_normal_size = 12 * (mBSPHeader.FVertex.size / 0xC + mBSPHeader.BVertex.size / 3 + mBSPHeader.WVertex.size / 6);
    float *v_normal = static_cast<float *>(Dmalloc(v_normal_size));
    memset_0(v_normal, 0, v_normal_size);

    unsigned int *v_color = reinterpret_cast<unsigned int *>(ptr);
    fread(v_color, mBSPHeader.VertexColor.size, 1, Stream);
    ptr += mBSPHeader.VertexColor.size;

    float *uv = reinterpret_cast<float *>(ptr);
    fread(uv, mBSPHeader.UV.size, 1, Stream);
    ptr += mBSPHeader.UV.size;

    short *lgtuv = reinterpret_cast<short *>(ptr);
    fread(lgtuv, mBSPHeader.LgtUV.size, 1, Stream);
    ptr += mBSPHeader.LgtUV.size;

    _BSP_READ_FACE *face = reinterpret_cast<_BSP_READ_FACE *>(ptr);
    fread(face, mBSPHeader.Face.size, 1, Stream);
    ptr += mBSPHeader.Face.size;

    unsigned int *faceId = reinterpret_cast<unsigned int *>(ptr);
    fread(faceId, mBSPHeader.FaceId.size, 1, Stream);
    ptr += mBSPHeader.FaceId.size;

    unsigned int *vertexId = reinterpret_cast<unsigned int *>(ptr);
    fread(vertexId, mBSPHeader.VertexId.size, 1, Stream);
    ptr += mBSPHeader.VertexId.size;

    _BSP_READ_M_GROUP *readmatgroup = reinterpret_cast<_BSP_READ_M_GROUP *>(ptr);
    fread(readmatgroup, mBSPHeader.ReadMatGroup.size, 1, Stream);

    if (IsServerMode())
    {
        OnlyStoreCollisionStructure(readmatgroup, bvertex, wvertex, fvertex, vertexId, face, faceId);
        MakeEdgeNormal();
        Dfree(read_alloc_buf);
        mEntityListNum = 0;
        mLeafEntityListNum = 0;
        mEntityIDNum = 0;
        mMapEntitiesListNum = 0;
        mSoundEntityIDNum = 0;
        mLeafSoundEntityListNum = 0;
        mSoundEntityListNum = 0;
        mSoundEntitiesListNum = 0;
        return;
    }

    int static_vertexnum = 0;
    int static_trinum = 0;
    _R3MATERIAL *MainMaterial = GetMainMaterial();
    int uv_num = 0;
    int st_num = 0;
    int vp_state = 2;
    int st_num_acc = 0;
    int matGroupCount = static_cast<int>(mBSPHeader.ReadMatGroup.size / 0x2A);

    if (matGroupCount > 0)
    {
        for (int i = 0; i < matGroupCount; ++i)
        {
            unsigned int old_v_num = static_vertexnum;
            int group_v_num = 0;
            int group_tri_num = 0;
            if (readmatgroup[i].face_num)
            {
                for (unsigned int j = 0; j < readmatgroup[i].face_num; ++j)
                {
                    unsigned int f_id = faceId[readmatgroup[i].face_start_id + j];
                    int v_num = face[f_id].v_num;
                    group_v_num += v_num;
                    group_tri_num += v_num - 2;
                }
            }
            static_vertexnum += group_v_num;
            static_trinum += group_tri_num;

            if (static_vertexnum > 0xFFFF)
            {
                static_vertexnum -= old_v_num;
                mVBVertexNum[mStaticVBCnt++] = old_v_num;
                old_v_num = 0;
                if (mStaticVBCnt >= 0x50)
                    Error(aMaxVbCnt, byte_140883769);
            }

            unsigned int mflag = 0;
            short mtl_id = readmatgroup[i].mtl_id;
            if (mtl_id != -1)
            {
                unsigned int layerNum = MainMaterial[mtl_id].m_dwLayerNum;
                for (unsigned int j = 0; j < layerNum; ++j)
                    mflag |= MainMaterial[mtl_id].m_Layer[j].m_dwFlag;
            }

            if (mflag & 0xA)
            {
                st_num = static_vertexnum - old_v_num + st_num_acc;
                st_num_acc = st_num;
                uv_num += static_vertexnum - old_v_num;
            }
            else
            {
                st_num = st_num_acc;
            }

            if (mflag & 4)
                vp_state = 1;
            if (mflag)
                uv_num += static_vertexnum - old_v_num;
        }
    }

    mVBVertexNum[mStaticVBCnt++] = static_vertexnum;

    unsigned int *multiLayerAlloc = static_cast<unsigned int *>(Dmalloc(8 * uv_num + 36 * st_num));
    mMultiLayerUV = multiLayerAlloc;
    mMultiLayerST = &multiLayerAlloc[2 * uv_num];

    unsigned int prevVBSize = GetTotalVertexBufferSize();
    for (int i = 0; i < static_cast<int>(mStaticVBCnt); ++i)
        mStaticVertexBuffer[i].InitVertexBuffer(44 * mVBVertexNum[i], vp_state, 0x252u);
    mStaticIndexedBuffer.InitIndexBuffer(6 * static_trinum, 2);
    mVertexBufferSize = GetTotalVertexBufferSize() - prevVBSize;

    _D3DR3VERTEX_TEX2 *static_vertexbp = reinterpret_cast<_D3DR3VERTEX_TEX2 *>(mStaticVertexBuffer[0].VPLock(0, 0, 0));
    unsigned short *static_indexbp = reinterpret_cast<unsigned short *>(mStaticIndexedBuffer.VPLock(0, 0, 0));

    int static_vb_cnt = 0;
    int static_v_cnt = 0;
    int static_v_index_cnt = 0;
    int all_static_v_cnt = 0;
    int total_vertex_num = 0;
    int base_char_index = mBSPHeader.BVertex.size / 3;
    unsigned int uv_used = 0;
    unsigned int st_used = 0;
    unsigned int uv_write = 0;
    unsigned int st_write = 0;

    if (matGroupCount > 0)
    {
        float v65 = FLOAT_0_5;
        float v66 = FLOAT_32767_0;
        float v67 = FLOAT_8_0;
        float v68 = FLOAT_255_0;

        for (int i = 0; i < matGroupCount; ++i)
        {
            _BSP_READ_M_GROUP &group = readmatgroup[i];
            mMatGroup[i].BBMin[0] = static_cast<float>(group.bb_min[0]);
            mMatGroup[i].BBMin[1] = static_cast<float>(group.bb_min[1]);
            mMatGroup[i].BBMin[2] = static_cast<float>(group.bb_min[2]);
            mMatGroup[i].BBMax[0] = static_cast<float>(group.bb_max[0]);
            mMatGroup[i].BBMax[1] = static_cast<float>(group.bb_max[1]);
            mMatGroup[i].BBMax[2] = static_cast<float>(group.bb_max[2]);
            mMatGroup[i].MtlId = group.mtl_id;
            mMatGroup[i].LgtId = group.lgt_id;
            mMatGroup[i].ObjectId = group.object_id;
            mMatGroup[i].CoronaAlpha = 0.0f;

            float v72 = (mMatGroup[i].BBMax[1] - mMatGroup[i].BBMin[1]) * v65;
            float v73 = (mMatGroup[i].BBMax[2] - mMatGroup[i].BBMin[2]) * v65;
            mMatGroup[i].Origin[0] = mMatGroup[i].BBMin[0] + (mMatGroup[i].BBMax[0] - mMatGroup[i].BBMin[0]) * v65;
            mMatGroup[i].Origin[1] = v72 + mMatGroup[i].BBMin[1];
            mMatGroup[i].Origin[2] = v73 + mMatGroup[i].BBMin[2];

            char *vertex = nullptr;
            int function_id = 4;
            if (group.attr & 0x8000)
            {
                vertex = reinterpret_cast<char *>(bvertex);
                function_id = 1;
            }
            else if (group.attr & 0x4000)
            {
                vertex = reinterpret_cast<char *>(wvertex);
                function_id = 2;
            }
            else
            {
                vertex = reinterpret_cast<char *>(fvertex);
                function_id = 4;
            }

            float w_matrix[16] = {};
            if (mMatGroup[i].ObjectId)
            {
                mObject[mMatGroup[i].ObjectId - 1].now_frame = 0.0f;
                GetObjectMatrix(reinterpret_cast<float (*)[4]>(w_matrix), mMatGroup[i].ObjectId, mObject, 0.0);
            }

            unsigned int m_ARGB = 0xFFFFFFFFu;
            if (group.mtl_id != -1)
                m_ARGB = MainMaterial[group.mtl_id].m_Layer[0].m_ARGB;

            mMatGroup[i].Type = 1;
            if (group.mtl_id != -1)
            {
                if (MainMaterial[group.mtl_id].m_Layer[0].m_dwAlphaType)
                    mMatGroup[i].Type |= 0x8000u;
                if (MainMaterial[group.mtl_id].m_Layer[0].m_dwAlphaType & 0x4000)
                    mMatGroup[i].Type |= 0x4000u;
            }
            mMatGroup[i].Type |= (group.attr & 0xF00);

            mMatGroup[i].VertexBufferId = static_vb_cnt;
            mMatGroup[i].VBMinIndex = static_v_cnt;
            mMatGroup[i].IBMinIndex = static_v_index_cnt;
            mMatGroup[i].CFaceStartVId = static_v_cnt + all_static_v_cnt;

            int temp_f_num = static_v_index_cnt / 3;
            int temp_v_num = static_v_cnt;
            int real_v_index = (function_id == 2) ? base_char_index : 0;
            int temp_static_v_cnt = static_v_cnt;

            if (group.face_num)
            {
                for (unsigned int j = 0; j < group.face_num; ++j)
                {
                    unsigned int faceIdx = faceId[group.face_start_id + j];
                    mCFace[faceIdx].Attr = 0;

                    if (face[faceIdx].v_num)
                    {
                        for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                        {
                            unsigned int v_id = face[faceIdx].v_start_id + k;
                            float pos[3];
                            GetVertexFromCompress[function_id](pos, vertex + 3 * function_id * vertexId[v_id], &group);
                            unsigned int c_v_id = real_v_index + vertexId[v_id];

                            if (mMatGroup[i].ObjectId)
                            {
                                mCVertex[c_v_id][0] = (pos[0] * w_matrix[0]) + (pos[1] * w_matrix[4]) + (pos[2] * w_matrix[8]) + w_matrix[12];
                                mCVertex[c_v_id][1] = (pos[0] * w_matrix[1]) + (pos[1] * w_matrix[5]) + (pos[2] * w_matrix[9]) + w_matrix[13];
                                mCVertex[c_v_id][2] = (pos[0] * w_matrix[2]) + (pos[1] * w_matrix[6]) + (pos[2] * w_matrix[10]) + w_matrix[14];
                            }
                            else
                            {
                                mCVertex[c_v_id][0] = pos[0];
                                mCVertex[c_v_id][1] = pos[1];
                                mCVertex[c_v_id][2] = pos[2];
                            }

                            mCVertexId[temp_static_v_cnt + all_static_v_cnt] = c_v_id;
                            ++temp_static_v_cnt;
                        }
                    }

                    unsigned int v_start = face[faceIdx].v_start_id;
                    GetNormal(
                        mCFace[faceIdx].Normal,
                        mCVertex[real_v_index + vertexId[v_start]],
                        mCVertex[real_v_index + vertexId[v_start + 1]],
                        mCVertex[real_v_index + vertexId[v_start + 2]]);

                    float n0 = mCFace[faceIdx].Normal[0];
                    float n1 = mCFace[faceIdx].Normal[1];
                    float n2 = mCFace[faceIdx].Normal[2];
                    int n3bits = *reinterpret_cast<int *>(&mCFace[faceIdx].Normal[3]);
                    if (std::isnan(n0) || std::isnan(n1) || std::isnan(n2) || n3bits == -4194304)
                    {
                        mCFace[faceIdx].Normal[0] = 0.0f;
                        mCFace[faceIdx].Normal[1] = 0.0f;
                        mCFace[faceIdx].Normal[2] = 1.0f;
                        mCFace[faceIdx].Normal[3] = 0.0f;
                        mCFace[faceIdx].Attr |= 0x40u;
                        mCFace[faceIdx].Attr |= 0x10u;
                    }

                    if (mCFace[faceIdx].Normal[1] < 0.0f)
                        mCFace[faceIdx].Attr |= 0x80u;

                    if (group.mtl_id != -1)
                    {
                        if (_bittest(reinterpret_cast<const LONG *>(&MainMaterial[group.mtl_id].m_Layer[0].m_dwFlag), 0x11u))
                            mCFace[faceIdx].Attr |= 0x40u;
                        if (_bittest(reinterpret_cast<const LONG *>(&MainMaterial[group.mtl_id].m_Layer[0].m_dwFlag), 0x12u))
                            mCFace[faceIdx].Attr |= 0x20u;
                        if (MainMaterial[group.mtl_id].m_Layer[1].m_dwFlag & 0x8000)
                            mCFace[faceIdx].Attr |= 0x20u;
                    }

                    if (face[faceIdx].v_num)
                    {
                        for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                        {
                            unsigned int v_id = face[faceIdx].v_start_id + k;
                            unsigned int c_v_id = real_v_index + vertexId[v_id];
                            v_normal[3 * c_v_id] += mCFace[faceIdx].Normal[0];
                            v_normal[3 * c_v_id + 1] += mCFace[faceIdx].Normal[1];
                            v_normal[3 * c_v_id + 2] += mCFace[faceIdx].Normal[2];
                        }
                    }
                }
            }

            for (unsigned int j = 0; j < group.face_num; ++j)
            {
                unsigned int faceIdx = faceId[group.face_start_id + j];
                for (unsigned int k = 0; k < face[faceIdx].v_num - 2; ++k)
                {
                    static_indexbp[static_v_index_cnt++] = static_cast<unsigned short>(static_v_cnt);
                    static_indexbp[static_v_index_cnt++] = static_cast<unsigned short>(static_v_cnt + k + 1);
                    static_indexbp[static_v_index_cnt++] = static_cast<unsigned short>(static_v_cnt + k + 2);
                }

                mCFace[faceIdx].VNum = face[faceIdx].v_num;
                mCFace[faceIdx].VStartId = static_v_cnt + all_static_v_cnt;
                mCFace[faceIdx].MatGIndex = static_cast<unsigned short>(i);

                unsigned int matFlag = 0;
                if (group.mtl_id != -1)
                {
                    unsigned int layerNum = MainMaterial[group.mtl_id].m_dwLayerNum;
                    for (unsigned int k = 0; k < layerNum; ++k)
                        matFlag |= MainMaterial[group.mtl_id].m_Layer[k].m_dwFlag;
                }

                for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                {
                    unsigned int v_id = face[faceIdx].v_start_id + k;
                    float temp[4];
                    GetVertexFromCompress[function_id](temp, vertex + 3 * function_id * vertexId[v_id], &group);

                    unsigned int color = m_ARGB;
                    if (group.mtl_id != -1 && (matFlag & 0x300) != 0)
                    {
                        float v135 = 0.0f;
                        float v136 = 0.0f;
                        float v137 = 0.0f;
                        float v138 = 0.0f;

                        if (matFlag & 0x100)
                        {
                            v135 = (static_cast<float>(HiByte(static_cast<unsigned int>(MainMaterial[group.mtl_id].m_Layer[0].m_sGradientAlpha))) - 100.0f) / 25.0f;
                            v136 = uv[2 * v_id] / v135;
                            if (v135 < 0.0f)
                                v136 = v136 + (-1.0f / v135);
                            if (v136 < 1.0f)
                            {
                                if (v136 <= 0.0f)
                                    v136 = 0.0f;
                                goto LABEL_GRAD_V;
                            }
                        }

                        v136 = FLOAT_1_0;
                        if ((matFlag & 0x200) == 0)
                            goto LABEL_GRAD_DONE;

LABEL_GRAD_V:
                        v137 = (static_cast<float>(LoByte(static_cast<unsigned int>(MainMaterial[group.mtl_id].m_Layer[0].m_sGradientAlpha))) - 100.0f) / 25.0f;
                        v138 = uv[2 * v_id + 1] / v137;
                        if (v137 < 0.0f)
                            v138 = v138 + (-1.0f / v137);
                        if (v138 < 1.0f)
                        {
                            if (v138 <= 0.0f)
                                v138 = 0.0f;
                        }
                        else
                        {
LABEL_GRAD_DONE:
                            v138 = FLOAT_1_0;
                        }

                        float alpha = (v138 * v136) * (static_cast<float>(HiByte(m_ARGB)) / v68) * v68;
                        color = (static_cast<unsigned int>(alpha) << 24) | (m_ARGB & 0xFFFFFF);
                    }

                    static_vertexbp[static_v_cnt].___u0.x = temp[0];
                    static_vertexbp[static_v_cnt].y = temp[1];
                    static_vertexbp[static_v_cnt].z = temp[2];
                    static_vertexbp[static_v_cnt].uv[0][0] = uv[2 * v_id];
                    static_vertexbp[static_v_cnt].uv[0][1] = uv[2 * v_id + 1];

                    if (group.mtl_id == -1
                        || MainMaterial[group.mtl_id].m_iDetailSurface == -1
                        || (MainMaterial[group.mtl_id].m_dwFlag & 1) == 0
                        || group.lgt_id != -1)
                    {
                        static_vertexbp[static_v_cnt].uv[1][0] = static_cast<float>(lgtuv[2 * v_id]) / v66;
                        static_vertexbp[static_v_cnt].uv[1][1] = static_cast<float>(lgtuv[2 * v_id + 1]) / v66;
                    }
                    else
                    {
                        static_vertexbp[static_v_cnt].uv[1][0] = MainMaterial[group.mtl_id].m_fDetailScale * uv[2 * v_id] * v67;
                        static_vertexbp[static_v_cnt].uv[1][1] = MainMaterial[group.mtl_id].m_fDetailScale * uv[2 * v_id + 1] * v67;
                    }

                    if (group.lgt_id == -1)
                        static_vertexbp[static_v_cnt].color = (color & 0xFF000000) | (v_color[vertexId[v_id]] & 0x00FFFFFF);
                    else
                        static_vertexbp[static_v_cnt].color = color;

                    ++static_v_cnt;
                }

                int start_index = static_v_cnt - face[faceIdx].v_num;
                if (start_index < static_v_cnt)
                {
                    for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                    {
                        unsigned int v_id = face[faceIdx].v_start_id + k;
                        unsigned int c_v_id = real_v_index + vertexId[v_id];

                        float nx = v_normal[3 * c_v_id];
                        float ny = v_normal[3 * c_v_id + 1];
                        float nz = v_normal[3 * c_v_id + 2];
                        float len = sqrtf_0((nx * nx) + (ny * ny) + (nz * nz));
                        v_normal[3 * c_v_id] = nx / len;
                        v_normal[3 * c_v_id + 1] = ny / len;
                        v_normal[3 * c_v_id + 2] = nz / len;

                        static_vertexbp[start_index + k].___u3.nx = v_normal[3 * c_v_id];
                        static_vertexbp[start_index + k].ny = v_normal[3 * c_v_id + 1];
                        static_vertexbp[start_index + k].nz = v_normal[3 * c_v_id + 2];
                    }
                }
            }

            mMatGroup[i].VCnt = static_v_cnt - temp_v_num;
            mMatGroup[i].TriNum = static_cast<unsigned short>(static_v_index_cnt / 3 - temp_f_num);

            unsigned int mflag = 0;
            if (group.mtl_id != -1)
            {
                unsigned int layerNum = MainMaterial[group.mtl_id].m_dwLayerNum;
                for (unsigned int j = 0; j < layerNum; ++j)
                    mflag |= MainMaterial[group.mtl_id].m_Layer[j].m_dwFlag;
            }

            mMatGroup[i].MultiSourceUV = nullptr;
            if (mflag)
            {
                unsigned int metalFlag = mflag & 0xA;
                char *multiSTBytes = reinterpret_cast<char *>(mMultiLayerST);
                float *multiST = reinterpret_cast<float *>(mMultiLayerST);
                float *multiUV = reinterpret_cast<float *>(mMultiLayerUV);

                if (metalFlag)
                    mMatGroup[i].MultiSourceST = multiSTBytes + 12 * st_used;
                mMatGroup[i].MultiSourceUV = reinterpret_cast<unsigned int *>(mMultiLayerUV) + 2 * uv_used;

                for (unsigned int j = 0; j < group.face_num; ++j)
                {
                    unsigned int faceIdx = faceId[group.face_start_id + j];
                    if (!face[faceIdx].v_num)
                        continue;

                    if (metalFlag)
                    {
                        float tex2temp[9] = {};
                        int base_st = static_cast<int>(st_write);
                        for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                        {
                            unsigned int v_id = face[faceIdx].v_start_id + k;
                            GetVertexFromCompress[function_id](&tex2temp[3 * k], vertex + 3 * function_id * vertexId[v_id], &group);

                            float *dst = multiST + 9 * (base_st + static_cast<int>(k));
                            dst[0] = tex2temp[3 * k];
                            dst[1] = tex2temp[3 * k + 1];
                            dst[2] = tex2temp[3 * k + 2];

                            multiUV[2 * uv_write] = uv[2 * v_id];
                            multiUV[2 * uv_write + 1] = uv[2 * v_id + 1];
                            ++uv_write;
                            ++uv_used;
                        }

                        float v181 = (tex2temp[8] - tex2temp[2]) * (tex2temp[4] - tex2temp[1])
                                   - (tex2temp[7] - tex2temp[1]) * (tex2temp[5] - tex2temp[2]);
                        float v182 = (tex2temp[6] - tex2temp[0]) * (tex2temp[5] - tex2temp[2])
                                   - (tex2temp[8] - tex2temp[2]) * (tex2temp[3] - tex2temp[0]);
                        float v183 = (tex2temp[7] - tex2temp[1]) * (tex2temp[3] - tex2temp[0])
                                   - (tex2temp[6] - tex2temp[0]) * (tex2temp[4] - tex2temp[1]);
                        float v184 = sqrtf_0((v182 * v182) + (v181 * v181) + (v183 * v183));
                        float v185 = v183 / v184;
                        float v186 = v181 / v184;
                        float v187 = v182 / v184;

                        double v188 = std::fabs(static_cast<double>(v185));
                        float v189;
                        float v190;
                        float v191;
                        if (v188 <= 0.98)
                        {
                            float v192 = sqrtf_0(((0.0f - v187) * (0.0f - v187)) + (v186 * v186));
                            v189 = (0.0f - v187) / v192;
                            v190 = v186 / v192;
                            v191 = 0.0f / v192;
                        }
                        else
                        {
                            v189 = FLOAT_1_0;
                            v190 = 0.0f;
                            v191 = 0.0f;
                        }

                        for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                        {
                            float *dst = multiST + 9 * (base_st + static_cast<int>(k));
                            dst[3] = v189;
                            dst[4] = v190;
                            dst[5] = v191;
                            dst[6] = (v190 * v185) - (v191 * v187);
                            dst[7] = (v191 * v186) - (v189 * v185);
                            dst[8] = (v189 * v187) - (v190 * v186);
                        }

                        st_used += face[faceIdx].v_num;
                        st_write += face[faceIdx].v_num;
                    }
                    else
                    {
                        for (unsigned int k = 0; k < face[faceIdx].v_num; ++k)
                        {
                            unsigned int v_id = face[faceIdx].v_start_id + k;
                            multiUV[2 * uv_write] = uv[2 * v_id];
                            multiUV[2 * uv_write + 1] = uv[2 * v_id + 1];
                            ++uv_write;
                            ++uv_used;
                        }
                    }
                }
            }

            if (static_v_cnt == static_cast<int>(mVBVertexNum[static_vb_cnt]))
            {
                total_vertex_num = static_v_cnt + all_static_v_cnt;
                all_static_v_cnt += static_v_cnt;
                mStaticVertexBuffer[static_vb_cnt].VPUnLock();
                ++static_vb_cnt;
                static_vertexbp = reinterpret_cast<_D3DR3VERTEX_TEX2 *>(mStaticVertexBuffer[static_vb_cnt].VPLock(0, 0, 0));
                static_v_cnt = 0;
            }

            v66 = FLOAT_32767_0;
            v67 = FLOAT_8_0;
            v65 = FLOAT_0_5;
            v68 = FLOAT_255_0;
        }
    }

    mStaticVertexBuffer[static_vb_cnt].VPUnLock();
    mStaticIndexedBuffer.VPUnLock();

    mVertexNum = total_vertex_num;
    mFaceNum = static_v_index_cnt / 3;
    memcpy_0(mLgtUV, lgtuv, mBSPHeader.LgtUV.size);
    memcpy_0(mVertexColor, v_color, mBSPHeader.VertexColor.size);
    Dfree(v_normal);
    Dfree(read_alloc_buf);
}

void CBsp::MakeEdgeNormal()
{
    if (!mCNEdgeNormal)
    {
        Warning(aMakeedgenormal, byte_140883769);
        return;
    }

    for (unsigned int i = 0; i < mCFaceNum; ++i)
    {
        float mid[3] = {0.0f, 0.0f, 0.0f};
        for (unsigned int j = 0; j < mCFace[i].VNum; ++j)
        {
            float *v = mCVertex[mCVertexId[mCFace[i].VStartId + j]];
            mid[0] += v[0];
            mid[1] += v[1];
            mid[2] += v[2];
        }
        mid[0] /= mCFace[i].VNum;
        mid[1] /= mCFace[i].VNum;
        mid[2] /= mCFace[i].VNum;

        for (unsigned int j = 0; j < mCFace[i].VNum; ++j)
        {
            float *v0 = mCVertex[mCVertexId[mCFace[i].VStartId + j]];
            float *v1 = mCVertex[mCVertexId[mCFace[i].VStartId + (j + 1) % mCFace[i].VNum]];
            float l = v1[0] - v0[0];
            float m = v1[1] - v0[1];
            float n = v1[2] - v0[2];
            float len2 = (l * l) + (m * m) + (n * n);
            if (len2 == 0.0f)
            {
                mCNEdgeNormal[4 * i + j][0] = 0.0f;
                mCNEdgeNormal[4 * i + j][1] = 0.0f;
                mCNEdgeNormal[4 * i + j][2] = 1.0f;
                mCNEdgeNormal[4 * i + j][3] = 0.0f;
                continue;
            }
            float t = -((l * (v0[0] - mid[0]) + m * (v0[1] - mid[1]) + n * (v0[2] - mid[2])) / len2);
            float edge_n[3];
            edge_n[0] = mid[0] - (l * t + v0[0]);
            edge_n[1] = mid[1] - (m * t + v0[1]);
            edge_n[2] = mid[2] - (n * t + v0[2]);
            if (std::fabs(edge_n[0]) < 0.01f && std::fabs(edge_n[1]) < 0.01f && std::fabs(edge_n[2]) < 0.01f)
            {
                mCNEdgeNormal[4 * i + j][0] = 0.0f;
                mCNEdgeNormal[4 * i + j][1] = 0.0f;
                mCNEdgeNormal[4 * i + j][2] = 1.0f;
                mCNEdgeNormal[4 * i + j][3] = 0.0f;
                continue;
            }
            float len = sqrtf_0((edge_n[0] * edge_n[0]) + (edge_n[1] * edge_n[1]) + (edge_n[2] * edge_n[2]));
            edge_n[0] /= len;
            edge_n[1] /= len;
            edge_n[2] /= len;
            mCNEdgeNormal[4 * i + j][0] = edge_n[0];
            mCNEdgeNormal[4 * i + j][1] = edge_n[1];
            mCNEdgeNormal[4 * i + j][2] = edge_n[2];
            mCNEdgeNormal[4 * i + j][3] = (edge_n[0] * v1[0]) + (edge_n[1] * v1[1]) + (edge_n[2] * v1[2]);
        }
    }
}

void CBsp::LoadEntities(_READ_MAP_ENTITIES_LIST *a2)
{
    if (!mEntityListNum)
        return;

    mEntity = reinterpret_cast<CEntity *>(Dmalloc(244 * mEntityListNum));
    memset_0(mEntity, 0, 244LL * mEntityListNum);
    mParticle = reinterpret_cast<CParticle *>(Dmalloc(1168 * mEntityListNum));
    memset_0(mParticle, 0, 1168LL * mEntityListNum);
    mTotalAllocSize += 1412 * mEntityListNum;

    SetMergeFileManager(&mMapEntityMFM);

    unsigned int idx = 0;
    if (mEntityListNum)
    {
        __int64 listIdx = 0;
        __int64 entIdx = 0;
        do
        {
            char filePath[256] = {};
            __int64 pos = 0;
            do
            {
                char v = byte_184A790F0[pos];
                filePath[pos++] = v;
            } while (byte_184A790F0[pos - 1]);

            if (mEntityList[listIdx].Name[0] == '\\')
            {
                char *dst = &filePath[std::strlen(filePath)];
                __int64 j = 0;
                do
                {
                    char v = mEntityList[idx].Name[j + 1];
                    dst[j++] = v;
                } while (mEntityList[idx].Name[j]);
            }
            else
            {
                char *dst = &filePath[std::strlen(filePath)];
                __int64 j = 0;
                do
                {
                    char v = mEntityList[idx].Name[j];
                    dst[j++] = v;
                } while (mEntityList[idx].Name[j - 1]);
            }

            mEntityList[listIdx].IsFileExist = 0;
            if (!dword_184A797AC)
                mEntityList[listIdx].ShaderID = 0;

            if (mEntityList[listIdx].IsParticle)
            {
                if (mParticle[entIdx].LoadParticleSPT(filePath, 0))
                {
                    mEntityList[listIdx].IsFileExist = 1;
                    mParticle[entIdx].InitParticle();
                    mParticle[entIdx].SetParticleState(1u);
                }
                else
                {
                    Warning(filePath, aAaiai);
                }
            }
            else
            {
                unsigned int flags = 0;
                if (mEntityList[listIdx].ShaderID)
                    flags = 2;
                if (mEntity[entIdx].LoadEntity(filePath, flags | 0x20u))
                {
                    if (mEntityList[listIdx].Flag & 0x40)
                        mEntity[entIdx].mFlag |= 0x40u;
                    mEntity[entIdx].RestoreTexMem();
                    mEntityList[listIdx].IsFileExist = 1;
                }
            }

            ++idx;
            ++entIdx;
            ++listIdx;
        } while (idx < mEntityListNum);
    }

    unsigned int i = 0;
    if (mMapEntitiesListNum)
    {
        __int64 listIdx = 0;
        float *posPtr = &a2->Pos[1];
        do
        {
            if (mEntityList[*reinterpret_cast<unsigned __int16 *>(posPtr - 5)].IsFileExist)
            {
                mMapEntitiesList[listIdx].ID = *reinterpret_cast<unsigned __int16 *>(posPtr - 5);
                mMapEntitiesList[listIdx].Pos[0] = *(posPtr - 1);
                mMapEntitiesList[listIdx].Pos[1] = *posPtr;
                mMapEntitiesList[listIdx].Pos[2] = posPtr[1];
                mMapEntitiesList[listIdx].RotX = posPtr[2];
                mMapEntitiesList[listIdx].RotY = posPtr[3];
                mMapEntitiesList[listIdx].Scale = *(posPtr - 2);
                mMapEntitiesList[listIdx].BBMin[0] = *reinterpret_cast<unsigned __int16 *>(posPtr + 8);
                mMapEntitiesList[listIdx].BBMin[1] = *reinterpret_cast<unsigned __int16 *>(posPtr + 9);
                mMapEntitiesList[listIdx].BBMin[2] = *reinterpret_cast<unsigned __int16 *>(posPtr + 10);
                mMapEntitiesList[listIdx].BBMax[0] = *reinterpret_cast<unsigned __int16 *>(posPtr + 11);
                mMapEntitiesList[listIdx].BBMax[1] = *reinterpret_cast<unsigned __int16 *>(posPtr + 12);
                mMapEntitiesList[listIdx].BBMax[2] = *reinterpret_cast<unsigned __int16 *>(posPtr + 13);
                mMapEntitiesList[listIdx].AddFrame = static_cast<float>(rand() % 256) * 0.25f;
                mMapEntitiesList[listIdx].Particle = nullptr;
                if (mEntityList[mMapEntitiesList[listIdx].ID].IsParticle)
                {
                    CParticle *p = reinterpret_cast<CParticle *>(operator new(0x490uLL));
                    CParticle *inst = p ? new (p) CParticle() : nullptr;
                    mMapEntitiesList[listIdx].Particle = inst;
                    memcpy_0(inst, &mParticle[mMapEntitiesList[listIdx].ID], sizeof(CParticle));
                    inst->InitParticle();
                }
            }
            else
            {
                _MAP_ENTITIES_LIST *dst = &mMapEntitiesList[i];
                *reinterpret_cast<unsigned long long *>(&dst->ID) = 0;
                *reinterpret_cast<unsigned long long *>(reinterpret_cast<char *>(dst->Pos) + 2) = 0;
                *reinterpret_cast<unsigned long long *>(reinterpret_cast<char *>(&dst->Pos[2]) + 2) = 0;
                *reinterpret_cast<unsigned long long *>(reinterpret_cast<char *>(&dst->RotY) + 2) = 0;
                *reinterpret_cast<unsigned long long *>(dst->BBMax) = 0;
                *reinterpret_cast<unsigned long long *>(reinterpret_cast<char *>(&dst->AddFrame) + 2) = 0;
                *reinterpret_cast<CParticle **>(reinterpret_cast<char *>(&dst->Particle) + 6) = 0;
                reinterpret_cast<unsigned short *>(&dst->mMapColor)[1] = 0;
            }
            ++i;
            posPtr = reinterpret_cast<float *>(reinterpret_cast<char *>(posPtr) + 38);
            ++listIdx;
        } while (i < mMapEntitiesListNum);
    }
}

void CBsp::LoadSoundEntities(_READ_SOUND_ENTITY_LIST *a2, _READ_SOUND_ENTITIES_LIST *a3)
{
    if (!mSoundEntityListNum || !mSoundEntitiesListNum)
        return;

    mTotalWaveSize = 0;
    unsigned int i = 0;
    __int64 idx = 0;
    do
    {
        memcpy_0(mSoundEntityList[idx].Name, a2[idx].name, sizeof(a2[idx].name));

        char path[256] = {};
        __int64 p = 0;
        do
        {
            char v = byte_184A79270[p];
            path[p++] = v;
        } while (byte_184A79270[p - 1]);
        char *dst = &path[std::strlen(path)];
        __int64 j = 0;
        do
        {
            char v = a2[i].name[j++];
            dst[j - 1] = v;
        } while (a2[i].name[j - 1]);

        mSoundEntityList[idx].ID = IM_LoadWave(path, 1u);
        mTotalWaveSize += GetFileSize(path);
        ++i;
        ++idx;
    } while (i < mSoundEntityListNum);

    unsigned int n = 0;
    if (mSoundEntitiesListNum)
    {
        __int64 si = 0;
        unsigned int *p_flag = &a3->flag;
        do
        {
            mSoundEntitiesList[si].ID = *(reinterpret_cast<unsigned short *>(p_flag) - 2);
            if (mSoundEntitiesList[si].ID > mSoundEntityListNum - 1)
                mSoundEntitiesList[si].ID = static_cast<unsigned short>(mSoundEntityListNum) - 1;

            mSoundEntitiesList[si].Flag = *p_flag;
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].Scale) = p_flag[1];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].Attn) = p_flag[2];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].Pos[0]) = p_flag[3];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].Pos[1]) = p_flag[4];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].Pos[2]) = p_flag[5];
            mSoundEntitiesList[si].BBMin[0] = mSoundEntitiesList[si].Pos[0] - mSoundEntitiesList[si].Attn;
            mSoundEntitiesList[si].BBMin[1] = mSoundEntitiesList[si].Pos[1] - mSoundEntitiesList[si].Attn;
            mSoundEntitiesList[si].BBMin[2] = mSoundEntitiesList[si].Pos[2] - mSoundEntitiesList[si].Attn;
            mSoundEntitiesList[si].BBMax[0] = mSoundEntitiesList[si].Pos[0] + mSoundEntitiesList[si].Attn;
            mSoundEntitiesList[si].BBMax[1] = mSoundEntitiesList[si].Pos[1] + mSoundEntitiesList[si].Attn;
            mSoundEntitiesList[si].BBMax[2] = mSoundEntitiesList[si].Pos[2] + mSoundEntitiesList[si].Attn;
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].BoxAttn) = p_flag[9];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].BoxRotX) = p_flag[10];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].BoxRotY) = p_flag[11];
            mSoundEntitiesList[si].EventTime = *(reinterpret_cast<unsigned short *>(p_flag) - 1);
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].BoxScale[0]) = p_flag[6];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].BoxScale[1]) = p_flag[7];
            *reinterpret_cast<unsigned int *>(&mSoundEntitiesList[si].BoxScale[2]) = p_flag[8];
            mSoundEntitiesList[si].NextPlayTime = 0.0f;

            float matScale[16] = {};
            float matRot[16] = {};
            MatrixScale(reinterpret_cast<float (*)[4]>(matScale), mSoundEntitiesList[si].BoxScale[0],
                        mSoundEntitiesList[si].BoxScale[1], mSoundEntitiesList[si].BoxScale[2]);
            MatrixRotate(reinterpret_cast<float (*)[4]>(matRot), mSoundEntitiesList[si].BoxRotX,
                         mSoundEntitiesList[si].BoxRotY, 0.0f);
            MatrixMultiply(reinterpret_cast<float (*)[4]>(matScale), reinterpret_cast<float (*)[4]>(matRot),
                           reinterpret_cast<float (*)[4]>(matScale));
            matScale[12] = mSoundEntitiesList[si].Pos[0];
            matScale[13] = mSoundEntitiesList[si].Pos[1];
            matScale[14] = mSoundEntitiesList[si].Pos[2];
            MatrixInvert(mSoundEntitiesList[n++].mInvMat, reinterpret_cast<float (*)[4]>(matScale));

            p_flag += 14;
            ++si;
        } while (n < mSoundEntitiesListNum);
    }
}

void CBsp::ReadDynamicDataExtBsp(FILE *Stream)
{
    if (!IsServerMode())
    {
        auto *buf = reinterpret_cast<_READ_MAP_ENTITIES_LIST *>(Dmalloc(
            mExtBSPHeader.MapEntitiesList.size + mExtBSPHeader.SoundEntityList.size + mExtBSPHeader.SoundEntitiesList.size));
        fread(buf, mExtBSPHeader.MapEntitiesList.size, 1, Stream);
        auto *soundList = reinterpret_cast<_READ_SOUND_ENTITY_LIST *>(reinterpret_cast<char *>(buf) + mExtBSPHeader.MapEntitiesList.size);
        fread(soundList, mExtBSPHeader.SoundEntityList.size, 1, Stream);
        auto *soundEntities = reinterpret_cast<_READ_SOUND_ENTITIES_LIST *>(reinterpret_cast<char *>(buf) +
                                                                            mExtBSPHeader.MapEntitiesList.size +
                                                                            mExtBSPHeader.SoundEntityList.size);
        fread(soundEntities, mExtBSPHeader.SoundEntitiesList.size, 1, Stream);
        LoadEntities(buf);
        LoadSoundEntities(soundList, soundEntities);
        Dfree(buf);
    }
}
