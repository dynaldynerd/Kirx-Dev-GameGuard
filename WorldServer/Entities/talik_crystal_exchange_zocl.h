#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _talik_crystal_exchange_zocl{ struct _remain_item { unsigned __int8 byRemainNum; }; unsigned __int8 byErrorCode; _remain_item RemainItem[24]; _talik_crystal_exchange_zocl() { memset_0(this, 0, sizeof(*this)); } __int64 size() { return 25; }};
#pragma pack(pop)
