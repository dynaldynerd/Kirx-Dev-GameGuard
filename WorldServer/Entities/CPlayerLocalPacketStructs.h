#pragma once

// Packet structs missing from current WorldServer headers (IDA names).
#pragma pack(push, 1)

struct _alter_tower_hp_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int16 wLeftHP;
};

struct _alter_equip_sp_inform_zocl
{
  float fEquipUseSP;
};

struct _set_dp_inform_zocl
{
  unsigned __int16 wDP;
};

struct _max_dp_zocl
{
  unsigned __int16 wMaxDP;
};

struct _buddy_off_reged_player_inform_zocl
{
  unsigned int dwSerial;
};

struct _buddy_add_fail_zocl
{
  char byRetCode;
  char wszDstName[17];
};

struct _party_join_invitation_question_zocl
{
  _CLID idBoss;
};

struct _party_join_application_question_zocl
{
  _CLID idApplicant;
};

struct _party_join_member_result_zocl
{
  unsigned int dwJoinerSerial;
  char wszJoinerName[17];
  char byLootShareMode;
  unsigned __int16 wIndex;
};

struct _party_leave_compulsion_result_zocl
{
  unsigned int dwExiterSerial;
};

struct _away_party_join_invitation_question_zocl
{
  _CLID idBoss;
  char wszCharName[17];
};

struct _guild_master_info_zocl
{
  char byState;
  char byGrade;
  char byEffectValue[4];
};

struct _set_group_target_object_result_zocl
{
  char byRetCode;
  char byGroupType;
  char byMapCode;
  char byID;
  unsigned int dwSerial;
  float fPos[3];
};

struct _set_group_map_point_result_zocl
{
  char byRetCode;
  char byGroupType;
  char byMapCode;
  char byRemain;
  float zPos[2];
};

struct _buddy_download_result_zocl
{
  unsigned __int16 wDataSize;
  char sData[10000];
};

#pragma pack(pop)
