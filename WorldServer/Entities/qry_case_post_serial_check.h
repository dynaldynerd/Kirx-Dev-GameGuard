#pragma once

#include "IdaCompat.h"

#include <cstring>

#pragma pack(push, 4)
struct _qry_case_post_serial_check
{
  struct __list
  {
    unsigned int dwIndex;
    unsigned __int8 bySenderDgr;
    unsigned __int8 bySenderRace;
    bool bCheckDgr;
    char wszRecvName[17];
    unsigned int dwReceiverSerial;
    unsigned __int8 byErr;

    __list() = default;
  };

  unsigned int dwCount;
  __list List[15];

  _qry_case_post_serial_check()
  {
    std::memset(this, 0, sizeof(*this));
  }

  int size() const
  {
    return 484LL;
  }
};
#pragma pack(pop)

static_assert(sizeof(_qry_case_post_serial_check::__list) == 32);
static_assert(sizeof(_qry_case_post_serial_check) == 484);
