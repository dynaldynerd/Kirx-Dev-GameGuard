#pragma once

#include "IdaCompat.h"
#include "qry_case_make_storage.h"

#pragma pack(push, 1)
struct _qry_case_make_storage_update_header
{
  _qry_case_make_storage base;
  unsigned __int8 byChangeCount;
  unsigned __int8 byReserved[3];
};

struct _qry_case_make_storage_update_entry
{
  unsigned __int8 bySlot;
  unsigned __int8 byReserved0[3];
  unsigned int dwItemKey;
  unsigned __int8 byAmount;
  unsigned __int8 byReserved1[3];
};
#pragma pack(pop)

static_assert(sizeof(_qry_case_make_storage_update_header) == 16);
static_assert(sizeof(_qry_case_make_storage_update_entry) == 12);
