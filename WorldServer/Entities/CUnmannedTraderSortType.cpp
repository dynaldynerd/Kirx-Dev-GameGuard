#include "pch.h"

#include "CUnmannedTraderSortType.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CLogFile.h"
#include "tinyxml.h"

CUnmannedTraderSortType::CUnmannedTraderSortType(unsigned int dwID)
  : m_dwID(dwID), m_szTypeName{}, m_szQuery{}
{
  strcpy_0(this->m_szTypeName, CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Sort_Type_Name);
  strcpy_0(this->m_szQuery, CUnmannedTraderEnvironmentValue::Unmanned_Trader_Default_Sort_Query);
}

bool CUnmannedTraderSortType::LoadXML(TiXmlElement *pkElemSortType, CLogFile *kLogger, unsigned int dwDivisionID)
{
  const char *typeName = pkElemSortType->Attribute("type");
  if (!typeName)
  {
    kLogger->Write(
      "CUnmannedTraderSortType::LoadXML( TiXmlElement * pkElement, CLogFile & pkLogger )\r\n"
      "\t\tDivisionID(%u), ClassID(%u) pkElemSortType->Attribute( type ) NULL!\r\n",
      dwDivisionID,
      this->m_dwID);
    return false;
  }

  const char *query = pkElemSortType->Attribute("query");
  if (!query)
  {
    kLogger->Write(
      "CUnmannedTraderSortType::LoadXML( TiXmlElement * pkElement, CLogFile & pkLogger )\r\n"
      "\t\tDivisionID(%u), ClassID(%u) pkElemSortType->Attribute( query ) NULL!\r\n",
      dwDivisionID,
      this->m_dwID);
    return false;
  }

  strcpy_0(this->m_szTypeName, typeName);
  strcpy_0(this->m_szQuery, query);
  return true;
}

unsigned int CUnmannedTraderSortType::GetID()
{
  return this->m_dwID;
}

char *CUnmannedTraderSortType::GetQuery()
{
  if (this->m_szQuery[0])
  {
    return this->m_szQuery;
  }
  return nullptr;
}

const CUnmannedTraderSortType *CUnmannedTraderSortType::operator=(const CUnmannedTraderSortType *lhs)
{
  this->m_dwID = lhs->m_dwID;
  strcpy_0(this->m_szTypeName, lhs->m_szTypeName);
  strcpy_0(this->m_szQuery, lhs->m_szQuery);
  return this;
}
