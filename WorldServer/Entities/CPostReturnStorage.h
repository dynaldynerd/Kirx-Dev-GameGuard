#pragma once

#include "IdaCompat.h"
#include "CPostData.h"

/* 1704 */
class  CPostReturnStorage
{
public:
  CPostReturnStorage();
  ~CPostReturnStorage();
  void Init();
  int GetSize();
  CPostData *GetPostDataFromInx(unsigned int nIndex);
  CPostData *AddReturnPost(
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
    unsigned __int64 lnUID);
  CPostData *GetPostDataFromSerial(unsigned int dwPostSerial);
  void DelPostData(unsigned int dwPostSerial);
  unsigned int GetReturnPostInx();

  CPostData m_PostData[10];
  int m_nSize;
};


