#pragma once

#include "IdaCompat.h"

class CWnd;
class CMerchant;
struct CMergeFileManager;
struct _R3MATERIAL;
struct _ANI_OBJECT;

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
bool IsServerMode();
bool IsInitR3Engine();
struct IDirect3DDevice8 *GetD3dDevice();

float DotProduct(const float *a1, const float *a2);
void GetNormal(float *const a1, const float *const a2, const float *const a3, const float *const a4);
int GetPlaneCrossPoint(const float *const a1, const float *const a2, float *const a3, const float *const a4, float a5);
void CrossProduct(const float *a1, const float *a2, float *a3);
bool CheckEdgeEpsilon(const float *const a1, const float *const a2, const float *const a3, const float *const a4);

void GetVertexFromBVertex(float *const a1, char *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromWVertex(float *const a1, short *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromFVertex(float *const a1, float *a2, _BSP_READ_M_GROUP *a3);
bool IsParticle(char *a1);

// Matrix/animation helpers (R3 engine).
float GetFloatMod(float a1, float a2);
void MatrixIdentity(float (*const a1)[4]);
void MatrixCopy(float (*const a1)[4], float (*const a2)[4]);
void MatrixMultiply(float (*const a1)[4], float (*const a2)[4], float (*const a3)[4]);
int MatrixInvert(float (*const a1)[4], float (*const a2)[4]);
void MatrixFromQuaternion(float (*const a1)[4], float a2, float a3, float a4, float a5);
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

// Vertex/Index buffer tracking.
void ResetTotalVertexBufferInfo();
unsigned int GetTotalVertexBufferSize();
unsigned int GetTotalVertexBufferCnt();

// Memory tracking.
void *Dmalloc(int size);
void Dfree(void *ptr);
unsigned int GetDmallocSize();
unsigned int GetDmallocCnt();

void ResetTexMemSize();
bool IsExistFile(char *szFileName);
void LoadMainR3M(char *szFileName);
struct R3Texture *R3GetTexInfoR3T(char *szFileName, int a2);
void SetNoLodTextere();
void LoadR3T(struct R3Texture *pTex);
unsigned int GetNowTexMemSize();
_R3MATERIAL *LoadMainMaterial(char *szFileName);
_R3MATERIAL *GetMainMaterial();
unsigned int GetMainMaterialNum();
void SetMergeFileManager(CMergeFileManager *pMgr);
void LoadLightMap(char *szFileName);
void CN_SetEnableSky(int bEnable);
void R3RestoreAllTextures();
void RTMovieCreate(char *szFileName, void *pLevel);
void RTMovieSetState(unsigned int nState);
void R3EnvironmentShakeOff();
void ClearDynamicLight();
void Error(char *source, char *msg);
void Warning(char *source, char *msg);

extern CWnd *g_pFrame;
