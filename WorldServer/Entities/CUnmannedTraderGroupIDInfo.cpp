#include "pch.h"

#include "CUnmannedTraderGroupIDInfo.h"
#include "CUnmannedTraderDivisionInfo.h"
#include "tinyxml.h"
#include <cstdarg>
#include <utility>

CUnmannedTraderGroupIDInfo::CUnmannedTraderGroupIDInfo()
  : m_pkLogger(nullptr), m_vecDivisionInfo()
{
}

CUnmannedTraderGroupIDInfo::~CUnmannedTraderGroupIDInfo()
{
  CleanUp();
}

void CUnmannedTraderGroupIDInfo::CleanUp()
{
  if (this->m_vecDivisionInfo.empty())
  {
    return;
  }

  for (size_t i = 0; i < this->m_vecDivisionInfo.size(); ++i)
  {
    CUnmannedTraderDivisionInfo *division = this->m_vecDivisionInfo[i];
    if (division)
    {
      delete division;
    }
  }
  this->m_vecDivisionInfo.clear();
}

void CUnmannedTraderGroupIDInfo::SetLogger(CLogFile *pkLogger)
{
  this->m_pkLogger = pkLogger;
}

void CUnmannedTraderGroupIDInfo::Log(const char *fmt, ...)
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

bool CUnmannedTraderGroupIDInfo::IsExistID(unsigned int dwID)
{
  if (dwID == static_cast<unsigned int>(-1))
  {
    return false;
  }
  if (this->m_vecDivisionInfo.empty())
  {
    return false;
  }

  for (CUnmannedTraderDivisionInfo *division : this->m_vecDivisionInfo)
  {
    if (division->GetID() == dwID)
    {
      return true;
    }
  }
  return false;
}

bool CUnmannedTraderGroupIDInfo::GetIDInfo(std::vector<std::pair<unsigned long, unsigned long>> *vecInfo)
{
  vecInfo->clear();
  if (this->m_vecDivisionInfo.empty())
  {
    Log(
      "CUnmannedTraderGroupIDInfo::GetIDAndMaxCntInfo( std::vector< std::pair<DWORD,DWORD> > & vecInfo )\r\n"
      "\t\tm_vecClassInfo.empty()!\r\n");
    return false;
  }

  vecInfo->reserve(this->m_vecDivisionInfo.size());
  for (CUnmannedTraderDivisionInfo *division : this->m_vecDivisionInfo)
  {
    const unsigned long id = static_cast<unsigned long>(division->GetID());
    const unsigned long size = static_cast<unsigned long>(division->GetSize());
    vecInfo->push_back(std::make_pair(id, size));
  }

  return this->m_vecDivisionInfo.size() == vecInfo->size();
}

bool CUnmannedTraderGroupIDInfo::LoadXML(const char *szFileName)
{
  if (!szFileName)
  {
    return false;
  }

  TiXmlDocument doc(szFileName);
  if (!doc.LoadFile(TIXML_ENCODING_UNKNOWN))
  {
    Log(
      "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n\t\tTiXmlDocument::LoadFile() Fail!\r\n",
      szFileName);
    return false;
  }

  TiXmlNode *root = doc.FirstChild("UnmannedTrader");
  if (!root)
  {
    Log(
      "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n"
      "\t\tTiXmlDocument::FirstChild(UnmannedTrader) NULL!\r\n",
      szFileName);
    return false;
  }

  TiXmlElement *elemDivision = root->FirstChildElement("division");
  if (!elemDivision)
  {
    Log(
      "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n"
      "\t\tnode->FirstChildElement( division ) Fail!\r\n",
      szFileName);
    return false;
  }

  int index = 0;
  while (elemDivision)
  {
    unsigned int dwID = static_cast<unsigned int>(-1);
    if (!elemDivision->Attribute("id", reinterpret_cast<int *>(&dwID)))
    {
      Log(
        "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n"
        "\t\t0 == pkElement->Attribute( id, &iID ) Fail!\r\n",
        szFileName);
      return false;
    }

    if (IsExistID(dwID))
    {
      Log(
        "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n"
        "\t\t%dth Division IsExistID( iID )( iID(%u) ) Exist!\r\n",
        szFileName,
        index,
        dwID);
      return false;
    }

    const char *szName = elemDivision->Attribute("name");
    if (!szName)
    {
      Log(
        "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n"
        "\t\t0 == pkElement->Attribute( name ) Fail!\r\n",
        szFileName);
      return false;
    }

    CUnmannedTraderDivisionInfo *division = new CUnmannedTraderDivisionInfo(dwID, const_cast<char *>(szName));
    if (!division)
    {
      Log(
        "CUnmannedTraderGroupItemInfoTable::LoadXML( szFileName(%s) )\r\n"
        "\t\t%dth Division new CUnmannedTraderDivisionInfo(iID(%d) NULL!\r\n",
        szFileName,
        index,
        dwID);
      return false;
    }

    if (!division->LoadXML(elemDivision, this->m_pkLogger))
    {
      return false;
    }

    this->m_vecDivisionInfo.push_back(division);
    elemDivision = elemDivision->NextSiblingElement("division");
    ++index;
  }

  return true;
}
