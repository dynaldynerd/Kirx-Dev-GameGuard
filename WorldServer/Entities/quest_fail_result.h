#pragma once

#include "IdaCompat.h"

struct _quest_fail_result
{
  struct __declspec(align(1)) _node
  {
    unsigned __int8 byQuestDBSlot;
  };

  unsigned __int8 m_byCheckNum;
  _node m_List[30];

  void init();
};
