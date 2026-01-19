#pragma once

#include "IdaCompat.h"

#include "CGuildList_vtbl.h"

struct __cppobj __guild_list_page
{
  struct __list
  {
    unsigned __int8 byGrade;
    char wszGuildName[17];
    char wszMasterName[17];
  };

  unsigned __int8 byListCnt;
  __list GuildList[4];
};

class __cppobj CGuildList
{
public:
  virtual ~CGuildList() = default;

  bool m_bInit;
  unsigned __int8 m_byMaxPage[3];
  __guild_list_page *m_pGuildList[3];
};
