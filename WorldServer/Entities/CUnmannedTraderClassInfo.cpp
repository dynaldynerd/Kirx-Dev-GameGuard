#include "pch.h"

#include "CUnmannedTraderClassInfo.h"

CUnmannedTraderClassInfo::CUnmannedTraderClassInfo(unsigned int dwID)
  : m_dwID(dwID), m_dwVer(0), m_szTypeName{}, m_szClassName{}
{
  strcpy_0(this->m_szTypeName, "NONE");
  strcpy_0(this->m_szClassName, "NONE");
}

CUnmannedTraderClassInfo::~CUnmannedTraderClassInfo()
{
}

const CUnmannedTraderClassInfo *CUnmannedTraderClassInfo::Copy(const CUnmannedTraderClassInfo *lhs)
{
  this->m_dwID = lhs->m_dwID;
  this->m_dwVer = lhs->m_dwVer;
  return this;
}

unsigned int CUnmannedTraderClassInfo::GetID()
{
  return this->m_dwID;
}

char *CUnmannedTraderClassInfo::GetTypeName()
{
  return this->m_szTypeName;
}
