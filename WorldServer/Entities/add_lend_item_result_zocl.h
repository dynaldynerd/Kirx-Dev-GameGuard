#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _add_lend_item_result_zocl{ unsigned __int8 byTblCode; unsigned __int16 wItemIdx; unsigned int dwDur; unsigned int dwUp; unsigned int dwItemSerial; unsigned __int8 byCsMethod; unsigned int dwT; unsigned __int16 size() const { return static_cast<unsigned __int16>(sizeof(*this)); }};
#pragma pack(pop)
