#pragma once
#include "IdaCompat.h"
#include "attack_gen_result_zocl.h"
#pragma pack(push, 1)
struct _attack_count_result_zocl{ unsigned int dwAtterSerial; unsigned __int8 byAttackPart; bool bCritical; bool bWPActive; unsigned __int8 byListNum; _attack_gen_result_zocl::_dam_list DamList[32]; _attack_count_result_zocl() : byListNum(0) {} int size() { if (byListNum > 0x20u) { byListNum = 0; } return 328 - 10 * (32 - byListNum); }};
#pragma pack(pop)
