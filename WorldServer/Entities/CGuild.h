#pragma once

#include "IdaCompat.h"

/* 1717 */
class __cppobj __declspec(align(8)) CGuild
{
public:
  class MakeBuddyPacket
  {
  public:
    class __l2
    {
    public:
      struct __declspec(align(2)) __guild_buddy_list
      {
        unsigned int dwSerial;
        unsigned __int16 wMapCode;
        unsigned __int8 byRegionIndex;
      };
    };
  };

  virtual ~CGuild() = default;
  void Init(unsigned int index);

public:
  int m_nIndex;
  unsigned int m_dwSerial;
  char m_wszName[17];
  char m_aszName[17];
  unsigned __int8 m_byGrade;
  long double m_dTotalDalant;
  long double m_dTotalGold;
  unsigned int m_dwEmblemBack;
  unsigned int m_dwEmblemMark;
  unsigned __int8 m_byRace;
  char m_wszGreetingMsg[256];
  _guild_master_info m_MasterData;
  int m_nMemberNum;
  _guild_member_info *m_MemberData;
  _guild_member_info *m_pGuildCommittee[3];
  int m_nApplierNum;
  _guild_applier_info *m_ApplierData;
  bool m_bNowProcessSgtMter;
  unsigned int m_dwSuggesterSerial;
  _suggested_matter m_SuggestedMatter;
  _guild_battle_suggest_matter m_GuildBattleSugestMatter;
  bool m_bInGuildBattle;
  bool m_bPossibleElectMaster;
  unsigned int m_dwGuildBattleTotWin;
  unsigned int m_dwGuildBattleTotDraw;
  unsigned int m_dwGuildBattleTotLose;
  _guild_member_download_zocl *m_DownPacket_Member;
  _guild_applier_download_zocl *m_DownPacket_Applier;
  _guild_query_info_result_zocl *m_QueryPacket_Info;
  _guild_money_io_download_zocl *m_MoneyIO_List;
  _guild_member_buddy_download_zocl *m_Buddy_List;
  int m_nIOMoneyHistoryNum;
  _io_money_data m_IOMoneyHistory[100];
  bool m_bDBWait;
  bool m_bIOWait;
  bool m_bRankWait;
  unsigned __int8 m_byMoneyOutputKind;
  int m_nTempMemberNum;
  unsigned int m_dwLastLoopTime;
  char m_szHistoryFileName[128];
};

