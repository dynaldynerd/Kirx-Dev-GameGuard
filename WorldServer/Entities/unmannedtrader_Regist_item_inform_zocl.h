#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _unmannedtrader_Regist_item_inform_zocl{ struct __list { unsigned __int16 wItemSerial; unsigned int dwRegistSerial; unsigned int dwPrice; unsigned int dwLeftSec; unsigned int dwListIndex; }; unsigned __int8 byNum; __list List[10]; _unmannedtrader_Regist_item_inform_zocl(); __int64 size();};
#pragma pack(pop)
inline _unmannedtrader_Regist_item_inform_zocl::_unmannedtrader_Regist_item_inform_zocl(){ byNum = 0;}inline __int64 _unmannedtrader_Regist_item_inform_zocl::size(){ if (byNum > 0xAu) { byNum = 0; } return 181 - 18LL * (10 - byNum);}