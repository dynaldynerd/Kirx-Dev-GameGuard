#pragma once

#include "IdaCompat.h"
#include "GlobalObjectDefs.h"
#include "CPlayer.h"

#pragma pack(push, 1)
#include <utility>
#include <vector>

struct _RTL_CRITICAL_SECTION_DEBUG;
class CRFWorldDatabase;
struct _PCBANG_PLAY_TIME;
struct _REGED;
struct _REGED_AVATOR_DB;
struct _NOT_ARRANGED_AVATOR_DB;
struct qry_case_cash_limsale;
struct qry_case_select_golden_box_item;
struct _db_cash_limited_sale;
struct _SUPPLEMENT_DB_BASE;
struct _TIMELIMITINFO_DB_BASE;
struct _INVEN_DB_BASE;
struct _LINK_DB_BASE;
struct _BUDDY_DB_BASE;
struct _TRUNK_DB_BASE;
struct _TRADE_DB_BASE;
struct _AIOC_A_MACRODATA;
struct _economy_history_data;
struct _CRYMSG_DB_BASE;
struct _worlddb_sf_delay_info;
struct _POTION_NEXT_USE_TIME_DB_BASE;
struct _CUTTING_DB_BASE;
struct _PCBANG_FAVOR_ITEM_DB_BASE;
class CEventLootTable;
class CItemLootTable;
class COreCuttingTable;
struct _monster_sp_fld;
struct _skill_fld;
struct _force_fld;
struct _monster_sp_group;
struct RFEventBase;
struct _event_participant_classrefine;
class CGameObject;
class CPlayer;
class CMapData;
struct _BSP_C_FACE;
struct _BSP_MAT_GROUP;
struct _SCALE_TRACK;
struct _POS_TRACK;
struct _ROT_TRACK;
struct _ANI_OBJECT;
struct _R3MATERIAL;
struct _GUID;
struct IDirect3DVertexBuffer8;
struct IDirect3DIndexBuffer8;
struct _D3DR3VERTEX_TEX1;
struct _D3DR3VERTEX_TEX2;
struct _ENTITY_M_GROUP;
class CEntity;
struct _PARTICLE_ELEMENT;
class CParticle;
struct _MERGE_FILE;
class CMergeFile;
struct _ENTITY_LIST;
struct _LEAF_ENTITIES_LIST_INFO;
struct _MAP_ENTITIES_LIST;
struct _SOUND_ENTITY_LIST;
struct _SOUND_ENTITIES_LIST;
struct _LEAF_SOUND_ENTITIES_LIST_INFO;
struct _BSP_NODE;
struct _BSP_LEAF;
struct _TOOL_COL_LINE;
struct _TOOL_COL_LEAF;
struct _EXT_DUMMY;
class CBsp;
class CSkyBox;
struct _ANI_CAMERA_EXT;
struct _ANI_CAMERA;
class CObjectList;
struct _mon_active_fld;
struct _mon_block_fld;
struct _dummy_position;
struct _mon_block;
struct INI_Section;
struct INI_Key;
struct BossSchedule;
class CBossMonsterScheduleSystem;
class CMapOperation;
struct _map_fld;
struct BossSchedule_Map;
struct BossSchedule_TBL;
struct ScheduleMSG;
struct _MULTI_BLOCK;
struct _LAYER_SET;
struct _portal_fld;
struct _portal_dummy;
struct _store_dummy;
struct _start_dummy;
struct _bind_dummy;
struct _res_dummy;
struct _quest_dummy;
struct _safe_dummy;
class CUserDB;
struct _PVPPOINT_LIMIT_DB_BASE;
struct _PVP_ORDER_VIEW_DB_BASE;
class CPartyPlayer;
class CDarkHoleChannel;
class CDarkHole;
struct _dh_quest_setup;
struct _dh_mission_setup;
struct _monster_fld;
struct __monster_group;
struct __dummy_block;
struct __add_monster;
struct __add_loot_item;
struct __change_monster;
struct __inner_check;
struct __respond_check;
struct __respawn_monster;
struct __add_time;
struct _dh_job_setup;
struct _react_sub_setup;
struct __dp_mission_potal;
class CMonster;
struct _event_respawn;
struct _event_set;
struct UsStateTBL;
class CLuaCommandEx;
struct AP_BatterySlot;
class AutominePersonal;
struct _class_fld;
class CGuild;
struct _guild_member_info;
struct _guild_applier_info;
struct _guild_member_download_zocl;
struct _guild_applier_download_zocl;
struct _guild_query_info_result_zocl;
struct _guild_money_io_download_zocl;
struct _guild_member_buddy_download_zocl;
class CGuardTower;
class CTrap;
struct _happen_event_node;
class CParkingUnit;
struct _qry_case_sendwebracebosssms;
struct _qry_case_gm_greetingmsg;
struct _qry_case_race_greetingmsg;
struct _qry_case_guild_greetingmsg;
struct _qry_case_guildroom_insert;
struct _qry_case_guildroom_update;
struct _qry_case_update_guildmaster;
struct _qry_case_lobby_logout;
struct _db_golden_box_item;
struct _worlddb_economy_history_info;
struct _animus_fld;
class CAnimus;
struct TimeLimitMgr;
struct _mob_message;

/* 1258 */
struct __cppobj _SRAND
{
};

/* 1259 */
struct _GLBID
{
  unsigned int dwIndex;
  unsigned int dwSerial;
};

/* 1260 */
struct __cppobj __declspec(align(2)) _BILLING_INFO
{
  _BILLING_INFO();

  __int16 iType;
  char szCMS[7];
  int lRemainTime;
  _SYSTEMTIME stEndDate;
  bool bPCCheat;
  bool bIsPcBang;
  bool bAgeLimit;

  bool IsPcBangType() const
  {
    return bIsPcBang;
  }
};

/* 1261 */
struct __cppobj _WAIT_ENTER_ACCOUNT
{
  _WAIT_ENTER_ACCOUNT();
  void Release();
  void SetData(
    unsigned int dwAccountSerial,
    char *pszAccountID,
    unsigned __int8 byUserDgr,
    unsigned __int8 bySubDgr,
    _GLBID *pgidGlobal,
    unsigned int *pdwKey,
    bool bChatLock);
  void SetBillingInfo(__int16 iType, char *szCMS, int lRemainTime, _SYSTEMTIME *pstEndDate);
  void SetPcBangFlag(bool bIsPcBang);
  void SetTransFlag(int nTrans);
  void SetAgeLimitFlag(bool bAgeLimit);
  void SetUILock(
    unsigned __int8 byUILock,
    char *szUILockPW,
    unsigned __int8 byUILockFailCnt,
    char *szAccountPW,
    unsigned __int8 byHintIndex,
    char *uszHintAnswer,
    unsigned __int8 byUILockFindPassFailCount);

  bool m_bLoad;
  unsigned int m_dwAccountSerial;
  char m_szAccountID[13];
  unsigned __int8 m_byUserDgr;
  unsigned __int8 m_bySubDgr;
  __unaligned __declspec(align(1)) _GLBID m_gidGlobal;
  unsigned int m_dwKey[4];
  unsigned int m_dwLoadTime;
  bool m_bChatLock;
  int m_nTrans;
  bool m_bAgeLimit;
  unsigned __int8 m_byUILock;
  char m_szUILock_PW[13];
  unsigned __int8 m_byUILock_FailCnt;
  char m_szAccount_PW[13];
  unsigned __int8 m_byUILock_HintIndex;
  char m_uszUILock_HintAnswer[17];
  unsigned __int8 m_byUILockFindPassFailCount;
  unsigned int m_dwRequestMoveCharacterSerialList[3];
  unsigned int m_dwTournamentCharacterSerialList[3];
  _BILLING_INFO m_BillingInfo;
};

/* 1262 */
#include "CFrameRate.h"

/* 1265 */
struct __cppobj _message
{
  _message();
  ~_message();
  void SetMsg(unsigned int message, unsigned int key1, unsigned int key2, unsigned int key3);
  unsigned int GetMessageA() const;
  unsigned int GetKey1() const;

  unsigned int dwMessage;
  unsigned int dwKey1;
  unsigned int dwKey2;
  unsigned int dwKey3;
  _message *pNext;
  _message *pPrev;
};

/* 1267 */
#include "CMyCriticalSection.h"

/* 1264 */
#include "CMsgData.h"

/* 1269 */
#include "CMsgProcess.h"

/* 1272 */
struct _USER_NUM_SHEET
{
  int nAveragePerHour;
  int nMaxPerHour;
  int nCount;
};

/* 1273 */
#include "CConnNumPHMgr.h"

/* 1277 */
struct __cppobj __declspec(align(4)) _DB_QRY_SYN_DATA
{
  bool m_bUse;
  bool m_bLoad;
  unsigned __int16 m_wAlignPad;
  unsigned int m_dwAccountSerial;
  _CLID m_idWorld;
  unsigned __int8 m_byQryCase;
  unsigned __int8 m_byResult;
  char m_sData[74530];
};

/* 1280 */
#include "CCheckSum.h"

/* 1283 */
#include "RecordDataTypes.h"

/* 1281 */
#include "CRecordData.h"

/* 1284 */
#include "CItemLootTable.h"

/* 1290 */
#include "COreCuttingTable.h"

/* 1294 */
#include "CItemUpgradeTable.h"

/* 1304 */
#include "CMonsterSPGroupTable.h"

/* 1132 */
#include "CLogFile.h"

/* 1806 */
struct _notify_race_leader_s_owner_u_taxrate
{
  char wszRaceLeaderName[9][17];
  char wszSettlement1OwnerGuildName[17];
  char wszSettlement1OwnerGuildMasterName[17];
  char wszSettlement2OwnerGuildName[17];
  char wszSettlement2OwnerGuildMasterName[17];
  unsigned __int8 byTaxRate;
};

/* 1807 */
#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"

/* 1248 */
struct __cppobj __declspec(align(4)) TournamentWinner
{
  unsigned int dwSerial;
  char wszCharName[17];
  unsigned __int8 byGrade;
};

/* 1808 */
#include "CBattleTournamentInfo.h"

/* 1809 */
struct _event_info
{
  bool bEnable;
  unsigned int dwStartDate;
  unsigned int dwEndDate;
};

/* 1811 */
struct __cppobj GuildCreateEventInfo
{
  void Init();
  void Loop();
  void ReadEventInfo();
  char CheckEventDate();
  char ApplyModifiedGuildEventInfo();
  void SetConsumeDalantFree(bool bEnable);
  __int64 GetEstConsumeDalant() const;
  __int64 GetEmblemDalant() const;

  bool m_bStartedEvent;
  _event_info m_EventInfo;
  _event_info m_ModifyInfo;
  unsigned int m_dwEstConsumeDalant;
  unsigned int m_dwEmblemDalant;
  CMyTimer m_tmDataFileCheckTime;
  _FILETIME m_ftWrite;
};

/* 1812 */
struct __declspec(align(4)) _server_rate_ini_data
{
  float ItemRootRate;
  float MineSpeedRate;
  float ForceLiverAccumRate;
  float MasteryGetRate;
  float AnimusExpRate;
  float PlayerExpRate;
  float DarkHoleRewardRate;
  float PlayerLostExp;
  float PremiumMiningSpeed;
  float PremiumPlayerExp;
  float PremiumAnimusExp;
  float PremiumBasseMastery;
  float PremiumSkillForceMastery;
  float PremiumItemDrop;
  float PremiumPvpPointRate;
  float PremiumPlayerLostExp;
  unsigned __int8 byBindHQ;
  unsigned __int8 bySetteMineElanMap;
  unsigned __int8 byScrollItem;
  unsigned __int8 byCashItem;
  unsigned __int8 byAddCharacter;
};

/* 1813 */
struct __cppobj _server_rate_realtime_load
{
  CMyTimer m_tmDataFileCheckTime;
  _FILETIME m_ftWrite;
  _server_rate_ini_data m_IniData;
};

/* 1305 */
class __cppobj __declspec(align(8)) CMainThread
{
public:
  _SRAND m_Rand;
  _WAIT_ENTER_ACCOUNT m_WaitEnterAccount[2532];
  CRFWorldDatabase *m_pWorldDB;
  CFrameRate m_MainFrameRate;
  CFrameRate m_DBFrameRate;
  CMsgProcess m_GameMsg;
  CConnNumPHMgr m_MgrConnNum;
  CConnNumPHMgr m_HisMainFPS;
  CConnNumPHMgr m_HisSendFPS;
  CConnNumPHMgr m_HisDataFPS;
  CMyTimer m_tmServerState;
  bool m_bVerCheck;
  CMyTimer m_tmEconomyState;
  int m_tmDbUpdate;
  _DB_QRY_SYN_DATA m_DBQrySynData[12660];
  CNetIndexList m_listDQSData;
  CNetIndexList m_listDQSDataComplete;
  CNetIndexList m_listDQSDataEmpty;
  CCheckSum m_CheckSum;
  int m_nLimUserNum;
  char m_szWorldName[33];
  char m_wszWorldName[33];
  char m_wszMainGreetingMsg[256];
  char m_wszRaceGreetingMsg[3][256];
  char m_wszGMName[17];
  char m_wszBossName[3][17];
  bool m_bAwayPartyConsumeItem;
  char m_strAwayPartyItemCode[64];
  bool m_bAwayPartyConsumeMoney;
  unsigned int m_dwAwayPartyMoney;
  char m_strAllRaceChatItemCode[64];
  bool m_bAllRaceChatItemConsume;
  bool m_bAllRaceChatMoneyConsume;
  unsigned int m_dwAllRaceChatMoney;
  unsigned __int8 m_byWorldCode;
  bool m_bWorldOpen;
  bool m_bWorldService;
  char m_szWorldDBName[64];
  unsigned int m_dwMessengerIP;
  unsigned int m_dwAccountIP;
  unsigned int m_dwCheckAccountOldTick;
  CMyTimer m_tmrCheckAvator;
  CMyTimer m_tmrCheckLoop;
  CMyTimer m_tmrAccountPing;
  CMyTimer m_tmrStateMsgGotoWeb;
  CMyTimer m_tmrCheckRadarDelay;
  int m_bFreeServer;
  bool m_bRuleThread;
  bool m_bDQSThread;
  CRecordData m_tblPlayer;
  CRecordData m_tblMonster;
  CRecordData m_tblNPC;
  CRecordData m_tblAnimus;
  CRecordData m_tblClass;
  CRecordData m_tblExp;
  CRecordData m_tblGrade;
  CItemLootTable m_tblItemLoot;
  COreCuttingTable m_tblOreCutting;
  CRecordData m_tblItemMakeData;
  CRecordData m_tblItemCombineData;
  CRecordData m_tblItemExchangeData;
  CItemUpgradeTable m_tblItemUpgrade;
  CRecordData m_tblItemData[37];
  CRecordData m_tblEffectData[4];
  CRecordData m_tblUnitPart[6];
  CRecordData m_tblUnitBullet;
  CRecordData m_tblUnitFrame;
  CRecordData m_tblEditData;
  CRecordData m_MonsterBaseSPData;
  CMonsterSPGroupTable m_MonsterSPGroupTable;
  CLogFile m_logBillCheck;
  CLogFile m_logSystemError;
  CLogFile m_logLoadingError;
  CLogFile m_logDungeon;
  CLogFile m_logKillMon;
  CLogFile m_logServerState;
  CLogFile m_logDTrade;
  CLogFile m_logGuild;
  CLogFile m_logDQS;
  CLogFile m_logRename;
  CLogFile m_logAutoTrade;
  CLogFile m_logEvent;
  CLogFile m_logMove;
  CLogFile m_logSave;
  CLogFile m_logReturnGate;
  CLogFile m_logHack;
  CLogFile m_logPvP;
  CLogFile m_logMonNum;
  CMyTimer m_tmForceUserExit;
  int m_nForceExitSocketIndexOffset;
  bool m_bServerClosing;
  bool m_bCheckOverTickCount;
  int m_nSleepTerm;
  int m_nSleepValue;
  int m_nSleepIgnore;
  bool m_bCheckSumActive;
  unsigned __int8 m_byWebAgentServerNetInx;
  bool m_bConnectedWebAgentServer;
  unsigned __int8 m_byControllServerNetInx;
  bool m_bConnectedControllServer;
  int m_iOldDay;
  bool m_bServiceKeyPass;
  unsigned __int8 m_byWorldType;
  bool m_bReleaseServiceMode;
  bool m_bExcuteService;
  RFEventBase *m_pRFEvent_ClassRefine;
  CNotifyNotifyRaceLeaderSownerUTaxrate m_kEtcNotifyInfo;
  CBattleTournamentInfo m_BattleTournamentInfo;
  GuildCreateEventInfo m_GuildCreateEventInfo;
  _server_rate_realtime_load m_ServerRateLoad;
  TimeLimitMgr *m_pTimeLimitMgr;
  CMyTimer m_tmCheckForceClose;
  unsigned int m_dwStartNPCQuestCnt[3];
  _mob_message *m_MobMessage;
  unsigned int m_dwServerResetToken;
  unsigned int m_dwCheatSetPlayTime;
  unsigned __int16 m_dwCheatSetScanerCnt;
  unsigned __int16 m_dwCheatSetLevel;
public:
  static char ms_szClientVerCheck[33];

  CMainThread();
  bool Init();
  void AccountServerLogin();
  void gm_ServerClose();
  bool gm_MonsterInit(CMonster *pExt);
  bool IsTestServer() const;
  bool IsReleaseServiceMode() const;
  bool IsExcuteService() const;
  unsigned int GetMonsterRecordNum() const;
  _DB_QRY_SYN_DATA *PushDQSData(
    unsigned int dwAccountSerial,
    _CLID *pidWorld,
    unsigned __int8 byQryCase,
    char *pQryData,
    int nSize);
  bool Push_ChargeItem(
    unsigned int dwSerial,
    unsigned int dwK,
    unsigned int dwD,
    unsigned int dwU,
    unsigned __int8 byType);
  void OnDQSRun();
  void DQSCompleteProcess();
  CPlayer *GetCharW(char *wpszCharName);
  CGameObject *GetObjectA(_object_id *pObjID);
  CGameObject *GetObjectA(int kind, int id, unsigned int index);
  unsigned __int8 db_Reged_Avator(
    unsigned int dwAccountSerial,
    _REGED *pRegedList,
    _NOT_ARRANGED_AVATOR_DB *pArrangedList,
    const char *pszIP);
  unsigned __int8 db_Insert_Avator(
    unsigned int dwAccountSerial,
    char *pszAccount,
    _REGED_AVATOR_DB *pCharDB,
    unsigned int *pdwAvatorSerial);
  unsigned __int8 db_Delete_Avator(unsigned int dwSerial, unsigned __int8 byRaceCode);
  unsigned __int8 db_Load_Avator(
    unsigned int dwSerial,
    unsigned int dwAccountSerial,
    _AVATOR_DATA *pData,
    bool *pbAddItem,
    unsigned int *pdwAddDalant,
    unsigned int *pdwAddGold,
    bool *pbTrunkAddItem,
    unsigned __int8 *pbyTrunkOldSlot,
    long double *pdTrunkOldDalant,
    long double *pdTrunkOldGold,
    bool *pbCreateTrunkFree,
    bool *pbExtTrunkAddItem,
    unsigned __int8 *pbyExtTrunkOldSlot,
    bool bAll,
    unsigned int *pdwCheckSum);
  char db_Update_Avator(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    bool bCheckLowHigh);
  unsigned __int8 db_Update_PostStorage(
    unsigned int dwAvatorSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData);
  unsigned __int8 db_Log_AvatorLevel(unsigned int dwSerial, unsigned int dwAccountSerial, unsigned __int8 byLevel);
  unsigned __int8 db_Log_UserNum(unsigned int dwRace, unsigned int dwUserNum);
  bool db_Insert_Economy_History(unsigned int dwDate, _worlddb_economy_history_info *pEconomyData);
  unsigned __int8 db_Select_Economy_History(
    _economy_history_data *pCurData,
    unsigned int *pnCurMgrValue,
    unsigned int *pnNextMgrValue,
    _economy_history_data *pHisData,
    int *pHistoryNum,
    unsigned int dwDate);
  unsigned __int8 db_Insert_CharacSelect_Log(
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
  unsigned __int8 db_Update_PvpInfo(
    unsigned int dwSerial,
    unsigned __int8 byGrade,
    __int16 *pwKillPoint,
    long double dPvPPoint);
  unsigned __int8 db_Add_PvpPoint(
    unsigned int dwSerial,
    unsigned int dwAddPoint,
    unsigned int dwAddCashBag);
  unsigned __int8 db_Insert_Item(
    unsigned int dwSerial,
    unsigned int dwItemCodeK,
    unsigned int dwItemCodeD,
    unsigned int dwItemCodeU,
    unsigned __int8 byType);
  char db_Insert_guild(
    unsigned int *dwSerial,
    char *pwszGuildName,
    unsigned __int8 byRace,
    unsigned int *dwGuildSerial);
  unsigned __int8 db_update_guildmember_add(
    unsigned int dwGuildSerial,
    unsigned int dwCharSerial,
    unsigned __int8 byClass,
    unsigned int dwLevel);
  unsigned __int8 db_update_guildmember_del(
    unsigned int dwGuildSerial,
    unsigned int dwCharSerial,
    unsigned int dwGuildIndex);
  unsigned __int8 db_input_guild_money(
    unsigned int dwGuildSerial,
    unsigned int dwCharSerial,
    unsigned int dwAccountSerial,
    unsigned int dwParam,
    long double *dCurTotalDalant,
    long double *dCurTotalGold,
    unsigned __int8 *pbyDate,
    const char *pszPurpose);
  unsigned __int8 db_output_guild_money(
    unsigned int dwGuildSerial,
    unsigned int dwCharSerial,
    unsigned int dwAccountSerial,
    unsigned int dwParam,
    long double *dCurTotalDalant,
    long double *dCurTotalGold,
    unsigned __int8 *pbyDate,
    const char *pszPurpose,
    unsigned __int8 *pbyRetCode);
  unsigned __int8 db_buy_emblem(
    unsigned int dwGuildSerial,
    unsigned int dwCharSerial,
    unsigned int dwGuildIndex,
    unsigned int dwEmblemBack,
    unsigned int dwEmblemMark,
    long double *dCurTotalDalant,
    long double *dCurTotalGold,
    unsigned __int8 *pbyDate,
    char *pwszGuildName,
    unsigned __int8 *pbyRetCode);
  unsigned __int8 db_disjoint_guild(unsigned int dwGuildSerial);
  unsigned __int8 db_char_set_alive(
    unsigned int dwSerial,
    unsigned __int8 byRaceCode,
    unsigned int dwDalant,
    char *szReviveMapCode,
    unsigned __int8 byReviveStat,
    _REGED *pReged);
  unsigned __int8 db_Insert_ChangeClass_AfterInitClass(
    unsigned int dwCharacSerial,
    unsigned __int8 byType,
    char *szPrevClassCode,
    char *szNextClassCode,
    int nClassInitCnt,
    unsigned __int8 byLastClassGrade,
    unsigned __int16 dwYear,
    unsigned __int8 byMonth,
    unsigned __int8 byDay,
    unsigned __int8 byHour,
    unsigned __int8 byMin,
    unsigned __int8 bySec);
  unsigned __int8 db_sendwebracebosssms(_qry_case_sendwebracebosssms *pSheet);
  unsigned __int8 db_GM_GreetingMsg(_qry_case_gm_greetingmsg *pSheet);
  unsigned __int8 db_RACE_GreetingMsg(_qry_case_race_greetingmsg *pSheet);
  unsigned __int8 db_GUILD_GreetingMsg(_qry_case_guild_greetingmsg *pSheet);
  unsigned __int8 db_Load_PostStorage(char *pData);
  unsigned __int8 db_Load_ReturnPost(char *pData);
  unsigned __int8 db_Load_Content(char *pData);
  unsigned __int8 db_update_guildmaster(_qry_case_update_guildmaster *pSheet);
  unsigned __int8 db_input_guild_money_atradetax(
    unsigned int dwGuildSerial,
    unsigned int dwCharSerial,
    unsigned int dwAccountSerial,
    long double *dCurTotalDalant,
    long double *dCurTotalGold,
    unsigned __int8 *pbyDate);
  unsigned __int8 check_min_max_guild_money(unsigned int dwGuildSerial, long double *pdDalant, long double *pdGold);
  unsigned __int8 _db_load_event_classrefine(
    unsigned int dwAvatorSerial,
    unsigned __int8 *byRefinedCnt,
    unsigned int *dwRefineDate);
  unsigned __int8 _db_update_event_classrefine(
    unsigned __int16 wIndex,
    unsigned int dwSerial,
    unsigned __int8 byRefinedCnt,
    unsigned int dwRefineDate);
  unsigned __int8 _db_Update_SFDelayData(unsigned int dwSerial, _AVATOR_DATA *pData);
  unsigned __int8 _db_init_classrefine_count();
  unsigned __int8 _db_GuildRoom_Insert(_qry_case_guildroom_insert *pSheet);
  bool _db_GuildRoom_Update(_qry_case_guildroom_update *pSheet);
  unsigned __int8 _db_Load_PatriarchComm(char *pData);
  unsigned __int8 _db_Load_Cash_LimSale(qry_case_cash_limsale *pSheet);
  unsigned __int8 _db_Update_Cash_LimSale(_db_cash_limited_sale *pNewData, _db_cash_limited_sale *pOldData);
  unsigned __int8 _db_Update_Set_Limit_Run();
  unsigned __int8 _db_Update_GoldBoxItem(int nDBSerial, _db_golden_box_item *pNewData, _db_golden_box_item *pOldData);
  unsigned __int8 _db_Select_RegeAvator_For_Lobby_Logout(char *pData);
  unsigned __int8 _db_Update_Data_For_Post_Send(char *pSheet);
  unsigned __int8 _db_Update_Data_For_Trade(char *pSheet);
  unsigned __int8 _db_Load_Base(unsigned int dwSerial, _AVATOR_DATA *pData);
  unsigned __int8 _db_Load_General(unsigned int dwSerial, unsigned __int8 byRace, _AVATOR_DATA *pData);
  unsigned __int8 _db_Load_Supplement(unsigned int dwSerial, _SUPPLEMENT_DB_BASE *pSupplement);
  unsigned __int8 _db_Load_TimeLimitInfo(unsigned int dwAccSerial, _TIMELIMITINFO_DB_BASE *pTimeLimit);
  unsigned __int8 _db_Load_Inven(unsigned int dwSerial, unsigned __int8 byBagNum, _INVEN_DB_BASE *pInven);
  unsigned __int8 _db_Load_Unit(unsigned int dwSerial, _UNIT_DB_BASE *pUnit);
  unsigned __int8 _db_Load_UI(unsigned int dwSerial, _LINK_DB_BASE *pLink, _SFCONT_DB_BASE *pSfcont);
  unsigned __int8 _db_Load_Quest(unsigned int dwSerial, _QUEST_DB_BASE *pQuest);
  unsigned __int8 _db_Load_NpcQuest_History(unsigned int dwSerial, _QUEST_DB_BASE *pQuest);
  unsigned __int8 _db_Load_Start_NpcQuest_History(unsigned int dwSerial, unsigned __int8 byRace, _QUEST_DB_BASE *pQuest);
  unsigned __int8 _db_Load_Buddy(unsigned int dwSerial, _BUDDY_DB_BASE *pBuddy);
  unsigned __int8 _db_Load_Trunk(
    unsigned int dwSerial,
    unsigned int dwAccountSerial,
    unsigned __int8 byRace,
    _TRUNK_DB_BASE *pTrunk);
  unsigned __int8 _db_Load_Trade(unsigned __int8 byRace, unsigned int dwSerial, _TRADE_DB_BASE *pTrade);
  unsigned __int8 _db_Load_ItemCombineEx(unsigned int dwSerial, _ITEMCOMBINE_DB_BASE *pItemCombineEx);
  unsigned __int8 _db_Load_MacroData(unsigned int dwSerial, _AIOC_A_MACRODATA *pMacro);
  unsigned __int8 _db_Load_PvpPointLimitData(unsigned int dwSerial, _PVPPOINT_LIMIT_DB_BASE *pLimit);
  unsigned __int8 _db_Load_CryMsg(unsigned int dwSerial, _CRYMSG_DB_BASE *pCryMsg);
  unsigned __int8 _db_Load_PvpOrderView(unsigned int dwSerial, _PVP_ORDER_VIEW_DB_BASE *pOrderView);
  unsigned __int8 _db_Load_SFDelayData(unsigned int dwSerial, _worlddb_sf_delay_info *pSFDelay);
  unsigned __int8 _db_Load_PrimiumPlayTime(unsigned int dwAccSerial, _PCBANG_PLAY_TIME *pPlayTime);
  unsigned __int8 _db_Load_PotionDelay(unsigned int dwSerial, _POTION_NEXT_USE_TIME_DB_BASE *pPotionDelay);
  unsigned __int8 _db_Load_OreCutting(unsigned int dwSerial, _CUTTING_DB_BASE *pCutting);
  unsigned __int8 _db_Load_PcBangFavor(unsigned int dwSerial, _PCBANG_FAVOR_ITEM_DB_BASE *pFavorItem);
  bool db_LoadGreetingMsg();
  void CreateSelectCharacterLogTable(unsigned __int8 byMonth);
  void _db_Load_BattleTournamentInfo();
  char LoadLimitInfo();
  unsigned __int8 _db_Load_GoldBoxItem(qry_case_select_golden_box_item *pDbGoldenboxitem, int *pnDBSerial);
  unsigned __int8 _db_Check_NpcData(unsigned int dwSerial, _AVATOR_DATA *pData);
  unsigned __int8 _db_load_raceboss(unsigned int dwSerial, _AVATOR_DATA *pData);
  unsigned __int8 _db_load_punishment(unsigned int dwSerial, _AVATOR_DATA *pData);
  unsigned __int8 _db_load_losebattlecount(unsigned int dwSerial, _AVATOR_DATA *pData);
  char _db_Update_Base(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    bool bCheckLowHigh);
  char _db_Update_General(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    bool bCheckLowHigh);
  char _db_Update_Supplement(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    int nBufferSize);
  char _db_Update_Inven(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_Unit(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_UI(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_Quest(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_NpcQuest_History(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_Start_NpcQuest_History(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData);
  char _db_Update_Buddy(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_ItemCombineEx(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  bool _db_Update_MacroData(
    unsigned int dwSerial,
    _AIOC_A_MACRODATA *pNewData,
    _AIOC_A_MACRODATA *pOldData);
  char _db_Update_Trunk(
    unsigned int dwAccountSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_Trunk_Extend(
    unsigned int dwAccountSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  unsigned __int8 _db_Update_TimeLimitInfo(
    unsigned int dwAccountSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    int nBufferSize);
  char _db_update_inven_AMP(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_PvpPointLimit(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    char *pErrMsg);
  char _db_Update_CryMsg(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery);
  char _db_Update_PvpOrderView(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    char *pErrMsg);
  char _db_Update_PrimiumPlayTime(
    unsigned int dwAccSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    char *pErrMsg);
  char _db_Update_PotionDelay(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    int nBufferSize);
  char _db_Update_OreCutting(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    int nBufferSize);
  char _db_Update_PcBangFavor(
    unsigned int dwSerial,
    _AVATOR_DATA *pNewData,
    _AVATOR_DATA *pOldData,
    char *pSzQuery,
    int nBufferSize);
  void Reged_Avator_Complete(_DB_QRY_SYN_DATA *pData);
  void Insert_Avator_Complete(_DB_QRY_SYN_DATA *pData);
  void Delete_Avator_Complete(_DB_QRY_SYN_DATA *pData);
  void Select_Avator_Complete(_DB_QRY_SYN_DATA *pData);
  void Logout_Account_Complete(_DB_QRY_SYN_DATA *pData);
  void Lobby_Account_Complete(_DB_QRY_SYN_DATA *pData);
  void Cont_UserSave_Complete(_DB_QRY_SYN_DATA *pData);
  void Alive_Char_Complete(_DB_QRY_SYN_DATA *pData);
  void SendWebRaceBossSMS(_DB_QRY_SYN_DATA *pData);
  void AddGuildBattleSchdule(_DB_QRY_SYN_DATA *pData);
  void UpdateGuildBattleWinLoseRankInfo(_DB_QRY_SYN_DATA *pData);
  void UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA *pData);
  void UpdateLoadGuildBattleRank(_DB_QRY_SYN_DATA *pData);
  void InGuildbattleCost(_DB_QRY_SYN_DATA *pData);
  void OutSrcGuildbattleCost(_DB_QRY_SYN_DATA *pData);
  void OutDestGuildbattleCost(_DB_QRY_SYN_DATA *pData);
  void InGuildbattleRewardMoney(_DB_QRY_SYN_DATA *pData);
  void UpdateReservedGuildBattleSchedule(_DB_QRY_SYN_DATA *pData);
  void CompleteLoadGuildBattleTotalRecord(unsigned __int8 byRet, char *pLoadData);
  void InAtradTaxMoney(_DB_QRY_SYN_DATA *pData);
  void Load_PostStorage_Complete(char *pData);
  void Load_ReturnPost_Complete(char *pData);
  void Load_Content_Complete(char *pData);
  void CompleteUpdateVoteAvailable(char *pData);
  void CompleteUpdatePlayerVoteInfo(char *pData);
  void CompleteUpdateServerToken(char *pData);
  void CompleteUpdateSetLimitRun(char byRet, char *pData);
  void Complete_Select_RegeAvator_For_Lobby_Logout(_qry_case_lobby_logout *pSheet);
  void Complete_db_Update_Data_For_Post_Send(char *pSheet);
  void Complete_db_Update_Data_For_Trade(char *pSheet);
  void _db_complete_event_classrefine(
    unsigned __int16 wSock,
    unsigned int dwAvatorSerial,
    unsigned __int8 byRefinedCnt,
    unsigned int dwRefineDate);
  void _db_complete_update_event_classrefine(unsigned __int16 wSock, unsigned int dwAvatorSerial);
  void pc_OpenWorldSuccessResult(unsigned __int8 byWorldCode, char *pszDBName, char *pszDBIP);
  void pc_OpenWorldFailureResult(char *szMsg);
  void pc_ForceCloseCommand(_CLID *pidWorld, bool bDirectly, unsigned __int8 byKickType, unsigned int dwPushIP);
  void pc_TransIPKeyInform(
    unsigned int dwAccountSerial,
    char *pszAccountID,
    unsigned __int8 byUserDgr,
    unsigned __int8 bySubDgr,
    unsigned int *pdwKey,
    _GLBID *pgidGlobal,
    unsigned int dwClientIP,
    bool bChatLock,
    __int16 iType,
    char *szCMS,
    _SYSTEMTIME *pstEndDate,
    int lRemainTime,
    unsigned __int8 byUILock,
    char *szUILockPW,
    unsigned __int8 byUILockFailCnt,
    char *szAccountPW,
    unsigned __int8 byUILock_HintIndex,
    char *uszUILock_HintAnswer,
    unsigned __int8 byUILockFindPassFailCount,
    bool bIsPcBang,
    int nTrans,
    bool bAgeLimit,
    unsigned int *pdwRequestMoveCharacterSerialList,
    unsigned int *pdwTournamentCharacterSerialList);
  void pc_EnterWorldResult(unsigned __int8 byRetCode, _CLID *pidWorld);
  void pc_UILockInitResult(char *pMsg);
  void pc_UILockUpdateResult(char *pMsg);
  void pc_AllUserMsgInform(char *pwszMsg);
  void pc_AllUserGMNoticeInform(char *pwszMsg);
  void pc_SetMainGreetingMsg(char *pwszGMName, char *pwszMsg);
  void pc_SetRaceGreetingMsg(int racenum, char *pwszBossName, char *pwszMsg);
  void pc_AllUserKickInform();
  void pc_ChatLockCommand(_CLID *pidLocal, unsigned __int16 wBlockTimeH);
  void pc_AlterWorldService(bool bService);
  void pc_CashDBInfoRecvResult(
    char *szIP,
    char *szDBName,
    char *szAccount,
    char *szPassword,
    unsigned int dwPort);
  void pc_TaiwanBillingUserCertify(char *szAccount, unsigned __int8 byCertify);
  void ManageClientLimitRunRequest(char *pBuf);
  void EndServer();
  char _CheckTotalSales();
  bool DatabaseInit(char *pszDBName, char *pszDBIP);
  bool CashDBInit(char *szIP, char *szDBName, char *szAccount, char *szPassword, unsigned int dwPort);
  char _GameDataBaseInit();
  void SerivceSelfStart();
  void SerivceSelfStop();
  void SerivceForceSet(bool bService);
  unsigned int CreateDataResetToken(_SYSTEMTIME *tm);
  void PushResetServerToken();
  virtual ~CMainThread() = default;

private:
  void OnRun();
  void CheckAvatorState();
  void CheckAccountLineState();
  void ForceCloseUserInTiming();
  void CheckConnNumLog();
  void ContUserSaveJobCheck();
  void CheckDayChangedPvpPointClear();
  void CheckRadarItemDelay();
  void CheckServerRateINIFile();
  void CheckForceClose();
  void ServerStateMsgGotoWebAgent();
  void PingToAccount();
  char LoadServerRateINIFile();
  void SetServerRate();
  bool LoadINI();
  int LoadWorldSystemINI();
  int LoadWorldInfoINI();
  void LoadItemConsumeINI();
  bool CheckDefine();
  bool check_dbsyn_data_size();
  bool DataFileInit();
  bool SetGlobalDataName();
  bool check_loaded_data();
  void gm_MainThreadControl();
  bool ObjectInit();
  bool NetworkInit();
  void MakeSystemTower();
  void AddPassablePacket();
  static void __cdecl RuleThread(void *param);
  static void __cdecl DQSThread(void *param);
};

extern CMainThread g_Main;

/* 1306 */

/* 1126 */
#include "CRFNewDatabase.h"

/* 1133 */
#include "CRFWorldDatabase.h"

/* 1263 */

/* 1266 */

/* 1268 */

/* 1275 */

/* 1282 */

/* 1285 */

/* 1286 */
#include "CEventLootTable.h"

/* 1296 */
#include "base_fld.h"
#include "SkillCommon.h"
#include "monster_sp_fld.h"
#include "skill_fld.h"
#include "force_fld.h"
#include "class_fld.h"
#include "map_fld.h"
#include "animus_fld.h"
#include "mon_active_fld.h"
#include "mon_block_fld.h"
#include "portal_fld.h"
#include "monster_fld.h"

/* 1303 */
struct __cppobj __unaligned __declspec(align(2)) _monster_sp_group : _base_fld
{
  union MonsterSP_Fld
  {
    _skill_fld *m_SkillFld;
    _force_fld *m_ForceFld;
    _skill_fld *m_ClassSkillFld;
  };

  unsigned __int16 m_wCount;
  _monster_sp_fld *m_pSPData[15];
  MonsterSP_Fld m_SPDataFld[15];
};

/* 1805 */
struct event_date_range
{
  char StartDate;
  char EndDate;
};

/* 1307 */
struct __cppobj __declspec(align(4)) RFEventBase
{
  unsigned int _nOldLoopTime;
  event_date_range _kDateRange;
  virtual ~RFEventBase() = default;
  virtual void Loop();
  virtual unsigned __int8 DoEvent(CPlayer *pOne);
  virtual bool IsDbUpdate(unsigned int nIdx);
  virtual _event_participant_classrefine *GetPlayerState(unsigned int nIdx, unsigned int nAvator);
  virtual bool SetPlayerState(void *const p, int size);
};

/* 1814 */
struct __declspec(align(8)) Player_TL_Status
{
  bool m_bUse;
  bool m_bAgeLimit;
  unsigned __int8 m_byTL_Status;
  long double m_dPercent;
  unsigned int m_dwFatigue;
  unsigned int m_dwStartTime;
  unsigned int m_dwAccountSerial;
  unsigned int m_dwLastLogoutTime;
  bool m_bUpdateLogout;

  unsigned __int8 GetTLStatus();
};

/* 1815 */
struct __cppobj TimeLimitMgr
{
  static TimeLimitMgr *Instance();
  void LoadTLINIFile();
  void InitializeTLMgr();
  void Chack_Time();
  void SetTLEnable(unsigned __int16 wState);
  unsigned __int16 GetPeriodCnt();
  void SetTime(unsigned __int16 dwTime, unsigned __int16 iIndex);
  void ReInitFatigue();
  void SetPlayFDegree(unsigned int dwDegree);
  void SetLogoutFDegree(unsigned int dwDegree);
  long double GetPlayerPenalty(unsigned __int16 wIndex);
  unsigned __int8 GetPlayerStatus(unsigned __int16 wIndex);
  void InsertPlayerStatus(
    unsigned __int16 wIndex,
    unsigned int dwAccountSerial,
    unsigned __int8 byStatus,
    unsigned int dwFatigue,
    unsigned int dwLastLogoutTime,
    bool bAgeLimit);
  char CheckPlayerStatus(
    unsigned __int16 wIndex,
    unsigned int dwLastContSaveTime,
    unsigned __int8 *pbyStatus,
    unsigned int *pdwFatigue);
  void Pop_Data(unsigned int dwAccountSerial, unsigned __int16 wIndex);
  void Push_Data(Player_TL_Status *data, unsigned __int16 wIndex);
  __int64 ClacLastLogoutTimeSec(unsigned int dwLastConnTime);
  __int64 ClacLastLogoutTimeToFatigue(unsigned int dwLastConnTime);
  unsigned __int16 GetEndPlayTime();
  char UpdatePlayerStatus(unsigned __int16 wIndex, unsigned int dwFatigue, unsigned __int8 wStatus);
  void ReSetPercent(unsigned __int16 wIndex);
  __int64 SumMinuteBetweenSec(tm *tmLast);
  Player_TL_Status *Find_Data(unsigned __int16 wIndex);
  Player_TL_Status *Find_Data(unsigned int dwSerial);
  __int64 GetPlayFDegree();

  static unsigned int m_dwCnt;
  CMyTimer m_tmLoopTime;
  Player_TL_Status m_lstTLStaus[2532];
  unsigned __int16 m_wEnable;
  unsigned __int16 m_wPeriodCnt;
  unsigned int m_dwLogoutTerm;
  unsigned int m_dwNotifyTerm;
  unsigned __int16 *m_pwTime;
  unsigned __int16 *m_pwFatigue;
  long double *m_pdPercent;
  unsigned int m_dwPlayFDegree;
  unsigned int m_dwLogoutFDegree;
};

/* 1816 */
struct __declspec(align(2)) _mob_message
{
  unsigned __int16 wIndex;
  unsigned __int8 byUsingNum;
};

/* 1127 */

/* 1287 */

/* 1288 */

/* 1804 */

/* 1312 */

/* 1311 */

/* 1320 */
struct __cppobj D3DXMATRIX
{
  float m[4][4];
};

/* 1427 */
#include "CAniCamera.h"

/* 1428 */
#include "CTimer.h"

/* 1415 */
#include "CExtDummy.h"

/* 1315 */
#include "CLevel.h"

/* 1524 */
#include "CDummyPosTable.h"

/* 1526 */
struct _bsp_info
{
  char m_szBspName[50];
  int m_nMapMaxSize[3];
  int m_nMapMinSize[3];
  int m_nMapSize[3];
  int m_nLeafNum;
};

/* 1527 */
struct _sec_info
{
  int m_nSecNumW;
  int m_nSecNumH;
  int m_nSecNum;
};

/* 1313 */
#include "CMapData.h"

/* 1538 */
struct _EQUIPKEY
{
  __int16 zItemIndex;
  void SetRelease();
  bool IsFilled();
  void LoadDBKey(__int16 key);
  __int16 CovDBKey();
};

/* 1539 */
struct __cppobj __unaligned __declspec(align(1)) _REGED_AVATOR_DB
{
  char m_wszAvatorName[17];
  unsigned int m_dwRecordNum;
  unsigned __int8 m_byRaceSexCode;
  unsigned __int8 m_bySlotIndex;
  char m_szClassCode[5];
  unsigned __int8 m_byLevel;
  unsigned int m_dwDalant;
  unsigned int m_dwGold;
  unsigned int m_dwBaseShape;
  _EQUIPKEY m_EquipKey[8];
  unsigned __int8 m_byEquipLv[8];
  unsigned int m_dwLastConnTime;
  _REGED_AVATOR_DB();
  void Init();
};

/* 1540 */
struct __cppobj __unaligned __declspec(align(1)) _REGED : _REGED_AVATOR_DB
{
  unsigned int m_dwFixEquipLv[8];
  unsigned int m_dwItemETSerial[8];
  unsigned __int64 m_lnUID[8];
  unsigned __int8 m_byCsMethod[8];
  unsigned int m_dwET[8];
  unsigned int m_dwLendRegdTime[8];
  void init();
  void UpdateEquipLv();
  bool Set(unsigned __int8 bySlot, const _STORAGE_LIST::_db_con *pItem);
  bool Release(unsigned __int8 bySlot);
};

/* 1541 */
struct __cppobj __unaligned __declspec(align(1)) _AVATOR_DB_BASE : _REGED
{
  unsigned int m_dwHP;
  unsigned int m_dwFP;
  unsigned int m_dwSP;
  unsigned int m_dwDP;
  long double m_dExp;
  long double m_dLossExp;
  long double m_dPvPPoint;
  long double m_dPvPCashBag;
  unsigned int m_dwPvpRank;
  unsigned __int16 m_wRankRate;
  unsigned int m_dwRadarDelayTime;
  unsigned __int8 m_byBagNum;
  unsigned __int8 m_byMapCode;
  __int16 m_zClassHistory[3];
  unsigned int m_dwClassInitCnt;
  unsigned __int8 m_byLastClassGrade;
  float m_fStartPos[3];
  unsigned int m_dwTotalPlayMin;
  unsigned int m_dwStartPlayTime;
  char m_szBindMapCode[12];
  char m_szBindDummy[12];
  unsigned int m_dwGuildSerial;
  unsigned __int8 m_byClassInGuild;
  unsigned int m_dwGuildExplusDate;
  unsigned __int8 m_byGuildExplusApprovNum;
  unsigned __int8 m_byGuildExplusSeniorNum;
  unsigned int m_dwAccountSerial;
  bool m_bOverlapVote;
  unsigned int m_dwGivebackCount;
  unsigned int m_dwCashAmount;
  unsigned int m_dwTakeLastMentalTicket;
  unsigned int m_dwTakeLastCriTicket;
  unsigned __int8 m_byMaxLevel;
  unsigned int m_dwPunishment[3];
  unsigned int m_dwElectSerial[3];
  unsigned int m_dwRaceBattleRecord[3];
  long double m_dPvPPointLeak;
};

/* 1542 */
struct _LINKKEY
{
  unsigned __int16 wEffectCode;

  bool IsFilled();
  void SetRelease();
  void LoadDBKey(unsigned __int16 pl_zKey);
  unsigned __int16 GetCode();
  unsigned __int16 GetIndex();
  void SetData(unsigned __int16 wCode, unsigned __int16 wIndex);
  unsigned __int16 CovDBKey();
};

/* 1544 */
struct __cppobj __unaligned __declspec(align(1)) _LINK_DB_BASE
{
  struct _LIST
  {
    _LINKKEY Key;

    _LIST();
    void Init();
  };

  _LIST m_LinkList[50];
  unsigned __int8 m_byLinkBoardLock;
  unsigned int m_dwSkill[2];
  unsigned int m_dwForce[2];
  unsigned int m_dwCharacter[2];
  unsigned int m_dwAnimus[2];
  unsigned int m_dwInven;
  unsigned int m_dwInvenBag[5];

  _LINK_DB_BASE();
  void Init();
};

/* 1545 */
struct _EMBELLKEY
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;

  bool IsFilled();
  void SetRelease();
  void LoadDBKey(_EMBELLKEY key);
  __int64 CovDBKey();
};

/* 1547 */
struct __cppobj _EQUIP_DB_BASE
{
  struct _EMBELLISH_LIST
  {
    _EMBELLKEY Key;
    unsigned __int16 wAmount;
    unsigned int dwItemETSerial;
    unsigned __int64 lnUID;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
    unsigned int dwLendRegdTime;

    bool Set(const _STORAGE_LIST::_db_con *pItem);
    bool Release();
  };

  _EMBELLISH_LIST m_EmbellishList[7];
};

/* 1548 */
struct _FORCEKEY
{
  unsigned int dwKey;

  bool IsFilled();
  void SetRelease();
  void SetKey(unsigned __int8 byItemIndex, unsigned int dwStat);
  void SetStat(unsigned int pl_dwStat);
  unsigned __int8 GetIndex();
  unsigned int GetStat();
  __int64 CovDBKey();
};

/* 1550 */
struct __cppobj _FORCE_DB_BASE
{
  struct _LIST
  {
    _FORCEKEY Key;
    unsigned int dwItemETSerial;
    unsigned __int64 lnUID;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
    unsigned int m_dwLendRegdTime;

    bool Set(const _STORAGE_LIST::_db_con *pItem);
    bool Release();
  };

  _LIST m_List[88];
};

/* 1551 */
struct _ANIMUSKEY
{
  unsigned __int8 byItemIndex;

  bool IsFilled();
  void SetRelease();
  void LoadDBKey(unsigned __int8 key);
  unsigned __int8 CovDBKey();
};

/* 1553 */
struct __cppobj _ANIMUS_DB_BASE
{
  struct _LIST
  {
    _ANIMUSKEY Key;
    unsigned __int64 dwExp;
    unsigned int dwParam;
    unsigned int dwItemETSerial;
    unsigned __int64 lnUID;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
    unsigned int dwLendRegdTime;

    bool Set(const _STORAGE_LIST::_db_con *pItem);
    bool Release();
  };

  _LIST m_List[4];
};

/* 1556 */
struct __cppobj _INVEN_DB_BASE
{
  struct _LIST
  {
    _INVENKEY Key;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned int dwItemETSerial;
    unsigned __int64 lnUID;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
    unsigned int dwLendRegdTime;

    _LIST &operator=(const _LIST &rhs);

    bool Set(const _STORAGE_LIST::_db_con *pItem);
    bool Release();
  };

  _LIST m_List[100];

  _INVEN_DB_BASE &operator=(const _INVEN_DB_BASE &rhs);
};

/* 1558 */
struct __cppobj __unaligned __declspec(align(2)) _CUTTING_DB_BASE
{
  bool m_bOldDataLoad;
  unsigned __int8 m_byLeftNum;
  struct _LIST
  {
    _INVENKEY Key;
    unsigned int dwDur;

    void Init();
  };

  _LIST m_List[20];

  void Init();
  void ReSetOldDataLoad();
};

/* 1568 */
struct __cppobj _TRADE_DB_BASE
{
  struct _LIST
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

    bool IsEmpty();
  };

  _LIST m_List[20];
};

/* 1570 */
struct __cppobj _BUDDY_DB_BASE
{
  struct _LIST
  {
    unsigned int dwSerial;
    char wszName[17];

    _LIST();
    void Init();
    bool IsFilled();
  };

  _LIST m_List[50];

  _BUDDY_DB_BASE();
  void Init();
};

/* 1572 */
struct __cppobj __unaligned __declspec(align(1)) _TRUNK_DB_BASE
{
  struct _LIST
  {
    _INVENKEY Key;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned __int8 byRace;
    unsigned int dwItemETSerial;
    unsigned __int64 lnUID;
    unsigned __int8 byCsMethod;
    unsigned int dwT;
    unsigned int dwLendRegdTime;

    bool Set(const _STORAGE_LIST::_db_con *pItem, unsigned __int8 byRaceCode);
    bool Release();
  };

  char wszPasswd[13];
  long double dDalant;
  long double dGold;
  unsigned __int8 byHintIndex;
  char wszHintAnswer[17];
  unsigned __int8 bySlotNum;
  _LIST m_List[100];
  unsigned __int8 byExtSlotNum;
  _LIST m_ExtList[40];
};

/* 1190 */
struct _AIOC_A_MACRODATA
{
  struct MACRO_POTIONDATA
  {
    unsigned int Potion[3];
    unsigned int PotionValue[3];
  };

  struct MACRO_ACTIONDATA
  {
    unsigned int Action[10];
  };

  struct MACRO_CHATDATA
  {
    char Chat[5][256];
  };

  MACRO_POTIONDATA mcr_Potion[1];
  MACRO_ACTIONDATA mcr_Action[3];
  MACRO_CHATDATA mcr_Chat[2];
};

/* 1577 */
struct __cppobj _POSTSTORAGE_DB_BASE
{
  struct __list
  {
    unsigned int dwPSSerial;
    int nNumber;
    unsigned __int8 byState;
    int nKey;
    unsigned __int64 dwDur;
    unsigned int dwUpt;
    unsigned int dwGold;
    bool bUpdate;
    bool bRetProc;
    bool bNew;
    bool bUpdateIndex;
    char wszSendName[17];
    char wszRecvName[17];
    char wszTitle[21];
    char wszContent[201];
    unsigned __int64 lnUID;
  };

  __list m_PostList[50];
  bool m_bUpdate;

  void Init();
  void UpdateInit();
};

/* 1578 */
struct __cppobj __unaligned __declspec(align(1)) _RETURNPOST_DB_BASE
{
  bool m_bUpdate;
  int m_nMax;
  int m_nCum;
  unsigned int m_RetSerials[30];

  void Init();
};

/* 1580 */
struct __cppobj __unaligned __declspec(align(1)) _DELPOST_DB_BASE
{
  struct __list
  {
    unsigned int dwDelSerial;
    int nStorageIndex;
  };

  bool m_bUpdate;
  int m_nMax;
  int m_nCum;
  __list m_List[50];

  void Init();
};

/* 1581 */
struct __cppobj _POSTDATA_DB_BASE
{
  _POSTSTORAGE_DB_BASE dbPost;
  _RETURNPOST_DB_BASE dbRetPost;
  _DELPOST_DB_BASE dbDelPost;

  void Init();
  void UpdateInit();
};

/* 1583 */
struct __cppobj _CRYMSG_DB_BASE
{
  struct _LIST
  {
    char wszCryMsg[65];
  };

  _LIST m_List[10];
};

/* 1585 */
struct __cppobj __unaligned __declspec(align(1)) _PERSONALAMINE_INVEN_DB_BASE
{
  bool bUsable;
  struct _LIST
  {
    _INVENKEY Key;
    unsigned int dwDur;

    bool Set(const _STORAGE_LIST::_db_con *pItem);
    bool Release();
  };

  _LIST m_List[40];
};

/* 1586 */
struct __cppobj __unaligned __declspec(align(2)) _PVPPOINT_LIMIT_DB_BASE
{
  __int64 tUpdatedate;
  bool bUseUp;
  unsigned __int8 byLimitRate;
  long double dOriginalPoint;
  long double dLimitPoint;
  long double dUsePoint;
};

/* 1587 */
struct __unaligned __declspec(align(1)) _PVP_ORDER_VIEW_DB_BASE
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

/* 1237 */
struct __cppobj _worlddb_sf_delay_info
{
  struct _eff_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int16 wEffectIndex;
    unsigned int dwNextTime;
  };

  struct _mas_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int8 byMastery;
    unsigned int dwNextTime;
  };

  _eff_list EFF[10];
  _mas_list MAS[10];
};

/* 1588 */
struct __cppobj __unaligned __declspec(align(1)) _SUPPLEMENT_DB_BASE
{
  long double dPvpPointLeak;
  bool bLastAttBuff;
  unsigned int dwBufPotionEndTime;
  unsigned int dwRaceBuffClear;
  unsigned __int8 byVoted;
  unsigned __int8 VoteEnable;
  unsigned __int16 wScanerCnt;
  unsigned int dwScanerGetDate;
  unsigned int dwAccumPlayTime;
  unsigned int dwLastResetDate;
  unsigned int dwActionPoint[3];
};

/* 1244 */
struct __unaligned __declspec(align(1)) _PCBANG_PLAY_TIME
{
  unsigned int dwAccSerial;
  unsigned int dwLastConnTime;
  unsigned int dwContPlayTime;
  bool bForcedClose;
  unsigned __int8 byReceiveCoupon;
  unsigned __int8 byEnsureTime;
};

/* 1589 */
struct __cppobj _POTION_NEXT_USE_TIME_DB_BASE
{
  unsigned int dwPotionNextUseTime[38];
};

/* 1590 */
struct _PCBANG_FAVOR_ITEM_DB_BASE
{
  unsigned __int64 lnUID[50];

  void Init();
  char InsertItem(_STORAGE_LIST::_db_con *Item);
  char IsDeleteItem(_STORAGE_LIST::_db_con *Item);
};

/* 1591 */
struct __unaligned __declspec(align(1)) _TIMELIMITINFO_DB_BASE
{
  unsigned int dwAccSerial;
  unsigned int dwFatigue;
  unsigned __int8 byTLStatus;
  unsigned int dwLastLogoutTime;
};

/* 1592 */
struct __cppobj __unaligned __declspec(align(1)) _AVATOR_DATA
{
  _AVATOR_DB_BASE dbAvator;
  _LINK_DB_BASE dbLink;
  _EQUIP_DB_BASE dbEquip;
  _FORCE_DB_BASE dbForce;
  _ANIMUS_DB_BASE dbAnimus;
  _STAT_DB_BASE dbStat;
  _INVEN_DB_BASE dbInven;
  _CUTTING_DB_BASE dbCutting;
  _QUEST_DB_BASE dbQuest;
  _UNIT_DB_BASE dbUnit;
  _SFCONT_DB_BASE dbSfcont;
  _TRADE_DB_BASE dbTrade;
  _BUDDY_DB_BASE dbBuddy;
  _TRUNK_DB_BASE dbTrunk;
  _ITEMCOMBINE_DB_BASE dbItemCombineEx;
  _AIOC_A_MACRODATA dbMacro;
  _POSTDATA_DB_BASE dbPostData;
  _CRYMSG_DB_BASE dbBossCry;
  unsigned __int8 m_byHSKTime;
  unsigned __int8 m_byPvpGrade;
  unsigned __int16 m_wKillPoint;
  int m_iPvpPoint;
  unsigned __int16 m_wDiePoint;
  unsigned __int8 m_byCristalBattleDBInfo;
  _PERSONALAMINE_INVEN_DB_BASE dbPersonalAmineInven;
  _PVPPOINT_LIMIT_DB_BASE dbPvpPointLimit;
  _PVP_ORDER_VIEW_DB_BASE dbPvpOrderView;
  _worlddb_sf_delay_info dbSFDelay;
  _SUPPLEMENT_DB_BASE dbSupplement;
  _PCBANG_PLAY_TIME dbPlayTimeInPcbang;
  int m_bCristalBattleDateUpdate;
  _POTION_NEXT_USE_TIME_DB_BASE dbPotionNextUseTime;
  _PCBANG_FAVOR_ITEM_DB_BASE dbPcBangFavorItem;
  _TIMELIMITINFO_DB_BASE dbTimeLimitInfo;
  void InitData();
  void PostUpdateInit();
};

/* 1593 */
struct __cppobj _SYNC_STATE
{
  bool bEnter;
  bool bReged;
  bool bSelect;
  void Init();
  void re_lobby();
  char chk_enter();
  char chk_reged();
  char chk_select();
};

/* 1595 */
struct __cppobj _detected_char_list
{
  struct _char_info
  {
    unsigned __int8 m_byCharType;
    float m_fPos[2];
  };

  int nSize;
  _char_info CharInfoList[50];
};

/* 1596 */
#include "CRadarItemMgr.h"

/* 1536 */

/* 1597 */

/* 1561 */

/* 1707 */
struct __cppobj __unaligned __declspec(align(2)) _personal_amine_mineore_zocl
{
  struct __changed
  {
    unsigned __int16 wItemIndex;
    __unaligned __declspec(align(1)) unsigned int dwDur;
    unsigned __int16 wItemSerial;
  };

  unsigned int dwObjSerial;
  unsigned __int8 m_byUseBattery;
  unsigned int dwBattery;
  unsigned __int8 byChangedNum;
  __changed change[40];

  void clear();
};

/* 5884 */
struct __unaligned __declspec(align(2)) _personal_automine_uninstall_zocl
{
  struct __unaligned __declspec(align(2)) __battery
  {
    unsigned int dwDur;
    unsigned __int16 wSerial;
  };

  unsigned int dwObjSerial;
  unsigned __int8 byActType;
  __unaligned __declspec(align(1)) unsigned int dwOwnerSerial;
  __unaligned __declspec(align(1)) unsigned __int16 wItemSerial;
  unsigned __int8 byCnt;
  __battery battery[2];

  _personal_automine_uninstall_zocl();
  unsigned int size() const;
};

/* 5886 */
struct __cppobj __unaligned __declspec(align(1)) _personal_automine_uninstall_circle_zocl
{
  unsigned int dwObjSerial;
  unsigned __int8 byActType;

  _personal_automine_uninstall_circle_zocl();
  unsigned int size() const;
};

/* 5896 */
struct __cppobj __unaligned __declspec(align(1)) _personal_automine_current_state_zocl
{
  unsigned __int16 wItemSerial;
  unsigned __int8 byFilledSlotCnt;
  unsigned int dwBatteryGage;

  _personal_automine_current_state_zocl();
  unsigned int size() const;
};

/* 1709 */
struct __cppobj _qry_case_update_mineore
{
  struct __changed
  {
    unsigned __int8 bySlot;
    unsigned int dwK;
    unsigned __int8 byNum;
  };

  unsigned __int8 bySubQry;
  unsigned int dwMachineIndex;
  unsigned int dwAvatorSerial;
  unsigned __int8 byChangedNum;
  __changed change[40];
};

/* 1710 */
class __cppobj AutominePersonal : public CCharacter
{
public:
  AutominePersonal();
  ~AutominePersonal();
  __int64 GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  __int64 GetHP() override;
  __int64 GetMaxHP() override;
  __int64 GetObjRace() override;
  bool IsBeAttackedAble(bool bFirst) override;
  char IsBeDamagedAble(CCharacter *pAtter) override;
  void Loop() override;
  void SendMsg_FixPosition(int n) override;
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  bool initialize(unsigned __int16 wIndex);
  void LoadDBComplete();
  bool is_installed();
  bool regist_to_map(
    CPlayer *pOwner,
    _STORAGE_LIST::_db_con *pInstallItem,
    unsigned __int8 byDummyIndex,
    unsigned int dwObjSerial,
    float fDelayProf);
  unsigned int get_objserial();
  unsigned __int16 get_itemserial();
  _STORAGE_LIST::_db_con *get_item();
  CPlayer *get_owner();
  unsigned int get_ownerserial();
  CPlayer *GetOwner();
  int insert_battery(unsigned __int8 bySlotIdx, unsigned __int16 wItemSerial);
  bool unregist_from_map(unsigned __int8 byDestroyType);
  bool extract_battery(unsigned __int8 bySlotIdx, _STORAGE_LIST::_db_con *pBattery);
  void set_selore(unsigned __int8 bySelOre);
  void set_work(bool bWork);
  bool is_run();
  void sub_filledslot();
  void set_openUI_Inven(bool bFlag);
  void set_openUI_battery(bool bFlag);
  void send_changed_packet(unsigned int n);
  void send_installed();
  void send_ecode(unsigned __int8 byCode);
  void send_current_state();
  unsigned int get_battery();
  unsigned int get_battery(int n);
  void make_minepacket(
    unsigned __int16 wItemIndex,
    unsigned __int16 wItemSerial,
    unsigned __int8 byStorageIndex,
    unsigned __int16 nNewOre,
    unsigned int dwDur);
  void set_delay(unsigned int dwDelay);
  void set_delaysec(unsigned __int8 dwDS);
  void send_attacked();
  unsigned __int8 sub_battery(unsigned int dwUsed);
  bool do_automine(unsigned int dwTime);

  bool m_bDBLoad;
  bool m_bOpenUI_Inven;
  bool m_bOpenUI_Battery;
  bool m_bInstalled;
  bool m_bInvenFull;
  bool m_bStart;
  unsigned __int8 m_bySelOre;
  unsigned __int16 m_wItemSerial;
  unsigned __int8 m_byFilledSlotCnt;
  unsigned int m_dwNextSendTime_CurState;
  unsigned __int8 m_dwDelaySec;
  unsigned int m_dwDelay;
  unsigned int m_dwNextMineTime;
  unsigned int m_dwChangeSendTime;
  int m_nMaxHP;
  _STORAGE_LIST::_db_con *m_pItem;
  CPlayer *m_pOwner;
  unsigned __int8 m_byUseBattery;
  AP_BatterySlot *m_pBatterySlot;
  bool m_bChanged;
  __unaligned __declspec(align(1)) _personal_amine_mineore_zocl m_changed_packet;
  unsigned int m_dwMineCount[15];
  _qry_case_update_mineore m_update_mineore_old;
  _qry_case_update_mineore m_update_mineore_new;
  CLogFile m_logProcess;
  CLogFile m_logSysErr;
};

/* 1720 */
struct __cppobj _guild_master_info
{
  _guild_master_info();
  void init();
  bool IsFill();

  unsigned int dwSerial;
  unsigned __int8 byPrevGrade;
  _guild_member_info *pMember;
};

/* 1722 */
struct __cppobj _suggested_matter
{
  _suggested_matter();
  void Clear();
  bool IsVotable(unsigned int dwSerial);

  unsigned __int8 byMatterType;
  unsigned int dwMatterDst;
  char wszMatterDst[64];
  unsigned int dwMatterObj1;
  unsigned int dwMatterObj2;
  unsigned int dwMatterObj3;
  unsigned __int8 byVoteState[2];
  char wszComment[65];
  unsigned int dwMatterVoteSynKey;
  unsigned int dwStartTime;
  int nTotal_VotableMemNum;
  _guild_member_info *VotableMem[50];
};

/* 1723 */
struct __cppobj __declspec(align(8)) _guild_battle_suggest_matter
{
  enum STATE : __int32
  {
    WAIT_BATTLE_REQUEST = 0x0,
    BATTLE_REQUEST_SUGGEST = 0x1,
    COMPLETE_REQUEST = 0x2,
    APPLY_BATTLE_REQUEST_SUGGEST = 0x3,
    COMPLETE_BATTLE_REQUEST = 0x4,
    STATE_MAX = 0x5,
  };

  STATE eState;
  CGuild *pkSrc;
  CGuild *pkDest;
  unsigned int dwStartTime;
  unsigned int dwNumber;
  unsigned int dwMapIdx;

  _guild_battle_suggest_matter();
  void Clear();
};

/* 1729 */
struct __declspec(align(8)) _io_money_data
{
  char wszIOerName[17];
  unsigned int dwIOerSerial;
  long double dIODalant;
  long double dIOGold;
  long double dLeftDalant;
  long double dLeftGold;
  unsigned __int8 byDate[4];
};

/* 1717 */
#include "CGuild.h"

/* 1719 */
struct __cppobj __declspec(align(8)) _guild_member_info
{
  unsigned int dwSerial;
  char wszName[17];
  unsigned __int8 byClassInGuild;
  unsigned __int8 byLv;
  unsigned int dwPvpPoint;
  unsigned __int8 byRank;
  unsigned __int8 byGrade;
  CPlayer *pPlayer;
  bool bVote;

  _guild_member_info();
  void init();
  bool IsFill();
};

/* 1690 */

/* 1693 */

/* 1742 */
#include "CGuardTower.h"

/* 1746 */
#include "CTrap.h"

/* 1686 */
struct __cppobj _dh_mission_mgr
{
  struct _count
  {
    int nCount;
    bool bPass;

    void Init();
  };

  struct _if_change
  {
    _dh_mission_setup *pMissionPtr;
    char *pszDespt;
    char *pszComMsg;

    void Init();
    bool IsFill();
  };

  struct _respawn_monster_act
  {
    struct _monster_data
    {
      CMonster *pMon;
      unsigned int dwSerial;
    };

    __respawn_monster *pData;
    int nCum;
    unsigned int dwLastRespawnTime;
    _monster_data NowMonster[128];
    bool bStart;

    void set(__respawn_monster *data);
  };

  _dh_mission_setup *pCurMssionPtr;
  _count Count[8];
  bool bOpenPortal[128];
  unsigned int dwMissionStartTime;
  unsigned int dwMissionEndTime;
  int nAddLimMSecTime;
  bool bInnerCheck[64];
  _if_change IfCont[100];
  int nRespawnActNum;
  _respawn_monster_act RespawnMonsterAct[32];

  void Init();
  bool IsOpenPortal(int nIndex);
  void NextMission(_dh_mission_setup *pNextMssionPtr);
  _if_change *GetMissionCont(_dh_mission_setup *pMsSetup);
  _if_change *SearchCurMissionCont();
  void OpenPortal(int nIndex);
  __int64 GetLimMSecTime();
};

/* 1688 */
struct __cppobj __declspec(align(8)) _dh_player_mgr
{
  struct _pos
  {
    CMapData *pMap;
    unsigned __int16 wLayer;
    float fPos[3];

    void init();
  };

  CPlayer *pOne;
  unsigned int dwSerial;
  _pos LastPos;
  int nEnterOrder;

  _dh_player_mgr();
  void Init();
  bool IsFill();
};

/* 1598 */

/* 1435
struct __cppobj __declspec(align(4)) _dummy_position
{
  char m_szCode[64];
  unsigned __int16 m_wLineIndex;
  bool m_bPosAble;
  signed __int16 m_zLocalMin[3];
  signed __int16 m_zLocalMax[3];
  float m_fMin[3];
  float m_fMax[3];
  float m_fRT[3];
  float m_fLB[3];
  float m_fCenterPos[3];
  float m_fDirection[3];
  unsigned __int16 m_wActiveMon;
};
*/

/* 1773 */
#include "CParkingUnit.h"

/* 1776 */
#include "CAITimer.h"

/* 1777 */
struct __cppobj __declspec(align(8)) _attack_param
{
  CCharacter *pDst;
  int nPart;
  int nTol;
  int nClass;
  int nMinAF;
  int nMaxAF;
  int nMinSel;
  int nMaxSel;
  int nExtentRange;
  int nShotNum;
  int nAddAttPnt;
  int nWpType;
  unsigned __int8 byEffectCode;
  _base_fld *pFld;
  float fArea[3];
  int nLevel;
  int nMastery;
  bool bPassCount;
  int nAttactType;
  bool bMatchless;
  int nMaxAttackPnt;
  bool bBackAttack;
  int nMinAFPlus;
  int nMaxAFPlus;
  int nEffShotNum;

  _attack_param();
};

/* 1778 */
struct __cppobj SKILL
{
  int m_Type;
  int m_Element;
  int m_MinDmg;
  int m_StdDmg;
  int m_MaxDmg;
  int m_CritDmg;
  int m_MinProb;
  int m_MaxProb;
  int m_IsCritical;
  _attack_param m_param;
  unsigned int m_Len;
  unsigned int m_CastDelay;
  unsigned int m_Delay;
  unsigned int m_bLoad;
  unsigned int m_Active;
  unsigned int m_BefTime;

  SKILL();
};

/* 1779 */

/* 1314 */

/* 1316 */

/* 1422 */
union $1D4D54E2B5971D5BE0EAD557ED232A85
{
  CEntity *mEnvEntity[2];
  CParticle *mEnvParticle[2];
};

/* 1402 */
#include "CMergeFileManager.h"

/* 1416 */


/* 1417 */
#include "CPathFinder.h"

/* 1390 */


/* 1391 */
#include "CVertexBuffer.h"

/* 1392 */


/* 1393 */
#include "CIndexBuffer.h"

/* 1418 */
#include "CBsp.h"
#include "CAlpha.h"

/* 1327 */
struct _E_ENTRY
{
  unsigned int offset;
  unsigned int size;
};

/* 1328 */
struct _ENTITY_FILE_HEADER
{
  _E_ENTRY CompHeader;
  _E_ENTRY Vertex;
  _E_ENTRY VColor;
  _E_ENTRY UV;
  _E_ENTRY Face;
  _E_ENTRY FaceId;
  _E_ENTRY VertexId;
  _E_ENTRY MatGroup;
  _E_ENTRY Object;
  _E_ENTRY Track;
};

/* 1395 */
#include "CEntity.h"

/* 1424 */
#include "CSkyBox.h"

/* 1426 */
struct _ANI_CAMERA
{
  char mName[64];
  unsigned int h_num;
  float fov;
  float tdist;
  unsigned int ext_num;
  _ANI_CAMERA_EXT *ext;
  _ANI_OBJECT *obj;
};

/* 1326 */
struct __unaligned __declspec(align(1)) _ANI_OBJECT
{
  char ObjectName[64];
  char ParentName[64];
  unsigned __int16 flag;
  unsigned __int16 parent;
  int frames;
  int Pos_cnt;
  int Rot_cnt;
  int Scale_cnt;
  float scale[3];
  float scale_quat[4];
  float pos[3];
  float quat[4];
  _SCALE_TRACK *Scale;
  _POS_TRACK *Pos;
  _ROT_TRACK *Rot;
  unsigned __int8 AniFrameCache;
  float s_matrix[4][4];
  float now_frame;
  float f_matrix[4][4];
};

/* 1413 */
union $5CA2DEBC757552518522447F5D1B1DF5
{
  unsigned int mdwArgv[4];
  float mfArgv[1];
};

/* 1414 */
struct _EXT_DUMMY
{
  char mName[32];
  unsigned int mFlag;
  float mBBmin[3];
  float mBBmax[3];
  unsigned int mID;
  $5CA2DEBC757552518522447F5D1B1DF5 ___u5;
  float mMat[4][4];
  float mInvMat[4][4];
};

/* 1437 */
struct __cppobj _mon_active
{
  _mon_active_fld *m_pActRec;
  _mon_block *m_pBlk;
  unsigned __int16 m_wMonRecIndex;
  signed __int16 m_zCurMonNum;
  unsigned int m_dwCumMonNum;
  unsigned int m_dwLastRespawnTime;
  int m_nLimRegenNum;
  BossSchedule *m_pBossSchedule;

  _mon_active();
  bool SetActive(_mon_active_fld *pActRec, _mon_block *pBlk, int nMonRecIndex);
  void SetZeroMonNum();
  char SetCurMonNum(int nAlter);
  void SetBossSchedule(BossSchedule *pBossSchedule);
  BossSchedule *GetBossSchedule();
  void BossScheduleSave();
};

/* 1515 */
struct __cppobj _LAYER_SET
{
  int m_nSecNum;
  CObjectList *m_pListSectorObj;
  CObjectList *m_pListSectorPlayer;
  CObjectList *m_pListSectorTower;
  _MULTI_BLOCK *m_pMB;
  _mon_active m_MonAct[200][300];
  unsigned int m_dwStartActiveTime;
  unsigned int m_dwLastInertTime;

  _LAYER_SET();
  void CreateLayer(int nSecNum);
  void ActiveLayer(_MULTI_BLOCK *pMB);
  char InertLayer();
  bool IsActiveLayer();
};

/* 1514 */
struct __cppobj _MULTI_BLOCK
{
  int m_nBlockNum;
  CRecordData *m_ptbMonBlock;
  _mon_active **m_ppMonAct;
};

/* 1436 */
struct _mon_block
{
  _mon_block_fld *m_pBlkRec;
  _dummy_position *m_pDumPos[20];
  bool m_bBossBlock;
  bool m_bRotate;
  CMapData *m_pMap;

  bool SetBlock(_mon_block_fld *pBlkRec, CMapData *pMap, _dummy_position **ppPos);
  void SetRotateBlock(bool IsRotateBlock);
  int SelectDummyIndex();
};


/* 1517 */
struct __cppobj _portal_dummy
{
  _portal_fld *m_pPortalRec;
  _dummy_position *m_pDumPos;
};

/* 1518
struct __cppobj _store_dummy
{
  int m_nStoreType;
  _base_fld *m_pStoreRec;
  _dummy_position *m_pDumPos;
};
*/

/* 1519 */
struct __cppobj _start_dummy
{
  _dummy_position *m_pDumPos;
};

/* 1520 */
struct __cppobj _bind_dummy
{
  _dummy_position *m_pDumPos;
};

/* 1521 */
struct __cppobj __declspec(align(4)) _res_dummy
{
  _dummy_position *m_pDumPos;
  float m_fMinLocal[3][3];
  float m_fMaxLocal[3][3];
  unsigned __int8 m_byGrade[3];
  unsigned int m_dwDelay[3][2];
  unsigned __int8 m_byQualityGrade;

  _res_dummy();
  bool SetDummy(_dummy_position *pDumPos, unsigned char byQualityGrade);
  void SetRangeGrade();
  unsigned __int8 GetQualityGrade();
};

/* 1522 */
struct __cppobj _quest_dummy
{
  _dummy_position *m_pDumPos;
};

/* 1523 */
struct __cppobj _safe_dummy
{
  _dummy_position *m_pDumPos;
};

/* 1525 */

/* 1537 */

/* 1600 */

/* 1705 */
struct __cppobj __unaligned __declspec(align(1)) AP_BatterySlot
{
  AP_BatterySlot();
  ~AP_BatterySlot();
  int insert(_STORAGE_LIST::_db_con *pItem);
  bool is_private_item(_STORAGE_LIST::_db_con *pItem);
  void clear();
  bool extract(_STORAGE_LIST::_db_con *pout_item);
  unsigned int get_dur();
  _STORAGE_LIST::_db_con *get_battery();
  unsigned int sub_dur(unsigned int dwSub);

  bool m_bFill;
  _STORAGE_LIST::_db_con battery_;
};

/* 1718 */

/* 1721 */
struct __cppobj __declspec(align(8)) _guild_applier_info
{
  CPlayer *pPlayer;
  unsigned int dwApplyTime;

  _guild_applier_info();
  void init();
  bool IsFill();
};

/* 1724 */
struct __cppobj __unaligned __declspec(align(1)) _guild_member_download_zocl
{
  unsigned __int8 byDownType;
  unsigned int dwGuildSerial;
  unsigned __int8 byGuildGrade;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  long double dDalant;
  long double dGold;
  unsigned __int8 byGuildRoomType;
  int GuildRoomRestTime;
  unsigned __int8 byCurTax;
  unsigned int dwTotWin;
  unsigned int dwTotDraw;
  unsigned int dwTotLose;
  bool bPossibleElectMaster;
  unsigned __int16 wDataSize;
  char sData[10000];
};

/* 1725 */
struct __cppobj _guild_applier_download_zocl
{
  unsigned __int16 wDataSize;
  char sData[10000];
};

/* 1726 */
struct __unaligned __declspec(align(1)) _guild_query_info_result_zocl
{
  unsigned int dwGuildSerial;
  char wszGuildName[17];
  unsigned __int8 byGrade;
  unsigned int dwEmblemBack;
  unsigned int dwEmblemMark;
  unsigned int dwTotWin;
  unsigned int dwTotDraw;
  unsigned int dwTotLose;
  unsigned __int8 byCurTax;
};

/* 1727 */
struct __cppobj _guild_money_io_download_zocl
{
  unsigned __int16 wDataSize;
  char sData[10000];
};

/* 1728 */
struct __cppobj _guild_member_buddy_download_zocl
{
  unsigned __int16 wDataSize;
  char sData[900];
};

/* 1599 */

/* 1631 */
struct __cppobj _dh_reward_sub_setup
{
  _dh_reward_sub_setup();
  virtual ~_dh_reward_sub_setup();

  int nItemNum;
  _STORAGE_LIST::_db_con *Item[128];
  unsigned int m_dwGivePercent[128];
  long double dExp;
  unsigned int dwPvp;
  unsigned int dwDalant;
};

/* 1630 */
struct __cppobj __dp_mission_potal
{
  __dp_mission_potal();
  ~__dp_mission_potal();
  void set(char *szCode);
  bool find(char *pcode);

  int m_nNums;
  int m_nCodeLen[32];
  char m_strCodes[32][64];
};

/* 1601 */
struct __cppobj _dh_quest_setup
{
  _dh_quest_setup();
  virtual ~_dh_quest_setup();
  void SetRealBoss(bool isReal);
  _dh_mission_setup *SearchMissionFromTitle(char *pszMissionTitle);

  int bRealBoss;
  char szQuestTitle[33];
  int bPartyOnly;
  unsigned int dwMaxMember;
  CMapData *pUseMap;
  _dh_mission_setup *pStartMissionSetup;
  unsigned int dwMonRepIndex;
  _dh_reward_sub_setup RewardOne;
  _dh_reward_sub_setup RewardOther;
  char szDescirptCode[17];
  char szStartMissionBuffer[33];
  int nEmbMissionSetupNum;
  _dh_mission_setup *EmbMissionSetup[100];
  int nDummyBlockNum;
  __dummy_block *pDummyBlock[32];
  int nMonsterGroupNum;
  __monster_group *pMonsterGroup[32];
  int nLimitLvMin;
  int nLimitLvMax;
};

/* 1603 */
struct __cppobj __declspec(align(8)) _dh_mission_setup
{
  _dh_mission_setup();
  virtual ~_dh_mission_setup();

  char szMissionTitle[33];
  _dummy_position *pAreaDummy;
  _dummy_position *pStartDummy;
  unsigned __int8 byJobOrder;
  unsigned int dwLimTimeMSec;
  unsigned __int8 byResultType;
  char *pszNextMissionTitle;
  char szDescirptCode[17];
  char szCompleteMsg[17];
  int nAddMonsterNum;
  __add_monster *pAddMonster[32];
  int nLootItemNum;
  __add_loot_item *pLootItem[32];
  int nChangeMonsterNum;
  __change_monster *pChangeMonster[32];
  int nInnerCheckNum;
  __inner_check *pInnerCheck[64];
  int nRespondCheckNum;
  __respond_check *pRespondCheck[32];
  int nRespawnMonsterNum;
  __respawn_monster *pRespawnMonster[32];
  int nAddSecCheckNum;
  __add_time *pAddSecCheck[32];
  int nEmbJobSetupNum;
  _dh_job_setup *EmbJobSetup[8];
  _dh_mission_setup *pNextMission;
  __dp_mission_potal facMissionPotal;
};

/* 407 */
enum __bitmask OBJ_DEF_TYPE : __int32
{
  react_kind_item = 0x1,
  react_kind_monster = 0x2,
  react_kind_mgrp = 0x4,
};

/* 1613 */
struct __cppobj __declspec(align(8)) _react_obj
{
  _react_obj();
  void copy(_react_obj *pObj);

  struct ITEM
  {
    unsigned __int8 byItemTableCode;
    _base_fld *pItemFld;
  };

  struct MONSTER
  {
    _monster_fld *pMonsterFld;
  };

  struct MON_GRP
  {
    __monster_group *pMonGrp;
  };

  union OBJ
  {
    ITEM item;
    MONSTER monster;
    MON_GRP mon_grp;
  };

  OBJ_DEF_TYPE ObjDefType;
  OBJ obj;
  unsigned __int16 wNum;
};

/* 408 */
enum __bitmask AREA_DEF_TYPE : __int32
{
  at_dummy = 0x1,
  at_block = 0x2,
  at_var = 0x4,
};

/* 1618 */
struct __cppobj _react_area
{
  _react_area();
  void copy(_react_area *pObj);

  struct DUMMY
  {
    _dummy_position *pPos;
  };

  struct BLOCK
  {
    __dummy_block *pBlk;
  };

  union OBJ
  {
    DUMMY dummy;
    BLOCK block;
  };

  AREA_DEF_TYPE AreaDefType;
  OBJ obj;
};

/* 1624 */
struct __cppobj __respawn_monster
{
  __respawn_monster();

  _react_obj ReactObj;
  _react_area ReactArea;
  unsigned int dwTermMSec;
  int nLim;
  bool bCallEvent;
  char *pszDefineCode;
};

/* 1642 */
struct __cppobj __declspec(align(8)) MonsterSFContDamageToleracne
{
  CMonster *m_pMonster;
  float m_fToleranceProb;
  float m_fToleranceProbMax;
  unsigned int m_dwLastUpdateTime;

  MonsterSFContDamageToleracne();
  void Init(float fMaxTolValue);
  void OnlyOnceInit(CMonster *pMonster);
  bool IsSFContDamage();
  float GetToleranceProb();
  void SetSFDamageToleracne_Variation(float fAddValue);
  void Update();
};

/* 1682 */
#include "CABC75A24E330D1CBB803A094EF8.h"

/* 1683 */
union $B1473DED9FF3433080ADDC1400B1BAA4
{
  $8861CABC75A24E330D1CBB803A094EF8 __s0;
  unsigned int m_nCommonStateChunk;
};

/* 1643 */
struct __cppobj EmotionPresentationChecker
{
  bool m_bIsSet;
  unsigned __int8 m_byType;
  unsigned __int16 m_wIndex;
  unsigned __int16 m_wRandIndex;
  CCharacter *m_pTarget;

  EmotionPresentationChecker();
  void ReSet();
  bool CheckEmotionState(CMonster *pThis, unsigned __int8 byCheckType, CCharacter *pTarget);
};

/* 1644 */
struct $4B355331FE7D104551ED610C3E15A592
{
  unsigned __int16 m_wMove_State : 1;
  unsigned __int16 m_wRunLockState : 1;
  unsigned __int16 m_wEffectIconType : 4;
  unsigned __int16 m_wCombatState : 1;
};

/* 1645 */
union $61453E1D3C89B67BD8C0921FE198A987
{
  $4B355331FE7D104551ED610C3E15A592 __s0;
  unsigned __int16 m_wSendChunkData;
};

/* 1646 */
struct __cppobj MonsterStateData
{
  $61453E1D3C89B67BD8C0921FE198A987 ___u0;

  MonsterStateData();
  unsigned __int16 GetStateChunk();
  bool operator!=(const MonsterStateData *rhs);
};

/* 1647 */

/* 1648 */

/* 1649 */
struct _event_loot_item
{
  unsigned __int8 byItemTable;
  unsigned __int16 wItemIndex;
};

/* 1668 */
struct __cppobj Us_FSM_Node
{
  Us_FSM_Node();
  ~Us_FSM_Node();
  void Init();
  void SetState(unsigned int dwState);
  unsigned int GetState();
  void SetParent(Us_FSM_Node *pParent);
  void SetLoopTime(unsigned int dwLoopTime);

  Us_FSM_Node *m_pParent;
  unsigned int m_dwState;
  int m_bLive;
  unsigned int m_dwLastTime;
  unsigned int m_dwLoopTime;
};

/* 1669 */
template <typename T>
struct UsPoint
{
  T *m_pObject;
};

template <>
struct UsPoint<UsStateTBL>
{
  UsPoint();
  UsPoint(UsStateTBL *pObject);
  UsPoint(const UsPoint<UsStateTBL> &other);
  ~UsPoint();
  UsPoint<UsStateTBL> &operator=(UsStateTBL *pObject);
  UsStateTBL *operator->();
  operator UsStateTBL *();

  UsStateTBL *m_pObject;
};

/* 1660 */
struct __cppobj Us_HFSM
{
public:
  int m_bSet;
  void *m_pObject;
  unsigned int m_dwUsedCount;
  Us_FSM_Node m_ArNode[10];
  UsPoint<UsStateTBL> m_spShareStateTBLPtr;
  Us_HFSM();
  virtual void __vtable_anchor();
  virtual ~Us_HFSM();
  void Init();
  void CleanUp();
  int SetMyData(UsStateTBL *pStateTBL, void *pObject);
  Us_FSM_Node *GetNode(unsigned int dwIndex);
  void *GetObjectA();
  unsigned int GetState(unsigned int dwIndex);
  unsigned int GetIndex(Us_FSM_Node *pNode);
  int Link(Us_FSM_Node *pParent, Us_FSM_Node *pChild);
  void SendMsg(unsigned int dwFSMIndex, unsigned int dwMSG, void *lpParam);
  void SendExternMsg(unsigned int dwMSG, void *lpParam, unsigned int nParam);
  void SetLoopTime(unsigned int nIndex, unsigned int dwLoopTime);
  void AddLoopDelayTime(unsigned int nIndex, unsigned int dwAddDelay);
  virtual void OnProcess(unsigned int dwLastTime);
};

/* 1670 */
struct __cppobj SF_Timer
{
  unsigned int m_dwLastCheckTime;
  unsigned int m_dwGapCheckTime;

  void Set(unsigned int dwTimeDelay);
  bool CheckTime(unsigned int dwLoopTime);
};

/* 1671 */
#include "CPathMgr.h"

/* 1673 */

/* 1680 */
#include "CLuaSignalReActor.h"

/* 1635 */

/* 1321 */


/* 1397 */
union $3E60C3DACEF2E33FF1D1871D4F2565FA
{
  float mElasticity;
  float mTexRepeat;
};

/* 1398 */
#include "CParticle.h"

/* 1401 */
#include "CMergeFile.h"

/* 1403 */
struct _ENTITY_LIST
{
  unsigned __int8 IsParticle;
  unsigned __int8 IsFileExist;
  char Name[62];
  float FadeStart;
  float FadeEnd;
  unsigned __int16 Flag;
  unsigned __int16 ShaderID;
  float Factor[2];
};

/* 1404 */
struct __unaligned __declspec(align(2)) _LEAF_ENTITIES_LIST_INFO
{
  unsigned int start_id;
  unsigned __int16 entities_num;
};

/* 1405 */
struct __unaligned __declspec(align(2)) _MAP_ENTITIES_LIST
{
  unsigned __int16 ID;
  float Scale;
  float Pos[3];
  float RotX;
  float RotY;
  __int16 BBMin[3];
  __int16 BBMax[3];
  float AddFrame;
  CParticle *Particle;
  unsigned int Color;
  unsigned int mMapColor;
};

/* 1406 */
struct _SOUND_ENTITY_LIST
{
  unsigned __int16 ID;
  char Name[62];
};

/* 1407 */
struct _SOUND_ENTITIES_LIST
{
  unsigned __int16 ID;
  unsigned __int16 EventTime;
  unsigned int Flag;
  float Scale;
  float Attn;
  float Pos[3];
  float BoxScale[3];
  float BoxAttn;
  float BoxRotX;
  float BoxRotY;
  float NextPlayTime;
  float BBMin[3];
  float BBMax[3];
  float mInvMat[4][4];
};

/* 1408 */
struct _LEAF_SOUND_ENTITIES_LIST_INFO
{
  unsigned __int16 start_id;
  unsigned __int16 entities_num;
};

/* 1409 */


/* 1332 */
/* 1357 */


/* 1329 */
struct __unaligned __declspec(align(2)) _ONE_LAYER
{
  __int16 m_iTileAniTexNum;
  int m_iSurface;
  unsigned int m_dwAlphaType;
  unsigned int m_ARGB;
  unsigned int m_dwFlag;
  __int16 m_sUVLavaWave;
  __int16 m_sUVLavaSpeed;
  __int16 m_sUVScrollU;
  __int16 m_sUVScrollV;
  __int16 m_sUVRotate;
  __int16 m_sUVScaleStart;
  __int16 m_sUVScaleEnd;
  __int16 m_sUVScaleSpeed;
  __int16 m_sUVMetal;
  __int16 m_sANIAlphaFlicker;
  unsigned __int16 m_sANIAlphaFlickerAni;
  __int16 m_sANITexFrame;
  __int16 m_sANITexSpeed;
  __int16 m_sGradientAlpha;
};

/* 1330 */
struct __unaligned __declspec(align(2)) _R3MATERIAL
{
  int m_iMatNum;
  char m_name[128];
  unsigned int m_dwFlag;
  int m_iDetailSurface;
  float m_fDetailScale;
  unsigned int m_dwLayerNum;
  _ONE_LAYER m_Layer[7];
};

/* 1394 */
struct __unaligned __declspec(align(4)) _ENTITY_M_GROUP
{
  unsigned __int16 Type;
  unsigned __int16 TriNum;
  __int16 MtlId;
  float BBMin[3];
  float BBMax[3];
  unsigned int VBMinIndex;
  unsigned int IBMinIndex;
  unsigned int VCnt;
  unsigned __int16 ObjectId;
  void *MultiSourceUV;
};

/* 1425 */
struct _ANI_CAMERA_EXT
{
  float frame;
  float fov;
  float tdist;
  float temp1;
  float temp2;
};

/* 1323 */
struct _SCALE_TRACK
{
  float frame;
  float scale[3];
  float scale_axis[4];
};

/* 1324 */
struct _POS_TRACK
{
  float frame;
  float pos[3];
};

/* 1325 */
struct _ROT_TRACK
{
  float frame;
  float quat[4];
};

/* 1452 */
struct __cppobj BossSchedule
{
  char m_strSection[64];
  char m_strMonCode[64];
  _mon_block *m_pBlock;
  _mon_active *m_pMonAct;
  int m_nBlockIndex;
  int m_nActIndex;
  ATL::CTime m_LastRespawnSystemTime;
  unsigned __int16 m_LiveCount;
  BossSchedule_Map *m_pParent;

  BossSchedule();
  ~BossSchedule();
  static unsigned __int16 Make_LiveCount(char *strTimeValue);
  static ATL::CTime *Make_LastTimeRespawnSystemTime(ATL::CTime *result, char *strTimeValue);
  void Save_LiveCount(unsigned __int16 wCount);
  void Save_LastRespawnSystemTime(ATL::CTime *systime);
  bool Make_LiveCountString(char *strBuff, int nBuffSize);
  bool Make_LastTimeRespawnSystemTimeString(char *strBuff, int nBuffSize);

  static const char *ms_KeyPair[3];
};

/* 1602 */

/* 1632 */

/* 1615 */
struct __cppobj __dummy_block
{
  __dummy_block();

  char *pszBlockName;
  int nSubDummyNum;
  _dummy_position *pSubDummy[32];
};

/* 1610 */
struct __cppobj __monster_group
{
  __monster_group();

  char *pszGroupName;
  int nSubMonsterNum;
  _monster_fld *pSubMonster[32];
};

/* 1604 */

/* 1619 */
struct __cppobj __add_monster
{
  __add_monster();

  _react_obj ReactObj;
  _react_area ReactArea;
};

/* 1620 */
struct __cppobj __add_loot_item
{
  __add_loot_item();

  unsigned __int8 byItemTableCode;
  _base_fld *pItemFld;
  unsigned int dwDur;
  _react_obj ReactObj;
};

/* 1621 */
struct __cppobj __change_monster
{
  __change_monster();
  ~__change_monster();

  _monster_fld *pMonsterFldA;
  _monster_fld *pMonsterFldB;
  int nProb;
  char *pszIfMissionDescirptCode;
  char *pszifCompleteMsg;
};

/* 1622 */
struct __cppobj __inner_check
{
  __inner_check();
  ~__inner_check();

  _react_area ReactArea_Evt;
  _react_area ReactArea_Aft;
  _react_obj ReactObj;
  char *pszMsg;
  char *pszRespawnCode;
};

/* 1623 */
struct __cppobj __respond_check
{
  __respond_check();
  ~__respond_check();

  unsigned __int8 byActTableCode;
  _base_fld *pActObjFld;
  _react_obj ReactObj;
  _react_area ReactArea;
  char *pszMsg;
};

/* 1625 */
struct __cppobj __add_time
{
  __add_time();

  _react_obj EventObj;
  unsigned int dwAddSec;
  char *pszMsg;
};

/* 409 */
enum EM_DH_EVENT : __int32
{
  dh_event_take = 0x0,
  dh_event_hunt = 0x1,
};

/* 1628 */
struct __cppobj _job_sub_setup
{
  _job_sub_setup();

  _base_fld *pEventFld;
  unsigned __int8 byTable;
  int nEventCount;
};

/* 1626 */
struct __cppobj __declspec(align(8)) _dh_job_setup
{
  _dh_job_setup();
  virtual ~_dh_job_setup();

  char szJobTitle[33];
  EM_DH_EVENT eventType;
  _job_sub_setup JobSetup;
  int nReactNum;
  _react_sub_setup *ReactSetup[10];
  char szDescirptCode[17];
};

/* 1641 */

/* 1667 */

/* 1661 */
struct __cppobj __declspec(align(8)) UsRefObject
{
  UsRefObject();
  virtual ~UsRefObject();
  void IncRefCount();
  void DecRefCount();

  static unsigned int ms_uiObjects;

  unsigned int m_uiRefCount;
};

/* 1665 */
struct __cppobj UsStateTBL : UsRefObject
{
  struct _TBLData
  {
    unsigned __int8 byKey;
    void (__fastcall *pEvnetFun)(Us_HFSM *, unsigned int, void *);

    _TBLData();
    void Init();
  };

  struct _HFSM_Node_Info
  {
    int m_ParentData;
    unsigned int m_dwStartState;
    int m_bUsed;
    unsigned int m_dwLoopTime;

    _HFSM_Node_Info();
    void Init();
  };

  unsigned int m_IdentityKey;
  _TBLData **m_ppTBL;
  _HFSM_Node_Info *m_pNodeInfo;
  int m_bAlloc;
  unsigned __int8 m_byHFSMSize;
  unsigned __int8 m_byStateSize;
  unsigned __int8 m_byMessageSize;
  void (__fastcall *m_pFun)(Us_HFSM *, unsigned int, unsigned int, void *);
  int (__fastcall *m_pInitFun)(UsStateTBL *, Us_HFSM *);
  void (__fastcall *m_pExternFun)(Us_HFSM *, unsigned int, void *, int);

  UsStateTBL();
  virtual ~UsStateTBL();
  void CleanUp();
  void Alloc(unsigned __int8 byHFSMSize, unsigned __int8 byStateSize, unsigned __int8 byMessageSize);
  int Add(
    unsigned __int8 byHFSMIndex,
    unsigned __int8 byCurrState,
    unsigned __int8 byEvent_IN,
    unsigned __int8 byNextState_OUT,
    void (__fastcall *pEvnetFun)(Us_HFSM *, unsigned int, void *));
  void SetHFSMNode(
    int nNodeIndex,
    unsigned int dwStartState,
    unsigned int dwLoopTime,
    int ParentData,
    int bUsed);
  void SetInitFunction(int (__fastcall *pInitFun)(UsStateTBL *, Us_HFSM *));
  void SetExternCallFunction(void (__fastcall *pExternFun)(Us_HFSM *, unsigned int, void *, int));
  void SetCallFunction(void (__fastcall *pFun)(Us_HFSM *, unsigned int, unsigned int, void *));
  unsigned __int8 GetHSFMSize();
  _TBLData *GetTransState(unsigned __int8 byState, unsigned __int8 byMessage);
  int SetHFSM(Us_HFSM *pHFSM, void *pObject);
  static void OnMsgProc(Us_HFSM *pHFS, unsigned int dwFSMIndex, unsigned int dwMSG, void *lpParam);
};

/* 1672 */

/* 1675 */
#include "CLuaCommand.h"

/* 1677 */
#include "CLuaCommandEx.h"

/* 1396 */
struct _PARTICLE_ELEMENT
{
  float mDirStep[3];
  float mDir[3];
  float mPos[3];
  float mTime;
  float mARGBStep[4];
  float mARGB[4];
  float mScaleStep;
  float mScale;
  float mZRotStep;
  float mZRot;
  float mYRotStep;
  float mYRot;
  unsigned __int8 mFlag;
  unsigned __int8 mIsLive;
  unsigned __int16 mNowTrack;
  float mNowFrame;

  void UpDate(float a2);
};

/* 1399 */
union $D702D035F589A3FF876782FB29EF6D4D
{
  char name[52];
  unsigned int name_cmp[13];
};

/* 1400 */
struct _MERGE_FILE
{
  $D702D035F589A3FF876782FB29EF6D4D ___u0;
  unsigned int file_length;
  unsigned __int16 name_cnt;
  unsigned __int16 cnt;
  unsigned int start_index;
};

/* 1333 */

/* 1430 */

/* 1438 */
#include "CIniFile.h"

/* 1473 */
struct __cppobj BossSchedule_Map
{
  int m_nIndex;
  char m_strMap[64];
  CIniFile m_INIFile;
  int m_nCount;
  BossSchedule **m_ScheduleList;
  CBossMonsterScheduleSystem *m_pSystem;

  BossSchedule_Map();
  ~BossSchedule_Map();
  void Clear();
  bool SaveAll();
  bool LoadAll();
};

/* 1627 */

/* 1629 */
struct __cppobj _react_sub_setup
{
  _react_sub_setup();

  unsigned __int8 byReactType;
  _dummy_position *pPortalDummy;
};

/* 1662 */

/* 1676 */

/* 1439 */

/* 1448 */
struct __cppobj INI_Section
{
  char m_strSection[64];
  std::vector<INI_Key *> m_KeyList;

  INI_Section();
  ~INI_Section();
  INI_Key *GetKey(const char *strKey);
};

/* 1449 */
struct __cppobj __declspec(align(8)) INI_Key
{
  INI_Section *m_pParentSection;
  char m_strKey[65];
  char m_strValue[65];

  INI_Key();
};

/* 1464 */
#include "CMapDataTable.h"

/* 1472 */
#include "RegionData.h"

/* 1462 */
#include "CMapOperation.h"

/* 1474 */
struct __cppobj BossSchedule_TBL
{
  int m_nCount;
  BossSchedule_Map **m_MapScheduleList;

  BossSchedule_TBL();
  ~BossSchedule_TBL();
};

/* 1477 */

/* 1476 */
struct __cppobj ScheduleMSG
{
  char m_strSection[64];
  unsigned __int8 m_byKey;
  char m_strValue[64];
  unsigned __int16 m_wIniFileIndex;

  ScheduleMSG();
  void Init();
};

#pragma pack(pop)


