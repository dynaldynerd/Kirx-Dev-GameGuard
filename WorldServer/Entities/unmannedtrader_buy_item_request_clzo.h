#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _unmannedtrader_buy_item_request_clzo{ struct __list { unsigned int dwRegistSerial; unsigned int dwPrice; }; unsigned __int8 byDivision; unsigned __int8 byClass; unsigned __int8 bySubClass; unsigned int dwVer; unsigned __int8 byStoreIndex; unsigned __int8 byNum; __list List[10]; int bUseNpcLink;};
#pragma pack(pop)
