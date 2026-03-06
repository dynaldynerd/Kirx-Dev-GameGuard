#pragma once

#include "../IdaCompat.h"

// Billing -> Zone packet structs.

// billing_close_request_bizo.h

#pragma pack(push, 1)
struct _billing_close_request_bizo
{
  char szID[13];
};
#pragma pack(pop)

static_assert(sizeof(_billing_close_request_bizo) == 13, "billing close packet size mismatch");

// expire_personal_request_bizo.h

#pragma pack(push, 1)
struct _expire_personal_request_bizo
{
  char szAccount[13];
};
#pragma pack(pop)

static_assert(sizeof(_expire_personal_request_bizo) == 13, "billing expire personal packet size mismatch");

// expire_pcbang_request_bizo.h

#pragma pack(push, 1)
struct _expire_pcbang_request_bizo
{
  char szCMSCode[7];
};
#pragma pack(pop)

static_assert(sizeof(_expire_pcbang_request_bizo) == 7, "billing expire pcbang packet size mismatch");

// expire_ipoverflow_request_bizo.h

#pragma pack(push, 1)
struct _expire_ipoverflow_request_bizo
{
  char szAccount[13];
};
#pragma pack(pop)

static_assert(sizeof(_expire_ipoverflow_request_bizo) == 13, "billing expire ipoverflow packet size mismatch");

// remaintime_personal_request_bizo.h

#pragma pack(push, 1)
struct _remaintime_personal_request_bizo
{
  char szAccount[13];
  __int16 iType;
  int lRemainTime;
  _SYSTEMTIME stEndDate;
};
#pragma pack(pop)

static_assert(sizeof(_remaintime_personal_request_bizo) == 35, "billing personal remain packet size mismatch");

// remaintime_pcbang_request_bizo.h

#pragma pack(push, 1)
struct _remaintime_pcbang_request_bizo
{
  char szCMSCode[7];
  __int16 iType;
  int lRemainTime;
  _SYSTEMTIME stEndDate;
};
#pragma pack(pop)

static_assert(sizeof(_remaintime_pcbang_request_bizo) == 29, "billing pcbang remain packet size mismatch");

// change_type_request_bizo.h

#pragma pack(push, 1)
struct _change_type_request_bizo
{
  char szAccount[13];
  char szCMSCode[7];
  __int16 iType;
  int lRemainTime;
  _SYSTEMTIME stEndDate;
  char byReason;
};
#pragma pack(pop)

static_assert(sizeof(_change_type_request_bizo) == 43, "billing change type packet size mismatch");

// taiwan_billing_user_certify_request_bizo.h

#pragma pack(push, 1)
struct _taiwan_billing_user_certify_request_bizo
{
  char szAccount[13];
  char byCertify;
};
#pragma pack(pop)

static_assert(sizeof(_taiwan_billing_user_certify_request_bizo) == 14, "taiwan billing certify packet size mismatch");

// china_billing_change_primium_request_bizo.h

#pragma pack(push, 1)
struct _china_billing_change_primium_request_bizo
{
  char szID[13];
  char byChangeType;
};
#pragma pack(pop)

static_assert(sizeof(_china_billing_change_primium_request_bizo) == 14, "china billing primium packet size mismatch");

// zone_alive_check_request_bizo.h

#pragma pack(push, 1)
struct _zone_alive_check_request_bizo
{
  unsigned int dwTick;
};
#pragma pack(pop)

static_assert(sizeof(_zone_alive_check_request_bizo) == 4, "zone alive check packet size mismatch");
