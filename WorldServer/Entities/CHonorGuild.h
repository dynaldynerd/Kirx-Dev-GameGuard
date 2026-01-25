#pragma once

#include "IdaCompat.h"

struct _guild_honor_list_result_zocl
{
  struct __list
  {
    unsigned int dwGuildSerial;
    unsigned int dwEmblemBack;
    unsigned int dwEmblemMark;
    char wszGuildName[17];
    char wszMasterName[17];
    unsigned __int8 byTaxRate;
  };

  unsigned __int8 byListNum;
  unsigned __int8 byUI;
  __list GuildList[5];

  _guild_honor_list_result_zocl();
  int size() const;
};

class __cppobj CHonorGuild
{
public:
  static CHonorGuild *Instance();
  bool Init();

  virtual ~CHonorGuild() = default;

  bool m_bNext[3];
  bool m_bSendInform[3];
  _guild_honor_list_result_zocl *m_pCurrHonorGuild[3];
  _guild_honor_list_result_zocl *m_pNextHonorGuild[3];
  bool m_bChageInform[3];
  unsigned int m_uiProccessIndex[3];
};
