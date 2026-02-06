#include "pch.h"

#include "CUnmannedTraderSubClassFactory.h"
#include "CUnmannedTraderSubClassInfoCode.h"
#include "CUnmannedTraderSubClassInfoDefault.h"
#include "CUnmannedTraderSubClassInfoForceLiverGrade.h"
#include "CUnmannedTraderSubClassInfoLevel.h"

CUnmannedTraderSubClassFactory::CUnmannedTraderSubClassFactory()
  : m_vecTable()
{
  CUnmannedTraderSubClassInfoDefault *defaultInfo = new CUnmannedTraderSubClassInfoDefault(0xFFFFFFFF);
  Regist(defaultInfo);

  CUnmannedTraderSubClassInfoLevel *levelInfo = new CUnmannedTraderSubClassInfoLevel(0xFFFFFFFF);
  Regist(levelInfo);

  CUnmannedTraderSubClassInfoCode *codeInfo = new CUnmannedTraderSubClassInfoCode(0xFFFFFFFF);
  Regist(codeInfo);

  CUnmannedTraderSubClassInfoForceLiverGrade *forceInfo =
    new CUnmannedTraderSubClassInfoForceLiverGrade(0xFFFFFFFF);
  Regist(forceInfo);
}

CUnmannedTraderSubClassFactory::~CUnmannedTraderSubClassFactory()
{
  Destroy();
}

CUnmannedTraderSubClassInfo *CUnmannedTraderSubClassFactory::Create(const char *szType, unsigned int dwID)
{
  if (this->m_vecTable.empty() || dwID == static_cast<unsigned int>(-1))
  {
    return nullptr;
  }

  for (CUnmannedTraderSubClassInfo *info : this->m_vecTable)
  {
    if (!info)
    {
      continue;
    }
    const char *typeName = info->GetTypeName();
    if (!strcmp_0(typeName, szType))
    {
      return info->Create(dwID);
    }
  }
  return nullptr;
}

bool CUnmannedTraderSubClassFactory::Regist(CUnmannedTraderSubClassInfo *pkType)
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

  for (CUnmannedTraderSubClassInfo *info : this->m_vecTable)
  {
    if (!info)
    {
      continue;
    }
    const char *lhsType = pkType->GetTypeName();
    const char *rhsType = info->GetTypeName();
    if (!strcmp_0(rhsType, lhsType))
    {
      delete pkType;
      return false;
    }
  }

  this->m_vecTable.push_back(pkType);
  return true;
}

void CUnmannedTraderSubClassFactory::Destroy()
{
  if (this->m_vecTable.empty())
  {
    return;
  }

  for (CUnmannedTraderSubClassInfo *info : this->m_vecTable)
  {
    if (info)
    {
      delete info;
    }
  }
  this->m_vecTable.clear();
}
