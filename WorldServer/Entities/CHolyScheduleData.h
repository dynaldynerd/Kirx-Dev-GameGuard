#pragma once

#include "IdaCompat.h"

class __cppobj __declspec(align(8)) CHolyScheduleData
{
public:
  struct __HolyScheduleNode
  {
    int m_nSceneTime[7];
  };

  bool m_bSet;
  __HolyScheduleNode *m_pSchedule;
  int m_nTotalSchedule;
};
