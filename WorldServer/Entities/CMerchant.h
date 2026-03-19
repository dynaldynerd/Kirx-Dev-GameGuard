#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

#include "ObjectCreateSetData.h"

class CMerchant : public CCharacter
{
public:
  CMerchant();
  ~CMerchant();

  int GetFireTol() override;
  char *GetObjName() override;
  int GetObjRace() override;
  int GetSoilTol() override;
  int GetWaterTol() override;
  int GetWindTol() override;
  void Loop() override;
  void OutOfSec() override;
  void SendMsg_FixPosition(int n) override;
  void SendMsg_RealFixPosition(bool bCircle) override;
  void SendMsg_RealMovePoint(int n) override;
  _dummy_position *GetStoreDummyName();

  CItemStore *m_pItemStore;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwLastDestroyTime;
  int m_nLeftTicketNum[2];

  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;

  bool Init(_object_id *pID);
  bool Create(_npc_create_setdata *pData);
  bool Destroy(CGameObject *pAttObj);
  static unsigned int GetNewMonSerial();
  void SendMsg_Create();
  void SendMsg_Destroy();
  void SendMsg_Move();
  void SendMsg_TransShipTicketNumInform(unsigned int n);
};
