#include "pch.h"

#include "CUnmannedTraderClassInfoTableCodeType.h"
#include "CUnmannedTraderItemCodeInfo.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "tinyxml.h"

CUnmannedTraderClassInfoTableCodeType::CUnmannedTraderClassInfoTableCodeType(unsigned int dwID)
  : CUnmannedTraderClassInfoTableType(dwID), m_vecCodeList()
{
  strcpy_0(this->m_szTypeName, "table and code");
}

CUnmannedTraderClassInfoTableCodeType::~CUnmannedTraderClassInfoTableCodeType()
{
  this->m_vecCodeList.clear();
}

bool CUnmannedTraderClassInfoTableCodeType::LoadXML(
  TiXmlElement *elemClass,
  CLogFile *kLogger,
  unsigned int dwDivisionID)
{
  if (!CUnmannedTraderClassInfoTableType::LoadXML(elemClass, kLogger, dwDivisionID))
  {
    return false;
  }

  unsigned int dwStartIndex = static_cast<unsigned int>(-1);
  unsigned int dwEndIndex = static_cast<unsigned int>(-1);
  TiXmlNode *child = elemClass->FirstChildElement("code");
  while (child)
  {
    const char *szCode = static_cast<TiXmlElement *>(child)->Attribute("code");
    if (!szCode)
    {
      kLogger->Write(
        "CUnmannedTraderClassInfoTableCodeType::LoadXML( TiXmlElement * elemClass, CLogFile & kLogger, DWORD dwDivisionID )\r\n"
        "\t\tDivisionID(%u), ClassID(%u) pkElement->Attribute( code ) NULL!\r\n",
        dwDivisionID,
        this->m_dwID);
      return false;
    }

    static_cast<TiXmlElement *>(child)->Attribute("startindex", reinterpret_cast<int *>(&dwStartIndex));
    static_cast<TiXmlElement *>(child)->Attribute("endindex", reinterpret_cast<int *>(&dwEndIndex));

    CUnmannedTraderItemCodeInfo info(szCode, dwStartIndex, dwEndIndex);
    this->m_vecCodeList.push_back(info);

    child = child->NextSiblingElement("code");
  }

  return true;
}

bool CUnmannedTraderClassInfoTableCodeType::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *byClass)
{
  if (this->m_byTableCode != byTableCode)
  {
    return false;
  }

  _base_fld *record = g_Main.m_tblItemData[byTableCode].GetRecord(wItemTableIndex);
  if (!record)
  {
    return false;
  }

  for (CUnmannedTraderItemCodeInfo &codeInfo : this->m_vecCodeList)
  {
    if (codeInfo == record->m_strCode)
    {
      *byClass = static_cast<unsigned char>(this->m_dwID);
      return true;
    }
  }

  return false;
}

bool CUnmannedTraderClassInfoTableCodeType::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *byClass,
  unsigned char *bySubClass)
{
  *bySubClass = 0;
  if (!CUnmannedTraderClassInfoTableCodeType::GetGroupID(byTableCode, wItemTableIndex, byClass))
  {
    return false;
  }
  if (this->m_vecSubClass.empty())
  {
    return true;
  }

  return CUnmannedTraderClassInfoTableType::GetGroupID(byTableCode, wItemTableIndex, byClass, bySubClass);
}

CUnmannedTraderClassInfo *CUnmannedTraderClassInfoTableCodeType::Create(unsigned int dwID)
{
  CUnmannedTraderClassInfoTableCodeType *newType = new CUnmannedTraderClassInfoTableCodeType(dwID);
  if (!newType)
  {
    return nullptr;
  }
  return newType;
}
