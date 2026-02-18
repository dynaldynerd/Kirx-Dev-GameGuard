#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct _action_node
{
  int m_nActType;
  char m_strActSub[64];
  char m_strActSub2[64];
  char m_strActArea[64];
  int m_nReqAct;
  int m_nSetCntPro_100;
  char m_strLinkQuestItem[64];
  int m_nOrder;
};

struct _quest_reward_item
{
  char m_strConsITCode[64];
  int m_nConsITCnt;
  int m_nLinkQuestIdx;
};

struct _quest_reward_mastery
{
  int m_nConsMasteryID;
  int m_nConsMasterySubID;
  int m_nConsMasteryCnt;
};

struct _quest_fail_condition
{
  int m_nFailCondition;
  char m_strFailCode[64];
};

struct  _Quest_fld : _base_fld
{
  int m_nLimLv;
  int m_nQuestType;
  int m_bQuestRepeat;
  long double m_dRepeatTime;
  int m_nDifficultyLevel;
  int m_n2;
  int m_bSelectQuestMenual;
  int m_bCompQuestType;
  _action_node m_ActionNode[3];
  int m_nMaxLevel;
  long double m_dConsExp;
  int m_nConsContribution;
  int m_nConsDalant;
  int m_nConspvppoint;
  int m_nConsGold;
  int m_bSelectConsITMenual;
  _quest_reward_item m_RewardItem[6];
  _quest_reward_mastery m_RewardMastery[2];
  char m_strConsSkillCode[64];
  int m_nConsSkillCnt;
  char m_strConsForceCode[64];
  int m_nConsForceCnt;
  char m_strLinkQuest[5][64];
  int m_nLinkQuestGroupID;
  int m_bFailCheck;
  _quest_fail_condition m_QuestFailCond[3];
  char m_strFailBriefCode[64];
  int m_nLinkDummyCond;
  char m_strLinkDummyCode[64];
  char m_strFailLinkQuest[64];
  int m_nViewportType;
  char m_strViewportCode[64];
  int m_nStore_trade;
  char m_txtQTExp[64];
};
#pragma pack(pop)
