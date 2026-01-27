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
};

struct __cppobj __declspec(align(4)) _QUEST_CASH_OTHER
{
  unsigned int dwAvatorSerial;
  unsigned __int8 byStoneMapMoveInfo;
};

class __cppobj __declspec(align(8)) CHolyStoneSystem
{
public:
  static bool InitHolySystem(CHolyStoneSystem *pSystem);
  bool ContinueStartSystem();
  void InitQuestCash_Other();

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
