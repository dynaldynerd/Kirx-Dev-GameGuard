#include "pch.h"

#include "CUnmannedTraderSubClassInfoCode.h"
#include "CLogFile.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "tinyxml.h"

CUnmannedTraderSubClassInfoCode::CUnmannedTraderSubClassInfoCode(unsigned int dwID)
  : CUnmannedTraderSubClassInfo(dwID), m_vecCodeList()
{
  std::strcpy(this->m_szName, "code");
}

CUnmannedTraderSubClassInfoCode::~CUnmannedTraderSubClassInfoCode()
{
  this->m_vecCodeList.clear();
}

CUnmannedTraderSubClassInfo *CUnmannedTraderSubClassInfoCode::Create(unsigned int dwID)
{
  CUnmannedTraderSubClassInfoCode *info = new CUnmannedTraderSubClassInfoCode(dwID);
  if (!info)
  {
    return nullptr;
  }
  return info;
}

bool CUnmannedTraderSubClassInfoCode::LoadXML(
  TiXmlElement *elemSubClass,
  CLogFile *kLogger,
  unsigned int dwDivisionID,
  unsigned int dwClassID)
{
  const char *szCode = elemSubClass->Attribute("code");
  unsigned int dwStartIndex = 0;
  unsigned int dwEndIndex = 0;
  if (szCode)
  {
    elemSubClass->Attribute("startindex", reinterpret_cast<int *>(&dwStartIndex));
    elemSubClass->Attribute("endindex", reinterpret_cast<int *>(&dwEndIndex));

    CUnmannedTraderItemCodeInfo info(szCode, dwStartIndex, dwEndIndex);
    this->m_vecCodeList.push_back(info);
    return true;
  }

  TiXmlNode *child = elemSubClass->FirstChildElement("code");
  while (child)
  {
    szCode = static_cast<TiXmlElement *>(child)->Attribute("code");
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

  return !this->m_vecCodeList.empty();
}

bool CUnmannedTraderSubClassInfoCode::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *bySubClass)
{
  _base_fld *record = g_Main.m_tblItemData[byTableCode].GetRecord(wItemTableIndex);
  if (!record)
  {
    return false;
  }

  for (CUnmannedTraderItemCodeInfo &codeInfo : this->m_vecCodeList)
  {
    if (codeInfo == record->m_strCode)
    {
      *bySubClass = static_cast<unsigned char>(this->m_dwID);
      return true;
    }
  }

  return false;
}
