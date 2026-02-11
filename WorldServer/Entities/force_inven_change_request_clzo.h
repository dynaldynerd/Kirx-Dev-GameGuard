#pragma once

#include "IdaCompat.h"
#include "StoragePos.h"

#pragma pack(push, 1)
struct _force_inven_change_request_clzo
{
  _STORAGE_POS_INDIV Item;
  unsigned __int16 wReplaceSerial;
};
#pragma pack(pop)
