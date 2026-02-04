#pragma once

#include "IdaCompat.h"
#include "CNetIndexList.h"
#include "CNetCriticalSection.h"

#include <new>

template <typename T>
class ListHeap
{
public:
  class CIndexListEx : public CNetIndexList
  {
  public:
    CIndexListEx() : CNetIndexList() {}
    ~CIndexListEx() = default;

    bool Push(CNetIndexList::_index_node *pos, unsigned int dwIndex)
    {
      m_csList.Lock();
      if (pos && m_BufHead.m_pNext != &m_BufTail)
      {
        CNetIndexList::_index_node *node = m_BufHead.m_pNext;
        m_BufHead.m_pNext = node->m_pNext;
        node->m_pNext->m_pPrev = &m_BufHead;
        --m_dwBufCount;

        node->m_dwIndex = dwIndex;
        node->m_pNext = pos;
        node->m_pPrev = pos->m_pPrev;
        pos->m_pPrev->m_pNext = node;
        pos->m_pPrev = node;
        ++m_dwCount;

        m_csList.Unlock();
        return true;
      }
      m_csList.Unlock();
      return false;
    }
  };

  ListHeap() = default;
  ~ListHeap()
  {
    _listData.~CIndexListEx();
    _listEmpty.~CIndexListEx();
  }

  bool Initialize(unsigned __int64 nMaxBuf)
  {
    const size_t allocSize = sizeof(T) * static_cast<size_t>(nMaxBuf);
    void *raw = operator new[](allocSize);
    if (!raw)
    {
      return false;
    }

    T *buffer = static_cast<T *>(raw);
    for (unsigned __int64 j = 0; j < nMaxBuf; ++j)
    {
      new (buffer + j) T();
    }

    _pBuf = buffer;
    memset_0(_pBuf, 0, allocSize);
    CNetIndexList::SetList(&_listData, static_cast<unsigned int>(nMaxBuf));
    CNetIndexList::SetList(&_listEmpty, static_cast<unsigned int>(nMaxBuf));
    for (unsigned int j = 0; j < nMaxBuf; ++j)
    {
      CNetIndexList::PushNode_Back(&_listEmpty, j);
    }
    _nMaxSize = nMaxBuf;
    return true;
  }

  void Release()
  {
    unsigned int outIndex = 0;
    for (unsigned __int64 j = 0; j < _nMaxSize && CNetIndexList::PopNode_Front(&_listData, &outIndex); ++j)
    {
      CNetIndexList::PushNode_Back(&_listEmpty, outIndex);
    }
  }

  bool push(const T &data)
  {
    unsigned int outIndex = 0;
    if (!CNetIndexList::PopNode_Front(&_listEmpty, &outIndex))
    {
      return false;
    }

    memcpy_0(&_pBuf[outIndex], &data, sizeof(T));

    CNetIndexList::_index_node *pos = _listData.m_Head.m_pNext;
    T *first = &_pBuf[pos->m_dwIndex];
    if (CNetIndexList::size(&_listData))
    {
      if (!CNetIndexList::size(&_listData) || !first->operator>(&data))
      {
        while (pos != &_listData.m_Tail)
        {
          pos = pos->m_pNext;
          if (pos == &_listData.m_Tail)
          {
            break;
          }
          T *cur = &_pBuf[pos->m_dwIndex];
          if (cur->operator>(&data))
          {
            if (!_listData.Push(pos, outIndex))
            {
              CNetIndexList::PushNode_Back(&_listEmpty, outIndex);
              return false;
            }
            return true;
          }
        }
        if (CNetIndexList::PushNode_Back(&_listData, outIndex))
        {
          return true;
        }
        CNetIndexList::PushNode_Back(&_listEmpty, outIndex);
        return false;
      }
      if (!CNetIndexList::PushNode_Front(&_listData, outIndex))
      {
        CNetIndexList::PushNode_Back(&_listEmpty, outIndex);
        return false;
      }
    }
    else
    {
      if (!CNetIndexList::PushNode_Front(&_listData, outIndex))
      {
        CNetIndexList::PushNode_Back(&_listEmpty, outIndex);
        return false;
      }
    }
    return true;
  }

  bool pop()
  {
    unsigned int outIndex = static_cast<unsigned int>(-1);
    return CNetIndexList::PopNode_Front(&_listData, &outIndex)
        && CNetIndexList::PushNode_Back(&_listEmpty, outIndex);
  }

  bool pop(const T &data)
  {
    CNetIndexList::_index_node *node = _listData.m_Head.m_pNext;
    while (node != &_listData.m_Tail)
    {
      T *cur = &_pBuf[node->m_dwIndex];
      if (cur->operator==(&data))
      {
        node->m_pPrev->m_pNext = node->m_pNext;
        node->m_pNext->m_pPrev = node->m_pPrev;
        --_listData.m_dwCount;

        CNetIndexList::_index_node *bufTail = &_listData.m_BufTail;
        node->m_pNext = bufTail;
        node->m_pPrev = bufTail->m_pPrev;
        bufTail->m_pPrev->m_pNext = node;
        bufTail->m_pPrev = node;
        ++_listData.m_dwBufCount;

        CNetIndexList::PushNode_Back(&_listEmpty, node->m_dwIndex);
        return true;
      }
      node = node->m_pNext;
    }
    return false;
  }

  T *pop(unsigned int index)
  {
    unsigned int curIndex = 0;
    CNetIndexList::_index_node *node = _listData.m_Head.m_pNext;
    while (node != &_listData.m_Tail)
    {
      if (curIndex == index)
      {
        return &_pBuf[node->m_dwIndex];
      }
      ++curIndex;
      node = node->m_pNext;
    }
    return nullptr;
  }

  T *top()
  {
    if (_listData.m_Head.m_pNext == &_listData.m_Tail)
    {
      return nullptr;
    }
    return &_pBuf[_listData.m_Head.m_pNext->m_dwIndex];
  }

  unsigned __int64 size() const
  {
    return _listData.m_dwCount;
  }

  bool empty() const
  {
    return _listData.m_dwCount == 0;
  }

  T *_pBuf = nullptr;
  CIndexListEx _listEmpty;
  CIndexListEx _listData;
  unsigned __int64 _nMaxSize = 0;
};
