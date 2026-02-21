#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderSchedule.h"

class CPlayer;

class  CRecallRequest
{
public:
  CRecallRequest(unsigned __int16 usID);

  void Clear();
  void Close(bool bDone);
  unsigned __int16 GetID();
  unsigned __int8 Regist(
    CPlayer *pkObj,
    CPlayer *pkDest,
    bool bRecallParty,
    bool bStone,
    bool bBattleModeUse);
  bool IsWait();
  bool IsTimeOut();
  bool IsRecallParty();
  bool IsRecallAfterStoneState();
  bool IsBattleModeUse();
  unsigned __int8 Recall(CPlayer *pkDest, bool bStone);

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

