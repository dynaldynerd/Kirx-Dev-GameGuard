#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _inven_download_result_zocl{ struct _list { unsigned __int8 byTableCode; unsigned __int16 wItemIndex; unsigned __int64 dwDurPoint; char sClientIndex; unsigned int dwUptInfo; unsigned __int8 byCsMethod; unsigned int dwT; }; unsigned __int8 byRetCode; unsigned __int8 byBagNum; unsigned __int8 bySlotNum; _list ItemSlotInfo[100]; _inven_download_result_zocl(); unsigned __int64 size();};
#pragma pack(pop)
