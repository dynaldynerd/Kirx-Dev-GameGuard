#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"
#include "StoragePos.h"

struct _AVATOR_DATA;

struct _worlddb_character_base_info
{
  char wszName[17];
  unsigned int dwSerial;
  unsigned __int8 byRace;
  char szClassCode[5];
  unsigned __int8 bySlotIndex;
  unsigned __int8 byLevel;
  unsigned int dwDalant;
  unsigned int dwGold;
  unsigned int dwBaseShape;
  unsigned __int64 dwLastConnTime;
  char szAccount[17];
  __int16 shEKArray[8];
  unsigned int dwEUArray[8];
  unsigned int dwETArray[8];
  unsigned __int64 lnUIDArray[8];
  unsigned int dwCheckSum;
};

struct _worlddb_character_base_info_array
{
  unsigned __int16 wCharacterCount;
  _worlddb_character_base_info CharacterInfo[200];
};

struct _worlddb_character_general_info
{
  unsigned int dwHP;
  unsigned int dwFP;
  unsigned int dwSP;
  unsigned int dwDP;
  long double dExp;
  long double dLoseExp;
  unsigned __int8 byBagNum;
  unsigned __int8 byMapCode;
  float fStartPos[3];
  unsigned int dwTotalPlayMin;
  char szLeftResList[160];
  int lEK[7];
  unsigned __int16 wED[7];
  unsigned int dwET[7];
  unsigned __int64 lnUID_E[7];
  int lF[88];
  unsigned __int64 lnUID_F[88];
  unsigned int dwWM[2];
  unsigned int dwFM[24];
  unsigned int dwSM[48];
  unsigned int dwMI[3];
  unsigned int dwSR;
  unsigned int dwDM;
  unsigned int dwPM;
  unsigned __int8 byAK[4];
  unsigned __int64 dwAD[4];
  unsigned int dwAP[4];
  unsigned __int64 lnUID_A[88];
  __int16 zClassHistory[3];
  unsigned int dwClassInitCnt;
  unsigned __int8 byLastClassGrade;
  long double dPvPPoint;
  long double dPvPCashBag;
  char szBindMapCode[12];
  char szBindDummy[12];
  unsigned int dwGuildSerial;
  unsigned __int8 byGuildGrade;
  unsigned int dwRadarDelayTime;
  unsigned int dwTakeLastMentalTicket;
  unsigned int dwTakeLastCriTicket;
  unsigned __int8 byMaxLevel;
};

struct _worlddb_character_supplement_info
{
  long double dPvpPointLeak;
  bool bLastAttBuff;
  unsigned __int64 dwBufPotionEndTime;
  unsigned int dwRaceBuffClear;
  unsigned __int8 byVoted;
  unsigned __int8 VoteEnable;
  unsigned __int64 dwScanerCnt;
  unsigned int dwAccumPlayTime;
  unsigned int dwLastResetDate;
  unsigned int dwActionPoint[3];
  unsigned __int64 dwGuildEntryDelay;
  unsigned __int8 byPlayerInteg;
};

struct _worlddb_time_limit_info
{
  unsigned int dwAccSerial;
  unsigned int dwFatigue;
  unsigned __int8 byTLStatus;
  unsigned __int64 dwLastLogoutTime;
};

struct _worlddb_inven_info
{
  struct __inven_key
  {
    int lK;
    unsigned __int64 dwD;
    unsigned int dwU;
    unsigned int dwT;
    unsigned __int64 lnUID;
  };

  __inven_key invenKey[100];
};

struct _worlddb_unit_info_array
{
  struct __worlddb_unit_info
  {
    unsigned __int8 byFrame;
    unsigned int dwGauge;
    unsigned __int8 byPart[6];
    unsigned int dwBullet[2];
    unsigned int dwSpare[8];
    int nKeepingFee;
    int nPullingFee;
    unsigned __int64 dwCutTime;
  };

  __worlddb_unit_info UnitInfo[4];
};

struct _worlddb_userinterface_info
{
  unsigned __int16 wLinkBoard[50];
  unsigned int dwDamageCForce[8];
  unsigned int dwHelpCForce[8];
  unsigned int dwSkill[2];
  unsigned int dwForce[2];
  unsigned int dwCharacter[2];
  unsigned int dwAnimus[2];
  unsigned int dwInven;
  unsigned int dwInvenBag[5];
  unsigned __int8 byLinkBoardLock;
};

struct _worlddb_quest_array
{
  struct __quest_list
  {
    unsigned __int8 byQuestType;
    unsigned __int16 wIndex;
    unsigned __int16 wNum[3];
    unsigned int dwPassSec;
  };

  __quest_list questList[30];
};

struct  _worlddb_npc_quest_complete_history
{
  struct __list
  {
    char szQuestCode[8];
    unsigned __int8 byLevel;
    unsigned __int64 dwEventEndTime;
  };

  __list List[70];

  _worlddb_npc_quest_complete_history();
};

struct _worlddb_start_npc_quest_complete_history
{
  struct __list
  {
    char szQuestCode[64];
    unsigned __int8 byLevel;
    __int64 nEndTime;

    __list();
  };

  unsigned int dwListCnt;
  __list *List;
};

struct _worlddb_buddy_info
{
  struct __buddy_data
  {
    unsigned int dwSerial;
    char wszName[17];
  };

  __buddy_data BuddyData[50];
};

struct _worlddb_trunk_info
{
  struct __trunk_key
  {
    int lK;
    unsigned __int64 dwD;
    unsigned int dwU;
    unsigned __int8 byRace;
    unsigned int dwT;
    unsigned __int64 lnUID;
  };

  char wszPasswd[13];
  long double dDalant;
  long double dGold;
  unsigned __int8 byHintIndex;
  char wszHintAnswer[17];
  unsigned __int8 bySlotNum;
  __trunk_key trunkKey[100];
  unsigned __int8 byExtSlotNum;
  __trunk_key ExttrunkKey[40];
  unsigned __int8 byTrunkInteg;
};

struct _worlddb_trade_info
{
  struct __trade_key
  {
    unsigned __int8 byState;
    unsigned int dwRegistSerial;
    unsigned __int8 byInvenIndex;
    unsigned int dwPrice;
    __int64 tStartTime;
    unsigned __int8 bySellTurm;
    unsigned int dwBuyerSerial;
    unsigned int dwTax;
    __int64 tResultTime;
    char wszBuyerName[17];
    char szBuyerAccount[13];
  };

  unsigned int dwCnt;
  __trade_key list[20];
};

struct _worlddb_itemcombineex_info
{
  struct __itemcombineex_key
  {
    int lK;
    unsigned int dwD;
    unsigned int dwU;
  };

  bool bIsResult;
  unsigned int dwCheckKey;
  unsigned __int8 byDlgType;
  unsigned int dwDalant;
  unsigned __int8 byItemListCount;
  unsigned __int8 byItemSelectCount;
  __itemcombineex_key invenKey[24];
  unsigned int dwResultEffectType;
  unsigned int dwResultEffectMsgCode;
};

struct _pvppointlimit_info
{
  __int64 tUpdatedate;
  bool bUseUp;
  unsigned __int8 byLimitRate;
  long double dOriginalPoint;
  long double dLimitPoint;
  long double dUsePoint;
};

struct _worlddb_crymsg_info
{
  struct __crymsg_data
  {
    char wszCryMsg[65];
  };

  __crymsg_data CryMsgData[10];
};

struct _pvporderview_info
{
  __int64 tUpdatedate;
  int nDeath;
  int nKill;
  long double dTodayStacked;
  long double dPvpPoint;
  long double dPvpTempCash;
  long double dPvpCash;
  unsigned int dwKillerSerial[10];
  unsigned __int8 byContHaveCash;
  unsigned __int8 byContLoseCash;
  bool bRaceWarRecvr;
};

struct  _worlddb_potion_delay_info
{
  unsigned int dwPotionDelay[38];

  _worlddb_potion_delay_info();
};

struct _worlddb_ore_cutting
{
  struct _ore_cutting_info
  {
    int nKey;
    unsigned int dwDur;
  };

  _ore_cutting_info List[20];
};

struct _worlddb_pcbang_favor_item
{
  unsigned __int64 lnUID[50];
};

struct _personal_amine_inven
{
  struct __list
  {
    int lK;
    unsigned __int8 byNum;
  };

  __list list[40];
};

#pragma pack(push, 1)
struct   _NOT_ARRANGED_AVATOR_DB
{
  unsigned int dwSerial;
  unsigned __int8 byLv;
  unsigned __int8 byRaceSexCode;
  unsigned int dwDalant;
  unsigned int dwGold;
  char wszName[17];
  char szServer[33];
  char szClassCode[5];

  _NOT_ARRANGED_AVATOR_DB();
  void Init();
};
#pragma pack(pop)

struct  _worlddb_arrange_char_info
{
  unsigned __int8 byCount;
  _NOT_ARRANGED_AVATOR_DB ArrangeChar[50];
};

struct _worlddb_update_char_query
{
  char *szBaseQuery;
  char *szGeneralQuery;
  char *szInvenQuery;
  char *szUnitQuery;
  char *szUIQuery;
  char *szQuestQuery;
  char *szItemCombineExQuery;
  char *wszBuddyQuery;
  char *wszTrunkQuery;
  char *szAMPInvenQuery;
  char *szPvpPointLimitQuery;
  char *wszBossCryMsgQuery;
  char *szPvpOrderViewQurey;
  char *szNPCQuestQuery;
  char *szPcBangPlayTimeQuery;
  char *szSupplementQuery;
  char *szPotionDelayQuery;
  char *szOreCuttingQuery;
  char *szPcBangFavorQuery;
  char *wszExtTrunkQuery;
  char *szTimeLimitInfoQuery;
};

struct _worlddb_economy_history_info
{
  long double dTradeGold[3];
  long double dTradeDalant[3];
  unsigned int dwManageValue;
  long double dMineOre[3][3];
  long double dCutOre[3][3];
};

struct _worlddb_economy_history_info_array
{
  unsigned __int16 wRowCount;
  _worlddb_economy_history_info EconomyData[12];
};

struct _worlddb_guild_info
{
  struct __guild_info
  {
    unsigned int dwGuildSerial;
    unsigned __int8 byGuildGrade;
    unsigned __int8 byRace;
    char wszGuildName[17];
    unsigned int dwEmblemBack;
    unsigned int dwEmblemMark;
    long double dDalant;
    long double dGold;
    unsigned int dwMasterSerial;
    unsigned __int8 byMasterPrevGrade;
    char wszGreetingMsg[256];
    unsigned int dwTotWin;
    unsigned int dwTotDraw;
    unsigned int dwTotLose;
  };

  unsigned __int16 wGuildCount;
  __guild_info GuildData[500];
};

struct _worlddb_cash_limited_sale
{
  struct _cash_limited_info
  {
    int nLimcode;
    int nLimcount;
  };

  unsigned __int8 byDck;
  unsigned __int8 byLimited_sale_num;
  _cash_limited_info List[20];
};

struct _patriarch_comm_list
{
  struct __list
  {
    unsigned int dwDalant;
    char pszDepDate[9];
  };

  unsigned int dwCount;
  __list List[7];
};

struct  _rege_char_data
{
  struct _reged_list
  {
    unsigned __int8 bySlotIndex;
    unsigned int dwCharSerial;
    char szCharName[17];
    int nLevel;
    unsigned int dwDalant;
    unsigned int dwGold;
  };

  int nCharNum;
  _reged_list RegeList[3];

  _rege_char_data();
};

struct _guildbattle_totalrecord
{
  unsigned int dwGuildSerial;
  unsigned int dwTotWinCnt;
  unsigned int dwTotLoseCnt;
  unsigned int dwTotDrawCnt;
};

struct _unmannedtrader_reserved_schedule_info
{
  struct __list
  {
    unsigned __int8 byType;
    unsigned int dwItemSerial;
    __int64 tEndTime;
    unsigned int dwOwnerSerial;
    unsigned int dwK;
  };

  unsigned int dwCnt;
  __list list[10];
};

struct _race_battle_log_info
{
  unsigned __int8 byNth;
  unsigned int dwEndTime;
  unsigned __int8 byWinRace;
  unsigned __int8 byLoseRace;
  unsigned int dwBossSerilal0;
  unsigned int dwBossSerilal1;
  unsigned int dwBossSerilal2;
};

struct _qry_case_sendwebracebosssms
{
  unsigned int dwWebSendDBID;
  char wszMsg[255];
  char szCharacterName[17];
  unsigned int dwCharactSerial;
  unsigned __int8 byRace;

  int size() const;
};

inline int _qry_case_sendwebracebosssms::size() const
{
  return static_cast<unsigned __int16>(284);
}

struct _qry_case_gm_greetingmsg
{
  unsigned __int8 type;
  char in_gmname[17];
  char in_gmgreetingmsg[256];

  int size() const;
};

struct _qry_case_race_greetingmsg
{
  unsigned __int8 type;
  char in_bossname[17];
  char in_racegreetingmsg[256];

  int size() const;
};

inline int _qry_case_gm_greetingmsg::size() const
{
  return 274;
}

inline int _qry_case_race_greetingmsg::size() const
{
  return 274;
}

struct _qry_case_guild_greetingmsg
{
  unsigned int in_guildserial;
  char in_guildgreetingmsg[256];
};

struct _qry_case_update_honor_guild
{
  unsigned __int8 byRace;

  int size() const;
};

inline int _qry_case_update_honor_guild::size() const
{
  // narrowing cast for thunk return parity
  return static_cast<int>(sizeof(*this));
}

struct _qry_case_guildroom_insert
{
  unsigned int dwGuildSerial;
  unsigned __int8 byRoomType;
  unsigned __int8 byRace;
};

struct _qry_case_guildroom_update
{
  unsigned int dwGuildSerial;
  unsigned __int8 byRoomType;
  unsigned __int8 byRace;
};

struct _qry_case_forceleave
{
  unsigned int in_leaverserial;
  unsigned int in_guildIndex;
  unsigned int in_guildserial;
  unsigned __int8 in_apprnum;
  int in_seniornum;
  int in_MemberNum;
  bool in_bPunish;

  int size() const;
};

struct _qry_case_outputgmoney
{
  unsigned int in_poperserial;
  char in_w_popername[17];
  unsigned int tmp_guildindex;
  unsigned int in_guildserial;
  unsigned int dwSubGold;
  unsigned int dwSubDalant;
  unsigned __int8 in_date[4];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned __int8 byProcRet;

  int size() const;
};

struct _qry_case_buyemblem
{
  unsigned int in_guildserial;
  unsigned int in_emblemback;
  unsigned int in_emblemmark;
  int in_emblemdlant;
  unsigned int in_suggestorSerial;
  unsigned __int8 in_date[4];
  unsigned int tmp_guildindex;
  char tmp_w_suggestorname[17];
  long double out_totalgold;
  long double out_totaldalant;
  unsigned __int8 byProcRet;

  int size() const;
};

struct _qry_case_update_guildmaster
{
  unsigned int in_guildserial;
  unsigned int tmp_guildindex;
  unsigned int in_guild_prev_masterSerial;
  unsigned __int8 in_guild_prev_masterPrevGrade;
  unsigned int in_guild_new_masterSerial;
  unsigned __int8 in_guild_new_masterPrevGrade;

  int size() const;
};

inline int _qry_case_forceleave::size() const
{
  return 28;
}

inline int _qry_case_outputgmoney::size() const
{
  return 72;
}

inline int _qry_case_buyemblem::size() const
{
  return 72;
}

inline int _qry_case_update_guildmaster::size() const
{
  return 24;
}

struct _qry_case_loadguildbattlerank
{
  unsigned __int8 byRace;
  unsigned __int8 byLoadDataStartPosition;

  int size();
};

struct _qry_case_load_guildbattle_totalrecord
{
  unsigned int dwGuildSerial;
  unsigned int dwTotWinCnt;
  unsigned int dwTotLoseCnt;
  unsigned int dwTotDrawCnt;

  int size();
};


struct _qry_case_raceboss_accumulation_winrate
{
  unsigned int dwTotalCnt[3];
  unsigned int dwWinCnt[3];

  int size() const;
};

struct _qry_case_update_vote_available
{
  unsigned __int8 byVoteEnable;
  unsigned int dwAccountSerial;
  unsigned int dwCharSerial;
  char wszCharName[17];

  int size() const;
};

struct _qry_case_cheat_player_vote_info
{
  unsigned int dwAccountSerial;
  unsigned int dwCharSerial;
  char wszCharName[17];

  int size() const;
};

struct _qry_case_update_player_vote_info
{
  unsigned int dwAccumPlayTime;
  unsigned __int8 byIsVoted;
  unsigned __int8 byVoteEnable;
  unsigned __int16 wScaner;
  unsigned __int64 dwScanerData;
  unsigned int dwAccountSerial;
  unsigned int dwCharSerial;
  char wszCharName[17];

  int size() const;
};

struct _qry_case_update_server_reset_token
{
  unsigned int dwServerToken;
  unsigned int dwESerial;
  unsigned __int16 wProcType;
};

struct _qry_case_select_charserial
{
  unsigned __int8 byRace;
  unsigned __int8 byType;
  unsigned int dwAvatorSerial;
  char wszCharName[17];
  char wszContent[1280];

  int size() const;
};

struct _qry_case_update_user_guild_data
{
  unsigned int dwAvatorSerial;
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned __int8 byGrade;

  int size() const;
};

inline int _qry_case_select_charserial::size() const
{
  return 1308;
}

inline int _qry_case_raceboss_accumulation_winrate::size() const
{
  return 24;
}

inline int _qry_case_update_vote_available::size() const
{
  return 32;
}

inline int _qry_case_update_user_guild_data::size() const
{
  return 16;
}

struct  _qry_case_select_guild_master_lastconn
{
  unsigned int dwSerial;
  unsigned int dwGuildIndex;
  unsigned int dwGuildSerial;
  unsigned __int64 dwLimitConnTime;
  unsigned __int64 dwLastConnTime;
};

struct  _qry_case_character_rename
{
  unsigned int dwCharSerial;
  unsigned int dwAlreadySerial;
  _STORAGE_POS_INDIV ItemInfo;
  char wszCharName[17];
  char wszOldName[17];
};

struct  _qry_case_insert_patriarch_comm
{
  unsigned int dwSerial;
  unsigned int dwDalant;
  char szDepDate[9];

  _qry_case_insert_patriarch_comm();
  int size();
};

struct  _qry_case_select_patriarch_comm
{
  unsigned int dwSerial;
  unsigned __int8 byDBRet;

  int size();
};

inline _qry_case_insert_patriarch_comm::_qry_case_insert_patriarch_comm()
{
  std::memset(this, 0, sizeof(*this));
}

inline int _qry_case_insert_patriarch_comm::size()
{
  return 20;
}

inline int _qry_case_select_patriarch_comm::size()
{
  return 8;
}

inline int _qry_case_cheat_player_vote_info::size() const
{
  return 28;
}

inline int _qry_case_update_player_vote_info::size() const
{
  return 48;
}

struct _qry_case_insert_timelimit_info
{
  unsigned __int16 wIndex;
  unsigned int dwAccSerial;
  unsigned int dwLastLogoutDate;
  unsigned int dwFatigue;
  __int16 wStatus;

  int size() const;
};

struct _qry_case_update_punishment
{
  unsigned __int8 byType;
  unsigned int dwValue;
  char wszCharName[17];
  unsigned int dwAvatorSerial;
  unsigned int dwElectSerial;

  int size();
};

inline int _qry_case_insert_timelimit_info::size() const
{
  return 20;
}

inline int _qry_case_update_punishment::size()
{
  return 36;
}

struct _post_storage_list
{
  struct __list
  {
    unsigned int dwSerial;
    unsigned __int8 byIndex;
    unsigned __int8 byState;
    char wszSendName[17];
    char wszTitle[21];
    int nK;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned __int64 lnUID;
    unsigned int dwGold;
    unsigned __int16 wStorageIndex;
  };

  unsigned int dwCount;
  __list List[50];
};

struct _return_post_list
{
  struct __list
  {
    unsigned int dwSerial;
    unsigned __int8 byState;
    char wszRecvName[17];
    char wszTitle[21];
    char wszContent[201];
    int nK;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned __int64 lnUID;
    unsigned int dwGold;
    unsigned __int8 byErr;
  };

  bool bContinue;
  unsigned int dwCount;
  __list List[10];
};

struct  _qry_case_post_storage_list_get
{
  struct __list
  {
    unsigned int dwSerial;
    unsigned __int8 byIndex;
    unsigned __int8 byState;
    char wszSendName[17];
    char wszTitle[21];
    _INVENKEY key;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned __int64 lnUID;
    unsigned int dwGold;
    unsigned __int16 wStorageIndex;

    __list();
  };

  unsigned int dwMasterSerial;
  unsigned __int8 byProcRet;
  unsigned int dwCount;
  __list List[50];

  _qry_case_post_storage_list_get();
  int size();
};

struct  _qry_case_post_return_list_get
{
  struct __list
  {
    unsigned int dwSerial;
    unsigned __int8 byState;
    char wszRecvName[17];
    char wszTitle[21];
    char wszContent[201];
    _INVENKEY key;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned __int64 lnUID;
    unsigned int dwGold;
    unsigned __int8 byErr;

    __list();
  };

  unsigned int dwMax;
  unsigned int dwMasterSerial;
  unsigned __int8 byProcRet;
  unsigned int dwCount;
  __list List[10];

  _qry_case_post_return_list_get();
  int size();
};

struct  _qry_case_post_content_get
{
  unsigned int dwSerial;
  unsigned __int8 byProcRet;
  unsigned int dwMasterSerial;
  unsigned int dwIndex;
  char wszContent[201];

  _qry_case_post_content_get();
  int size();
};

struct  _qry_case_update_data_for_post_send
{
  unsigned int dwSerial;
  unsigned int dwGlod;
  _AVATOR_DATA *pNewData;
  _AVATOR_DATA *pOldData;

  _qry_case_update_data_for_post_send();
  int size();
};

struct  _qry_case_update_data_for_trade
{
  struct list
  {
    unsigned int dwSerial;
    unsigned int dwDalant;
    unsigned int dwGlod;
    _AVATOR_DATA *pNewData;
    _AVATOR_DATA *pOldData;
  };

  list tradelist[2];

  _qry_case_update_data_for_trade();
  int size();
};


inline int _qry_case_loadguildbattlerank::size()
{
  return 2;
}

inline int _qry_case_load_guildbattle_totalrecord::size()
{
  return 16;
}


inline _qry_case_post_storage_list_get::__list::__list()
{
  key = _INVENKEY();
}

inline _qry_case_post_storage_list_get::_qry_case_post_storage_list_get()
{
  std::memset(this, 0, sizeof(*this));
}

inline int _qry_case_post_storage_list_get::size()
{
  return 4016;
}

inline _worlddb_npc_quest_complete_history::_worlddb_npc_quest_complete_history()
{
  std::memset(this, 0, sizeof(*this));
}

inline _worlddb_start_npc_quest_complete_history::__list::__list()
{
  std::memset(this, 0, sizeof(*this));
}

inline _worlddb_potion_delay_info::_worlddb_potion_delay_info()
{
  std::memset(this, 0, sizeof(*this));
}

inline _qry_case_post_return_list_get::__list::__list()
{
  key = _INVENKEY();
}

inline _qry_case_post_return_list_get::_qry_case_post_return_list_get()
{
  std::memset(this, 0, sizeof(*this));
  dwMax = 10;
}

inline int _qry_case_post_return_list_get::size()
{
  return 2816;
}

inline _qry_case_post_content_get::_qry_case_post_content_get()
{
  std::memset(this, 0, sizeof(*this));
}

inline int _qry_case_post_content_get::size()
{
  return 220;
}

inline _qry_case_update_data_for_post_send::_qry_case_update_data_for_post_send()
{
  std::memset(this, 0, sizeof(*this));
}

inline int _qry_case_update_data_for_post_send::size()
{
  return 24;
}

inline _qry_case_update_data_for_trade::_qry_case_update_data_for_trade()
{
  std::memset(this, 0, sizeof(*this));
}

inline int _qry_case_update_data_for_trade::size()
{
  return 64;
}

inline _rege_char_data::_rege_char_data()
{
  std::memset(this, 0, sizeof(*this));
}

#if defined(_MSC_VER)
static_assert(sizeof(_worlddb_character_base_info) == 232, "_worlddb_character_base_info size mismatch");
static_assert(offsetof(_worlddb_character_base_info, dwSerial) == 20, "_worlddb_character_base_info.dwSerial offset mismatch");
static_assert(offsetof(_worlddb_character_base_info, byRace) == 24, "_worlddb_character_base_info.byRace offset mismatch");
static_assert(offsetof(_worlddb_character_base_info, dwDalant) == 32, "_worlddb_character_base_info.dwDalant offset mismatch");
static_assert(offsetof(_worlddb_character_base_info, lnUIDArray) == 160, "_worlddb_character_base_info.lnUIDArray offset mismatch");
static_assert(offsetof(_worlddb_character_base_info, dwCheckSum) == 224, "_worlddb_character_base_info.dwCheckSum offset mismatch");

static_assert(sizeof(_worlddb_time_limit_info) == 24, "_worlddb_time_limit_info size mismatch");
static_assert(offsetof(_worlddb_time_limit_info, dwAccSerial) == 0, "_worlddb_time_limit_info.dwAccSerial offset mismatch");
static_assert(offsetof(_worlddb_time_limit_info, dwFatigue) == 4, "_worlddb_time_limit_info.dwFatigue offset mismatch");
static_assert(offsetof(_worlddb_time_limit_info, byTLStatus) == 8, "_worlddb_time_limit_info.byTLStatus offset mismatch");
static_assert(offsetof(_worlddb_time_limit_info, dwLastLogoutTime) == 16, "_worlddb_time_limit_info.dwLastLogoutTime offset mismatch");

static_assert(sizeof(_worlddb_trade_info::__trade_key) == 80, "_worlddb_trade_info::__trade_key size mismatch");
static_assert(
  offsetof(_worlddb_trade_info::__trade_key, dwRegistSerial) == 4,
  "_worlddb_trade_info::__trade_key.dwRegistSerial offset mismatch");
static_assert(
  offsetof(_worlddb_trade_info::__trade_key, dwPrice) == 12,
  "_worlddb_trade_info::__trade_key.dwPrice offset mismatch");
static_assert(
  offsetof(_worlddb_trade_info::__trade_key, tStartTime) == 16,
  "_worlddb_trade_info::__trade_key.tStartTime offset mismatch");
static_assert(
  offsetof(_worlddb_trade_info::__trade_key, wszBuyerName) == 48,
  "_worlddb_trade_info::__trade_key.wszBuyerName offset mismatch");

static_assert(sizeof(_worlddb_update_char_query) == 168, "_worlddb_update_char_query size mismatch");
static_assert(offsetof(_worlddb_update_char_query, szBaseQuery) == 0, "_worlddb_update_char_query.szBaseQuery offset mismatch");
static_assert(
  offsetof(_worlddb_update_char_query, szTimeLimitInfoQuery) == 160,
  "_worlddb_update_char_query.szTimeLimitInfoQuery offset mismatch");
#endif
