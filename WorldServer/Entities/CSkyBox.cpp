#include "pch.h"

#include "WorldServerUtil.h"

#include "CSkyBox.h"
#include "CMainThread.h"
#include "R3EngineState.h"

#include <cstdlib>
#include <cstring>

CSkyBox::CSkyBox()
{
  mIsSkyLoad = 0;
}

CSkyBox::~CSkyBox()
{
}

__int64 CSkyBox::GetUsedVertexBufferSize()
{
  return mEntity.GetUsedVertexBufferSize();
}

void CSkyBox::ReleaseSkyBox()
{
  if (mIsSkyLoad)
  {
    mIsSkyLoad = 0;
    mEntity.ReleaseEntity();
  }
}

int GetMaterialNameNum(struct _R3MATERIAL *mat)
{
  char buffer[256];

  strcpy(buffer, "00000");
  memset(&buffer[6], '0', 3);
  memset_0(&buffer[9], 0, sizeof(buffer) - 9);

  int index = static_cast<int>(strlen(mat->m_name)) - 1;
  if (index >= 0)
  {
    char *out = &buffer[4];
    do
    {
      const char ch = mat->m_name[index];
      if (ch == '_')
      {
        break;
      }
      *out-- = ch;
      --index;
    }
    while (index >= 0);
  }

  return atoi(buffer);
}

void CSkyBox::LoadSkyBox(char *szFileName)
{
  int materialNameNums[514];

  SetMergeFileManager(nullptr);
  if (static_cast<unsigned int>(this->mEntity.LoadEntity(szFileName, 0)))
  {
    _R3MATERIAL *materials = this->mEntity.mMat;
    this->mIsSkyLoad = 1;
    int materialIndex = 0;
    const int materialCount = materials->m_iMatNum;
    this->mMatGroupNum = this->mEntity.mMatGroupNum;
    this->mMatGroup = this->mEntity.mMatGroup;
    this->mMatNum = materialCount;
    if (materialCount > 0)
    {
      int *materialNameCursor = materialNameNums;
      do
      {
        const __int16 materialNameNum = GetMaterialNameNum(&this->mEntity.mMat[materialIndex++]);
        *materialNameCursor = materialNameNum;
        ++materialNameCursor;
      }
      while (materialIndex < this->mMatNum);
    }
    for (unsigned int groupIndex = 0; groupIndex < this->mMatGroupNum; ++groupIndex)
      memset_0(this->mMatGroupSort, 0, 2LL * this->mMatGroupNum);

    __int64 sortIndex = 0;
    unsigned int materialNameIndex = 0;
    if (this->mMatNum)
    {
      int *materialNameIter = materialNameNums;
      do
      {
        unsigned int groupIter = 0;
        if (this->mMatGroupNum)
        {
          const int materialName = *materialNameIter;
          __int16 *sortCursor = &this->mMatGroupSort[sortIndex];
          do
          {
            if (materialName == this->mMatGroup[groupIter].MtlId)
            {
              *sortCursor = static_cast<__int16>(groupIter);
              ++sortIndex;
              ++sortCursor;
            }
            ++groupIter;
          }
          while (groupIter < this->mMatGroupNum);
        }
        ++materialNameIndex;
        ++materialNameIter;
      }
      while (materialNameIndex < this->mMatNum);
    }
  }
}

void CSkyBox::DrawSkyBox(const D3DXMATRIX *viewMatrix)
{
  mEntity.PrepareAnimation();
  IDirect3DDevice8 *const device = GetD3dDevice();
  if (!device || !viewMatrix)
    return;

  if (stState.mIsFog)
  {
    device->SetRenderState(device, D3DRS_FOGENABLE, 1u);
    device->SetRenderState(device, D3DRS_FOGCOLOR, stState.mFogColor);
    device->SetRenderState(device, D3DRS_FOGSTART, *reinterpret_cast<unsigned int *>(&stState.mFogStart));
    device->SetRenderState(device, D3DRS_FOGEND, *reinterpret_cast<unsigned int *>(&stState.mFogEnd));
    device->SetRenderState(device, D3DRS_FOGTABLEMODE, D3DFOG_NONE);
    device->SetRenderState(device, D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
    device->SetRenderState(device, D3DRS_RANGEFOGENABLE, stState.mFogRangeEnable);
  }
  else
  {
    device->SetRenderState(device, D3DRS_FOGENABLE, 0);
  }

  if (stState.mIsNoFogSky)
  {
    device->SetRenderState(device, D3DRS_FOGENABLE, 0);
  }

  CIndexBuffer *const indexBuffer = mEntity.GetStaticIndexedBuffer();
  if (!mIsSkyLoad || !indexBuffer || !indexBuffer->m_lpIndexBuffer)
  {
    device->SetTextureStageState(device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
    device->SetTransform(device, D3DTS_VIEW, reinterpret_cast<const _D3DMATRIX *>(viewMatrix));
    return;
  }

  D3DXMATRIX savedView = *viewMatrix;
  D3DXMATRIX skyView = *viewMatrix;
  skyView.m[3][0] = 0.0f;
  skyView.m[3][1] = 0.0f;
  skyView.m[3][2] = 0.0f;
  device->SetTransform(device, D3DTS_VIEW, reinterpret_cast<const _D3DMATRIX *>(&skyView));
  device->SetRenderState(device, D3DRS_ZENABLE, 0);
  device->SetRenderState(device, D3DRS_ZWRITEENABLE, 0);

  CVertexBuffer *const vertexBuffer = mEntity.GetStaticVertexBuffer();
  SetVPIPTex1(vertexBuffer->m_lpVertexBuffer, indexBuffer->m_lpIndexBuffer);
  MultiTexOff();
  device->SetTextureStageState(device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE2X);

  _ANI_OBJECT *const object = mEntity.GetObjectA();
  _ENTITY_M_GROUP *const matGroup = mEntity.GetMatGroup();
  for (int i = 0; i < mMatGroupNum; ++i)
  {
    float objectMatrix[4][4]{};
    const int sortedGroupIndex = mMatGroupSort[i];
    GetObjectMatrix(objectMatrix, matGroup[sortedGroupIndex].ObjectId, object, R3GetTime() * 30.0f);
    device->SetTransform(device, static_cast<_D3DTRANSFORMSTATETYPE>(256), reinterpret_cast<const _D3DMATRIX *>(objectMatrix));
    mEntity.DrawOneMatGroup(&matGroup[sortedGroupIndex]);
  }

  device->SetTextureStageState(device, 0, D3DTSS_COLOROP, D3DTOP_MODULATE);
  device->SetRenderState(device, D3DRS_ZENABLE, 1u);
  device->SetRenderState(device, D3DRS_ZWRITEENABLE, 1u);
  device->SetTransform(device, D3DTS_VIEW, reinterpret_cast<const _D3DMATRIX *>(&savedView));
}

