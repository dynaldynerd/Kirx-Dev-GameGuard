#pragma once

#include "IdaCompat.h"

class strFILE;
class CDarkHoleDungeonQuestSetup;
struct _react_obj;
struct _react_area;

using DH_COMMAND_FN = bool (__fastcall *)(strFILE *, CDarkHoleDungeonQuestSetup *, char *);

struct QUEST_COMMAND
{
  const char *pszCommand;
  DH_COMMAND_FN fn;
};

struct MISSION_COMMAND
{
  const char *pszCommand;
  DH_COMMAND_FN fn;
};

struct JOB_COMMAND
{
  const char *pszCommand;
  DH_COMMAND_FN fn;
};

extern QUEST_COMMAND quest_command[];
extern MISSION_COMMAND mission_command[];
extern JOB_COMMAND job_command[];

bool (__fastcall *SearchQuestCommandFn(char *pszCommand))(strFILE *, CDarkHoleDungeonQuestSetup *, char *);
bool (__fastcall *SearchMissionCommandFn(char *pszCommand))(strFILE *, CDarkHoleDungeonQuestSetup *, char *);
bool (__fastcall *SearchJobCommandFn(char *pszCommand))(strFILE *, CDarkHoleDungeonQuestSetup *, char *);

bool __fastcall _false(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup);
bool __fastcall GetReactObject(
  strFILE *fstr,
  CDarkHoleDungeonQuestSetup *pSetup,
  _react_obj *poutReactObject,
  bool bReadCnt,
  unsigned int dwDesiredApply,
  char *pszoutErrMsg,
  char *pszoutEventCode);
bool __fastcall GetReactArea(
  strFILE *fstr,
  CDarkHoleDungeonQuestSetup *pSetup,
  _react_area *poutReactArea,
  unsigned int dwDesiredApply,
  char *pszoutErrMsg,
  char *pszoutEventCode);

bool __fastcall qc_MembershipParty(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_MemberNum(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_UseMap(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_StartMission(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_LimitLvMin(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_LimitLvMax(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_DummyBlock(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_monsterGroup(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_RewardItem(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_RewardExp(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_RewardPvp(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_Dalant(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall qc_Description(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);

bool __fastcall mc_Area(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_StartPos(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_GatePos(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_JobOrder(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_LimTimeSec(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_ResultType(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_ResultContents(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_Description(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_CompleteMsg(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_LootItem(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_AddMonster(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_RespawnMonster(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_RespawnMonsterOption(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_ChangeMonster(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_If(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_Inner(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_respond(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall mc_AddTime(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);

bool __fastcall jc_Type(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall jc_Contents(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall jc_Count(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall jc_ReactType(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall jc_ReactContents(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
bool __fastcall jc_Description(strFILE *fstr, CDarkHoleDungeonQuestSetup *pSetup, char *pszoutErrMsg);
