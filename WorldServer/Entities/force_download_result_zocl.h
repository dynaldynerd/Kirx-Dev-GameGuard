#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _force_download_result_zocl{ struct _list { unsigned __int16 wItemIndex; unsigned int dwCum; unsigned __int8 byCsMethod; unsigned int dwT; }; unsigned __int8 byRetCode; unsigned __int8 bySlotNum; _list ItemSlotInfo[88]; _force_download_result_zocl(); unsigned __int64 size();};
#pragma pack(pop)
