#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "CLogFile.h"
#include "CRecordData.h"
#include "CHolyScheduleData.h"
#include "CHolyStoneSaveData.h"

class CMapData;
class CPlayer;
struct _monster_fld;
struct _portal_dummy;

struct __cppobj __holy_keeper_data
{
  CMapData *pCreateMap;
  _dummy_position CreateDummy;
  _dummy_position ActiveDummy;
  _dummy_position CenterDummy;
  _monster_fld *pRec;
};

struct __cppobj __declspec(align(8)) __holy_stone_data
{
  CMapData *pCreateMap;
  _dummy_position CreateDummy;
  _monster_fld *pRec;
  int nRace;
};

struct __cppobj __declspec(align(4)) _QUEST_CASH
{
  unsigned int dwAvatorSerial;
  unsigned __int8 byQuestType;
  int nPvpPoint;
  unsigned __int16 wKillPoint;
  unsigned __int16 wDiePoint;
  unsigned __int8 byCristalBattleDBInfo;
  unsigned __int8 byHSKTime;

  bool isload();
  void init()
  {
    dwAvatorSerial = static_cast<unsigned int>(-1);
  }
};

struct __cppobj __declspec(align(4)) _QUEST_CASH_OTHER
{
  unsigned int dwAvatorSerial;
  unsigned __int8 byStoneMapMoveInfo;

  bool isLoaded();
  void init()
  {
    dwAvatorSerial = 0;
  }
};

class __cppobj __declspec(align(8)) CHolyStoneSystem
{
public:
  bool InitHolySystem();
  void OnLoop();
  void AlterSchedule(unsigned __int8 byScheduleCode, unsigned __int8 byNumOfTime);
  bool ContinueStartSystem();
  void InitQuestCash_Other();
  void InitQuestCash();
  bool SetScene(unsigned __int8 byNumOfTime, int nSceneCode, unsigned int nPassTime, int nChangeReason);
  void SetTermTimeDefault(unsigned __int8 byNumOfTime);
  void CheckKeeperPlusTime();
  unsigned __int8 GetNumOfTime();
  int GetDestroyerState();
  unsigned int GetDestroyerSerial();
  int GetHolyMasterRace();
  int GetSceneCode();
  bool IsControlScene();
  bool IsMinigeTicketCheck();
  int GetDestroyStoneRace();
  unsigned int GetStartBattleTickTime();
  unsigned __int16 GetStartYear();
  unsigned __int8 GetStartMonth();
  unsigned __int8 GetStartDay();
  unsigned __int8 GetStartHour();
  unsigned __int8 GetStartMin();
  unsigned __int8 GetKeeperDestroyRace();
  unsigned int GetDestroyerGuildSerial();
  void SetHolyMasterRace(int nMaster);
  void SetDestroyStoneRace(int nRace);
  char IsItemLootAuthority(CPlayer *pOne, unsigned __int8 byCreateCode);
  void ReleaseLastAttBuff();
  void CreateHolyStone();
  void DestroyHolyStone();
  void CreateHolyKeeper(int nCreateType);
  void DestroyHolyKeeper();
  bool IsUseReturnItem(unsigned int dwObjSerial);
  bool GetGoldBoxConsumable();
  void SetGoldBoxConsumable(bool bFlag);
  void SetKeeperDestroyRace(unsigned __int8 byRace);
  void UnAllRegisterPerAutoMine();
  void SendIsArriveDestroyer(char byArrive);
  void SendMsg_to_webagent_about_last_attacker_for_keeper(CPlayer *pPlayer, int bByAnimus);
  void SendMsg_HolyStoneSystemState(int nPlayerIndex);
  void SendMsg_WaitStone(unsigned int n);
  void SendMsg_NoticeNextQuest(unsigned int n, unsigned __int8 byStoneMapMoveInfo);
  void SendMsg_NotifyHolyKeeperAttackTimeBeKeepKeeper(char bKeepKeeper);
  void SendMsg_EnterStone(unsigned int n);
  void SendMsg_ExitStone();
  void SendMsg_StartBattle();
  void SendMsg_EndBattle(char byLoseRace);
  void SendMsg_EnterKeeper(unsigned int n);
  void SendMsg_WaitKeeper(unsigned int n, char byWaitType);
  void SendMsg_HolyKeeperStateChaos();
  void SendMsg_HolyKeeperAttackAbleState(bool bAttackAble);
  void SendMsg_CreateHolyMaster(CPlayer *pkDestroyer, int nControlSec);
  void SendSMS_MineTimeExtend(int nControlSec);
  void SendSMS_CompleteQuest(
    char byDestroyedRace,
    char *pwszMasterName,
    int nControlSec,
    char *szMasterClass,
    unsigned __int8 byMasterLv);
  void GiveHSKQuest();
  void SendHolyStoneHPToRaceBoss();
  void SendHolyStoneHP(CPlayer *pkPlayer);
  void SendNotifyHolyStoneDestroyedToRaceBoss();
  void CheckDestroyerIsArriveMine();
  void HSKRespawnSystem();
  void UpdateNotifyHolyStoneHPToRaceBoss();
  _QUEST_CASH *FindStoragedQuestCash(unsigned int dwAvatorSerial);
  _QUEST_CASH_OTHER *PopStoredQuestCash_Other(unsigned int dwAvatorSerial);
  bool IsMentalPass();
  void WriteLogPer10Min_Combat();
  void SetEffectToDestroyerGuildMember();
  void RecoverPvpCash();
  char CheckHolyMaster(CPlayer *pAtter, unsigned __int8 byDestroyStoneRaceCode);
  void PushStoreQuestCash(
    unsigned int dwAvatorSerial,
    unsigned __int8 byQuestType,
    int nPvpPoint,
    unsigned __int16 wKillPoint,
    unsigned __int16 wDiePoint,
    unsigned __int8 byCristalBattleDBInfo,
    unsigned __int8 byHSKTime);
  void PushQuestCash_Other(unsigned int dwAvatorSerial, unsigned __int8 byStoneMapMoveInfo);
  void PeneltyLoseRace(unsigned __int8 byDestroyedRace);
  void PeneltyFailRace(unsigned __int8 byFailRace);
  void On_HS_SCENE_BATTLE_TIME();
  void On_HS_SCENE_BATTLE_END_WAIT_TIME();
  void On_HS_SCENE_KEEPER_ATTACKABLE_TIME();
  void On_HS_SCENE_KEEPER_DEATTACKABLE_TIME();
  void On_HS_SCENE_KEEPER_DIE_TIME();
  void On_HS_SCENE_KEEPER_CHAOS_TIME();

  CRecordData m_tblQuest;
  CLogFile m_logQuest;
  CLogFile m_logQuestDestroy;
  CLogFile m_logPer10Min;
  CMyTimer m_tmrHSKSystem;
  CPlayer *m_pkDestroyer;
  unsigned int m_dwNextStartTime;
  int m_nHolyStoneNum;
  __holy_keeper_data m_HolyKeeperData;
  __holy_stone_data m_HolyStoneData[3];
  unsigned int m_dwCheckTime[7];
  CHolyScheduleData m_ScheculeData;
  CHolyStoneSaveData m_SaveData;
  CMyTimer m_tmrCumPlayer;
  char m_strHolyMental[64];
  _QUEST_CASH m_cashQuest[5064];
  float m_fKeeperHPRate;
  float m_fFirstKeeperHPRate;
  int m_bScheduleCodePre;
  _QUEST_CASH_OTHER m_cashQuestOther[5064];
  _portal_dummy *m_pPortalDummy[3];
  int m_nRaceBattlePoint[3][2];
  unsigned __int8 m_byKeeperDestroyRace;
  bool m_bConsumable;
  bool m_pMentalPass;
  bool bFreeMining;
};
