#pragma once

#include <windows.h>

struct BONE_DATA
{
  DWORD dwID;
  char pPathName[128];
  char pFileName[64];
  char pBBoxName[64];
};

struct MESH_DATA
{
  DWORD dwID;
  DWORD dwBoneID;
  char pPathName[128];
  char pFileName[64];
  char pTexturePath[128];
};

struct ANI_DATA
{
  DWORD dwID;
  DWORD dwNum;
  char pPathName[128];
  char pFileName[64];
  DWORD dwEffectNum;
  DWORD dwEffectFrame[10];
};
