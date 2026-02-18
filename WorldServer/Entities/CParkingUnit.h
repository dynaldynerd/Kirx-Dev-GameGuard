#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"
#include "ObjectCreateSetData.h"

class CPlayer;

/* 2753 */
struct  __declspec(align(8)) _parkingunit_create_setdata : _object_create_setdata
{
  CPlayer *pOwner;
  unsigned __int8 byCreateType;
  unsigned __int8 byFrame;
  unsigned __int8 byPartCode[6];
  unsigned __int8 byTransDistCode;
  unsigned __int16 wHPRate;
};

/* 1773 */
class  __declspec(align(8)) CParkingUnit : public CGameObject
{
public:
  static unsigned int s_dwSerialCounter;
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
  char Create(_parkingunit_create_setdata *pParam);
  bool Destroy(unsigned __int8 byDestoryType);
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDestoryType);
  void SendMsg_ChangeOwner(unsigned __int8 byUnitSlotIndex, CPlayer *pOldOwner);
  void Loop() override;
  void SendMsg_FixPosition(int n) override;
};

