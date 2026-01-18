#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

/* 1742 */
struct __cppobj __declspec(align(8)) CGuardTower : CCharacter
{
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
};
