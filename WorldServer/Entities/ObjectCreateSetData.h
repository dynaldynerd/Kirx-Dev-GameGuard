#pragma once

#include "IdaCompat.h"

struct _base_fld;
struct _dummy_position;
struct _mon_active;
class CMapData;
class CItemStore;
class CMonster;
class CPlayer;

struct  _object_create_setdata
{
  _base_fld *m_pRecordSet;
  CMapData *m_pMap;
  int m_nLayerIndex;
  float m_fStartPos[3];

  _object_create_setdata()
  {
    m_pRecordSet = nullptr;
    m_pMap = nullptr;
    m_nLayerIndex = 0;
    m_fStartPos[0] = m_fStartPos[1] = m_fStartPos[2] = 0.0f;
  }
};

struct  _character_create_setdata : _object_create_setdata
{
};

struct  _monster_create_setdata : _character_create_setdata
{
  _mon_active *pActiveRec;
  _dummy_position *pDumPosition;
  bool bDungeon;
  bool bRobExp;
  bool bRewardExp;
  CMonster *pParent;

  _monster_create_setdata()
  {
    pActiveRec = nullptr;
    pDumPosition = nullptr;
    bDungeon = false;
    bRobExp = false;
    bRewardExp = false;
    pParent = nullptr;
  }
};

struct  __declspec(align(8)) _npc_create_setdata : _character_create_setdata
{
  CItemStore *m_pLinkItemStore;
  unsigned __int8 m_byRaceCode;

  _npc_create_setdata()
  {
    m_pLinkItemStore = nullptr;
    m_byRaceCode = 0;
  }
};

struct  _keeper_create_setdata : _character_create_setdata
{
  int nMasterRace;
  _dummy_position *pPosCreate;
  _dummy_position *pPosActive;
  _dummy_position *pPosCenter;

  _keeper_create_setdata()
  {
    nMasterRace = -1;
    pPosCreate = nullptr;
    pPosActive = nullptr;
    pPosCenter = nullptr;
  }
};

struct  __declspec(align(8)) _animus_create_setdata : _character_create_setdata
{
  int nHP;
  int nFP;
  unsigned __int64 dwExp;
  CPlayer *pMaster;
  int nMaxAttackPnt;

  _animus_create_setdata();
};
