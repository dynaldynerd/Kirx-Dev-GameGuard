#include "pch.h"

#include "log_sheet_lv.h"

int _log_sheet_lv::size()
{
  // narrowing cast for thunk return parity
  return static_cast<int>(12LL);
}
