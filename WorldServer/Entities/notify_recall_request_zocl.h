#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _notify_recall_request_zocl
{
  unsigned __int16 wRequestID;
  char wszPerformerName[17];
};
#pragma pack(pop)
