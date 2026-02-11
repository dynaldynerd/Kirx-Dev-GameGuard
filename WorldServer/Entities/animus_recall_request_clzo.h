#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _animus_recall_request_clzo
{
  unsigned int dwAnimusItemSerial;
  unsigned __int16 wAnimusClientHP;
  unsigned __int16 wAnimusClientFP;
};
#pragma pack(pop)
