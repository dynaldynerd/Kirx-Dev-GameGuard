#pragma once
#include "IdaCompat.h"
#pragma pack(push, 1)
struct  _quest_history_download_result_zocl{ struct __list { unsigned __int8 byIndex; char szQuestCode[8]; }; unsigned __int8 bySlotNum; __list SlotInfo[70]; _quest_history_download_result_zocl(); unsigned __int64 size();};
#pragma pack(pop)
