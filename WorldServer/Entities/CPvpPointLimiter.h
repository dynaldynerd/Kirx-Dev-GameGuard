#pragma once

#include "IdaCompat.h"

struct _PVPPOINT_LIMIT_DB_BASE;

/* 1793 */
class __cppobj CPvpPointLimiter
{
public:
  CPvpPointLimiter();
  ~CPvpPointLimiter();
  char Set(long double dOriginalPvpPoint, _PVPPOINT_LIMIT_DB_BASE *pkInfo);
  void CheatUpdate(long double dOriginalPvpPoint);

private:
  void Update(__int64 tUpdateTime, long double dOriginalPvpPoint, double dUsePoint, int unused);

  _PVPPOINT_LIMIT_DB_BASE *m_pkInfo;
};

