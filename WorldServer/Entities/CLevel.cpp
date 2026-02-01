#include "pch.h"
#include "CLevel.h"
#include "CBsp.h"
#include "CSkyBox.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"

#include <cstdio>
#include <cstring>

namespace
{
  const char kMissingMapMsg[] =
    "\xC1\xB8\xC0\xE7\xC7\xCF\xC1\xF6\xBE\xCA\xC0\xBA \xB8\xCA\xC0\xBB \xB7\xCE\xB5\xF9\xC7\xCF\xB7\xC1 \xC7\xCF\xB0\xED\xC0\xD6\xBD\xC0\xB4\xCF\xB4\xD9.";
  const char kMissingMapFileMsg[] = "<-\xC0\xCC \xB8\xCA\xC6\xC4\xC0\xCF\xC0\xCC \xBE\xF8\xBD\xC0\xB4\xCF\xB4\xD9.";
  const char kEmptyMsg[] = "";
}

float CLevel::GetFirstYpos(float *fCenterPos, float *fMin, float *fMax)
{
    if (this->mIsLoadedBsp && this->mBsp)
        return this->mBsp->GetFirstYpos(fCenterPos, fMin, fMax);
    return 0.0f;
}

void CLevel::LoadLevel(const char *szFileName)
{
  char buffer[256]{};
  char v61[256]{};
  char v62[256]{};
  char v63[256]{};
  char v64[256]{};

  SetMergeFileManager(nullptr);
  if (!szFileName)
  {
    if (!mMapName[0])
      return;
    std::sprintf(buffer, "%s\\%s\\%s.bsp", stState.MapPath, mMapName, mMapName);
    szFileName = buffer;
  }

  FILE *fp = std::fopen(szFileName, "rb");
  if (!fp)
  {
    mBsp->mIsLoaded = 0;
    if (szFileName && szFileName[0])
      Error(const_cast<char *>(szFileName), const_cast<char *>(kMissingMapFileMsg));
    else
      Error(const_cast<char *>(kMissingMapMsg), const_cast<char *>(kEmptyMsg));
    return;
  }
  std::fclose(fp);

  std::strcpy(v64, szFileName);
  StripEXT(v64);
  StripPath(v64);

  std::strcpy(v63, stState.MapPath);
  std::strcat(v63, "\\");
  std::strcat(v63, v64);
  std::strcat(v63, "\\");
  std::strcat(v63, v64);
  std::strcat(v63, ".bsp");

  ResetTexMemSize();
  dword_184A79824 = 0;
  if (IsServerMode())
  {
    std::strcpy(v62, v63);
    StripEXT(v62);
    std::strcat(v62, ".r3m");
    if (IsExistFile(v62))
      LoadMainR3M(v62);
  }
  else
  {
    std::strcpy(v62, v63);
    StripEXT(v62);
    std::strcat(v62, ".r3m");

    std::strcpy(v64, szFileName);
    StripEXT(v64);
    StripPath(v64);
    std::strcpy(mMapName, v64);

    if (IsExistFile(v62))
    {
      LoadMainR3M(v62);
      std::strcpy(v62, v63);
      StripEXT(v62);
      std::strcat(v62, ".r3t");
      R3Texture *tex = R3GetTexInfoR3T(v62, 0);
      SetNoLodTextere();
      LoadR3T(tex);
      mMapTexMemSize += GetNowTexMemSize();
    }
    else
    {
      std::strcpy(v62, stState.MapPath);
      std::strcat(v62, "\\");
      std::strcat(v62, v64);
      std::strcat(v62, "\\Tex\\");
      LoadMainMaterial(v62);
      SetNoLodTextere();
      mMapTexMemSize += GetNowTexMemSize();
    }
  }

  SetEnvironment(this, dword_184A79820);
  ResetTexMemSize();
  mBsp->LoadBsp(v63);

  std::strcpy(v61, v63);
  StripEXT(v61);
  std::strcat(v61, ".ebp");
  mBsp->LoadExtBsp(v61);
  mEntityTexMemSize += GetNowTexMemSize();

  if (!IsServerMode())
  {
    SetMergeFileManager(nullptr);
    std::strcpy(v61, v63);
    StripEXT(v61);
    std::strcat(v61, "Lgt.R3T");
    ResetTexMemSize();
    LoadLightMap(v61);
    mLightTexMemSize += GetNowTexMemSize();
    mBsp->CalcEntitiesMainColor();

    ResetTexMemSize();
    std::strcpy(v61, stState.MapPath);
    std::strcat(v61, "\\");
    std::strcat(v61, v64);
    std::strcat(v61, "\\sky2\\");
    std::strcat(v61, mMapName);
    std::strcat(v61, "sky2.r3e");
    if (IsExistFile(v61))
    {
      CN_SetEnableSky(1);
      mSkyBox->LoadSkyBox(v61);
    }
    else
    {
      CN_SetEnableSky(0);
      std::strcpy(v61, stState.MapPath);
      std::strcat(v61, "\\");
      std::strcat(v61, v64);
      std::strcat(v61, "\\sky\\");
      std::strcat(v61, mMapName);
      std::strcat(v61, "sky.r3e");
      if (IsExistFile(v61))
        mSkyBox->LoadSkyBox(v61);
    }
    mSkyTexMemSize += GetNowTexMemSize();

    ResetTexMemSize();
    R3RestoreAllTextures();
    const unsigned int now_tex_mem_size = GetNowTexMemSize();
    mIsLoadedBsp = 1;
    mMapTexMemSize += now_tex_mem_size;

    std::strcpy(v61, v63);
    StripEXT(v61);
    std::strcat(v61, ".rvp");
    RTMovieCreate(v61, this);
    RTMovieSetState(4u);

    std::strcpy(v61, v63);
    StripEXT(v61);
    std::strcat(v61, "EXT.spt");
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
