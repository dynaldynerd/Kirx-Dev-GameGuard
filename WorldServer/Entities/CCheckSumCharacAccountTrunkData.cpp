#include "pch.h"

#include "CCheckSumCharacAccountTrunkData.h"

#include "CCheckSumCharacTrunkConverter.h"
#include "CRFWorldDatabase.h"

void CCheckSumCharacAccountTrunkData::Encode(_AVATOR_DATA *pAvator)
{
  CCheckSumCharacTrunkConverter converter{};
  CCheckSumCharacTrunkConverter::Convert(&converter, pAvator, this);
}

bool CCheckSumCharacAccountTrunkData::Update(CRFWorldDatabase *pkDB)
{
  if (!pkDB)
  {
    return false;
  }

  return pkDB->Update_NpcData(m_dwSerial, m_dwValues)
      && pkDB->Update_AnimusData(m_dwAccountSerial, m_byRace, m_dValues);
}
