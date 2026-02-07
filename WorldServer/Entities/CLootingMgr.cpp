#include "pch.h"

#include "CLootingMgr.h"

CLootingMgr::_list::_list()
{
  Init();
}

void CLootingMgr::_list::Init()
{
  memset_0(this, 0, sizeof(CLootingMgr::_list));
}

CLootingMgr::CLootingMgr()
{
  m_bFirst = false;
  m_byUserNode = 0;
  for (int j = 0; j < 64; ++j)
  {
    m_AtterList[j].Init();
  }
}

void CLootingMgr::Init(unsigned __int8 nUserNode)
{

  m_byUserNode = nUserNode;
  for (int j = 0; j < m_byUserNode; ++j)
  {
    m_AtterList[j].Init();
  }
  m_bFirst = true;
}