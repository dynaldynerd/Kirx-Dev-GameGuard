#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _darkhole_member_info_inform_zocl{ struct __list { char wszName[17]; unsigned int dwSerial; }; unsigned int dwLeaderSerial; unsigned __int16 wMemberNum; __list List[32]; _darkhole_member_info_inform_zocl() { wMemberNum = 0; } unsigned __int16 size();};
#pragma pack(pop)
inline unsigned __int16 _darkhole_member_info_inform_zocl::size(){ if (wMemberNum > 0x20u) { wMemberNum = 32; } return static_cast<unsigned __int16>(678 - 21 * (32 - wMemberNum));}