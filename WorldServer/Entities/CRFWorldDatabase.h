#pragma once

#include "CRFNewDatabase.h"
#include "DqsDbStructs.h"
#include "unmannedtrader_page_info.h"

struct _AIOC_A_MACRODATA;
struct _patriarch_comm_list;
struct _post_storage_list;
struct _return_post_list;
struct _rege_char_data;
struct _qry_case_gm_greetingmsg;
struct _qry_case_guild_greetingmsg;
struct _qry_case_race_greetingmsg;
struct _worlddb_arrange_char_info;
struct _worlddb_cash_limited_sale;
struct _worlddb_character_base_info;
struct _worlddb_character_base_info_array;
struct _worlddb_economy_history_info;
struct _worlddb_guild_info;
struct _worlddb_sf_delay_info;
struct _worlddb_update_char_query;
struct _personal_amine_inven;

/* 1133 */
class __cppobj CRFWorldDatabase : public CRFNewDatabase
{
public:
  CRFWorldDatabase();
  virtual ~CRFWorldDatabase();

  unsigned __int8 Select_CharacterBaseInfoBySerial(
    unsigned int dwAccountSerial,
    _worlddb_character_base_info_array *pCharacterDataArray);
  char Select_CharacterBaseInfo(unsigned int dwCharacterSerial, _worlddb_character_base_info *pCharacterData);
  unsigned __int8 Select_NotArrangeCharacter(unsigned int dwAccountSerial, _worlddb_arrange_char_info *pCharData);
  char Select_Equal_Name(char *pwszCharacterName);
  char Select_Equal_DeleteName_NoArranged(char *pwszCharacterName);
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
  unsigned __int8 Select_Exist_Economy(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData);
  bool Insert_Economy_History(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData);
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
  bool Update_UserGuildData(unsigned int dwAvatorSerial, unsigned int dwGuildSerial, unsigned __int8 byGrade);
  bool Check_GuildMemberCount(unsigned int dwGuildSerial);
  bool Update_GuildMemberCount(unsigned int dwGuildSerial, int nMemberNum);
  bool Update_InputGuildMoney(
    unsigned int dwGuildSerial,
    signed int dwAddDalant,
    int dwAddGold);
  char Select_GuildData(unsigned int dwGuildSerial, _worlddb_guild_info::__guild_info *pGuildData);
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
  char Update_CharSlot(unsigned int dwAvatorSerial);
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
  unsigned __int8 select_amine_personal(unsigned int dwSerial);
  unsigned __int8 select_amine_personal(unsigned int dwSerial, _personal_amine_inven *pInven);
  bool insert_amine_personal(unsigned int dwSerial);
  bool Update_Post(char *szPostQuery);
  char Select_PostStorageRecordCheck();
  bool Insert_PostStorageRecord();
  char Select_PostStorageEmptyRecordSerial(unsigned int *pdwStorageSerial);
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
  bool Update_GuildMaster(unsigned int dwGuildSerial, unsigned int dwMasterSerial, unsigned __int8 byPrevGrade);
  bool Update_Dalant(unsigned int dwSerial, unsigned int dwDalant);
  bool Update_Gold(unsigned int dwSerial, unsigned int dwGold);
  unsigned __int8 Select_PatriarchComm(unsigned int dwSerial, _patriarch_comm_list *pOutList);
  bool Delete_PatriarchComm(unsigned int dwSerial, char *pszDepDate);
  unsigned __int8 Select_CashLimSale(_worlddb_cash_limited_sale *pLimitedSale);
  bool Insert_CashLimSale();
  unsigned __int8 Select_Limit_Run_Record();
  bool Insert_Set_Limit_Run(unsigned __int8 *pHash, int nSize);
  bool Update_Set_Limit_Run(unsigned __int8 *pHash, int nSize);
  unsigned __int8 Select_RFEvent_ClassRefine(
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
};

