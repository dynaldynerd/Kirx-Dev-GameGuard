#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _trunk_res_division_result_zocl{ unsigned __int8 byErrCode; unsigned __int16 wParentSerial; unsigned __int16 wParentAmount; unsigned __int16 wChildSerial; unsigned __int16 wChildAmount; _trunk_res_division_result_zocl();};
#pragma pack(pop)
inline _trunk_res_division_result_zocl::_trunk_res_division_result_zocl(){ memset_0(this, 0, sizeof(*this));}