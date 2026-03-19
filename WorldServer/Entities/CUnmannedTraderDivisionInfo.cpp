#include "pch.h"

#include "CUnmannedTraderDivisionInfo.h"
#include "CUnmannedTraderClassInfo.h"
#include "CUnmannedTraderClassInfoFactory.h"
#include "CUnmannedTraderSortType.h"
#include "CUnmannedTraderSubClassFactory.h"
#include "CLogFile.h"
#include "tinyxml.h"

CUnmannedTraderDivisionInfo::CUnmannedTraderDivisionInfo(unsigned int dwID, char *szName)
  : m_dwID(dwID), m_szName{}, m_vecClass(), m_vecSortType()
{
  const char *source = szName ? szName : "NONE";
  std::strcpy(this->m_szName, source);
}

CUnmannedTraderDivisionInfo::~CUnmannedTraderDivisionInfo()
{
  CleanUp();
}

const CUnmannedTraderDivisionInfo *CUnmannedTraderDivisionInfo::Copy(
  const CUnmannedTraderDivisionInfo *lhs)
{
  m_dwID = lhs->m_dwID;
  std::strcpy(m_szName, lhs->m_szName);
  m_vecClass.clear();
  if (!lhs->m_vecClass.empty())
  {
    m_vecClass.insert(m_vecClass.begin(), lhs->m_vecClass.begin(), lhs->m_vecClass.end());
  }
  return this;
}

CUnmannedTraderSortType *CUnmannedTraderDivisionInfo::FindSortType(unsigned int dwID)
{
  if (m_vecSortType.empty())
  {
    return nullptr;
  }

  for (CUnmannedTraderSortType *sortType : m_vecSortType)
  {
    if (sortType && dwID == sortType->GetID())
    {
      return sortType;
    }
  }

  return nullptr;
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

unsigned int CUnmannedTraderDivisionInfo::GetSize()
{
  return static_cast<unsigned int>(this->m_vecClass.size());
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

unsigned int CUnmannedTraderDivisionInfo::GetMaxClassCnt()
{
  // narrowing cast for thunk return parity
  return static_cast<unsigned int>(this->m_vecClass.size());
}

bool CUnmannedTraderDivisionInfo::GetGroupID(
  unsigned __int8 byTableCode,
  unsigned __int16 wItemTableIndex,
  unsigned __int8 *byDivision,
  unsigned __int8 *byClass)
{
  if (this->m_vecClass.empty())
  {
    return false;
  }

  for (CUnmannedTraderClassInfo *info : this->m_vecClass)
  {
    if (info && info->GetGroupID(byTableCode, wItemTableIndex, byClass))
    {
      *byDivision = static_cast<unsigned __int8>(this->m_dwID);
      return true;
    }
  }

  return false;
}

bool CUnmannedTraderDivisionInfo::IsExistGroupID(
  unsigned __int8 byDivision,
  unsigned __int8 byClass,
  unsigned __int8 bySubClass,
  unsigned __int8 bySortType,
  unsigned int *dwListIndex)
{
  *dwListIndex = 0;
  if (this->m_dwID != byDivision)
  {
    return false;
  }
  if (bySortType == 255)
  {
    return (this->m_vecSortType.empty() ? 1 : 0) != 0;
  }
  if (!IsExistSortTypeID(bySortType))
  {
    return false;
  }

  unsigned int classIndex = 0;
  for (CUnmannedTraderClassInfo *info : this->m_vecClass)
  {
    if (info && info->IsExistGroupID(byClass, bySubClass))
    {
      *dwListIndex = classIndex;
      return true;
    }
    ++classIndex;
  }

  return false;
}

CUnmannedTraderSortType *CUnmannedTraderDivisionInfo::GetSortType(unsigned __int8 bySortType)
{
  if (bySortType == 0xFFu || this->m_vecSortType.empty())
  {
    return nullptr;
  }

  for (CUnmannedTraderSortType *sortType : this->m_vecSortType)
  {
    if (sortType->GetID() == bySortType)
    {
      return sortType;
    }
  }

  return nullptr;
}

bool CUnmannedTraderDivisionInfo::GetGroupID(
  unsigned __int8 byTableCode,
  unsigned __int16 wItemTableIndex,
  unsigned __int8 *byDivision,
  unsigned __int8 *byClass,
  unsigned __int8 *bySubClass,
  unsigned int *dwListIndex)
{
  if (this->m_vecClass.empty())
  {
    *dwListIndex = 0;
    return false;
  }

  unsigned int classIndex = 0;
  for (CUnmannedTraderClassInfo *info : this->m_vecClass)
  {
    if (info && info->GetGroupID(byTableCode, wItemTableIndex, byClass, bySubClass))
    {
      *byDivision = static_cast<unsigned __int8>(this->m_dwID);
      *dwListIndex = classIndex;
      return true;
    }
    ++classIndex;
  }

  return false;
}
