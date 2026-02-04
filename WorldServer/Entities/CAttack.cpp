#include "pch.h"

#include "CAttack.h"
#include "CMainThread.h"

#include <cstring>

CRecordData *CAttack::s_pSkillData = nullptr;
_attack_param CAttack::s_DefParam = {};

CAttack::CAttack(CCharacter *pThis)
  : m_pp(&s_DefParam),
    m_pAttChar(pThis),
    m_bIsCrtAtt(false),
    m_nDamagedObjNum(0),
    m_bFailure(false)
{
  std::memset(m_DamList, 0, sizeof(m_DamList));
}

void CAttack::SetStaticMember(CRecordData *effectTable)
{
  s_pSkillData = effectTable;
}
