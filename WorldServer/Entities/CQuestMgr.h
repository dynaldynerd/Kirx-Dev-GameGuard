#pragma once

#include "IdaCompat.h"
#include "quest_check_result.h"
#include "Quest_fld.h"

class CRecordData;
/* 1755 */
class __cppobj __declspec(align(8)) CQuestMgr
{
public:
  static bool LoadQuestData();
  static bool CheckRewardMasteryData(int iInx, int iNth, _quest_reward_mastery *pData);
  static bool CalcStartNPCQuestCnt(unsigned int *pdwCnt);
  static CRecordData *s_tblQuest;
  static CRecordData *s_tblQuestHappenEvent;

  _quest_check_result *CheckReqAct(int nActCode, char *pszReqCode, unsigned __int16 wActCount, bool bPartyState);
  char CheckFailCondition(unsigned __int8 byQuestDBSlot, int nFailCond, char *pszCode);
  char DeleteQuestItem(char *pszItemCode, unsigned __int16 wCount);
  char __CheckCond_Have(int nAmonut, char *pszItemCode);
  void DeleteQuestData(unsigned __int8 bySlot);
  char IsCompleteNpcQuest(char *pszCode, int bQuestRepeat);
  char IsProcNpcQuest(char *pszCode);
  __int64 GetCountQuestType(int nType);
  _Quest_fld *GetQuestFromEvent(unsigned __int8 bySelect);
  unsigned __int8 InsertNpcQuestHistory(char *pszQuestCode, char byLevel, long double dRepeatTime);

  CPlayer *m_pMaster;
  _QUEST_DB_BASE *m_pQuestData;
  _happen_event_cont m_LastHappenEvent;
  _happen_event_cont *m_pTempHappenEvent;
  unsigned int m_dwOldTimeoutChecktime;
};

