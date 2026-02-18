#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _a_trade_clear_item_result_zocl{ unsigned __int8 byRetCode; unsigned __int16 wItemSerial; unsigned int dwRegistSerial; __int64 size() const;};
#pragma pack(pop)
inline __int64 _a_trade_clear_item_result_zocl::size() const{ if (byRetCode) { return 1; } return 7;}