#include "pch.h"

#include "CEntity.h"
#include "CMainThread.h"

#include <algorithm>
#include <cstring>

#include "WorldServerUtil.h"
#include "R3EngineGlobals.h"
#include "GlobalObjects.h"

namespace
{
unsigned __int8 GetPackedColorByte(unsigned int value, unsigned int shift)
{
  return static_cast<unsigned __int8>((value >> shift) & 0xFF);
}

float GetPackedColorFloat(unsigned int value, unsigned int shift)
{
  return static_cast<float>(GetPackedColorByte(value, shift)) / 255.0f;
}
} // namespace

CEntity::CEntity()
{
  mVetexBufferSize = 0;
  mIsEntityLoad = 0;
  mTextureSize = 0;
  mMat = nullptr;
  mFrame = 0.0f;
  mFlag = 0;
  mIndependencyTex = nullptr;
}

CEntity::~CEntity()
{
}

#pragma pack(push, 1)
struct _ENTITY_COMP_HEADER
{
  unsigned __int16 type;
  short bb_min[3];
  short bb_max[3];
  float pos[3];
  float scale;
  float uv_min;
  float uv_max;
};

struct _READ_ENTITY_M_GROUP
{
  unsigned __int16 face_num;
  unsigned int face_start_id;
  __int16 mtl_id;
  unsigned __int16 object_id;
  short bb_min[3];
  short bb_max[3];
};

struct _READ_ENTITY_FACE
{
  unsigned __int16 v_num;
  unsigned int v_start_id;
};
#pragma pack(pop)

static void ExtractVertex(
  __int16 type,
  int num,
  float (*outVertex)[3],
  char *compVertex,
  float *const pos,
  float scale)
{
  const __int64 count = num;
  if (type >= 0)
  {
    if ((type & 0x4000) != 0)
    {
      __int64 v16 = 0;
      if (count >= 4)
      {
        float *dst = &(*outVertex)[2];
        char *src = compVertex + 4;
        unsigned __int64 blocks = ((count - 4) >> 2) + 1;
        v16 = 4 * blocks;
        do
        {
          int v = *reinterpret_cast<short *>(src - 4);
          dst += 12;
          src += 24;
          --blocks;
          *(dst - 14) = (static_cast<float>(v) / 32767.0f) * scale + pos[0];
          *(dst - 13) = (static_cast<float>(*reinterpret_cast<short *>(src - 26)) / 32767.0f) * scale + pos[1];
          *(dst - 12) = (static_cast<float>(*reinterpret_cast<short *>(src - 24)) / 32767.0f) * scale + pos[2];
          *(dst - 11) = (static_cast<float>(*reinterpret_cast<short *>(src - 22)) / 32767.0f) * scale + pos[0];
          *(dst - 10) = (static_cast<float>(*reinterpret_cast<short *>(src - 20)) / 32767.0f) * scale + pos[1];
          *(dst - 9) = (static_cast<float>(*reinterpret_cast<short *>(src - 18)) / 32767.0f) * scale + pos[2];
          *(dst - 8) = (static_cast<float>(*reinterpret_cast<short *>(src - 16)) / 32767.0f) * scale + pos[0];
          *(dst - 7) = (static_cast<float>(*reinterpret_cast<short *>(src - 14)) / 32767.0f) * scale + pos[1];
          *(dst - 6) = (static_cast<float>(*reinterpret_cast<short *>(src - 12)) / 32767.0f) * scale + pos[2];
          *(dst - 5) = (static_cast<float>(*reinterpret_cast<short *>(src - 10)) / 32767.0f) * scale + pos[0];
          *(dst - 4) = (static_cast<float>(*reinterpret_cast<short *>(src - 8)) / 32767.0f) * scale + pos[1];
          *(dst - 3) = (static_cast<float>(*reinterpret_cast<short *>(src - 6)) / 32767.0f) * scale + pos[2];
        } while (blocks);
      }
      if (v16 < count)
      {
        __int64 remain = count - v16;
        float *dst = &(*outVertex)[3 * v16 + 2];
        char *src = &compVertex[6 * v16 + 4];
        do
        {
          int v = *reinterpret_cast<short *>(src - 4);
          dst += 3;
          src += 6;
          --remain;
          *(dst - 5) = (static_cast<float>(v) / 32767.0f) * scale + pos[0];
          *(dst - 4) = (static_cast<float>(*reinterpret_cast<short *>(src - 8)) / 32767.0f) * scale + pos[1];
          *(dst - 3) = (static_cast<float>(*reinterpret_cast<short *>(src - 6)) / 32767.0f) * scale + pos[2];
        } while (remain);
      }
    }
    else
    {
      memcpy_0(outVertex, compVertex, 12LL * num);
    }
  }
  else
  {
    __int64 v7 = 0;
    if (count >= 4)
    {
      float *dst = &(*outVertex)[2];
      char *src = compVertex + 2;
      unsigned __int64 blocks = ((count - 4) >> 2) + 1;
      v7 = 4 * blocks;
      do
      {
        int v = *(src - 2);
        dst += 12;
        src += 12;
        --blocks;
        *(dst - 14) = (static_cast<float>(v) / 127.0f) * scale + pos[0];
        *(dst - 13) = (static_cast<float>(*(src - 13)) / 127.0f) * scale + pos[1];
        *(dst - 12) = (static_cast<float>(*(src - 12)) / 127.0f) * scale + pos[2];
        *(dst - 11) = (static_cast<float>(*(src - 11)) / 127.0f) * scale + pos[0];
        *(dst - 10) = (static_cast<float>(*(src - 10)) / 127.0f) * scale + pos[1];
        *(dst - 9) = (static_cast<float>(*(src - 9)) / 127.0f) * scale + pos[2];
        *(dst - 8) = (static_cast<float>(*(src - 8)) / 127.0f) * scale + pos[0];
        *(dst - 7) = (static_cast<float>(*(src - 7)) / 127.0f) * scale + pos[1];
        *(dst - 6) = (static_cast<float>(*(src - 6)) / 127.0f) * scale + pos[2];
        *(dst - 5) = (static_cast<float>(*(src - 5)) / 127.0f) * scale + pos[0];
        *(dst - 4) = (static_cast<float>(*(src - 4)) / 127.0f) * scale + pos[1];
        *(dst - 3) = (static_cast<float>(*(src - 3)) / 127.0f) * scale + pos[2];
      } while (blocks);
    }
    if (v7 < count)
    {
      __int64 remain = count - v7;
      float *dst = &(*outVertex)[3 * v7 + 2];
      char *src = &compVertex[3 * v7 + 2];
      do
      {
        int v = *(src - 2);
        dst += 3;
        src += 3;
        --remain;
        *(dst - 5) = (static_cast<float>(v) / 127.0f) * scale + pos[0];
        *(dst - 4) = (static_cast<float>(*(src - 4)) / 127.0f) * scale + pos[1];
        *(dst - 3) = (static_cast<float>(*(src - 3)) / 127.0f) * scale + pos[2];
      } while (remain);
    }
  }
}

__int64 CEntity::LoadEntity(char *a2, unsigned int a3)
{
  mMapColor = 0xFF808080;
  mStartTime = 0.0f;
  mFlag = a3;

  _strlwr(a2);
  FILE *fp = fopenMFM(a2, "rb");
  if (!fp)
  {
    Warning(a2, aAiAaiai_1);
    return 0;
  }

  unsigned int header[2]{};
  fread(header, sizeof(header), 1, fp);
  if (static_cast<float>(header[0]) != 113.0f)
  {
    Warning(a2, aAai);
    fclose(fp);
    return 0;
  }

  fread(&mHeader, sizeof(_ENTITY_FILE_HEADER), 1, fp);

  unsigned int loadOption = 2;
  if (mFlag & 1)
  {
    loadOption = 3;
    mIsUseMemory = 1;
  }

  mStartTexID = static_cast<int>(GetNowR3D3DTexCnt());
  int orgR3TexNum = static_cast<int>(GetNowR3TexCnt());
  if (mFlag & 8)
    SetR3TexManageFlag(1);

  unsigned int oldTexSize = GetNowTexMemSize();

  char r3mPath[256]{};
  strcpy_s(r3mPath, a2);
  StripEXT(r3mPath);
  strcat_s(r3mPath, ".r3m");
  if (IsExistFileAndMergeFile(r3mPath))
  {
    mMat = LoadSubR3M(r3mPath);
    StripEXT(r3mPath);
    strcat_s(r3mPath, ".r3t");
    R3Texture *r3tex = R3GetTexInfoR3T(r3mPath, 0);
    if (r3tex)
    {
      if (r3tex->mSameCnt)
      {
        AdjustIndependenceR3M(mMat, mStartTexID, r3tex->mStartID);
        mStartTexID = r3tex->mStartID;
        mTexNum = r3tex->mTexNum;
      }
      else
      {
        LoadR3T(r3tex);
        mTexNum = static_cast<int>(GetNowR3D3DTexCnt()) - mStartTexID;
      }
    }
  }
  else
  {
    char matPath[256]{};
    strcpy_s(matPath, a2);
    StripName(matPath);
    mMat = LoadSubMaterial(matPath);
    if (!mMat)
    {
      fclose(fp);
      return 0;
    }
    mTexNum = static_cast<int>(GetNowR3D3DTexCnt()) - mStartTexID;
  }

  if (mFlag & 8)
  {
    for (int i = 0; i < mTexNum; ++i)
      R3LoadTextureMem(i + mStartTexID);
    auto *ppTex = static_cast<IDirect3DTexture8 **>(Dmalloc(mTexNum * sizeof(IDirect3DTexture8 *)));
    for (int i = 0; i < mTexNum; ++i)
      ppTex[i] = R3GetSurface(mStartTexID + i);
    mIndependencyTex = ppTex;
    SetNowR3D3DTexCnt(mStartTexID);
    SetNowR3TexCnt(orgR3TexNum);
    SetR3TexManageFlag(0);
  }

  mTextureSize += GetNowTexMemSize() - oldTexSize;
  mIsEntityLoad = 1;

  unsigned int bufSize = 0;
  bufSize += mHeader.CompHeader.size;
  bufSize += mHeader.Vertex.size;
  bufSize += mHeader.VColor.size;
  bufSize += mHeader.UV.size;
  bufSize += mHeader.Face.size;
  bufSize += mHeader.FaceId.size;
  bufSize += mHeader.VertexId.size;
  bufSize += mHeader.MatGroup.size;
  bufSize += mHeader.Object.size;
  bufSize += mHeader.Track.size;

  auto *loadBuf = static_cast<unsigned char *>(Dmalloc(static_cast<int>(bufSize)));
  unsigned int offset = 0;

  auto *compHeader = reinterpret_cast<_ENTITY_COMP_HEADER *>(loadBuf + offset);
  offset += mHeader.CompHeader.size;
  auto *compVertex = reinterpret_cast<char *>(loadBuf + offset);
  offset += mHeader.Vertex.size;
  auto *color = reinterpret_cast<unsigned int *>(loadBuf + offset);
  offset += mHeader.VColor.size;
  auto *uv = reinterpret_cast<short (*)[2]>(loadBuf + offset);
  offset += mHeader.UV.size;
  auto *entityFace = reinterpret_cast<_READ_ENTITY_FACE *>(loadBuf + offset);
  offset += mHeader.Face.size;
  auto *fId = reinterpret_cast<unsigned __int16 *>(loadBuf + offset);
  offset += mHeader.FaceId.size;
  auto *vId = reinterpret_cast<unsigned __int16 *>(loadBuf + offset);
  offset += mHeader.VertexId.size;
  auto *readMatGroup = reinterpret_cast<_READ_ENTITY_M_GROUP *>(loadBuf + offset);
  offset += mHeader.MatGroup.size;
  auto *readObject = reinterpret_cast<_READ_ANI_OBJECT *>(loadBuf + offset);
  offset += mHeader.Object.size;
  auto *track = reinterpret_cast<unsigned __int8 *>(loadBuf + offset);
  offset += mHeader.Track.size;

  fread(compHeader, mHeader.CompHeader.size, 1, fp);
  fread(compVertex, mHeader.Vertex.size, 1, fp);
  fread(color, mHeader.VColor.size, 1, fp);
  fread(uv, mHeader.UV.size, 1, fp);
  fread(entityFace, mHeader.Face.size, 1, fp);
  fread(fId, mHeader.FaceId.size, 1, fp);
  fread(vId, mHeader.VertexId.size, 1, fp);
  fread(readMatGroup, mHeader.MatGroup.size, 1, fp);
  fread(readObject, mHeader.Object.size, 1, fp);
  fread(track, mHeader.Track.size, 1, fp);
  fclose(fp);

  unsigned int vertexNum = 0;
  if (compHeader->type == 0x8000)
    vertexNum = mHeader.Vertex.size / 3;
  else if (compHeader->type == 0x4000)
    vertexNum = mHeader.Vertex.size / 6;
  else
    vertexNum = mHeader.Vertex.size / 12;

  auto *vertex = static_cast<float *>(Dmalloc(12 * vertexNum));
  ExtractVertex(compHeader->type, static_cast<int>(vertexNum), reinterpret_cast<float (*)[3]>(vertex), compVertex, compHeader->pos, compHeader->scale);

  int staticTriNum = 0;
  int staticVertNum = 0;
  int matGroupNum = static_cast<int>(mHeader.MatGroup.size / sizeof(_READ_ENTITY_M_GROUP));
  for (int i = 0; i < matGroupNum; ++i)
  {
    for (int j = 0; j < readMatGroup[i].face_num; ++j)
    {
      unsigned int faceId = fId[readMatGroup[i].face_start_id + j];
      staticTriNum += entityFace[faceId].v_num - 2;
      staticVertNum += entityFace[faceId].v_num;
    }
  }

  mVetexBufferSize = staticVertNum * sizeof(_D3DR3VERTEX_TEX1);
  mVetexBufferSize += staticTriNum * 3 * sizeof(unsigned __int16);
  mMatGroupNum = matGroupNum;
  mObjectNum = static_cast<int>(mHeader.Object.size / sizeof(_READ_ANI_OBJECT));

  mMatGroup = reinterpret_cast<_ENTITY_M_GROUP *>(
    Dmalloc(mHeader.Track.size + sizeof(_ENTITY_M_GROUP) * mMatGroupNum + sizeof(_ANI_OBJECT) * mObjectNum + 8 * staticVertNum));
  mObject = reinterpret_cast<_ANI_OBJECT *>(mMatGroup + mMatGroupNum);
  mTrack = reinterpret_cast<unsigned char *>(mObject) + sizeof(_ANI_OBJECT) * mObjectNum;
  mOrgUV = reinterpret_cast<float (*)[2]>(mTrack + mHeader.Track.size);

  memcpy_0(mTrack, track, mHeader.Track.size);
  ConvAniObject(mObjectNum, mTrack, readObject, mObject);

  if (mFlag & 2)
    mStaticVertexBuffer.InitVertexBuffer(staticVertNum * sizeof(_D3DR3VERTEX_TEX1), loadOption, 0);
  else
    mStaticVertexBuffer.InitVertexBuffer(staticVertNum * sizeof(_D3DR3VERTEX_TEX1), loadOption, 0x152u);
  mStaticIndexedBuffer.InitIndexBuffer(staticTriNum * 3 * sizeof(unsigned __int16), loadOption);

  auto *staticVertex = reinterpret_cast<_D3DR3VERTEX_TEX1 *>(mStaticVertexBuffer.VPLock());
  auto *staticIndex = reinterpret_cast<unsigned __int16 *>(mStaticIndexedBuffer.VPLock());
  if (!staticVertex || !staticIndex)
  {
    ReleaseEntity();
    Dfree(vertex);
    Dfree(loadBuf);
    mIsEntityLoad = 0;
    return 1;
  }

  std::vector<float> normalAcc(staticVertNum * 3, 0.0f);

  // Accumulate normals
  for (int i = 0; i < matGroupNum; ++i)
  {
    for (int j = 0; j < readMatGroup[i].face_num; ++j)
    {
      unsigned int faceId = fId[readMatGroup[i].face_start_id + j];
      unsigned int vStart = entityFace[faceId].v_start_id;
      for (int k = 0; k < entityFace[faceId].v_num - 2; ++k)
      {
        unsigned int idx0 = vId[vStart];
        unsigned int idx1 = vId[vStart + k + 1];
        unsigned int idx2 = vId[vStart + k + 2];
        float normal[3]{};
        GetNormal(
          normal,
          &vertex[3 * idx0],
          &vertex[3 * idx1],
          &vertex[3 * idx2]);
        normalAcc[3 * idx0] += normal[0];
        normalAcc[3 * idx0 + 1] += normal[1];
        normalAcc[3 * idx0 + 2] += normal[2];
        normalAcc[3 * idx1] += normal[0];
        normalAcc[3 * idx1 + 1] += normal[1];
        normalAcc[3 * idx1 + 2] += normal[2];
        normalAcc[3 * idx2] += normal[0];
        normalAcc[3 * idx2 + 1] += normal[1];
        normalAcc[3 * idx2 + 2] += normal[2];
      }
    }
  }

  int staticIndexCnt = 0;
  unsigned __int16 staticVertCnt = 0;
  float uvScale = (compHeader->uv_max - compHeader->uv_min) * 0.5f;
  float uvPos = compHeader->uv_min + uvScale;

  mBBMin[0] = 67000.0f;
  mBBMin[1] = 67000.0f;
  mBBMin[2] = 67000.0f;
  mBBMax[0] = -67000.0f;
  mBBMax[1] = -67000.0f;
  mBBMax[2] = -67000.0f;
  mIsAlpha = 0;

  for (int i = 0; i < mMatGroupNum; ++i)
  {
    mMatGroup[i].BBMin[0] = static_cast<float>(readMatGroup[i].bb_min[0]);
    mMatGroup[i].BBMin[1] = static_cast<float>(readMatGroup[i].bb_min[1]);
    mMatGroup[i].BBMin[2] = static_cast<float>(readMatGroup[i].bb_min[2]);
    mMatGroup[i].BBMax[0] = static_cast<float>(readMatGroup[i].bb_max[0]);
    mMatGroup[i].BBMax[1] = static_cast<float>(readMatGroup[i].bb_max[1]);
    mMatGroup[i].BBMax[2] = static_cast<float>(readMatGroup[i].bb_max[2]);
    mMatGroup[i].MtlId = readMatGroup[i].mtl_id;
    mMatGroup[i].MultiSourceUV = &mOrgUV[staticVertCnt];
    mMatGroup[i].ObjectId = readMatGroup[i].object_id;

    mBBMin[0] = (std::min)(mBBMin[0], mMatGroup[i].BBMin[0]);
    mBBMin[1] = (std::min)(mBBMin[1], mMatGroup[i].BBMin[1]);
    mBBMin[2] = (std::min)(mBBMin[2], mMatGroup[i].BBMin[2]);
    mBBMax[0] = (std::max)(mBBMax[0], mMatGroup[i].BBMax[0]);
    mBBMax[1] = (std::max)(mBBMax[1], mMatGroup[i].BBMax[1]);
    mBBMax[2] = (std::max)(mBBMax[2], mMatGroup[i].BBMax[2]);

    mMatGroup[i].Type = 1;
    unsigned int alpha = 0xFF000000;
    bool lightEnable = false;
    if (mMatGroup[i].MtlId != -1)
    {
      _R3MATERIAL *mat = &mMat[mMatGroup[i].MtlId];
      if (mat->m_Layer[0].m_dwAlphaType)
      {
        mMatGroup[i].Type |= 0x8000u;
        mIsAlpha = 1;
      }
      alpha = mat->m_Layer[0].m_ARGB & 0xFF000000;
      lightEnable = (mat->m_dwFlag & 1) != 0;
      if (alpha != 0xFF000000)
        mIsAlpha = 1;
    }

    mMatGroup[i].VBMinIndex = staticVertCnt;
    mMatGroup[i].IBMinIndex = staticIndexCnt;
    int tempFaceNum = staticIndexCnt / 3;
    int tempVertNum = staticVertCnt;

    for (int j = 0; j < readMatGroup[i].face_num; ++j)
    {
      unsigned int faceId = fId[readMatGroup[i].face_start_id + j];
      for (int k = 0; k < entityFace[faceId].v_num - 2; ++k)
      {
        staticIndex[staticIndexCnt++] = staticVertCnt;
        staticIndex[staticIndexCnt++] = staticVertCnt + k + 1;
        staticIndex[staticIndexCnt++] = staticVertCnt + k + 2;
      }

      for (int k = 0; k < entityFace[faceId].v_num; ++k)
      {
        unsigned int vrId = entityFace[faceId].v_start_id + k;
        unsigned int vid = vId[vrId];

        staticVertex[staticVertCnt].x = vertex[3 * vid];
        staticVertex[staticVertCnt].y = vertex[3 * vid + 1];
        staticVertex[staticVertCnt].z = vertex[3 * vid + 2];

        float nx = normalAcc[3 * vid];
        float ny = normalAcc[3 * vid + 1];
        float nz = normalAcc[3 * vid + 2];
        float len = sqrtf_0(nx * nx + ny * ny + nz * nz);
        if (len != 0.0f)
        {
          nx /= len;
          ny /= len;
          nz /= len;
        }
        staticVertex[staticVertCnt].nx = nx;
        staticVertex[staticVertCnt].ny = ny;
        staticVertex[staticVertCnt].nz = nz;

        mOrgUV[staticVertCnt][0] = (uv[vrId][0] / 32767.0f) * uvScale + uvPos;
        mOrgUV[staticVertCnt][1] = (uv[vrId][1] / 32767.0f) * uvScale + uvPos;
        staticVertex[staticVertCnt].u = mOrgUV[staticVertCnt][0];
        staticVertex[staticVertCnt].v = mOrgUV[staticVertCnt][1];

        if (lightEnable)
          staticVertex[staticVertCnt].color = 0xFF808080;
        else
          staticVertex[staticVertCnt].color = color[vid];
        staticVertex[staticVertCnt].color |= alpha;
        ++staticVertCnt;
      }

      mMatGroup[i].VCnt = staticVertCnt - tempVertNum;
      mMatGroup[i].TriNum = staticIndexCnt / 3 - tempFaceNum;
    }
  }

  mStaticVertexBuffer.VPUnLock();
  mStaticIndexedBuffer.VPUnLock();
  Dfree(vertex);
  Dfree(loadBuf);
  return 1;
}

void CEntity::ReleaseEntity()
{
  if (!mIsEntityLoad)
    return;

  mIsEntityLoad = 0;
  mVetexBufferSize = 0;
  mTextureSize = 0;
  mObjectNum = 0;

  mStaticVertexBuffer.ReleaseVertexBuffer();
  mStaticIndexedBuffer.ReleaseIndexBuffer();

  if (mMatGroup)
  {
    Dfree(mMatGroup);
    mMatGroup = nullptr;
  }
  if (mMat)
  {
    ReleaseSubMaterial(mMat);
    mMat = nullptr;
  }
  if (mIndependencyTex)
  {
    if (mFlag & 8)
      ReleaseTexMem();
    mIndependencyTex = nullptr;
  }
}

void CEntity::ReleaseTexMem()
{
  if (mFlag & 8)
  {
    auto *texList = reinterpret_cast<unsigned long long *>(mIndependencyTex);
    for (int i = 0; i < mTexNum; ++i)
    {
      if (texList[i])
      {
        auto *tex = reinterpret_cast<IDirect3DTexture8 *>(texList[i]);
        if (tex && tex->__vftable && tex->__vftable->Release)
          tex->__vftable->Release(reinterpret_cast<IDA_IUnknown *>(tex));
      }
    }
    Dfree(mIndependencyTex);
    mIndependencyTex = nullptr;
  }
  else
  {
    for (int j = 0; j < mTexNum; ++j)
      R3ReleaseTextureMem(j + mStartTexID);
  }
}

void CEntity::RestoreTexMem()
{
  if ((mFlag & 8) == 0)
  {
    int old = GetNowTexMemSize();
    for (int i = 0; i < mTexNum; ++i)
      R3LoadTextureMem(i + mStartTexID);
    mTextureSize += GetNowTexMemSize() - old;
  }
}

void CEntity::PrepareAnimation()
{
  if (mObject)
  {
    int idx = 0;
    if (mObjectNum > 0)
    {
      __int64 objIdx = 0;
      do
      {
        ++idx;
        mObject[objIdx++].AniFrameCache = 0;
      } while (idx < mObjectNum);
    }
  }
}

__int64 CEntity::GetUsedVertexBufferSize()
{
  return mVetexBufferSize;
}

void CEntity::GetAnimationMatrix(float (*const a2)[4], _ANI_OBJECT *a3, double a4)
{
  GetAniMatrix(a2, a3, a4);
}

void CEntity::AddFlag(int a2)
{
  mFlag |= a2;
}

_ENTITY_M_GROUP *CEntity::GetMatGroup()
{
  return mMatGroup;
}

__int64 CEntity::GetMatGroupNum()
{
  return static_cast<unsigned int>(mMatGroupNum);
}

__int64 CEntity::GetMatNum()
{
  return mMat->m_iMatNum;
}

_ANI_OBJECT *CEntity::GetObjectA()
{
  return mObject;
}

CIndexBuffer *CEntity::GetStaticIndexedBuffer()
{
  return &mStaticIndexedBuffer;
}

CVertexBuffer *CEntity::GetStaticVertexBuffer()
{
  return &mStaticVertexBuffer;
}

__int64 CEntity::IsAlpha()
{
  return static_cast<unsigned int>(mIsAlpha);
}

bool CEntity::IsEnableShaderID(int a2)
{
  if (!a2 || (a2 != 1 && a2 != 2))
    return false;
  if (mFlag & 0x40)
    return IsEnableShader(1u) != 0;
  return IsEnableShader(0) != 0;
}

bool CEntity::IsFirstAlpha()
{
  return mMatGroupNum > 0 && mMat != nullptr && mMat->m_Layer[0].m_dwAlphaType != 0;
}

void CEntity::SetMapColor(unsigned int a2)
{
  mMapColor = a2;
}

void CEntity::DrawOneMatGroup(_ENTITY_M_GROUP *a2)
{
  IDirect3DDevice8 *const d3dDevice = GetD3dDevice();
  _R3MATERIAL *const mat = &mMat[a2->MtlId];

  if (a2->MtlId == -1)
  {
    d3dDevice->SetTexture(d3dDevice, 0, nullptr);
    d3dDevice->DrawIndexedPrimitive(d3dDevice, D3DPT_TRIANGLELIST, a2->VBMinIndex, a2->VCnt, a2->IBMinIndex, a2->TriNum);
    return;
  }

  for (unsigned int layerIndex = 0; layerIndex < mat->m_dwLayerNum; ++layerIndex)
  {
    _ONE_LAYER &layer = mat->m_Layer[layerIndex];
    D3DXMATRIX texMat{};
    const bool hasTextureMatrix = GetTextureMatrix(mat, layerIndex, &texMat, mStartTime) != 0;
    if (hasTextureMatrix)
    {
      d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, 2u);
      d3dDevice->SetTransform(d3dDevice, D3DTS_TEXTURE0, reinterpret_cast<const _D3DMATRIX *>(&texMat));
    }

    if (layer.m_dwFlag & 4)
    {
      auto *const locked = mStaticVertexBuffer.VPLock(36 * a2->VBMinIndex, 36 * a2->VCnt, 0);
      MakeUV(reinterpret_cast<_D3DR3VERTEX_TEX1 *>(locked), a2->VCnt, reinterpret_cast<unsigned int *>(a2->MultiSourceUV), mat, layerIndex);
      mStaticVertexBuffer.VPUnLock();
    }

    if (mFlag & 8)
    {
      auto **const independentTextures = reinterpret_cast<IDirect3DBaseTexture8 **>(mIndependencyTex);
      d3dDevice->SetTexture(d3dDevice, 0, independentTextures[layer.m_iSurface - mStartTexID]);
    }
    else if (layer.m_dwFlag & 0x1000)
    {
      const int tileOffset = GetTileAniTextureAddId(mat, layerIndex, mStartTime);
      d3dDevice->SetTexture(d3dDevice, 0, R3GetSurface(layer.m_iSurface + tileOffset));
    }
    else
    {
      d3dDevice->SetTexture(d3dDevice, 0, R3GetSurface(layer.m_iSurface));
    }

    if (layer.m_dwAlphaType)
      BlendOn(layer.m_dwAlphaType);

    d3dDevice->DrawIndexedPrimitive(d3dDevice, D3DPT_TRIANGLELIST, a2->VBMinIndex, a2->VCnt, a2->IBMinIndex, a2->TriNum);

    if (hasTextureMatrix)
      d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_TEXTURETRANSFORMFLAGS, 24u);

    if (layer.m_dwFlag & 0x400)
    {
      d3dDevice->SetRenderState(d3dDevice, D3DRS_LIGHTING, 0);
      d3dDevice->SetRenderState(d3dDevice, D3DRS_DIFFUSEMATERIALSOURCE, 1u);
      d3dDevice->SetRenderState(d3dDevice, D3DRS_ALPHATESTENABLE, 1u);
    }

    if (layer.m_dwAlphaType)
      BlendOff();
  }
}

void CEntity::DrawOneMatGroupVS(_ENTITY_M_GROUP *a2)
{
  IDirect3DDevice8 *const d3dDevice = GetD3dDevice();
  _R3MATERIAL *const mat = &mMat[a2->MtlId];

  if (a2->MtlId == -1)
  {
    d3dDevice->SetTexture(d3dDevice, 0, nullptr);
    d3dDevice->DrawIndexedPrimitive(d3dDevice, D3DPT_TRIANGLELIST, a2->VBMinIndex, a2->VCnt, a2->IBMinIndex, a2->TriNum);
    return;
  }

  for (unsigned int layerIndex = 0; layerIndex < mat->m_dwLayerNum; ++layerIndex)
  {
    _ONE_LAYER &layer = mat->m_Layer[layerIndex];
    if (mFlag & 8)
    {
      auto **const independentTextures = reinterpret_cast<IDirect3DBaseTexture8 **>(mIndependencyTex);
      d3dDevice->SetTexture(d3dDevice, 0, independentTextures[layer.m_iSurface - mStartTexID]);
    }
    else if (layer.m_dwFlag & 0x1000)
    {
      const int tileOffset = GetTileAniTextureAddId(mat, layerIndex, 0.0);
      d3dDevice->SetTexture(d3dDevice, 0, R3GetSurface(layer.m_iSurface + tileOffset));
    }
    else
    {
      d3dDevice->SetTexture(d3dDevice, 0, R3GetSurface(layer.m_iSurface));
    }

    if (layer.m_dwAlphaType)
      BlendOn(layer.m_dwAlphaType);

    d3dDevice->DrawIndexedPrimitive(d3dDevice, D3DPT_TRIANGLELIST, a2->VBMinIndex, a2->VCnt, a2->IBMinIndex, a2->TriNum);

    if (layer.m_dwAlphaType)
      BlendOff();
  }
}

void CEntity::SetMaterialAndLight(unsigned int a2)
{
  D3DMATERIAL8 material{};
  IDirect3DDevice8 *const d3dDevice = GetD3dDevice();

  if (mFlag & 0x40)
  {
    d3dDevice->LightEnable(d3dDevice, 0, 1);
    MultiTexOff();

    D3DLIGHT8 light{};
    const unsigned int dayColor = CN_MixDayColor();
    GetMatLightFromColor(&light, &material, dayColor, 1.0f);

    light.Direction.x = -light.Direction.x;
    light.Direction.y = -light.Direction.y;
    light.Direction.z = -light.Direction.z;

    material.Ambient.r += 0.1f;
    material.Ambient.g += 0.1f;
    material.Ambient.b += 0.1f;

    material.Diffuse.a = GetPackedColorFloat(a2, 24);
    material.Diffuse.r += 0.25f;
    material.Diffuse.g += 0.2f;
    material.Diffuse.b += 0.2f;

    if (material.Diffuse.r > 1.0f)
      material.Diffuse.r = 1.0f;
    if (material.Diffuse.g > 1.0f)
      material.Diffuse.g = 1.0f;
    if (material.Diffuse.b > 1.0f)
      material.Diffuse.b = 1.0f;

    if (material.Ambient.r > 1.0f)
      material.Ambient.r = 1.0f;
    if (material.Ambient.g > 1.0f)
      material.Ambient.g = 1.0f;
    if (material.Ambient.b > 1.0f)
      material.Ambient.b = 1.0f;

    d3dDevice->SetLight(d3dDevice, 0, &light);
    d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_COLORARG1, 4u);
    d3dDevice->SetRenderState(d3dDevice, D3DRS_COLORVERTEX, 0);

    material.Emissive.a = GetPackedColorFloat(a2, 24);
    material.Emissive.r = 0.0f;
    material.Emissive.g = 0.0f;
    material.Emissive.b = 0.0f;
    d3dDevice->SetRenderState(d3dDevice, D3DRS_EMISSIVEMATERIALSOURCE, 0);
    d3dDevice->LightEnable(d3dDevice, 0, 1);
  }
  else
  {
    const float alpha = GetPackedColorFloat(a2, 24);
    material.Diffuse.a = alpha;
    const float diffuseRed = GetPackedColorFloat(a2, 16);
    const float diffuseGreen = GetPackedColorFloat(a2, 8);
    const float diffuseBlue = GetPackedColorFloat(a2, 0);
    material.Diffuse.r = diffuseRed / diffuseRed;
    material.Diffuse.g = diffuseGreen / diffuseGreen;
    material.Diffuse.b = diffuseBlue / diffuseBlue;

    if ((a2 & 0xFFFFFF) == 0xFFFFFF)
    {
      d3dDevice->SetRenderState(d3dDevice, D3DRS_EMISSIVEMATERIALSOURCE, 1u);
      d3dDevice->LightEnable(d3dDevice, 0, 0);
    }
    else
    {
      material.Emissive.a = alpha;
      material.Emissive.r = GetPackedColorFloat(a2, 16);
      material.Emissive.g = GetPackedColorFloat(a2, 8);
      material.Emissive.b = GetPackedColorFloat(a2, 0);
      d3dDevice->SetRenderState(d3dDevice, D3DRS_EMISSIVEMATERIALSOURCE, 0);
      d3dDevice->LightEnable(d3dDevice, 0, 1);
    }

    d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_COLORARG1, 5u);
    d3dDevice->SetRenderState(d3dDevice, D3DRS_COLORVERTEX, 1u);

    if (mFlag & 0x20)
    {
      d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_COLORARG2, 3u);
      d3dDevice->SetRenderState(d3dDevice, D3DRS_TEXTUREFACTOR, CN_MixDayColor());
      d3dDevice->SetTextureStageState(d3dDevice, 1, D3DTSS_TEXCOORDINDEX, 1u);
      d3dDevice->SetTextureStageState(d3dDevice, 1, D3DTSS_COLORARG1, 2u);
      d3dDevice->SetTextureStageState(d3dDevice, 1, D3DTSS_COLORARG2, 1u);
      d3dDevice->SetTextureStageState(d3dDevice, 1, D3DTSS_COLOROP, 5u);
    }
    else
    {
      MultiTexOff();
    }
  }

  d3dDevice->SetRenderState(d3dDevice, D3DRS_LIGHTING, 1u);
  d3dDevice->SetMaterial(d3dDevice, &material);
  d3dDevice->SetRenderState(d3dDevice, D3DRS_DIFFUSEMATERIALSOURCE, 0);
}

void CEntity::SetVertexShaderID(_ENTITY_LIST *a2, float (*const a3)[4], unsigned int a4)
{
  const float alpha = static_cast<float>(HIBYTE(a4)) / 255.0f;
  if ((a2->ShaderID - 1) > 1)
    return;

  if (mFlag & 0x40)
  {
    float invWorld[20]{};
    MatrixInvert(reinterpret_cast<float (*)[4]>(invWorld), a3);
    invWorld[12] = 0.0f;
    invWorld[13] = 0.0f;
    invWorld[14] = 0.0f;

    float transformedDir[3]{};
    Vector3fTransform(transformedDir, stState.mMainLightNomal, reinterpret_cast<float (*)[4]>(invWorld));

    const float dirLength =
      sqrtf_0((transformedDir[0] * transformedDir[0]) + (transformedDir[1] * transformedDir[1]) + (transformedDir[2] * transformedDir[2]));
    transformedDir[0] /= dirLength;
    transformedDir[1] /= dirLength;
    transformedDir[2] /= dirLength;

    SetLitGrassVS(mAddFrame, a2->Factor, alpha, CN_MixDayColor(), transformedDir);
  }
  else
  {
    SetGrassVS(mAddFrame, a2->Factor, alpha, CN_MixDayColor());
  }
}

__int64 CEntity::DrawEntity(float (*const a2)[4], unsigned int a3, double a4)
{
  double aniFrame = a4;
  CEntity::PrepareAnimation();
  IDirect3DDevice8 *const d3dDevice = GetD3dDevice();
  if (!mIsEntityLoad || !mStaticVertexBuffer.m_lpVertexBuffer)
    return 0;

  if (mFlag & 0x10)
    d3dDevice->SetRenderState(d3dDevice, D3DRS_ZENABLE, 0);

  CEntity::SetMaterialAndLight(a3);
  SetVPIPTex1(mStaticVertexBuffer.m_lpVertexBuffer, mStaticIndexedBuffer.m_lpIndexBuffer);

  if (static_cast<float>(a4) == -1.0f)
    aniFrame = static_cast<float>(R3GetTime() + mAddFrame) * 30.0f;

  for (int i = 0; i < mMatGroupNum; ++i)
  {
    float objectMatrix[16]{};
    GetObjectMatrix(reinterpret_cast<float (*)[4]>(objectMatrix), mMatGroup[i].ObjectId, mObject, aniFrame);
    if (SLOBYTE(mFlag) < 0)
    {
      float billboard[16]{};
      memcpy_0(billboard, &dword_184A79AEC, sizeof(billboard));
      billboard[12] = 0.0f;
      billboard[13] = 0.0f;
      billboard[14] = 0.0f;
      MatrixMultiply(reinterpret_cast<float (*)[4]>(objectMatrix), reinterpret_cast<float (*)[4]>(billboard), reinterpret_cast<float (*)[4]>(objectMatrix));
    }
    MatrixMultiply(reinterpret_cast<float (*)[4]>(objectMatrix), a2, reinterpret_cast<float (*)[4]>(objectMatrix));
    d3dDevice->SetTransform(d3dDevice, static_cast<_D3DTRANSFORMSTATETYPE>(256), reinterpret_cast<const _D3DMATRIX *>(objectMatrix));

    if (GetPackedColorFloat(a3, 24) != 1.0f)
      BlendOn(13);

    const unsigned int alphaType = mMat[mMatGroup[i].MtlId].m_Layer[0].m_dwAlphaType;
    if (alphaType)
    {
      if (alphaType & 0x2000)
        d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 1u);
      else
        d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 0);
      if (dword_184A79808)
        d3dDevice->SetRenderState(d3dDevice, D3DRS_FOGENABLE, 0);
    }
    else
    {
      if (dword_184A79808)
        d3dDevice->SetRenderState(d3dDevice, D3DRS_FOGENABLE, 1u);
      d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 1u);
    }

    CEntity::DrawOneMatGroup(&mMatGroup[i]);
    d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 1u);
  }

  if (mFlag & 0x20)
    d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_COLORARG2, 3u);

  BlendOff();
  d3dDevice->SetRenderState(d3dDevice, D3DRS_ALPHABLENDENABLE, 0);
  d3dDevice->SetRenderState(d3dDevice, D3DRS_LIGHTING, 0);
  d3dDevice->SetRenderState(d3dDevice, D3DRS_DIFFUSEMATERIALSOURCE, 1u);
  if ((mFlag & 0x40) == 0 && (a3 & 0xFFFFFF) == 0xFFFFFF)
    d3dDevice->SetRenderState(d3dDevice, D3DRS_EMISSIVEMATERIALSOURCE, 0);
  if (mFlag & 0x10)
    d3dDevice->SetRenderState(d3dDevice, D3DRS_ZENABLE, 1u);
  return 1;
}

__int64 CEntity::DrawEntityVS(_ENTITY_LIST *a2, float (*const a3)[4], unsigned int a4)
{
  CEntity::PrepareAnimation();
  IDirect3DDevice8 *const d3dDevice = GetD3dDevice();
  if (!mIsEntityLoad || !mStaticVertexBuffer.m_lpVertexBuffer)
    return 0;

  const float alpha = static_cast<float>(HIBYTE(a4)) / 255.0f;
  MultiTexOff();
  SetVPIPTex1(mStaticVertexBuffer.m_lpVertexBuffer, mStaticIndexedBuffer.m_lpIndexBuffer);
  CEntity::SetVertexShaderID(a2, a3, a4);

  if (mFlag & 0x40)
    d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_COLOROP, 4u);
  else
    d3dDevice->SetTextureStageState(d3dDevice, 0, D3DTSS_COLOROP, 5u);

  if (dword_184A79808)
  {
    d3dDevice->SetRenderState(d3dDevice, D3DRS_FOGENABLE, 1u);
    d3dDevice->SetRenderState(d3dDevice, D3DRS_FOGVERTEXMODE, 0);
  }

  for (int i = 0; i < mMatGroupNum; ++i)
  {
    float objectMatrix[16]{};
    GetObjectMatrix(reinterpret_cast<float (*)[4]>(objectMatrix), mMatGroup[i].ObjectId, mObject, 0.0);
    if (SLOBYTE(mFlag) < 0)
    {
      float billboard[16]{};
      memcpy_0(billboard, &dword_184A79AEC, sizeof(billboard));
      billboard[12] = 0.0f;
      billboard[13] = 0.0f;
      billboard[14] = 0.0f;
      MatrixMultiply(reinterpret_cast<float (*)[4]>(objectMatrix), reinterpret_cast<float (*)[4]>(billboard), reinterpret_cast<float (*)[4]>(objectMatrix));
    }
    MatrixMultiply(reinterpret_cast<float (*)[4]>(objectMatrix), a3, reinterpret_cast<float (*)[4]>(objectMatrix));

    if (alpha != 1.0f)
      BlendOn(13);

    const unsigned int alphaType = mMat[mMatGroup[i].MtlId].m_Layer[0].m_dwAlphaType;
    if (!alphaType || (alphaType & 0x2000))
      d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 1u);
    else
      d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 0);

    SetWorldViewMatrixVS(reinterpret_cast<float (*)[4]>(objectMatrix));
    CEntity::DrawOneMatGroupVS(&mMatGroup[i]);
    d3dDevice->SetRenderState(d3dDevice, D3DRS_ZWRITEENABLE, 1u);
  }

  if (dword_184A79808)
    d3dDevice->SetRenderState(d3dDevice, D3DRS_FOGVERTEXMODE, 3u);

  BlendOff();
  return 1;
}

__int64 CEntity::OnceDrawEntity(float (*const a2)[4], unsigned int a3)
{
  const double frame = mFrame;
  if (static_cast<float>(frame) > static_cast<float>(mObject->frames))
    return 0;
  CEntity::DrawEntity(a2, a3, frame);
  mFrame += static_cast<float>(R3GetLoopTime() * 30.0);
  return 1;
}

