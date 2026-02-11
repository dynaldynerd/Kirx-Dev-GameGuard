#pragma once

#include "IdaCompat.h"

struct __unaligned __declspec(align(2)) _atrade_taxrate_result_zocl
{
  unsigned __int8 byTaxRate;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  char wszGuildName[17];

  __int64 size();
};
