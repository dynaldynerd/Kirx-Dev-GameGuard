#pragma once

#include "../IdaCompat.h"

// Zone -> Billing packet structs.

// billing_start_request_zobi.h

#pragma pack(push, 1)
struct _billing_start_request_zobi
{
  char szWorldName[33];
};
#pragma pack(pop)

static_assert(sizeof(_billing_start_request_zobi) == 33, "billing start packet size mismatch");

// billing_logout_request_zobi.h

#pragma pack(push, 1)
struct _billing_logout_request_zobi
{
  char szID[13];
  char szIP[16];
  char szCMS[7];
  __int16 iType;
};
#pragma pack(pop)

static_assert(sizeof(_billing_logout_request_zobi) == 38, "billing logout packet size mismatch");

// billing_alive_request_zobi.h

#pragma pack(push, 1)
struct _billing_alive_request_zobi
{
  char szID[13];
  char szIP[16];
  char szCMS[7];
  __int16 iType;
  bool bIsPcBang;
};
#pragma pack(pop)

static_assert(sizeof(_billing_alive_request_zobi) == 39, "billing alive packet size mismatch");

// billing_login_request_zobi.h

#pragma pack(push, 1)
struct _billing_login_request_zobi
{
  char szID[13];
  char szIP[16];
  char szCMS[7];
  __int16 iType;
  int lRemainTime;
  _SYSTEMTIME stEndDate;
};
#pragma pack(pop)

static_assert(sizeof(_billing_login_request_zobi) == 58, "billing login packet size mismatch");
