#pragma once

#include "IdaCompat.h"

class CWnd;
class CMerchant;

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
char* StripEXT(char* szPath);
char* StripPath(char* szPath);
bool IsServerMode();

float DotProduct(const float *a1, const float *a2);
void GetNormal(float *const a1, const float *const a2, const float *const a3, const float *const a4);
int GetPlaneCrossPoint(const float *const a1, const float *const a2, float *const a3, const float *const a4, float a5);
void CrossProduct(const float *a1, const float *a2, float *a3);
bool CheckEdgeEpsilon(const float *const a1, const float *const a2, const float *const a3, const float *const a4);

void GetVertexFromBVertex(float *const a1, char *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromWVertex(float *const a1, short *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromFVertex(float *const a1, float *a2, _BSP_READ_M_GROUP *a3);

void ResetTexMemSize();
bool IsExistFile(char *szFileName);
void LoadMainR3M(char *szFileName);
struct R3Texture *R3GetTexInfoR3T(char *szFileName, int a2);
void SetNoLodTextere();
void LoadR3T(struct R3Texture *pTex);
unsigned int GetNowTexMemSize();
void LoadMainMaterial(char *szFileName);
void SetMergeFileManager(void *pMgr);
void LoadLightMap(char *szFileName);
void CN_SetEnableSky(int bEnable);
void R3RestoreAllTextures();
void RTMovieCreate(char *szFileName, void *pLevel);
void RTMovieSetState(unsigned int nState);
void R3EnvironmentShakeOff();
void ClearDynamicLight();
void Error(char *source, char *msg);

extern CWnd *g_pFrame;
