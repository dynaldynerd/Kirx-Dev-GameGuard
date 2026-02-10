#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

#pragma pack(push, 1)
struct __cppobj _quest_download_result_zocl
{
  struct __cppobj __list
  {
    unsigned __int8 byDBSlotIndex;
    _QUEST_DB_BASE::_LIST cont;

    __list();
  };

  unsigned __int8 byRetCode;
  unsigned __int8 bySlotNum;
  __list SlotInfo[30];

  _quest_download_result_zocl();
  unsigned __int64 size();
};
#pragma pack(pop)
