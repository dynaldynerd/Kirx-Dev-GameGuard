#pragma once

#include "IdaCompat.h"

class CPlayer;

struct _guild_master_advantage_info
{
  float attack_value;
  float defence_value;
};

class  CGuildMasterEffect
{
public:
  static CGuildMasterEffect *GetInstance();

  CGuildMasterEffect();
  char init();
  unsigned __int8 get_AdjustableGrade();
  unsigned __int8 get_AttactValueByGrade(unsigned __int8 byGrade);
  unsigned __int8 get_DefenceValueByGrade(unsigned __int8 byGrade);
  void adjust_effect(CPlayer *pP, unsigned __int8 byGrade, bool bAdd);
  char in_player(CPlayer *pP, unsigned __int8 byGrade);
  char out_player(CPlayer *pP, unsigned __int8 byGrade);
  char change_player(CPlayer *pP, unsigned __int8 byBeforeGrade, unsigned __int8 byAfterGrade);
  void show_to_all(CPlayer *pP, unsigned __int8 byBeforeGrade, unsigned __int8 byGrade, unsigned __int8 byState);

  unsigned __int8 m_byAdjustableGrade;
  _guild_master_advantage_info m_EffectData[8];
};
