#pragma once

#include "IdaCompat.h"
#include "QuestHappenEvent_fld.h"

enum QUEST_HAPPEN : __int32
{
  quest_happen_type_dummy = 0x0,
  quest_happen_type_npc = 0x1,
  quest_happen_type_pk = 0x2,
  quest_happen_type_lv = 0x3,
  quest_happen_type_class = 0x4,
  quest_happen_type_grade = 0x5,
  quest_happen_type_item = 0x6,
  quest_happen_type_mastery = 0x7,
  quest_happen_type_maxlevel = 0x8,
  QUEST_HAPPEN_TYPE_NUM = 0x9,
};

struct __declspec(align(8)) _happen_event_cont
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
