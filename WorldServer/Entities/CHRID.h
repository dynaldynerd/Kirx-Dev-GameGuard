#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct _CLID
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};

struct _WA_AVATOR_CODE
{
  _CLID m_id;
  char m_wszName[17];
};

struct _CHRID
{
  char byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};
#pragma pack(pop)
