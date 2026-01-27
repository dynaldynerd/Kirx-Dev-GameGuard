#include "pch.h"

#include "CAttack.h"

CRecordData *CAttack::s_pSkillData = nullptr;

void CAttack::SetStaticMember(CRecordData *effectTable)
{
  s_pSkillData = effectTable;
}
