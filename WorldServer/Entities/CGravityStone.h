#pragma once

#include "IdaCompat.h"

#include "CGameObject.h"

class CPlayer;

class CGravityStone : public CGameObject
{
public:
  explicit CGravityStone(unsigned __int16 wInx);
  ~CGravityStone();
  static unsigned int ms_dwSerialCnt;

  bool IsValidOwner(CPlayer *pkPlayer);
  char Regen(_object_create_setdata *pParam);
  unsigned __int8 Drop(CPlayer *pkPlayer);
  void SendMsg_Create();
  void SendMsg_Destroy();
  void SendMsg_FixPosition(int n) override;
  void Destroy();
  void Clear();

  unsigned int m_dwTakeLimitTime;
  CPlayer *m_pkOwner;
};

