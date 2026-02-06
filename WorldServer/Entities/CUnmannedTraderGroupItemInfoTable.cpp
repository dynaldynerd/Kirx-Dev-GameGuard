#include "pch.h"

#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include <cstdarg>

CUnmannedTraderGroupItemInfoTable *CUnmannedTraderGroupItemInfoTable::ms_Instance = nullptr;

CUnmannedTraderGroupItemInfoTable::CUnmannedTraderGroupItemInfoTable()
  : m_pkLogger(nullptr), m_kVerInfo(), m_kGroupIDInfo()
{
}

CUnmannedTraderGroupItemInfoTable *CUnmannedTraderGroupItemInfoTable::Instance()
{
  if (!CUnmannedTraderGroupItemInfoTable::ms_Instance)
  {
    CUnmannedTraderGroupItemInfoTable::ms_Instance = new CUnmannedTraderGroupItemInfoTable();
  }
  return CUnmannedTraderGroupItemInfoTable::ms_Instance;
}

bool CUnmannedTraderGroupItemInfoTable::Init()
{
  if (!this->m_kGroupIDInfo.LoadXML(
        CUnmannedTraderEnvironmentValue::Unmanned_Trader_Setting_Xml_File))
  {
    return false;
  }

  std::vector<std::pair<unsigned long, unsigned long>> vecInfo;
  if (this->m_kGroupIDInfo.GetIDInfo(&vecInfo))
  {
    if (this->m_kVerInfo.Init(&vecInfo))
    {
      return true;
    }

    this->Log("CUnmannedTraderGroupItemInfoTable::Init() : m_vecVerInfo.Init( vecInfo ) Fail!\r\n");
    return false;
  }

  return false;
}

void CUnmannedTraderGroupItemInfoTable::SetLogger(CLogFile *pkLogger)
{
  this->m_pkLogger = pkLogger;
  this->m_kGroupIDInfo.SetLogger(pkLogger);
}

void CUnmannedTraderGroupItemInfoTable::Log(const char *fmt, ...)
{
  if (!this->m_pkLogger)
  {
    return;
  }

  va_list args;
  va_start(args, fmt);
  this->m_pkLogger->WriteFromArg(fmt, args);
  va_end(args);
}
