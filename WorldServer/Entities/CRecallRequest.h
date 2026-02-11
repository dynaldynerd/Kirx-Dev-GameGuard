#pragma once

#include "IdaCompat.h"
#include "CUnmannedTraderSchedule.h"

class CPlayer;

class __cppobj __declspec(align(8)) CRecallRequest
{
public:
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

  CRecallRequest(unsigned __int16 usID);
  void Clear();
  unsigned __int8 Regist(CPlayer *pkObj, CPlayer *pkDest, bool bRecallParty, bool bStone, bool bBattleModeUse);
  unsigned __int8 Recall(CPlayer *pkDest, bool bStone);
  unsigned __int16 GetID();
  CPlayer *GetOwner();
  bool IsRecallParty();
  bool IsBattleModeUse();
  bool IsRecallAfterStoneState();
  bool IsTimeOut();
  bool IsWait();
  bool IsClose();
  void Close(bool bDone);
};
