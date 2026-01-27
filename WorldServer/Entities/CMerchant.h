#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"

#include "ObjectCreateSetData.h"

class CMerchant : public CCharacter
{
public:
  CItemStore *m_pItemStore;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwLastDestroyTime;
  int m_nLeftTicketNum[2];

  static int s_nLiveNum;

  void Init(_object_id *pID);
  bool Create(_npc_create_setdata *pData);
  static unsigned int GetNewMonSerial();
  static void SendMsg_Create(CMerchant *pMerchant);
};
