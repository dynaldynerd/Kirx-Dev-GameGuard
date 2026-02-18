#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _darkhole_create_zocl{ unsigned __int16 wQuestIndex; unsigned __int16 wIndex; unsigned int dwSerial; short zPos[3]; unsigned __int8 bHurry; char wszOpenerName[17]; unsigned int dwOpenerSerial; _darkhole_create_zocl(); unsigned __int16 size() const;};
#pragma pack(pop)
inline _darkhole_create_zocl::_darkhole_create_zocl(){ memset_0(this, 0, sizeof(_darkhole_create_zocl));}inline unsigned __int16 _darkhole_create_zocl::size() const{ return 36;}