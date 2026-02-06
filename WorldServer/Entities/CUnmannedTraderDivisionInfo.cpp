#include "pch.h"

#include "CUnmannedTraderDivisionInfo.h"
#include "CUnmannedTraderClassInfo.h"
#include "CUnmannedTraderClassInfoFactory.h"
#include "CUnmannedTraderSortType.h"
#include "CUnmannedTraderSubClassFactory.h"
#include "tinyxml.h"

CUnmannedTraderDivisionInfo::CUnmannedTraderDivisionInfo(unsigned int dwID, char *szName)
  : m_dwID(dwID), m_szName{}, m_vecClass(), m_vecSortType()
{
  const char *source = szName ? szName : "NONE";
  strcpy_0(this->m_szName, source);
}

CUnmannedTraderDivisionInfo::~CUnmannedTraderDivisionInfo()
{
  CleanUp();
}

void CUnmannedTraderDivisionInfo::CleanUp()
{
  if (!this->m_vecClass.empty())
  {
    for (size_t i = 0; i < this->m_vecClass.size(); ++i)
    {
      CUnmannedTraderClassInfo *info = this->m_vecClass[i];
      if (info)
      {
        delete info;
      }
    }
    this->m_vecClass.clear();
  }

  if (!this->m_vecSortType.empty())
  {
    for (size_t i = 0; i < this->m_vecSortType.size(); ++i)
    {
      CUnmannedTraderSortType *sortType = this->m_vecSortType[i];
      if (sortType)
      {
        delete sortType;
      }
    }
    this->m_vecSortType.clear();
  }
}

bool CUnmannedTraderDivisionInfo::LoadXML(TiXmlElement *pkElement, CLogFile *kLogger)
{
  if (!pkElement)
  {
    return false;
  }

  unsigned int dwID = static_cast<unsigned int>(-1);
  TiXmlElement *pkElemSortType = pkElement->FirstChildElement("sorttype");
  if (!pkElemSortType)
  {
    CUnmannedTraderSortType *sortType = new CUnmannedTraderSortType(0);
    if (!sortType)
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tDivisionID(%u) %dth sorttype new CUnmannedTraderSortType( %u ) NULL!\r\n",
        this->m_dwID,
        0,
        0);
      return false;
    }
    this->m_vecSortType.push_back(sortType);
  }

  int sortIndex = 0;
  while (pkElemSortType)
  {
    dwID = static_cast<unsigned int>(-1);
    if (!pkElemSortType->Attribute("id", reinterpret_cast<int *>(&dwID)))
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tDivisionID(%u) %dth sorttype elemSortType->Attribute( id, &iID ) Invalid!\r\n",
        this->m_dwID,
        sortIndex);
      return false;
    }
    if (IsExistSortTypeID(dwID))
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tDivisionID(%u) %dth sorttype IsValidSortTypeID( iID(%u) ) Repeated ID!\r\n",
        this->m_dwID,
        sortIndex,
        dwID);
      return false;
    }

    CUnmannedTraderSortType *sortType = new CUnmannedTraderSortType(dwID);
    if (!sortType)
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tDivisionID(%u) %dth sorttype new CUnmannedTraderSortType( %u ) NULL!\r\n",
        this->m_dwID,
        sortIndex,
        dwID);
      return false;
    }
    if (!sortType->LoadXML(pkElemSortType, kLogger, this->m_dwID))
    {
      return false;
    }
    this->m_vecSortType.push_back(sortType);
    pkElemSortType = pkElemSortType->NextSiblingElement("sorttype");
    ++sortIndex;
  }

  const char *szType = nullptr;
  CUnmannedTraderClassInfo *classInfo = nullptr;
  CUnmannedTraderClassInfoFactory factory;
  TiXmlNode *elemClass = pkElement->FirstChildElement("class");
  int classIndex = 0;
  while (elemClass)
  {
    dwID = static_cast<unsigned int>(-1);
    if (!static_cast<TiXmlElement *>(elemClass)->Attribute("id", reinterpret_cast<int *>(&dwID)))
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\t0 == elemClass->Attribute( id, &iID ) Fail!\r\n");
      return false;
    }
    if (!IsValidID(dwID))
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\tDivisionID(%u) %dth Class IsValidID( iID(%u) ) Invalid!\r\n",
        this->m_dwID,
        classIndex,
        dwID);
      return false;
    }

    szType = static_cast<TiXmlElement *>(elemClass)->Attribute("type");
    if (!szType)
    {
      kLogger->Write(
        "CUnmannedTraderDivisionInfo::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger )\r\n"
        "\t\t0 == elemClass->Attribute( type ) Fail!\r\n");
      return false;
    }

    classInfo = factory.Create(szType, dwID);
    if (!classInfo->LoadXML(static_cast<TiXmlElement *>(elemClass), kLogger, this->m_dwID))
    {
      return false;
    }
    this->m_vecClass.push_back(classInfo);
    elemClass = elemClass->NextSiblingElement("class");
    ++classIndex;
  }

  return true;
}

unsigned int CUnmannedTraderDivisionInfo::GetID()
{
  return this->m_dwID;
}

unsigned long long CUnmannedTraderDivisionInfo::GetSize()
{
  return this->m_vecClass.size();
}

bool CUnmannedTraderDivisionInfo::IsExistSortTypeID(unsigned int dwID)
{
  if (dwID == static_cast<unsigned int>(-1))
  {
    return true;
  }
  if (this->m_vecSortType.empty())
  {
    return false;
  }

  for (CUnmannedTraderSortType *sortType : this->m_vecSortType)
  {
    if (sortType->GetID() == dwID)
    {
      return true;
    }
  }
  return false;
}

bool CUnmannedTraderDivisionInfo::IsValidID(unsigned int dwID)
{
  if (dwID == static_cast<unsigned int>(-1))
  {
    return false;
  }
  if (this->m_vecClass.empty())
  {
    return true;
  }

  for (CUnmannedTraderClassInfo *info : this->m_vecClass)
  {
    if (info->GetID() == dwID)
    {
      return false;
    }
  }
  return true;
}
