#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _personal_automine_delbattery_zocl{ unsigned int dwObjSerial; unsigned __int8 bySlot; _personal_automine_delbattery_zocl(); unsigned __int16 size() const;};
#pragma pack(pop)
inline _personal_automine_delbattery_zocl::_personal_automine_delbattery_zocl(){ memset_0(this, 0, sizeof(_personal_automine_delbattery_zocl));}inline unsigned __int16 _personal_automine_delbattery_zocl::size() const{ return 5;}