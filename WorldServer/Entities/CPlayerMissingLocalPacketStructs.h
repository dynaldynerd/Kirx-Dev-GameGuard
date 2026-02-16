#pragma once

// Packet structs missing from current WorldServer headers (IDA names).
#pragma pack(push, 1)

struct _unit_sell_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  unsigned __int16 wKeySerial;
  unsigned int dwNonPayDalant;
  int nAddMoney[7];
  unsigned int dwLeftMoney[7];
};

struct _unit_part_tuning_result_zocl
{
  char byRetCode;
  char bySlotIndex;
  char byPart[6];
  unsigned int dwBullet[2];
  int nCost[7];
  unsigned int dwLeftMoney[7];
};

struct _skill_result_one_zocl
{
  char byErrCode;
  char byAttackSerial;
};

struct _skill_result_other_zocl
{
  char byRetCode;
  _CHRID idPerformer;
  _CHRID idDster;
  char bySkillIndex;
  char bySkillLv;
  char byAttackSerial;
};

struct _class_skill_result_one_zocl
{
  char byErrCode;
  char byAttackSerial;
};

struct _force_result_other_zocl
{
  char byRetCode;
  char byForceIndex;
  char byForceLv;
  _CHRID idPerformer;
  _CHRID idDster;
  char byAttackSerial;
};

struct _potion_delay_time_information_zocl
{
  int nMaxNum;
  unsigned int dwPotionDelayTime[38];
};

struct _guild_list_result_zocl
{
  struct __list
  {
    char byGrade;
    char wszGuildName[17];
    char wszMasterName[17];
  };

  char byPage;
  char byMaxPage;
  char byListCnt;
  _guild_list_result_zocl::__list GuildList[4];
};

#pragma pack(pop)
