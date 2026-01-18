#pragma once

#include "IdaCompat.h"

/* 1794 */
struct CPvpOrderView
{
  unsigned int m_dwLastAttackTime;
  unsigned int m_dwLastDamagedTime;
  int m_nKillCnt;
  int m_nDeahtCnt;
  long double m_dTodayPvpPoint;
  long double m_dOriginalPvpPoint;
  long double m_dPvpPoint;
  long double m_dPvpTempCash;
  long double m_dPvpCash;
  bool m_bAttack;
  bool m_bDamaged;
  _PVP_ORDER_VIEW_DB_BASE *m_pkInfo;
};
