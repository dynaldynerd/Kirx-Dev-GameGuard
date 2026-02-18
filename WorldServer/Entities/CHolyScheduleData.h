#pragma once

#include "IdaCompat.h"

class  CHolyScheduleData
{
public:
  struct __HolyScheduleNode
  {
    int m_nSceneTime[7];
  };

  CHolyScheduleData();
  virtual ~CHolyScheduleData();

  void Init();
  __HolyScheduleNode *GetIndex(int nNumOfTime);
  unsigned int GetTotalSceduleTerm(int nNumOfTime);

  bool m_bSet;
  __HolyScheduleNode *m_pSchedule;
  int m_nTotalSchedule;
};
