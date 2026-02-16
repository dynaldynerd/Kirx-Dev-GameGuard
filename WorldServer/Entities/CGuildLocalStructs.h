#pragma once

  struct CommitteeResultMsg
  {
    char byAppoint;
    char wszCommitteeName[17];
  };

  #pragma pack(push, 1)
  struct GuildIOMoneyMsg
  {
    unsigned int dwIOerSerial;
    unsigned __int8 byMoneyOutputKind;
    bool bInPut;
    long double dIODalant;
    long double dIOGold;
    long double dTotalDalant;
    long double dTotalGold;
    unsigned __int8 byDate[4];
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct ApplyGuildBattleResultMsg
  {
    char byRet;
    char wszDestGuildName[17];
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct GuildInfoUpdateMsg
  {
    unsigned int dwSerial;
    unsigned __int8 byGrade;
    unsigned int dwEmblemBack;
    unsigned int dwEmblemMark;
    unsigned int dwTotWin;
    unsigned int dwTotDraw;
    unsigned int dwTotLose;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct DelJoinApplierMsg
  {
    unsigned int dwSerial;
    unsigned __int8 byDelCode;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct AddJoinApplierMsg
  {
    unsigned int dwSerial;
    char wszName[18];
    int nPvpPoint;
    unsigned int dwApplyTime;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct GuildLeaveMsg
  {
    char bSelf;
    char bPunish;
    unsigned int dwMemberSerial;
  };
  #pragma pack(pop)

  #pragma pack(push, 1)
  struct GuildJoinAcceptMsg
  {
    unsigned int dwAcceptSerial;
    unsigned int dwSerial;
    char wszNameAndClassAndLv[19];
    unsigned int dwPvpPoint;
    unsigned __int8 byRank;
  };
  #pragma pack(pop)

#pragma pack(push, 1)
  struct GuildVoteStateMessage
  {
    unsigned int dwMatterVoteSynKey;
    unsigned __int8 byApprovePoint;
    unsigned __int8 byOpposePoint;
    char byLoginSeniorNum;
    char byTotalVotableNum;
  };
#pragma pack(pop)

#pragma pack(push, 1)
  struct GuildTaxRateChangeMessage
  {
    unsigned int dwGuildSerial;
    unsigned __int8 byTax;
  };
#pragma pack(pop)

    struct GuildBuddyData
    {
      unsigned int dwSerial;
      unsigned __int16 wMapCode;
      unsigned __int8 byRegionIndex;
    };

  struct GuildRankSortEntry
  {
    unsigned __int8 byGrade;
    unsigned int dwPvpPoint;
    int nIndex;
  };

