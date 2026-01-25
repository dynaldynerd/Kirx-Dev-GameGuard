#pragma once

#include "IdaCompat.h"
#include "CNetCriticalSection.h"

/* 1279 */
class __cppobj __declspec(align(8)) CNetIndexList
{
public:
  struct _index_node
  {
    unsigned int m_dwIndex;
    _index_node *m_pPrev;
    _index_node *m_pNext;
  };

  _index_node m_Head;
  _index_node m_Tail;
  _index_node m_BufHead;
  _index_node m_BufTail;
  _index_node *m_pBufNode;
  CNetCriticalSection m_csList;
  unsigned int m_dwCount;
  unsigned int m_dwBufCount;
  unsigned int m_dwMaxBufNum;
};

