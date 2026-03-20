#include "pch.h"

#include "pt_automine_info_zocl.h"

_DB_LOAD_AUTOMINE_MACHINE::_DB_LOAD_AUTOMINE_MACHINE()
{
  std::memset(this, 0, sizeof(_DB_LOAD_AUTOMINE_MACHINE));
}

int _DB_LOAD_AUTOMINE_MACHINE::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(977 - 12LL * (80 - this->bySlotCnt));
}

_pt_automine_info_zocl::_pt_automine_info_zocl()
{
  this->INFO = _DB_LOAD_AUTOMINE_MACHINE();
}

int _pt_automine_info_zocl::size()
{
  return static_cast<int>(this->INFO.size());
}
