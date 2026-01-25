#pragma once

#include "IdaCompat.h"

class CRecordData;

/* 1755 */
class __cppobj __declspec(align(8)) CQuestMgr
{
public:
  static bool LoadQuestData();
  static CRecordData *s_tblQuest;
  static CRecordData *s_tblQuestHappenEvent;

  CPlayer *m_pMaster;
  _QUEST_DB_BASE *m_pQuestData;
  _happen_event_cont m_LastHappenEvent;
  _happen_event_cont *m_pTempHappenEvent;
  unsigned int m_dwOldTimeoutChecktime;
};

