#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _pvp_cash_recover_itemlist_result_zocl{ unsigned __int8 byItemNum; int nTalikInfo[14]; _pvp_cash_recover_itemlist_result_zocl() { memset_0(this, 0, sizeof(_pvp_cash_recover_itemlist_result_zocl)); } unsigned __int16 size() { if (byItemNum > 14u) { byItemNum = 0; } return static_cast<unsigned __int16>(1 + (4 * byItemNum)); }};
#pragma pack(pop)
