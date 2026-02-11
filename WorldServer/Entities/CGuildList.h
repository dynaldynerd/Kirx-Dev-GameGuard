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

  __guild_list_page();
};

class __cppobj CGuildList
{
public:
  CGuildList();
  virtual ~CGuildList();
  char Init();
  void AddList(unsigned __int8 byRace, unsigned __int8 byGrade, char *pwszGuildName, char *pwszMasterName);
  void SetGrade(unsigned __int8 byRace, char *pwszGuildName, unsigned __int8 byGrade);
  void SetGuildMaster(unsigned __int8 byRace, char *pwszGuildName, char *pwszMasterName);

  bool m_bInit;
  unsigned __int8 m_byMaxPage[3];
  __guild_list_page *m_pGuildList[3];
};
