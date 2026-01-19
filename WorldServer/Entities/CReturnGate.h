#pragma once

#include "IdaCompat.h"
#include "CGameObject.h"
#include "CUnmannedTraderSchedule.h"

class CPlayer;
class CMapData;

class __cppobj CReturnGate : public CGameObject
{
public:
  CUnmannedTraderSchedule::STATE m_eState;
  CPlayer *m_pkOwner;
  unsigned int m_dwOwnerSerial;
  CMapData *m_pDestMap;
  float m_fBindPos[3];
  unsigned int m_dwCloseTime;
};
