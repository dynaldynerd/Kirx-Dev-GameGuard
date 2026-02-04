#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "ObjectCreateSetData.h"

struct _monster_fld;
struct _dummy_position;
class CMapData;

struct __cppobj __declspec(align(8)) _stone_create_setdata : _character_create_setdata
{
  _dummy_position *pDumPosition;
  unsigned char byMasterRace;

  _stone_create_setdata()
  {
    pDumPosition = nullptr;
    byMasterRace = 0;
  }
};

class __cppobj CHolyStone : public CCharacter
{
public:
  bool Create(_stone_create_setdata *pData);
  static unsigned int GetNewStoneSerial();
  void SendMsg_Create();
  void SetOper(bool bOper, float fHPRate);
  void SendMsg_StoneAlterOper();
  unsigned int CalcCurHPRate();
  __int64 GetHP();
  char SetHP(int nHP, bool bOver = false);

  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;

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

  bool Init(_object_id *pID);
};
