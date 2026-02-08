#include "pch.h"

#include "CNationSettingFactory.h"

CNationSettingFactory::CNationSettingFactory(int iType)
  : m_iType(iType)
{
}

bool CNationSettingFactory::IsNULL()
{
  // this is not a stub
  return false;
}

int CNationSettingFactory::GetKey()
{
  return m_iType;
}
