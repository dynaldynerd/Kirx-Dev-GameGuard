#include "pch.h"

#include "CRFMonsterAIMgr.h"

CRFMonsterAIMgr *CRFMonsterAIMgr::ms_Instance = nullptr;

CRFMonsterAIMgr::CRFMonsterAIMgr()
{
  for (int i = 0; i < 1; ++i)
  {
    new (&m_spStateTBLPoolPtr[i]) UsPoint<UsStateTBL>();
  }
}

CRFMonsterAIMgr::~CRFMonsterAIMgr()
{
  for (int i = 0; i < 1; ++i)
  {
    m_spStateTBLPoolPtr[i].~UsPoint<UsStateTBL>();
  }
}

CRFMonsterAIMgr *CRFMonsterAIMgr::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CRFMonsterAIMgr();
  }
  return ms_Instance;
}

void CRFMonsterAIMgr::Destory()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

UsPoint<UsStateTBL> *CRFMonsterAIMgr::GetStateTBL(UsPoint<UsStateTBL> *result, int nIndex)
{
  if (nIndex)
  {
    new (result) UsPoint<UsStateTBL>(static_cast<UsStateTBL *>(nullptr));
  }
  else
  {
    new (result) UsPoint<UsStateTBL>(m_spStateTBLPoolPtr[0]);
  }
  return result;
}
