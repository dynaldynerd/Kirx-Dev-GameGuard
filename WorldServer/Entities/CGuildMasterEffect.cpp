#include "pch.h"

#include "CGuildMasterEffect.h"

#include "CPlayer.h"
#include "CGuild.h"

CGuildMasterEffect *CGuildMasterEffect::GetInstance()
{
  static CGuildMasterEffect s_instance;
  return &s_instance;
}

CGuildMasterEffect::CGuildMasterEffect()
{
  init();
}

char CGuildMasterEffect::init()
{
  m_EffectData[0].attack_value = 0.0f;
  m_EffectData[0].defence_value = 0.0f;
  m_EffectData[1].attack_value = 0.0f;
  m_EffectData[1].defence_value = 0.0f;
  m_EffectData[2].attack_value = 0.05f;
  m_EffectData[2].defence_value = 0.05f;
  m_EffectData[3].attack_value = 0.1f;
  m_EffectData[3].defence_value = 0.1f;
  m_EffectData[4].attack_value = 0.15000001f;
  m_EffectData[4].defence_value = 0.15000001f;
  m_EffectData[5].attack_value = 0.2f;
  m_EffectData[5].defence_value = 0.2f;
  m_EffectData[6].attack_value = 0.25f;
  m_EffectData[6].defence_value = 0.25f;
  m_EffectData[7].attack_value = 0.30000001f;
  m_EffectData[7].defence_value = 0.30000001f;
  m_byAdjustableGrade = 3;
  return 1;
}

unsigned __int8 CGuildMasterEffect::get_AdjustableGrade()
{
  return m_byAdjustableGrade;
}

unsigned __int8 CGuildMasterEffect::get_AttactValueByGrade(unsigned __int8 byGrade)
{
  if (byGrade && byGrade >= m_byAdjustableGrade && byGrade <= 8u)
  {
    return static_cast<unsigned __int8>(m_EffectData[byGrade - 1].attack_value * 100.0f);
  }
  return 0;
}

unsigned __int8 CGuildMasterEffect::get_DefenceValueByGrade(unsigned __int8 byGrade)
{
  if (byGrade && byGrade >= m_byAdjustableGrade && byGrade <= 8u)
  {
    return static_cast<unsigned __int8>(m_EffectData[byGrade - 1].defence_value * 100.0f);
  }
  return 0;
}

void CGuildMasterEffect::adjust_effect(CPlayer *pP, unsigned __int8 byGrade, bool bAdd)
{
  if (pP && byGrade && byGrade >= m_byAdjustableGrade)
  {
    pP->apply_normal_item_std_effect(6, m_EffectData[byGrade - 1].attack_value, bAdd);
    pP->apply_normal_item_std_effect(7, m_EffectData[byGrade - 1].defence_value, bAdd);
  }
}

char CGuildMasterEffect::in_player(CPlayer *pP, unsigned __int8 byGrade)
{
  if (!pP || !byGrade || byGrade < m_byAdjustableGrade || byGrade > 8u)
  {
    return 0;
  }
  if (!pP->m_Param.m_pGuild)
  {
    return 0;
  }
  if (pP->m_Param.m_byClassInGuild != 2)
  {
    return 0;
  }
  if (pP->m_Param.m_pGuild->GetGrade() != byGrade)
  {
    return 0;
  }
  adjust_effect(pP, byGrade, true);
  show_to_all(pP, 0, byGrade, 0);
  return 1;
}

char CGuildMasterEffect::out_player(CPlayer *pP, unsigned __int8 byGrade)
{
  if (!pP || !byGrade || byGrade < m_byAdjustableGrade || byGrade > 8u)
  {
    return 0;
  }
  if (!pP->m_Param.m_pGuild)
  {
    return 0;
  }
  adjust_effect(pP, byGrade, false);
  show_to_all(pP, 0, byGrade, 1);
  return 1;
}

char CGuildMasterEffect::change_player(
  CPlayer *pP,
  unsigned __int8 byBeforeGrade,
  unsigned __int8 byAfterGrade)
{
  if (!pP || byBeforeGrade > 8u || byAfterGrade > 8u)
  {
    return 0;
  }
  if (byBeforeGrade == byAfterGrade)
  {
    return 1;
  }
  if (!pP->m_Param.m_pGuild)
  {
    return 0;
  }
  if (pP->m_Param.m_byClassInGuild != 2)
  {
    return 0;
  }
  adjust_effect(pP, byBeforeGrade, false);
  if (byAfterGrade >= m_byAdjustableGrade)
  {
    adjust_effect(pP, byAfterGrade, true);
  }
  show_to_all(pP, byBeforeGrade, byAfterGrade, 2u);
  return 1;
}

void CGuildMasterEffect::show_to_all(
  CPlayer *pP,
  unsigned __int8 byBeforeGrade,
  unsigned __int8 byGrade,
  unsigned __int8 byState)
{
  if (!pP)
  {
    return;
  }
  const unsigned __int8 byEffAddDefence = get_DefenceValueByGrade(byGrade);
  const unsigned __int8 byEffAddAttack = get_AttactValueByGrade(byGrade);
  const unsigned __int8 byEffSubDefence = get_DefenceValueByGrade(byBeforeGrade);
  const unsigned __int8 byEffSubAttack = get_AttactValueByGrade(byBeforeGrade);
  pP->SendMsg_GuildMasterEffect(byState, byGrade, byEffSubAttack, byEffSubDefence, byEffAddAttack, byEffAddDefence);
  pP->SenseState();
}
