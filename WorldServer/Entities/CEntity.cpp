#include "pch.h"

#include "CEntity.h"

#include <cstring>

#include "WorldServerUtil.h"

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
    Dmalloc(mHeader.Track.size + 52 * mMatGroupNum + sizeof(_ANI_OBJECT) * mObjectNum + 8 * staticVertNum));
  mObject = reinterpret_cast<_ANI_OBJECT *>(reinterpret_cast<unsigned char *>(mMatGroup) + 52 * mMatGroupNum);
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

    mBBMin[0] = std::min(mBBMin[0], mMatGroup[i].BBMin[0]);
    mBBMin[1] = std::min(mBBMin[1], mMatGroup[i].BBMin[1]);
    mBBMin[2] = std::min(mBBMin[2], mMatGroup[i].BBMin[2]);
    mBBMax[0] = std::max(mBBMax[0], mMatGroup[i].BBMax[0]);
    mBBMax[1] = std::max(mBBMax[1], mMatGroup[i].BBMax[1]);
    mBBMax[2] = std::max(mBBMax[2], mMatGroup[i].BBMax[2]);

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
    auto **texList = reinterpret_cast<unsigned long long *>(mIndependencyTex);
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

