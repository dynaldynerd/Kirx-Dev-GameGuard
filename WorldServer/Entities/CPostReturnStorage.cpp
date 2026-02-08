#include "pch.h"

#include "CPostReturnStorage.h"

CPostReturnStorage::CPostReturnStorage()
{
  Init();
}

void CPostReturnStorage::Init()
{
  for (int j = 0; j < 10; ++j)
  {
    m_PostData[j].Init();
  }
  m_nSize = 0;
}

int CPostReturnStorage::GetSize()
{
  return m_nSize;
}

CPostData *CPostReturnStorage::GetPostDataFromInx(unsigned int nIndex)
{
  if (nIndex < 10)
  {
    return &m_PostData[nIndex];
  }
  return nullptr;
}
