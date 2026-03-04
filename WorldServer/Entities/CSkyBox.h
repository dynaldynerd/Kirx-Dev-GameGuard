#pragma once

#include "IdaCompat.h"
#include "CEntity.h"

struct _R3MATERIAL;
struct _ENTITY_M_GROUP;
struct D3DXMATRIX;

int GetMaterialNameNum(struct _R3MATERIAL *a1);

/* 1424 */
class  CSkyBox
{
  int mIsSkyLoad;
  CEntity mEntity;
  _ENTITY_M_GROUP *mMatGroup;
  int mMatNum;
  int mMatGroupNum;
  __int16 mMatGroupSort[512];
public:
  CSkyBox();
  ~CSkyBox();

  void LoadSkyBox(char *szFileName);
  void DrawSkyBox(const D3DXMATRIX *viewMatrix);
  __int64 GetUsedVertexBufferSize();
  void ReleaseSkyBox();
};

