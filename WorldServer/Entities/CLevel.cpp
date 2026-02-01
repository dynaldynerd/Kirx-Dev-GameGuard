#include "pch.h"
#include "CLevel.h"
#include "CBsp.h"
#include "CSkyBox.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "R3EngineGlobals.h"

#include <cstdio>
#include <cstring>

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

  SetEnvironment(this, dword_184A79820);
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

void CLevel::SetEnvironment(CLevel *pLevel, int nEnv)
{
    pLevel->mEnvironment = nEnv;
}
