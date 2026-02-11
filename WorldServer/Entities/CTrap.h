#pragma once

#include "IdaCompat.h"
#include "CCharacter.h"
#include "ObjectCreateSetData.h"

class CPlayer;
struct _TrapItem_fld;

struct __cppobj __declspec(align(8)) _trap_create_setdata : _character_create_setdata
{
  int nHP;
  CPlayer *pMaster;
  int nTrapMaxAttackPnt;

  _trap_create_setdata()
  {
    nHP = 0;
    pMaster = nullptr;
    nTrapMaxAttackPnt = 0;
  }
};

/* 1746 */
class __cppobj CTrap : public CCharacter
{
public:
  int m_nHP;
  CPlayer *m_pMaster;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwMasterSerial;
  char m_wszMasterName[17];
  char m_aszMasterName[17];
  long double m_dMasterPvPPoint;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bBreakTransparBuffer;
  unsigned int m_dwLastDestroyTime;
  int m_nTrapMaxAttackPnt;

  void Init(_object_id *pID);
  void MasterNetClose(long double dPvPPoint);
};

CTrap *CreateTrap(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  CPlayer *pMaster,
  int nTrapItemIndex);

unsigned __int8 IsOtherInvalidObjNear(
  CGameObject *pEster,
  float *pfEstPos,
  CTrap *pEstObj,
  _TrapItem_fld *pEstTrapItemInfo);

