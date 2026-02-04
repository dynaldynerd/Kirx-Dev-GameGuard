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

bool CNetIndexList::SetList(CNetIndexList *list, unsigned int maxBufNum)
{
  if (list == nullptr)
  {
    return false;
  }
  if (list->m_pBufNode != nullptr || list->m_dwMaxBufNum != 0)
  {
    return false;
  }

  list->m_dwMaxBufNum = maxBufNum;
  list->m_pBufNode = new _index_node[maxBufNum];
  list->m_dwBufCount = 0;
  list->m_dwCount = 0;

  InitList(list->m_Head, list->m_Tail);
  InitList(list->m_BufHead, list->m_BufTail);

  for (unsigned int j = 0; j < list->m_dwMaxBufNum; ++j)
  {
    InsertBefore(&list->m_BufTail, &list->m_pBufNode[j]);
    ++list->m_dwBufCount;
  }

  return true;
}

bool CNetIndexList::PushNode_Back(CNetIndexList *list, unsigned int index)
{
  if (list == nullptr)
  {
    return false;
  }

  _index_node *node = PopFront(list->m_BufHead, list->m_BufTail);
  if (node == nullptr)
  {
    return false;
  }

  node->m_dwIndex = index;
  InsertBefore(&list->m_Tail, node);
  --list->m_dwBufCount;
  ++list->m_dwCount;
  return true;
}

bool CNetIndexList::PushNode_Front(CNetIndexList *list, unsigned int index)
{
  if (list == nullptr)
  {
    return false;
  }

  _index_node *node = PopFront(list->m_BufHead, list->m_BufTail);
  if (node == nullptr)
  {
    return false;
  }

  node->m_dwIndex = index;
  node->m_pNext = list->m_Head.m_pNext;
  node->m_pPrev = &list->m_Head;
  list->m_Head.m_pNext->m_pPrev = node;
  list->m_Head.m_pNext = node;
  --list->m_dwBufCount;
  ++list->m_dwCount;
  return true;
}

bool CNetIndexList::PopNode_Front(CNetIndexList *list, unsigned int *outIndex)
{
  if (list == nullptr || outIndex == nullptr)
  {
    return false;
  }

  _index_node *node = PopFront(list->m_Head, list->m_Tail);
  if (node == nullptr)
  {
    return false;
  }

  *outIndex = node->m_dwIndex;
  InsertBefore(&list->m_BufTail, node);
  --list->m_dwCount;
  ++list->m_dwBufCount;
  return true;
}

bool CNetIndexList::CopyFront(CNetIndexList *list, unsigned int *outIndex)
{
  if (list == nullptr || outIndex == nullptr)
  {
    return false;
  }

  list->m_csList.Lock();
  if (list->m_Head.m_pNext == &list->m_Tail)
  {
    list->m_csList.Unlock();
    return false;
  }

  *outIndex = list->m_Head.m_pNext->m_dwIndex;
  list->m_csList.Unlock();
  return true;
}

CNetIndexList::_index_node *CNetIndexList::FindNode(CNetIndexList *list, unsigned int index)
{
  if (list == nullptr)
  {
    return nullptr;
  }

  list->m_csList.Lock();
  _index_node *node = list->m_Head.m_pNext;
  while (node != &list->m_Tail)
  {
    if (node->m_dwIndex == index)
    {
      node->m_pPrev->m_pNext = node->m_pNext;
      node->m_pNext->m_pPrev = node->m_pPrev;
      --list->m_dwCount;
      InsertBefore(&list->m_BufTail, node);
      ++list->m_dwBufCount;
      list->m_csList.Unlock();
      return node;
    }
    node = node->m_pNext;
  }
  list->m_csList.Unlock();
  return nullptr;
}

void CNetIndexList::ResetList(CNetIndexList *list)
{
  if (list == nullptr)
  {
    return;
  }

  unsigned int outIndex = 0;
  while (CNetIndexList::PopNode_Front(list, &outIndex))
  {
  }
}

bool CNetIndexList::IsInList(CNetIndexList *list, unsigned int index)
{
  if (list == nullptr)
  {
    return false;
  }

  _index_node *node = list->m_Head.m_pNext;
  while (node != &list->m_Tail)
  {
    if (node->m_dwIndex == index)
    {
      return true;
    }
    node = node->m_pNext;
  }
  return false;
}

unsigned int CNetIndexList::size(const CNetIndexList *list)
{
  if (list == nullptr)
  {
    return 0;
  }
  return list->m_dwCount;
}

