#pragma once

#include "IdaCompat.h"
#include "CGuildList.h"

struct _guild_member_refresh_data;
class CMgrGuildHistory;

/* 1717 */
class __cppobj __declspec(align(8)) CGuild
{
public:
  static CGuildList s_GuildList;
  static CMgrGuildHistory s_MgrHistory;

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
  bool IsFill();
  void ClearVote();
  void StartRankJob();
  void EndRankJob();
  char LogoffMember(unsigned int dwMemberSerial);
  _guild_member_info *LoginMember(unsigned int dwMemberSerial, CPlayer *pPtr);
  void SendMsg_GuildMemberLogoff(unsigned int dwSerial);
  void SendMsg_GuildMemberLogin(unsigned int dwSerial, unsigned __int16 wMapCode, unsigned __int16 wRegionIndex);
  char PopApplier(unsigned int dwApplierSerial, unsigned __int8 byDelCode);
  char PushApplier(CPlayer *pApplier);
  unsigned __int8 GetGrade();
  void UpdateGrade(unsigned __int8 byGrade);
  void RefreshGuildMemberData(_guild_member_refresh_data *pRefreshMember);
  void SendMsg_AlterMemberState();
  void SendMsg_AlterMemberGrade();
  void SendMsg_VoteComplete(bool bPass);
  void InitVote();
  void MakeDownMemberPacket();
  void MakeQueryInfoPacket();
  void SendMsg_GuildInfoUpdateInform();
  void SendMsg_AddJoinApplier(_guild_applier_info *p);
  void SendMsg_DelJoinApplier(_guild_applier_info *p, unsigned __int8 byDelCode);
  void MakeDownApplierPacket();
  unsigned int GetGuildMasterSerial();
  _guild_member_info *GetMemberFromSerial(unsigned int dwMemberSerial);
  void SetGreetingmsg_GUILD(char *wszgreetmsg);
  unsigned __int8 ManageAcceptORRefuseGuildBattle(bool bAccept);
  void SendMsg_ApplyGuildBattleResultInform(char byRet, char *wszDestGuildName);
  void SendMsg_GuildBattleRefused(char *pwszName);
  void SendMsg_GuildBattleSuggestResult(unsigned __int8 byRet, char *wszDestGuildName);
  char SendMsg_GuildBattleProposed(char *pwszName);
  void PushDQSInGuildBattleCost();
  void PushDQSDestGuildOutputGuildBattleCost();
  void CompleteOutGuildbattleCost(
    unsigned int dwSrcGuildSerial,
    unsigned int dwStartTimeInx,
    unsigned int dwMemberCntInx,
    unsigned int dwMapInx);
  void UpdateGuildBattleWinCnt(unsigned int dwTotWin, unsigned int dwTotDraw, unsigned int dwTotLose);
  unsigned __int8 CheckGuildBattleSuggestRequestToDestGuild(
    unsigned int dwSrcGuildSerial,
    unsigned int dwStartTimeInx,
    unsigned int dwMemberCntInx,
    unsigned int dwMapInx);
  unsigned __int8 DestGuildIsAvailableBattleRequestState();
  void SetGuildBattleMatter(
    unsigned int dwSrcGuildSerial,
    unsigned int dwStartTime,
    unsigned int dwNumber,
    unsigned int dwMapIdx);
  void IOMoney(
    const char *pwszIOerName,
    unsigned int dwIOerSerial,
    long double dIODalant,
    long double dIOGold,
    long double dTotalDalant,
    long double dTotalGold,
    unsigned __int8 *pbyDate,
    bool bInPut);
  void PushHistory_IOMoney(
    bool bInput,
    const char *pwszIOerName,
    unsigned int dwIOerSerial,
    long double dIODalant,
    long double dIOGold,
    long double dLeftDalant,
    long double dLeftGold,
    unsigned __int8 *pbyDate);
  void MakeMoneyIOPacket();
  void SendMsg_IOMoney(
    unsigned int dwIOerSerial,
    long double dIODalant,
    long double dIOGold,
    bool bInPut,
    unsigned __int8 *pbyDate);
  long double GetTotalDalant();
  long double GetTotalGold();
  _guild_member_info *GetMemberFromSerial(unsigned int dwMemberSerial);
  unsigned int GetGuildMasterSerial();
  void SendMsg_ManageGuildCommitteeResult(char bAppoint, char *pwszCommitteeName);
  void SendMsg_MasterElectPossible(char bPossible);
  void Complete_DB_Update_Committee(char *pData);
  void CompleteSelectMasterLastConn(unsigned int dwLastConnTime);
  unsigned int GetMemberNum();

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

CGuild *GetGuildDataFromSerial(CGuild *pData, int nNum, unsigned int dwSerial);
CGuild *GetGuildPtrFromName(CGuild *pData, int nNum, char *pwszGuildName);

