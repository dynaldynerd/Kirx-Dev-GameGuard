#pragma once

#include "IdaCompat.h"
#include "QuestHappenEvent_fld.h"

enum QUEST_HAPPEN : __int32
{
  quest_happen_type_dummy = 0,
  quest_happen_type_npc = 1,
  quest_happen_type_pk = 2,
  quest_happen_type_lv = 3,
  quest_happen_type_class = 4,
  quest_happen_type_grade = 5,
  quest_happen_type_item = 6,
  quest_happen_type_mastery = 7,
  quest_happen_type_maxlevel = 8,
  QUEST_HAPPEN_TYPE_NUM = 9,
};

struct _happen_event_cont
{
  _happen_event_node *m_pEvent;
  QUEST_HAPPEN m_QtHpType;
  int m_nIndexInType;
  int m_nRaceCode;

  _happen_event_cont();
  void init();
  bool isset() const;
  void set(_happen_event_node *pPoint, QUEST_HAPPEN QtHpType, int nIndexInType, int nRaceCode);
};

