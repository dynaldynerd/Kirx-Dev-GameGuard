#pragma once

#include "IdaCompat.h"

/* 1395 */
class __cppobj __unaligned __declspec(align(4)) CEntity
{
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
};

