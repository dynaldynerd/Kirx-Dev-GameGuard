#pragma once

#include "IdaCompat.h"
#include "CPlayer.h"

#pragma pack(push, 1)
#include <utility>
#include <vector>



struct _RTL_CRITICAL_SECTION_DEBUG;
struct CRFWorldDatabase;
struct _PCBANG_PLAY_TIME;
struct CEventLootTable;
struct CItemLootTable;
struct COreCuttingTable;
struct _monster_sp_fld;
struct _skill_fld;
struct _force_fld;
struct _monster_sp_group;
struct RFEventBase;
class CGameObject;
struct CPlayer;
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
struct CEntity;
struct _PARTICLE_ELEMENT;
struct CParticle;
struct _MERGE_FILE;
struct CMergeFile;
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
struct CBsp;
struct CSkyBox;
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
struct CBossMonsterScheduleSystem;
struct CMapOperation;
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
struct CUserDB;
struct _PVPPOINT_LIMIT_DB_BASE;
struct _PVP_ORDER_VIEW_DB_BASE;
struct CPartyPlayer;
struct CDarkHoleChannel;
struct CDarkHole;
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
struct CMonster;
struct _event_respawn;
struct _event_set;
struct UsStateTBL;
struct CLuaCommandEx;
struct AP_BatterySlot;
struct AutominePersonal;
struct _class_fld;
struct CGuild;
struct _guild_member_info;
struct _guild_applier_info;
struct _guild_member_download_zocl;
struct _guild_applier_download_zocl;
struct _guild_query_info_result_zocl;
struct _guild_money_io_download_zocl;
struct _guild_member_buddy_download_zocl;
struct CGuardTower;
struct CTrap;
struct _happen_event_node;
struct CParkingUnit;
struct _animus_fld;
struct CAnimus;
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
  __int16 iType;
  char szCMS[7];
  int lRemainTime;
  _SYSTEMTIME stEndDate;
  bool bPCCheat;
  bool bIsPcBang;
  bool bAgeLimit;
};

/* 1261 */
struct __cppobj _WAIT_ENTER_ACCOUNT
{
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
struct __cppobj __declspec(align(8)) CFrameRate
{
  float m_fSpeedPerFrame;
  unsigned int m_dwFrames;
  unsigned int m_dwFrameTime;
  unsigned int m_dwFrameCount;
  unsigned int m_dwDisplayTick;
  virtual ~CFrameRate() = default;
};

/* 1265 */
struct __cppobj _message
{
  unsigned int dwMessage;
  unsigned int dwKey1;
  unsigned int dwKey2;
  unsigned int dwKey3;
  _message *pNext;
  _message *pPrev;
};

/* 1267 */
struct __cppobj CMyCriticalSection
{
  _RTL_CRITICAL_SECTION m_cs;
  virtual ~CMyCriticalSection() = default;
};

/* 1264 */
struct __cppobj CMsgData
{
  int m_nObjNum;
  int m_nMaxBufNum;
  _message m_gmListHead;
  _message m_gmListTail;
  _message *m_gmBuf;
  _message m_gmListEmptyHead;
  _message m_gmListEmptyTail;
  CMyCriticalSection m_csList;
  CMyCriticalSection m_csEmpty;
  virtual ~CMsgData() = default;
};

/* 1269 */
struct __cppobj CMsgProcess : CMsgData
{
};

/* 1272 */
struct _USER_NUM_SHEET
{
  int nAveragePerHour;
  int nMaxPerHour;
  int nCount;
};

/* 1273 */
struct __cppobj CConnNumPHMgr
{
  struct __cnt_per_h
  {
    int nUserCumPerMin;
    unsigned int dwCounting;
    int nMaxUserNum;
  };

  __cnt_per_h m_cph;
  _USER_NUM_SHEET m_LastResult;
  int m_nLastHour;
  unsigned int m_dwCheckLastTime;
};

/* 1277 */
struct __cppobj __declspec(align(4)) _DB_QRY_SYN_DATA
{
  bool m_bUse;
  bool m_bLoad;
  unsigned int m_dwAccountSerial;
  _CLID m_idWorld;
  unsigned __int8 m_byQryCase;
  unsigned __int8 m_byResult;
  char m_sData[74530];
};

/* 1280 */
struct __cppobj CCheckSum
{
};

/* 1283 */
struct __cppobj _record_bin_header
{
  int m_nRecordNum;
  int m_nFieldNum;
  int m_nRecordSize;
};

/* 1281 */
struct __cppobj CRecordData
{
  bool m_bLoad;
  char m_szFileName[129];
  unsigned int m_dwTotalSize;
  _record_bin_header m_Header;
  int m_nLowNum;
  char **m_ppsRecord;
  unsigned int *m_pdwHashList;
  virtual ~CRecordData() = default;
};

/* 1284 */
struct __cppobj __declspec(align(8)) CItemLootTable
{
  struct _linker_code
  {
    unsigned __int8 byTableCode;
    unsigned __int16 wItemIndex;
    int bExist;
  };

  CRecordData m_tblLoot;
  CEventLootTable *m_pTblEvent;
  _linker_code **m_ppLinkCode;
  int m_nLootNum;
  virtual ~CItemLootTable() = default;
};

/* 1290 */
struct __cppobj COreCuttingTable
{
  struct _ore_cut_list
  {
    struct _res_list
    {
      unsigned __int16 wResIndex;
      unsigned int dwRate;
      unsigned int dwCumRate;
    };

    int nResNum;
    unsigned int dwTotalRate;
    _res_list ResList[100];
  };

  CRecordData m_tblOreCutting;
  int m_nOreNum;
  _ore_cut_list *pOreList;
  virtual ~COreCuttingTable() = default;
};

/* 1294 */
struct __cppobj CItemUpgradeTable
{
  CRecordData m_tblItemUpgrade;
  int m_nResNum;
  unsigned __int16 *m_pwResIndex;
  virtual ~CItemUpgradeTable() = default;
};

/* 1304 */
struct __cppobj CMonsterSPGroupTable
{
  unsigned int m_dwRecordNum;
  _monster_sp_group *m_pRecordData;
};

/* 1132 */
struct __cppobj __declspec(align(8)) CLogFile
{
  char m_szFileName[128];
  unsigned int m_dwLogCount;
  int m_bWriteAble;
  CNetCriticalSection m_cs;
  bool m_bAddCount;
  bool m_bDate;
  bool m_bTrace;
  bool m_bInit;
};

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
struct __cppobj CNotifyNotifyRaceLeaderSownerUTaxrate
{
  _notify_race_leader_s_owner_u_taxrate m_Send[3];
};

/* 1248 */
struct __cppobj __declspec(align(4)) TournamentWinner
{
  unsigned int dwSerial;
  char wszCharName[17];
  unsigned __int8 byGrade;
};

/* 1808 */
struct __cppobj CBattleTournamentInfo
{
  bool m_bLoad;
  int m_nCurNum;
  TournamentWinner m_WinnerInfo[48];
};

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
struct __cppobj __declspec(align(8)) CMainThread
{
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
  virtual ~CMainThread() = default;
};

/* 1306 */

/* 1126 */
struct __cppobj __declspec(align(8)) CRFNewDatabase
{
  void *m_hEnv;
  void *m_hDbc;
  void *m_hStmtSelect;
  void *m_hStmtUpdate;
  bool m_bConectionActive;
  bool m_bSaveDBLog;
  CLogFile m_ProcessLogW;
  CLogFile m_ErrorLogW;
  CLogFile m_ProcessLogA;
  CLogFile m_ErrorLogA;
  unsigned __int8 m_byLogFileHour;
  char m_szOdbcName[32];
  char m_szAccountName[32];
  char m_szPassword[32];
  bool m_bReconnectFailExit;
  char m_szLogUpperPath[128];
  virtual ~CRFNewDatabase() = default;
};

/* 1133 */
struct __cppobj CRFWorldDatabase : CRFNewDatabase
{
};

/* 1263 */

/* 1266 */

/* 1268 */

/* 1275 */

/* 1282 */

/* 1285 */

/* 1286 */
struct __cppobj CEventLootTable
{
  struct _event_drop
  {
    char strCode[64];
    unsigned __int16 wMagnifications;
    unsigned __int16 wRange;
    unsigned __int16 wDropCntOnce;
    unsigned __int16 wDropDelay;
    _event_drop *pNext;
  };

  _event_drop *m_pEventDropList;
  virtual ~CEventLootTable() = default;
};


/* 1296 */
struct __cppobj _base_fld
{
  unsigned int m_dwIndex;
  char m_strCode[64];
};

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
};

/* 1815 */
struct __cppobj TimeLimitMgr
{
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

/* 1297 */
struct __cppobj _monster_sp_fld : _base_fld
{
  int m_nSpecialAttType;
  char m_strSpecialAttCode[64];
  int m_nAttLv;
  float m_fSpecialRange;
  int m_nMotiveCondition;
  int m_nMotiveValue;
  int m_nMotiveExceptionCondition;
  int m_nMotiveExceptionValue;
  int m_nMotiveTime;
  int m_nDstType;
  int m_nLimitCount;
  int m_nProbability;
};

/* 1298 */
struct _consume_item_list
{
  char m_itmNeedItemCode[8];
  int m_nNeedItemCount;
};

/* 1299 */
struct _cont_param_list
{
  int m_nContParamCode;
  int m_nContParamIndex;
  float m_fContValue[7];
};

/* 1300 */
struct __cppobj _skill_fld : _base_fld
{
  int m_nClass;
  int m_nIconIDX;
  int m_nMastIndex;
  char m_strMastKorName[64];
  char m_strMastEngName[64];
  char m_strKorName[64];
  char m_strEngName[64];
  int m_nLv;
  int m_bActivate;
  int m_bEnable;
  char m_strUsableRace[64];
  char m_strActableDst[64];
  char m_strGradeLimit[64];
  int m_nNeedMastIndex;
  char m_strFixWeapon[64];
  int m_bFixshield;
  int m_nSpecialType;
  int m_nNeedSpecialType;
  int m_nNeedHP;
  int m_nNeedFP;
  int m_nNeedSP;
  _consume_item_list m_ConsumeItemList[3];
  float m_fActDelay;
  int m_bCumulType;
  int m_nCumulCounter;
  int m_nNewEffCount;
  char m_strEffectCode[64];
  int m_nAttackable;
  int m_nAttType[7];
  int m_nAttConstant[7];
  float m_fAttFormulaConstant;
  int m_nAttNeedBt;
  int m_nBonusDistance;
  char m_strRangeEffCode[64];
  int m_nTempEffectType;
  int m_nTempParamCode;
  float m_fTempValue[7];
  int m_nContEffectType;
  int m_nEffLimType;
  int m_nEffLimType2;
  int m_nContAreaType;
  _cont_param_list m_ContParamList[5];
  int m_nContEffectSec[7];
  int m_nEtc;
  float m_f1_2speed;
  float m_f1_2distance;
  float m_f2_3speed;
  float m_f2_3distance;
  int m_nEffectClass;
};

/* 1301 */
struct __cppobj _force_fld : _base_fld
{
  int m_nClass;
  int m_nIconIDX;
  int m_nMastIndex;
  char m_strMastKorName[64];
  char m_strMastEngName[64];
  char m_strKorName[64];
  char m_strEngName[64];
  int m_nLv;
  int m_bActivate;
  int m_bEnable;
  char m_strUsableRace[64];
  char m_strActableDst[64];
  char m_strGradeLimit[64];
  int m_nNeedMastIndex;
  char m_strFixWeapon[64];
  int m_bFixshield;
  int m_nSpecialType;
  int m_nNeedSpecialType;
  int m_nNeedHP;
  int m_nNeedFP;
  int m_nNeedSP;
  _consume_item_list m_ConsumeItemList[3];
  float m_fActDelay;
  int m_bCumulType;
  int m_nCumulCounter;
  int m_nNewEffCount;
  char m_strEffectCode[64];
  int m_bAttackable;
  int m_nProperty;
  int m_nEffectGroup;
  float m_fAttFormulaConstant;
  int m_nActDistance;
  char m_strRangeEffCode[64];
  int m_nTempEffectType;
  int m_nTempParamCode;
  float m_fTempValue[7];
  int m_nContEffectType;
  int m_nContAreaType;
  _cont_param_list m_ContParamList[5];
  int m_nContEffectSec[7];
  int m_nEffectClass;
};

/* 1804 */

/* 1312 */

/* 1311 */

/* 1320 */
struct __cppobj D3DXMATRIX
{
  float m[4][4];
};

/* 1427 */
struct __cppobj CAniCamera
{
  _ANI_CAMERA *AniCamera;
  _ANI_OBJECT *mDummy;
  unsigned int mDummyNum;
  unsigned int mAniCameraNum;
  unsigned int mStartFrame;
  unsigned int mEndFrame;
  unsigned int mPlayStartFrame;
  unsigned int mPlayEndFrame;
  int mPlayIndex;
  int mIsSetPerspect;
  float mStartTick;
  unsigned int mFlag;
  float mOldFrame;
  float mNowFrame;
};

/* 1428 */
struct __cppobj CTimer
{
  float mLoopTime;
  float mTime;
  float mRealTime;
  float mMinFPS;
  float mfLoopHop;
  unsigned int mOldTime;
  unsigned int mLoopCnt;
  unsigned int mLoopHop;
  int m_bUsingQPF;
  int m_bTimerInitialized;
  float mFPS;
  unsigned int mLoopFPSCnt;
  float mFPSTime;
  float m_fTicksPerSec;
  float m_fFramesPerSec;
  float m_fAverageFramesPerSec;
  float m_fSecsPerFrame;
  float m_fLamTime;
  unsigned __int64 m_qwTicks;
  unsigned __int64 m_qwStartTicks;
  unsigned __int64 m_qwTicksPerSec;
  unsigned __int64 m_qwTicksPerFrame;
};

/* 1415 */
struct __cppobj CExtDummy
{
  unsigned int mMaxNum;
  unsigned int mNum;
  _EXT_DUMMY *mDummy;
};

/* 1315 */
struct __cppobj __declspec(align(8)) CLevel
{
  char mMapName[256];
  float mCamPos[3];
  D3DXMATRIX mMatView;
  int mIsLoadedBsp;
  CBsp *mBsp;
  CSkyBox *mSkyBox;
  CAniCamera mAutoAniCam;
  CTimer mTimer;
  CExtDummy mDummy;
  unsigned int mLightTexMemSize;
  unsigned int mMapTexMemSize;
  unsigned int mSkyTexMemSize;
  unsigned int mEntityTexMemSize;
  unsigned int mEnvironment;
  virtual ~CLevel() = default;
};

/* 1524 */
struct __cppobj __declspec(align(8)) CDummyPosTable
{
  _dummy_position *m_pDumPos;
  int m_nDumPosDataNum;
  virtual ~CDummyPosTable() = default;
};

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
class __cppobj __declspec(align(8)) CMapData
{
  bool m_bUse;
  bool m_bLoad;
  int m_nMapIndex;
  CLevel m_Level;
  int m_nMapCode;
  _LAYER_SET *m_ls;
  _MULTI_BLOCK *m_mb;
  CExtDummy m_Dummy;
  int m_nMapInPlayerNum;
  int m_nMapInMonsterNum;
  int m_nMonBlockNum;
  _mon_block *m_pMonBlock;
  int m_nMonDumNum;
  int m_nPortalNum;
  _portal_dummy *m_pPortal;
  int m_nItemStoreDumNum;
  _store_dummy *m_pItemStoreDummy;
  int m_nStartDumNum;
  _start_dummy *m_pStartDummy;
  int m_nBindDumNum;
  _bind_dummy *m_pBindDummy;
  int m_nResDumNum;
  _res_dummy *m_pResDummy;
  int m_nQuestDumNum;
  _quest_dummy *m_pQuestDummy;
  _map_fld *m_pMapSet;
  CExtDummy *m_pExtDummy_Town;
  int m_nSafeDumNum;
  _safe_dummy *m_pSafeDummy;
  CDummyPosTable m_tbSafeDumPos;
  CRecordData m_tbMonBlk;
  CRecordData m_tbPortal;
  CDummyPosTable m_tbMonDumPos;
  CDummyPosTable m_tbPortalDumPos;
  CDummyPosTable m_tbStoreDumPos;
  CDummyPosTable m_tbStartDumPos;
  CDummyPosTable m_tbBindDumPos;
  CDummyPosTable m_tbResDumPosHigh;
  CDummyPosTable m_tbResDumPosMiddle;
  CDummyPosTable m_tbResDumPosLow;
  CDummyPosTable m_tbQuestDumPos;
  _bsp_info m_BspInfo;
  _sec_info m_SecInfo;
  CMyTimer m_tmrMineGradeReSet;
  int m_nMonTotalCount;
  virtual ~CMapData() = default;
};

/* 1538 */
struct _EQUIPKEY
{
  __int16 zItemIndex;
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
};

/* 1544 */
struct __cppobj __unaligned __declspec(align(1)) _LINK_DB_BASE
{
  struct _LIST
  {
    _LINKKEY Key;
  };

  _LIST m_LinkList[50];
  unsigned __int8 m_byLinkBoardLock;
  unsigned int m_dwSkill[2];
  unsigned int m_dwForce[2];
  unsigned int m_dwCharacter[2];
  unsigned int m_dwAnimus[2];
  unsigned int m_dwInven;
  unsigned int m_dwInvenBag[5];
};

/* 1545 */
struct _EMBELLKEY
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;
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
  };

  _EMBELLISH_LIST m_EmbellishList[7];
};

/* 1548 */
struct _FORCEKEY
{
  unsigned int dwKey;
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
  };

  _LIST m_List[88];
};

/* 1551 */
struct _ANIMUSKEY
{
  unsigned __int8 byItemIndex;
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
  };

  _LIST m_List[100];
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
  };

  _LIST m_List[20];
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
  };

  _LIST m_List[50];
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
};

/* 1578 */
struct __cppobj __unaligned __declspec(align(1)) _RETURNPOST_DB_BASE
{
  bool m_bUpdate;
  int m_nMax;
  int m_nCum;
  unsigned int m_RetSerials[30];
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
};

/* 1581 */
struct __cppobj _POSTDATA_DB_BASE
{
  _POSTSTORAGE_DB_BASE dbPost;
  _RETURNPOST_DB_BASE dbRetPost;
  _DELPOST_DB_BASE dbDelPost;
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
};

/* 1174 */
struct __cppobj __unaligned __declspec(align(1)) _NOT_ARRANGED_AVATOR_DB
{
  unsigned int dwSerial;
  unsigned __int8 byLv;
  unsigned __int8 byRaceSexCode;
  unsigned int dwDalant;
  unsigned int dwGold;
  char wszName[17];
  char szServer[33];
  char szClassCode[5];
};

/* 1593 */
struct __cppobj _SYNC_STATE
{
  bool bEnter;
  bool bReged;
  bool bSelect;
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
struct __cppobj __declspec(align(8)) CRadarItemMgr
{
  bool m_bUse;
  bool m_bUpdate;
  unsigned int m_dwStartTime;
  unsigned int m_dwDurTime;
  unsigned int m_dwDelayTime;
  CPlayer *m_pMaster;
  char m_strRadarCode[64];
  CMapData *m_pDestMap;
  bool m_bPlayerEnd;
  bool m_bMonEnd;
  int m_nPlayerNum;
  int m_nMonNum;
  bool m_bSameRace;
  bool m_bNorDiffRace;
  bool m_bChiefDiffRace;
  bool m_bEliteMonster;
  _detected_char_list m_RadarResult;
};

/* 1536 */
struct __cppobj __declspec(align(8)) CUserDB
{
  _GLBID m_gidGlobal;
  _CLID m_idWorld;
  unsigned int m_dwIP;
  unsigned int m_dwTotalPlayMin;
  char m_szAccountID[13];
  unsigned int m_dwAccountSerial;
  unsigned int m_ipAddress;
  unsigned __int8 m_byUserDgr;
  unsigned __int8 m_bySubDgr;
  char m_wszAvatorName[17];
  char m_aszAvatorName[17];
  unsigned int m_dwSerial;
  unsigned __int8 m_byNameLen;
  _REGED m_RegedList[3];
  _AVATOR_DATA m_AvatorData;
  _AVATOR_DATA m_AvatorData_bk;
  _NOT_ARRANGED_AVATOR_DB m_NotArrangedChar[50];
  unsigned int m_dwArrangePassCase0[50];
  bool m_bActive;
  bool m_bField;
  bool m_bWndFullMode;
  bool m_bDBWaitState;
  _DB_QRY_SYN_DATA *m_pDBPushData;
  bool m_bChatLock;
  _SYNC_STATE m_ss;
  unsigned int m_dwMessengerKey[4];
  unsigned int m_dwOperLobbyTime;
  bool m_bCreateTrunkFree;
  CMyTimer m_tmrCheckPlayMin;
  bool m_bDataUpdate;
  unsigned int m_dwTermContSaveTime;
  unsigned int m_dwLastContSaveTime;
  bool m_bNoneUpdateData;
  __declspec(align(4)) _BILLING_INFO m_BillingInfo;
  bool m_bBillingNoLogout;
  int m_nTrans;
  CRadarItemMgr m_RadarItemMgr;
  unsigned __int8 m_byUILock;
  bool m_bUILock_Updated;
  unsigned __int8 m_byUILock_InitFailCnt;
  unsigned __int8 m_byUILock_FailCnt;
  char m_szUILock_PW[13];
  char m_szAccount_PW[13];
  unsigned __int8 m_byUILock_HintIndex;
  char m_uszUILock_HintAnswer[17];
  unsigned __int8 m_byUILock_InitFindPassFailCount;
  unsigned __int8 m_byUILockFindPassFailCount;
  unsigned int m_dwRequestMoveCharacterSerialList[3];
  unsigned int m_dwTournamentCharacterSerialList[3];
  char m_szLobbyHistoryFileName[64];
  virtual ~CUserDB() = default;
};

/* 1597 */
struct __cppobj CPartyPlayer
{
  bool m_bLogin;
  __unaligned __declspec(align(1)) _CLID m_id;
  char m_wszName[17];
  unsigned __int16 m_wZoneIndex;
  CPartyPlayer *m_pPartyBoss;
  CPartyPlayer *m_pPartyMember[8];
  bool m_bLock;
  unsigned __int8 m_byLootShareSystem;
  CPartyPlayer *m_pLootAuthor;
  CDarkHole *m_pDarkHole;
};

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
struct __cppobj AutominePersonal : CCharacter
{
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

/* 1713 */
struct _mastery_lim_data
{
  int m_nBnsMMastery[2];
  int m_nBnsSMastery;
  int m_nBnsDefMastery;
  int m_nBnsPryMastery;
  int m_nBnsMakeMastery[3];
  int m_nBnsSkillMastery[8];
  int m_nBnsForceMastery[24];
};

/* 1715 */
struct __cppobj _class_fld : _base_fld
{
  struct _bns_item
  {
    char strDefaultItem[64];
    int nAmount;
  };

  int m_nRaceCode;
  int m_nClass;
  int m_nIconIDX;
  int m_nGrade;
  int m_nUpGradeLv;
  char m_strCh_Class[8][64];
  char m_strTemp[64];
  char m_strKorName[64];
  char m_strEngName[64];
  int m_nConLim;
  char m_strLinkClassSkill[10][64];
  int m_bUnitUsable;
  int m_bAnimusUsable;
  int m_bLauncherUsable;
  int m_bWMKToolUsable;
  int m_bDMKToolUnitUsable;
  int m_bBMKToolUnitUsable;
  int m_nMakeTrapMaxNum;
  int m_nBnsForHP;
  int m_nBnsForFP;
  int m_nBnsForSP;
  int m_nUpValueDefMastery;
  _mastery_lim_data m_MasteryLim;
  int m_bSelectRewardItem;
  _class_fld::_bns_item m_DefaultItem[9];
};

/* 1720 */
struct __cppobj _guild_master_info
{
  unsigned int dwSerial;
  unsigned __int8 byPrevGrade;
  _guild_member_info *pMember;
};

/* 1722 */
struct __cppobj _suggested_matter
{
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
struct __cppobj __declspec(align(8)) CGuild
{
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
  virtual ~CGuild() = default;
};

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
};

/* 1690 */

/* 1693 */

/* 1742 */
struct __cppobj __declspec(align(8)) CGuardTower : CCharacter
{
  int m_nHP;
  CPlayer *m_pMasterTwr;
  unsigned int m_dwMasterSerial;
  unsigned __int8 m_byRaceCode;
  _STORAGE_LIST::_db_con *m_pItem;
  unsigned __int16 m_wItemSerial;
  bool m_bSystemStruct;
  int m_nIniIndex;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bQuick;
  CCharacter *m_pTarget;
  CCharacter *m_pMasterSetTarget;
  unsigned int m_dwLastDestroyTime;
};

/* 1746 */
struct __cppobj CTrap : CCharacter
{
  int m_nHP;
  CPlayer *m_pMaster;
  unsigned __int8 m_byRaceCode;
  unsigned int m_dwMasterSerial;
  char m_wszMasterName[17];
  char m_aszMasterName[17];
  long double m_dMasterPvPPoint;
  unsigned int m_dwStartMakeTime;
  bool m_bComplete;
  bool m_bBreakTransparBuffer;
  unsigned int m_dwLastDestroyTime;
  int m_nTrapMaxAttackPnt;
};

/* 1752 */
struct _happen_event_condition_node
{
  int m_nCondType;
  int m_nCondSubType;
  char m_sCondVal[64];
};

/* 1753 */
struct _happen_event_node
{
  int m_bUse;
  int m_bQuestRepeat;
  int m_nQuestType;
  int m_bSelectQuestManual;
  int m_nAcepProNum;
  int m_nAcepProDen;
  _happen_event_condition_node m_CondNode[5];
  char m_strLinkQuest[5][64];
};

/* 1686 */
struct __cppobj _dh_mission_mgr
{
  struct _count
  {
    int nCount;
    bool bPass;
  };

  struct _if_change
  {
    _dh_mission_setup *pMissionPtr;
    char *pszDespt;
    char *pszComMsg;
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
};

/* 1688 */
struct __cppobj __declspec(align(8)) _dh_player_mgr
{
  struct _pos
  {
    CMapData *pMap;
    unsigned __int16 wLayer;
    float fPos[3];
  };

  CPlayer *pOne;
  unsigned int dwSerial;
  _pos LastPos;
  int nEnterOrder;
};

/* 1598 */
struct __cppobj __declspec(align(8)) CDarkHoleChannel
{
  unsigned __int16 m_wChannelIndex;
  unsigned int m_dwChannelSerial;
  CDarkHole *m_pHoleObj;
  unsigned int m_dwHoleSerial;
  unsigned int m_dwQuestStartTime;
  _dh_quest_setup *m_pQuestSetup;
  unsigned __int16 m_wLayerIndex;
  _LAYER_SET *m_pLayerSet;
  _dh_mission_mgr m_MissionMgr;
  char m_wszOpenerName[17];
  char m_aszOpenerName[17];
  unsigned int m_dwOpenerSerial;
  int m_nOpenerDegree;
  int m_nOpenerSubDegree;
  bool m_bCheckMemberClose;
  CPartyPlayer *m_pPartyMng;
  _dh_player_mgr m_Quester[32];
  _dh_player_mgr *m_pLeaderPtr;
  unsigned int m_dwEnterOrderCounter;
  unsigned int m_dwNextCloseTime;
  unsigned int m_dwSendNewMissionMsgNextTime;
  CIndexList m_listEnterMember;
  bool m_bMoveNextMission;
  virtual ~CDarkHoleChannel() = default;
};

/* 1435 */
struct __cppobj __declspec(align(4)) _dummy_position
{
  char m_szCode[64];
  unsigned __int16 m_wLineIndex;
  bool m_bPosAble;
  __int16 m_zLocalMin[3];
  __int16 m_zLocalMax[3];
  float m_fMin[3];
  float m_fMax[3];
  float m_fRT[3];
  float m_fLB[3];
  float m_fCenterPos[3];
  float m_fDirection[3];
  unsigned __int16 m_wActiveMon;
};

/* 1773 */
struct __cppobj __declspec(align(8)) CParkingUnit : CGameObject
{
  CPlayer *m_pOwner;
  unsigned int m_dwOwnerSerial;
  unsigned __int8 m_byFrame;
  unsigned __int8 m_byPartCode[6];
  unsigned __int8 m_byCreateType;
  unsigned __int8 m_byTransDistCode;
  unsigned int m_dwParkingStartTime;
  unsigned __int16 m_wHPRate;
  unsigned int m_dwLastDestroyTime;
};

/* 1776 */
struct __cppobj CAITimer
{
  unsigned int m_BefTime;
  unsigned int m_Delay;
  unsigned int m_DDelay;
};

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
};

/* 1779 */
struct __cppobj CAnimus : CCharacter
{
  unsigned __int8 m_byClassCode;
  int m_nHP;
  int m_nFP;
  unsigned __int64 m_dwExp;
  CPlayer *m_pMaster;
  unsigned int m_dwMasterSerial;
  char m_wszMasterName[17];
  char m_aszMasterName[17];
  unsigned __int8 m_byRoleCode;
  unsigned int m_dwLastDestroyTime;
  float m_fMoveSpeed;
  unsigned __int8 m_byPosRaceTown;
  CMapData *m_pBeforeTownCheckMap;
  float m_fBeforeTownCheckPos[2];
  unsigned int m_dwStunTime;
  unsigned int m_dwBeAttackedTargetTime;
  CCharacter *m_pNextTarget;
  int m_nMaxAttackPnt;
  unsigned int m_tmNextEatMasterFP;
  _animus_fld *m_pRecord;
  int m_nMaxHP;
  int m_nMaxFP;
  float m_Mightiness;
  int m_DefPart[5];
  unsigned int m_dwAIMode;
  CCharacter *m_pTarget;
  CAITimer m_AITimer[3];
  SKILL m_Skill[2];
};

/* 1314 */

/* 1316 */

/* 1422 */
union $1D4D54E2B5971D5BE0EAD557ED232A85
{
  CEntity *mEnvEntity[2];
  CParticle *mEnvParticle[2];
};

/* 1402 */
struct __cppobj CMergeFileManager
{
  char mPath[256];
  unsigned int mPathNameLeng;
  unsigned int mMergeFileNum;
  CMergeFile *mMergeFile;
};

/* 1416 */
struct _PATH_NODE
{
  float TotalLeng;
  float Path[17][3];
  int WhatDirection;
  unsigned __int16 FrontLinetId;
  unsigned __int16 BackLineId;
  int PathCnt;
  int IsFind;
};

/* 1417 */
struct __cppobj CPathFinder
{
  int mMaxDepth;
  _PATH_NODE mPathNode[12];
  float mStart[3];
  _PATH_NODE mStackPathNode[20];
  int mStackPoint;
};

/* 1390 */
union $36F8BC5CB18F337A8B949972D0D6A08A
{
  IDirect3DVertexBuffer8 *m_lpVertexBuffer;
  _D3DR3VERTEX_TEX1 *m_VertexBufferTex1;
  _D3DR3VERTEX_TEX2 *m_VertexBufferTex2;
};

/* 1391 */
struct __cppobj CVertexBuffer
{
  unsigned int m_Flag;
  unsigned int m_Size;
  $36F8BC5CB18F337A8B949972D0D6A08A ___u2;
};

/* 1392 */
union $736DB51CE9487475C2E813E4336CA27C
{
  IDirect3DIndexBuffer8 *m_lpIndexBuffer;
  unsigned __int16 *m_IndexBuffer;
};

/* 1393 */
struct __cppobj CIndexBuffer
{
  unsigned int m_Flag;
  unsigned int m_Size;
  $736DB51CE9487475C2E813E4336CA27C ___u2;
};

/* 1418 */
struct _ENTRY
{
  unsigned int offset;
  unsigned int size;
};

/* 1419 */
struct _BSP_FILE_HEADER
{
  unsigned int version;
  _ENTRY CPlanes;
  _ENTRY CFaceId;
  _ENTRY Node;
  _ENTRY Leaf;
  _ENTRY MatListInLeaf;
  _ENTRY Object;
  _ENTRY Track;
  _ENTRY EventObjectID;
  _ENTRY ReadSpare[35];
  _ENTRY BVertex;
  _ENTRY WVertex;
  _ENTRY FVertex;
  _ENTRY VertexColor;
  _ENTRY UV;
  _ENTRY LgtUV;
  _ENTRY Face;
  _ENTRY FaceId;
  _ENTRY VertexId;
  _ENTRY ReadMatGroup;
  _ENTRY FreeSpare[32];
};

/* 1420 */
struct _EXT_BSP_FILE_HEADER
{
  unsigned int version;
  _ENTRY CFVertex;
  _ENTRY CFLine;
  _ENTRY CFLineId;
  _ENTRY CFLeaf;
  _ENTRY EntityList;
  _ENTRY EntityID;
  _ENTRY LeafEntityList;
  _ENTRY SoundEntityID;
  _ENTRY LeafSoundEntityList;
  _ENTRY ReadSpare[18];
  _ENTRY MapEntitiesList;
  _ENTRY SoundEntityList;
  _ENTRY SoundEntitiesList;
  _ENTRY FreeSpare[18];
};

/* 1421 */
struct __cppobj CAlpha
{
  unsigned int mAlphaFaceCnt;
  unsigned int *mAlphaFace;
  __int16 *mAlphaFaceZ;
  unsigned int *mTempAlphaFace;
  __int16 *mTempAlphaFaceZ;
  int mAlphaSize;
  void *mBsp;
};

/* 1423 */
struct __cppobj CBsp
{
  float (*mCVertex)[3];
  float (*mCNNormal)[3];
  unsigned int *mCVertexId;
  _BSP_C_FACE *mCFace;
  float (*mCNEdgeNormal)[4];
  unsigned int *mCFaceId;
  unsigned int *mVertexColor;
  __int16 (*mLgtUV)[2];
  int mNowCFaceId;
  unsigned __int16 *MatListInLeafId;
  unsigned int mMatGroupNum;
  _BSP_MAT_GROUP *mMatGroup;
  unsigned int mCVertexNum;
  unsigned int mCFaceNum;
  unsigned int mVertexNum;
  unsigned int mFaceNum;
  int mIsLoaded;
  unsigned int mObjectNum;
  _ANI_OBJECT *mObject;
  unsigned __int16 *mEventObjectID;
  unsigned int mEnvID[2];
  $1D4D54E2B5971D5BE0EAD557ED232A85 ___u21;
  CMergeFileManager mMapEntityMFM;
  int mIsLoadEBP;
  unsigned __int8 *mEntityCache;
  unsigned int mEntityCacheSize;
  unsigned int mNowRenderMatGroupNum;
  unsigned int mNowShadowMatGroupNum;
  CEntity *mEntity;
  CParticle *mParticle;
  _ENTITY_LIST *mEntityList;
  unsigned __int16 *mEntityID;
  _LEAF_ENTITIES_LIST_INFO *mLeafEntityList;
  _MAP_ENTITIES_LIST *mMapEntitiesList;
  unsigned int mEntityListNum;
  unsigned int mLeafEntityListNum;
  unsigned int mEntityIDNum;
  unsigned int mMapEntitiesListNum;
  unsigned __int8 *mSoundEntityCache;
  unsigned int mSoundEntityCacheSize;
  unsigned int mSoundEntityIDNum;
  unsigned int mLeafSoundEntityListNum;
  unsigned int mSoundEntityListNum;
  unsigned int mSoundEntitiesListNum;
  _SOUND_ENTITY_LIST *mSoundEntityList;
  _SOUND_ENTITIES_LIST *mSoundEntitiesList;
  unsigned __int16 *mSoundEntityID;
  _LEAF_SOUND_ENTITIES_LIST_INFO *mLeafSoundEntityList;
  unsigned int mTotalAllocSize;
  unsigned int mTotalWaveSize;
  unsigned int mVertexBufferSize;
  _BSP_NODE *mNode;
  _BSP_LEAF *mLeaf;
  unsigned int mLeafNum;
  unsigned int mNodeNum;
  unsigned int mCFVertexNum;
  unsigned int mCFLineNum;
  unsigned int mCFLineIdNum;
  float (*mCFVertex)[3];
  _TOOL_COL_LINE *mCFLine;
  unsigned __int16 *mCFLineId;
  _TOOL_COL_LEAF *mCFLeaf;
  float (*mCFVNormal)[3];
  float (*mCFNormal)[4];
  CExtDummy *mDummy;
  CPathFinder mPathFinder;
  int mColFaceId;
  unsigned int mFindPathCnt;
  __int16 mNowPlayerNum;
  unsigned int mStaticVBCnt;
  CVertexBuffer mStaticVertexBuffer[80];
  unsigned int mVBVertexNum[80];
  CIndexBuffer mStaticIndexedBuffer;
  void *mMultiLayerUV;
  void *mMultiLayerST;
  unsigned __int8 *mStaticAlloc;
  unsigned int mStaticAllocSize;
  unsigned __int8 *mExtBspStaticAlloc;
  unsigned int mExtBspStaticAllocSize;
  int mPickPoly;
  unsigned __int8 *mMatGroupCache;
  int mMatGroupCacheSize;
  float mTempCamera[3];
  __int16 mTempSearchOk;
  __int16 mNowLeafNum;
  _BSP_FILE_HEADER mBSPHeader;
  _EXT_BSP_FILE_HEADER mExtBSPHeader;
  CAlpha mAlpha;
};

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
struct __cppobj __unaligned __declspec(align(4)) CEntity
{
  int mIsUseMemory;
  int mIsEntityLoad;
  int mIsAlpha;
  _ENTITY_FILE_HEADER mHeader;
  unsigned int mVetexBufferSize;
  float (*mOrgUV)[2];
  unsigned int mMapColor;
  unsigned int mFlag;
  _R3MATERIAL *mMat;
  CVertexBuffer mStaticVertexBuffer;
  CIndexBuffer mStaticIndexedBuffer;
  unsigned int mTextureSize;
  int mMatGroupNum;
  _ENTITY_M_GROUP *mMatGroup;
  int mObjectNum;
  _ANI_OBJECT *mObject;
  unsigned __int8 *mTrack;
  int mStartTexID;
  int mTexNum;
  unsigned int mVertexNum;
  void *mIndependencyTex;
  float mStartTime;
  float mBBMin[3];
  float mBBMax[3];
  float mAddFrame;
  float mFrame;
};

/* 1424 */
struct __cppobj CSkyBox
{
  int mIsSkyLoad;
  CEntity mEntity;
  _ENTITY_M_GROUP *mMatGroup;
  int mMatNum;
  int mMatGroupNum;
  __int16 mMatGroupSort[512];
};

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
  __int16 m_zCurMonNum;
  unsigned int m_dwCumMonNum;
  unsigned int m_dwLastRespawnTime;
  int m_nLimRegenNum;
  BossSchedule *m_pBossSchedule;
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
};

/* 1517 */
struct __cppobj _portal_dummy
{
  _portal_fld *m_pPortalRec;
  _dummy_position *m_pDumPos;
};

/* 1518 */
struct __cppobj _store_dummy
{
  int m_nStoreType;
  _base_fld *m_pStoreRec;
  _dummy_position *m_pDumPos;
};

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
};

/* 1522 */
struct __cppobj _quest_dummy
{
  _dummy_position *m_pDumPos;
};

/* 1466 */
struct __cppobj _map_fld : _base_fld
{
  char m_strFileName[64];
  int m_nMapType;
  int m_nLayerNum;
  int m_nRaceVillageCode;
  int m_nMonsterSetFileNum;
  int m_nMapClass;
  int m_nRadius;
  int m_nLevelLimit;
  int m_nUpLevelLim;
  int m_nPotionLim;
  int m_nRacePvpUsable[3];
};

/* 1523 */
struct __cppobj _safe_dummy
{
  _dummy_position *m_pDumPos;
};

/* 1525 */

/* 1537 */

/* 1600 */
struct __cppobj __declspec(align(4)) CDarkHole : CGameObject
{
  CDarkHoleChannel *m_pChannel;
  unsigned int m_dwChannelSerial;
  unsigned int m_dwOpenerSerial;
  char m_wszOpenerName[17];
  char m_aszOpenerName[17];
  unsigned int m_dwNextCloseTime;
  unsigned int m_dwNextHurryTime;
  bool m_bHurry;
};

/* 1705 */
struct __cppobj __unaligned __declspec(align(1)) AP_BatterySlot
{
  bool m_bFill;
  _STORAGE_LIST::_db_con battery_;
};

/* 1718 */

/* 1721 */
struct __cppobj __declspec(align(8)) _guild_applier_info
{
  CPlayer *pPlayer;
  unsigned int dwApplyTime;
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
  int nItemNum;
  _STORAGE_LIST::_db_con *Item[128];
  unsigned int m_dwGivePercent[128];
  long double dExp;
  unsigned int dwPvp;
  unsigned int dwDalant;
  virtual ~_dh_reward_sub_setup() = default;
};

/* 1630 */
struct __cppobj __dp_mission_potal
{
  int m_nNums;
  int m_nCodeLen[32];
  char m_strCodes[32][64];
};

/* 1601 */
struct __cppobj _dh_quest_setup
{
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
  virtual ~_dh_quest_setup() = default;
};

/* 1603 */
struct __cppobj __declspec(align(8)) _dh_mission_setup
{
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
  virtual ~_dh_mission_setup() = default;
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
  _react_obj ReactObj;
  _react_area ReactArea;
  unsigned int dwTermMSec;
  int nLim;
  bool bCallEvent;
  char *pszDefineCode;
};

/* 1637 */
struct __cppobj CLootingMgr
{
  struct _list
  {
    CPlayer *pAtter;
    unsigned int dwAtterSerial;
    unsigned int dwAttCount;
    unsigned int dwDamage;
    unsigned int dwLastAttTime;
  };

  bool m_bFirst;
  unsigned __int8 m_byUserNode;
  _list m_AtterList[64];
};

/* 1638 */
struct __cppobj CAggroNode
{
  CCharacter *m_pCharacter;
  unsigned int m_dwObjectSerial;
  int m_nAggroData;
  int m_nDamageData;
  int m_nKingPowerDamage;
};

/* 1639 */
struct __cppobj CMonsterAggroMgr
{
  CCharacter *m_pTopAggroCharacter;
  CCharacter *m_pTopDamageCharacter;
  CCharacter *m_pKingPowerDamageCharacter;
  CAggroNode m_AggroPool[10];
  unsigned int m_dwAggroCount;
  unsigned int m_dwAllResetLastTime;
  unsigned int m_dwShortRankLastTime;
  unsigned int m_dwAllResetTimer;
  unsigned int m_dwShortRankTimer;
  CMonster *m_pMonster;
};

/* 1640 */
struct __cppobj __declspec(align(8)) CMonsterHierarchy
{
  unsigned int m_dwTotalCount;
  CMonster *m_pThisMon;
  CMonster *m_pParentMon;
  unsigned int m_dwParentSerial;
  CMonster *m_pChildMon[3][10];
  unsigned int m_dwMonCount[3];
  unsigned __int8 m_byChildMonSetNum;
  unsigned int m_dwChildRecallTime;
  virtual ~CMonsterHierarchy() = default;
};

/* 1642 */
struct __cppobj __declspec(align(8)) MonsterSFContDamageToleracne
{
  CMonster *m_pMonster;
  float m_fToleranceProb;
  float m_fToleranceProbMax;
  unsigned int m_dwLastUpdateTime;
};

/* 1682 */
struct $8861CABC75A24E330D1CBB803A094EF8
{
  unsigned __int32 m_nMove_State : 1;
  unsigned __int32 m_nCombat_State : 1;
  unsigned __int32 m_nEmotion_State : 3;
};

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
};

/* 1647 */
struct __cppobj __declspec(align(8)) CMonsterSkill
{
  bool m_bExit;
  int m_UseType;
  int m_nSFCode;
  unsigned __int16 m_wSFIndex;
  _base_fld *m_pSF_Fld;
  _monster_sp_fld *m_pSPConst;
  unsigned int m_BefTime;
  unsigned int m_dwDelayTime;
  unsigned int m_dwCastDelay;
  float m_fAttackDist;
  int m_nMotive;
  int m_nMotivevalue;
  int m_nCaseType;
  int m_nAccumulationCount;
  int m_nSFLv;
  int m_Element;
  int m_StdDmg;
  int m_MinDmg;
  int m_MaxDmg;
  int m_MinProb;
  int m_MaxProb;
};

/* 1648 */
struct __cppobj CMonsterSkillPool
{
  CMonster *m_pMyMon;
  int m_nSize;
  CMonsterSkill m_MonSkill[16];
};

/* 1649 */
struct _event_loot_item
{
  unsigned __int8 byItemTable;
  unsigned __int16 wItemIndex;
};

/* 1668 */
struct __cppobj Us_FSM_Node
{
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

/* 1660 */
struct __cppobj Us_HFSM
{
  int m_bSet;
  void *m_pObject;
  unsigned int m_dwUsedCount;
  Us_FSM_Node m_ArNode[10];
  UsPoint<UsStateTBL> m_spShareStateTBLPtr;
  virtual ~Us_HFSM() = default;
};

/* 1670 */
struct __cppobj SF_Timer
{
  unsigned int m_dwLastCheckTime;
  unsigned int m_dwGapCheckTime;
};

/* 1671 */
struct __cppobj __declspec(align(8)) CPathMgr
{
  float m_PosPool[16][3];
  unsigned __int8 m_Size;
  unsigned __int8 m_StartPos;
  virtual ~CPathMgr() = default;
};

/* 1673 */
struct __cppobj CMonsterAI : Us_HFSM
{
  SF_Timer m_SFCheckTime[4];
  CMonster *m_pAsistMonster;
  CPathMgr m_PathFinder;
  unsigned int m_dwBattleModeTime;
  int m_nCurPathFindFailCount;
};

/* 1680 */
struct __cppobj CLuaSignalReActor
{
  struct _Action
  {
    unsigned __int8 m_bySignalCode;
    CLuaCommandEx *m_pLuaCommandEx;
  };

  _Action m_ActionData[3];
};

/* 1635 */
struct __cppobj CMonster : CCharacter
{
  bool m_bOper;
  bool m_bApparition;
  bool m_bDungeon;
  unsigned int m_dwLastDestroyTime;
  unsigned int m_dwDestroyNextTime;
  unsigned int m_dwLastRecoverTime;
  float m_fCreatePos[3];
  float m_fLookAtPos[3];
  bool m_bRobExp;
  bool m_bRewardExp;
  bool m_bStdItemLoot;
  _mon_active *m_pActiveRec;
  _monster_fld *m_pMonRec;
  _dummy_position *m_pDumPosition;
  int m_nHP;
  CLootingMgr m_LootMgr;
  CMonsterAggroMgr m_AggroMgr;
  CMonsterHierarchy m_MonHierarcy;
  MonsterSFContDamageToleracne m_SFContDamageTolerance;
  unsigned __int8 m_byCreateDate[4];
  unsigned int m_LifeMax;
  unsigned int m_LifeCicle;
  $B1473DED9FF3433080ADDC1400B1BAA4 ___u23;
  EmotionPresentationChecker m_EmotionPresentationCheck;
  float m_fYAngle;
  float m_fStartLookAtPos[3];
  bool m_bRotateMonster;
  MonsterStateData m_MonsterStateData;
  MonsterStateData m_BeforeMonsterStateData;
  CCharacter *m_pTargetChar;
  CMonsterSkillPool m_MonsterSkillPool;
  int m_DefPart[5];
  int m_nEventItemNum;
  _event_loot_item m_eventItem[16];
  _event_respawn *m_pEventRespawn;
  _event_set *m_pEventSet;
  CMonsterAI m_AI;
  CLuaSignalReActor m_LuaSignalReActor;
};

/* 1775 */
struct __cppobj _animus_fld : _base_fld
{
  int m_nLevel;
  unsigned __int64 m_nForLvUpExp;
  int m_nUseFP;
  float m_fPenalty;
  float m_fAttGap;
  int m_nAttack_DP;
  int m_nAttFcStd;
  int m_nMinAFSelProb;
  int m_nMaxAFSelProb;
  int m_nAttSklUnit;
  int m_nDefSklUnit;
  float m_fWeakPart;
  int m_nStdDefFc;
  float m_fDefGap;
  float m_fDefFacing;
  int m_nFireTol;
  int m_nWaterTol;
  int m_nSoilTol;
  int m_nWindTol;
  int m_nForceLevel;
  int m_nForceMastery;
  int m_nForceAttStd;
  char m_strAttTechID1[64];
  int m_nAttTech1UseProb;
  int m_nAttTechID1MotionTime;
  char m_strPSecTechID[64];
  int m_nPSecTechIDMotionTime;
  char m_strMSecTechID[64];
  int m_nMSecTechIDMotionTime;
  int m_nMaxHP;
  int m_nHPRecDelay;
  int m_nHPRecUnit;
  int m_nMaxFP;
  int m_nFPRecDelay;
  int m_nFPRecUnit;
  int m_nAttSpd;
  int m_nAttMoTime1;
  int m_nAttMoTime2;
  int m_nCrtMoTime;
  int m_nViewExt;
  int m_nRefExt;
  int m_nAttExt;
  int m_nMovSpd;
  int m_nScaleRate;
  int m_nWidth;
  int m_nAttEffType;
  int m_nDefEffType;
};

/* 1321 */
struct _BSP_C_FACE
{
  unsigned __int8 Attr;
  unsigned __int8 VNum;
  __unaligned __declspec(align(1)) unsigned int VStartId;
  unsigned __int16 MatGIndex;
  float Normal[4];
};

/* 1322 */
struct __unaligned __declspec(align(2)) _BSP_MAT_GROUP
{
  unsigned __int16 Type;
  unsigned __int16 TriNum;
  __int16 MtlId;
  __int16 LgtId;
  float BBMin[3];
  float BBMax[3];
  float Origin[3];
  unsigned int VBMinIndex;
  unsigned int IBMinIndex;
  unsigned int VertexBufferId;
  unsigned int VCnt;
  unsigned int CFaceStartVId;
  void *MultiSourceUV;
  void *MultiSourceST;
  unsigned __int16 ObjectId;
  float CoronaAlpha;
};

/* 1397 */
union $3E60C3DACEF2E33FF1D1871D4F2565FA
{
  float mElasticity;
  float mTexRepeat;
};

/* 1398 */
struct __cppobj CParticle
{
  char mEntityName[128];
  int mNum;
  _PARTICLE_ELEMENT *mElement;
  CEntity *mEntity;
  float mTotalTime;
  float mLiveTime;
  float mStartTimeRange;
  float mTimeSpeed;
  float mGravity[3];
  float mStartPower[2][3];
  float mStartScale[2];
  float mStartZRot[2];
  float mStartYRot[2];
  unsigned __int8 mFlickerAlpha;
  float mFlickerTime;
  float mStartARGB[4][2];
  float mOnePerTimeEpsilon;
  float mRotMat[4][4];
  $3E60C3DACEF2E33FF1D1871D4F2565FA ___u18;
  float mZFront;
  float mEmitTime;
  unsigned __int16 mSpecialID;
  unsigned __int16 mTrackCnt;
  float mTimeTrack[12];
  unsigned __int8 mTrackFlag[12];
  unsigned __int8 mATrack[12];
  unsigned __int8 mRTrack[12][2];
  unsigned __int8 mGTrack[12][2];
  unsigned __int8 mBTrack[12][2];
  float mScaleTrack[12][2];
  float mZRotTrack[12][2];
  float mYRotTrack[12][2];
  float mPowerTrack[12][2][3];
  float mSpecialARGV[2][3];
  void *mBsp;
  unsigned int mFlag;
  unsigned int mAlphaType;
  float mStartPos[2][3];
  float mCreatePos[3];
  int mState;
  float mOnePerTime;
  float mOnePerTimeEpsilonTemp;
  float mParticleTimer;
  float mNextCreatorTime;
};

/* 1401 */
struct CMergeFile
{
  char mFileName[256];
  _MERGE_FILE *mMergeFNF;
  unsigned int *mOffset;
  unsigned int *mIndex;
  unsigned int mCnt;
  unsigned int mHeaderSize;
};

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
struct _BSP_NODE
{
  unsigned int f_normal_id;
  float d;
  __int16 front;
  __int16 back;
  __int16 bb_min[3];
  __int16 bb_max[3];
};

/* 1410 */
struct __unaligned __declspec(align(1)) _BSP_LEAF
{
  unsigned __int8 type;
  unsigned __int16 face_num;
  unsigned int face_start_id;
  unsigned __int16 m_group_num;
  unsigned int m_group_start_id;
  __int16 bb_min[3];
  __int16 bb_max[3];
};

/* 1411 */
struct _TOOL_COL_LINE
{
  unsigned int attr;
  unsigned __int16 start_v;
  unsigned __int16 end_v;
  float height;
  unsigned __int16 front;
  unsigned __int16 back;
};

/* 1412 */
struct __unaligned __declspec(align(2)) _TOOL_COL_LEAF
{
  unsigned int start_id;
  unsigned __int16 line_num;
};

/* 1332 */
/* 1357 */
struct __cppobj IDirect3DResource8 : IUnknown
{
};

/* 1367 */
struct __cppobj IDirect3DVertexBuffer8 : IDirect3DResource8
{
};

/* 1385 */
union $B3185CC329BDC5A6DA4B2C96D7DC3DD4
{
  float x;
  float pos[1];
};

/* 1386 */
union $25C8602110B22A2EB54BB3EE6752756B
{
  float nx;
  float normal[1];
};

/* 1387 */
struct _D3DR3VERTEX_TEX1
{
  $B3185CC329BDC5A6DA4B2C96D7DC3DD4 ___u0;
  float y;
  float z;
  $25C8602110B22A2EB54BB3EE6752756B ___u3;
  float ny;
  float nz;
  unsigned int color;
  float u;
  float v;
};

/* 1388 */
union $42A39AC029B3127F486DF502FF8A7E09
{
  float x;
  float local[1];
};

/* 1389 */
struct _D3DR3VERTEX_TEX2
{
  $42A39AC029B3127F486DF502FF8A7E09 ___u0;
  float y;
  float z;
  $25C8602110B22A2EB54BB3EE6752756B ___u3;
  float ny;
  float nz;
  unsigned int color;
  float uv[2][2];
};

/* 1368 */
struct __cppobj IDirect3DIndexBuffer8 : IDirect3DResource8
{
};

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

/* 1432 */
struct __cppobj _mon_active_fld : _base_fld
{
  unsigned int m_dwRegenTime;
  unsigned int m_dwRegenLimNum;
  unsigned int m_dwRegenProp;
  unsigned int m_dwRegenMinNum;
  unsigned int m_dwStdKill;
  unsigned int m_dwRegenMaxNum;
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
};

/* 1434 */
struct __cppobj _mon_block_fld : _base_fld
{
  struct _dummy_info
  {
    char m_strDummyCode[64];
    unsigned int m_dwSelectProp;
  };

  unsigned int m_dwDummyNum;
  _dummy_info m_DummyInfo[20];
  int m_nMin;
  int m_nMob;
  int m_nMax;
};

/* 1516 */
struct __cppobj _portal_fld : _base_fld
{
  char m_strLinkMapCode[64];
  char m_strLinkPortalCode[64];
  char m_szMenu[64];
  int m_bNeedConCheck;
  int m_nNeedChrLevel;
  int m_nUpLevelLim;
  _consume_item_list m_ConsumeItemList[3];
  int m_nKind;
  char m_strUseRace[64];
};

/* 1602 */

/* 1632 */

/* 1615 */
struct __cppobj __dummy_block
{
  char *pszBlockName;
  int nSubDummyNum;
  _dummy_position *pSubDummy[32];
};

/* 1610 */
struct __cppobj __monster_group
{
  char *pszGroupName;
  int nSubMonsterNum;
  _monster_fld *pSubMonster[32];
};

/* 1604 */

/* 1619 */
struct __cppobj __add_monster
{
  _react_obj ReactObj;
  _react_area ReactArea;
};

/* 1620 */
struct __cppobj __add_loot_item
{
  unsigned __int8 byItemTableCode;
  _base_fld *pItemFld;
  unsigned int dwDur;
  _react_obj ReactObj;
};

/* 1621 */
struct __cppobj __change_monster
{
  _monster_fld *pMonsterFldA;
  _monster_fld *pMonsterFldB;
  int nProb;
  char *pszIfMissionDescirptCode;
  char *pszifCompleteMsg;
};

/* 1622 */
struct __cppobj __inner_check
{
  _react_area ReactArea_Evt;
  _react_area ReactArea_Aft;
  _react_obj ReactObj;
  char *pszMsg;
  char *pszRespawnCode;
};

/* 1623 */
struct __cppobj __respond_check
{
  unsigned __int8 byActTableCode;
  _base_fld *pActObjFld;
  _react_obj ReactObj;
  _react_area ReactArea;
  char *pszMsg;
};

/* 1625 */
struct __cppobj __add_time
{
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
  _base_fld *pEventFld;
  unsigned __int8 byTable;
  int nEventCount;
};

/* 1626 */
struct __cppobj __declspec(align(8)) _dh_job_setup
{
  char szJobTitle[33];
  EM_DH_EVENT eventType;
  _job_sub_setup JobSetup;
  int nReactNum;
  _react_sub_setup *ReactSetup[10];
  char szDescirptCode[17];
  virtual ~_dh_job_setup() = default;
};

/* 1608 */
struct __cppobj _monster_fld : _base_fld
{
  struct __child_mon
  {
    char strChildMon[64];
    int nChildMonNum;
  };

  struct _EmotionPresentation
  {
    int m_nEmotionCon;
    int m_nEmotionClass;
    char m_strEmotionCode[64];
  };

  char m_strName[64];
  char m_strEffectCode[64];
  int m_nMobGrade;
  int m_nRaceCode;
  int m_nMobRace;
  int m_nKillPoint;
  int m_nToCombatTime;
  int m_nPincerCnt;
  int m_nPreAttRange;
  int m_nMinMoveDistance;
  int m_nMaxMoveDistance;
  int m_nMobAlienation;
  int m_nMinMoveArea;
  int m_nMaxMoveArea;
  int m_nGuardRecallTimeMS;
  int m_nGuardingArea;
  float m_fTarDecType;
  int m_nAPTime;
  int m_nAPReset;
  int m_nUglierType;
  float m_fLevel;
  int m_bMonsterCondition;
  int m_nCriticalTol;
  int m_bExpDown;
  int m_nUpLooting;
  int m_nDnLooting;
  float m_fExt;
  float m_fAttFcStd;
  int m_nAttack_DP;
  int m_nProperty;
  float m_fAttGap;
  int m_bAttRangeType;
  int m_nAttType;
  float m_fMinAFSelProb;
  float m_fMaxAFSelProb;
  float m_fAttSklUnit;
  float m_fDefSklUnit;
  float m_fWeakPart;
  int m_bUseDefence;
  float m_fStdDefFc;
  float m_fDefGap;
  float m_fDefFacing;
  int m_nShieldBlock;
  int m_nBlockPer;
  float m_fFireTol;
  float m_fWaterTol;
  float m_fSoilTol;
  float m_fWindTol;
  char m_strSPCode[15][64];
  float m_fForceLevel;
  float m_fForceMastery;
  float m_fForceAttStd;
  char m_strAttTechID1[64];
  float m_fAttTech1UseProb;
  float m_fAttTechID1MotionTime;
  char m_strAttTechID2[64];
  float m_fAttTech2UseProb;
  float m_fAttTechID2MotionTime;
  char m_strAttTechID3[64];
  float m_fAttTech3UseProb;
  float m_fAttTechID3MotionTime;
  char m_strPSecTechID[64];
  float m_fPSecTechIDMotionTime;
  char m_strMSecTechID[64];
  float m_fMSecTechIDMotionTime;
  int m_nInjuryLimit;
  float m_fMaxHP;
  float m_fHPRecDelay;
  float m_fHPRecUnit;
  float m_fAttSpd;
  float m_fAttMoTime1;
  float m_fAttMoTime2;
  float m_fCrtMoTime;
  int m_nViewAngle;
  int m_nViewAngleCap;
  int m_nCapacityValue;
  float m_fViewExt;
  float m_fAttExt;
  float m_fMRefExt;
  float m_fCopTime;
  float m_fMovSpd;
  float m_fWarMovSpd;
  float m_fScaleRate;
  int m_bScaleChange;
  float m_fWidth;
  float m_fWaitTime;
  int m_nAsitReqRate;
  int m_nAsitAptRate;
  int m_nAsitType;
  __child_mon m_Child[3];
  float m_fEmoType;
  float m_fOffensiveRate;
  int m_nOffensiveType;
  float m_fDamHPStd;
  float m_fEmoImpStdTime;
  float m_fGoodToOrdHPPer;
  float m_fOrdToBadHPPer;
  float m_fBadToWorseHPPer;
  float m_fEspTFProb;
  float m_fTypeCompTerms;
  float m_fPSecTechChat;
  float m_fPAttTechChat;
  float m_fEmo0Chat;
  float m_fEmo0ChatProb;
  float m_fEmo1Chat;
  float m_fEmo1ChatProb;
  float m_fEmo2Chat;
  float m_fEmo2ChatProb;
  float m_fEmo3Chat;
  float m_fEmo3ChatProb;
  float m_fEmo4Chat;
  float m_fEmo4ChatProb;
  float m_fAsitReqSteEspChat;
  float m_fAsitReqSteEspChatProb;
  float m_fAsitReqSteHelpChat;
  float m_fAsitReqSteHelpChatProb;
  float m_fAsitReqSteCopChat;
  float m_fAsitReqSteCopChatProb;
  _EmotionPresentation m_EmotionChecker[5];
  int m_nAttEffType;
  int m_nDefEffType;
};

/* 1641 */

/* 1651 */
/* 1655 */
struct __cppobj _event_respawn
{
  struct _mon
  {
    _base_fld *pMonsterFld;
    unsigned __int16 wNum;
  };

  struct _option
  {
    bool bKillAfterStop;
    bool bExpPenalty;
    bool bExpReward;
    bool bItemLoot;
  };

  struct _reward_item
  {
    unsigned __int8 byItemTableCode;
    _base_fld *pItemFld;
    _monster_fld *pDstMonFld;
    int nProb;
  };

  struct _state
  {
    struct _mon
    {
      CMonster *pMon;
      unsigned int dwSerial;
      _base_fld *pMonFld;
    };

    unsigned int dwLastUpdateTime;
    int nRespawnNum;
    _mon MonInfo[640];
  };

  bool bLoad;
  unsigned __int16 wMonSetNum;
  _mon MonSet[16];
  CMapData *pMap;
  float fPos[3];
  unsigned int dwTermMSec;
  _option Option;
  int nUseRewardItemNum;
  _reward_item RewardItem[128];
  char szScriptName[64];
  bool bActive;
  _state State;
};

/* 1659 */
struct __cppobj __declspec(align(8)) _event_set
{
  struct _monster_set
  {
    struct _state
    {
      struct _mon
      {
        CMonster *pMon;
        unsigned int dwSerial;
        _base_fld *pMonFld;
      };

      unsigned int dwStartTime;
      unsigned int dwLastUpdateTime;
      int nRespawnNum;
      bool bOper;
      _mon MonInfo[100];
    };

    CMapData *pMap;
    float fPos[3];
    _base_fld *pMonsterFld;
    unsigned __int16 wNum;
    unsigned int dwRegenTerm;
    unsigned __int8 byRegenProb;
    unsigned int dwDuring;
    bool bUnknownMap;
    bool bIsSet;
    _state m_State;
  };

  char m_strId[64];
  _monster_set m_MonSet[10];
  bool m_bOper;
};

/* 1667 */

/* 1661 */
struct __cppobj __declspec(align(8)) UsRefObject
{
  unsigned int m_uiRefCount;
  virtual ~UsRefObject() = default;
};

/* 1665 */
struct __cppobj UsStateTBL : UsRefObject
{
  struct _TBLData
  {
    unsigned __int8 byKey;
    void (__fastcall *pEvnetFun)(Us_HFSM *, unsigned int, void *);
  };

  struct _HFSM_Node_Info
  {
    int m_ParentData;
    unsigned int m_dwStartState;
    int m_bUsed;
    unsigned int m_dwLoopTime;
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
};

/* 1672 */

/* 1675 */
struct __cppobj __declspec(align(8)) CLuaCommand
{
  unsigned __int8 m_byCommand;
  char m_strBuff[2048];
  virtual ~CLuaCommand() = default;
};

/* 1677 */
struct __cppobj __declspec(align(8)) CLuaCommandEx : CLuaCommand
{
  char m_strScriptName[260];
};

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
struct __cppobj CIniFile
{
  char m_strPath[260];
  std::vector<INI_Section *> m_SectionList;
  virtual ~CIniFile() = default;
};

/* 1473 */
struct __cppobj BossSchedule_Map
{
  int m_nIndex;
  char m_strMap[64];
  CIniFile m_INIFile;
  int m_nCount;
  BossSchedule **m_ScheduleList;
  CBossMonsterScheduleSystem *m_pSystem;
};

/* 1627 */

/* 1629 */
struct __cppobj _react_sub_setup
{
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
};

/* 1449 */
struct __cppobj __declspec(align(8)) INI_Key
{
  INI_Section *m_pParentSection;
  char m_strKey[65];
  char m_strValue[65];
};

/* 1464 */
struct __cppobj __declspec(align(8)) CMapDataTable
{
  _map_fld *m_pRecord;
  unsigned int m_dwRecordNum;
  virtual ~CMapDataTable() = default;
};

/* 1472 */
struct __declspec(align(8)) _region_data
{
  char szRegionData[32];
  CMapData *pMap;
  unsigned __int16 wDummyLineIndex;
};

/* 1462 */
struct __cppobj CMapOperation
{
  int m_nLoopStartPoint;
  unsigned int m_dwSpeedHackStdTime;
  CMapDataTable m_tblMapData;
  int m_nMapNum;
  int m_nStdMapNum;
  std::vector<std::pair<int,int>> m_vecStandardMapCodeTable;
  CMapData *m_Map;
  int m_nRegionNum;
  _region_data m_RegionData[100];
  bool m_bReSpawnMonster;
  CMyTimer m_tmrObjTerm;
  CMyTimer m_tmrSystem;
  CMyTimer m_tmrRecover;
  unsigned int m_dwLastObjLoopTime;
  CMapData *m_SettlementMapData[3][2];
  virtual ~CMapOperation() = default;
};

/* 1474 */
struct __cppobj BossSchedule_TBL
{
  int m_nCount;
  BossSchedule_Map **m_MapScheduleList;
};

/* 1477 */

/* 1476 */
struct __cppobj ScheduleMSG
{
  char m_strSection[64];
  unsigned __int8 m_byKey;
  char m_strValue[64];
  unsigned __int16 m_wIniFileIndex;
};

#pragma pack(pop)

