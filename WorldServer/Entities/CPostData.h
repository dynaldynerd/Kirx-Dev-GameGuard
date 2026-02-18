#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

/* 1215 */
class  __declspec(align(8)) CPostData
{
public:
  CPostData();
  void Init();
  unsigned __int8 GetState();
  void SetState(unsigned __int8 byState);
  void SetPostContent(char *wszContent);
  void SetPostItemSerial(unsigned __int64 lnUID);
  void SetPostData(
    int nNumber,
    unsigned int dwSenderSerial,
    char *wszSendName,
    char *wszRecvName,
    char *wszTitle,
    _INVENKEY Key,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    unsigned int dwPSSerial,
    unsigned __int8 bySendRace,
    unsigned __int8 bySenderDgr);
  void SetPostTitleData(
    int nNumber,
    unsigned int dwPSSerial,
    unsigned __int8 byState,
    char *wszSendName,
    char *wszTitle,
    _INVENKEY Key,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    bool bUpdateIndex);
  void SetReturnPostData(
    unsigned __int8 byErrCode,
    unsigned int dwPostSerial,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    _INVENKEY Key,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold);

  int m_nNumber;
  unsigned __int8 m_byState;
  unsigned __int8 m_byErrCode;
  unsigned __int8 m_bySendRace;
  unsigned __int8 m_bySenderDgr;
  unsigned int m_dwSenderSerial;
  char m_wszSendName[17];
  char m_wszRecvName[17];
  char m_wszTitle[21];
  char m_wszContent[201];
  _INVENKEY m_Key;
  unsigned __int64 m_dwDur;
  unsigned int m_dwUpt;
  unsigned __int64 m_lnUID;
  unsigned int m_dwGold;
  unsigned int m_dwPSSerial;
  bool m_bContentLoad;
  bool m_bUpdateIndex;
};

