#pragma once

#include "IdaCompat.h"
#include "StoragePos.h"

#pragma pack(push, 1)
struct __cppobj __unaligned __declspec(align(1)) _make_item_request_clzo
{
  _STORAGE_POS_INDIV ipMakeTool;
  unsigned __int16 wManualIndex;
  unsigned __int8 byMaterialNum;
  _STORAGE_POS_INDIV ipMaterials[100];
};
#pragma pack(pop)
