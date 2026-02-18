#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _guild_battle_get_gravity_stone_result_zocl{ unsigned __int8 byRet; char wszGuildName[17]; char wszCharName[17]; int iTakePortalInx; unsigned __int8 byColor; unsigned __int16 size() const { return static_cast<unsigned __int16>(sizeof(_guild_battle_get_gravity_stone_result_zocl)); }};
#pragma pack(pop)
