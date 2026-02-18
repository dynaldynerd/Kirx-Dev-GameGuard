#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _darkhole_enter_result_zocl{ unsigned __int8 byRetCode; unsigned int dwHoleSerial; unsigned __int8 byMapCode; __int16 zPos[3]; unsigned __int16 size() const;};
#pragma pack(pop)
inline unsigned __int16 _darkhole_enter_result_zocl::size() const{ return 12;}