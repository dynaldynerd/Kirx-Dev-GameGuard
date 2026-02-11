#pragma once

#include "IdaCompat.h"

#include <sqltypes.h>

struct _guildroom_info
{
  struct gr_info
  {
    unsigned int dwGuildSerial;
    char uszGuildName[17];
    unsigned __int8 byRoomType;
    unsigned __int8 byRace;
    tagTIMESTAMP_STRUCT ts;
  };

  unsigned __int8 byCount;
  gr_info info[90];
};
