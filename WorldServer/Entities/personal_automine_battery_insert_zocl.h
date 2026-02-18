#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _personal_automine_battery_insert_zocl{ unsigned int dwObjSerial; unsigned int dwOwnerSerial; unsigned __int16 wItemSerial; _personal_automine_battery_insert_zocl(); __int64 size() const;};
#pragma pack(pop)
inline _personal_automine_battery_insert_zocl::_personal_automine_battery_insert_zocl(){ memset_0(this, 0, sizeof(_personal_automine_battery_insert_zocl));}inline __int64 _personal_automine_battery_insert_zocl::size() const{ return 10;}