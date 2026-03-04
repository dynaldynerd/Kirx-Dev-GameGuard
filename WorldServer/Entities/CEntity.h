#pragma once

#include "IdaCompat.h"
#include "EntityTypes.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"

struct _R3MATERIAL;
struct _ENTITY_M_GROUP;

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
#pragma pack(push, 1)
class   CEntity
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
  CEntity();
  ~CEntity();

  __int64 LoadEntity(char *a2, unsigned int a3);
  void AddFlag(int a2);
  __int64 DrawEntity(float (*const a2)[4], unsigned int a3, double a4);
  __int64 DrawEntityVS(_ENTITY_LIST *a2, float (*const a3)[4], unsigned int a4);
  void SetMaterialAndLight(unsigned int a2);
  void SetVertexShaderID(_ENTITY_LIST *a2, float (*const a3)[4], unsigned int a4);
  void DrawOneMatGroup(_ENTITY_M_GROUP *a2);
  void DrawOneMatGroupVS(_ENTITY_M_GROUP *a2);
  void GetAnimationMatrix(float (*const a2)[4], _ANI_OBJECT *a3, double a4);
  _ENTITY_M_GROUP *GetMatGroup();
  __int64 GetMatGroupNum();
  __int64 GetMatNum();
  _ANI_OBJECT *GetObjectA();
  CIndexBuffer *GetStaticIndexedBuffer();
  CVertexBuffer *GetStaticVertexBuffer();
  __int64 GetUsedVertexBufferSize();
  __int64 IsAlpha();
  bool IsEnableShaderID(int a2);
  bool IsFirstAlpha();
  void SetMapColor(unsigned int a2);
  __int64 OnceDrawEntity(float (*const a2)[4], unsigned int a3);
  void ReleaseEntity();
  void ReleaseTexMem();
  void RestoreTexMem();
  void PrepareAnimation();
};
#pragma pack(pop)
