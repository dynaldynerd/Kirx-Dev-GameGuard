#pragma once

#include "IdaCompat.h"

struct _quest_check_result
{
  struct __declspec(align(2)) _node
  {
    unsigned __int8 byQuestDBSlot;
    unsigned __int8 byActIndex;
    unsigned __int16 wCount;
    bool bORComplete;
  };

  unsigned __int8 m_byCheckNum;
  _node m_List[30];

  void init();
};
