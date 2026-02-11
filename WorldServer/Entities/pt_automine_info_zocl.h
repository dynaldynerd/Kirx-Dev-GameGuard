#pragma once

#include "IdaCompat.h"
#include "DB_LOAD_AUTOMINE_MACHINE.h"

struct _pt_automine_info_zocl
{
  _DB_LOAD_AUTOMINE_MACHINE INFO;

  _pt_automine_info_zocl();
  int size();
};
