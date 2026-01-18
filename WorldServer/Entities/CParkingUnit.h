#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"

struct CPlayer;

/* 1773 */
struct __cppobj __declspec(align(8)) CParkingUnit : CGameObject
{
  CPlayer *m_pOwner;
  unsigned int m_dwOwnerSerial;
  unsigned __int8 m_byFrame;
  unsigned __int8 m_byPartCode[6];
  unsigned __int8 m_byCreateType;
  unsigned __int8 m_byTransDistCode;
  unsigned int m_dwParkingStartTime;
  unsigned __int16 m_wHPRate;
  unsigned int m_dwLastDestroyTime;
};
