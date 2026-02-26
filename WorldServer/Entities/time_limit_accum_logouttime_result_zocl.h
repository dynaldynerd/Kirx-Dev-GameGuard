#pragma once

struct _time_limit_accum_logouttime_result_zocl
{
  int bEnable;
  unsigned int dwAccumLogoutTime;
};

static_assert(sizeof(_time_limit_accum_logouttime_result_zocl) == 8);
