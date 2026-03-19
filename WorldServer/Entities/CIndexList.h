#pragma once

#include "IdaCompat.h"
#include "CSyncCS.h"

/* 1689 */
class  CIndexList
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

    _index_node();
    ~_index_node();
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

  CIndexList();
  bool SetList(unsigned int maxBufNum, unsigned int infoSize, bool useMultiThread);
  bool PushNode_Back(unsigned int index, char *infoData);
  bool PushNode_Front(unsigned int index, char *infoData);
  bool PopNode_Front(unsigned int *pdwOutIndex, char *pInfoData);
  bool FindNode(unsigned int dwIndex, char *pInfoData);
  bool IsInList(unsigned int dwIndex, char *pInfoData);
  bool IsSetting();
  bool CopyFront(unsigned int *pdwOutIndex, char *pInfoData);
  _index_node *GetAllNode(unsigned int *pdwMaxNodeNum);
  unsigned int GetSize();
  void ResetList();
  virtual ~CIndexList();
};


