#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

#include <cstring>

struct _qry_case_post_send
{
  struct __list
  {
    unsigned __int8 byErr;
    unsigned int dwIndex;
    unsigned int dwPSSerial;
    unsigned int dwReceiverSerial;
    unsigned int dwSenderSerial;
    char wszSendName[17];
    char wszRecvName[17];
    char wszTitle[21];
    char wszContent[201];
    _INVENKEY key;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned __int64 lnUID;
    unsigned int dwGold;

    __list() = default;
  };

  unsigned int dwCount;
  __list List[15];

  _qry_case_post_send();
  __int64 size() const;
  char pushdata(
    unsigned int dwIndex,
    unsigned __int8 byErr,
    unsigned int dwReceiverSerial,
    unsigned int dwSenderSerial,
    char *wszSendName,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    _INVENKEY key,
    unsigned __int64 dwDur,
    unsigned int dwUpt,
    unsigned int dwGold,
    unsigned __int64 lnUID);
};

inline _qry_case_post_send::_qry_case_post_send()
{
  std::memset(this, 0, sizeof(*this));
}

inline __int64 _qry_case_post_send::size() const
{
  return 4688LL;
}

inline char _qry_case_post_send::pushdata(
  unsigned int dwIndex,
  unsigned __int8 byErr,
  unsigned int dwReceiverSerial,
  unsigned int dwSenderSerial,
  char *wszSendName,
  char *wszRecvName,
  char *wszTitle,
  char *wszContent,
  _INVENKEY key,
  unsigned __int64 dwDur,
  unsigned int dwUpt,
  unsigned int dwGold,
  unsigned __int64 lnUID)
{
  if (dwCount >= 15)
  {
    return 0;
  }

  List[dwCount].dwIndex = dwIndex;
  List[dwCount].byErr = byErr;
  List[dwCount].dwReceiverSerial = dwReceiverSerial;
  List[dwCount].dwSenderSerial = dwSenderSerial;
  List[dwCount].key = key;
  List[dwCount].dwDur = dwDur;
  List[dwCount].dwUpt = dwUpt;
  List[dwCount].dwGold = dwGold;
  List[dwCount].lnUID = lnUID;
  strcpy_s(List[dwCount].wszSendName, 17, wszSendName);
  strcpy_s(List[dwCount].wszRecvName, 17, wszRecvName);
  strcpy_s(List[dwCount].wszTitle, 21, wszTitle);
  strcpy_s(List[dwCount].wszContent, 201, wszContent);
  ++dwCount;
  return 1;
}

