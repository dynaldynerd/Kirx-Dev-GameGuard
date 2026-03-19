#pragma once

#include "IdaCompat.h"

struct _qry_case_inputgmoney
{
  unsigned int in_pusherserial;
  char in_w_pushername[17];
  unsigned int tmp_guildindex;
  unsigned int in_guildserial;
  unsigned int dwAddGold;
  unsigned int dwAddDalant;
  unsigned __int8 in_date[4];
  long double out_totalgold;
  long double out_totaldalant;

  int size() const;
};

static_assert(sizeof(_qry_case_inputgmoney) == 64);

inline int _qry_case_inputgmoney::size() const
{
  return 64;
}
