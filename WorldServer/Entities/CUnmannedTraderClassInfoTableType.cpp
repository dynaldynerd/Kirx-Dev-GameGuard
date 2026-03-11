#include "pch.h"

#include "CUnmannedTraderClassInfoTableType.h"
#include "CUnmannedTraderSubClassFactory.h"
#include "CUnmannedTraderSubClassInfo.h"
#include "CLogFile.h"
#include "tinyxml.h"

CUnmannedTraderClassInfoTableType::CUnmannedTraderClassInfoTableType(unsigned int dwID)
  : CUnmannedTraderClassInfo(dwID), m_byTableCode(static_cast<unsigned char>(-1)), m_vecSubClass()
{
  std::strcpy(this->m_szTypeName, "table");
}

CUnmannedTraderClassInfoTableType::~CUnmannedTraderClassInfoTableType()
{
  CleanUp();
}

void CUnmannedTraderClassInfoTableType::CleanUp()
{
  if (this->m_vecSubClass.empty())
  {
    return;
  }

  for (size_t i = 0; i < this->m_vecSubClass.size(); ++i)
  {
    CUnmannedTraderSubClassInfo *subClass = this->m_vecSubClass[i];
    if (subClass)
    {
      delete subClass;
    }
  }
  this->m_vecSubClass.clear();
}

bool CUnmannedTraderClassInfoTableType::LoadXML(
  TiXmlElement *elemClass,
  CLogFile *kLogger,
  unsigned int dwDivisionID)
{
  if (!elemClass)
  {
    return false;
  }

  int tableIndex = -1;
  if (!elemClass->Attribute("tableIndex", &tableIndex))
  {
    kLogger->Write(
      "CUnmannedTraderClassInfo::LoadXML( TiXmlElement * pkElement, CLogFile & pkLogger )\r\n"
      "\t\tDivisionID(%u), ClassID(%u) pkElement->Attribute( tableIndex ) NULL!\r\n",
      dwDivisionID,
      this->m_dwID);
    return false;
  }

  const char *className = elemClass->Attribute("name");
  if (!className)
  {
    kLogger->Write(
      "CUnmannedTraderClassInfo::LoadXML( TiXmlElement * pkElement, CLogFile & pkLogger )\r\n"
      "\t\tDivisionID(%u), ClassID(%u) pkElement->Attribute( name ) NULL!\r\n",
      dwDivisionID,
      this->m_dwID);
    return false;
  }

  CUnmannedTraderSubClassFactory factory;
  CUnmannedTraderSubClassInfo *subClassInfo = nullptr;
  unsigned int subClassId = static_cast<unsigned int>(-1);
  TiXmlNode *child = elemClass->FirstChildElement("subclass");
  if (!child)
  {
    subClassInfo = factory.Create("default", 0);
    if (!subClassInfo)
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tkFactoy.Create( default, 0 ) NULL!\r\n");
      return false;
    }
    this->m_vecSubClass.push_back(subClassInfo);
  }

  int subIndex = 0;
  while (child)
  {
    const char *subType = static_cast<TiXmlElement *>(child)->Attribute("type");
    if (!subType)
    {
      kLogger->Write(
        "CUnmannedTraderClassInfo::LoadXML( TiXmlElement * pkElement, CLogFile & pkLogger, DWORD dwDivisionID(%u) )\r\n"
        "\t\tDivisionID(%u), ClassID(%u) pkElement->Attribute( code ) NULL!\r\n",
        dwDivisionID,
        dwDivisionID,
        this->m_dwID);
      return false;
    }

    subClassId = static_cast<unsigned int>(-1);
    if (!static_cast<TiXmlElement *>(child)->Attribute("id", reinterpret_cast<int *>(&subClassId)))
    {
      kLogger->Write(
        "CUnmannedTraderClassInfo::LoadXML( TiXmlElement * pkElement, CLogFile & pkLogger, DWORD dwDivisionID(%u) )\r\n"
        "\t\tDivisionID(%u), ClassID(%u) 0 == elemSubClass->Attribute( id, &iID ) Fail!\r\n",
        dwDivisionID,
        dwDivisionID,
        this->m_dwID);
      return false;
    }

    if (!IsValidID(subClassId))
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tDivisionID(%u), ClassID(%u) %dth SubClass IsValidID( iID(%u) ) Invalid!\r\n",
        dwDivisionID,
        this->m_dwID,
        subIndex,
        subClassId);
      return false;
    }

    subClassInfo = factory.Create(subType, subClassId);
    if (!subClassInfo)
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tkFactoy.Create( szSubClassType(%s), iID(%u) ) NULL!\r\n",
        subType,
        subClassId);
      return false;
    }

    if (!subClassInfo->LoadXML(static_cast<TiXmlElement *>(child), kLogger, dwDivisionID, this->m_dwID))
    {
      return false;
    }

    this->m_vecSubClass.push_back(subClassInfo);
    child = child->NextSiblingElement("subclass");
    ++subIndex;
  }

  this->m_byTableCode = static_cast<unsigned char>(tableIndex);
  std::strcpy(this->m_szClassName, className);
  return true;
}

bool CUnmannedTraderClassInfoTableType::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *byClass,
  unsigned char *bySubClass)
{
  if (this->m_byTableCode != byTableCode || this->m_vecSubClass.empty())
  {
    return false;
  }

  for (CUnmannedTraderSubClassInfo *subClass : this->m_vecSubClass)
  {
    if (subClass->GetGroupID(byTableCode, wItemTableIndex, bySubClass))
    {
      *byClass = static_cast<unsigned char>(this->m_dwID);
      return true;
    }
  }
  return false;
}

bool CUnmannedTraderClassInfoTableType::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *byClass)
{
  if (this->m_byTableCode != byTableCode)
  {
    return false;
  }
  *byClass = static_cast<unsigned char>(this->m_dwID);
  return true;
}

CUnmannedTraderClassInfo *CUnmannedTraderClassInfoTableType::Create(unsigned int dwID)
{
  CUnmannedTraderClassInfoTableType *newType = new CUnmannedTraderClassInfoTableType(dwID);
  if (!newType)
  {
    return nullptr;
  }
  return newType;
}

bool CUnmannedTraderClassInfoTableType::IsExistGroupID(unsigned char byClass, unsigned char bySubClass)
{
  if (byClass != this->m_dwID)
  {
    return false;
  }

  if (bySubClass == 255)
  {
    return this->m_vecSubClass.empty();
  }

  for (CUnmannedTraderSubClassInfo *subClass : this->m_vecSubClass)
  {
    if (static_cast<int>(bySubClass) == subClass->GetID())
    {
      return true;
    }
  }

  return false;
}

bool CUnmannedTraderClassInfoTableType::IsValidID(unsigned int dwID)
{
  if (dwID == static_cast<unsigned int>(-1))
  {
    return false;
  }

  if (this->m_vecSubClass.empty())
  {
    return true;
  }

  for (CUnmannedTraderSubClassInfo *subClass : this->m_vecSubClass)
  {
    if (subClass->GetID() == dwID)
    {
      return false;
    }
  }
  return true;
}
