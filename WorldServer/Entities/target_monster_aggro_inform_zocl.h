#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _target_monster_aggro_inform_zocl{ struct _aggro_node { unsigned __int8 m_IsData; char m_Name[32]; int m_nAggroData; int m_nDamageData; int m_nKingPowerDamage; }; unsigned int dwSerial; _aggro_node m_AggroNode[10]; float fTimer; _target_monster_aggro_inform_zocl(); void Init();};
#pragma pack(pop)
