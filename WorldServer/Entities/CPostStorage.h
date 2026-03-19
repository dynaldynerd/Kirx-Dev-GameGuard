#pragma once

#include "IdaCompat.h"
#include "CPostData.h"

/* 1703 */
class  CPostStorage
{
public:
  CPostStorage();
  ~CPostStorage();
  void Init();
  int GetSize();
  CPostData *GetPostDataFromInx(unsigned int nIndex);
  int AddPostTitleData(
    unsigned int nIndex,
    unsigned int dwSerial,
    unsigned __int8 byState,
    char *wszSendName,
    char *wszTitle,
    _INVENKEY Key,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    unsigned __int64 lnUID);
  bool AddPostTitleDataByStorageIndex(
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
    unsigned __int64 lnUID);
  int AddNewPost(
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
    unsigned __int64 lnUID);
  CPostData *GetPostDataFromSerial(unsigned int dwPostSerial);
  void DelPostData(unsigned int dwIndex);
  bool IsContentLoad(unsigned int dwIndex);
  int SetPostContent(unsigned int dwSerial, char *wszContent);

  CPostData m_PostData[50];
  int m_nSize;
  bool m_bUpdate;
};


