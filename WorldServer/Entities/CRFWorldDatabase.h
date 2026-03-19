#pragma once

#include "CRFNewDatabase.h"
#include "CandidateMgr.h"
#include "DqsDbStructs.h"
#include "unmannedtrader_page_info.h"

struct _AIOC_A_MACRODATA;
struct _PCBANG_PLAY_TIME;
struct _patriarch_comm_list;
struct _post_storage_list;
struct _return_post_list;
struct _rege_char_data;
struct _guild_honor_list_result_zocl;
struct _qry_case_gm_greetingmsg;
struct _qry_case_guild_greetingmsg;
struct _qry_case_race_greetingmsg;
struct _worlddb_arrange_char_info;
struct _worlddb_buddy_info;
struct _worlddb_cash_limited_sale;
struct _worlddb_character_base_info;
struct _worlddb_character_general_info;
struct _worlddb_character_base_info_array;
struct _worlddb_character_array_info;
struct _worlddb_crymsg_info;
struct _worlddb_economy_history_info;
struct _worlddb_economy_history_info_array;
struct _worlddb_guild_info;
struct _worlddb_guild_member_info;
struct _worlddb_guild_money_io_info;
struct _worlddb_guild_battle_info;
struct _worlddb_guild_battle_rank_list;
struct _worlddb_guild_battle_schedule_list;
struct _worlddb_guild_battle_reserved_schedule_info;
struct _worlddb_inven_info;
struct _worlddb_itemcombineex_info;
struct _worlddb_npc_quest_complete_history;
struct _worlddb_ore_cutting;
struct _worlddb_pcbang_favor_item;
struct _worlddb_potion_delay_info;
struct _worlddb_quest_array;
struct _worlddb_sf_delay_info;
struct _worlddb_time_limit_info;
struct _worlddb_update_char_query;
struct _worlddb_user_count_info;
struct _worlddb_item_list;
struct _personal_amine_inven;
struct _pvporderview_info;
struct _pvppointlimit_info;
struct _pvppoint_guild_rank_info;
struct _PVP_RANK_DATA;
struct _raceboss_acc_winrate;
struct _sel_patriarch_elect_state;
struct _total_guild_rank_info;
struct _unmannedtrader_buy_item_info;
struct _unmannedtrader_registsingleitem;
struct _unmannedtrader_seller_info;
struct _unmannedtrader_stade_id_info;
struct _weeklyguildrank_owner_info;
struct _worlddb_rankinguild_info;
struct _worlddb_golden_box_item;
struct _guildroom_info;
struct TournamentWinner;
struct _qry_case_all_store_limit_item;
struct _DB_LOAD_AUTOMINE_MACHINE;
class CPostData;

/* 1133 */
class  CRFWorldDatabase : public CRFNewDatabase
{
public:
  CRFWorldDatabase();
  virtual ~CRFWorldDatabase();

  unsigned __int8 Select_CharacterBaseInfoBySerial(
    unsigned int dwAccountSerial,
    _worlddb_character_base_info_array *pCharacterDataArray);
  unsigned __int8 Select_CharacterBaseInfoByName(char *pwszCharacterName, _worlddb_character_base_info *pCharacterData);
  unsigned __int8 Select_CharacterBaseInfo(unsigned int dwCharacterSerial, _worlddb_character_base_info *pCharacterData);
  char Select_CharactersInfo(unsigned int dwAccountSerial, _worlddb_character_array_info *pCharacterData);
  unsigned __int8 Select_CharacterGeneralInfo(unsigned int dwCharacterSerial, _worlddb_character_general_info *pCharacterData);
  unsigned __int8 Select_Inven(unsigned int dwSerial, unsigned __int16 wBagCount, _worlddb_inven_info *pInvenData);
  unsigned __int8 Select_Quest(unsigned int dwSerial, _worlddb_quest_array *pQuestData);
  bool Insert_Quest(unsigned int dwSerial);
  unsigned __int8 Select_ItemCombineEx(unsigned int dwSerial, _worlddb_itemcombineex_info *pItemCombineExInfo);
  bool Insert_ItemCombineEx(unsigned int dwSerial);
  unsigned __int8 Select_Buddy(unsigned int dwSerial, _worlddb_buddy_info *pBuddyData);
  bool Insert_Buddy(unsigned int dwSerial);
  unsigned __int8 Select_BossCryMsg(unsigned int dwSerial, _worlddb_crymsg_info *pCryMsg);
  bool Insert_BossCryRecord(unsigned int dwSerial);
  unsigned __int8 Select_NpcQuest_History(unsigned int dwSerial, _worlddb_npc_quest_complete_history *pNpcQHis);
  bool Insert_NpcQuest_History(unsigned int dwSerial);
  unsigned __int8 Select_Start_NpcQuest_History_Count(unsigned int dwSerial, unsigned int *pdwCount);
  unsigned __int8 Select_Start_NpcQuest_History(
    unsigned int dwSerial,
    _worlddb_start_npc_quest_complete_history *pNpcQHis,
    unsigned int dwCount);
  int Select_WinBattleCount(unsigned __int8 byRace, unsigned int dwSerial, unsigned int *dwCount);
  int Select_FailBattleCount(unsigned __int8 byRace, unsigned int dwSerial, unsigned int *dwCount);
  int Select_LoseBattleCount(unsigned __int8 byRace, unsigned int dwSerial, unsigned int *dwCount);
  unsigned __int8 Select_MacroData(unsigned int dwSerial, _AIOC_A_MACRODATA *pMacro);
  int Select_OreCutting(unsigned int dwSerial, _worlddb_ore_cutting *pOreCutting);
  bool Insert_OreCutting(unsigned int dwSerial);
  int Select_PcBangFavorItem(unsigned int dwSerial, _worlddb_pcbang_favor_item *pFavorItem);
  bool Insert_PcBangFavorItem(unsigned int dwSerial);
  int Select_PotionDelay(unsigned int dwSerial, _worlddb_potion_delay_info *pPotionDelayInfo);
  bool Insert_PotionDelay(unsigned int dwSerial);
  unsigned __int8 Select_SFDelayInfo(unsigned int dwSerial, _worlddb_sf_delay_info *pSFDelay);
  bool Insert_SFDelayInfo(unsigned int dwSerial, _worlddb_sf_delay_info *pSFDelay);
  int Select_PrimiumPlayTime(unsigned int dwAccSerial, _PCBANG_PLAY_TIME *pPlayTime);
  bool Insert_PrimiumPlayTime(unsigned int dwAccSerial);
  int Select_Punishment(unsigned int dwSerial, unsigned int *pdwElectSerial, unsigned int *pdwPunishment);
  int Select_PatriarchVoted(unsigned __int8 byRace, unsigned int dwSerial, bool *bOverlapVote);
  int Select_PatriarchRefundCount(unsigned __int8 byRaceCode, unsigned int dwAvatorSerial, unsigned int *pdwCnt);
  int Select_PvpOrderViewInfo(unsigned int dwSerial, _pvporderview_info *pInfo);
  bool Insert_PvpOrderViewInfo(unsigned int dwSerial);
  unsigned __int8 Select_PvpPointLimitInfo(unsigned int dwSerial, _pvppointlimit_info *pInfo);
  bool Insert_PvpPointLimitInfoRecord(unsigned int dwSerial);
  int Select_Supplement(unsigned int dwSerial, _worlddb_character_supplement_info *pSupplement);
  int Select_Supplement_Ex(unsigned int dwSerial, _worlddb_character_supplement_info *pSupplement);
  int Select_Supplement_ActPoint(unsigned int dwSerial, _worlddb_character_supplement_info *pSupplement);
  unsigned __int8 Select_Trade(
    unsigned __int8 byType,
    unsigned int dwSerial,
    unsigned __int8 byRace,
    _worlddb_trade_info *pTradeData);
  unsigned __int8 Select_AccountTrunk(unsigned int dwAccountSerial, unsigned __int8 byRace, _worlddb_trunk_info *pTrunkData);
  bool Insert_AccountTrunk(unsigned int dwAccountSerial);
  unsigned __int8 Select_AccountTrunkExtend(unsigned int dwAccountSerial, _worlddb_trunk_info *pTrunkData);
  bool Insert_AccountTrunkExtend(unsigned int dwAccountSerial);
  unsigned __int8 Select_UserInterface(unsigned int dwSerial, _worlddb_userinterface_info *pUserinterfaceInfo);
  bool Insert_UserInterface(unsigned int dwSerial);
  unsigned __int8 Select_Unit(unsigned int dwCharacterSerial, _worlddb_unit_info_array *pUnitInfo);
  bool Insert_Unit(unsigned int dwCharacterSerial);
  unsigned __int8 Select_NotArrangeCharacter(unsigned int dwAccountSerial, _worlddb_arrange_char_info *pCharData);
  char Select_Equal_Name(char *pwszCharacterName);
  bool Select_Equal_DeleteName_NoArranged(char *pwszCharacterName);
  char Select_CharacterSerial(char *pwszCharacterName, unsigned int *pSerial);
  bool Select_AccountSerial(char *pwszCharacterName, char *szAccount, unsigned int *pSerial);
  char Select_AccountByAvatorName(char *pwszAvatorName, char *szAccount);
  bool Insert_CharacterData(
    char *pwszCharacterName,
    char *szClassCode,
    unsigned int dwAccountSerial,
    char *szAccount,
    unsigned __int8 bySlotIndex,
    unsigned __int8 byRace,
    unsigned int dwBaseShape,
    int nMapIndex,
    unsigned int *pdwSerial);
  bool Insert_MacroData(unsigned int dwSerial);
  bool Delete_CharacterData(unsigned int dwSerial);
  bool Insert_Level_Log(unsigned int dwCharacterSerial, unsigned __int8 byLevel, unsigned int dwTotalPlayMin);
  bool Insert_UserNum_Log(int nAvgUserNum, int nMaxUserNum);
  bool Select_UserCountInfo(
    char *szStartDate,
    char *szEndDate,
    _worlddb_user_count_info *pUserCountData);
  unsigned __int8 Select_Exist_Economy(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData);
  unsigned __int8 Select_Economy_History(_worlddb_economy_history_info_array *pEconomyData, unsigned int dwDate);
  bool Insert_Economy_History(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData);
  bool Select_BattleTournamentInfo(TournamentWinner *pWinnerInfo, int nMax);
  unsigned __int8 Select_CheckGreetRecord(int nUseType);
  bool Insert_GreetingRecord(int nUseType, const char *wszName, const char *wszMessage);
  bool LoadGreetingMsg(
    char *pwszGMGreetingmsg,
    char *pwszRaceGreetingmsgA,
    char *pwszRaceGreetingmsgB,
    char *pwszRaceGreetingmsgC,
    char *pwszGMName,
    char *pwszNameA,
    char *pwszNameB,
    char *pwszNameC);
  unsigned __int8 Select_LimitInfo(unsigned __int8 *pData, unsigned __int64 tDataSize);
  int Select_GodenBoxItem(_worlddb_golden_box_item *goldenboxitem, int *pnSerial);
  bool Insert_GoldenBoxItem();
  bool Select_GuildRoomInfo(_guildroom_info *pInfo);
  unsigned __int8 Select_UsedLimitItemRecordNum(unsigned int *pdwUsedNum);
  unsigned __int8 Select_TotalRecordNum(unsigned int *pdwTotalNum);
  unsigned __int8 Select_StoreLimitItem(_qry_case_all_store_limit_item *pData);
  bool CreateCharacterSelectLogTable(const char *szTableName);
  bool InsertCharacterSelectLog(
    unsigned int dwAccountSerial,
    char *szAccount,
    unsigned int dwCharacSerial,
    char *pwszCharacName,
    unsigned __int16 dwYear,
    unsigned __int8 byMonth,
    unsigned __int8 byDay,
    unsigned __int8 byHour,
    unsigned __int8 byMin,
    unsigned __int8 bySec);
  bool Update_PvpPointInfo(unsigned int dwSerial, __int16 *pzClassHistory, long double dPvpPoint);
  bool Add_PvpPoint(unsigned int dwSerial, unsigned int dwPoint, unsigned int dwCashBag);
  bool Insert_ItemChargeInGame(
    unsigned int dwSerial,
    unsigned int dwItemCodeK,
    unsigned int dwItemCodeD,
    unsigned int dwItemCodeU,
    unsigned __int8 byType);
  bool Insert_Guild(char *pwszGuildName, unsigned __int8 byRace);
  bool Select_GuildSerial(char *pwszGuildName, unsigned int *dwGuildSerial);
  bool Insert_WeeklyGuildPvpPointSum(unsigned int dwGuildSerial);
  bool Insert_DefaultWeeklyGuildPvpPointSumRecord();
  bool Update_UserGuildData(unsigned int dwAvatorSerial, unsigned int dwGuildSerial, unsigned __int8 byGrade);
  unsigned __int8 Select_RaceBossAccumulationWinRate(
    unsigned __int8 byRace,
    unsigned int dwBossSerial,
    _raceboss_acc_winrate *windata);
  unsigned __int8 Select_RaceBossCurrentWinRate(
    unsigned __int8 byRace,
    char *szDate,
    unsigned int *dwTotalCnt,
    unsigned int *dwWinCnt);
  bool Check_GuildMemberCount(unsigned int dwGuildSerial);
  bool Update_GuildMemberCount(unsigned int dwGuildSerial, unsigned __int16 wMemberNum);
  bool Update_InputGuildMoney(
    unsigned int dwGuildSerial,
    unsigned int dwDalant,
    unsigned int dwGold);
  bool Update_OutputGuildMoney(
    unsigned int dwGuildSerial,
    unsigned int dwDalant,
    unsigned int dwGold);
  unsigned __int16 Select_AllGuildNum();
  bool UpdateGuildMoney(unsigned int dwSerial, long double dDalant, long double dGold);
  bool Select_GuildData(unsigned int dwGuildSerial, _worlddb_guild_info::__guild_info *pGuildData);
  bool Select_AllGuildData(_worlddb_guild_info *pGuildInfo);
  bool Select_GuildMemberData(
    unsigned __int16 wMaxMember,
    unsigned int dwGuildSerial,
    _worlddb_guild_member_info *pGuildMemberInfo);
  bool Select_GuildMoneyIOData(unsigned int dwGuildSerial, _worlddb_guild_money_io_info *pGuildIOData);
  bool Insert_GuildMoneyHistory(
    unsigned int dwGuildSerial,
    long double dIO_Dalant,
    long double dIO_Gold,
    long double dTotalDalant,
    long double dTotalGold,
    char *szDate,
    unsigned int dwCharSerial,
    const char *wszCharName);
  bool Update_SetGuildMoney(unsigned int dwGuildSerial, long double dTotalDalant, long double dTotalGold);
  bool Update_GuildEmblem(
    unsigned int dwGuildSerial,
    long double dTotalDalant,
    unsigned int dwEmblemBack,
    unsigned int dwEmblemMark);
  bool Delete_Guild(unsigned int dwGuildSerial);
  unsigned __int8 Select_CharNumInWorld(unsigned int dwAccountSerial, unsigned __int8 *byCharNum);
  bool Select_ArrangeInfo(unsigned int dwSerial);
  bool Update_SetActive(unsigned int dwSerial, char *pwszName, unsigned __int8 bySlotIndex);
  bool Update_CharSlot(unsigned int dwAvatorSerial);
  bool InsertChangeClassLogAfterInitClass(
    unsigned int dwCharacSerial,
    unsigned __int8 byType,
    char *szPrevClass,
    char *szNextClass,
    int nClassInitCnt,
    unsigned __int8 byLastClassGrade,
    unsigned __int16 dwYear,
    unsigned __int8 byMonth,
    unsigned __int8 byDay,
    unsigned __int8 byHour,
    unsigned __int8 byMin,
    unsigned __int8 bySec);
  bool Select_ChracterSerialRace(char *pwszCharName, unsigned int *pCharSerial, unsigned __int8 *pRace);
  bool Update_GmGreet(_qry_case_gm_greetingmsg *pSheet);
  bool Update_RaceGreet(_qry_case_race_greetingmsg *pSheet);
  bool Update_GuildGreet(_qry_case_guild_greetingmsg *pSheet);
  unsigned __int8 Select_PostStorageList(unsigned int dwSerial, _post_storage_list *pListData);
  unsigned __int8 Select_ReturnPost(unsigned int dwMax, unsigned int dwMasterSerial, _return_post_list *pRetData);
  unsigned __int8 Select_PostContent(unsigned int dwSerial, char *pszContent, int nMaxContent);
  char Update_CharacterData(unsigned int dwSerial, _worlddb_update_char_query *pUpdateQuery);
  bool Update_IncreaseWeeklyGuildKillPvpPointSum(unsigned int dwSerial, long double dPvpPoint);
  bool Update_CristalBattleCharInfo(
    unsigned int dwSerial,
    unsigned __int8 byHSKTime,
    unsigned __int8 byPvpGrade,
    int iPvpPoint,
    unsigned __int16 wKillPoint,
    unsigned __int16 wDiePoint);
  bool update_cristalbattle_date(unsigned int dwCharSerial, unsigned __int8 bHSKTime);
  bool Insert_Supplement(unsigned int dwSerial);
  bool create_automine_table();
  bool create_sumtotal_dungeon(int nRecodeNum, char **ppKey);
  int select_automine(_DB_LOAD_AUTOMINE_MACHINE *pData);
  int exist_automine(unsigned __int8 byCollisionType, unsigned __int8 byRace);
  bool create_amine_personal();
  int exist_aminpersonal_inven(unsigned int dwSerial);
  int select_amine_personal(unsigned int dwSerial);
  int select_amine_personal(unsigned int dwSerial, _personal_amine_inven *pInven);
  bool insert_amine_personal(unsigned int dwSerial);
  bool update_amine_personal(char *pQry);
  bool update_amine_dck(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial);
  bool insert_amine_newowner(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial);
  bool update_amine_battery(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial, unsigned int dwBattery);
  bool update_amine_mineore(
    unsigned __int8 byType,
    unsigned __int8 byRace,
    unsigned int dwSerial,
    unsigned __int8 bySlot,
    unsigned int dwK,
    unsigned __int8 byNum,
    unsigned int dwGage);
  bool update_amine_workstate(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial, bool bWorking);
  bool update_amine_selore(unsigned __int8 byType, unsigned __int8 byRace, unsigned int dwSerial, unsigned __int8 byOreIdx);
  bool update_amine_moveore(
    unsigned __int8 byType,
    unsigned __int8 byRace,
    unsigned int dwSerial,
    unsigned __int8 bySSlot,
    unsigned int dwSK,
    unsigned __int8 bySNum,
    unsigned __int8 byDSlot,
    unsigned int dwDK,
    unsigned __int8 byDNum);
  bool Update_Post(char *szPostQuery);
  bool Select_PostStorageRecordCheck();
  bool Insert_PostStorageRecord();
  bool Select_PostStorageEmptyRecordSerial(unsigned int *pdwStorageSerial);
  bool Update_DisappearOwnerRecord();
  int Select_PostStorageEmptyRecord();
  bool Insert_PSDefaultRecord(unsigned int dwCum);
  unsigned __int8 Select_PostRegistryData(unsigned int dwMax, CPostData *pPostData);
  bool Update_PostStorageSendToRecver(
    unsigned int dwOwner,
    unsigned int dwPostSerial,
    unsigned __int8 byPostState,
    char *wszSendName,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    int nK,
    unsigned __int64 dwD,
    unsigned int dwU,
    unsigned int dwGold,
    unsigned __int8 byErr,
    unsigned __int16 wStorageIndex,
    unsigned __int8 *pbyNumber,
    bool bGetNumber,
    unsigned __int64 lnUID);
  bool Insert_Start_NpcQuest_History(
    unsigned int dwSerial,
    char *szQuestCode,
    unsigned __int8 byLevel,
    char *szTime,
    __int64 nEndTime);
  bool Update_Start_NpcQuest_History(
    unsigned int dwSerial,
    char *szQuestCode,
    unsigned __int8 byLevel,
    char *szTime,
    __int64 nEndTime);
  bool Update_Level(unsigned int dwSerial, unsigned __int8 byLv);
  bool Update_GuildMaster(unsigned int dwGuildSerial, unsigned int dwMasterSerial, unsigned __int8 byPrevGrade);
  bool Update_Dalant(unsigned int dwSerial, unsigned int dwDalant);
  bool Update_Gold(unsigned int dwSerial, unsigned int dwGold);
  int Select_PatriarchComm(unsigned int dwSerial, _patriarch_comm_list *pOutList);
  unsigned __int8 Delete_PatriarchComm(unsigned int dwSerial, char *pszDepDate);
  int Select_CashLimSale(_worlddb_cash_limited_sale *pLimitedSale);
  bool Insert_CashLimSale();
  unsigned __int8 Select_Limit_Run_Record();
  bool Insert_Set_Limit_Run(unsigned __int8 *pHash, int nSize);
  bool Update_Set_Limit_Run(unsigned __int8 *pHash, int nSize);
  int Select_RFEvent_ClassRefine(
    unsigned int dwAvatorSerial,
    unsigned __int8 *byRefinedCnt,
    unsigned int *dwRefineDate);
  bool Insert_RFEvent_ClassRefine(unsigned int dwAvatorSerial);
  bool Update_RFEvent_ClassRefine(
    unsigned int dwAvatorSerial,
    unsigned __int8 byRefinedCnt,
    unsigned int dwRefineDate);
  bool Insert_GuidRoom(unsigned int dwGuildSerial, unsigned __int8 byRoomType, unsigned __int8 byRace);
  bool Update_GuildRoom(unsigned int dwGuildSerial);
  unsigned __int8 Select_RegeAvator_For_Lobby_Logout(unsigned int dwAccountSerial, _rege_char_data *pRegeCharData);
  bool Update_MacroData(unsigned int dwSerial, _AIOC_A_MACRODATA *pMacro);
  bool Update_SFDelayInfo(unsigned int dwSerial, _worlddb_sf_delay_info *pSFDelay);

  bool Insert_NpcData(unsigned int dwSerial);
  bool Insert_NpcData(unsigned int dwSerial, unsigned int *pNpcData);
  unsigned __int8 Select_NpcData(unsigned int dwSerial, unsigned int *pNpcData);
  bool Update_NpcData(unsigned int dwSerial, unsigned int *pNpcData);
  bool Insert_AnimusData(unsigned int dwSerial, long double *pVal);
  unsigned __int8 Select_AnimusData(unsigned int dwSerial, unsigned __int8 byRace, long double *pVal);
  bool Update_AnimusData(unsigned int dwSerial, unsigned __int8 byRace, long double *pAnimusData);
  bool Insert_NpcLog(unsigned int dwSerial, char *wszName, unsigned int dwIndex, unsigned int dwOrgVal, unsigned int dwChgVal);
  bool Insert_AnimusLog(
    unsigned int dwSerial,
    char *wszName,
    unsigned int dwIndex,
    long double dOrgVal,
    long double dChgVal);
  bool Select_TrunkMoney(unsigned int dwSerial, long double *pVal);

  unsigned __int8 Select_UnitData(unsigned int dwSerial, long double *pUnitData);
  bool Insert_UnitData(unsigned int dwSerial, long double *pUnitData);
  bool Update_UnitData(unsigned int dwSerial, long double *pUnitData);
  bool Insert_UnitLog(
    unsigned int dwSerial,
    char *wszName,
    unsigned int dwIndex,
    long double dOrgVal,
    long double dChgVal);

  int Select_PlayerTimeLimitInfo(unsigned int dwAccountSerial, _worlddb_time_limit_info *pTimeLimiInfo);
  int Select_PlayerTimeLimitInfo(
    unsigned int dwAccountSerial,
    unsigned int *pdwFatigue,
    unsigned __int8 *pbyStatus);

  bool Insert_RenamePotionLog(unsigned int dwSerial, char *wszOldName, char *wszNewName);
  bool Insert_SettlementOwnerLog(
    unsigned __int8 byNth,
    unsigned __int8 byRace,
    unsigned int dwGuildSerial,
    const char *wszGuildName,
    unsigned __int16 wRank,
    unsigned __int8 byGrade,
    long double dKillPvpPoint,
    long double dGuildBattlePvpPoint,
    unsigned int dwSumLv);
  unsigned __int8 Select_UnmannedTraderSearchGroupTotalRowCount(
    unsigned __int8 byType,
    unsigned __int8 byRace,
    unsigned __int8 byClass1,
    unsigned __int8 byClass2,
    unsigned __int8 byClass3,
    unsigned int *dwCount);
  unsigned __int8 Select_UnmannedTraderSearchPageInfo(
    unsigned __int8 byType,
    unsigned __int8 byRace,
    unsigned __int8 byClass1,
    unsigned __int8 byClass2,
    unsigned __int8 byClass3,
    unsigned int dwMaxRowCount,
    unsigned int dwExcludeRowCount,
    const char *szSortQuery,
    _unmannedtrader_page_info *pkInfo);

  int SelectRowCountGuildBattleInfo();
  int SelectRowCountGuildBattleScheduleInfo();
  bool LoadGuildBattleInfo(unsigned int dwStartID, unsigned int dwRowCnt, _worlddb_guild_battle_info *pkInfo);
  unsigned __int8 LoadGuildBattleScheduleInfo(
    unsigned int uiStartListID,
    unsigned int uiScheduleUnitCnt,
    _worlddb_guild_battle_schedule_list *pkInfo);
  bool DeleteGuildBattleInfo();
  bool DeleteGuildBattleScheduleInfo();
  bool InsertGuildBattleDefaultRecord(unsigned int dwRowCnt);
  bool InsertGuildBattleScheduleDefaultRecord(
    unsigned int uiDayCnt,
    unsigned int uiMapCnt,
    unsigned __int8 byMaxHour,
    unsigned __int8 byUnitTimeCntPerTime);
  bool CreateGuildBattleRankTable(char *szDate);
  bool UpdateClearGuildBattleRank();
  bool UpdateClearGuildBattleInfo(unsigned int dwStartID, unsigned int dwEndID);
  bool UpdateWinGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore);
  bool UpdateLoseGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore);
  bool UpdateDrawGuildBattleResult(unsigned int dwGuildSerial, unsigned int dwScore);
  bool SelectGuildBattleRankRecord(unsigned int dwGuildSerial);
  bool InsertGuildBattleRankRecord(unsigned int dwGuildSerial);
  bool SelectGuildBattleRankList(unsigned __int8 byRace, _worlddb_guild_battle_rank_list *pkInfo);
  unsigned __int8 SelectGuildBattleScheduleInfoID(unsigned int dwID);
  bool SelectGuildBattleRerservedList(
    unsigned int uiStartSLID,
    unsigned int uiEndSLID,
    _worlddb_guild_battle_reserved_schedule_info *pkInfo);
  bool UpdateGuildBattleInfo(
    unsigned int dwID,
    unsigned int dwP1GuildSerial,
    unsigned int dwP2GuildSerial,
    unsigned int dwMapID,
    unsigned __int8 byNumber);
  bool UpdateGuildBattleScheduleInfo(
    unsigned int dwID,
    unsigned int dwSLID,
    unsigned __int8 byState,
    __int64 tStartTime,
    unsigned __int16 wTurmMin);
  bool UpdateClearGuildBattleScheduleInfo(unsigned int uiStartListID, unsigned int uiEndListID);
  bool Select_GuildBattleRecord(unsigned int dwSerial, _guildbattle_totalrecord *pkInfo);
  bool Insert_GuildBatlleResultLog(
    char *szStartTime,
    char *szEndTime,
    unsigned int dwRedSerial,
    char *wszRedName,
    unsigned int dwBlueSerial,
    char *wszBlueName,
    unsigned int dwRedScore,
    unsigned int dwBlueScore,
    unsigned int dwRedMaxJoinCnt,
    unsigned int dwBlueMaxJoinCnt,
    unsigned int dwRedGoalCntSum,
    unsigned int dwBlueGoalCntSum,
    unsigned int dwRedKillCntSum,
    unsigned int dwBlueKillCntSum,
    unsigned __int8 byBattleResult,
    unsigned int dwMaxGoalCharacSerial,
    char *wszMaxGoalCharacName,
    unsigned int dwMaxKillCharacSerial,
    char *wszMaxKillCharacName,
    unsigned __int8 byJoinLimit,
    unsigned int dwGuildBattleCostGold,
    char *szBattleMapCode);
  bool Insert_GuildBatlleResultLogBattelInfo(
    char *szStartTime,
    char *szEndTime,
    unsigned int dwRedSerial,
    char *wszRedName,
    unsigned int dwBlueSerial,
    char *wszBlueName,
    unsigned int dwRedScore,
    unsigned int dwBlueScore,
    unsigned int dwRedMaxJoinCnt,
    unsigned int dwBlueMaxJoinCnt,
    unsigned int dwRedGoalCntSum,
    unsigned int dwBlueGoalCntSum,
    unsigned int dwRedKillCntSum,
    unsigned int dwBlueKillCntSum,
    unsigned __int8 byBattleResult,
    unsigned int dwMaxGoalCharacSerial,
    char *wszMaxGoalCharacName,
    unsigned int dwMaxKillCharacSerial,
    char *wszMaxKillCharacName,
    unsigned __int8 byJoinLimit,
    unsigned int dwGuildBattleCostGold,
    char *szBattleMapCode);
  unsigned __int8 Select_BattleResultLogLatest(unsigned int *pkLogSerial);
  bool Update_BattleResultLogBattleResultAndPvpPoint(
    unsigned int kLogSerial,
    unsigned int dwRedSerial,
    unsigned int dwBlueSerial);

  unsigned __int8 Select_TotalGuildRank(char *szDate, _total_guild_rank_info *pkInfo);
  unsigned __int8 Select_PvpPointGuildRank(char *szDate, _pvppoint_guild_rank_info *pkInfo);
  bool Update_PvpPointGuildRankRecord(char *szDate, unsigned int dwSerial, unsigned __int16 wRank);
  bool Update_PvpPointGuildRankSumLv(
    char *szDate,
    unsigned __int8 byRace,
    unsigned __int8 byLimitCnt,
    unsigned __int8 byLimitGrade);
  unsigned __int8 Select_WeeklyGuildRankOwnerGuild(
    char *szDate,
    unsigned __int8 byRace,
    unsigned __int8 byLimitCnt,
    _weeklyguildrank_owner_info *pkInfo);
  bool Create_PvpPointGuildRankTable(char *szDate);
  bool Insert_PvpPointGuildRankData(char *szDate);
  bool Update_ClearWeeklyPvpPointSum();
  bool Update_IncreaseWeeklyGuildGuildBattlePvpPointSum(unsigned int dwSerial, long double dPvpPoint);

  bool Update_GuildRank(char *szDate);
  bool Update_RaceRank(char *szDate);
  bool Update_RaceRank_Step1(char *szDate);
  bool Update_RaceRank_Step2(char *szDate);
  bool Update_RaceRank_Step3(char *szDate);
  bool Update_RaceRank_Step4(char *szDate);
  bool Update_RaceRank_Step5(char *szDate);
  bool Update_RaceRank_Step6(char *szDate);
  bool Update_RaceRank_Step_6_1(char *szDate);
  bool Update_RaceRank_Step_6_2(char *szDate);
  bool Update_RaceRank_Step_6_3(char *szDate);
  bool Update_RaceRank_Step7(char *szDate);
  bool Update_RaceRank_Step8(char *szDate);
  bool Update_RaceRank_Step9(char *szDate);
  unsigned __int8 Select_PvpRankInfo(unsigned __int8 byRace, char *szDate, _PVP_RANK_DATA *rankData);

  bool Update_GuildRank_Step1(char *szDate);
  bool Update_GuildRank_Step2(char *szDate);
  bool Update_GuildRank_Step3(char *szDate);
  char Update_GuildRank_Step4(char *szDate);
  bool SelectAllGuildSerial(unsigned int *pdwCount, unsigned int *pdwSerial);
  bool SelectAllGuildSerialGrade(unsigned int *pdwCount, unsigned int *pdwSerial, unsigned __int8 *pbyGrade);
  bool Update_GuildGrade();

  unsigned __int8 Update_RankInGuild_Step1(unsigned int dwGuildSerial);
  bool Update_RankInGuild(unsigned int dwGuildSerial, _worlddb_rankinguild_info *pGuildMemberRankData);
  bool Update_RankInGuild_Step2(unsigned int dwGuildSerial);
  bool Update_RankInGuild_Step3(unsigned int dwGuildSerial);
  bool Update_RankInGuild_Step4(unsigned int dwGuildSerial);
  bool Update_RankInGuild_Step5(unsigned int dwGuildSerial, _worlddb_rankinguild_info *pGuildMemberRankData);
  bool Update_RankInGuild_Step6();
  bool Update_RankInGuild_Step7();
  bool Update_RankInGuild_Step8();
  bool Update_RankInGuild_Step9();

  bool Update_Punishment(char *szData);
  int Select_PunishmentCount(unsigned __int8 byType, unsigned int dwAvatorSerial, unsigned int *pdwCnt);

  int Select_GetCharSerialByNameRace(char *pwszName, unsigned __int8 byRace, unsigned int *pSerial);
  unsigned __int8 Select_GuildMasterLastConn(
    unsigned int dwSerial,
    unsigned long long dwLimitConnTime,
    unsigned long long *pdwLastConnTime);
  bool Select_CharacterReName(char *pwszName, unsigned int *pSerial);
  bool Update_CharacterReName(char *pwszName, unsigned int dwSerial);
  bool Rebirth_Base(unsigned int dwCharacterSerial, char *pwszName);

  int Updatet_Account_Vote_Available(unsigned int dwSerial, unsigned __int8 *byVoteEnable);
  bool Update_Player_Vote_Info(
    unsigned int dwSerial,
    unsigned int dwAccPlayTime,
    unsigned __int8 IsVote,
    unsigned __int8 VoteEnable,
    unsigned int dwScanerData);
  bool UpdateVotedReset_Cheat(unsigned int dwSerial);
  bool UpdateVotedReset_Supplement(unsigned int dwSerial);
  bool UpdateVotedReset_General(unsigned int dwSerial);
  bool UpdateServerResetToken(unsigned int dwToken, unsigned __int16 wProcType, unsigned int dwESerial);

  int Select_Player_Last_LogoutTime(unsigned int dwAccSerial, unsigned int *pdwLastLogoutTime);
  bool Insert_PlayerTimeLimitInfo(unsigned int dwAccountSerial);
  bool Update_Player_TimeLimit_Info(unsigned int dwAccSerial, unsigned int dwFatigue, unsigned __int8 wStatus);

  bool Insert_OreReset_Log(unsigned __int8 byType, int nLiveUsercnt, unsigned int dwOreRemain, unsigned int dwTAmount);
  unsigned __int8 Insert_PatriarchComm(unsigned int dwSerial, unsigned int dwDalant, char *pszDepDate);
  int Select_PatriarchCommCount(unsigned int dwSerial, char *pszDate, unsigned int *dwCnt);
  unsigned __int8 Update_PatriarchComm(unsigned int dwSerial, unsigned int dwDalant, char *pszDepDate);
  bool Insert_RaceBattleLog(_race_battle_log_info *pInfo);

  bool Select_CheckSumValue(unsigned int dwSerial, unsigned int *pdwCheckSum);
  unsigned __int8 Select_PvpRate(unsigned int dwSerial, char *szDate, unsigned int *pdwRank, unsigned __int16 *pwRankRate);

  char Select_ItemCharge(
    unsigned int dwAvatorSerial,
    unsigned __int8 *pbyType,
    unsigned int *pDwItemCode_K,
    unsigned __int64 *pDwItemCode_D,
    unsigned int *pDwItemCode_U,
    unsigned int *pDwItemChargeIndex,
    int *piTime);
  bool Select_AccountItemCharge(
    unsigned int dwAccountSerial,
    unsigned __int8 *pbyType,
    long double *pdMoney,
    unsigned int *pdwItemCode_K,
    unsigned __int64 *pdwItemCode_D,
    unsigned int *pdwItemCode_U,
    unsigned __int8 *pbyRace,
    unsigned int *pdwDBID,
    int *piTime);
  bool Select_AccountItemCharge_Extend(
    unsigned int dwAccountSerial,
    unsigned __int8 *pbyType,
    unsigned int *pdwItemCode_K,
    unsigned __int64 *pdwItemCode_D,
    unsigned int *pdwItemCode_U,
    unsigned __int8 *pbyRace,
    unsigned int *pdwDBID,
    int *piTime);
  bool Delete_ItemCharge(unsigned int dwItemChargeIndex);
  bool Delete_TrunkItemCharge(unsigned int dwDBID);
  bool Delete_TrunkItemCharge_Extend(unsigned int dwDBID);
  bool Insert_PatrirchItemChargeRefund(char *szData);
  unsigned __int8 Select_WaitItem(unsigned int dwAvatorSerial, _worlddb_item_list *itemList);
  unsigned __int8 Select_TakeItem(unsigned int dwAvatorSerial, _worlddb_item_list *itemList);

  unsigned __int8 Select_LimitItemUsedRecord(
    unsigned __int8 byType,
    unsigned int dwTypeSerial,
    unsigned int dwStoreInx,
    unsigned int *pdwSerial);
  unsigned __int8 Select_LimitItemEmptyRecord(unsigned int *pdwSerial);
  bool Insert_LimitItemRecord(unsigned int *pdwSerial);
  bool Update_LimitItemNum(char *pszQuery);
  bool Update_DisableInstanceStore(unsigned int dwSerial);
  bool create_table_atrade_taxrate();
  int select_atrade_taxrate(
    unsigned __int8 byRace,
    char *pwszName,
    unsigned __int8 *byCurrTax,
    unsigned __int8 *byNextTax);

  bool insert_atrade_taxrate(
    unsigned __int8 byRace,
    unsigned int dwSerial,
    char *wszName,
    unsigned int dwMatterDst,
    char *wszMatterDst,
    unsigned __int8 byCurrTax,
    unsigned int dwNext);

  int Select_HonorGuild(unsigned __int8 byRace, _guild_honor_list_result_zocl *pOutList, bool bNext);
  int Select_ClearHonorGuild(unsigned __int8 byRace, unsigned int *dwSerial);

  bool Select_CharacterName(unsigned int dwSerial, char *pwszCharacterName, char *szAccount);

  unsigned __int8 Select_UnmannedTraderReservedSchedule(
    unsigned int dwMaxCnt,
    _unmannedtrader_reserved_schedule_info *pkInfo);
  int Select_UnmannedTraderItemStateInfoCnt(unsigned int *pdwCnt);
  int Select_UnmannedTraderItemStateInfo(_unmannedtrader_stade_id_info *pkInfo, unsigned int dwMaxCnt);
  bool Truncate_UnmannedTraderItemStateRecord();
  bool Insert_UnmannedTraderItemStateRecord(unsigned int dwRowCnt, wchar_t **ppwszStr);
  bool Update_UnmannedTraderClearDanglingOwnerRecord();
  int Select_UnmannedTraderSingleItemEmptyRecordCnt();
  int Select_UnmannedTraderSingleItemEmptyRecordSerial(unsigned int *dwSerial);
  bool Select_UnmannedTraderSingleItemBottomSerial(unsigned int *dwSerial);
  bool Insert_UnmannedTraderSingleDefaultRecord(unsigned int dwRowCnt);
  unsigned __int8 Select_UnmannedTraderItemState(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 *byState);
  bool Update_UnmannedTraderItemState(
    unsigned __int8 byType,
    unsigned int dwItemSerial,
    unsigned __int8 byState,
    const _SYSTEMTIME *kCurTime);
  bool Update_UnmannedTraderSingleItemInfo(unsigned int dwRegSerial, const _unmannedtrader_registsingleitem *kInfo);
  bool Update_UnmannedTraderSellInfo(
    unsigned int dwRegSerial,
    const _unmannedtrader_registsingleitem *kInfo,
    const _SYSTEMTIME *kCurTime);
  bool Regist_UnmannedTraderSingleItem(
    unsigned int dwRegSerial,
    const _unmannedtrader_registsingleitem *kInfo,
    bool bInsertRecord);
  unsigned __int8 Select_UnmannedTraderBuySingleItemInfo(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    _unmannedtrader_buy_item_info *kData);
  bool Update_UnmannedTraderSellInfoPrice(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned int dwOwner,
    unsigned int dwNewPrice);
  unsigned __int8 Select_UnmannedTraderSellInfo(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 byRace,
    _unmannedtrader_seller_info *kData);
  unsigned __int8 Select_utSellWaitItems_SalesTotals(
    unsigned __int8 byType,
    unsigned int dwOwnor,
    unsigned int *pSalesTotals);
  bool Update_UnmannedTraderResutlInfo(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 byState,
    unsigned int dwBuyer,
    unsigned int dwTax,
    const _SYSTEMTIME *kCurTime);
  unsigned __int8 Select_UnmannedTraderRegister(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned int *pdwRegister);
  unsigned __int8 Update_UnmannedTraderReRegist(
    unsigned __int8 byType,
    unsigned int dwRegistSerial,
    unsigned __int8 byState,
    unsigned int dwPrice,
    unsigned int dwTax,
    const _SYSTEMTIME *kCurTime);
  bool Update_UnmannedTraderCheatUpdateRegistDate(unsigned __int8 byType, unsigned int dwRegistSerial);
  unsigned __int8 Select_UnmannedTraderItemRecordCntByState(
    unsigned __int8 byType,
    unsigned __int8 byState,
    unsigned int *pdwSerial,
    unsigned __int16 wMaxCnt,
    unsigned __int16 *pwRecordCnt);
  bool Update_UnmannedTraderSingleTypeClearUseCompleteRecord(unsigned int dwSerial, const _SYSTEMTIME *pkCurTime);

  bool Update_PostRegistry(
    unsigned int dwIndex,
    unsigned int dwSenderSerial,
    char *wszSendName,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    int nK,
    unsigned __int64 dwD,
    unsigned int dwU,
    unsigned int dwGold,
    unsigned __int8 bySendRace,
    unsigned __int8 bySenderDgr,
    unsigned __int64 lnUID);
  bool Update_PostRegistryDisable(unsigned int dwIndex);
  unsigned __int8 Select_PostRecvSerialFromName(
    char *wszRecvName,
    unsigned int *pdwOutSerial,
    unsigned int *pdwAccSerial,
    unsigned int *pdwRace);
  int Select_PostRecvStorageCheck(unsigned int dwSerial);
  int Select_IsValidChar(unsigned int dwSerial, unsigned int *dwDbSerial);
  unsigned __int8 Select_PatriarchCandidate(
    unsigned int dwSerial,
    unsigned __int8 byRace,
    CandidateMgr::_candidate_info *p);
  unsigned __int8 Select_PatriarchGroup(unsigned __int8 byRace, CandidateMgr::_candidate_info *p);
  int Select_OldVerPatriarchGroup(unsigned __int8 byRace, CandidateMgr::_candidate_info *p);
  int Select_PatriarchWinCnt(
    unsigned __int8 byRace,
    unsigned int dwAvatorSerial,
    unsigned int *dwWinCnt);
  int Select_PatriarchElectState(_sel_patriarch_elect_state *pSheet);
};

