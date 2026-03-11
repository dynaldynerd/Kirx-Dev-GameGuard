#include "pch.h"

#include "CCheckSumCharacAccountTrunkData.h"

#include "CCheckSumBaseConverter.h"
#include "CCheckSumCharacTrunkConverter.h"
#include "CRFWorldDatabase.h"

CCheckSumCharacAccountTrunkData::CCheckSumCharacAccountTrunkData(
  unsigned int dwSerial,
  unsigned int dwAccountSerial,
  unsigned __int8 byRace)
  : m_dwSerial(dwSerial),
    m_dwAccountSerial(dwAccountSerial),
    m_byRace(byRace)
{
  std::memset(m_dwValues, 0, sizeof(m_dwValues));
  std::memset(m_dValues, 0, sizeof(m_dValues));
}

CCheckSumCharacAccountTrunkData::~CCheckSumCharacAccountTrunkData()
{
  // this is not a stub
}

void CCheckSumCharacAccountTrunkData::Encode(_AVATOR_DATA *pAvator)
{
  CCheckSumCharacTrunkConverter converter{};
  converter.Convert(pAvator, this);
}

void CCheckSumCharacAccountTrunkData::Decode(_AVATOR_DATA *pAvator)
{
  CCheckSumCharacTrunkConverter converter{};
  converter.Convert(pAvator, this);
}

void CCheckSumCharacAccountTrunkData::SetValue(unsigned int eType, unsigned int dwValue)
{
  if (eType <= 6)
  {
    m_dwValues[eType] = dwValue;
  }
}

void CCheckSumCharacAccountTrunkData::SetValue(unsigned int eType, long double dValue)
{
  if (eType < 2)
  {
    m_dValues[eType] = dValue;
  }
}

bool CCheckSumCharacAccountTrunkData::InsertCharacData(CRFWorldDatabase *pkDB)
{
  return pkDB && pkDB->Insert_NpcData(m_dwSerial, m_dwValues);
}

bool CCheckSumCharacAccountTrunkData::InsertTrunkData(CRFWorldDatabase *pkDB)
{
  if (!pkDB)
  {
    return false;
  }

  long double trunkVals[9]{};
  if (!pkDB->Select_TrunkMoney(m_dwAccountSerial, trunkVals))
  {
    return false;
  }

  CCheckSumCharacTrunkConverter converter{};
  converter.ConvertTrunk(m_dwAccountSerial, trunkVals);
  return pkDB->Insert_AnimusData(m_dwAccountSerial, trunkVals);
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

__int64 CCheckSumCharacAccountTrunkData::Load(CRFWorldDatabase *pkDB, CCheckSumCharacAccountTrunkData *kSrcValue)
{
  if (!m_dwSerial || m_dwSerial == static_cast<unsigned int>(-1) || m_dwAccountSerial == static_cast<unsigned int>(-1)
      || !pkDB)
  {
    return 0;
  }

  unsigned int resultFlag = 0;
  unsigned __int8 ret = pkDB->Select_NpcData(m_dwSerial, m_dwValues);
  if (ret == 1)
  {
    return -1;
  }
  if (ret == 2)
  {
    if (!kSrcValue->InsertCharacData(pkDB))
    {
      return -1;
    }
    std::memcpy(m_dwValues, kSrcValue->m_dwValues, sizeof(m_dwValues));
    resultFlag = 1;
  }

  ret = pkDB->Select_AnimusData(m_dwAccountSerial, m_byRace, m_dValues);
  if (ret == 1)
  {
    return 24;
  }
  if (ret == 2)
  {
    if (!kSrcValue->InsertTrunkData(pkDB))
    {
      return -1;
    }
    std::memcpy(m_dValues, kSrcValue->m_dValues, sizeof(m_dValues));
    return 1;
  }

  return resultFlag;
}

__int64 CCheckSumCharacAccountTrunkData::CheckDiff(
  CRFWorldDatabase *pkDB,
  char *wszName,
  CCheckSumCharacAccountTrunkData *kSrcValue)
{
  if (!pkDB || !wszName)
  {
    return 1;
  }

  bool changed = false;
  CCheckSumBaseConverter converter{};
  for (int j = 0; j < 6; ++j)
  {
    if (m_dwValues[j] != kSrcValue->m_dwValues[j])
    {
      changed = true;
      const unsigned int chgValue = converter.ProcCode(static_cast<unsigned __int8>(j), m_dwSerial, kSrcValue->m_dwValues[j]);
      const unsigned int orgValue = converter.ProcCode(static_cast<unsigned __int8>(j), m_dwSerial, m_dwValues[j]);
      if (!pkDB->Insert_NpcLog(m_dwSerial, wszName, j, orgValue, chgValue))
      {
        return -1;
      }
    }
  }

  for (int k = 0; k < 2; ++k)
  {
    if (m_dValues[k] != kSrcValue->m_dValues[k])
    {
      changed = true;
      const long double chgVal =
        converter.ProcCode(static_cast<unsigned __int8>(k), m_dwAccountSerial, kSrcValue->m_dValues[k]);
      const long double orgVal =
        converter.ProcCode(static_cast<unsigned __int8>(k), m_dwAccountSerial, m_dValues[k]);
      if (!pkDB->Insert_AnimusLog(m_dwAccountSerial, wszName, k, orgVal, chgVal))
      {
        return -1;
      }
    }
  }

  return changed ? 1 : 0;
}
