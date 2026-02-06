#include "pch.h"

#include "CUnmannedTraderSubClassInfoForceLiverGrade.h"
#include "CLogFile.h"
#include "CPlayer.h"
#include "CRecordData.h"
#include "GlobalObjects.h"
#include "tinyxml.h"

CUnmannedTraderSubClassInfoForceLiverGrade::CUnmannedTraderSubClassInfoForceLiverGrade(unsigned int dwID)
  : CUnmannedTraderSubClassInfo(dwID), m_byGrade(static_cast<unsigned char>(-1))
{
  strcpy_0(this->m_szName, "forcelivergrade");
}

CUnmannedTraderSubClassInfoForceLiverGrade::~CUnmannedTraderSubClassInfoForceLiverGrade()
{
}

CUnmannedTraderSubClassInfo *CUnmannedTraderSubClassInfoForceLiverGrade::Create(unsigned int dwID)
{
  CUnmannedTraderSubClassInfoForceLiverGrade *info = new CUnmannedTraderSubClassInfoForceLiverGrade(dwID);
  if (!info)
  {
    return nullptr;
  }
  return info;
}

bool CUnmannedTraderSubClassInfoForceLiverGrade::LoadXML(
  TiXmlElement *elemSubClass,
  CLogFile *kLogger,
  unsigned int dwDivisionID,
  unsigned int dwClassID)
{
  int grade = -1;
  if (elemSubClass->Attribute("grade", &grade) && grade < 0x100u)
  {
    this->m_byGrade = static_cast<unsigned char>(grade);
    return true;
  }

  kLogger->Write(
    "CUnmannedTraderSubClassInfoCode::LoadXML( TiXmlElement * elemSubClass, CLogFile & kLogger, DWORD dwDivisionID, DWORD dwClassID )\r\n"
    "\t\tDivisionID(%u), ClassID(%u) 0 == elemSubClass->Attribute( grade, &iGrade ) || 0 > iGrade || 255 < iGrade(%d) )!\r\n",
    dwDivisionID,
    dwClassID,
    this->m_dwID);
  return false;
}

bool CUnmannedTraderSubClassInfoForceLiverGrade::GetGroupID(
  unsigned char byTableCode,
  unsigned short wItemTableIndex,
  unsigned char *bySubClass)
{
  if (byTableCode != 15)
  {
    return false;
  }

  _base_fld *record =
    g_Main.m_tblEffectData[1].GetRecord(*(CPlayer::s_pnLinkForceItemToEffect + wItemTableIndex));
  if (!record)
  {
    return false;
  }

  if (this->m_byGrade != record[4].m_strCode[60])
  {
    return false;
  }

  *bySubClass = static_cast<unsigned char>(this->m_dwID);
  return true;
}
