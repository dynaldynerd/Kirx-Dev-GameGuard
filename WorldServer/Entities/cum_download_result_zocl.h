#pragma once
#include "IdaCompat.h"
#include "CPlayer.h"
#pragma pack(push, 1)
struct _cum_download_result_zocl{ unsigned __int8 byRetCode; _STAT_DB_BASE Stat; unsigned __int8 byLeftCuttingResNum; unsigned __int16 wleftResList[20]; _cum_download_result_zocl(); unsigned __int64 size();};
#pragma pack(pop)
