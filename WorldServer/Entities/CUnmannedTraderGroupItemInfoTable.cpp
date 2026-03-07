#include "pch.h"

#include "base_fld.h"

#include "GlobalObjects.h"

#include "CUnmannedTraderGroupItemInfoTable.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CRecordData.h"
#include <cstdarg>

CUnmannedTraderGroupItemInfoTable *CUnmannedTraderGroupItemInfoTable::ms_Instance;

CUnmannedTraderGroupItemInfoTable::CUnmannedTraderGroupItemInfoTable()
  : m_pkLogger(nullptr), m_kVerInfo(), m_kGroupIDInfo()
{
}

CUnmannedTraderGroupItemInfoTable::~CUnmannedTraderGroupItemInfoTable() = default;

CUnmannedTraderGroupItemInfoTable *CUnmannedTraderGroupItemInfoTable::Instance()
{
  if (!CUnmannedTraderGroupItemInfoTable::ms_Instance)
  {
    CUnmannedTraderGroupItemInfoTable::ms_Instance = new CUnmannedTraderGroupItemInfoTable();
  }
  return CUnmannedTraderGroupItemInfoTable::ms_Instance;
}

void CUnmannedTraderGroupItemInfoTable::Destroy()
{
  if (CUnmannedTraderGroupItemInfoTable::ms_Instance)
  {
    delete CUnmannedTraderGroupItemInfoTable::ms_Instance;
    CUnmannedTraderGroupItemInfoTable::ms_Instance = nullptr;
  }
}

bool CUnmannedTraderGroupItemInfoTable::Init()
{
  if (!this->m_kGroupIDInfo.LoadXML(
        CUnmannedTraderEnvironmentValue::Unmanned_Trader_Setting_Xml_File))
  {
    return false;
  }

  std::vector<std::pair<unsigned long, unsigned long>> vecInfo;
  if (this->m_kGroupIDInfo.GetIDInfo(&vecInfo))
  {
    if (this->m_kVerInfo.Init(&vecInfo))
    {
      return true;
    }

    this->Log("CUnmannedTraderGroupItemInfoTable::Init() : m_vecVerInfo.Init( vecInfo ) Fail!\r\n");
    return false;
  }

  return false;
}

void CUnmannedTraderGroupItemInfoTable::SetLogger(CLogFile *pkLogger)
{
  this->m_pkLogger = pkLogger;
  this->m_kGroupIDInfo.SetLogger(pkLogger);
}

void CUnmannedTraderGroupItemInfoTable::Log(const char *fmt, ...)
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

bool CUnmannedTraderGroupItemInfoTable::GetGroupID(
  unsigned __int8 byTableCode,
  unsigned __int16 wItemTableIndex,
  unsigned __int8 *byDivision,
  unsigned __int8 *byClass,
  unsigned __int8 *bySubClass,
  unsigned int *dwListIndex)
{
  return m_kGroupIDInfo.GetGroupID(byTableCode, wItemTableIndex, byDivision, byClass, bySubClass, dwListIndex);
}

bool CUnmannedTraderGroupItemInfoTable::GetVersion(
  unsigned __int8 byDivision,
  unsigned __int8 byClass,
  unsigned int *dwVer)
{
  return this->m_kVerInfo.GetVersion(byDivision, byClass, dwVer);
}

bool CUnmannedTraderGroupItemInfoTable::IsExistGroupID(
  unsigned __int8 byDivision,
  unsigned __int8 byClass,
  unsigned __int8 bySubClass,
  unsigned __int8 bySortType,
  unsigned int *dwListIndex)
{
  return this->m_kGroupIDInfo.IsExistGroupID(byDivision, byClass, bySubClass, bySortType, dwListIndex) != 0;
}

CUnmannedTraderSortType *CUnmannedTraderGroupItemInfoTable::GetSortType(
  unsigned __int8 byDivision,
  unsigned __int8 bySortType)
{
  return this->m_kGroupIDInfo.GetSortType(byDivision, bySortType);
}

char CUnmannedTraderGroupItemInfoTable::IncreaseVersion(unsigned __int8 byTableCode, unsigned __int16 wItemTableIndex)
{
  if (byTableCode == 0xFF || wItemTableIndex == 65535)
  {
    return 0;
  }

  if (byTableCode >= 37)
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u) )\r\n"
      "\t\titem_tbl_num <= byTableCode!\r\n",
      byTableCode,
      wItemTableIndex);
    return 0;
  }

  _base_fld *record = g_Main.m_tblItemData[byTableCode].GetRecord(wItemTableIndex);
  if (!record)
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u) )\r\n"
      "\t\tg_Main.m_tblItemData[byTableCode].GetRecord( wItemTableIndex ) NULL!\r\n",
      byTableCode,
      wItemTableIndex);
    return 0;
  }

  unsigned __int8 byDivision[32]{};
  unsigned __int8 byClass[36]{};
  byDivision[0] = static_cast<unsigned __int8>(-1);
  byClass[0] = static_cast<unsigned __int8>(-1);
  if (!m_kGroupIDInfo.GetGroupID(byTableCode, wItemTableIndex, byDivision, byClass))
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u) )\r\n"
      "\t\tm_kGroupIDInfo.GetDivsionAndClassID( byTableCode, pFld->m_strCode(%s) Invalid!\r\n",
      byTableCode,
      wItemTableIndex,
      record->m_strCode);
    return 0;
  }

  if (!m_kVerInfo.IncreaseVersion(byDivision[0], byClass[0]))
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u) )\r\n"
      "\t\tm_kVerInfo.IncreaseVersion( byDivisioin(%u), byClass(%u) ) Fail!\r\n",
      byTableCode,
      wItemTableIndex,
      byDivision[0],
      byClass[0]);
    return 0;
  }

  return 1;
}

char CUnmannedTraderGroupItemInfoTable::IncreaseVersion(
  unsigned __int8 byTableCode,
  unsigned __int16 wItemTableIndex,
  unsigned __int8 byRegistDivision,
  unsigned __int8 byRegistClass)
{
  if (byTableCode == 0xFF || wItemTableIndex == 65535)
  {
    return 0;
  }

  if (byTableCode >= 37)
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u), BYTE byRegistDivision(%u), BYTE byRegistClass(%u) )\r\n"
      "\t\titem_tbl_num <= byTableCode!\r\n",
      byTableCode,
      wItemTableIndex,
      byRegistDivision,
      byRegistClass);
    return 0;
  }

  _base_fld *record = g_Main.m_tblItemData[byTableCode].GetRecord(wItemTableIndex);
  if (!record)
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u), BYTE byRegistDivision(%u), BYTE byRegistClass(%u) )\r\n"
      "\t\tg_Main.m_tblItemData[byTableCode].GetRecord( wItemTableIndex ) NULL!\r\n",
      byTableCode,
      wItemTableIndex,
      byRegistDivision,
      byRegistClass);
    return 0;
  }

  unsigned __int8 byDivision[32]{};
  unsigned __int8 byClass[16]{};
  byDivision[0] = static_cast<unsigned __int8>(-1);
  byClass[0] = static_cast<unsigned __int8>(-1);
  if (!m_kGroupIDInfo.GetGroupID(byTableCode, wItemTableIndex, byDivision, byClass))
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u), BYTE byRegistDivision(%u), BYTE byRegistClass(%u) )\r\n"
      "\t\tm_kGroupIDInfo.GetDivsionAndClassID( byTableCode, pFld->m_strCode(%s) Invalid!\r\n",
      byTableCode,
      wItemTableIndex,
      byRegistDivision,
      byRegistClass,
      record->m_strCode);
    return 0;
  }

  if (byRegistDivision != byDivision[0] || byRegistClass != byClass[0])
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u), BYTE byRegistDivision(%u), BYTE byRegistClass(%u) )\r\n"
      "\t\t( byRegistDivision(%u) != byDivision(%u) || byRegistClass(%u) != byClass(%u) ) Invalid!\r\n",
      byTableCode,
      wItemTableIndex,
      byRegistDivision,
      byRegistClass,
      byRegistDivision,
      byDivision[0],
      byRegistClass,
      byClass[0]);
    return 0;
  }

  if (!m_kVerInfo.IncreaseVersion(byDivision[0], byClass[0]))
  {
    Log(
      "UnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byTableCode(%u), WORD wItemTableIndex(%u), BYTE byRegistDivision(%u), BYTE byRegistClass(%u) )\r\n"
      "\t\tm_kVerInfo.IncreaseVersion( byDivision(%u), byClass(%u) ) Fail!\r\n",
      byTableCode,
      wItemTableIndex,
      byRegistDivision,
      byRegistClass,
      byDivision[0],
      byClass[0]);
    return 0;
  }

  return 1;
}

char CUnmannedTraderGroupItemInfoTable::IncreaseVersion(unsigned __int8 byDivision, unsigned __int8 byClass)
{
  if (m_kVerInfo.IncreaseVersion(byDivision, byClass))
  {
    return 1;
  }

  Log(
    "CUnmannedTraderGroupItemInfoTable::IncreaseVersion( BYTE byDivision, BYTE byClass )\r\n"
    "\t\tm_kVerInfo.IncreaseVersion( byDivision(%u), byClass(%u) ) Fail!\r\n",
    byDivision,
    byClass);
  return 0;
}
