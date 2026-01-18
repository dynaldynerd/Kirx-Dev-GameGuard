#pragma once

#include "IdaCompat.h"

/* 1313 */
class __cppobj __declspec(align(8)) CMapData
{
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
  virtual ~CMapData() = default;
};
