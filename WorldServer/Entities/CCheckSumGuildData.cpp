#include "pch.h"

#include "CCheckSumGuildData.h"

#include "CCheckSumBaseConverter.h"
#include "CCheckSumGuildConverter.h"
#include "CRFWorldDatabase.h"

CCheckSumGuildData::CCheckSumGuildData(unsigned int dwSerial)
  : m_dwGuildSerial(dwSerial)
{
}

CCheckSumGuildData::~CCheckSumGuildData()
{
  // this is not a stub
}

void CCheckSumGuildData::Encode(long double dDalant, long double dGold)
{
  CCheckSumGuildConverter converter{};
  converter.Convert(dDalant, dGold, this);
}

void CCheckSumGuildData::Decode(long double dDalant, long double dGold)
{
  CCheckSumGuildConverter converter{};
  converter.Convert(dDalant, dGold, this);
}

long double CCheckSumGuildData::GetDalant()
{
  CCheckSumBaseConverter converter{};
  const long double decoded = converter.ProcCode(6u, m_dwGuildSerial, m_dValues[0]);
  return decoded;
}

long double CCheckSumGuildData::GetGold()
{
  CCheckSumBaseConverter converter{};
  const long double decoded = converter.ProcCode(7u, m_dwGuildSerial, m_dValues[1]);
  return decoded;
}

void CCheckSumGuildData::SetValue(unsigned int eType, long double dValue)
{
  if (eType < 2)
  {
    m_dValues[eType] = dValue;
  }
}

bool CCheckSumGuildData::Insert(CRFWorldDatabase *pkDB)
{
  return pkDB && pkDB->Insert_UnitData(m_dwGuildSerial, m_dValues);
}

bool CCheckSumGuildData::Update(CRFWorldDatabase *pkDB)
{
  return pkDB && pkDB->Update_UnitData(m_dwGuildSerial, m_dValues);
}

__int64 CCheckSumGuildData::Load(CRFWorldDatabase *pkDB, CCheckSumGuildData *kSrcValue)
{
  if (!m_dwGuildSerial || m_dwGuildSerial == static_cast<unsigned int>(-1) || !pkDB)
  {
    return 0;
  }

  unsigned int resultFlag = 0;
  unsigned __int8 ret = pkDB->Select_UnitData(m_dwGuildSerial, m_dValues);
  if (ret == 1)
  {
    return -1;
  }
  if (ret == 2)
  {
    if (!kSrcValue->Insert(pkDB))
    {
      return -1;
    }
    return 1;
  }

  return resultFlag;
}

__int64 CCheckSumGuildData::CheckDiff(CRFWorldDatabase *pkDB, char *wszName, CCheckSumGuildData *kSrcValue)
{
  if (!pkDB || !wszName)
  {
    return 1;
  }

  bool changed = false;
  for (int j = 0; j < 2; ++j)
  {
    if (m_dValues[j] != kSrcValue->m_dValues[j])
    {
      changed = true;
      CCheckSumBaseConverter converter{};
      const long double chgVal =
        converter.ProcCode(static_cast<unsigned __int8>(j + 6), m_dwGuildSerial, kSrcValue->m_dValues[j]);
      const long double orgVal =
        converter.ProcCode(static_cast<unsigned __int8>(j + 6), m_dwGuildSerial, m_dValues[j]);
      if (!pkDB->Insert_UnitLog(m_dwGuildSerial, wszName, j, orgVal, chgVal))
      {
        return -1;
      }
    }
  }

  return changed ? 1 : 0;
}
