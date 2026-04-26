#pragma once

#include <windows.h>

#pragma pack(push, 1)
struct _reged_char_request_zone
{
  char dummy[1];
};

struct _sel_char_request_zone
{
  char bySlotIndex;
};
#pragma pack(pop)

static_assert(sizeof(_reged_char_request_zone) == 1, "_reged_char_request_zone packing mismatch");
static_assert(sizeof(_sel_char_request_zone) == 1, "_sel_char_request_zone packing mismatch");
