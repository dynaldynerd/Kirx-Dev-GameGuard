#include "pch.h"

#include "CUnmannedTraderSubClassInfoDefault.h"

CUnmannedTraderSubClassInfoDefault::CUnmannedTraderSubClassInfoDefault(unsigned int dwID)
  : CUnmannedTraderSubClassInfo(0u)
{
  std::strcpy(this->m_szName, "default");
}

CUnmannedTraderSubClassInfo *CUnmannedTraderSubClassInfoDefault::Create(unsigned int dwID)
{
  CUnmannedTraderSubClassInfoDefault *info = new CUnmannedTraderSubClassInfoDefault(dwID);
  if (!info)
  {
    return nullptr;
  }
  return info;
}

bool CUnmannedTraderSubClassInfoDefault::LoadXML(
  TiXmlElement *elemSubClass,
  CLogFile *kLogger,
  unsigned int dwDivisionID,
  unsigned int dwClassID)
{
  return CUnmannedTraderSubClassInfo::LoadXML(elemSubClass, kLogger, dwDivisionID, dwClassID);
}

bool CUnmannedTraderSubClassInfoDefault::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *bySubClass)
{
  return CUnmannedTraderSubClassInfo::GetGroupID(byTableCode, wItemTableIndex, bySubClass);
}
