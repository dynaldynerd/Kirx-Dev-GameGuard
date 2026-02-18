#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _a_trade_adjust_price_result_zocl{ unsigned __int8 byRetCode; unsigned __int8 byTaxRate; unsigned int dwLeftDalant; unsigned int dwTax; unsigned __int16 wItemSerial; unsigned int dwNewPrice; unsigned int dwRegistSerial; __int64 size() const;};
#pragma pack(pop)
inline __int64 _a_trade_adjust_price_result_zocl::size() const{ if (byRetCode) { return 6; } return 20;}