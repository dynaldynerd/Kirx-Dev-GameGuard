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

unsigned int CN_GetFieldColor();
void RTMovieFrameMove();
void RTMovieRender();
void RTMovieRelease();
void SetDefaultFlags();
void SetFrustumNormalPlane();
void SetMultiLayerTime(float a1);
void CN_NatureFrameMove();
void UpdateStreamMP3();
void DrawPrimitiveUPLine(unsigned int a1, void *a2);
int DrawFadeSKy();
void FramebufferToJPG(char *a1);
int IsRayAABB(float *const a1, float *const a2, float *const a3, float *const a4, float (*a5)[3]);
void SetReLoadState(int a1);
void ReLoadMaterial(char *a1, _R3MATERIAL *a2);
__int64 D3DXMatrixPerspectiveFovLH_0(void *a1);
__int64 D3DXMatrixInverse_0(D3DXMATRIX *a1, void *a2, const D3DXMATRIX *a3);
void GetFrustumNormalPlane(float (*const a1)[4]);
void R3GetQuakeVector(float *const a1);
void DrawTestBox(float a1, float a2, float a3, unsigned int a4);
extern unsigned int dword_184A79B2C[16];
extern D3DXMATRIX stru_184A79A6C;

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
  char workPath[256]{};
  char resourcePath[256]{};
  char mapBspPath[256]{};
  char mapStemName[256]{};
  char defaultBspPath[256]{};

  SetMergeFileManager(nullptr);
  if (!szFileName)
  {
    if (!mMapName[0])
      return;
    sprintf_s(defaultBspPath, sizeof(defaultBspPath), "%s\\%s\\%s.bsp", stState.MapPath, mMapName, mMapName);
    szFileName = defaultBspPath;
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

  strcpy_s(mapStemName, sizeof(mapStemName), szFileName);
  StripEXT(mapStemName);
  StripPath(mapStemName);

  strcpy_s(mapBspPath, sizeof(mapBspPath), stState.MapPath);
  strcat_s(mapBspPath, sizeof(mapBspPath), "\\");
  strcat_s(mapBspPath, sizeof(mapBspPath), mapStemName);
  strcat_s(mapBspPath, sizeof(mapBspPath), "\\");
  strcat_s(mapBspPath, sizeof(mapBspPath), mapStemName);
  strcat_s(mapBspPath, sizeof(mapBspPath), ".bsp");

  ResetTexMemSize();
  dword_184A79824 = 0;
  if (IsServerMode())
  {
    strcpy_s(resourcePath, sizeof(resourcePath), mapBspPath);
    StripEXT(resourcePath);
    strcat_s(resourcePath, sizeof(resourcePath), ".r3m");
    if (IsExistFile(resourcePath))
      LoadMainR3M(resourcePath);
  }
  else
  {
    strcpy_s(resourcePath, sizeof(resourcePath), mapBspPath);
    StripEXT(resourcePath);
    strcat_s(resourcePath, sizeof(resourcePath), ".r3m");

    strcpy_s(mapStemName, sizeof(mapStemName), szFileName);
    StripEXT(mapStemName);
    StripPath(mapStemName);
    strcpy_s(mMapName, sizeof(mMapName), mapStemName);

    if (IsExistFile(resourcePath))
    {
      LoadMainR3M(resourcePath);
      strcpy_s(resourcePath, sizeof(resourcePath), mapBspPath);
      StripEXT(resourcePath);
      strcat_s(resourcePath, sizeof(resourcePath), ".r3t");
      R3Texture *tex = R3GetTexInfoR3T(resourcePath, 0);
      SetNoLodTextere();
      LoadR3T(tex);
      mMapTexMemSize += GetNowTexMemSize();
    }
    else
    {
      strcpy_s(resourcePath, sizeof(resourcePath), stState.MapPath);
      strcat_s(resourcePath, sizeof(resourcePath), "\\");
      strcat_s(resourcePath, sizeof(resourcePath), mapStemName);
      strcat_s(resourcePath, sizeof(resourcePath), "\\Tex\\");
      LoadMainMaterial(resourcePath);
      SetNoLodTextere();
      mMapTexMemSize += GetNowTexMemSize();
    }
  }

  SetEnvironment(dword_184A79820);
  ResetTexMemSize();
  mBsp->LoadBsp(mapBspPath);

  strcpy_s(workPath, sizeof(workPath), mapBspPath);
  StripEXT(workPath);
  strcat_s(workPath, sizeof(workPath), ".ebp");
  mBsp->LoadExtBsp(workPath);
  mEntityTexMemSize += GetNowTexMemSize();

  if (!IsServerMode())
  {
    SetMergeFileManager(nullptr);
    strcpy_s(workPath, sizeof(workPath), mapBspPath);
    StripEXT(workPath);
    strcat_s(workPath, sizeof(workPath), "Lgt.R3T");
    ResetTexMemSize();
    LoadLightMap(workPath);
    mLightTexMemSize += GetNowTexMemSize();
    mBsp->CalcEntitiesMainColor();

    ResetTexMemSize();
    strcpy_s(workPath, sizeof(workPath), stState.MapPath);
    strcat_s(workPath, sizeof(workPath), "\\");
    strcat_s(workPath, sizeof(workPath), mapStemName);
    strcat_s(workPath, sizeof(workPath), "\\sky2\\");
    strcat_s(workPath, sizeof(workPath), mMapName);
    strcat_s(workPath, sizeof(workPath), "sky2.r3e");
    if (IsExistFile(workPath))
    {
      CN_SetEnableSky(1);
      mSkyBox->LoadSkyBox(workPath);
    }
    else
    {
      CN_SetEnableSky(0);
      strcpy_s(workPath, sizeof(workPath), stState.MapPath);
      strcat_s(workPath, sizeof(workPath), "\\");
      strcat_s(workPath, sizeof(workPath), mapStemName);
      strcat_s(workPath, sizeof(workPath), "\\sky\\");
      strcat_s(workPath, sizeof(workPath), mMapName);
      strcat_s(workPath, sizeof(workPath), "sky.r3e");
      if (IsExistFile(workPath))
        mSkyBox->LoadSkyBox(workPath);
    }
    mSkyTexMemSize += GetNowTexMemSize();

    ResetTexMemSize();
    R3RestoreAllTextures();
    mIsLoadedBsp = 1;
    mMapTexMemSize += GetNowTexMemSize();

    strcpy_s(workPath, sizeof(workPath), mapBspPath);
    StripEXT(workPath);
    strcat_s(workPath, sizeof(workPath), ".rvp");
    RTMovieCreate(workPath, this);
    RTMovieSetState(4u);

    strcpy_s(workPath, sizeof(workPath), mapBspPath);
    StripEXT(workPath);
    strcat_s(workPath, sizeof(workPath), "EXT.spt");
    SetMergeFileManager(nullptr);
    mDummy.LoadExtDummy(workPath);
    mBsp->mDummy = &mDummy;
  }

  R3EnvironmentShakeOff();
  mIsLoadedBsp = 1;
  ClearDynamicLight();
  std::memset(&gAsci, 0, 256);
  mBsp->mIsLoaded = 1;
}

void CLevel::SetEnvironment(int nEnv)
{
    this->mEnvironment = nEnv;
}

void CLevel::ReLoadAllMaterial()
{
  char materialName[256]{};
  SetReLoadState(1);

  char *src = mMapName;
  char *dst = materialName;
  while (true)
  {
    *dst = *src;
    if (*src == 0)
    {
      break;
    }
    ++src;
    ++dst;
  }

  _R3MATERIAL *mainMaterial = GetMainMaterial();
  ReLoadMaterial(materialName, mainMaterial);
  SetReLoadState(0);
}

void CLevel::SetCameraPos(float *const a2)
{
  mCamPos[0] = a2[0];
  mCamPos[1] = a2[1];
  mCamPos[2] = a2[2];
}

void CLevel::SetViewMatrix(D3DXMATRIX *a2)
{
  float quakeOffset[3]{};
  std::memcpy(&mMatView, a2, sizeof(mMatView));
  std::memcpy(&stru_184A79A6C, &mMatView, sizeof(stru_184A79A6C));
  R3GetQuakeVector(quakeOffset);

  const float offsetX = quakeOffset[0];
  const float offsetY = quakeOffset[1];
  const float offsetZ = quakeOffset[2];
  mMatView.m[3][0] += offsetX;
  mMatView.m[3][1] += offsetY;
  mMatView.m[3][2] += offsetZ;
  stru_184A79A6C.m[3][0] += offsetX;
  stru_184A79A6C.m[3][1] += offsetY;
  stru_184A79A6C.m[3][2] += offsetZ;
}

void CLevel::FrameMove()
{
  RTMovieFrameMove();

  D3DXMATRIX world{};
  std::memset(&world, 0, sizeof(world));
  world.m[0][0] = FLOAT_1_0;
  world.m[1][1] = FLOAT_1_0;
  world.m[2][2] = FLOAT_1_0;
  world.m[3][3] = FLOAT_1_0;

  IDirect3DDevice8 *device = GetD3dDevice();
  device->SetTransform(device, static_cast<_D3DTRANSFORMSTATETYPE>(256), reinterpret_cast<const _D3DMATRIX *>(&world));
  device->SetTransform(device, D3DTS_VIEW, reinterpret_cast<const _D3DMATRIX *>(&mMatView));

  SetDefaultFlags();
  SetFrustumNormalPlane();
  SetMultiLayerTime(R3GetTime());
  CN_NatureFrameMove();
  ClearDynamicLight();
  ResetVertexBufferCache();
  mBsp->ReadyBspRender(mCamPos);
  mBsp->FrameMoveEnvironment();
  mBsp->CalcObjectLoop();
  UpdateStreamMP3();
  CalcR3Fog();
}

void CLevel::DrawSkyBoxRender()
{
  D3DXMatrixPerspectiveFovLH_0(dword_184A79B2C);
  IDirect3DDevice8 *device = GetD3dDevice();
  device->SetTransform(device, D3DTS_PROJECTION, reinterpret_cast<const _D3DMATRIX *>(dword_184A79B2C));
  mSkyBox->DrawSkyBox(&mMatView);
  DrawFadeSKy();
  D3DXMatrixPerspectiveFovLH_0(dword_184A79B2C);
  IDirect3DDevice8 *device2 = GetD3dDevice();
  device2->SetTransform(device2, D3DTS_PROJECTION, reinterpret_cast<const _D3DMATRIX *>(dword_184A79B2C));
}

void CLevel::DrawMapRender()
{
  IDirect3DDevice8 *device = GetD3dDevice();
  device->SetRenderState(device, D3DRS_TEXTUREFACTOR, CN_GetFieldColor());
  mBsp->DrawBspRender();
  mBsp->DrawDynamicLights();
  RTMovieRender();
}

void CLevel::DrawMapEntitiesRender()
{
  mBsp->DrawMapEntitiesRender();
}

void CLevel::DrawMapAlphaRender(float *const a2)
{
  if (mIsLoadedBsp)
  {
    ResetVertexBufferCache();
    a2[0] = unk_184A79B1C;
    a2[1] = reinterpret_cast<float *>(&qword_184A79B20)[0];
    a2[2] = reinterpret_cast<float *>(&qword_184A79B20)[1];
    mBsp->DrawAlphaRender(a2);
  }
}

void CLevel::DrawLeafBBox()
{
  mBsp->DrawLeafBBox();
}

void CLevel::DrawMatBBox()
{
  mBsp->DrawMatBBox();
}

void CLevel::PrepareShadowRender(float *const a2, void *a3, float a4, unsigned int a5, float a6, float a7)
{
  mBsp->PrepareShadowRender(a2, a3, a4, a5, a6, a7);
}

void CLevel::DrawShadowRender(float *const a2, float *a3, float *a4)
{
  mBsp->DrawShadowRender(a2, a3, a4);
}

void CLevel::HearMapSound()
{
  mBsp->HearMapSound();
}

void CLevel::GetFrustumNormalPlane(float (*const a2)[4])
{
  ::GetFrustumNormalPlane(a2);
}

void CLevel::DrawBBox(float *const a2, float *const a3, int a4)
{
  D3DXMATRIX world{};
  std::memset(&world, 0, sizeof(world));
  world.m[0][0] = FLOAT_1_0;
  world.m[1][1] = FLOAT_1_0;
  world.m[2][2] = FLOAT_1_0;
  world.m[3][3] = FLOAT_1_0;

  IDirect3DDevice8 *device = GetD3dDevice();
  device->SetTransform(device, static_cast<_D3DTRANSFORMSTATETYPE>(256), reinterpret_cast<const _D3DMATRIX *>(&world));
  device->SetTexture(device, 0, nullptr);

  struct _LINE_VERTEX
  {
    float x;
    float y;
    float z;
    unsigned int color;
  };

  _LINE_VERTEX vertices[24]{};
  const unsigned int lineColor = static_cast<unsigned int>(a4 | 0xFF000000);
  int idx = 0;

  auto pushLine = [&](float x, float y, float z)
  {
    vertices[idx].x = x;
    vertices[idx].y = y;
    vertices[idx].z = z;
    vertices[idx].color = lineColor;
    ++idx;
  };

  pushLine(a2[0], a2[1], a2[2]);
  pushLine(a3[0], a2[1], a2[2]);
  pushLine(a2[0], a2[1], a2[2]);
  pushLine(a2[0], a2[1], a3[2]);
  pushLine(a3[0], a2[1], a2[2]);
  pushLine(a3[0], a2[1], a3[2]);
  pushLine(a2[0], a2[1], a3[2]);
  pushLine(a3[0], a2[1], a3[2]);

  pushLine(a2[0], a3[1], a2[2]);
  pushLine(a3[0], a3[1], a2[2]);
  pushLine(a2[0], a3[1], a2[2]);
  pushLine(a2[0], a3[1], a3[2]);
  pushLine(a3[0], a3[1], a2[2]);
  pushLine(a3[0], a3[1], a3[2]);
  pushLine(a2[0], a3[1], a3[2]);
  pushLine(a3[0], a3[1], a3[2]);

  pushLine(a2[0], a2[1], a2[2]);
  pushLine(a2[0], a3[1], a2[2]);
  pushLine(a3[0], a2[1], a2[2]);
  pushLine(a3[0], a3[1], a2[2]);
  pushLine(a2[0], a2[1], a3[2]);
  pushLine(a2[0], a3[1], a3[2]);
  pushLine(a3[0], a2[1], a3[2]);
  pushLine(a3[0], a3[1], a3[2]);

  DrawPrimitiveUPLine(12, vertices);
}

void CLevel::DrawBBox(__int16 *const a2, __int16 *const a3, int a4)
{
  float minPos[3]{};
  float maxPos[3]{};
  minPos[0] = static_cast<float>(a2[0]);
  minPos[1] = static_cast<float>(a2[1]);
  minPos[2] = static_cast<float>(a2[2]);
  maxPos[0] = static_cast<float>(a3[0]);
  maxPos[1] = static_cast<float>(a3[1]);
  maxPos[2] = static_cast<float>(a3[2]);
  DrawBBox(minPos, maxPos, a4);
}

void CLevel::DrawTestBox(float *const a2, float *const a3, unsigned int a4)
{
  D3DXMATRIX world{};
  std::memset(&world, 0, sizeof(world));
  world.m[0][0] = FLOAT_1_0;
  world.m[1][1] = FLOAT_1_0;
  world.m[2][2] = FLOAT_1_0;
  world.m[3][3] = FLOAT_1_0;
  world.m[3][0] = a3[0];
  world.m[3][1] = a3[1];
  world.m[3][2] = a3[2];

  IDirect3DDevice8 *device = GetD3dDevice();
  device->SetTransform(device, static_cast<_D3DTRANSFORMSTATETYPE>(256), reinterpret_cast<const _D3DMATRIX *>(&world));
  ::DrawTestBox(a2[0], a2[1], a2[2], a4);
}

void CLevel::ScreenShot()
{
  char buffer[256]{};
  for (int index = 0; index < 9999; ++index)
  {
    sprintf_s(buffer, sizeof(buffer), "%s%s%04d.jpg", stState.ScreenShotsPath, mMapName, index);
    if (!IsExistFile(buffer))
    {
      FramebufferToJPG(buffer);
      return;
    }
  }
}

char *CLevel::GetMapName()
{
  return mMapName;
}

__int64 CLevel::GetPointFromScreenRay(int a2, int a3, float (*a4)[3])
{
  float src[6]{};
  return mBsp->GetPointFromScreenRay(static_cast<float>(a2 - stState.mViewSx), static_cast<float>(a3 - stState.mViewSy), reinterpret_cast<float *const>(a4), src);
}

__int64 CLevel::GetPointFromScreenRayFar(int a2, int a3, float (*a4)[3])
{
  float src[6]{};
  return mBsp->GetPointFromScreenRayFar(static_cast<float>(a2 - stState.mViewSx), static_cast<float>(a3 - stState.mViewSy), reinterpret_cast<float *const>(a4), src);
}

__int64 CLevel::GetNextYpos(float *const a2, float *a3)
{
  float bestY = FLOAT_N100000_0;
  if (!mIsLoadedBsp)
  {
    return 0;
  }

  a2[1] -= FLOAT_23_0;
  const __int16 leafId0 = mBsp->GetLeafNum(a2);
  a2[1] += FLOAT_23_0 * 2.0f;
  const __int16 leafId1 = mBsp->GetLeafNum(a2);
  a2[1] -= FLOAT_23_0;

  const int leafCount = (leafId0 != leafId1) ? 2 : 1;
  __int16 leafIds[2]{leafId0, leafId1};
  float bCamera[3]{a2[0], a2[1] + 30000.0f, a2[2]};
  float nCamera[3]{a2[0], a2[1] - 30000.0f, a2[2]};

  unsigned int found = 0;
  for (int i = 0; i < leafCount; ++i)
  {
    const float yHeight = mBsp->GetYposInLeaf(bCamera, nCamera, FLOAT_23_0, a2[1], leafIds[i]);
    if (yHeight != FLOAT_N32000_0 && yHeight > bestY)
    {
      found = 1;
      bestY = yHeight;
    }
  }

  if (found)
  {
    *a3 = bestY;
  }
  return found;
}

__int64 CLevel::GetNextYposFar(float *const a2, float *const a3, float *a4)
{
  if (!mIsLoadedBsp)
  {
    return 0;
  }
  if (a2[0] == a3[0] && a2[1] == a3[1] && a2[2] == a3[2])
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
    if (dist > FLOAT_25_0)
    {
      float dir[3]{dst[0] - src[0], dst[1] - src[1], dst[2] - src[2]};
      Normalize(dir);
      dst[0] = src[0] + (dir[0] * FLOAT_25_0);
      dst[1] = src[1] + (dir[1] * FLOAT_25_0);
      dst[2] = src[2] + (dir[2] * FLOAT_25_0);
      dist = FLOAT_25_0;
    }

    float ableHeight = dist * 1.73f;
    if (ableHeight < FLOAT_23_0)
    {
      ableHeight = FLOAT_23_0;
    }

    float begin[3]{dst[0], dst[1] + ableHeight, dst[2]};
    float end[3]{dst[0], dst[1] - ableHeight, dst[2]};
    int leafCount = 0;
    mBsp->GetLeafList(begin, end, &leafCount, leafIds, 32000);

    float bCamera[3]{dst[0], dst[1] + FLOAT_9999_0, dst[2]};
    float nCamera[3]{dst[0], dst[1] - FLOAT_9999_0, dst[2]};

    unsigned int found = 0;
    for (int i = 0; i < leafCount; ++i)
    {
      const float yHeight = mBsp->GetYposInLeaf(bCamera, nCamera, ableHeight, dst[1], leafIds[i]);
      if (yHeight != FLOAT_N32000_0 && yHeight > bestY)
      {
        found = 1;
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

__int64 CLevel::GetNextYposNoAttr(float *const a2, float *a3)
{
  float bestY = FLOAT_N100000_0;
  if (!mIsLoadedBsp)
  {
    return 0;
  }

  a2[1] -= FLOAT_23_0;
  const __int16 leafId0 = mBsp->GetLeafNum(a2);
  a2[1] += FLOAT_23_0 * 2.0f;
  const __int16 leafId1 = mBsp->GetLeafNum(a2);
  a2[1] -= FLOAT_23_0;

  const int leafCount = (leafId0 != leafId1) ? 2 : 1;
  __int16 leafIds[2]{leafId0, leafId1};
  float bCamera[3]{a2[0], a2[1] + FLOAT_9999_0, a2[2]};
  float nCamera[3]{a2[0], a2[1] - FLOAT_9999_0, a2[2]};

  unsigned int found = 0;
  for (int i = 0; i < leafCount; ++i)
  {
    const float yHeight = mBsp->GetYposInLeafNoAttr(bCamera, nCamera, FLOAT_23_0, a2[1], leafIds[i]);
    if (yHeight != FLOAT_N32000_0 && yHeight > bestY)
    {
      found = 1;
      bestY = yHeight;
    }
  }

  if (found)
  {
    *a3 = bestY;
  }
  return found;
}

bool CLevel::IsCollisionRayAABB(int a2, int a3, float *const a4, float *const a5, float (*a6)[3])
{
  const float nx = ((((static_cast<float>(a2 - stState.mViewSx) * 2.0f) / static_cast<float>(stState.mViewXL)) - 1.0f) / stState.mMatProj[0][0]) * 3660000.0f;
  const float ny = -((((static_cast<float>(a3 - stState.mViewSy) * 2.0f) / static_cast<float>(stState.mViewYL)) - 1.0f) / stState.mMatProj[1][1]) * 3660000.0f;

  D3DXMATRIX inverseView{};
  D3DXMatrixInverse_0(&inverseView, nullptr, reinterpret_cast<const D3DXMATRIX *>(&stState.mMatView[0][0]));

  float at[3]{};
  float from[3]{};
  at[0] = (inverseView.m[1][0] * ny) + (inverseView.m[0][0] * nx) + (inverseView.m[2][0] * 3660000.0f);
  at[1] = (inverseView.m[1][1] * ny) + (inverseView.m[0][1] * nx) + (inverseView.m[2][1] * 3660000.0f);
  at[2] = (inverseView.m[1][2] * ny) + (inverseView.m[0][2] * nx) + (inverseView.m[2][2] * 3660000.0f);
  from[0] = inverseView.m[3][0];
  from[1] = inverseView.m[3][1];
  from[2] = inverseView.m[3][2];
  return IsRayAABB(a4, a5, from, at, a6) != 0;
}

void CLevel::ReleaseLevel()
{
  if (mBsp)
  {
    mBsp->ReleaseBsp();
  }

  mIsLoadedBsp = 0;
  if (IsServerMode())
  {
    ReleaseMainMaterial();
  }
  else
  {
    R3ReleaseAllTextures();
    ReleaseMainMaterial();
    ReleaseLightMap();
    R3ReleasePreTextures();

    if (mSkyBox)
    {
      mSkyBox->ReleaseSkyBox();
    }

    dword_184A798D0 = -1;
    mLightTexMemSize = 0;
    mMapTexMemSize = 0;
    mSkyTexMemSize = 0;
    mEntityTexMemSize = 0;
    RTMovieRelease();
    mDummy.ReleaseExtDummy();
  }
}

void CLevel::CalcR3Fog()
{
  unsigned int dummyCount = 0;
  unsigned int dummyIndices[10000]{};
  mDummy.GetDummyList(2, &dummyCount, dummyIndices);
  if (!dummyCount)
  {
    return;
  }

  unsigned int targetColor = dword_184A7984C;
  unsigned int targetNearBits = dword_184A79844;
  unsigned int targetFarBits = static_cast<unsigned int>(qword_184A79848);

  float cameraPos[3]{};
  cameraPos[0] = unk_184A79B1C;
  cameraPos[1] = reinterpret_cast<float *>(&qword_184A79B20)[0];
  cameraPos[2] = reinterpret_cast<float *>(&qword_184A79B20)[1];

  for (unsigned int i = 0; i < dummyCount; ++i)
  {
    if (!mDummy.IsInBBox(dummyIndices[i], cameraPos))
    {
      continue;
    }

    _EXT_DUMMY *dummy = mDummy.GetDummy(dummyIndices[i]);
    targetColor = dummy->___u5.mdwArgv[0];
    targetNearBits = dummy->___u5.mdwArgv[1];
    targetFarBits = dummy->___u5.mdwArgv[2];
    break;
  }

  if (dword_184A7981C != targetColor)
  {
    const float currentR = static_cast<float>((dword_184A7981C >> 16) & 0xFF);
    const float currentG = static_cast<float>((dword_184A7981C >> 8) & 0xFF);
    const float currentB = static_cast<float>(dword_184A7981C & 0xFF);

    const float targetR = static_cast<float>((targetColor >> 16) & 0xFF);
    const float targetG = static_cast<float>((targetColor >> 8) & 0xFF);
    const float targetB = static_cast<float>(targetColor & 0xFF);

    const float loopTime = R3GetLoopTime();
    const int newR = static_cast<int>((loopTime * (targetR - currentR)) + currentR);
    const int newG = static_cast<int>((loopTime * (targetG - currentG)) + currentG);
    const int newB = static_cast<int>((loopTime * (targetB - currentB)) + currentB);

    dword_184A7981C = ((newR & 0xFF) << 16) | ((newG & 0xFF) << 8) | (newB & 0xFF);
  }

  if (dword_184A79814 != targetNearBits)
  {
    float currentNear = 0.0f;
    float targetNear = 0.0f;
    memcpy(&currentNear, &dword_184A79814, sizeof(float));
    memcpy(&targetNear, &targetNearBits, sizeof(float));

    currentNear += R3GetLoopTime() * (targetNear - currentNear);
    if ((currentNear - targetNear) < 2.0f)
    {
      currentNear = targetNear;
    }

    memcpy(&dword_184A79814, &currentNear, sizeof(float));
  }

  unsigned int currentFarBits = static_cast<unsigned int>(qword_184A79818);
  if (currentFarBits != targetFarBits)
  {
    float currentFar = 0.0f;
    float targetFar = 0.0f;
    memcpy(&currentFar, &currentFarBits, sizeof(float));
    memcpy(&targetFar, &targetFarBits, sizeof(float));

    currentFar += R3GetLoopTime() * (targetFar - currentFar);
    unsigned int updatedFarBits = 0;
    memcpy(&updatedFarBits, &currentFar, sizeof(float));
    if ((currentFar - targetFar) < 2.0f)
    {
      updatedFarBits = targetFarBits;
      currentFar = targetFar;
    }

    qword_184A79818 = (qword_184A79818 & 0xFFFFFFFF00000000ull) | updatedFarBits;
    dword_184A79778 = updatedFarBits;
    D3DXMatrixPerspectiveFovLH_0(dword_184A79B2C);
    IDirect3DDevice8 *device = GetD3dDevice();
    device->SetTransform(device, D3DTS_PROJECTION, reinterpret_cast<const _D3DMATRIX *>(dword_184A79B2C));
  }
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
      const float len = std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
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
    mBsp->GetLeafList(start, end, &leafCount, leafIds, 32000);

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
      const float len = std::sqrt((dx * dx) + (dy * dy) + (dz * dz));
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
    mBsp->GetLeafList(start, end, &leafCount, leafIds, 10000);

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

