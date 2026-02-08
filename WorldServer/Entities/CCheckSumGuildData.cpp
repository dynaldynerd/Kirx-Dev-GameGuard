#include "pch.h"

#include "CCheckSumGuildData.h"

#include "CCheckSumGuildConverter.h"
#include "CRFWorldDatabase.h"

void CCheckSumGuildData::Encode(long double dDalant, long double dGold)
{
  CCheckSumGuildConverter converter{};
  CCheckSumGuildConverter::Convert(&converter, dDalant, dGold, this);
}

bool CCheckSumGuildData::Insert(CRFWorldDatabase *pkDB)
{
  return pkDB && pkDB->Insert_UnitData(m_dwGuildSerial, m_dValues);
}

bool CCheckSumGuildData::Update(CRFWorldDatabase *pkDB)
{
  return pkDB && pkDB->Update_UnitData(m_dwGuildSerial, m_dValues);
}
