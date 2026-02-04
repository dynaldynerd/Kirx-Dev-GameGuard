#pragma once

#include "IdaCompat.h"

#include <unordered_map>

struct _CashShop_fld;

class __cppobj CashItemRemoteStore
{
public:
  static CashItemRemoteStore *Instance();
  bool Initialize();
  static const _CashShop_fld *FindCashRec(unsigned int nTbl, int nIdx);

private:
  static std::unordered_map<int, const _CashShop_fld *> s_cashTables[0x25];
};
