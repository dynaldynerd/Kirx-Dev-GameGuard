#pragma once

#include "IdaCompat.h"

struct _guild_honor_set_request_clzo;

#pragma pack(push, 1)
struct _guild_honor_list_result_zocl
{
  #pragma pack(push, 1)
  struct __list
  {
    unsigned int dwGuildSerial;
    unsigned int dwEmblemBack;
    unsigned int dwEmblemMark;
    char wszGuildName[17];
    char wszMasterName[17];
    unsigned __int8 byTaxRate;
  };
  #pragma pack(pop)

  unsigned __int8 byListNum;
  unsigned __int8 byUI;
  __list GuildList[5];

  _guild_honor_list_result_zocl();
  int size() const;
};
#pragma pack(pop)

class  CHonorGuild
{
public:
  static CHonorGuild *m_pInstance;

  CHonorGuild();

  static CHonorGuild *Instance();
  static void Destroy(CHonorGuild *thisPtr);
  bool Init();
  void Loop();
  void LoopSubProcSendInform(unsigned __int8 byRace);
  void SendCurrHonorGuildList(unsigned __int16 wIndex, unsigned __int8 byRace, unsigned __int8 byUI);
  void SendNextHonorGuildList(unsigned __int16 wIndex, unsigned __int8 byRace);
  char LoadDB();
  unsigned __int8 FindHonorGuildRank(unsigned __int8 byRace, unsigned int dwGuildSerial);
  char CheckHonorGuild(unsigned __int8 byRace, unsigned int dwSerial);
  unsigned __int8 UpdateNextHonorGuild(unsigned __int8 byRace);
  unsigned __int8 UpdateChangeHonorGuild(unsigned __int8 byRace);
  unsigned __int8 SetNextHonorGuild(unsigned __int8 byRace, _guild_honor_set_request_clzo *pRecv);
  void ChangeHonorGuild(unsigned __int8 byRace);
  void SendInformChange(unsigned __int8 byRace, unsigned __int16 wIndex);
  void UpdateHonorGuildMark(_guild_honor_list_result_zocl *pList, int bSet);
  void SetGuildMaintainMoney(unsigned __int8 byRace, unsigned int dwTax, unsigned int dwSeller);
  void DQSCompleteInAtradTaxMoney(char *pdata);

  virtual ~CHonorGuild();

  bool m_bNext[3];
  bool m_bSendInform[3];
  _guild_honor_list_result_zocl *m_pCurrHonorGuild[3];
  _guild_honor_list_result_zocl *m_pNextHonorGuild[3];
  bool m_bChageInform[3];
  unsigned int m_uiProccessIndex[3];
};
