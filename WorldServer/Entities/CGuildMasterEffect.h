#pragma once

#include "IdaCompat.h"

struct _guild_master_advantage_info
{
  float attack_value;
  float defence_value;
};

class __cppobj CGuildMasterEffect
{
public:
  unsigned __int8 m_byAdjustableGrade;
  _guild_master_advantage_info m_EffectData[8];
};
