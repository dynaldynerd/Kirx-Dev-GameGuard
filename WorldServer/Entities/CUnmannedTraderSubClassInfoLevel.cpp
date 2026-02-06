#include "pch.h"

#include "CUnmannedTraderSubClassInfoLevel.h"
#include "CLogFile.h"
#include "WorldServerUtil.h"
#include "tinyxml.h"

CUnmannedTraderSubClassInfoLevel::CUnmannedTraderSubClassInfoLevel(unsigned int dwID)
  : CUnmannedTraderSubClassInfo(dwID), m_byMin(0), m_byMax(0)
{
  strcpy_0(this->m_szName, "level");
}

CUnmannedTraderSubClassInfoLevel::~CUnmannedTraderSubClassInfoLevel()
{
}

CUnmannedTraderSubClassInfo *CUnmannedTraderSubClassInfoLevel::Create(unsigned int dwID)
{
  CUnmannedTraderSubClassInfoLevel *info = new CUnmannedTraderSubClassInfoLevel(dwID);
  if (!info)
  {
    return nullptr;
  }
  return info;
}

bool CUnmannedTraderSubClassInfoLevel::LoadXML(
  TiXmlElement *elemSubClass,
  CLogFile *kLogger,
  unsigned int dwDivisionID,
  unsigned int dwClassID)
{
  int minLevel = 0;
  int maxLevel = 0;
  if (elemSubClass->Attribute("min", &minLevel) && minLevel < 0x100u)
  {
    if (elemSubClass->Attribute("max", &maxLevel) && maxLevel < 0x100u)
    {
      if (maxLevel >= minLevel)
      {
        this->m_byMin = static_cast<unsigned char>(minLevel);
        this->m_byMax = static_cast<unsigned char>(maxLevel);
        return true;
      }

      kLogger->Write(
        "CUnmannedTraderSubClassInfoCode::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger, DWORD dwDivisionID, DWORD dwClassID )\r\n"
        "\t\tDivisionID(%u), ClassID(%u) iMax(%d) < iMin(%d)!\r\n",
        dwDivisionID,
        dwClassID,
        this->m_dwID,
        maxLevel);
      return false;
    }

    kLogger->Write(
      "CUnmannedTraderSubClassInfoCode::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger, DWORD dwDivisionID, DWORD dwClassID )\r\n"
      "\t\tDivisionID(%u), ClassID(%u) 0 == elemSubClass->Attribute( max, &iMax ) || 0 > iMax(%d) || 255 < iMax(%d)!\r\n",
      dwDivisionID,
      dwClassID,
      this->m_dwID,
      maxLevel);
    return false;
  }

  kLogger->Write(
    "CUnmannedTraderSubClassInfoCode::LoadXML( TiXmlElement * pkElement, CLogFile & kLogger, DWORD dwDivisionID, DWORD dwClassID )\r\n"
    "\t\tDivisionID(%u), ClassID(%u) 0 == elemSubClass->Attribute( min, &iMin ) || 0 > iMin(%d) || 255 < iMin(%d)!\r\n",
    dwDivisionID,
    dwClassID,
    this->m_dwID,
    minLevel);
  return false;
}

bool CUnmannedTraderSubClassInfoLevel::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *bySubClass)
{
  const int itemLevel = GetItemEquipLevel(byTableCode, wItemTableIndex);
  if (this->m_byMin > itemLevel || itemLevel > this->m_byMax)
  {
    return false;
  }
  *bySubClass = static_cast<unsigned char>(this->m_dwID);
  return true;
}
