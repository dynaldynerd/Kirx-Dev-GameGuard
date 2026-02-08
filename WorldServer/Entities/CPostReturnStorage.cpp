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

CPostData *CPostReturnStorage::AddReturnPost(
  unsigned __int8 byErrCode,
  unsigned int dwPostSerial,
  unsigned __int8 byState,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned __int64 lnUID)
{
  if (m_nSize >= 10)
  {
    return nullptr;
  }

  m_PostData[m_nSize].SetState(byState);
  m_PostData[m_nSize].SetReturnPostData(
    byErrCode,
    dwPostSerial,
    wszRecvName,
    wszTitle,
    wszContent,
    Key,
    dwDur,
    dwUpt,
    dwGold);
  m_PostData[m_nSize].SetPostItemSerial(lnUID);
  return &m_PostData[m_nSize++];
}

CPostData *CPostReturnStorage::GetPostDataFromSerial(unsigned int dwPostSerial)
{
  for (int j = 0; j < 10; ++j)
  {
    if (m_PostData[j].m_dwPSSerial == dwPostSerial)
    {
      return &m_PostData[j];
    }
  }
  return nullptr;
}

void CPostReturnStorage::DelPostData(unsigned int dwPostSerial)
{
  for (int j = 0; j < 10; ++j)
  {
    if (m_PostData[j].m_dwPSSerial == dwPostSerial)
    {
      m_PostData[j].Init();
      if (--m_nSize < 0)
      {
        m_nSize = 0;
      }
      return;
    }
  }
}

unsigned int CPostReturnStorage::GetReturnPostInx()
{
  for (int j = 0; j < 10; ++j)
  {
    if (m_PostData[j].GetState() == 100)
    {
      return static_cast<unsigned int>(j);
    }
  }
  return 0xFFFFFFFFu;
}
