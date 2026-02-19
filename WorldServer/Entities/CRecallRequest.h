#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderSchedule.h"

class CPlayer;

class  CRecallRequest
{
public:
  bool IsClose();
  CPlayer *GetOwner();

  unsigned __int16 m_usID;
  CUnmannedTraderSchedule::STATE m_eState;
  CPlayer *m_pkOwner;
  unsigned int m_dwOwnerSerial;
  CPlayer *m_pkDest;
  unsigned int m_dwDestSerial;
  unsigned int m_dwCloseTime;
  bool m_bRecallParty;
  bool m_bStone;
  bool m_bBattleModeUse;
};

