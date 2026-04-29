#pragma once

#include <windows.h>

#pragma pack(push, 1)
struct _reged_char_request_zone
{
  char dummy[1];
};

struct _add_char_request_zone
{
  unsigned __int8 bySlotIndex;
  char szCharName[17];
  unsigned __int8 byRaceSexCode;
  char szClassCode[5];
  unsigned int dwBaseShape;

  int size() const
  {
    return sizeof(*this);
  }
};

struct _del_char_request_zone
{
  unsigned __int8 bySlotIndex;

  int size() const
  {
    return sizeof(*this);
  }
};

struct _sel_char_request_zone
{
  char bySlotIndex;
};
#pragma pack(pop)

static_assert(sizeof(_reged_char_request_zone) == 1, "_reged_char_request_zone packing mismatch");
static_assert(sizeof(_add_char_request_zone) == 28, "_add_char_request_zone packing mismatch");
static_assert(sizeof(_del_char_request_zone) == 1, "_del_char_request_zone packing mismatch");
static_assert(sizeof(_sel_char_request_zone) == 1, "_sel_char_request_zone packing mismatch");
