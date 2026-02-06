#include "pch.h"

#include "CLootingMgr.h"

CLootingMgr::_list::_list()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  Init();
}

void CLootingMgr::_list::Init()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  memset_0(this, 0, sizeof(CLootingMgr::_list));
}

CLootingMgr::CLootingMgr()
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 8; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }
  m_bFirst = false;
  m_byUserNode = 0;
  for (int j = 0; j < 64; ++j)
  {
    m_AtterList[j].Init();
  }
}

void CLootingMgr::Init(unsigned __int8 nUserNode)
{
  __int64 stackFill = 0;
  auto *fillPtr = &stackFill;
  for (int fillCount = 12; fillCount; --fillCount)
  {
    *reinterpret_cast<unsigned int *>(fillPtr) = 0xCCCCCCCC;
    fillPtr = reinterpret_cast<__int64 *>(reinterpret_cast<char *>(fillPtr) + 4);
  }

  m_byUserNode = nUserNode;
  for (int j = 0; j < m_byUserNode; ++j)
  {
    m_AtterList[j].Init();
  }
  m_bFirst = true;
}
