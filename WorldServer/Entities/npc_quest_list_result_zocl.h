#pragma once

#include "IdaCompat.h"

#pragma pack(push, 1)
struct __unaligned __declspec(align(1)) _npc_quest_list_result_zocl
{
  unsigned __int8 byQuestNum;
  unsigned int QuestIndexList[30];

  _npc_quest_list_result_zocl()
  {
    memset_0(this, 0, sizeof(_npc_quest_list_result_zocl));
  }
};
#pragma pack(pop)
