#include "pch.h"

#include "CUnmannedTraderUserInfoTable.h"

CUnmannedTraderUserInfoTable *CUnmannedTraderUserInfoTable::ms_Instance = nullptr;

CUnmannedTraderUserInfoTable::CUnmannedTraderUserInfoTable()
  : m_pkLogger(nullptr), m_pkServiceLogger(nullptr), m_veckInfo()
{
}

CUnmannedTraderUserInfoTable *CUnmannedTraderUserInfoTable::Instance()
{
  if (!CUnmannedTraderUserInfoTable::ms_Instance)
  {
    CUnmannedTraderUserInfoTable::ms_Instance = new CUnmannedTraderUserInfoTable();
  }
  return CUnmannedTraderUserInfoTable::ms_Instance;
}

bool CUnmannedTraderUserInfoTable::Init()
{
  CUnmannedTraderUserInfo info;
  this->m_veckInfo.assign(0x9E4u, info);
  if (this->m_veckInfo.size() != 0x9E4u)
  {
    return false;
  }

  for (unsigned short index = 0; index < 0x9E4u; ++index)
  {
    CUnmannedTraderUserInfo &entry = this->m_veckInfo[index];
    if (!entry.Init(index))
    {
      return false;
    }
  }
  return true;
}

void CUnmannedTraderUserInfoTable::SetLogger(CLogFile *pkLogger, CLogFile *pkServiceLogger)
{
  this->m_pkLogger = pkLogger;
  this->m_pkServiceLogger = pkServiceLogger;
}
