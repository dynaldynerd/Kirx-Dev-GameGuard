#include "pch.h"
#include "CLevel.h"
#include "CBsp.h"
#include "CSkyBox.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "R3EngineGlobals.h"

#include <cstdio>
#include <cstring>
#include <new>

CLevel::CLevel()
{
  mBsp = new (std::nothrow) CBsp();
  mSkyBox = new (std::nothrow) CSkyBox();
  mIsLoadedBsp = 0;
  mMapName[0] = 0;
  mLightTexMemSize = 0;
  mMapTexMemSize = 0;
  mSkyTexMemSize = 0;
  mEntityTexMemSize = 0;
}

CLevel::~CLevel()
{
  if (mSkyBox)
  {
    delete mSkyBox;
    mSkyBox = nullptr;
  }
  if (mBsp)
  {
    delete mBsp;
    mBsp = nullptr;
  }
}

__int64 CLevel::IsLoadedBsp()
{
  return mIsLoadedBsp;
}

float CLevel::GetFirstYpos(float *fCenterPos, float *fMin, float *fMax)
{
    if (this->mIsLoadedBsp && this->mBsp)
        return this->mBsp->GetFirstYpos(fCenterPos, fMin, fMax);
    return 0.0f;
}

void CLevel::LoadLevel(const char *szFileName)
{
  char v61[256]{};
  char v62[256]{};
  char v63[256]{};
  char v64[256]{};
  char Buffer[256]{};

  SetMergeFileManager(nullptr);
  if (!szFileName)
  {
    if (!mMapName[0])
      return;
    sprintf_s(Buffer, sizeof(Buffer), "%s\\%s\\%s.bsp", stState.MapPath, mMapName, mMapName);
    szFileName = Buffer;
  }

  FILE *fp = nullptr;
  if (fopen_s(&fp, szFileName, "rb") != 0 || !fp)
  {
    mBsp->mIsLoaded = 0;
    if (szFileName && *szFileName)
      Error(const_cast<char *>(szFileName), aAi_0);
    else
      Error(aA_1, byte_140883769);
    return;
  }
  fclose(fp);

  strcpy_s(v64, sizeof(v64), szFileName);
  StripEXT(v64);
  StripPath(v64);

  strcpy_s(v63, sizeof(v63), stState.MapPath);
  strcat_s(v63, sizeof(v63), "\\");
  strcat_s(v63, sizeof(v63), v64);
  strcat_s(v63, sizeof(v63), "\\");
  strcat_s(v63, sizeof(v63), v64);
  strcat_s(v63, sizeof(v63), ".bsp");

  ResetTexMemSize();
  dword_184A79824 = 0;
  if (IsServerMode())
  {
    strcpy_s(v62, sizeof(v62), v63);
    StripEXT(v62);
    strcat_s(v62, sizeof(v62), ".r3m");
    if (IsExistFile(v62))
      LoadMainR3M(v62);
  }
  else
  {
    strcpy_s(v62, sizeof(v62), v63);
    StripEXT(v62);
    strcat_s(v62, sizeof(v62), ".r3m");

    strcpy_s(v64, sizeof(v64), szFileName);
    StripEXT(v64);
    StripPath(v64);
    strcpy_s(mMapName, sizeof(mMapName), v64);

    if (IsExistFile(v62))
    {
      LoadMainR3M(v62);
      strcpy_s(v62, sizeof(v62), v63);
      StripEXT(v62);
      strcat_s(v62, sizeof(v62), ".r3t");
      R3Texture *tex = R3GetTexInfoR3T(v62, 0);
      SetNoLodTextere();
      LoadR3T(tex);
      mMapTexMemSize += GetNowTexMemSize();
    }
    else
    {
      strcpy_s(v62, sizeof(v62), stState.MapPath);
      strcat_s(v62, sizeof(v62), "\\");
      strcat_s(v62, sizeof(v62), v64);
      strcat_s(v62, sizeof(v62), "\\Tex\\");
      LoadMainMaterial(v62);
      SetNoLodTextere();
      mMapTexMemSize += GetNowTexMemSize();
    }
  }

  SetEnvironment(dword_184A79820);
  ResetTexMemSize();
  mBsp->LoadBsp(v63);

  strcpy_s(v61, sizeof(v61), v63);
  StripEXT(v61);
  strcat_s(v61, sizeof(v61), ".ebp");
  mBsp->LoadExtBsp(v61);
  mEntityTexMemSize += GetNowTexMemSize();

  if (!IsServerMode())
  {
    SetMergeFileManager(nullptr);
    strcpy_s(v61, sizeof(v61), v63);
    StripEXT(v61);
    strcat_s(v61, sizeof(v61), "Lgt.R3T");
    ResetTexMemSize();
    LoadLightMap(v61);
    mLightTexMemSize += GetNowTexMemSize();
    mBsp->CalcEntitiesMainColor();

    ResetTexMemSize();
    strcpy_s(v61, sizeof(v61), stState.MapPath);
    strcat_s(v61, sizeof(v61), "\\");
    strcat_s(v61, sizeof(v61), v64);
    strcat_s(v61, sizeof(v61), "\\sky2\\");
    strcat_s(v61, sizeof(v61), mMapName);
    strcat_s(v61, sizeof(v61), "sky2.r3e");
    if (IsExistFile(v61))
    {
      CN_SetEnableSky(1);
      mSkyBox->LoadSkyBox(v61);
    }
    else
    {
      CN_SetEnableSky(0);
      strcpy_s(v61, sizeof(v61), stState.MapPath);
      strcat_s(v61, sizeof(v61), "\\");
      strcat_s(v61, sizeof(v61), v64);
      strcat_s(v61, sizeof(v61), "\\sky\\");
      strcat_s(v61, sizeof(v61), mMapName);
      strcat_s(v61, sizeof(v61), "sky.r3e");
      if (IsExistFile(v61))
        mSkyBox->LoadSkyBox(v61);
    }
    mSkyTexMemSize += GetNowTexMemSize();

    ResetTexMemSize();
    R3RestoreAllTextures();
    mIsLoadedBsp = 1;
    mMapTexMemSize += GetNowTexMemSize();

    strcpy_s(v61, sizeof(v61), v63);
    StripEXT(v61);
    strcat_s(v61, sizeof(v61), ".rvp");
    RTMovieCreate(v61, this);
    RTMovieSetState(4u);

    strcpy_s(v61, sizeof(v61), v63);
    StripEXT(v61);
    strcat_s(v61, sizeof(v61), "EXT.spt");
    SetMergeFileManager(nullptr);
    mDummy.LoadExtDummy(v61);
    mBsp->mDummy = &mDummy;
  }

  R3EnvironmentShakeOff();
  mIsLoadedBsp = 1;
  ClearDynamicLight();
  memset_0(&gAsci, 0, 0x100u);
  mBsp->mIsLoaded = 1;
}

void CLevel::SetEnvironment(int nEnv)
{
    this->mEnvironment = nEnv;
}

unsigned int CLevel::GetEnvironment()
{
  return this->mEnvironment;
}

unsigned int CLevel::GetPath(
  float *const a2,
  float *const a3,
  float (*const a4)[3],
  unsigned int *a5,
  int a6)
{
  const float dx = *a2 - *a3;
  if (fabsf(dx) >= 0.1f)
  {
    return mBsp->GetPathFind(a2, a3, a4, a5, a6);
  }
  const float dz = a2[2] - a3[2];
  if (fabsf(dz) >= 0.1f)
  {
    return mBsp->GetPathFind(a2, a3, a4, a5, a6);
  }
  (*a4)[0] = *a2;
  (*a4)[1] = a2[1];
  (*a4)[2] = a2[2];
  *a5 = 0;
  return 0;
}

unsigned int CLevel::GetPathFromDepth(
  float *const a2,
  float *const a3,
  int a4,
  float (*const a5)[3],
  unsigned int *a6)
{
  const float dx = *a2 - *a3;
  if (fabsf(dx) >= 0.1f)
  {
    return mBsp->GetPathFind(a2, a3, a5, a6, a4);
  }
  const float dz = a2[2] - a3[2];
  if (fabsf(dz) >= 0.1f)
  {
    return mBsp->GetPathFind(a2, a3, a5, a6, a4);
  }
  (*a5)[0] = *a2;
  (*a5)[1] = a2[1];
  (*a5)[2] = a2[2];
  *a6 = 0;
  return 0;
}

__int64 CLevel::GetNextYposForServer(float *const a2, float *a3)
{
  CBsp *bsp = mBsp;
  a2[1] = a2[1] - FLOAT_23_0;
  const __int16 leaf0 = bsp->GetLeafNum(a2);
  const float baseY = a2[1];
  a2[1] = baseY + (FLOAT_23_0 * 2.0f);
  const __int16 leaf1 = bsp->GetLeafNum(a2);

  const float startX = *a2;
  const float startZ = a2[2];
  const int leafDiff = leaf0 != leaf1;
  const int leafCount = leafDiff + 1;
  float bestY = FLOAT_N100000_0;
  unsigned int found = 0;

  float start[3]{};
  float end[3]{};
  start[0] = startX;
  start[2] = startZ;
  end[0] = startX;
  end[2] = startZ;

  a2[1] = baseY;
  start[1] = baseY + FLOAT_9999_0;
  end[1] = baseY - FLOAT_9999_0;

  __int16 leafIds[2]{leaf0, leaf1};
  for (int index = 0; index < leafCount; ++index)
  {
    const float ypos = bsp->GetYposInLeafUseEdgeNormal(start, end, FLOAT_23_0, a2[1], leafIds[index]);
    if (ypos != FLOAT_N32000_0 && ypos > bestY)
    {
      found = 1;
      bestY = ypos;
    }
  }

  if (found)
  {
    *a3 = bestY;
  }
  return found;
}

__int64 CLevel::GetNextYposForServerFar(float *const a2, float *const a3, float *a4)
{
  if (*a2 == *a3 && a2[1] == a3[1] && a2[2] == a3[2])
  {
    *a4 = a2[1];
    return 1;
  }

  float src[3]{a2[0], a2[1], a2[2]};
  float dst[3]{a3[0], a3[1], a3[2]};

  __int16 leafIds[32000]{};
  while (true)
  {
    float bestY = FLOAT_N100000_0;
    float dist = GetDist(src, dst);

    if (dist > FLOAT_30_0)
    {
      const float dx = dst[0] - src[0];
      const float dy = dst[1] - src[1];
      const float dz = dst[2] - src[2];
      const float len = sqrtf_0((dx * dx) + (dy * dy) + (dz * dz));
      const float scale = FLOAT_25_0 / len;
      dst[0] = src[0] + (dx * scale);
      dst[1] = src[1] + (dy * scale);
      dst[2] = src[2] + (dz * scale);
      dist = FLOAT_30_0;
    }

    float ableHeight = dist * 1.73f;
    if (ableHeight < FLOAT_23_0)
    {
      ableHeight = FLOAT_23_0;
    }

    float start[3]{dst[0], dst[1] + ableHeight, dst[2]};
    float end[3]{dst[0], dst[1] - ableHeight, dst[2]};
    int leafCount = 0;
    mBsp->GetLeafList(start, end, &leafCount, leafIds, 0x7D00u);

    float bCamera[3]{dst[0], dst[1] + FLOAT_9999_0, dst[2]};
    float nCamera[3]{dst[0], dst[1] - FLOAT_9999_0, dst[2]};

    bool found = false;
    for (int i = 0; i < leafCount; ++i)
    {
      const float yHeight = mBsp->GetYposInLeaf(bCamera, nCamera, ableHeight, dst[1], leafIds[i]);
      if (yHeight != FLOAT_N32000_0 && yHeight > bestY)
      {
        found = true;
        bestY = yHeight;
      }
    }

    if (!found)
    {
      return 0;
    }

    if (dst[0] == a3[0] && dst[2] == a3[2])
    {
      *a4 = bestY;
      return 1;
    }

    src[0] = dst[0];
    src[1] = bestY;
    src[2] = dst[2];

    dst[0] = a3[0];
    dst[1] = bestY;
    dst[2] = a3[2];
  }
}

__int64 CLevel::GetNextYposFarProgress(float *const a2, float *const a3, float *a4)
{
  if (!mIsLoadedBsp)
  {
    return 0;
  }
  if (*a2 == *a3 && a2[1] == a3[1] && a2[2] == a3[2])
  {
    *a4 = a2[1];
    return 1;
  }

  a3[1] = a2[1];
  float src[3]{a2[0], a2[1], a2[2]};
  float dst[3]{a3[0], a3[1], a3[2]};
  float finalPos[3]{a3[0], a3[1], a3[2]};

  __int16 leafIds[10512]{};
  while (true)
  {
    float bestY = FLOAT_N100000_0;
    float dist = GetDist(src, dst);

    if (dist > FLOAT_25_0)
    {
      const float dx = dst[0] - src[0];
      const float dy = dst[1] - src[1];
      const float dz = dst[2] - src[2];
      const float len = sqrtf_0((dx * dx) + (dy * dy) + (dz * dz));
      const float scale = FLOAT_25_0 / len;
      dst[0] = src[0] + (dx * scale);
      dst[1] = src[1] + (dy * scale);
      dst[2] = src[2] + (dz * scale);
      dist = FLOAT_25_0;
    }

    float ableHeight = dist * 1.73f;
    if (ableHeight < FLOAT_23_0)
    {
      ableHeight = FLOAT_23_0;
    }

    float start[3]{dst[0], dst[1] + 30000.0f, dst[2]};
    float end[3]{dst[0], dst[1] - 30000.0f, dst[2]};
    int leafCount = 0;
    mBsp->GetLeafList(start, end, &leafCount, leafIds, 0x2710u);

    float bCamera[3]{dst[0], dst[1] + 30000.0f, dst[2]};
    float nCamera[3]{dst[0], dst[1] - 30000.0f, dst[2]};

    bool found = false;
    unsigned int bestLevel = 10000;
    for (int i = 0; i < leafCount; ++i)
    {
      for (int j = 0; j < 2; ++j)
      {
        const float yHeight = mBsp->GetBestYposInLeaf(bCamera, nCamera, ableHeight, dst[1], leafIds[i]);
        if (yHeight != FLOAT_N32000_0)
        {
          const unsigned int level = sub_1404E1570(dst[1], ableHeight, yHeight);
          if (bestLevel == level)
          {
            if (bestY < yHeight)
            {
              found = true;
              bestY = yHeight;
            }
          }
          else if (bestLevel > level)
          {
            bestLevel = level;
            found = true;
            bestY = yHeight;
          }
        }
        bCamera[0] += 0.5f;
        nCamera[0] += 0.5f;
      }
      bCamera[0] -= 1.0f;
      nCamera[0] -= 1.0f;
    }

    if (!found)
    {
      return 0;
    }

    if (dst[0] == finalPos[0] && dst[2] == finalPos[2])
    {
      *a4 = bestY;
      return 1;
    }

    src[0] = dst[0];
    src[1] = bestY;
    src[2] = dst[2];

    dst[0] = finalPos[0];
    dst[1] = bestY;
    dst[2] = finalPos[2];
  }
}
