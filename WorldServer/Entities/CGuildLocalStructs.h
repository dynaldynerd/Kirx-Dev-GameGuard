#pragma once

#pragma pack(push, 1)
struct GuildBuddyData_msg_zocl
{
  unsigned int dwSerial;
  unsigned __int16 wMapCode;
  unsigned __int8 byRegionIndex;
};
#pragma pack(pop)

#pragma pack(push, 1)
struct GuildRankSortEntry
{
  unsigned __int8 byGrade;
  unsigned int dwPvpPoint;
  int nIndex;
};
#pragma pack(pop)

