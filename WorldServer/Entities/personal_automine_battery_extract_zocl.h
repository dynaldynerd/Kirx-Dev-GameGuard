#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _personal_automine_battery_extract_zocl{ unsigned int dwObjSerial; unsigned __int16 wSerial; unsigned int dwDur; unsigned __int8 byRetCode; _personal_automine_battery_extract_zocl(); __int64 size() const;};
#pragma pack(pop)
inline _personal_automine_battery_extract_zocl::_personal_automine_battery_extract_zocl(){ memset_0(this, 0, sizeof(_personal_automine_battery_extract_zocl));}inline __int64 _personal_automine_battery_extract_zocl::size() const{ return 11;}