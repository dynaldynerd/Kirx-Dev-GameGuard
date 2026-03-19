#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"
#include "ObjectCreateSetData.h"

class CPlayer;

/* 2753 */
struct  _parkingunit_create_setdata : _object_create_setdata
{
  CPlayer *pOwner;
  unsigned __int8 byCreateType;
  unsigned __int8 byFrame;
  unsigned __int8 byPartCode[6];
  unsigned __int8 byTransDistCode;
  unsigned __int16 wHPRate;
};

/* 1773 */
class  CParkingUnit : public CGameObject
{
public:
  CParkingUnit();
  ~CParkingUnit();

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
  bool Create(_parkingunit_create_setdata *pParam);
  bool Destroy(unsigned __int8 byDestoryType);
  unsigned __int16 CalcCurHPRate() override;
  void ChangeOwner(CPlayer *pNewOwner, unsigned __int8 byUnitSlotIndex);
  bool IsRideRight(CPlayer *pOne);
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDestoryType);
  void SendMsg_ChangeOwner(unsigned __int8 byUnitSlotIndex, CPlayer *pOldOwner);
  void Loop() override;
  void SendMsg_FixPosition(int n) override;
};


