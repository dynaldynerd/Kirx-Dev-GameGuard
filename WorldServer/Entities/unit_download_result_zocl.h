#pragma once
#include "IdaCompat.h"
#include "CPlayer.h"
#pragma pack(push, 1)
struct  _unit_download_result_zocl{ struct  _list { unsigned __int8 bySlotIndex; _UNIT_DB_BASE::_LIST UnitData; _list(); }; unsigned __int8 byUnitNum; _list UnitList[4]; _unit_download_result_zocl(); unsigned __int64 size();};
#pragma pack(pop)
