#pragma once

#include "IdaCompat.h"
#include "D3D8Compat.h"
#include <cstdio>
#include <string>
#include <vector>

class CWnd;
class CMerchant;
class CItemStore;
class CPlayer;
class CRecordData;
struct _FILETIME;
struct CMergeFileManager;
struct _R3MATERIAL;
struct _ANI_OBJECT;
struct _READ_ANI_OBJECT;
struct _LIGHTMAP;
struct _animus_fld;
struct _EQUIP_MASTERY_LIM;
struct tm;

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
int GetCurrentYear();
int GetCurrentMonth();
void GetTodayStr(char *szToday);
unsigned int GetLocalDate();
unsigned int eGetLocalDate();
int GetCurDay();
int GetNextDay();
bool GetDateStrAfterDay(char *szDate, int iBuffSize, unsigned __int16 wDayAfter);
bool IsDayChanged(int *iOldDay);
bool GetDateTimeStr(char *szTime);
unsigned int GetKorLocalTime();
int GetCurrentHour();
int GetCurrentMin();
int GetCurrentSec();
int GetCurwDay();
void GetNowDateTime(char *szDateTime);
unsigned int GetConnectTime_AddBySec(int iSec);
bool GetLastWriteFileTime(const char *szFileName, _FILETIME *ftWrite);
unsigned __int8 GetItemKindCode(int nTableCode);
char *DisplayItemUpgInfo(int nTableCode, int dwLvBit);
int GetItemTableCode(const char *psItemCode);
int GetItemStdPrice(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind);
int GetItemStdPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind);
int GetItemProcPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind);
int GetItemKillPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind);
int GetItemGoldPoint(int nTableCode, int nItemIndex, int nRace, unsigned __int8 *pbyMoneyKind);
int GetUsePcCashType(unsigned __int8 byTblCode, int nIndex);
int IsCashItem(unsigned __int8 byTblCode, int dwIndex);
unsigned int GetItemDurPoint(int nTableCode, int nIndex);
char *GetItemKorName(int nTableCode, int nItemIndex);
char *GetItemEquipCivil(int nTableCode, int nItemIndex);
bool IsItemEquipCivil(int nTableCode, int nItemIndex, unsigned __int8 byRaceSex);
int IsTimeItem(unsigned __int8 byTblCode, int dwIndex);
int IsOverLapItem(int nTableCode);
int IsSellItem(int nTableCode, int nItemIndex);
int IsAbrItem(int nTableCode, int nItemIndex);
int IsItemSerialNum(int nTableCode);
int IsStorageRange(unsigned __int8 byStorageCode, unsigned __int8 byStorageIndex);
unsigned __int8 GetItemGrade(int nTableCode, int nItemIndex);
int GetItemEquipLevel(int nTableCode, int nItemIndex);
int GetItemEquipUpLevel(int nTableCode, int nItemIndex);
_EQUIP_MASTERY_LIM *GetItemEquipMastery(int nTableCode, int nItemIndex, int *pnLimNum);
unsigned __int8 GetItemUpgedLv(unsigned int dwLvBit);
unsigned __int8 GetItemUpgLimSocket(unsigned int dwLvBit);
unsigned __int8 GetDefItemUpgSocketNum(int nTableCode, int nItemIndex);
unsigned __int8 GetTalikFromSocket(unsigned int dwLvBit, unsigned __int8 bySocketIndex);
unsigned int GetBitAfterSetLimSocket(unsigned __int8 byLimSocketNum);
unsigned int GetBitAfterUpgrade(unsigned int dwCurBit, char dwTalikCode, unsigned __int8 byCurLv);
unsigned int GetBitAfterDowngrade(unsigned int dwCurBit, unsigned __int8 byCurLv);
_animus_fld *GetAnimusFldFromExp(int nAnimusClass, unsigned __int64 dwExp);
unsigned int GetMaxParamFromExp(int nAnimusClass, unsigned __int64 dwExp);
int GetMaxResKind();
unsigned __int8 GetWeaponClass(int nItemIndex);
int CalcRoundUp(float fVal);
int CalcMastery(int nMasteryCode, int nMasteryIndex, int dwMasteryCum, unsigned int nRaceCode);
int GetSFLevel(int nLv, unsigned int dwHitCount);
int GetStaffMastery(unsigned int *pdwForceLvCum);
void InitMasteryFormula(CRecordData *pSkillData, CRecordData *pForceData);
int ParsingCommandA(char *pszSrc, int nMaxWordNum, char **ppszDst, int nMaxWordSize);
int ParsingCommandW(char *pwszSrc, int nMaxWordNum, char **ppwszDst, int nMaxWordSize);
bool IsSQLValidString(const char *wszStr);
char *cvt_string(int nVal);
void WriteServerStartHistory(const char *format, ...);
void clear_file(const char *directory, int keepCount);
int MyMessageBox(const char *title, const char *format, ...);
int MyCrtDebugReportHook(int reportType, char *message, int *returnValue);
void ServerProgramExit(const char *source, int reason);
void IOFileWrite_0(char *pszFileName, unsigned int nLen, char *pszData);
void IOFileWrite_1(char *pszFileName, unsigned int nLen, char *pszData);
void IOFileWrite(char *pszFileName, unsigned int nLen, char *pszData);
void __trace(const char *fmt, ...);
void OutputDebugLog(const char *szFormat, ...);
__time64_t time_20(__int64 *_Time);
__time64_t time_18(__int64 *_Time);
__time64_t time_17(__int64 *_Time);
__time64_t time_5(__int64 *_Time);
__time64_t mktime_3(tm *_Tm);
tm *localtime_5(const __int64 *_Time);
tm *localtime_2(const __int64 *_Time);
tm *localtime_3(const __int64 *_Time);
CMerchant *FindEmptyNPC(CMerchant *pNPC, int nMax);
void NetTrace(const char *fmt, ...);
void StripEXT(char *szPath);
void StripPath(char *szPath);
void StripName(char *szPath);
char W2M(char *lpwStr, char *szTran, unsigned int wTranBuffSize);
char M2W(char *lpStr, char *wszTran, unsigned int wTranBuffSize);
bool IsServerMode();
bool IsInitR3Engine();
float R3GetTime();
float R3GetLoopTime();
struct IDirect3DDevice8 *GetD3dDevice();
unsigned __int64 SplitString(char *strSrc, const char *_Delim, std::vector<std::string> *stringlist);

int eGetRate(int nRaceCode);
float eGetTex(int nRaceCode);
unsigned int eGetTexRate(int nRaceCode);
float eGetOreRate(int nRaceCode);
CItemStore *IsBeNearStore(CPlayer *p, int nStoreCode);
void eAddDalant(int nRaceCode, int nAdd);
void eAddGold(int nRaceCode, int nAdd);
int IsStorageCodeWithItemKind(int nTableCode, int nStorageCode);

float DotProduct(const float *a1, const float *a2);
float GetSqrt(float *fPos, float *fTar);
float GetDist(float *const a1, float *const a2);
float Get3DSqrt(float *Pos, float *Tar);
float GetYAngle(float *Pos, float *Tar);
void Normalize(float *v);
void FloatToShort(float *pFloat, short *pShort, int size);
void GetNormal(float *const a1, const float *const a2, const float *const a3, const float *const a4);
int GetPlaneCrossPoint(const float *const a1, const float *const a2, float *const a3, const float *const a4, float a5);
void CrossProduct(const float *a1, const float *a2, float *a3);
void sub_1404E2FB0(float *a1, float *a2, float *a3);
bool CheckEdgeEpsilon(const float *const a1, const float *const a2, const float *const a3, const float *const a4);
bool IsCollisionBBoxPoint(float *const a1, float *const a2, float *const a3);
bool IsCollisionBBoxPoint(short *const a1, short *const a2, float *const a3);

void GetVertexFromBVertex(float *const a1, char *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromWVertex(float *const a1, short *a2, _BSP_READ_M_GROUP *a3);
void GetVertexFromFVertex(float *const a1, float *a2, _BSP_READ_M_GROUP *a3);
bool IsParticle(char *a1);
void sub_14050C650(float *a1, float *a2, float *a3);
void Vector3fTransform(float *const a1, float *const a2, float (*const a3)[4]);

unsigned int sub_1404E1570(float a1, float a2, float a3);
unsigned int sub_1404E46E0(float a1, float a2, float a3);

// Matrix/animation helpers (R3 engine).
float GetFloatMod(float a1, float a2);
void MatrixIdentity(float (*const a1)[4]);
void MatrixCopy(float (*const a1)[4], float (*const a2)[4]);
void MatrixMultiply(float (*a1)[4], float (*const a2)[4], float (*const a3)[4]);
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
void GetObjectMatrix(float (*a1)[4], unsigned short a2, _ANI_OBJECT *a3, double a4);
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
__int64 CalcFileSize(char *pszFileName);
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

bool CanAddMoneyForMaxLimMoney(unsigned __int64 ui64AddMoney, unsigned __int64 ui64HasMoney);
bool CanAddMoneyForMaxLimGold(unsigned __int64 ui64AddGold, unsigned __int64 ui64HasGold);
void MakeBinaryStr(const unsigned __int8 *pBuff, unsigned __int64 tBufSize, char *pOut, rsize_t tOutSize);

// Merge-file helpers.
CMergeFileManager *GetMergeFileManager();
FILE *fopenMFM(char *a1, const char *Mode);
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
extern const char *dayofweek[7];
