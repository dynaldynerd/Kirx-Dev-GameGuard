#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _darkhole_open_result_zocl{ unsigned __int8 byRetCode; unsigned __int16 wHoleIndex; unsigned int dwHoleSerial; unsigned __int16 size() const;};
#pragma pack(pop)
inline unsigned __int16 _darkhole_open_result_zocl::size() const{ return 7;}