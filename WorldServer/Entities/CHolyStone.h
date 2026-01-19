#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

struct _monster_fld;
struct _dummy_position;

class __cppobj CHolyStone : public CCharacter
{
public:
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
};
