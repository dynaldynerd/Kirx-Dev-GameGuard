#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _result_csi_buy_zocl{ struct __item { unsigned __int8 byTblCode; unsigned __int16 wItemIdx; unsigned int dwDur; unsigned int dwUp; unsigned int dwItemSerial; unsigned __int8 byCsMethod; unsigned int dwT; }; int nCashAmount; unsigned __int8 nNum; bool bAdjustDiscount; __item item[40]; _result_csi_buy_zocl() { nCashAmount = 0; nNum = 0; } unsigned __int16 size() const { return static_cast<unsigned __int16>(806 - 20LL * (40 - nNum)); }};
#pragma pack(pop)
