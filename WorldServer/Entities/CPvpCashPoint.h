#pragma once

#include "IdaCompat.h"
#include "CIndexList.h"

struct _PVP_ORDER_VIEW_DB_BASE;
class CPlayer;

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
  int GetMyClassVal(char *pClass);
  long double CalPvpCashPoint(int nDstLv, int nSrcLv, char *pSrcClass, unsigned __int8 nParty);
  bool IsPvpMap(CPlayer *pOne);
  bool CheckPvpLoseCondition(CPlayer *pKiller, CPlayer *pDier);
  bool CheckPvpHaveCondition(CPlayer *pKiller, CPlayer *pDier, double dOldTempPoint);
  int GetMaxTempPoint(unsigned __int8 nLv, bool bPremium);
  int GetMinTempPoint(unsigned __int8 nLv);
  int GetContPvpHave();
  int GetContPvpLose();
  void SetContPvpHave(unsigned __int8 byCnt);
  void SetContPvpLose(unsigned __int8 byCnt);
  bool GetRaceWarRecvr();
  void SetRaceWarRecvr(bool bUsed);
  bool SetKillerList(unsigned int dwKillerSerial);
  void UpdateKillerList(_PVP_ORDER_VIEW_DB_BASE *pkInfo);
  void SendMsg_RecoverResult(unsigned int n, char byRet, int nRecvrPoint);
  void SendMsg_PvpCashInform(unsigned int n, unsigned __int8 byError);

  CIndexList m_KillerList;
  unsigned __int8 m_byContPvpHave;
  unsigned __int8 m_byContPvpLose;
  bool m_bRaceWarRecvr;
};


