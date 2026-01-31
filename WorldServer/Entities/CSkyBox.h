#pragma once

#include "IdaCompat.h"

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

