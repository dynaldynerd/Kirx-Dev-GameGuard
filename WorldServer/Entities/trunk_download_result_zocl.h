#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _trunk_download_result_zocl{ struct _list { unsigned __int16 wSerial; unsigned __int8 byTableCode; unsigned __int16 wItemIndex; unsigned __int64 dwDurPoint; unsigned __int8 byClientIndex; unsigned int dwUptInfo; unsigned __int8 byRace; unsigned __int8 byCsMethod; unsigned int dwT; }; unsigned __int8 byRetCode; long double dDalant; long double dGold; unsigned __int8 byListNum; unsigned __int8 byPackNum; unsigned __int8 byExtListNum; unsigned __int8 byExtPackNum; _list ExtList[40]; _list List[100]; _trunk_download_result_zocl(); unsigned __int16 size();};
#pragma pack(pop)
inline _trunk_download_result_zocl::_trunk_download_result_zocl(){ memset_0(this, 0, sizeof(*this)); byListNum = 0; byExtListNum = 0;}inline unsigned __int16 _trunk_download_result_zocl::size(){ if (byRetCode) { return 1; } if (byListNum > 100u) { byListNum = 0; } return static_cast<unsigned __int16>(3381 - 24 * (100 - byListNum));}