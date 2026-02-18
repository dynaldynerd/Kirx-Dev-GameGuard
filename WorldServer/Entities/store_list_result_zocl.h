#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _store_list_result_zocl{ struct _store_pos { unsigned int dwStoreIndex; float fPos[3]; }; unsigned __int8 byStoreNum; _store_pos StorePos[100]; _store_list_result_zocl() : byStoreNum(0) {} unsigned __int16 size() { if (byStoreNum > 0x64u) { byStoreNum = 0; } return static_cast<unsigned __int16>(1601 - 16LL * (100 - byStoreNum)); }};
#pragma pack(pop)
