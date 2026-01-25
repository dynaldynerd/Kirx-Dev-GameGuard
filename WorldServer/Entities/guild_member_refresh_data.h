#pragma once

struct _guild_member_refresh_data
{
  struct __refresh_data
  {
    unsigned int dwSerial;
    unsigned __int8 byGrade;
    unsigned __int8 byLv;
    unsigned int dwPvpPoint;
  };

  unsigned int dwGuildSerial;
  unsigned __int16 wMemberCount;
  __refresh_data rMemberData[50];
};
