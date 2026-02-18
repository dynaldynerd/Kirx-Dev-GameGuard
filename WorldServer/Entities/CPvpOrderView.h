#pragma once

#include "IdaCompat.h"

struct _PVP_ORDER_VIEW_DB_BASE;
class CPlayer;

#pragma pack(push, 1)
struct  _pvp_order_view_inform_zocl
{
  int nKillCnt;
  int nDeahtCnt;
  long double dTodayPvpPoint;
  long double dOriginalPvpPoint;

  _pvp_order_view_inform_zocl();
  int size() const;
};
#pragma pack(pop)

/* 1794 */
class CPvpOrderView
{
public:
  long double GetPvpCash();
  long double GetPvpTempCash();
  void SetPvpCash(long double dAlter);
  char SetPvpOrderView(long double dPvpPoint, _PVP_ORDER_VIEW_DB_BASE *pkInfo, CPlayer *pOne);
  void UpdatePvpCash(double dPvpCash);
  void Update_PvpTempCash(unsigned __int16 wIndex, double dTempPvpCash);
  void UpdatePvPPoint(long double dUpPoint, long double dNewPoint);
  void Notify_OrderView(unsigned __int16 wIndex);
  void Notify_Point(unsigned __int16 wIndex, long double dChangePoint, unsigned int dwTarSerial);
  void Notify_PvPEnd(unsigned __int16 wIndex);
  void Notify_PvpTempCash(unsigned __int16 wIndex);
  void Update_RaceWarRecvr(bool bUse);
  void ResetPvPOrderView();
  void Update(
    __int64 tUpdateDate,
    int nDeath,
    int nKill,
    long double dTodayStacked,
    long double dPvpPoint,
    long double dPvpTempCash);

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

