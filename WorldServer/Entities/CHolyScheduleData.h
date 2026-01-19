#pragma once

#include "IdaCompat.h"

class __cppobj CHolyScheduleData
{
public:
  struct __HolyScheduleNode
  {
    int m_nSceneTime[7];
  };

  virtual ~CHolyScheduleData() = default;

  bool m_bSet;
  __HolyScheduleNode *m_pSchedule;
  int m_nTotalSchedule;
};
