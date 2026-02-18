#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _happen_event_condition_node
{
  int m_nCondType;
  int m_nCondSubType;
  char m_sCondVal[64];
};

struct _happen_event_node
{
  int m_bUse;
  int m_bQuestRepeat;
  int m_nQuestType;
  int m_bSelectQuestManual;
  int m_nAcepProNum;
  int m_nAcepProDen;
  _happen_event_condition_node m_CondNode[5];
  char m_strLinkQuest[5][64];
};

struct  _QuestHappenEvent_fld : _base_fld
{
  int m_nEevntNo;
  _happen_event_node m_Node[3];
};
#pragma pack(pop)
