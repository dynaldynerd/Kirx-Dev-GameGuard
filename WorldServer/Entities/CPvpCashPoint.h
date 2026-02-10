#pragma once

#include "IdaCompat.h"

struct _PVP_ORDER_VIEW_DB_BASE;

/* 1795 */
class __cppobj __declspec(align(8)) CPvpCashPoint
{
public:
  CPvpCashPoint();
  ~CPvpCashPoint();
  void Init(_PVP_ORDER_VIEW_DB_BASE *pkInfo);
  void KillerListInit();

  CIndexList m_KillerList;
  unsigned __int8 m_byContPvpHave;
  unsigned __int8 m_byContPvpLose;
  bool m_bRaceWarRecvr;
};

