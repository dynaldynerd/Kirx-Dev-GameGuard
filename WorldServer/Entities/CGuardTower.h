#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"
#include "TEMP_WAIT_TOWER.h"

/* 1742 */
class __cppobj __declspec(align(8)) CGuardTower : CCharacter
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

