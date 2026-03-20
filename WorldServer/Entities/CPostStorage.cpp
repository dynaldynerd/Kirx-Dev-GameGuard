#include "pch.h"

#include "CPostStorage.h"

#include "CPostSystemManager.h"

CPostStorage::CPostStorage()
{
  Init();
}

CPostStorage::~CPostStorage()
{
  // this is not a stub
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
  if (m_PostData[nIndex].m_byState == 255)
  {
    return nullptr;
  }
  return &m_PostData[nIndex];
}

int CPostStorage::AddPostTitleData(
  unsigned int nIndex,
  unsigned int dwSerial,
  unsigned __int8 byState,
  char *wszSendName,
  char *wszTitle,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned __int64 lnUID)
{
  if (nIndex >= 50)
  {
    return -1;
  }

  for (int j = 0; j < 50; ++j)
  {
    if (m_PostData[j].GetState() == 255)
    {
      m_PostData[j].SetPostTitleData(
        nIndex,
        dwSerial,
        byState,
        wszSendName,
        wszTitle,
        Key,
        dwDur,
        dwUpt,
        dwGold,
        true);
      m_PostData[j].m_bUpdateIndex = true;
      m_PostData[j].SetPostItemSerial(lnUID);
      // narrowing cast for thunk return parity
      return static_cast<int>(static_cast<unsigned int>(++m_nSize));
    }
  }

  return -1;
}

bool CPostStorage::AddPostTitleDataByStorageIndex(
  unsigned int nStorageIndex,
  int nNumber,
  unsigned int dwSerial,
  unsigned __int8 byState,
  char *wszSendName,
  char *wszTitle,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned __int64 lnUID)
{
  if (nStorageIndex <= 0x31 && m_nSize <= 50)
  {
    m_PostData[nStorageIndex].SetPostTitleData(
      nNumber,
      dwSerial,
      byState,
      wszSendName,
      wszTitle,
      Key,
      dwDur,
      dwUpt,
      dwGold,
      false);
    m_PostData[nStorageIndex].SetPostItemSerial(lnUID);
    ++m_nSize;
    return true;
  }

  CPostSystemManager *manager = CPostSystemManager::Instace();
  manager->Log(
    "CPostStorage::AddPostTitleDataByStorageIndex() : Invalid nStorageIndex(%d) : StorageSize(%d)",
    nStorageIndex,
    m_nSize);
  return false;
}

int CPostStorage::AddNewPost(
  unsigned int dwSenderSerial,
  char *wszSendName,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  _INVENKEY Key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned int dwPSSerial,
  unsigned __int8 byState,
  int *nNumber,
  unsigned __int64 lnUID)
{
  if (m_nSize >= 50)
  {
    return -1;
  }

  for (int j = 0; j < 50; ++j)
  {
    if (m_PostData[j].GetState() == 255)
    {
      m_PostData[j].SetState(byState);
      m_PostData[j].SetPostData(
        m_nSize,
        dwSenderSerial,
        wszSendName,
        wszRecvName,
        wszTitle,
        Key,
        dwDur,
        dwUpt,
        dwGold,
        dwPSSerial,
        0,
        0);
      m_PostData[j].SetPostItemSerial(lnUID);
      m_PostData[j].SetPostContent(wszContent);
      *nNumber = m_nSize++;
      // narrowing cast for thunk return parity
      return static_cast<int>(static_cast<unsigned int>(j));
    }
  }

  return -1;
}

CPostData *CPostStorage::GetPostDataFromSerial(unsigned int dwPostSerial)
{
  for (int j = 0; j < 50; ++j)
  {
    if (m_PostData[j].m_dwPSSerial == dwPostSerial)
    {
      return &m_PostData[j];
    }
  }
  return nullptr;
}

void CPostStorage::DelPostData(unsigned int dwIndex)
{
  if (dwIndex < 50)
  {
    m_PostData[dwIndex].Init();
    --m_nSize;
  }
}

bool CPostStorage::IsContentLoad(unsigned int dwIndex)
{
  return dwIndex >= 50 || m_PostData[dwIndex].m_bContentLoad;
}

int CPostStorage::SetPostContent(unsigned int dwSerial, char *wszContent)
{
  for (int j = 0; j < 50; ++j)
  {
    if (m_PostData[j].m_dwPSSerial == dwSerial)
    {
      m_PostData[j].SetPostContent(wszContent);
      // narrowing cast for thunk return parity
      return static_cast<int>(static_cast<unsigned int>(j));
    }
  }
  return -1;
}
