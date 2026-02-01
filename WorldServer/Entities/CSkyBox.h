#pragma once

#include "IdaCompat.h"

struct _R3MATERIAL;

int GetMaterialNameNum(struct _R3MATERIAL *a1);

/* 1424 */
class __cppobj CSkyBox
{
  int mIsSkyLoad;
  CEntity mEntity;
  _ENTITY_M_GROUP *mMatGroup;
  int mMatNum;
  int mMatGroupNum;
  __int16 mMatGroupSort[512];
public:
  void LoadSkyBox(char *szFileName);
};

