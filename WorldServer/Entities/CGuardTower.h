#pragma once

#include "IdaCompat.h"
#include "ObjectCreateSetData.h"
#include "CPlayer.h"
#include "TEMP_WAIT_TOWER.h"

struct __cppobj __declspec(align(4)) _tower_create_setdata : _character_create_setdata
{
  int nHP;
  CPlayer *pMaster;
  unsigned __int8 byRaceCode;
  _STORAGE_LIST::_db_con *pItem;
  int nIniIndex;
  bool bQuick;

  _tower_create_setdata()
  {
    nHP = 0;
    pMaster = nullptr;
    byRaceCode = 0;
    pItem = nullptr;
    nIniIndex = 0;
    bQuick = false;
  }
};

/* 1742 */
class __cppobj __declspec(align(8)) CGuardTower : public CCharacter
{
public:
  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;
  static __TEMP_WAIT_TOWER s_Temp[MAX_PLAYER];

  int m_nHP;
  CPlayer *m_pMasterTwr;
  unsigned int m_dwMasterSerial;
  unsigned __int8 m_byRaceCode;
  _STORAGE_LIST::_db_con *m_pItem;
  unsigned __int16 m_wItemSerial;
  bool m_bSystemStruct;
  int m_nIniIndex;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bQuick;
  CCharacter *m_pTarget;
  CCharacter *m_pMasterSetTarget;
  unsigned int m_dwLastDestroyTime;

  void Init(_object_id *pID);
  bool Destroy(unsigned __int8 byDesType, bool bSystemBack);
  void SendMsg_Destroy(unsigned __int8 byDesType);
  void SendMsg_Create();
  static __int64 GetNewSerial();
};

CGuardTower *CreateSystemTower(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  int nTowerIndex,
  unsigned __int8 byRaceCode,
  int nIniIndex);

CGuardTower *CreateGuardTower(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  _STORAGE_LIST::_db_con *pItem,
  CPlayer *pMaster,
  unsigned __int8 byRaceCode,
  bool bQuick);

bool IsOtherTowerNear(CGameObject *pEster, float *pfEstPos, CGuardTower *pEstObj);

