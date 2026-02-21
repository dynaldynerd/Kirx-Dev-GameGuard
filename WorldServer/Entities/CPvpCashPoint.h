#pragma once

#include "IdaCompat.h"
#include "CIndexList.h"

struct _PVP_ORDER_VIEW_DB_BASE;

/* 1795 */
class  CPvpCashPoint
{
public:
  CPvpCashPoint();
  ~CPvpCashPoint();
  void Init(_PVP_ORDER_VIEW_DB_BASE *pkInfo);
  void KillerListInit();
  void SendMsg_TalikList(unsigned int n);
  int GetTalikRecvrPoint(unsigned __int8 byTblCode, unsigned int dwIndex);
  int GetMaxTempPoint(unsigned __int8 nLv, bool bPremium);
  void SendMsg_RecoverResult(unsigned int n, char byRet, int nRecvrPoint);
  void SendMsg_PvpCashInform(unsigned int n, unsigned __int8 byError);

  CIndexList m_KillerList;
  unsigned __int8 m_byContPvpHave;
  unsigned __int8 m_byContPvpLose;
  bool m_bRaceWarRecvr;
};


