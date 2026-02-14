#pragma once

#include "IdaCompat.h"

#include "DummyPosition.h"
#include "CExtDummy.h"
#include "CLevel.h"
#include "mon_block_fld.h"
#include "mon_active_fld.h"

class CObjectList;
class CGameObject;
struct _pnt_rect;

/* 1313 */
class __cppobj __declspec(align(8)) CMapData
{
public:
  CMapData();
  virtual ~CMapData() = default;

  _bsp_info *GetBspInfo();
  _sec_info *GetSecInfo();
  unsigned __int8 GetMapCode();
  unsigned __int8 GetRaceTown(float *fPos, unsigned __int8 byRaceCode);
  int GetSectorIndex(float *pPos);
  CObjectList *GetSectorListObj(unsigned __int16 wLayerIndex, unsigned int dwSecIndex);
  CObjectList *GetSectorListPlayer(unsigned __int16 wLayerIndex, unsigned int dwSecIndex);
  CObjectList *GetSectorListTower(unsigned __int16 wLayerIndex, unsigned int dwSecIndex);
  void GetRectInRadius(_pnt_rect *pRect, int nRadius, unsigned int nSecNum);
  void EnterMap(CGameObject *pObj, unsigned int dwSecIndex);
  void ExitMap(CGameObject *pObj, unsigned int dwSecIndex);
  bool IsMapIn(float *fPos);
  void Init(_map_fld *pMapSet);
  bool OpenMap(char *szFileName, _map_fld *pMapSet, bool bUse);
  bool ConvertLocalToWorldDummy(CDummyPosTable *pTable, bool bCheckCenter);
  bool GetRandPosInDummy(_dummy_position *pDumPos, float *fOutPos, bool bRePos);
  __int64 GetResDummySector(int nDummyIndex, float *pCurPos);
  bool GetRandPosInRange(float *pStdPos, int nRange, float *pNewPos);
  bool GetRandPosVirtualDumExcludeStdRange(float *pStdPos, int nRange, int iExcludeRange, float *pNewPos);
  bool LoadHolySystemDummy(char *pszDummyCode, _dummy_position *pPos);
  bool LoadDummy(char *pszDummyCode, _dummy_position *pPos);
  _dummy_position *GetDummyPostion(char *pszDummyCode);
  void OnLoop();
  _portal_dummy *GetPortal(int nPortalIndex);
  _portal_dummy *GetPortal(char *pPortalCode);
  int GetPortalInx(char *pPortalCode);

  bool _LoadBspSec(char *pszMapCode);
  bool _LoadMonBlk(char *pszMapCode, _map_fld *pMapFld);
  bool _LoadPortal(char *pszMapCode);
  bool _LoadStoreDummy(char *pszMapCode);
  bool _LoadStart(char *pszMapCode);
  bool _LoadResource(char *pszMapCode);
  bool _LoadBind(char *pszMapCode);
  bool _LoadQuest(char *pszMapCode);
  bool _LoadSafe(char *pszMapCode);
  bool ConvertLocal(_dummy_position *pPos);
  bool CheckCenterPosDummy(_dummy_position *pPos);

  bool m_bUse;
  bool m_bLoad;
  int m_nMapIndex;
  CLevel m_Level;
  int m_nMapCode;
  _LAYER_SET *m_ls;
  _MULTI_BLOCK *m_mb;
  CExtDummy m_Dummy;
  int m_nMapInPlayerNum;
  int m_nMapInMonsterNum;
  int m_nMonBlockNum;
  _mon_block *m_pMonBlock;
  int m_nMonDumNum;
  int m_nPortalNum;
  _portal_dummy *m_pPortal;
  int m_nItemStoreDumNum;
  _store_dummy *m_pItemStoreDummy;
  int m_nStartDumNum;
  _start_dummy *m_pStartDummy;
  int m_nBindDumNum;
  _bind_dummy *m_pBindDummy;
  int m_nResDumNum;
  _res_dummy *m_pResDummy;
  int m_nQuestDumNum;
  _quest_dummy *m_pQuestDummy;
  _map_fld *m_pMapSet;
  CExtDummy *m_pExtDummy_Town;
  int m_nSafeDumNum;
  _safe_dummy *m_pSafeDummy;
  CDummyPosTable m_tbSafeDumPos;
  CRecordData m_tbMonBlk;
  CRecordData m_tbPortal;
  CDummyPosTable m_tbMonDumPos;
  CDummyPosTable m_tbPortalDumPos;
  CDummyPosTable m_tbStoreDumPos;
  CDummyPosTable m_tbStartDumPos;
  CDummyPosTable m_tbBindDumPos;
  CDummyPosTable m_tbResDumPosHigh;
  CDummyPosTable m_tbResDumPosMiddle;
  CDummyPosTable m_tbResDumPosLow;
  CDummyPosTable m_tbQuestDumPos;
  _bsp_info m_BspInfo;
  _sec_info m_SecInfo;
  CMyTimer m_tmrMineGradeReSet;
  int m_nMonTotalCount;
};
