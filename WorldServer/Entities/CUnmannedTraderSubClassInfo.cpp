#include "pch.h"

#include "CUnmannedTraderSubClassInfo.h"

CUnmannedTraderSubClassInfo::CUnmannedTraderSubClassInfo(unsigned int dwID)
  : m_szName{}, m_dwID(dwID)
{
  std::strcpy(this->m_szName, "NONE");
}

CUnmannedTraderSubClassInfo::CUnmannedTraderSubClassInfo(const CUnmannedTraderSubClassInfo *lhs)
  : m_szName{}, m_dwID(0)
{
  std::strcpy(this->m_szName, "NONE");
  Copy(lhs);
}

CUnmannedTraderSubClassInfo::~CUnmannedTraderSubClassInfo()
{
}

const CUnmannedTraderSubClassInfo *CUnmannedTraderSubClassInfo::Copy(const CUnmannedTraderSubClassInfo *lhs)
{
  this->m_dwID = lhs->m_dwID;
  std::strcpy(this->m_szName, lhs->m_szName);
  return this;
}

bool CUnmannedTraderSubClassInfo::LoadXML(
  TiXmlElement *pkElement,
  CLogFile *kLogger,
  unsigned int dwDivisionID,
  unsigned int dwClassID)
{
  return true;
}

bool CUnmannedTraderSubClassInfo::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *bySubClass)
{
  *bySubClass = static_cast<unsigned char>(this->m_dwID);
  return true;
}

unsigned int CUnmannedTraderSubClassInfo::GetID()
{
  return this->m_dwID;
}

char *CUnmannedTraderSubClassInfo::GetTypeName()
{
  return this->m_szName;
}
