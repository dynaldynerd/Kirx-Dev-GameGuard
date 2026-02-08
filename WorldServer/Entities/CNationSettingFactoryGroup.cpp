#include "pch.h"

#include "CNationSettingFactoryGroup.h"

#include "CNationSettingFactoryBR.h"
#include "CNationSettingFactoryCN.h"
#include "CNationSettingFactoryES.h"
#include "CNationSettingFactoryGB.h"
#include "CNationSettingFactoryID.h"
#include "CNationSettingFactoryJP.h"
#include "CNationSettingFactoryKR.h"
#include "CNationSettingFactoryPH.h"
#include "CNationSettingFactoryRU.h"
#include "CNationSettingFactoryTH.h"
#include "CNationSettingFactoryTW.h"
#include "CNationSettingFactoryUS.h"

#include <new>

CNationSettingFactoryGroup::CNationSettingFactoryGroup()
  : m_kPool()
{
}

CNationSettingFactoryGroup::~CNationSettingFactoryGroup() = default;

int CNationSettingFactoryGroup::Init()
{
  CNationSettingFactory *factory = nullptr;
  auto *factoryKr = static_cast<CNationSettingFactoryKR *>(operator new(0x10uLL));
  if (factoryKr)
  {
    new (factoryKr) CNationSettingFactoryKR();
    factory = factoryKr;
  }
  if (m_kPool.regist(factory))
  {
    return -1;
  }

  factory = nullptr;
  auto *factoryGb = static_cast<CNationSettingFactoryGB *>(operator new(0x10uLL));
  if (factoryGb)
  {
    new (factoryGb) CNationSettingFactoryGB();
    factory = factoryGb;
  }
  if (m_kPool.regist(factory))
  {
    return -2;
  }

  factory = nullptr;
  auto *factoryId = static_cast<CNationSettingFactoryID *>(operator new(0x10uLL));
  if (factoryId)
  {
    new (factoryId) CNationSettingFactoryID();
    factory = factoryId;
  }
  if (m_kPool.regist(factory))
  {
    return -3;
  }

  factory = nullptr;
  auto *factoryJp = static_cast<CNationSettingFactoryJP *>(operator new(0x10uLL));
  if (factoryJp)
  {
    new (factoryJp) CNationSettingFactoryJP();
    factory = factoryJp;
  }
  if (m_kPool.regist(factory))
  {
    return -4;
  }

  factory = nullptr;
  auto *factoryPh = static_cast<CNationSettingFactoryPH *>(operator new(0x10uLL));
  if (factoryPh)
  {
    new (factoryPh) CNationSettingFactoryPH();
    factory = factoryPh;
  }
  if (m_kPool.regist(factory))
  {
    return -5;
  }

  factory = nullptr;
  auto *factoryRu = static_cast<CNationSettingFactoryRU *>(operator new(0x10uLL));
  if (factoryRu)
  {
    new (factoryRu) CNationSettingFactoryRU();
    factory = factoryRu;
  }
  if (m_kPool.regist(factory))
  {
    return -6;
  }

  factory = nullptr;
  auto *factoryBr = static_cast<CNationSettingFactoryBR *>(operator new(0x10uLL));
  if (factoryBr)
  {
    new (factoryBr) CNationSettingFactoryBR();
    factory = factoryBr;
  }
  if (m_kPool.regist(factory))
  {
    return -7;
  }

  factory = nullptr;
  auto *factoryTw = static_cast<CNationSettingFactoryTW *>(operator new(0x10uLL));
  if (factoryTw)
  {
    new (factoryTw) CNationSettingFactoryTW();
    factory = factoryTw;
  }
  if (m_kPool.regist(factory))
  {
    return -8;
  }

  factory = nullptr;
  auto *factoryCn = static_cast<CNationSettingFactoryCN *>(operator new(0x10uLL));
  if (factoryCn)
  {
    new (factoryCn) CNationSettingFactoryCN();
    factory = factoryCn;
  }
  if (m_kPool.regist(factory))
  {
    return -9;
  }

  factory = nullptr;
  auto *factoryUs = static_cast<CNationSettingFactoryUS *>(operator new(0x10uLL));
  if (factoryUs)
  {
    new (factoryUs) CNationSettingFactoryUS();
    factory = factoryUs;
  }
  if (m_kPool.regist(factory))
  {
    return -10;
  }

  factory = nullptr;
  auto *factoryEs = static_cast<CNationSettingFactoryES *>(operator new(0x10uLL));
  if (factoryEs)
  {
    new (factoryEs) CNationSettingFactoryES();
    factory = factoryEs;
  }
  if (m_kPool.regist(factory))
  {
    return -11;
  }

  factory = nullptr;
  auto *factoryTh = static_cast<CNationSettingFactoryTH *>(operator new(0x10uLL));
  if (factoryTh)
  {
    new (factoryTh) CNationSettingFactoryTH();
    factory = factoryTh;
  }
  if (m_kPool.regist(factory))
  {
    return -12;
  }

  return 0;
}

CNationSettingData *CNationSettingFactoryGroup::Create(
  int iNationCode,
  const char *szNationCodeStr,
  bool bServiceMode)
{
  CNationSettingFactory *factory = nullptr;
  if (m_kPool.get(iNationCode, &factory))
  {
    return factory->Create(iNationCode, szNationCodeStr, bServiceMode);
  }
  return nullptr;
}
