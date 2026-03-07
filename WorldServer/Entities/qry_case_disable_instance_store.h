#pragma once

#include "IdaCompat.h"

struct _qry_case_disable_instance_store_entry
{
  unsigned __int8 byResult;
  unsigned __int8 byPadding[3];
  unsigned int dwDBSerial;
};

static_assert(sizeof(_qry_case_disable_instance_store_entry) == 8, "disable instance store entry size mismatch");

struct _qry_case_disable_instance_store
{
  unsigned int dwCount;
  unsigned int dwReserved;
  _qry_case_disable_instance_store_entry *pEntry;
};

static_assert(sizeof(_qry_case_disable_instance_store) == 16, "disable instance store query size mismatch");
