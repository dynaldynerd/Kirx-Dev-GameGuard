#include "pch.h"

#include "CNetIndexList.h"

namespace
{
  void InitList(CNetIndexList::_index_node &head, CNetIndexList::_index_node &tail)
  {
    head.m_pPrev = nullptr;
    head.m_pNext = &tail;
    tail.m_pPrev = &head;
    tail.m_pNext = nullptr;
  }

  void InsertBefore(CNetIndexList::_index_node *pos, CNetIndexList::_index_node *node)
  {
    node->m_pNext = pos;
    node->m_pPrev = pos->m_pPrev;
    pos->m_pPrev->m_pNext = node;
    pos->m_pPrev = node;
  }

  CNetIndexList::_index_node *PopFront(CNetIndexList::_index_node &head, CNetIndexList::_index_node &tail)
  {
    CNetIndexList::_index_node *node = head.m_pNext;
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

CNetIndexList::_index_node::_index_node()
  : m_dwIndex(0),
    m_pPrev(nullptr),
    m_pNext(nullptr)
{
}

CNetIndexList::CNetIndexList()
  : m_Head(),
    m_Tail(),
    m_BufHead(),
    m_BufTail(),
    m_pBufNode(nullptr),
    m_dwCount(0),
    m_dwBufCount(0),
    m_dwMaxBufNum(0)
{
  m_Head.m_pPrev = &m_Head;
  m_Head.m_pNext = &m_Tail;
  m_Tail.m_pPrev = &m_Head;
  m_Tail.m_pNext = &m_Tail;
  m_BufHead.m_pPrev = &m_BufHead;
  m_BufHead.m_pNext = &m_BufTail;
  m_BufTail.m_pPrev = &m_BufHead;
  m_BufTail.m_pNext = &m_BufTail;
}

CNetIndexList::~CNetIndexList()
{
  if (m_pBufNode)
  {
    delete[] m_pBufNode;
    m_pBufNode = nullptr;
  }
}

bool CNetIndexList::SetList(unsigned int maxBufNum)
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
  this->m_dwBufCount = 0;
  this->m_dwCount = 0;

  InitList(this->m_Head, this->m_Tail);
  InitList(this->m_BufHead, this->m_BufTail);

  for (unsigned int j = 0; j < this->m_dwMaxBufNum; ++j)
  {
    InsertBefore(&this->m_BufTail, &this->m_pBufNode[j]);
    ++this->m_dwBufCount;
  }

  return true;
}

bool CNetIndexList::PushNode_Back(unsigned int index)
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
  InsertBefore(&this->m_Tail, node);
  --this->m_dwBufCount;
  ++this->m_dwCount;
  this->m_csList.Unlock();
  return true;
}

bool CNetIndexList::PushNode_Front(unsigned int index)
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
  node->m_pNext = this->m_Head.m_pNext;
  node->m_pPrev = &this->m_Head;
  this->m_Head.m_pNext->m_pPrev = node;
  this->m_Head.m_pNext = node;
  --this->m_dwBufCount;
  ++this->m_dwCount;
  this->m_csList.Unlock();
  return true;
}

bool CNetIndexList::PopNode_Front(unsigned int *outIndex)
{
  if (this == nullptr || outIndex == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  _index_node *node = PopFront(this->m_Head, this->m_Tail);
  if (node == nullptr)
  {
    this->m_csList.Unlock();
    return false;
  }

  *outIndex = node->m_dwIndex;
  InsertBefore(&this->m_BufTail, node);
  --this->m_dwCount;
  ++this->m_dwBufCount;
  this->m_csList.Unlock();
  return true;
}

bool CNetIndexList::PopNode_Back(unsigned int *outIndex)
{
  if (this == nullptr || outIndex == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  if (this->m_Tail.m_pPrev == &this->m_Head)
  {
    this->m_csList.Unlock();
    return false;
  }

  _index_node *node = this->m_Tail.m_pPrev;
  *outIndex = node->m_dwIndex;
  this->m_Tail.m_pPrev = node->m_pPrev;
  node->m_pPrev->m_pNext = &this->m_Tail;
  --this->m_dwCount;
  InsertBefore(&this->m_BufTail, node);
  ++this->m_dwBufCount;
  this->m_csList.Unlock();
  return true;
}

bool CNetIndexList::CopyFront(unsigned int *outIndex)
{
  if (this == nullptr || outIndex == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  if (this->m_Head.m_pNext == &this->m_Tail)
  {
    this->m_csList.Unlock();
    return false;
  }

  *outIndex = this->m_Head.m_pNext->m_dwIndex;
  this->m_csList.Unlock();
  return true;
}

int CNetIndexList::CopyIndexList(unsigned int *pdwList, int nMax)
{
  if (this == nullptr || pdwList == nullptr || nMax <= 0)
  {
    return 0;
  }

  this->m_csList.Lock();
  _index_node *node = this->m_Head.m_pNext;
  int count = 0;
  while (node != &this->m_Tail)
  {
    pdwList[count++] = node->m_dwIndex;
    if (count >= nMax)
    {
      break;
    }
    node = node->m_pNext;
  }
  this->m_csList.Unlock();
  return count;
}

CNetIndexList::_index_node *CNetIndexList::FindNode(unsigned int index)
{
  if (this == nullptr)
  {
    return nullptr;
  }

  this->m_csList.Lock();
  _index_node *node = this->m_Head.m_pNext;
  while (node != &this->m_Tail)
  {
    if (node->m_dwIndex == index)
    {
      node->m_pPrev->m_pNext = node->m_pNext;
      node->m_pNext->m_pPrev = node->m_pPrev;
      --this->m_dwCount;
      InsertBefore(&this->m_BufTail, node);
      ++this->m_dwBufCount;
      this->m_csList.Unlock();
      return node;
    }
    node = node->m_pNext;
  }
  this->m_csList.Unlock();
  return nullptr;
}

void CNetIndexList::ResetList()
{
  if (this == nullptr)
  {
    return;
  }

  unsigned int outIndex = 0;
  while (PopNode_Front(&outIndex))
  {
  }
}

bool CNetIndexList::IsInList(unsigned int index)
{
  if (this == nullptr)
  {
    return false;
  }

  this->m_csList.Lock();
  _index_node *node = this->m_Head.m_pNext;
  while (node != &this->m_Tail)
  {
    if (node->m_dwIndex == index)
    {
      this->m_csList.Unlock();
      return true;
    }
    node = node->m_pNext;
  }
  this->m_csList.Unlock();
  return false;
}

unsigned int CNetIndexList::size()
{
  if (this == nullptr)
  {
    return 0;
  }
  return this->m_dwCount;
}

