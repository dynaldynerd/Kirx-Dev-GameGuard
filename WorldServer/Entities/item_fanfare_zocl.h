#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct _item_fanfare_zocl{ struct _SubType { unsigned __int8 bitParmaType : 2; unsigned __int8 bitGrade : 6; }; unsigned __int8 byTakeType; _SubType bySubType; unsigned __int8 byTableCode; unsigned __int16 wItemIndex; unsigned __int16 wMonsterIndex; unsigned __int16 wMapIndex; unsigned int dwCharacterSerial; char strCharacterName[17]; bool bAllSend; _item_fanfare_zocl(); void Init();};
#pragma pack(pop)
