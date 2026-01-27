#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

struct _monster_fld;
struct _dummy_position;
class CMapData;

struct _stone_create_setdata
{
  CMapData *m_pMap;
  int m_nLayerIndex;
  _monster_fld *m_pRecordSet;
  float m_fStartPos[3];
  _dummy_position *pDumPosition;
  unsigned char byMasterRace;
};

class __cppobj CHolyStone : public CCharacter
{
public:
  static bool Create(CHolyStone *pStone, _stone_create_setdata *pData);
  bool m_bOper;
  int m_nHP;
  int m_nMaxHP;
  unsigned int m_dwLastDestroyTime;
  _monster_fld *m_pRec;
  _dummy_position *m_pDum;
  unsigned __int8 m_byMasterRaceCode;
  int m_nDefPart[5];
  int m_nOldRate;
  unsigned int m_dwLastRecoverTime;
  int m_nCurrLootIndex;
  int m_nEndLootIndex;
  int m_nCurrDropIndex;
  unsigned __int16 m_wMagnifications;
  unsigned __int16 m_wRange;
  unsigned __int16 m_wDropCntOnce;
  unsigned __int16 m_wAddCountWithPlayer;
  CMyTimer m_tmrDropTime;

  void Init(_object_id *pID);
};
