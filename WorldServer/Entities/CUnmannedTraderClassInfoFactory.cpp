#include "pch.h"

#include "CUnmannedTraderClassInfoFactory.h"
#include "CUnmannedTraderClassInfoTableCodeType.h"
#include "CUnmannedTraderClassInfoTableType.h"

CUnmannedTraderClassInfoFactory::CUnmannedTraderClassInfoFactory()
  : m_vecTable()
{
  CUnmannedTraderClassInfoTableType *tableType = new CUnmannedTraderClassInfoTableType(-1);
  Regist(tableType);

  CUnmannedTraderClassInfoTableCodeType *codeType = new CUnmannedTraderClassInfoTableCodeType(-1);
  Regist(codeType);
}

CUnmannedTraderClassInfoFactory::~CUnmannedTraderClassInfoFactory()
{
  Destroy();
}

CUnmannedTraderClassInfo *CUnmannedTraderClassInfoFactory::Create(const char *szType, unsigned int dwID)
{
  if (this->m_vecTable.empty() || dwID == static_cast<unsigned int>(-1))
  {
    return nullptr;
  }

  for (CUnmannedTraderClassInfo *info : this->m_vecTable)
  {
    if (!info)
    {
      continue;
    }

    const char *typeName = info->GetTypeName();
    if (!std::strcmp(typeName, szType))
    {
      return info->Create(dwID);
    }
  }
  return nullptr;
}

bool CUnmannedTraderClassInfoFactory::Regist(CUnmannedTraderClassInfo *pkType)
{
  if (!pkType)
  {
    return false;
  }

  if (this->m_vecTable.empty())
  {
    this->m_vecTable.push_back(pkType);
    return true;
  }

  for (CUnmannedTraderClassInfo *info : this->m_vecTable)
  {
    if (!info)
    {
      continue;
    }
    const char *lhsType = pkType->GetTypeName();
    const char *rhsType = info->GetTypeName();
    if (!std::strcmp(rhsType, lhsType))
    {
      delete pkType;
      return false;
    }
  }

  this->m_vecTable.push_back(pkType);
  return true;
}

void CUnmannedTraderClassInfoFactory::Destroy()
{
  if (this->m_vecTable.empty())
  {
    return;
  }

  for (CUnmannedTraderClassInfo *info : this->m_vecTable)
  {
    if (info)
    {
      delete info;
    }
  }
}
