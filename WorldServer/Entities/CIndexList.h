#pragma once

#include "IdaCompat.h"
#include "CSyncCS.h"

/* 1689 */
class __cppobj __declspec(align(8)) CIndexList
{
public:
  struct _index_node
  {
    bool m_bLoad;
    unsigned int m_dwIndex;
    unsigned int m_dwInfoDataSize;
    char *m_pInfo;
    _index_node *m_pPrev;
    _index_node *m_pNext;

    bool AllocInfo(unsigned int infoSize);
  };

  _index_node m_Head;
  _index_node m_Tail;
  _index_node m_BufHead;
  _index_node m_BufTail;
  _index_node *m_pBufNode;
  CSyncCS m_csList;
  unsigned int m_dwCount;
  unsigned int m_dwBufCount;
  unsigned int m_dwMaxBufNum;

  static bool SetList(CIndexList *list, unsigned int maxBufNum, unsigned int infoSize, bool useMultiThread);
  static bool PushNode_Back(CIndexList *list, unsigned int index, char *infoData);
  static bool PushNode_Front(CIndexList *list, unsigned int index, char *infoData);
  virtual ~CIndexList() = default;
};

