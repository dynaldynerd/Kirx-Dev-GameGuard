#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _create_holy_master_zocl{ unsigned __int8 byHolyStoneRaceCode; unsigned __int8 byPlayerRaceCode; unsigned __int8 byNameLen; char wszName[17]; int nControlSec; unsigned int dwObjSerial; unsigned __int16 size() const { return static_cast<unsigned __int16>(sizeof(_create_holy_master_zocl)); }};
#pragma pack(pop)
