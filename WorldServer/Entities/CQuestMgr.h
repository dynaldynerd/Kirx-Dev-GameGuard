#pragma once

#include "IdaCompat.h"
#include "Quest_fld.h"
#include "QuestHappenEvent_fld.h"
#include "QuestHappenTypes.h"
#include "quest_check_result.h"
#include "quest_fail_result.h"

class CRecordData;
class CPlayer;
struct _NPCQuestIndexTempData;
struct _QUEST_DB_BASE;
/* 1755 */
class  CQuestMgr
{
public:
  CQuestMgr();
  ~CQuestMgr();
  static bool LoadQuestData();
  static bool CheckRewardMasteryData(int iInx, int iNth, _quest_reward_mastery *pData);
  static bool CalcStartNPCQuestCnt(unsigned int *pdwCnt);
  static CRecordData *s_tblQuest;
  static CRecordData *s_tblQuestHappenEvent;
  void InitMgr(CPlayer *pMaster, _QUEST_DB_BASE *pQuestData);
  void Loop();
  void CheckFailLoop(int nFailCond, char *pszCode);

  _quest_check_result *CheckReqAct(int nActCode, char *pszReqCode, unsigned __int16 wActCount, bool bPartyState);
  _happen_event_cont *GetLastHappenEvent();
  _happen_event_cont *CheckQuestHappenEvent(QUEST_HAPPEN HappenType, char *pszEventCode, unsigned __int8 byRaceCode);
  char IsRecvedQuestByNPC(int nEventIndex);
  char IsRecvedQuestByNPC(const char *pszNPCCode);
  void CheckNPCQuestList(char *pszEventCode, unsigned __int8 byRaceCode, _NPCQuestIndexTempData *pQuestIndexData);
  _happen_event_cont *CheckNPCQuestStartable(
    char *pszEventCode,
    unsigned __int8 byRaceCode,
    int dwQuestIndex,
    unsigned int dwHappenIndex);
  _Quest_fld *GetQuestFromEvent(unsigned __int8 bySelect);
  __int64 GetCountQuestType(int nType);
  bool _CheckCondition(_happen_event_condition_node *pCond);
  char __CheckCond_LV(unsigned __int8 byCompare, int nLv);
  char __CheckCond_Grade(unsigned __int8 byCompare, int nGrade);
  char __CheckCond_Dalant(unsigned __int8 byCompare, unsigned int nDalant);
  char __CheckCond_Gold(unsigned __int8 byCompare, unsigned int nGold);
  bool __CheckCond_Party(int belong);
  bool __CheckCond_Guild(int belong);
  bool __CheckCond_Nation(int belong);
  bool __CheckCond_Equip(char *pszItemCode);
  char __CheckCond_Have(int nAmount, char *pszItemCode);
  bool __CheckCond_Mastery(unsigned __int8 byCompare, char *pszMasteryPack);
  char __CheckCond_Dummy(int bInclude, char *pszDummyCode);
  bool __CheckCond_Race(char *pszRaceSexFlag);
  char __CheckCond_Class(char *pszClassCode);
  char DeleteQuestItem(char *pszItemCode, unsigned __int16 wCount);
  char ReturnItem(char *pszItemCode, int nEndReturnItemCnt, unsigned __int8 byQuestDBSlot);
  bool CanGiveupQuest(unsigned __int8 byQuestDBSlot);
  char CheckFailCondition(unsigned __int8 byQuestDBSlot, int nFailCond, char *pszCode);
  void SendMsgToMaster_ReturnItemAfterQuest(unsigned __int16 wItemSerial, unsigned __int8 byNum, char byQuestDBSlot);
  void SendMsgToMaster_NoCompleteQuestFromNPC(char byQuestDBSlot);
  void SendMsgToMaster_NoHaveGiveItem(char byQuestDBSlot);
  void SendMsgToMaster_NoHaveReturnItem(char byQuestDBSlot);
  void DeleteQuestData(unsigned __int8 bySlot);
  unsigned __int8 InsertNpcQuestHistory(char *pszQuestCode, char byLevel, long double dRepeatTime);
  _quest_fail_result *CheckLimLv(int nNewLv);
  char IsCompleteNpcQuest(char *pszCode, int bQuestRepeat);
  char IsProcNpcQuest(char *pszCode);
  char IsProcLinkNpcQuest(char *pszCode, int nLinkQuestGroupID);
  bool IsPossibleRepeatNpcQuest(char *pszCode, int nLinkQuestGroupID);

  CPlayer *m_pMaster;
  _QUEST_DB_BASE *m_pQuestData;
  _happen_event_cont m_LastHappenEvent;
  _happen_event_cont *m_pTempHappenEvent;
  unsigned int m_dwOldTimeoutChecktime;
};


