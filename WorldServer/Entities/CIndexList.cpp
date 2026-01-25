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

bool CIndexList::SetList(CIndexList *list, unsigned int maxBufNum, unsigned int infoSize, bool useMultiThread)
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
  if (list->m_pBufNode == nullptr)
  {
    return false;
  }
  list->m_dwCount = 0;
  list->m_dwBufCount = 0;

  InitList(list->m_Head, list->m_Tail);
  InitList(list->m_BufHead, list->m_BufTail);

  for (unsigned int j = 0; j < list->m_dwMaxBufNum; ++j)
  {
    _index_node &node = list->m_pBufNode[j];
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
    InsertBefore(&list->m_BufTail, &node);
    ++list->m_dwBufCount;
  }

  CSyncCS::SetUse(&list->m_csList, useMultiThread);
  return true;
}

bool CIndexList::PushNode_Back(CIndexList *list, unsigned int index, char *infoData)
{
  if (list == nullptr)
  {
    return false;
  }

  CSyncCS::Lock(&list->m_csList);
  _index_node *node = PopFront(list->m_BufHead, list->m_BufTail);
  if (node == nullptr)
  {
    CSyncCS::Unlock(&list->m_csList);
    return false;
  }

  node->m_dwIndex = index;
  if (infoData != nullptr && node->m_dwInfoDataSize != 0 && node->m_pInfo != nullptr)
  {
    std::memcpy(node->m_pInfo, infoData, node->m_dwInfoDataSize);
  }
  InsertBefore(&list->m_Tail, node);
  --list->m_dwBufCount;
  ++list->m_dwCount;
  node->m_bLoad = true;
  CSyncCS::Unlock(&list->m_csList);
  return true;
}

bool CIndexList::PushNode_Front(CIndexList *list, unsigned int index, char *infoData)
{
  if (list == nullptr)
  {
    return false;
  }

  CSyncCS::Lock(&list->m_csList);
  _index_node *node = PopFront(list->m_BufHead, list->m_BufTail);
  if (node == nullptr)
  {
    CSyncCS::Unlock(&list->m_csList);
    return false;
  }

  node->m_dwIndex = index;
  if (infoData != nullptr && node->m_dwInfoDataSize != 0 && node->m_pInfo != nullptr)
  {
    std::memcpy(node->m_pInfo, infoData, node->m_dwInfoDataSize);
  }
  node->m_pNext = list->m_Head.m_pNext;
  node->m_pPrev = &list->m_Head;
  list->m_Head.m_pNext->m_pPrev = node;
  list->m_Head.m_pNext = node;
  --list->m_dwBufCount;
  ++list->m_dwCount;
  node->m_bLoad = true;
  CSyncCS::Unlock(&list->m_csList);
  return true;
}

