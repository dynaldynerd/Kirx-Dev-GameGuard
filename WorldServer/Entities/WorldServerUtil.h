#pragma once

#include "IdaCompat.h"
#include "D3D8Compat.h"
#include <cstdio>

class CWnd;
class CMerchant;
struct CMergeFileManager;
struct _R3MATERIAL;
struct _ANI_OBJECT;
struct _READ_ANI_OBJECT;
struct _LIGHTMAP;

struct R3Texture
{
  char mName[128];
  unsigned int mStartID;
  unsigned int mTexNum;
  unsigned int mFlag;
  unsigned int mSameCnt;
};

struct _LIGHTMAP
{
  unsigned short xl;
  unsigned short yl;
  unsigned short *buf;
};

unsigned int GetLoopTime();
int GetCurrentDay();
int GetCurDay();
bool GetDateTimeStr(char *szTime);
unsigned int GetKorLocalTime();
int GetCurrentHour();
int GetCurrentMin();
int GetCurrentSec();
int GetItemTableCode(const char *psItemCode);
int ParsingCommandA(char *pszSrc, int nMaxWordNum, char **ppszDst, int nMaxWordSize);
void WriteServerStartHistory(const char *format, ...);
void clear_file(const char *directory, int keepCount);
int MyMessageBox(const char *title, const char *format, ...);
int MyCrtDebugReportHook(int reportType, char *message, int *returnValue);
void ServerProgramExit(const char *source, int reason);
CMerchant *FindEmptyNPC(CMerchant *pNPC, int nMax);
void NetTrace(const char *fmt, ...);
void StripEXT(char *szPath);
void StripPath(char *szPath);
void StripName(char *szPath);
bool IsServerMode();
bool IsInitR3Engine();
struct IDirect3DDevice8 *GetD3dDevice();

float DotProduct(const float *a1, const float *a2);
void GetNormal(float *const a1, const float *const a2, const float *const a3, const float *const a4);
int GetPlaneCrossPoint(const float *const a1, const float *const a2, float *const a3, const float *const a4, float a5);
void CrossProduct(const float *a1, const float *a2, float *a3);
void sub_1404E2FB0(float *a1, float *a2, float *a3);
bool CheckEdgeEpsilon(const float *const a1, const float *const a2, const float *const a3, const float *const a4);

void GetVertexFromBVertex(float *const a1, char *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromWVertex(float *const a1, short *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromFVertex(float *const a1, float *a2, _BSP_READ_M_GROUP *a3);
bool IsParticle(char *a1);
void sub_14050C650(float *a1, float *a2, float *a3);

// Matrix/animation helpers (R3 engine).
float GetFloatMod(float a1, float a2);
void MatrixIdentity(float (*const a1)[4]);
void MatrixCopy(float (*const a1)[4], float (*const a2)[4]);
void MatrixMultiply(float (*const a1)[4], float (*const a2)[4], float (*const a3)[4]);
int MatrixInvert(float (*const a1)[4], float (*const a2)[4]);
void MatrixFromQuaternion(float (*const a1)[4], float a2, float a3, float a4, float a5);
void MatrixScale(float (*const a1)[4], float a2, float a3, float a4);
void MatrixRotate(float (*const a1)[4], float a2, float a3, float a4);
void MatrixRotateX(float (*const a1)[4], float a2);
void MatrixRotateY(float (*const a1)[4], float a2);
void MatrixRotateZ(float (*const a1)[4], float a2);
void QuaternionSlerp(
  float *a1,
  float *a2,
  float *a3,
  float *a4,
  float a5,
  float a6,
  float a7,
  float a8,
  float a9,
  float a10,
  float a11,
  float a12,
  float a13);
void GetMatrixFrom3DSMAXMatrix(float (*const a1)[4]);
void GetAniMatrix(float (*const a1)[4], _ANI_OBJECT *a2, double a3);
void GetObjectMatrix(float (*const a1)[4], unsigned short a2, _ANI_OBJECT *a3, double a4);
void ConvAniObject(int a1, unsigned __int8 *a2, _READ_ANI_OBJECT *a3, _ANI_OBJECT *a4);

// Vertex/Index buffer tracking.
void ResetTotalVertexBufferInfo();
unsigned int GetTotalVertexBufferSize();
unsigned int GetTotalVertexBufferCnt();

// Memory tracking.
void *Dmalloc(int size);
void Dfree(void *ptr);
void *ReAlloc(void *ptr, unsigned int oldSize, unsigned int newSize);
unsigned int GetDmallocSize();
unsigned int GetDmallocCnt();

void ResetTexMemSize();
bool IsExistFile(char *szFileName);
unsigned int GetFileSizeAndMergeFile(char *szFileName);
__int64 IsExistFileAndMergeFile(char *szFileName);
struct _R3MATERIAL *LoadMainR3M(char *szFileName);
struct _R3MATERIAL *LoadSubR3M(char *szFileName);
struct _R3MATERIAL *LoadIndependenceR3M(char *szFileName);
struct _R3MATERIAL *LoadSubMaterial(char *szFileName);
void ReleaseMainMaterial();
void ReleaseSubMaterial(struct _R3MATERIAL *mat);
void AdjustIndependenceR3M(struct _R3MATERIAL *mat, int startId, int newStartId);
struct R3Texture *R3GetTexInfoR3T(char *szFileName, int a2);
int R3GetPreTextureId(char *szFileName);
void R3GetPreAniTextureId(char *path, char *name, int *startId, int *num);
void R3ReleasePreTextures();
__int64 IsAniTex(char *name);
void SetNoLodTextere();
void LoadR3T(struct R3Texture *pTex);
unsigned int GetNowTexMemSize();
__int64 GetNowR3D3DTexCnt();
void SetNowR3D3DTexCnt(int cnt);
__int64 GetNowR3TexCnt();
void SetNowR3TexCnt(int cnt);
void SetR3TexManageFlag(unsigned int flag);
__int64 GetR3TexManageFlag();
void R3LoadTextureMem(int id);
void R3ReleaseTextureMem(int id);
struct IDirect3DTexture8 *R3GetSurface(int id);
void SetR3D3DTexture(unsigned int id, int flag);
struct IDirect3DTexture8 *R3LoadDDS(char *name, unsigned int mipmap, unsigned int maxX, unsigned int maxY);
struct IDirect3DTexture8 *R3LoadDDSFromFP(FILE *fp, size_t size, unsigned int mipmap, unsigned int maxX, unsigned int maxY);
struct IDirect3DTexture8 *R3LoadDDSAndTextureMem(char *name);
_R3MATERIAL *LoadMainMaterial(char *szFileName);
_R3MATERIAL *GetMainMaterial();
unsigned int GetMainMaterialNum();
void SetMergeFileManager(CMergeFileManager *pMgr);
void LoadLightMap(char *szFileName);
void ReleaseLightMap();
struct IDirect3DTexture8 *GetLightMapSurface(int id);
unsigned int GetLightMapTexSize();
unsigned int GetLightMapColor(float *const uv, int id);
void CN_SetEnableSky(int bEnable);
void R3RestoreAllTextures();
void RTMovieCreate(char *szFileName, void *pLevel);
void RTMovieSetState(unsigned int nState);
void R3EnvironmentShakeOff();
void ClearDynamicLight();
void Error(char *source, char *msg);
void Warning(char *source, char *msg);

// Merge-file helpers.
CMergeFileManager *GetMergeFileManager();
FILE *fopenMFM(char *a1, char *Mode);
unsigned int GetFileSize(char *a1);

// Particle/script helpers.
__int64 GetTokenFloat(char *a1, float *a2);
__int64 GetRandOrNum(FILE *Stream, float *a2, float *a3);

// Sound system stubs (IDA).
__int64 IM_LoadWave(char *a1, unsigned int a2);
void IM_StopWave(unsigned int a1);
void IM_ReleaseWave(unsigned int a1);
void IM_ReleaseAllWaves();

extern CWnd *g_pFrame;
