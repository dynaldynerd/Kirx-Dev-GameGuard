#pragma once

#include "IdaCompat.h"
#include "CAlpha.h"
#include "CVertexBuffer.h"
#include "CIndexBuffer.h"
#include "CMergeFileManager.h"
#include "CPathFinder.h"

struct _BSP_ENTRY
{
  unsigned int offset;
  unsigned int size;
};

struct _BSP_FILE_HEADER
{
  unsigned int version;
  _BSP_ENTRY CPlanes;
  _BSP_ENTRY CFaceId;
  _BSP_ENTRY Node;
  _BSP_ENTRY Leaf;
  _BSP_ENTRY MatListInLeaf;
  _BSP_ENTRY Object;
  _BSP_ENTRY Track;
  _BSP_ENTRY EventObjectID;
  _BSP_ENTRY ReadSpare[35];
  _BSP_ENTRY BVertex;
  _BSP_ENTRY WVertex;
  _BSP_ENTRY FVertex;
  _BSP_ENTRY VertexColor;
  _BSP_ENTRY UV;
  _BSP_ENTRY LgtUV;
  _BSP_ENTRY Face;
  _BSP_ENTRY FaceId;
  _BSP_ENTRY VertexId;
  _BSP_ENTRY ReadMatGroup;
  _BSP_ENTRY FreeSpare[32];
};

struct _EXT_BSP_FILE_HEADER
{
  unsigned int version;
  _BSP_ENTRY CFVertex;
  _BSP_ENTRY CFLine;
  _BSP_ENTRY CFLineId;
  _BSP_ENTRY CFLeaf;
  _BSP_ENTRY EntityList;
  _BSP_ENTRY EntityID;
  _BSP_ENTRY LeafEntityList;
  _BSP_ENTRY SoundEntityID;
  _BSP_ENTRY LeafSoundEntityList;
  _BSP_ENTRY ReadSpare[18];
  _BSP_ENTRY MapEntitiesList;
  _BSP_ENTRY SoundEntityList;
  _BSP_ENTRY SoundEntitiesList;
  _BSP_ENTRY FreeSpare[18];
};

// Read mat-group entries are packed to 2-byte alignment (size 0x2A).
#pragma pack(push, 2)
struct _BSP_READ_M_GROUP
{
  unsigned short attr;
  unsigned short face_num;
  unsigned int face_start_id;
  short mtl_id;
  short lgt_id;
  short bb_min[3];
  short bb_max[3];
  float pos[3];
  float scale;
  unsigned short object_id;
};
#pragma pack(pop)

// Read face entries are packed to 2-byte alignment (size 0x06).
#pragma pack(push, 2)
struct _BSP_READ_FACE
{
  unsigned short v_num;
  unsigned int v_start_id;
};
#pragma pack(pop)

struct _BSP_NODE
{
  unsigned int f_normal_id;
  float d;
  short front;
  short back;
  short bb_min[3];
  short bb_max[3];
};

// Leaf entries are packed to 1-byte alignment (size 0x19).
#pragma pack(push, 1)
struct _BSP_LEAF
{
  unsigned char type;
  unsigned short face_num;
  unsigned int face_start_id;
  unsigned short m_group_num;
  unsigned int m_group_start_id;
  short bb_min[3];
  short bb_max[3];
};
#pragma pack(pop)

// Collision face entries are packed to 1-byte alignment (size 0x18).
#pragma pack(push, 1)
struct _BSP_C_FACE
{
  unsigned char Attr;
  unsigned char VNum;
  unsigned int VStartId;
  unsigned short MatGIndex;
  float Normal[4];
};
#pragma pack(pop)

struct _TOOL_COL_LINE
{
  unsigned int attr;
  unsigned short start_v;
  unsigned short end_v;
  float height;
  unsigned short front;
  unsigned short back;
};

// Tool collision leaf entries are packed to 2-byte alignment (size 0x06).
#pragma pack(push, 2)
struct _TOOL_COL_LEAF
{
  unsigned int start_id;
  unsigned short line_num;
};
#pragma pack(pop)

// Mat-group entries are packed to 2-byte alignment (size 0x56).
#pragma pack(push, 2)
struct _BSP_MAT_GROUP
{
  unsigned short Type;
  unsigned short TriNum;
  short MtlId;
  short LgtId;
  float BBMin[3];
  float BBMax[3];
  float Origin[3];
  unsigned int VBMinIndex;
  unsigned int IBMinIndex;
  unsigned int VertexBufferId;
  unsigned int VCnt;
  unsigned int CFaceStartVId;
  void *MultiSourceUV;
  void *MultiSourceST;
  unsigned short ObjectId;
  float CoronaAlpha;
};
#pragma pack(pop)

struct _ANI_OBJECT;
struct _MAP_ENTITIES_LIST;
struct _READ_MAP_ENTITIES_LIST;
struct _READ_SOUND_ENTITY_LIST;
struct _READ_SOUND_ENTITIES_LIST;
class CEntity;
class CParticle;
class CExtDummy;

/* 1423 */
class __cppobj CBsp
{
public:
  float (*mCVertex)[3];
  float (*mCNNormal)[3];
  unsigned int *mCVertexId;
  _BSP_C_FACE *mCFace;
  float (*mCNEdgeNormal)[4];
  unsigned int *mCFaceId;
  unsigned int *mVertexColor;
  __int16 (*mLgtUV)[2];
  int mNowCFaceId;
  unsigned __int16 *MatListInLeafId;
  unsigned int mMatGroupNum;
  _BSP_MAT_GROUP *mMatGroup;
  unsigned int mCVertexNum;
  unsigned int mCFaceNum;
  unsigned int mVertexNum;
  unsigned int mFaceNum;
  int mIsLoaded;
  unsigned int mObjectNum;
  _ANI_OBJECT *mObject;
  unsigned __int16 *mEventObjectID;
  unsigned int mEnvID[2];
  $1D4D54E2B5971D5BE0EAD557ED232A85 ___u21;
  CMergeFileManager mMapEntityMFM;
  int mIsLoadEBP;
  unsigned __int8 *mEntityCache;
  unsigned int mEntityCacheSize;
  unsigned int mNowRenderMatGroupNum;
  unsigned int mNowShadowMatGroupNum;
  CEntity *mEntity;
  CParticle *mParticle;
  _ENTITY_LIST *mEntityList;
  unsigned __int16 *mEntityID;
  _LEAF_ENTITIES_LIST_INFO *mLeafEntityList;
  _MAP_ENTITIES_LIST *mMapEntitiesList;
  unsigned int mEntityListNum;
  unsigned int mLeafEntityListNum;
  unsigned int mEntityIDNum;
  unsigned int mMapEntitiesListNum;
  unsigned __int8 *mSoundEntityCache;
  unsigned int mSoundEntityCacheSize;
  unsigned int mSoundEntityIDNum;
  unsigned int mLeafSoundEntityListNum;
  unsigned int mSoundEntityListNum;
  unsigned int mSoundEntitiesListNum;
  _SOUND_ENTITY_LIST *mSoundEntityList;
  _SOUND_ENTITIES_LIST *mSoundEntitiesList;
  unsigned __int16 *mSoundEntityID;
  _LEAF_SOUND_ENTITIES_LIST_INFO *mLeafSoundEntityList;
  unsigned int mTotalAllocSize;
  unsigned int mTotalWaveSize;
  unsigned int mVertexBufferSize;
  _BSP_NODE *mNode;
  _BSP_LEAF *mLeaf;
  unsigned int mLeafNum;
  unsigned int mNodeNum;
  unsigned int mCFVertexNum;
  unsigned int mCFLineNum;
  unsigned int mCFLineIdNum;
  float (*mCFVertex)[3];
  _TOOL_COL_LINE *mCFLine;
  unsigned __int16 *mCFLineId;
  _TOOL_COL_LEAF *mCFLeaf;
  float (*mCFVNormal)[3];
  float (*mCFNormal)[4];
  CExtDummy *mDummy;
  CPathFinder mPathFinder;
  int mColFaceId;
  unsigned int mFindPathCnt;
  __int16 mNowPlayerNum;
  unsigned int mStaticVBCnt;
  CVertexBuffer mStaticVertexBuffer[80];
  unsigned int mVBVertexNum[80];
  CIndexBuffer mStaticIndexedBuffer;
  void *mMultiLayerUV;
  void *mMultiLayerST;
  unsigned __int8 *mStaticAlloc;
  unsigned int mStaticAllocSize;
  unsigned __int8 *mExtBspStaticAlloc;
  unsigned int mExtBspStaticAllocSize;
  int mPickPoly;
  unsigned __int8 *mMatGroupCache;
  int mMatGroupCacheSize;
  float mTempCamera[3];
  __int16 mTempSearchOk;
  __int16 mNowLeafNum;
  _BSP_FILE_HEADER mBSPHeader;
  _EXT_BSP_FILE_HEADER mExtBSPHeader;
  CAlpha mAlpha;

  float GetFirstYpos(float *const a2, float *const a3, float *const a4);
  float GetFirstYpos(float *const a2, int a3);
  __int16 GetLeafNum(float *const a2);
  float GetYposInLeaf(float *const a2, float *const a3, float a4, float a5, int a6);
  float GetYposInLeafUseEdgeNormal(float *const a2, float *const a3, float a4, float a5, int a6);
  float GetBestYposInLeaf(float *const a2, float *const a3, float a4, float a5, int a6);
  __int64 GetLightFromPoint(float *const a2, int a3);
  __int64 GetColorFromPoint(int a2, float *const a3);
  void GetLightMapUVFromPoint(float *const a2, int a3, float *const a4);
  void GetLeafList(float *const a2, float *const a3, int *const a4, __int16 *a5, unsigned int a6);
  __int64 IsCollisionFace(float *const a2, float *const a3, float (*a4)[3], float (*a5)[4]);

  void LoadBsp(char *szFileName);
  void LoadExtBsp(char *szFileName);
  void CalcEntitiesMainColor();

private:
  float GetFirstYpos(float *const a2, __int16 *const a3, __int16 *const a4);
  void GetFaceFrontPoint(float (*a2)[3], int a3);
  int IsExistSelfPoint(int a2, int a3);
  void SearchNode(__int16 a2);
  void WalkNodeForLeafList(short nNodeId, float *const a3, float *const a4, short *a5, int *a6);
  void SubLeafList(float a2, _BSP_NODE *pNode, float *const a4, float *const a5, short *a6, int *a7);
  bool EdgeTest(float *pPos, unsigned int nNormalId);
  void SetCFNormal();
  void ReadDynamicDataFillVertexBuffer(FILE *Stream);
  void ReadDynamicDataExtBsp(FILE *Stream);
  void OnlyStoreCollisionStructure(_BSP_READ_M_GROUP *pRM, char (*pBV)[3], short (*pWV)[3], float (*pFV)[3], unsigned int *pVI, _BSP_READ_FACE *pRF, unsigned int *pFI);
  void MakeEdgeNormal();
  void LoadEntities(_READ_MAP_ENTITIES_LIST *a2);
  void LoadSoundEntities(_READ_SOUND_ENTITY_LIST *a2, _READ_SOUND_ENTITIES_LIST *a3);
public:
  bool CanYouGoThere(float *const a2, float *const a3, float (*a4)[3]);
  int GetPathCrossPoint(float *const a2, float *const a3, float (*a4)[3], int a5, int a6);
  int GetPathFind(float *const a2, float *const a3, float (*const a4)[3], unsigned int *a5, int a6);
  int GetPath(float *const a2, float *const a3);
  int GetFinalPath(float *a2, float *const a3, float (*const a4)[3]);
};

