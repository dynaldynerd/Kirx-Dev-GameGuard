#pragma once

#include "IdaCompat.h"

struct _PVPPOINT_LIMIT_DB_BASE;
class CPlayer;

/* 1793 */
class  CPvpPointLimiter
{
public:
  CPvpPointLimiter();
  ~CPvpPointLimiter();
  bool Set(long double dOriginalPvpPoint, _PVPPOINT_LIMIT_DB_BASE *pkInfo);
  bool TakePvpPoint(long double *dPvpPoint, CPlayer *pkSelf, CPlayer *pkDest);
  void CheatUpdate(long double dOriginalPvpPoint);
  void Clear(__int64 tUpdateTime, long double dOriginalPvpPoint, CPlayer *pkSelf);

private:
  void Update(__int64 tUpdateTime, long double dOriginalPvpPoint, double dUsePoint);

  _PVPPOINT_LIMIT_DB_BASE *m_pkInfo;
};

