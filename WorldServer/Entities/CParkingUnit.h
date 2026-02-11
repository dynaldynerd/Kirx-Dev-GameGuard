#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"

class CPlayer;
struct _parkingunit_create_setdata;

/* 1773 */
class __cppobj __declspec(align(8)) CParkingUnit : CGameObject
{
public:
  static int s_nLiveNum;
  static unsigned int s_dwSerialCounter;

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
  bool Create(_parkingunit_create_setdata *pParam);
  bool Destroy(unsigned __int8 byDestoryType);
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDestoryType);
};

CParkingUnit *FindEmptyParkingUnit(CParkingUnit *pItem, int nMax);

