#pragma once

#include "IdaCompat.h"

// Read-only animation object data (packed).
#pragma pack(push, 1)
struct _READ_ANI_OBJECT
{
  unsigned __int16 flag;
  unsigned __int16 parent;
  int frames;
  int Pos_cnt;
  int Rot_cnt;
  int Scale_cnt;
  float scale[3];
  float scale_quat[4];
  float pos[3];
  float quat[4];
  unsigned int pos_offset;
  unsigned int rot_offset;
  unsigned int scale_offset;
};
#pragma pack(pop)

/* 1395 */
class __cppobj __unaligned __declspec(align(4)) CEntity
{
public:
  int mIsUseMemory;
  int mIsEntityLoad;
  int mIsAlpha;
  _ENTITY_FILE_HEADER mHeader;
  unsigned int mVetexBufferSize;
  float (*mOrgUV)[2];
  unsigned int mMapColor;
  unsigned int mFlag;
  _R3MATERIAL *mMat;
  CVertexBuffer mStaticVertexBuffer;
  CIndexBuffer mStaticIndexedBuffer;
  unsigned int mTextureSize;
  int mMatGroupNum;
  _ENTITY_M_GROUP *mMatGroup;
  int mObjectNum;
  _ANI_OBJECT *mObject;
  unsigned __int8 *mTrack;
  int mStartTexID;
  int mTexNum;
  unsigned int mVertexNum;
  void *mIndependencyTex;
  float mStartTime;
  float mBBMin[3];
  float mBBMax[3];
  float mAddFrame;
  float mFrame;

public:
  __int64 LoadEntity(char *a2, unsigned int a3);
  void ReleaseEntity();
  void ReleaseTexMem();
  void RestoreTexMem();
  void PrepareAnimation();
};

