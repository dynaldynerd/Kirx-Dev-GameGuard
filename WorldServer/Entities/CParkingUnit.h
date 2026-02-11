#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"

class CPlayer;

/* 1773 */
class __cppobj __declspec(align(8)) CParkingUnit : public CGameObject
{
public:
  static int s_nLiveNum;

  CPlayer *m_pOwner;
  unsigned int m_dwOwnerSerial;
  unsigned __int8 m_byFrame;
  unsigned __int8 m_byPartCode[6];
  unsigned __int8 m_byCreateType;
  unsigned __int8 m_byTransDistCode;
  unsigned int m_dwParkingStartTime;
  unsigned __int16 m_wHPRate;
  unsigned int m_dwLastDestroyTime;

  void Init(_object_id *pID);
  bool Destroy(unsigned __int8 byDestoryType);
  void SendMsg_Destroy(unsigned __int8 byDestoryType);
};

