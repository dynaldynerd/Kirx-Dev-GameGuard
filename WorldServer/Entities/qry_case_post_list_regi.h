#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

#include <cstring>

struct _qry_case_post_list_regi
{
  struct __declspec(align(8)) __list
  {
    unsigned int dwIndex;
    bool bRet;
    unsigned __int8 bySendRace;
    unsigned __int8 bySenderDgr;
    unsigned __int8 byState;
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
  __list List[20];

  _qry_case_post_list_regi()
  {
    std::memset(this, 0, sizeof(*this));
  }

  __int64 size() const
  {
    return 6088LL;
  }
};

static_assert(sizeof(_qry_case_post_list_regi::__list) == 304);
static_assert(sizeof(_qry_case_post_list_regi) == 6088);
