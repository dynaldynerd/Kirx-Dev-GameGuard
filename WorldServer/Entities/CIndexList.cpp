#include "pch.h"

#include "CIndexList.h"

#include <cstring>

namespace
{
  void InitList(CIndexList::_index_node &head, CIndexList::_index_node &tail)
  {
    head.m_pPrev = nullptr;
    head.m_pNext = &tail;
    tail.m_pPrev = &head;
    tail.m_pNext = nullptr;
  }

  void InsertBefore(CIndexList::_index_node *pos, CIndexList::_index_node *node)
  {
    node->m_pNext = pos;
    node->m_pPrev = pos->m_pPrev;
    pos->m_pPrev->m_pNext = node;
    pos->m_pPrev = node;
  }

  CIndexList::_index_node *PopFront(CIndexList::_index_node &head, CIndexList::_index_node &tail)
  {
    CIndexList::_index_node *node = head.m_pNext;
    if (node == &tail)
    {
      return nullptr;
    }
    head.m_pNext = node->m_pNext;
    node->m_pNext->m_pPrev = &head;
    node->m_pPrev = nullptr;
    node->m_pNext = nullptr;
    return node;
  }
}

CIndexList::_index_node::_index_node()
{
  m_bLoad = false;
  m_pNext = nullptr;
  m_pPrev = nullptr;
  m_dwInfoDataSize = 0;
  m_pInfo = nullptr;
}

CIndexList::_index_node::~_index_node()
{
  if (m_pInfo)
  {
    delete[] m_pInfo;
  }
}

CIndexList::CIndexList()
{
  m_Head.m_pPrev = &m_Head;
  m_Head.m_pNext = &m_Tail;
  m_Tail.m_pPrev = &m_Head;
  m_Tail.m_pNext = &m_Tail;
  m_BufHead.m_pPrev = &m_BufHead;
  m_BufHead.m_pNext = &m_BufTail;
  m_BufTail.m_pPrev = &m_BufHead;
  m_BufTail.m_pNext = &m_BufTail;
  m_dwCount = 0;
  m_dwBufCount = 0;
  m_dwMaxBufNum = 0;
  m_pBufNode = nullptr;
}

CIndexList::~CIndexList()
{
  if (m_pBufNode)
  {
    delete[] m_pBufNode;
    m_pBufNode = nullptr;
  }
}

bool CIndexList::_index_node::AllocInfo(unsigned int infoSize)
{
  if (infoSize == 0)
  {
    m_dwInfoDataSize = 0;
    m_pInfo = nullptr;
    return true;
  }
  m_pInfo = new char[infoSize];
  if (m_pInfo == nullptr)
  {
    m_dwInfoDataSize = 0;
    return false;
  }
  m_dwInfoDataSize = infoSize;
  std::memset(m_pInfo, 0, infoSize);
  return true;
}

bool CIndexList::SetList(unsigned int maxBufNum, unsigned int infoSize, bool useMultiThread)
{
  if (this == nullptr)
  {
    return false;
  }
  if (this->m_pBufNode != nullptr || this->m_dwMaxBufNum != 0)
  {
    return false;
  }

  this->m_dwMaxBufNum = maxBufNum;
  this->m_pBufNode = new _index_node[maxBufNum];
  if (this->m_pBufNode == nullptr)
  {
    return false;
  }
  this->m_dwCount = 0;
  this->m_dwBufCount = 0;

  InitList(this->m_Head, this->m_Tail);
  InitList(this->m_BufHead, this->m_BufTail);

  for (unsigned int j = 0; j < this->m_dwMaxBufNum; ++j)
  {
    _index_node &node = this->m_pBufNode[j];
    node.m_bLoad = false;
    node.m_dwIndex = 0;
    node.m_dwInfoDataSize = 0;
    node.m_pInfo = nullptr;
    node.m_pPrev = nullptr;
    node.m_pNext = nullptr;
    if (infoSize != 0 && !node.AllocInfo(infoSize))
    {
      return false;
    }
    InsertBefore(&this->m_BufTail, &node);
    ++this->m_dwBufCount;
  }

  this->m_csList.SetUse(useMultiThread);
  return true;
}

bool CIndexList::PushNode_Back(unsigned int index, char *infoData)
{
  if (this == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  _index_node *node = PopFront(this->m_BufHead, this->m_BufTail);
  if (node == nullptr)
  {
    this->m_csList.Unlock();
    return false;
  }

  node->m_dwIndex = index;
  if (infoData != nullptr && node->m_dwInfoDataSize != 0 && node->m_pInfo != nullptr)
  {
    std::memcpy(node->m_pInfo, infoData, node->m_dwInfoDataSize);
  }
  InsertBefore(&this->m_Tail, node);
  --this->m_dwBufCount;
  ++this->m_dwCount;
  node->m_bLoad = true;
  this->m_csList.Unlock();
  return true;
}

bool CIndexList::PushNode_Front(unsigned int index, char *infoData)
{
  if (this == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  _index_node *node = PopFront(this->m_BufHead, this->m_BufTail);
  if (node == nullptr)
  {
    this->m_csList.Unlock();
    return false;
  }

  node->m_dwIndex = index;
  if (infoData != nullptr && node->m_dwInfoDataSize != 0 && node->m_pInfo != nullptr)
  {
    std::memcpy(node->m_pInfo, infoData, node->m_dwInfoDataSize);
  }
  node->m_pNext = this->m_Head.m_pNext;
  node->m_pPrev = &this->m_Head;
  this->m_Head.m_pNext->m_pPrev = node;
  this->m_Head.m_pNext = node;
  --this->m_dwBufCount;
  ++this->m_dwCount;
  node->m_bLoad = true;
  this->m_csList.Unlock();
  return true;
}

bool CIndexList::PopNode_Front(unsigned int *pdwOutIndex, char *pInfoData)
{
  if (this == nullptr)
  {
    return false;
  }

  m_csList.Lock();
  if (m_Head.m_pNext == &m_Tail)
  {
    m_csList.Unlock();
    return false;
  }

  _index_node *node = m_Head.m_pNext;
  if (pdwOutIndex)
  {
    *pdwOutIndex = node->m_dwIndex;
  }
  if (pInfoData && node->m_dwInfoDataSize)
  {
    std::memcpy(pInfoData, node->m_pInfo, node->m_dwInfoDataSize);
  }

  m_Head.m_pNext = node->m_pNext;
  node->m_pNext->m_pPrev = &m_Head;
  --m_dwCount;
  node->m_bLoad = false;
  node->m_pNext = &m_BufTail;
  node->m_pPrev = m_BufTail.m_pPrev;
  m_BufTail.m_pPrev->m_pNext = node;
  m_BufTail.m_pPrev = node;
  ++m_dwBufCount;
  m_csList.Unlock();
  return true;
}

void CIndexList::ResetList()
{
  while (PopNode_Front(nullptr, nullptr))
  {
  }
}

bool CIndexList::FindNode(unsigned int dwIndex, char *pInfoData)
{
  if (this == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  for (_index_node *node = m_Head.m_pNext; node != &m_Tail; node = node->m_pNext)
  {
    if (node->m_dwIndex == dwIndex)
    {
      if (pInfoData && node->m_dwInfoDataSize)
      {
        std::memcpy(pInfoData, node->m_pInfo, node->m_dwInfoDataSize);
      }
      node->m_pPrev->m_pNext = node->m_pNext;
      node->m_pNext->m_pPrev = node->m_pPrev;
      --m_dwCount;
      node->m_bLoad = false;
      node->m_pNext = &m_BufTail;
      node->m_pPrev = m_BufTail.m_pPrev;
      m_BufTail.m_pPrev->m_pNext = node;
      m_BufTail.m_pPrev = node;
      ++m_dwBufCount;
      this->m_csList.Unlock();
      return true;
    }
  }

  this->m_csList.Unlock();
  return false;
}

bool CIndexList::IsInList(unsigned int dwIndex, char *pInfoData)
{
  if (this == nullptr)
  {
    return false;
  }

  m_csList.Lock();
  for (_index_node *node = m_Head.m_pNext; node != &m_Tail; node = node->m_pNext)
  {
    if (node->m_dwIndex == dwIndex)
    {
      if (pInfoData && node->m_dwInfoDataSize)
      {
        std::memcpy(pInfoData, node->m_pInfo, node->m_dwInfoDataSize);
      }
      m_csList.Unlock();
      return true;
    }
  }

  m_csList.Unlock();
  return false;
}

bool CIndexList::IsSetting()
{
  return m_pBufNode != nullptr;
}

bool CIndexList::CopyFront(unsigned int *pdwOutIndex, char *pInfoData)
{
  m_csList.Lock();
  if (m_Head.m_pNext == &m_Tail)
  {
    m_csList.Unlock();
    return false;
  }

  _index_node *frontNode = m_Head.m_pNext;
  if (pdwOutIndex)
  {
    *pdwOutIndex = frontNode->m_dwIndex;
  }

  if (pInfoData && frontNode->m_dwInfoDataSize)
  {
    std::memcpy(pInfoData, frontNode->m_pInfo, frontNode->m_dwInfoDataSize);
  }

  m_csList.Unlock();
  return true;
}

CIndexList::_index_node *CIndexList::GetAllNode(unsigned int *pdwMaxNodeNum)
{
  if (m_csList.IsUse())
  {
    return nullptr;
  }

  if (pdwMaxNodeNum)
  {
    *pdwMaxNodeNum = m_dwMaxBufNum;
  }

  return m_pBufNode;
}

unsigned int CIndexList::GetSize()
{
  return m_dwCount;
}

