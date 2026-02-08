#include "pch.h"

#include "CPostStorage.h"

CPostStorage::CPostStorage()
{
  Init();
}

void CPostStorage::Init()
{
  for (int j = 0; j < 50; ++j)
  {
    m_PostData[j].Init();
  }
  m_nSize = 0;
  m_bUpdate = false;
}

int CPostStorage::GetSize()
{
  return m_nSize;
}

CPostData *CPostStorage::GetPostDataFromInx(unsigned int nIndex)
{
  if (nIndex >= 50)
  {
    return nullptr;
  }
  if (m_PostData[nIndex].m_byState == 0xFF)
  {
    return nullptr;
  }
  return &m_PostData[nIndex];
}
