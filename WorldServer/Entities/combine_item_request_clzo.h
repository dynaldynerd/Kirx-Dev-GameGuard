#pragma once
#include "IdaCompat.h"
#include "StoragePos.h"
#pragma pack(push, 1)
struct  _combine_item_request_clzo{ unsigned __int16 wManualIndex; unsigned __int16 wOverlapItemSerial; unsigned __int8 byMaterialNum; _STORAGE_POS_INDIV ipMaterials[100];};
#pragma pack(pop)
