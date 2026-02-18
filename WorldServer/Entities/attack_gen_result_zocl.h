#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _attack_gen_result_zocl{ struct _dam_list { unsigned __int8 byDstID; unsigned int dwDstSerial; unsigned __int16 wDamage; bool bActive; unsigned __int16 wActiveDamage; }; unsigned __int8 byAtterID; unsigned int dwAtterSerial; unsigned __int8 byAttackPart; unsigned __int16 wBulletIndex; bool bCritical; bool bWPActive; unsigned __int8 byListNum; _dam_list DamList[32]; _attack_gen_result_zocl() : byListNum(0) {} int size() { if (byListNum > 0x20u) { byListNum = 0; } return 331 - 10 * (32 - byListNum); }};
#pragma pack(pop)
