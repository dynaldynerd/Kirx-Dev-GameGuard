#pragma once

#include "IdaCompat.h"
#include "InvenKey.h"

#pragma pack(push, 1)

#include "CCharacter.h"
#include "DELAY_PROCESS.h"
#include "StorageList.h"
#include "CMgrAvatorItemHistory.h"
#include "CMgrAvatorLvHistory.h"
#include "CRecordData.h"
#include "SKILL_IDX_PER_MASTERY.h"
#include "BILLING_FORCE_CLOSE_DELAY.h"
#include "talik_crystal_exchange_clzo.h"
#include "buy_store_request_clzo.h"
#include "sell_store_request_clzo.h"
#include "alter_item_slot_request_clzo.h"
#include "alter_link_slot_request_clzo.h"
#include "make_tower_request_clzo.h"
#include "unit_pack_fill_request_clzo.h"
#include "tuning_data.h"
#include "combine_ex_item_request_clzo.h"
#include <vector>



struct _RTL_CRITICAL_SECTION_DEBUG;
struct _DB_QRY_SYN_DATA;
struct _PCBANG_PLAY_TIME;
struct _DB_QRY_SYN_DATA;
struct _base_fld;
struct _monster_sp_fld;
struct _skill_fld;
struct _force_fld;
struct _ResourceItem_fld;
struct _ITEM_EFFECT;
struct _consume_item_list;
struct _BulletItem_fld;
struct _UnitBullet_fld;
struct _UnitPart_fld;
struct _unit_bullet_param;
struct _combine_ex_item_accept_request_clzo;
struct _combine_ex_item_result_zocl;
struct _combine_ex_item_accept_result_zocl;
struct _ItemCombine_exp_fld;
struct _guildroom_enter_request_clzo;
struct _guildroom_out_request_clzo;
struct _guildroom_rent_request_clzo;
struct _guild_honor_set_request_clzo;
struct _attack_param;
struct _be_damaged_char;
class CAttack;
class CGameObject;
class CItemBox;
class CItemStore;
class CMerchant;
struct _buy_offer;
struct _sell_offer;
struct _limit_amount_info;
struct _dh_reward_sub_setup;
class CPlayer;
class CPlayerAttack;
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
struct si_interpret;
struct _safe_dummy;
struct _effect_parameter::__param_data;
class CUserDB;
class CRecordData;
struct _PVPPOINT_LIMIT_DB_BASE;
struct _PVP_ORDER_VIEW_DB_BASE;
struct _w_name;
class CPartyPlayer;
class CPartyModeKillMonsterExpNotify;
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
struct _animus_fld;
class CAnimus;

/* 1312 */

/* 1695 */
struct __cppobj __unaligned __declspec(align(2)) _character_db_load
{
  char m_wszCharID[17];
  unsigned int m_dwSerial;
  unsigned __int8 m_byRaceSexCode;
  unsigned int m_dwHP;
  unsigned int m_dwFP;
  unsigned int m_dwSP;
  unsigned int m_dwDP;
  long double m_dExp;
  long double m_dLossExp;
  unsigned int m_dwRank;
  unsigned __int16 m_wRankRate;
  long double m_dPvPPoint;
  long double m_dPvPCashBag;
  unsigned __int8 m_byLevel;
  unsigned int m_dwDalant;
  unsigned int m_dwGold;
  char m_sStartMapCode;
  float m_fStartPos[3];
  unsigned __int8 m_byDftPart_Face;
  unsigned __int8 m_byDftPart[5];
  unsigned __int8 m_byUseBagNum;
  unsigned __int8 m_byMaxLevel;
};

/* 1696 */
struct __cppobj _bag_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[100];

  _bag_db_load();
};

/* 1697 */
struct __cppobj _equip_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[8];

  _equip_db_load();
};

/* 1698 */
struct __cppobj __unaligned __declspec(align(2)) _embellish_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[7];

  _embellish_db_load();
};

/* 1699 */
struct __cppobj _force_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[88];

  _force_db_load();
};

/* 1700 */
struct __cppobj _animus_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[4];

  _animus_db_load();
};

/* 1701 */
struct __cppobj _trunk_db_load : _STORAGE_LIST
{
  unsigned __int8 m_byItemSlotRace[100];
  _STORAGE_LIST::_db_con m_List[100];

  _trunk_db_load();
};

/* 1702 */
struct __cppobj _Exttrunk_db_load : _STORAGE_LIST
{
  unsigned __int8 m_byItemSlotRace[40];
  _STORAGE_LIST::_db_con m_ExtList[40];

  _Exttrunk_db_load();
};

/* 1564 */
struct __cppobj _UNIT_DB_BASE
{
  struct __cppobj __unaligned __declspec(align(2)) _LIST
  {
    unsigned __int8 bySlotIndex;
    unsigned __int8 byFrame;
    unsigned int dwGauge;
    unsigned __int8 byPart[6];
    unsigned int dwBullet[2];
    unsigned int dwSpare[8];
    int nPullingFee;
    unsigned int dwCutTime;
    unsigned __int16 wBooster;

    _LIST();
    void Init(unsigned __int8 byIndex);
    void DelUnit();
  };

  _LIST m_List[4];

  _UNIT_DB_BASE();
  void Init();
};

/* 1562 */
struct __cppobj _QUEST_DB_BASE
{
  struct __cppobj __unaligned __declspec(align(1)) _LIST
  {
    unsigned __int8 byQuestType;
    unsigned __int16 wIndex;
    unsigned __int16 wNum[3];
    unsigned int dwPassSec;

    _LIST();
    void Init();
  };

  struct __cppobj __unaligned __declspec(align(1)) _NPC_QUEST_HISTORY
  {
    char szQuestCode[8];
    int nEventNo;
    unsigned __int8 byLevel;
    unsigned int dwEventEndTime;

    _NPC_QUEST_HISTORY();
    void Init();
  };

  struct __cppobj __unaligned __declspec(align(1)) _START_NPC_QUEST_HISTORY
  {
    char szQuestCode[64];
    unsigned __int8 byLevel;
    _SYSTEMTIME tmStartTime;
    __int64 nEndTime;

    _START_NPC_QUEST_HISTORY();
    void Init();
  };

  _LIST m_List[30];
  _NPC_QUEST_HISTORY m_History[70];
  unsigned int dwListCnt;
  _START_NPC_QUEST_HISTORY *m_StartHistory;

  _QUEST_DB_BASE();
  void Init();
};

/* 1566 */
struct __cppobj _SFCONT_DB_BASE
{
  struct _LIST
  {
    unsigned int dwKey;

    void Init();
    bool IsFilled() const;
    unsigned int GetEffectCode() const;
    unsigned int GetEffectIndex() const;
    unsigned int GetLv() const;
    unsigned int GetLeftTime() const;
    unsigned int GetOrder() const;
    void SetKey(
      unsigned __int8 byOrder,
      unsigned __int8 byEffectCode,
      unsigned __int16 wEffectIndex,
      unsigned __int8 byLv,
      unsigned __int16 wLeftTime);
    void SetLeftTime(unsigned __int16 wLeftTime);
    void SetOrder(unsigned __int8 byOrder);
  };

  _LIST m_List[2][8];

  _SFCONT_DB_BASE();
  void Init();
};

/* 1573 */
struct _COMBINEKEY
{
  unsigned __int8 byRewardIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;

  void SetRelease();
  bool IsFilled();
  void LoadDBKey(_COMBINEKEY pl_nKey);
  __int64 CovDBKey();
};

/* 1575 */
struct __cppobj _ITEMCOMBINE_DB_BASE
{
  struct _LIST
  {
    _COMBINEKEY Key;
    unsigned int dwDur;
    unsigned int dwUpt;

    _LIST();
    void Init();
  };

  bool m_bIsResult;
  unsigned __int8 m_byItemListNum;
  unsigned __int8 m_byDlgType;
  __unaligned __declspec(align(1)) unsigned int m_dwDalant;
  __unaligned __declspec(align(1)) unsigned int m_dwCheckKey;
  unsigned __int8 m_bySelectItemCount;
  _LIST m_List[24];
  unsigned int m_dwResultEffectType;
  unsigned int m_dwResultEffectMsgCode;

  _ITEMCOMBINE_DB_BASE();
  void Init();
  bool IsCombineData();
};

/* 1215 */
#include "CPostData.h"

/* 1703 */
#include "CPostStorage.h"

/* 1704 */
#include "CPostReturnStorage.h"

/* 1712 */
struct __cppobj _personal_amine_inven_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[40];

  _personal_amine_inven_db_load();
};

/* 1716 */
struct __cppobj _quick_link
{
  unsigned __int8 byLinkIndex;
  unsigned __int16 wSerial;

  _quick_link();
  void init();
};

/* 1730 */
#include "CPlayerDB.h"

/* 1276 */
struct __unaligned __declspec(align(2)) _CLID
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};

struct __unaligned __declspec(align(1)) _WA_AVATOR_CODE
{
  _CLID m_id;
  char m_wszName[17];
};

struct __unaligned __declspec(align(1)) _CHRID
{
  unsigned __int8 byID;
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};

/* 1692 */
#include "CSyncCS.h"

/* 1689 */
#include "CIndexList.h"


/* 1735 */
#include "CRealMoveRequestDelayChecker.h"

/* 1736 */
struct __declspec(align(8)) _WEAPON_PARAM
{
  _STORAGE_LIST::_db_con *pFixWp;
  _UNIT_DB_BASE::_LIST *pFixUnit;
  int nGaMaxAF;
  int nGaMinAF;
  unsigned __int8 byGaMinSel;
  unsigned __int8 byGaMaxSel;
  int nMaMaxAF;
  int nMaMinAF;
  unsigned __int8 byMaMinSel;
  unsigned __int8 byMaMaxSel;
  unsigned __int8 byAttTolType;
  unsigned __int8 byWpClass;
  unsigned __int8 byWpType;
  unsigned __int16 wGaAttRange;
  unsigned __int16 wMaAttRange;
  int nActiveType;
  char strActiveCode_key[64];
  int nActiveEffLvl;
  int nActiveProb;
  char strEffBulletType[64];

  static CRecordData *s_pWeaponData;
  static void SetStaticMember(CRecordData *itemTable);
  void Init();
  void FixWeapon(_STORAGE_LIST::_db_con *pWeapon);
  unsigned int GetWeaponTolType(_STORAGE_LIST::_db_con *pItem);
  unsigned int GetAttackDelay(int nLv, int nAddDelay);
  unsigned int GetAttackToolType();
};

/* 1737 */
struct __declspec(align(4)) _DTRADE_ITEM
{
  bool bLoad;
  unsigned __int8 byStorageCode;
  unsigned int dwSerial;
  unsigned __int8 byAmount;
};

/* 1738 */
struct __cppobj _DTRADE_PARAM
{
  bool bDTradeMode;
  bool bDTradeLock;
  bool bDTradeOK;
  unsigned __int16 wDTradeDstIndex;
  unsigned int dwDTradeDstSerial;
  _DTRADE_ITEM DItemNode[15];
  unsigned int dwDTrade_Dalant;
  unsigned int dwDTrade_Gold;
  unsigned __int8 byEmptyInvenNum;
  unsigned __int8 bySellItemNum;
  unsigned int dwKey[4];

  void Init();
};

/* 1554 */
struct __cppobj _STAT_DB_BASE
{
  unsigned int m_dwDamWpCnt[2];
  unsigned int m_dwDefenceCnt;
  unsigned int m_dwShieldCnt;
  unsigned int m_dwSkillCum[48];
  unsigned int m_dwForceCum[24];
  unsigned int m_dwMakeCum[3];
  unsigned int m_dwSpecialCum;

  _STAT_DB_BASE();
  void Init();
  static int GetStatIndex(unsigned __int8 byMasteryClass, unsigned __int8 byIndex);
  static bool IsRangePerMastery(unsigned __int8 byMasteryClass, unsigned __int8 byIndex);
};

/* 1739 */
struct __cppobj _mastery_up_data
{
  bool bUpdate;
  unsigned __int8 byCode;
  unsigned __int8 byIndex;
  unsigned __int8 byMastery;

  void init();
  void set(unsigned __int8 code, unsigned __int8 index, unsigned __int8 mastery);
};

/* 1740 */
struct __declspec(align(2)) _skill_lv_up_data
{
  bool bUpdate;
  unsigned __int16 wIndex;
  unsigned __int8 byLv;

  void init();
  void set(unsigned __int16 index, unsigned __int8 lv);
};

/* 1741 */
struct __cppobj __declspec(align(8)) _MASTERY_PARAM
{
  unsigned __int8 m_byRaceCode;
  __unaligned __declspec(align(1)) _STAT_DB_BASE m_BaseCum;
  unsigned int m_dwSkillMasteryCum[8];
  unsigned int m_dwForceLvCum[4];
  unsigned __int8 m_mtyWp[2];
  unsigned __int8 m_mtySuffer;
  unsigned __int8 m_mtyShield;
  unsigned __int8 m_lvSkill[48];
  unsigned __int8 m_mtySkill[8];
  unsigned __int8 m_mtyForce[24];
  unsigned __int8 m_mtyStaff;
  unsigned __int8 m_mtyMakeItem[3];
  unsigned __int8 m_mtySpecial;
  unsigned int *m_ppdwMasteryCumPtr[7];
  unsigned __int8 *m_ppbyMasteryPtr[7];
  unsigned __int8 *m_ppbyEquipMasteryPrt[6];
  _mastery_up_data m_MastUpData;
  _skill_lv_up_data m_SkillUpData;
  bool m_bUpdateEquipMast;

  static CRecordData *s_pSkillData;
  static CRecordData *s_pForceData;
  static void SetStaticMember(CRecordData *effectTable, CRecordData *forceTable);
  char Init(_STAT_DB_BASE *pStatBase, unsigned __int8 byRaceCode);
  static char IsValidMasteryCode(unsigned __int8 byCode, unsigned __int8 byIndex);
  unsigned __int8 GetMasteryPerMast(unsigned __int8 byCode, unsigned __int8 byMast);
  unsigned int GetCumPerMast(unsigned __int8 byCode, unsigned __int8 byMast);
  bool AlterCumPerMast(unsigned __int8 byClass, unsigned __int8 byIndex, unsigned int dwAlterCum, unsigned int *pdwAfterCum);
  unsigned __int8 GetSkillLv(unsigned __int8 bySkillIndex);
  unsigned __int8 GetEquipMastery(int nEquipMasteryCode);
  void UpdateCumPerMast(unsigned __int8 byClass, unsigned __int8 byIndex, unsigned int dwNewCum);
  float GetAveForceMasteryPerClass(unsigned __int8 byClass);
  float GetAveSkillMasteryPerClass(unsigned __int8 byClass);
};

/* 1745 */
struct _TOWER_PARAM
{
  struct _list
  {
    _STORAGE_LIST::_db_con *m_pTowerItem;
    unsigned __int16 m_wItemSerial;
    CGuardTower *m_pTowerObj;

    void init();
  };

  int m_nCount;
  _list m_List[6];

  void Init();
  char PushList(_STORAGE_LIST::_db_con *pTowerItem, CGuardTower *pTowerObj);
  void NotifyOwnerAttackInform(CCharacter *pDst);
};

/* 1749 */
struct __cppobj __declspec(align(8)) _TRAP_PARAM
{
  struct __cppobj __declspec(align(8)) _param
  {
    CTrap *pItem;
    unsigned int dwSerial;

    void init();
    bool isLoad();
  };

  _param m_Item[20];
  int m_nCount;

  void Init();
  char PushItem(CTrap *pTrap, unsigned int dwTrapSerial);
};

/* 1131 */
#include "CNetCriticalSection.h"

/* 1279 */
#include "CNetIndexList.h"

/* 1751 */
struct __cppobj _BUDDY_LIST
{
  struct __list
  {
    unsigned int dwSerial;
    char wszName[17];
    CPlayer *pPtr;

    __list();
    void init();
    bool fill();
    void ON(char *pwszName, CPlayer *ptr);
  };

  __list m_List[50];
  CNetIndexList m_LastApply;

  _BUDDY_LIST();
  ~_BUDDY_LIST();
  void Init();
  int GetBuddyNum();
  __list *GetEmptyData();
  int PushBuddy(unsigned int dwSerial, char *pwszName, CPlayer *pPtr);
  int PopBuddy(unsigned int dwSerial, CPlayer **ppPoper);
  bool IsBuddy(unsigned int dwSerial);
  bool SearchBuddyLogoff(unsigned int dwSerial);
  bool SearchBuddyLogin(CPlayer *pLoger, unsigned int dwSerial, char *pwszName);
  void PushLastApplyTemp(unsigned int dwDstSerial);
  void PopLastApplyTemp(unsigned int dwDstSerial);
  bool IsPushLastApply(unsigned int dwDstSerial);
};

/* 67 */
enum QUEST_HAPPEN : __int32
{
  quest_happen_type_dummy = 0x0,
  quest_happen_type_npc = 0x1,
  quest_happen_type_pk = 0x2,
  quest_happen_type_lv = 0x3,
  quest_happen_type_class = 0x4,
  quest_happen_type_grade = 0x5,
  quest_happen_type_item = 0x6,
  quest_happen_type_mastery = 0x7,
  quest_happen_type_maxlevel = 0x8,
  QUEST_HAPPEN_TYPE_NUM = 0x9,
};

enum PVP_POINT_ALTER_TYPE : int
{
  kill_s_inc = 0,
  kill_p_inc = 1,
  die_dec = 2,
  quest_inc = 3,
  logoff_inc = 4,
  logoff_dec = 5,
  holy_award = 6,
  holy_dec = 7,
  cheat = 8,
  guildbattle = 9,
};

enum PVP_CASH_IO_CODE : int
{
  pm_kill = 0,
  pm_reward = 1,
  pm_quest = 2,
};

/* 1754 */
struct __cppobj __declspec(align(8)) _happen_event_cont
{
  _happen_event_node *m_pEvent;
  QUEST_HAPPEN m_QtHpType;
  int m_nIndexInType;
  int m_nRaceCode;

  _happen_event_cont();
  void init();
  bool isset() const;
  void set(_happen_event_node *pPoint, QUEST_HAPPEN QtHpType, int nIndexInType, int nRaceCode);
};

/* 1755 */
#include "CQuestMgr.h"

/* 1756 */
struct __cppobj ItemCombineMgr
{
  static bool LoadData();
  static bool CheckLoadData();
  void InitMgr(CPlayer *pOne);
  void OnPlayerCreateCompleteProc();
  unsigned __int8 RequestCombineProcess(_combine_ex_item_request_clzo *pRecv, _combine_ex_item_result_zocl *pSend);
  unsigned __int8 RequestCombineAcceptProcess(
    _combine_ex_item_accept_request_clzo *pRecv,
    _combine_ex_item_accept_result_zocl *pSend);
  unsigned __int8 LoadDB_CombineResult(_combine_ex_item_result_zocl *pLoadData);
  char ClearDB_CombineResult();
  unsigned __int8 MakeNewItems(
    _ITEMCOMBINE_DB_BASE *pPlayerItemDB,
    _combine_ex_item_accept_request_clzo *pRecv,
    _combine_ex_item_accept_result_zocl *pSend);
  char ConsumeMeterial_And_CalculateNewItems(
    _STORAGE_LIST::_db_con **pMt_Sv_Inv,
    unsigned __int8 byMtSlotNum,
    _combine_ex_item_request_clzo::_list *pipMaterials,
    _combine_ex_item_result_zocl *pSaveData,
    _ItemCombine_exp_fld *pfld,
    unsigned __int8 byLinkTableIndex,
    int nType);
  char UpdateDB_CombineResult(_combine_ex_item_result_zocl *pSaveData);
  static CRecordData ms_tbl_ItemCombine;
  static CRecordData ms_tbl_ItemCombine_Link_Stuff;
  static CRecordData ms_tbl_ItemCombine_Link_Result;

  CPlayer *m_pMaster;
};

/* 1757 */
struct $42A3CB4E031CC824EFC3D0F4D1894CCF
{
  unsigned __int32 nNumofTime : 4;
  unsigned __int32 nHour : 5;
  unsigned __int32 nDay : 5;
  unsigned __int32 nMonth : 4;
  unsigned __int32 nYear : 14;
};

/* 1758 */
union $96C5C4A8485BFF889935F41DA1669980
{
  $42A3CB4E031CC824EFC3D0F4D1894CCF __s0;
  unsigned int uiData;
};

/* 1760 */
struct __cppobj MiningTicket
{
  struct _AuthKeyTicket
  {
    $96C5C4A8485BFF889935F41DA1669980 ___u0;
    void Init();
    void Set(unsigned int uiSrc);
    void Set(
      unsigned __int16 byYear,
      unsigned __int8 byMonth,
      unsigned __int8 byDay,
      unsigned __int8 byHour,
      unsigned __int8 byNumofTime);
    bool operator==(const _AuthKeyTicket &src) const;
    bool operator!=(const _AuthKeyTicket &src) const;
  };

  _AuthKeyTicket m_dwTakeLastMentalTicket;
  _AuthKeyTicket m_dwTakeLastCriTicket;
  void Init();
  bool AuthLastMentalTicket(
    unsigned __int16 byCurrentYear,
    unsigned __int8 byCurrentMonth,
    unsigned __int8 byCurrentDay,
    unsigned __int8 byCurrentHour,
    unsigned __int8 byNumOfTime);
  bool AuthLastCriTicket(
    unsigned __int16 byCurrentYear,
    unsigned __int8 byCurrentMonth,
    unsigned __int8 byCurrentDay,
    unsigned __int8 byCurrentHour,
    unsigned __int8 byNumOfTime);
  unsigned int GetLastMentalTicket();
  unsigned int GetLastCriTicket();
  void SetLastMentalTicket(unsigned int uiMentalTicket);
  void SetLastMentalTicket(
    unsigned __int16 byCurrentYear,
    unsigned __int8 byCurrentMonth,
    unsigned __int8 byCurrentDay,
    unsigned __int8 byCurrentHour,
    unsigned __int8 byNumOfTime);
  void SetLastCriTicket(unsigned int uiCriTicket);
  void SetLastCriTicket(
    unsigned __int16 byCurrentYear,
    unsigned __int8 byCurrentMonth,
    unsigned __int8 byCurrentDay,
    unsigned __int8 byCurrentHour,
    unsigned __int8 byNumOfTime);
};

/* 1762 */
#include "CSetItemEffect.h"

/* 1763 */
#include "CEquipItemSFAgent.h"

/* 1765 */
struct __cppobj _CRYMSG_LIST
{
  struct _LIST
  {
    char wszCryMsg[65];

    void Init();
  };

  _LIST m_List[10];

  void Init();
};

/* 1766 */
#include "StoragePos.h"

/* 1768 */
struct __cppobj _RENAME_POTION_USE_INFO
{
  _STORAGE_POS_INDIV ItemInfo;
  char wszChangeName[17];

  void Init();
};

/* 1769 */
struct $621D0DDFB6A4DE55506A65C7CCDC95CE
{
  unsigned __int32 m_CashReName : 3;
  unsigned __int32 m_CashUnused : 5;
};


/* 1772 */
struct __cppobj _NPCQuestIndexTempData
{
  struct _IndexData
  {
    unsigned int dwQuestHappenIndex;
    unsigned int dwQuestIndex;
  };

  int nQuestNum;
  _IndexData IndexData[30];
};

/* 1781 */
struct __declspec(align(8)) _MEM_PAST_WHISPER
{
  bool bMemory;
  char wszName[17];
  unsigned __int8 byNameLen;
  CPlayer *pDst;
  unsigned int dwMemoryTime;
};

/* 1782 */
struct __cppobj _ContPotionData
{
  unsigned int m_dwPotionEffectIndex;
  unsigned int m_dwStartSec;
  unsigned __int16 m_wDurCapSec;
  unsigned int m_dwID;
  static unsigned int ms_dwCount;

  _ContPotionData();

  void Init();
  void Set(unsigned int dwPotionIndex, unsigned int dwStartTime, unsigned __int16 wDurCapSec);
  bool IsLive();
  unsigned int GetEffectIndex();
};

/* 1783 */
#include "CPotionParam.h"

/* 1784 */
#include "CExtPotionBuf.h"

/* 1786 */
struct __cppobj __unaligned __declspec(align(1)) _target_monster_contsf_allinform_zocl
{
  struct _moncontsf
  {
    unsigned __int16 wSfcode;
  };

  unsigned int dwSerial;
  unsigned __int8 byContCount;
  _moncontsf m_MonContSf[8];

  _target_monster_contsf_allinform_zocl();
  void Init();
};

/* 1788 */
struct __cppobj __unaligned __declspec(align(1)) _target_player_damage_contsf_allinform_zocl
{
  struct _playercontsf
  {
    unsigned __int16 wSfcode;
    unsigned __int8 byContCount;
  };

  unsigned int dwSerial;
  unsigned __int8 byContCount;
  _playercontsf m_PlayerContSf[8];

  _target_player_damage_contsf_allinform_zocl();
  void Init();
};



/* 1792 */
struct __cppobj __declspec(align(4)) _ATTACK_DELAY_CHECKER
{
  struct _eff_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int16 wEffectIndex;
    unsigned int dwNextTime;

    _eff_list();
    static void init(_eff_list *self);
    static bool fill(_eff_list *self);
  };

  struct _mas_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int8 byMastery;
    unsigned int dwNextTime;

    _mas_list();
    static void init(_mas_list *self);
    static bool fill(_mas_list *self);
  };

  _eff_list EFF[10];
  _mas_list MAS[10];
  unsigned int dwNextEffTime;
  unsigned int dwNextGenTime;
  unsigned int dwLastGnAttackTime;
  unsigned int dwLastSFAttackTime;
  int nFailCount;
  int m_nNextAddTime;
  unsigned __int8 byTemp_EffectCode;
  unsigned __int16 wTemp_EffectIndex;
  unsigned __int8 byTemp_EffectMastery;

  static int s_nSpareTime;

  _ATTACK_DELAY_CHECKER();
  static void Init(_ATTACK_DELAY_CHECKER *self);
  static char IsDelay(_ATTACK_DELAY_CHECKER *self, unsigned __int8 code, unsigned __int16 index, unsigned __int8 mastery);
  static void SetDelay(_ATTACK_DELAY_CHECKER *self, unsigned int delay);
  static char _delay_check(_ATTACK_DELAY_CHECKER *self, unsigned __int8 code, unsigned __int16 index, unsigned __int8 mastery);
};

/* 1793 */
#include "CPvpPointLimiter.h"

/* 1794 */
#include "CPvpOrderView.h"

/* 1795 */
#include "CPvpCashPoint.h"

/* 1796 */
#include "CouponInfo.h"

/* 1797 */
#include "CCouponMgr.h"

/* 1798 */
struct __cppobj _NameChangeBuddyInfo
{
  bool bNameChange;
  int nSendNum;
  void Init();
};

/* 1800 */
struct __cppobj _other_shape_all_zocl
{
  struct __unaligned __declspec(align(1)) _model
  {
    unsigned __int16 wPartIndex;
    unsigned __int8 byLv;
  };

  unsigned __int16 wIndex;
  __unaligned __declspec(align(1)) unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  unsigned __int8 byCashChangeStateFlag;
  unsigned __int8 byRecIndex;
  unsigned __int8 byFaceIndex;
  unsigned __int8 byHairIndex;
  _model ModelPerPart[8];
  unsigned __int8 byUserGrade;
  __unaligned __declspec(align(1)) unsigned int dwGuildSerial;
  char wszName[17];
  unsigned __int8 byColor;
  unsigned __int8 byHonorGuildRank;
  unsigned __int8 bySpecialPart;
  unsigned __int8 byFrameIndex;
  unsigned __int8 byUnitPartIndex[6];

  _other_shape_all_zocl();
  unsigned __int64 size();
};

/* 1802 */
struct __cppobj __unaligned __declspec(align(2)) _other_shape_part_zocl
{
  struct __unaligned __declspec(align(1)) _model
  {
    unsigned __int16 wPartIndex;
    unsigned __int8 byLv;
  };

  unsigned __int16 wIndex;
  unsigned int dwSerial;
  unsigned __int16 wEquipVer;
  unsigned __int8 byCashChangeStateFlag;
  _model ModelPerPart[8];
  unsigned __int8 byHonorGuildRank;
  unsigned __int8 bySpecialPart;
  unsigned __int8 byFrameIndex;
  unsigned __int8 byUnitPartIndex[6];

  _other_shape_part_zocl();
  unsigned __int64 size();
};

/* 1310 */
class __cppobj __declspec(align(8)) CPlayer : public CCharacter
{
public:
  union CashChangeStateFlag
  {
    $621D0DDFB6A4DE55506A65C7CCDC95CE __s0;
    unsigned __int8 m_byStateFlag;

    CashChangeStateFlag(char cashrename = 0);
  };

private:
  struct __declspec(align(8)) __target
  {
    CGameObject *pObject;
    unsigned __int8 byKind;
    unsigned __int8 byID;
    unsigned int dwSerial;
    unsigned __int16 wHPRate;
    _target_monster_contsf_allinform_zocl m_PrevTargetMonsterContInfo;
    _target_player_damage_contsf_allinform_zocl m_PrevTargetPlayerDamageContInfo;

    __target();
    void init();
  };

public:
  CPlayer();
  virtual ~CPlayer();

  static _skill_fld *ms_pXmas_Snow_Effect;
  static _skill_fld *ms_pXmas_Snow_Bullet_Effect;
  static CMgrAvatorItemHistory s_MgrItemHistory;
  static CMgrAvatorLvHistory s_MgrLvHistory;
  static int s_nLiveNum;
  static _DELAY_PROCESS s_AnimusReturnDelay;
  static int *s_pnLinkForceItemToEffect;
  static _SKILL_IDX_PER_MASTERY s_SkillIndexPerMastery[8];
  static int s_nAddMstFc[100];
  static int s_nStdDefPoint;
  static int s_nRevDefPoint;
  static CRecordData s_tblLimMastery[3][4];
  static CRecordData s_tblLimMasteryContinue[3][4];
  static CRecordData s_tblLimMasteryCum[3][4];
  static CRecordData s_tblLimMasteryCumContinue[3][4];
  static _BILLING_FORCE_CLOSE_DELAY s_BillingForceCloseDelay;
  static int s_nRaceNum[3];
  static float s_fExpDivUnderParty_Kill[8];
  static unsigned int s_dwTotalCloseCount;
  static unsigned int s_dwAbnormalCloseCount;
  static void SetStaticMember();
  static void Guild_Insert_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Join_Accept_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Self_Leave_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Force_Leave_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Push_Money_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Pop_Money_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Buy_Emblem_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Disjoint_Complete(_DB_QRY_SYN_DATA *pData);
  static void Guild_Update_GuildMater_Complete(_DB_QRY_SYN_DATA *pData);
  void Init(_object_id *pID);
  void PastWhisperInit();
  _STORAGE_LIST::_db_con *Emb_AddStorage(
    unsigned __int8 byStorageCode,
    _STORAGE_LIST::_storage_con *pCon,
    bool bMessage,
    bool bLog);
  bool Emb_DelStorage(
    unsigned __int8 byStorageCode,
    unsigned __int8 byStorageIndex,
    bool bEquipChange,
    bool bDelete,
    const char *strErrorCodePos);
  unsigned __int64 Emb_AlterDurPoint(
    unsigned __int8 byStorageCode,
    unsigned __int8 byStorageIndex,
    int nAlter,
    bool bUpdate,
    bool bSend);
  void Emb_ItemUpgrade(
    unsigned __int8 byUpgradeType,
    unsigned __int8 byStorageCode,
    unsigned __int8 byStorageIndex,
    unsigned int dwGradeInfo);
  char Create();
  char IntoMap(unsigned __int8 byMapInMode);
  void SendData_PartyMemberInfo(unsigned __int16 wDstIndex);
  void SendData_PartyMemberInfoToMembers();
  void SendData_PartyMemberPos();
  void CheckPos_Region();
  void CheckUnitCutTime();
  void SendMsg_EconomyHistoryInform();
  void SendMsg_EconomyRateInform(char bStart);
  void SendMsg_SFDelayRequest();
  void SendMsg_PotionDelayTime(unsigned int *pdwPotionNextUseTime, unsigned int dwCurTime);
  void _set_db_sf_effect(_SFCONT_DB_BASE *pDBBase);
  bool OutOfMap(CMapData *pIntoMap, unsigned __int16 wLayerIndex, unsigned __int8 byMapOutType, float *pfStartPos);
  void SendMsg_GotoRecallResult(
    char byErrCode,
    char byMapCode,
    float *pfStartPos,
    unsigned __int8 byMapInType);
  void SendMsg_GotoBasePortalResult(char byErrCode);
  void SendMsg_MovePortal(
    char byRet,
    char byMapIndex,
    unsigned __int8 byPotalIndex,
    float *pfStartPos,
    bool bEqualZone);
  void SendMsg_MovePortal(char byMapIndex, float *pfStartPos, unsigned __int8 byZoneCode);
  void SendMsg_GuildRoomOutResult(
    unsigned __int8 byRetCode,
    unsigned __int8 byMapIndex,
    unsigned __int16 wMapLayer,
    float *pPos);
  void SendMsg_RegistBindResult(char byRetCode);
    void SendMsg_MoveError(char byRetCode);
    void SendMsg_Stop(bool bAll);
    void SendMsg_MoveNext(bool bOtherSend);
    void SendMsg_MapEnvInform(char byMapCode, unsigned int dwMapEnvCode);
    void SendMsg_MapOut(unsigned __int8 byMapOutCode, unsigned __int8 byNextMapCode);
    void SendMsg_AlterTownOrField();
    void SendMsg_AlterRegionInform(__int16 nRegionIndex);
  void SendMsg_MineCancle();
  void SendMsg_CuttingCompleteResult(unsigned __int8 byRet);
  void SendMsg_SetFPInform();
  void SendMsg_SetSPInform();
  void SendMsg_GestureInform(unsigned __int8 byGestureType);
  void SendMsg_JadeEffectErr(char byErrorCode);
  void SendMsg_HonorGuildMark(char byRank);
  void SendMsg_AdjustAmountInform(char byStorageCode, unsigned __int16 wSerial, unsigned int dwDur);
  void SendMsg_AlterPvPCash(char nIOCode);
  void SendMsg_AlterMoneyInform(char byReasonCode);
  void SendMsg_DeleteStorageInform(char byStorageCode, unsigned __int16 wSerial);
  void SendMsg_BuddhaEventMsg(char byErrorCode);
  void SendMsg_AlterItemDurInform(char byStorageCode, unsigned __int16 wItemSerial, unsigned __int64 dwDur);
  void SendMsg_EquipPartChange(unsigned __int8 byPart);
  void SendMsg_TakeNewResult(char byErrCode, _STORAGE_LIST::_db_con *pItem);
  void SendMsg_TakeAddResult(char byErrCode, _STORAGE_LIST::_db_con *pItem);
  void SendMsg_ThrowStorageResult(char byErrCode);
  void SendMsg_AddBagResult(char byErrCode);
  void SendMsg_EquipPartResult(char byErrCode);
  void SendMsg_EmbellishResult(char byErrCode);
  void SendMsg_OffPartResult(char byErrCode);
  void SendMsg_MakeItemResult(char byErrCode);
  void SendMsg_ItemUpgrade(char byErrCode);
  void SendMsg_ItemDowngrade(char byErrCode);
  void SendMsg_CombineItemResult(char byErrCode, unsigned int dwFee, _STORAGE_LIST::_db_con *pNewItem);
  void SendMsg_CombineLendItemResult(unsigned __int8 byErrCode, unsigned int dwFee, _STORAGE_LIST::_db_con *pNewItem);
  void SendMsg_ExchangeItemResult(char byErrCode, _STORAGE_LIST::_db_con *pNewItem);
  void SendMsg_CombineItemExResult(_combine_ex_item_result_zocl *pSend);
  void SendMsg_CombineItemExAcceptResult(_combine_ex_item_accept_result_zocl *pSend);
  void SendMsg_UsePotionResult(char byErrCode, unsigned __int16 wSerial, unsigned __int8 byLeftNum);
  void SendMsg_UseRadarResult(char byErrCode, unsigned __int16 wSerial, unsigned int dwDelay);
  void SendMsg_RadarCharSearchResult();
  void SendMsg_FanfareItem(unsigned __int8 byGetType, _STORAGE_LIST::_db_con *pItem, CItemBox *pItemBox);
  void SendMsg_PartyLootItemInform(
    unsigned int dwTakerSerial,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex,
    unsigned __int8 byNum);
  void SendMsg_PartyJoinInvitationQuestion(unsigned __int16 wJoinerIndex);
  void SendMsg_PartyJoinApplicationQuestion(CPlayer *pJoiner);
  void SendMsg_PartyJoinJoinerResult();
  void SendMsg_PartyJoinMemberResult(CPartyPlayer *pJoiner, char byLootShareMode);
  void SendMsg_PartyJoinFailLevel();
  void SendMsg_AwayPartyRequestResult(char byRetCode);
  void SendMsg_AwayPartyInvitationQuestion(unsigned __int16 wJoinerIndex);
  void SendMsg_PartyLeaveCompulsionResult(CPartyPlayer *pLeaver);
  void SendMsg_PartyDisjointResult(char bSuccess);
  void SendMsg_PartyLockResult(char byRet);
  void SendMsg_PartyAlterLootShareResult(char byLootShareMode);
  void SendMsg_Notify_Get_Golden_Box(
    char byBoxType,
    unsigned int dwCharSerial,
    const char *szCharName,
    _STORAGE_LIST::_db_con *pItem,
    bool bCircle);
  void SendMsg_Notify_Me_Get_Golden_Box(char byBoxType, _STORAGE_LIST::_db_con *pItem);
  void SendMsg_ExchangeLendItemResult(unsigned __int8 byErrCode, _STORAGE_LIST::_db_con *pNewItem);
  void SendMsg_TalikCrystalExchangeResult(
    unsigned __int8 byRet,
    unsigned __int8 byExchangeNum,
    _STORAGE_LIST::_db_con *pNewItem);
  void SendMsg_CharacterRenameCashResult(char bChange, char byErrCode);
  void SendMsg_SetItemCheckResult(char byResult, unsigned int dwSetItem, unsigned __int8 bySetEffectNum);
  void SendMsg_QuestGiveUpResult(char byReturnSlot);
  void SendMsg_SelectWaitedQuest(char byEventType, unsigned int dwEventIndex, unsigned __int8 byEventNodeIndex);
  void SendMsg_GuildManageResult(char byRetCode);
  void SendMsg_NPCLinkItemCheckResult(unsigned __int8 byResCode, _STORAGE_POS_INDIV *pStorage);
  void SendMsg_PostDelivery(
    char byIndex,
    unsigned int dwPostSerial,
    char *wszSendName,
    char *wszTitle,
    bool bItem,
    bool bGold,
    unsigned __int8 byState);
  void SendMsg_PostReturn(
    char byErrCode,
    unsigned int dwPostSerial,
    char *wszRecvName,
    char *wszTitle,
    char *wszContent,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex,
    unsigned __int64 dwDur,
    unsigned int dwLv,
    unsigned int dwGold);
  void SendMsg_PostContent(
    unsigned __int8 byErrCode,
    unsigned int dwPostSerial,
    char *wszContent,
    unsigned __int8 byTableCode,
    unsigned __int16 wItemIndex,
    unsigned __int64 dwDur,
    unsigned int dwLv,
    unsigned int dwGold);
  void SendMsg_PostSendReply(unsigned __int8 byErrCode);
  void SendMsg_PostItemGold(char byErrCode);
  void SendMsg_PostDelete(unsigned __int8 byErrCode, unsigned int dwPostSerial);
  void SendMsg_PostReturnConfirm(char byErrCode, unsigned int dwPostSerial);
  void SendMsg_DarkHoleOpenFail(unsigned int n, unsigned __int8 byErrCode);
  void SendMsg_DarkHoleOpenResult(
    unsigned int n,
    int bPartyOnly,
    unsigned __int8 byErrCode,
    unsigned __int16 wHoleIndex,
    unsigned int dwHoleSerial);
  void SendMsg_EnterDarkHole(unsigned __int8 byErrCode, unsigned int dwHoleSerial);
  void SendMsg_GiveupDarkHole(unsigned __int8 byErrCode);
  void SendMsg_ClearDarkHole(unsigned __int8 byErrCode);
  void SendMsg_ReEnterDarkHoleResult(unsigned __int8 byRetCode);
  void Reward_DarkDungeon(
    _dh_reward_sub_setup *pSetup,
    char *pszTitle,
    bool bRealBoss,
    _STORAGE_LIST::_db_con *pItem,
    int *bIsRewarded);
  int GetRewardItems_DarkDungeon(
    _dh_reward_sub_setup *pSetup,
    _STORAGE_LIST::_db_con *pItems,
    bool bRealBoss);
  void SendMsg_DarkHoleRewardMessage(_STORAGE_LIST::_db_con *pItem, unsigned int dwMemberIndex, int isRewarded);
  bool UpdateDelPost(unsigned int dwPostSerial, int nIndex);
  void DelPostData(unsigned int dwIndex);
  void SortPost(int nNumber);
  void UpdatePostAddLog(int dwIndex, bool bLog, int nItemKey);
  bool IsReturnPostUpdate();
  void UpdateReturnPost(unsigned int dwSerial);
  void UpdatePost(unsigned int dwIndex);
  void pc_UpdateDataForPostSend();
  void SendData_ChatTrans(
    unsigned __int8 byChatType,
    unsigned int dwSenderSerial,
    unsigned __int8 byRaceCode,
    bool bFilter,
    char *pwszMessage,
    unsigned __int8 byPvpGrade,
    char *pwszSender);
  void AddGold(int dwPush, bool bApply);
  void SubGold(unsigned int dwSub);
  void SubPoint(int dwSub);
  void SubActPoint(unsigned __int8 byCode, unsigned int dwSub);
  void AlterGold(double dGold);
  unsigned int GetMoney(unsigned __int8 byMoneyCode);
  void SendMsg_ChatFarFailure(char bBlock);
  CPlayer *FindFarChatPlayerWithTemp(char *pwszName);
  void pc_ChatOperatorRequest(unsigned __int8 byRaceCode, char *pwszChatData);
  void pc_ChatCircleRequest(char *pwszChatData);
  void pc_ChatFarRequest(char *pwszName, char *pwszChatData);
  void pc_ChatPartyRequest(char *pwszChatData);
  void pc_ChatRaceRequest(char *pwszChatData);
  void pc_ChatMgrWhisperRequest(char *pwszChatData);
  void pc_ChatMapRequest(char *pwszChatData);
  void pc_ChatRaceBossRequest(char *pwszChatData);
  void pc_ChatGuildEstSenRequest(char *pwszChatData);
  void pc_ChatRePresentationRequest(char *pwszChatData);
  void pc_ChatAllRequest(char *pwszChatData);
  void pc_ChatGmNoticeRequest(char *pwszChatData);
  void pc_ChatTradeRequestMsg(unsigned __int8 bySubType, char *pwszTradeMsg);
  void pc_ChatGuildRequest(unsigned int dwDstSerial, char *pwszChatData);
  void pc_ChatMultiFarRequest(unsigned __int8 byDstNum, _w_name *pDstName, char *pwszMsg);
  void pc_ChatRaceBossCryRequest(char *pwszChatData);
  void pc_NotifyRaceBossCryMsg();
  void pc_SetRaceBossCryMsg(unsigned __int8 bySlot, char *pwszCryMsg);
  void pc_ForceInvenChange(_STORAGE_POS_INDIV *pItem, unsigned __int16 wReplaceSerial);
  void pc_AnimusInvenChange(_STORAGE_POS_INDIV *pItem, unsigned __int16 wReplaceSerial);
  void pc_ResSeparation(unsigned __int16 wStartSerial, unsigned __int8 byMoveAmount);
  void pc_ResDivision(unsigned __int16 wStartSerial, unsigned __int16 wTarSerial, unsigned __int8 byMoveAmount);
  void pc_PotionSeparation(unsigned __int16 wSerial, unsigned __int8 byAmount);
  void pc_PotionDivision(unsigned __int16 wSerial, unsigned __int16 wTarSerial, unsigned __int8 byAmount);
  void pc_AlterItemSlotRequest(unsigned __int8 byNum, _alter_item_slot_request_clzo::__list *pList);
  void pc_AlterLinkBoardSlotRequest(
    unsigned __int8 byNum,
    _alter_link_slot_request_clzo::__list *pList,
    unsigned __int8 byLBLock);
  void SendMsg_ResSeparation(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pNewOre);
  void SendMsg_PotionSeparation(
    unsigned __int16 wParentSerial,
    unsigned __int8 byParentAmount,
    unsigned __int16 wChildSerial,
    unsigned __int8 byChildAmount,
    char nRet);
  void SendMsg_ResDivision(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pTargetOre);
  void SendMsg_PotionDivision(
    unsigned __int16 wSerial,
    unsigned __int8 byParentAmount,
    unsigned __int16 wChildSerial,
    unsigned __int8 byChildAmount,
    char nRet);
  void SendMsg_ForceInvenChange(char byErrCode);
  void SendMsg_AnimusInvenChange(char byErrCode);
  void pc_SkillRequest(unsigned __int8 bySkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial);
  void pc_ClassSkillRequest(unsigned __int16 wSkillIndex, _CHRID *pidDst, unsigned __int16 *pConsumeSerial);
  void pc_ForceRequest(unsigned __int16 wForceSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial);
  void pc_ThrowSkillRequest(unsigned __int16 wBulletSerial, _CHRID *pidDst, unsigned __int16 *pConsumeSerial);
  void pc_ThrowUnitRequest(_CHRID *pidDst, unsigned __int16 *pConsumeSerial);
  void pc_MakeTowerRequest(
    unsigned __int16 wSkillIndex,
    unsigned __int16 wTowerItemSerial,
    unsigned __int8 byMaterialNum,
    _make_tower_request_clzo::__material *pMaterial,
    float *pfPos,
    unsigned __int16 *pConsumeSerial);
  void pc_BackTowerRequest(unsigned int dwTowerObjSerial);
  void pc_MakeTrapRequest(
    unsigned __int16 wSkillIndex,
    unsigned __int16 wTrapItemSerial,
    float *pfPos,
    unsigned __int16 *pConsumeSerial);
  void pc_BackTrapRequest(unsigned int dwTrapObjSerial, unsigned __int16 wAddSerial);
  void pc_ChangeModeType(unsigned __int8 nModeType, unsigned __int8 nStandType);
  void pc_GestureRequest(unsigned __int8 byGestureType);
  void pc_AlterWindowInfoRequest(
    unsigned int *pdwSkill,
    unsigned int *pdwForce,
    unsigned int *pdwChar,
    unsigned int *pdwAnimus,
    unsigned int dwInven,
    unsigned int *pdwInvenBag);
  void pc_SetTargetObjectRequest(CGameObject *pTar, unsigned int dwSerial, bool bForce);
  void pc_ReleaseTargetObjectRequest();
  void pc_SetGroupTargetObjectRequest(CGameObject *pTar, unsigned int dwSerial, unsigned __int8 byGroupType);
  void pc_ReleaseGroupTargetObjectRequest(unsigned __int8 byGroupType);
  void pc_SetGroupMapPointRequest(unsigned __int8 byGroupType, float *pzTar);
  void pc_PartyReqBlock(bool bBlock);
  void pc_WhisperBlock(bool bBlock);
  void pc_TradeBlock(bool bBlock);
  void pc_GuildBattleBlock(bool bBlock);
  void pc_MacroUpdate(char *pBuf);
  void pc_RequestTaxRate();
  void pc_RequestChangeTaxRate(unsigned __int8 byTaxRate);
  void pc_ReleaseSiegeModeRequest();
  void pc_TransformSiegeModeRequest(unsigned __int16 wItemSerial);
  void pc_TransShipRenewTicketRequest(unsigned __int16 wTicketItemSerial);
  void pc_TrunkEstRequest(char *pwszPassword, unsigned __int8 byHintIndex, char *pwszHintAnswer);
  void pc_TrunkDownloadRequest(char *pwszPassword);
  void pc_TrunkChangePasswdRequest(
    char *pwszPrevPassword,
    char *pwszChngPassword,
    unsigned __int8 byHintIndex,
    char *pwszHintAnswer);
  void pc_TrunkExtendRequest();
  void pc_TrunkAlterItemSlotRequest(unsigned int dwItemSerial, unsigned __int8 byClientSlotIndex, unsigned __int8 byStorageIndex);
  void pc_TrunkResDivision(
    unsigned __int16 wStartSerial,
    unsigned __int16 wTarSerial,
    unsigned __int16 wMoveAmount,
    unsigned __int8 byStorageIndex);
  void pc_TrunkPotionDivision(
    unsigned __int16 wStartSerial,
    unsigned __int16 wTarSerial,
    unsigned __int16 wMoveAmount,
    unsigned __int8 byStorageIndex);
  void pc_TrunkIoMoveRequest(
    unsigned __int8 byStartStorageIndex,
    unsigned __int8 byTarStorageIndex,
    unsigned __int16 wItemSerial,
    unsigned __int8 byClientSlotIndex);
  void pc_TrunkIoSwapRequest(
    unsigned __int8 byStartStorageIndex,
    unsigned __int8 byTarStorageIndex,
    unsigned __int16 wStartItemSerial,
    unsigned __int16 wTarItemSerial);
  void pc_TrunkIoMergeRequest(
    unsigned __int8 byStartStorageIndex,
    unsigned __int8 byTarStorageIndex,
    unsigned __int16 wStartItemSerial,
    unsigned __int16 wTarItemSerial,
    unsigned __int16 wMoveAmount);
  void pc_TrunkIoMoneyRequest(unsigned __int8 byCase, int dwDalant, int dwGold);
  void pc_TrunkPwHintIndexRequest();
  void pc_TrunkHintAnswerRequest(char *pwszAnswer);
  unsigned __int8 pc_TrunkCreateCostIsFreeRequest();
  void pc_RequestUILockInit(
    CUserDB *pUserDB,
    char *szUILockPW,
    char *szUILockPW_Confirm,
    unsigned __int8 byUILock_HintIndex,
    char *uszUILock_HintAnswer);
  void pc_RequestUILockCertify(CUserDB *pUserDB, char *uszUILockPW);
  void pc_RequestUILockUpdate(
    char *uszUILockPWOld,
    char *uszUILockPW,
    char *uszUILockPW_Confirm,
    unsigned __int8 byUILock_HintIndex,
    char *uszUILock_HintAnswer);
  void pc_RequestUILockFindPW(CUserDB *pUserDB, char *uszHintAnswer);
  void pc_MineStart(unsigned __int8 byMineIndex, unsigned __int8 byOreIndex, unsigned __int16 wBatterySerial);
  void pc_MineCancle();
  void pc_OreCutting(unsigned __int16 wOreSerial, unsigned __int8 byProcessNum);
  void pc_OreIntoBag(unsigned __int16 wResIndex, unsigned __int16 wSerial, unsigned __int8 byAddAmount);
  void pc_CuttingComplete(unsigned __int8 byNpcRace);
  void pc_PartyJoinInvitation(unsigned __int16 wDstIndex);
  void pc_PartyJoinInvitationAnswer(_CLID *pidBoss);
  void pc_PartyJoinApplication(unsigned __int16 wBossIndex);
  void pc_PartyJoinApplicationAnswer(_CLID *pidApplicant);
  void pc_PartyLeaveCompulsionReqeuest(unsigned int dwExiterSerial);
  void pc_PartyDisJointReqeuest();
  void pc_PartySuccessionReqeuest(unsigned int dwSuccessorSerial);
  void pc_PartyLockReqeuest(bool bLock);
  void pc_PartyAlterLootShareReqeuest(unsigned __int8 byLootShareMode);
  void pc_AwaypartyInvitationRequest(char *pwszCharName);
  void pc_AwayPartyJoinInvitationAnswer(_CLID *pidBoss, unsigned __int8 byRetCode);
  void pc_MoveToOwnStoneMapRequest();
  void pc_DarkHoleOpenRequest(unsigned __int16 dwItemSerial);
  void pc_DarkHoleEnterRequest(unsigned __int16 wHoleIndex, unsigned int dwHoleSerial);
  void pc_DarkHoleGiveupOutRequest();
  void pc_DarkHoleClearOutRequest();
  void pc_DarkHoleAnswerReenterRequest(
    bool bEnter,
    unsigned __int16 wChannelIndex,
    unsigned int dwChannelSerial);
  void SendMsg_DTradeAskResult(char byErrCode);
  void pc_DTradeAskRequest(unsigned __int16 wDstIndex);
  void pc_DTradeAnswerRequest(_CLID *pidAsker);
  void pc_DTradeCancleRequest();
  void pc_DTradeLockRequest();
  void pc_DTradeAddRequest(unsigned __int8 bySlotIndex, unsigned __int8 byStorageCode, unsigned int dwSerial, unsigned __int8 byAmount);
  void pc_DTradeDelRequest(unsigned __int8 bySlotIndex);
  void pc_DTradeBetRequest(unsigned __int8 byMoneyUnit, unsigned int dwBetAmount);
  void pc_UpdateDataForTrade(CPlayer *pTrader);
  void pc_DTradeOKRequest(unsigned int *pdwKey);
  void pc_AnimusRecallRequest(
    unsigned __int16 wAnimusItemSerial,
    unsigned __int16 wAnimusClientHP,
    unsigned __int16 wAnimusClientFP);
  void pc_AnimusReturnRequest();
  void pc_AnimusCommandRequest(unsigned __int8 byCommandCode);
  void pc_AnimusTargetRequest(unsigned __int8 byObjectID, unsigned __int16 wObjectIndex, unsigned int dwObjectSerial);
  void pc_UnitFrameBuyRequest(unsigned __int8 byFrameCode, int bUseNPCLinkIntem);
  void pc_UnitSellRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem);
  void pc_UnitPartTuningRequest(unsigned __int8 bySlotIndex, unsigned __int8 byTuningNum, _tuning_data *pTuningData, int bUseNPCLinkIntem);
  void pc_UnitFrameRepairRequest(unsigned __int8 bySlotIndex, int bUseNPCLinkIntem);
  void pc_UnitBulletFillRequest(unsigned __int8 bySlotIndex, unsigned __int16 *pwBulletIndex, int bUseNPCLinkIntem);
  void pc_UnitPackFillRequest(
    unsigned __int8 bySlotIndex,
    unsigned __int8 byFillNum,
    _unit_pack_fill_request_clzo::__list *pList,
    int bUseNPCLinkIntem);
  void pc_UnitDeliveryRequest(unsigned __int8 bySlotIndex, CItemStore *pStore, bool bPayFee, float *pfNewPos, int bUseNPCLinkIntem);
  void pc_UnitReturnRequest();
  void pc_UnitTakeRequest();
  void pc_UnitLeaveRequest(float *pfNewPos);
  void pc_UnitBulletReplaceRequest(unsigned __int8 bySlotIndex, unsigned __int8 byPackIndex, unsigned __int8 byBulletPart);
  void pc_SelectQuestAfterHappenEvent(unsigned __int8 bySelectIndex);
  void pc_SelectQuestReward(
    unsigned __int8 byQuestDBSlot,
    unsigned __int8 bySelectItemSlotIndex,
    unsigned __int8 bySelectLinkQuestIndex);
  void pc_RequestDialogWithNPC(CItemStore *pStore);
  void pc_RequestWatchingWithNPC(CItemStore *pStore);
  void pc_RequestQuestListFromNPC(CItemStore *pStore);
  void pc_RequestQuestFromNPC(CItemStore *pStore, unsigned int dwNPCQuestIndex);
  void pc_BriefPass(unsigned __int8 byQuestSlotIndex);
  void pc_CastVoteRequest(int nVoteSerial, unsigned __int8 byCode);
  void pc_MoveNext(unsigned __int8 byMoveType, float *pfCur, float *pfTar, unsigned __int8 byDirect);
  void pc_RealMovPos(float *pfCur);
  void pc_MoveStop(float *pfCur);
  void pc_MoveModeChangeRequest(unsigned __int8 byMoveType);
  void pc_GotoBasePortalRequest(unsigned __int16 wItemSerial);
  void pc_GotoAvatorRequest(const char *pwszAvatorName);
  void RecallRandomPositionInRange(
    CMapData *pIntoMap,
    unsigned __int16 wMapLayerIndex,
    float *pStartPos,
    int iRange);
  void pc_MovePortal(int nPortalIndex, unsigned __int16 *pConsumeSerial);
  void pc_RegistBind(CItemStore *pStore);
  bool IsOutExtraStopPos(float *pfStopPos);
  char Emb_CheckActForQuest(int nActCode, char *pszReqCode, unsigned __int16 wAddCount, bool bParty);
  void Emb_CheckActForQuestParty(int nActCode, char *pszReqCode, unsigned __int16 wAddCount);
  char Emb_StartQuest(unsigned __int8 bySelectQuest, _happen_event_cont *pHappenEvent);
  bool Emb_CreateNPCQuest(char *pszEventCode, unsigned int dwNPCQuestIndex);
  char Emb_CreateQuestEvent(__int32 HappenType, char *pszEventCode);
  void Emb_CompleteQuest(
    unsigned __int8 byQuestDBSlot,
    unsigned __int8 byRewardItemIndex,
    unsigned __int8 byLinkQuestIndex);
  _Quest_fld *_Reward_Quest(_Quest_fld *pQuestFld, unsigned __int8 byRewardItemIndex);
  void SendMsg_QuestProcess(char byQuestDBSlot, char byActIndex, unsigned __int16 wCount);
  void SendMsg_SelectQuestReward(char byQuestDBSlot);
  void SendMsg_QuestFailure(char byFailCode, char byQuestDBSlot);
  void SendMsg_InsertQuestFailure(char byEventType, unsigned int dwEventIndex, unsigned __int8 byEventNodeIndex);
  void SendMsg_InsertNextQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB);
  void SendMsg_InsertNewQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB);
  void SendMsg_QuestComplete(char byQuestDBSlot);
  void SendMsg_NpcQuestHistoryInform(char bySlotIndex);
  void SendMsg_HSKQuestActCum();
  void pc_QuestGiveupRequest(unsigned __int8 byQuestDBSlot);
  void SendMsg_MakeItemCheatSendButtonEnable(char bEnableSendButton);
  void AlterDalant(double dDalant);
  void SendMsg_AlterExpInform();
  void SendMsg_NotifyGetExpInfo(long double dOldExp, long double dAlterExp, long double dCurExp);
  void AlterExp(double dAlterExp, bool bReward, bool bUseExpRecoverItem, bool bUseExpAdditionItem);
  void AlterMaxLevel(unsigned __int8 byMaxLevel);
  void SetLevel(unsigned __int8 byNewLevel);
  void SetLevelD(unsigned __int8 byDownLevel);
  void AlterPvPCashBag(long double dAlter, int IOCode);
  void SendMsg_Level(char nLevel);
  void SendData_PartyMemberLv();
  void LimLvNpcQuestDelete(unsigned __int8 byQuestDBSlot);
  void IncCriEffPvPCashBag(double dAlter);
  void SendMsg_EquipItemLevelLimit(int nCurPlayerLv);
  void SendMsg_GuildEstablishFail(char byRetCode);
  void SendMsg_GuildJoinOtherInform();
  void SendMsg_AlterUnitHPInform(char bySlotIndex, unsigned int dwGauge);
  void SendMsg_FcitemInform(unsigned __int16 wItemSerial, unsigned int dwNewStat);
  void SendMsg_ExchangeMoneyResult(char byErrCode);
  void SendMsg_InformTaxIncome(unsigned __int8 byRet, unsigned int dwComm, char *pwszDate);
  void SendMsg_RaceBossCryMsg();
  void SendMsg_ResultChangeTaxRate(unsigned __int8 byRetCode, unsigned __int8 byNextTax);
  void SendMsg_InsertItemInform(char byStorageCode, _STORAGE_LIST::_db_con *pItem);
  unsigned __int16 GetVisualVer();
  void UpdateVisualVer(CashChangeStateFlag byChangeFlagMask);
  unsigned __int8 GetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex);
  void SetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex, unsigned __int8 byCode);
  void SetEquipEffect(_STORAGE_LIST::_db_con *pItem, bool bEquip);
  char ApplyEquipItemEffect(int iItemEffectCode, bool bEquip);
  char IsEffectableEquip(_STORAGE_LIST::_storage_con *pCon);
  unsigned int _check_equipmastery_lim(int EquipMasteryCode);
  bool _check_equip_part(_STORAGE_LIST::_db_con *pFixingItem);
  bool _check_embel_part(_STORAGE_LIST::_db_con *pFixingItem);
  __int64 _check_mastery_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex);
  unsigned int _check_mastery_cum_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex);
  void Emb_AlterStat(
    unsigned __int8 byMasteryClass,
    unsigned __int8 byIndex,
    int dwAlter,
    unsigned __int8 byReason,
    const char *strErrorCodePos,
    bool bNoUseExpMasteryBonus);
  void apply_normal_item_std_effect(int nEffCode, float fVal, bool bEquip);
  void apply_case_equip_std_effect(_STORAGE_LIST::_db_con *pItem, bool bEquip);
  void apply_case_equip_upgrade_effect(_STORAGE_LIST::_db_con *pItem, bool bEquip);
  void apply_have_item_std_effect(int nEffCode, float fVal, bool bAdd, int nDiffCnt);
  _ITEM_EFFECT *_GetItemEffect(_STORAGE_LIST::_db_con *pItem);
  void SetEquipJadeEffect(int nParam, float fCurVal, bool bAdd);
  bool DecHalfSFContDam(float fEffVal);
  void HideNameEffect(bool bAdd);
  void SetMstPt(int nMstCode, float fVal, bool bAdd, unsigned int nWpType);
  void CalcDefTol();
  bool IsRidingUnit();
  void CalcEquipSpeed();
  float GetAddSpeed();
  void BreakCloakBooster();
  void SendMsg_AlterSPInform();
  void SendMsg_AlterEquipSPInform();
  void CalcEquipMaxDP(int bSendMsg);
  bool IsSiegeMode();
  bool IsActingSiegeMode();
  bool IsChaosMode();
  void SetSiege(_STORAGE_LIST::_db_con *pItem);
  void SetHaveEffect(char bSet);
  void SetMstHaveEffect(_ResourceItem_fld *pFld, _STORAGE_LIST::_db_con *pItem, int a3, int a4);
  void SetHaveEffectUseTime(_STORAGE_LIST::_db_con *pItem, bool bAdd);
  bool IsUsableAccountType(int nCashType);
  bool IsApplyPcbangPrimium();
  void SetPotionActDelay(unsigned __int8 byPotionClass, unsigned int dwCurrTime, unsigned int dwActDelay);
  void SendMsg_PremiumCashItemUse(unsigned __int16 wSerial);
  int GetCashAmount();
  void SetCashAmount(int nAmount);
  void DeleteCouponItem(_STORAGE_POS_INDIV *CouponItem, int n);
  void DTradeInit();
  void SendMsg_DTradeCancleResult(char byErrCode);
  void SendMsg_DTradeCancleInform();
  void SendMsg_DTradeCloseInform(char byCloseCode);
  void NetClose(bool bMoveOutLobby);
  void _TowerAllReturn(unsigned __int8 byDestroyType, bool bForceReturn);
  __int16 _TowerReturn(_STORAGE_LIST::_db_con *pTowerItem);
  void _TowerDestroy(CGuardTower *pTowerObj);
  void ExitUpdateDataToWorld();
  void _AnimusReturn(unsigned __int8 byReturnType);
  void SendMsg_AnimusReturnResult(char byRetCode, unsigned __int16 wAnimusItemSerial, unsigned __int8 byReturnType);
  CAnimus *GetRecallAnimus();
  bool IsHaveMentalTicket();
  bool IsMiningByMinigTicket();
  void UpdateLastMetalTicket(
    unsigned __int16 byCurrentYear,
    unsigned __int8 byCurrentMonth,
    unsigned __int8 byCurrentDay,
    unsigned __int8 byCurrentHour,
    unsigned __int8 byNumOfTime);
  void UpdateLastCriTicket(
    unsigned __int16 byCurrentYear,
    unsigned __int8 byCurrentMonth,
    unsigned __int8 byCurrentDay,
    unsigned __int8 byCurrentHour,
    unsigned __int8 byNumOfTime);
  void CheckMentalTakeAndUpdateLastMetalTicket(const char *strItemCode);
  bool IsUseReleaseRaceBuffPotion();
  void SetUseReleaseRaceBuffPotion();
  char IsOverOneDay();
  char mgr_tracing(int bOper);
  void SendMsg_AlterTol();
  void SendMsg_AlterTowerHP(unsigned __int16 wItemSerial, unsigned __int16 wLeftHP);
  void SendMsg_Destroy();
  void SendMsg_NewViewOther(unsigned __int8 byViewType);
  void SetLastAttBuff(bool bSet);
  bool IsLastAttBuff();
  void SetShapeAllBuffer();
  __int64 GetLevel();
  __int64 GetHP();
  __int64 GetMaxHP();
  __int64 CalcCurHPRate();
  __int64 CalcCurFPRate();
  __int64 CalcCurSPRate();
  unsigned __int64 GetStateFlag();
  void SetStateFlag();
  void SenseState();
  void SendMsg_StateInform(unsigned __int64 dwStateFlag);
  char IsPunished(unsigned __int8 byType, bool bSend);
  bool IsMapLoading();
  bool IsRidingShip();
  CMapData *GetBindMap(float *pfPos, bool bIgnoreMapClass);
  CMapData *GetBindMapData();
  _dummy_position *GetBindDummy();
  void SetBindMapData(CMapData *pMapData);
  void SetBindDummy(_dummy_position *pDummy);
  bool SetBindPosition(CMapData *pMap, _dummy_position *pDummy);
  void ClearGravityStone();
  void pc_SetInGuildBattle(bool bInGuildBattle, unsigned __int8 byColorInx);
  char pc_GiveItem(_STORAGE_LIST::_db_con *kItem, char *szReason, bool bDrop);
  int GetFP();
  int GetSP();
  int GetMaxFP();
  int GetMaxSP();
  int GetDP();
  int GetMaxDP();
  bool SetFP(int nFP, bool bOver);
  bool SetSP(int nSP, bool bOver);
  char SetHP(int nHP, bool bOver);
  char SF_HFSInc_Once(CPlayer *pDstObj);
  char SetDP(int nDP, bool bOver);
  void SendMsg_SetDPInform();
  void SendMsg_AlterMaxDP();
  void SendMsg_TransformSiegeModeResult(char byRetCode);
  void SendMsg_ReleaseSiegeModeResult(char byRetCode);
  void SendMsg_TransShipRenewTicketResult(char byErrCode);
  void SendMsg_TrunkEstResult(unsigned __int8 byRetCode, unsigned int dwLeftDalant);
  void SendMsg_TrunkDownloadResult(unsigned __int8 byRetCode);
  void SendMsg_TrunkChangPasswdResult(char byRetCode);
  void SendMsg_TrunkExtendResult(
    char byRetCode,
    unsigned __int8 bySlotNum,
    unsigned int dwLeftDalant,
    unsigned int dwConsumDanlant);
  void SendMsg_TrunkResDivision(char byErrCode, _STORAGE_LIST::_db_con *pStartOre, _STORAGE_LIST::_db_con *pTargetOre);
  void SendMsg_TrunkPotionDivision(
    unsigned __int16 wSerial,
    unsigned __int16 wParentAmount,
    unsigned __int16 wChildSerial,
    unsigned __int16 wChildAmount,
    char nRet);
  void SendMsg_TrunkIoResult(
    char byCase,
    char byRetCode,
    unsigned int dwLeftDalant,
    unsigned int dwConsumDanlant);
  void SendMsg_TrunkIoMoneyResult(
    char byRetCode,
    long double dTrunkDalant,
    long double dTrunkGold,
    unsigned int dwDalant,
    unsigned int dwGold,
    unsigned int dwFeeDalant);
  void SendMsg_TrunkPwHintIndexResult(char byRetCode, char byHintIndex);
  void SendMsg_TrunkHintAnswerResult(char byRetCode, char *pwszPassword);
  void ReCalcMaxHFSP(bool bSend, bool bRatio);
  int _CalcMaxHP();
  int _CalcMaxFP();
  int _CalcMaxSP();
  void RecvHSKQuest(
    unsigned __int8 byHSKQuestCode,
    unsigned __int8 byCristalBattleDBInfo,
    int nPvpPoint,
    unsigned __int16 wKillPoint,
    unsigned __int16 wDieCount,
    unsigned __int8 byHSKTime);
  void HSKQuestEnd_Att(unsigned __int8 byDestroyStoneRaceCode, CPlayer *pDestroyer);
  void SendMsg_RecvHSKQuest();
  void AlterPvPPoint(double dAlter, int AlterType, unsigned int dwDstSerial);
  void SetGrade(unsigned __int8 byGrade);
  void SetRankRate(unsigned __int16 wRankRate, unsigned int dwRank);
  void SendMsg_AlterGradeInform();
  void SendMsg_AlterPvPRank(unsigned __int16 wPvpRate, unsigned int dwPvpRank);
  void SendMsg_RaceTopInform(char bTop);
  void SendMsg_PvpRankListVersionUp(char byVersion);
  void SendMsg_AlterPvPPoint();
  CPvpPointLimiter *GetPvpPointLimiter(CPvpPointLimiter *result);
  CPvpOrderView *GetPvpOrderView();
  void SendMsg_RaceBattlePenelty(int nAlterPoint, char byAlterType);
  void SetCntEnable(bool bSet);
  void ExtractStringToTime(unsigned int dwTemp, _SYSTEMTIME *tm);
  void AlterPvpPointLeak(long double dAlter);
  void SetPvpPointLeak(long double dValue);
  bool pc_Resurrect(bool bQuickPotion);
  void SendMsg_ResurrectInform();
  void SendMsg_Resurrect(char byRet, bool bQuickPotion);
  void pc_NuclearAfterEffect();
    void SendMsg_RewardAddItem(_STORAGE_LIST::_db_con *pItem, unsigned __int8 byReason);
    void SendMsg_StartContSF(_sf_continous *pCont);
    void SendMsg_Circle_DelEffect(
      unsigned __int8 byEffectCode,
      unsigned __int16 wEffectIndex,
      unsigned __int8 byLv,
      bool bToOne);
    void SendMsg_AlterContEffectTime(unsigned __int8 byContType);
    void SendMsg_TLStatusPenalty(char byErrCode);
    void SendMsg_TLStatusInfo(unsigned int dwFatigue, unsigned __int8 wStatus);
    void SendMsg_UpdateTLStatusInfo(unsigned int dwFatigue, unsigned __int8 wStatus);
    void SendMsg_MonsterAggroData(CCharacter *pCharacter);
    int Emb_UpdateStat(unsigned int dwStatIndex, unsigned int dwNewData, unsigned int dwOldData);
    void SendMsg_StatInform(unsigned __int8 byStatIndex, unsigned int dwNewStat, unsigned __int8 byReason);
    void SendMsg_ProposeVoteResult(char byRetCode);
    void pc_RequestPatriarchPunishment(unsigned __int8 byType, char *pwszName, char *pwszCont);
    CGameObject *GetTargetObj();
    __target *GetGroupTarget(unsigned __int8 byGroupType);
  void SendMsg_SetGroupTargetObjectResult(char byRetCode, char byGroupType);
  void SendMsg_SetGroupMapPoint(char byRetCode, char byGroupType, char byMapCode, float *pzTar, unsigned __int8 byRemain);
  void SendMsg_ReleaseGroupTargetObjectResult(unsigned __int8 byGroupType);
  void SendMsg_BuddyDelResult(char byRetCode, unsigned int dwSerial);
  void SendMsg_BuddyLogoffInform(unsigned int dwObjSerial);
  void SendMsg_BuddyLoginInform(unsigned int dwObjSerial, unsigned __int8 byMapIndex, unsigned __int8 byPosCode);
  void SendMsg_BuddyPosInform(unsigned int dwDstSerial, unsigned __int8 byMapIndex, unsigned __int8 byPosCode);
  void SendMsg_GM_Greeting(char *wszGMName, char *wszMsg);
  void SendMsg_RACE_Greeting(char *wszBossName, char *wszMsg);
  void SendMsg_GUILD_Greeting(char *wszName, char *wszMsg);
  void SendMsg_Notify_ExceptFromRaceRanking(int bExcepted);
  void SendMsg_GuildMasterEffect(
    char byState,
    char byGrade,
    char byEffSubAttack,
    unsigned __int8 byEffSubDefence,
    unsigned __int8 byEffAddAttack,
    unsigned __int8 byEffAddDefence);
  void SendMsg_GuildSelfLeaveResult(char byRetCode);
  void pc_GuildSelfLeaveRequest();
  void pc_GuildManageRequest(
    unsigned __int8 byType,
    unsigned int dwDst,
    unsigned int dwObj1,
    unsigned int dwObj2,
    unsigned int dwObj3);
  void pc_ProposeVoteRequest(unsigned __int8 byLimGrade, char *pwszCont);
  void pc_GuildEstablishRequest(char *pwszGuildName);
  void pc_GuildDownLoadRequest();
  void pc_GuildJoinApplyRequest(char *pwszGuildName);
  void pc_GuildJoinApplyCancelRequest();
  void pc_GuildJoinAcceptRequest(unsigned int dwApplierSerial, bool bAccept);
  void pc_GuildOfferSuggestRequest(
    unsigned __int8 byMatterType,
    unsigned int dwMatterDst,
    char *pwszComment,
    unsigned int dwMatterObj1,
    unsigned int dwMatterObj2,
    unsigned int dwMatterObj3);
  void pc_GuildCancelSuggestRequest(unsigned int dwMatterVoteSynKey);
  void pc_GuildVoteRequest(unsigned int dwMatterVoteSynKey, unsigned __int8 byVoteCode);
  void pc_GuildQueryInfoRequest(unsigned int dwGuildSerial);
  void pc_GuildPushMoneyRequest(unsigned int dwPushDalant, unsigned int dwPushGold);
  void pc_GuildRoomRentRequest(_guildroom_rent_request_clzo *pProtocol);
  void pc_GuildRoomEnterRequest(_guildroom_enter_request_clzo *pProtocol);
  void pc_GuildRoomOutRequest(_guildroom_out_request_clzo *pProtocol);
  void pc_GuildHonorListRequest(unsigned __int8 byUI);
  void pc_GuildSetHonorRequest(_guild_honor_set_request_clzo *pData);
  void pc_GuildListRequest(unsigned __int8 byPage);
  void pc_GuildNextHonorListRequest();
  void pc_BuddyDownloadRequest();
  void pc_BuddyAddRequest(unsigned __int16 wDstIndex, unsigned int dwDstSerial, char *pwszDstName);
  void pc_BuddyAddAnswer(bool bAccept, unsigned __int16 wAskerIndex, unsigned int dwAskerSerial);
  void pc_BuddyDelRequest(unsigned int dwSerial);
  void SendMsg_BuddyAddFail(char byRetCode, char *pwszDstName);
  void SendMsg_BuddyAddAsk(unsigned __int16 wAskerIndex, unsigned int dwAskerSerial, char *pwszAskerName);
  void SendMsg_BuddyAddAnswerResult(
    char byRetCode,
    char bAccept,
    unsigned int dwAskerSerial,
    unsigned __int16 wIndex,
    unsigned int dwSerial,
    char *pwszCharName);
  void SendMsg_GuildForceLeaveBoradori();
  void pc_PartyLeaveSelfReqeuest();
  void SendMsg_PartyLeaveSelfResult(CPartyPlayer *pLeaver, bool bWorldExit);
  void SendMsg_PartySuccessResult(CPartyPlayer *pSuccessor);
  void ForcePullUnit(bool bLogout);
  void _UpdateUnitDebt(unsigned __int8 bySlotIndex, unsigned int dwPull);
  bool _LockUnitKey(unsigned __int8 bySlotIndex, bool bLock);
  void SendMsg_UnitForceReturnInform(char bySlotIndex, unsigned int dwDebt);
  void SendMsg_UnitAlterFeeInform(char bySlotIndex, unsigned int dwPullingFee);
  void SendMsg_AlterBooster();
  void AddDalant(int dwPush, bool bApply);
  void SubDalant(unsigned int dwSub);
  void SendMsg_RemainOreRate();
  void SendMsg_OreTransferCount();
  void SendMsg_LendItemTimeExpired(char byStorageCode, unsigned __int16 wSerial);
  void pc_TakeGroundingItem(CItemBox *pBox, unsigned __int16 wAddSerial);
  void pc_ThrowStorageItem(_STORAGE_POS_INDIV *pItem);
  void pc_UsePotionItem(CPlayer *pTargetPlayer, _STORAGE_POS_INDIV *pItem);
  char IsEquipAbleGrade(unsigned __int8 byGradeLv);
  void Emb_EquipLink();
  void pc_EquipPart(_STORAGE_POS_INDIV *pItem);
  void pc_EmbellishPart(_STORAGE_POS_INDIV *pItem, unsigned __int16 wChangeSerial);
  void pc_OffPart(_STORAGE_POS_INDIV *pItem);
  void pc_MakeItem(
    _STORAGE_POS_INDIV *pipMakeTool,
    unsigned __int16 wManualIndex,
    unsigned __int8 byMaterialNum,
    _STORAGE_POS_INDIV *pipMaterials);
  void pc_UpgradeItem(
    _STORAGE_POS_INDIV *pposTalik,
    _STORAGE_POS_INDIV *pposToolItem,
    _STORAGE_POS_INDIV *pposUpgItem,
    unsigned __int8 byJewelNum,
    _STORAGE_POS_INDIV *pposUpgJewel);
  void pc_DowngradeItem(_STORAGE_POS_INDIV *pposTalik, _STORAGE_POS_INDIV *pposToolItem, _STORAGE_POS_INDIV *pposUpgItem);
  void pc_AddBag(unsigned __int16 wBagItemSerial);
  char pc_UseRecoverLossExpItem(unsigned __int16 wItemSerial);
  void pc_CombineItem(
    unsigned __int16 wManualIndex,
    unsigned __int8 byMaterialNum,
    _STORAGE_POS_INDIV *pipMaterials,
    unsigned __int16 wOverlapItemSerial);
  void pc_ExchangeItem(unsigned __int16 wManualIndex, unsigned __int16 wItemSerial);
  void pc_CombineItemEx(_combine_ex_item_request_clzo *pRecv);
  void pc_CombineItemExAccept(_combine_ex_item_accept_request_clzo *pRecv);
  __int64 pc_UseFireCracker(unsigned __int16 wItemSerial);
  unsigned __int8 pc_UserSoccerBall(unsigned __int16 wItemSerial, unsigned __int16 *wItemIndex);
  char pc_UseRadarItem(_STORAGE_POS_INDIV *pItem, unsigned __int16 *pConsumeSerial);
  char pc_RadarCharInfo();
  char pc_NPCLinkCheckItemRequest(_STORAGE_POS_INDIV *pStorage);
  unsigned __int8 pc_NPCLinkCheckItemRequest_Check(_STORAGE_POS_INDIV *pStorage);
  unsigned __int8 pc_NPCLinkCheckItemRequest_Use(_STORAGE_POS_INDIV *pStorage);
  void pc_TalikCrystalExchange(unsigned __int8 byExchangeNum, _talik_crystal_exchange_clzo::_list *pList);
  unsigned __int8 pc_RenameItemNConditionCheck(_STORAGE_POS_INDIV *pItemInfo, _STORAGE_LIST::_db_con **ppItem);
  char pc_CharacterRenameCash(bool bChange, _STORAGE_POS_INDIV *pItem, const char *strCharacterName);
  char pc_CharacterRenameCheck(const char *strCharacterName);
  char pc_SetItemCheckRequest(
    unsigned int dwSetItem,
    unsigned __int8 bySetItemNum,
    unsigned __int8 bySetEffectNum,
    bool bSet);
  void ApplySetItemEffect(
    si_interpret *pSI,
    unsigned int dwSetItem,
    unsigned __int8 bySetItemNum,
    unsigned __int8 bySetEffectNum,
    bool bSetEffect);
  void pc_NewPosStart();
  void pc_Revival(bool bUseableJade);
  void pc_ExitWorldRequest();
  void pc_LinkBoardRequest();
  void pc_PostListRequest();
  void pc_PostContentRequest(unsigned int dwIndex);
  void pc_PostItemGoldRequest(unsigned int dwIndex);
  void pc_PostDeleteRequest(unsigned int dwIndex);
  void pc_PostReturnConfirmRequest(unsigned int dwPostSerial);
  void CheckPosInTown();
  void NewViewCircleObject();
  void CreateComplete();
  void SendMsg_MadeTrapNumInform(char byNum);
  void SendMsg_TowerContinue(unsigned __int16 wItemSerial, CGuardTower *pTwr);
  void SendMsg_ReEnterAsk(unsigned __int16 wChannelIndex, unsigned int dwChannelSerial);
  void SendMsg_MoveToOwnStoneMapResult(unsigned __int8 byRetCode, unsigned __int8 byMapIndex, float *pos);
  void SendMsg_MoveToOwnStoneMapInform(unsigned __int8 byStoneMapMoveInfo);
  void SendMsg_AnimusRecallResult(unsigned __int8 byResultCode, unsigned __int16 wLeftFP, CAnimus *pNewAnimus);
  void SendMsg_AnimusTargetResult(char byRetCode);
  void SendMsg_BackTowerResult(char byErrCode, unsigned __int16 wItemSerial, unsigned __int16 wLeftHP);
  void SendMsg_BackTrapResult(char byErrCode);
  void SendMsg_CancelSuggestResult(char byRetCode);
  void SendMsg_ClassSkillResult(
    unsigned __int8 byErrCode,
    const _CHRID *targetId,
    unsigned __int16 wSkillIndex);
  void SendMsg_CreateTowerResult(char byErrCode, unsigned int dwTowerObjSerial);
  void SendMsg_CreateTrapResult(char byErrCode, unsigned int dwTrapObjSerial);
  void SendMsg_DTradeAccomplishInform(bool bSucc, unsigned __int16 wStartSerial);
  void SendMsg_DTradeOKInform();
  void SendMsg_DTradeOKResult(char byErrCode);
  void SendMsg_ForceResult(
    unsigned __int8 byErrCode,
    const _CHRID *targetId,
    _STORAGE_LIST::_db_con *pForceItem,
    unsigned __int8 nSFLv);
  void SendMsg_GroupTargetInform(char byGroupType, const char *pwszName);
  void SendMsg_GuildJoinAcceptFail(char byRetCode, unsigned int dwApplierSerial);
  void SendMsg_GuildJoinApplyCancelResult(char byRetCode);
  void SendMsg_GuildJoinApplyRejectInform();
  void SendMsg_GuildJoinApplyResult(char byRetCode, CGuild *pApplyGuild);
  void SendMsg_GuildPushMoneyResult(char byRetCode);
  void SendMsg_GuildRoomEnterResult(
    unsigned __int8 byRetCode,
    unsigned __int8 bySubRetCode,
    unsigned __int8 byMapIndex,
    unsigned __int16 wMapLayer,
    float *pPos,
    int nRestTime);
  void SendMsg_GuildRoomRentResult(
    unsigned __int8 byRetCode,
    unsigned __int8 bySubRetCode,
    unsigned __int8 byRoomType);
  void SendMsg_GuildSetHonorResult(char byRetCode);
  void SendMsg_MineStartResult(unsigned __int8 resultCode);
  void SendMsg_NpcQuestListResult(_NPCQuestIndexTempData *pQuestIndexData);
  void SendMsg_OfferSuggestResult(char byRetCode);
  void SendMsg_OreCuttingResult(unsigned __int8 resultCode, unsigned __int8 leftOreCount, unsigned int consumedDalant);
  void SendMsg_OreIntoBagResult(char resultCode, unsigned __int16 newSerial, unsigned __int8 lendType, unsigned int lendTime);
  void SendMsg_ResultNpcQuest(char bSucc);
  void SendMsg_SetTargetObjectResult(char byRetCode, bool bForce);
  void SendMsg_SkillResult(
    unsigned __int8 byErrCode,
    const _CHRID *targetId,
    unsigned __int8 bySkillIndex,
    unsigned __int8 nSFLv);
  void SendMsg_ThrowSkillResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int8 bySkillIndex);
  void SendMsg_ThrowUnitResult(unsigned __int8 byErrCode, _CHRID *pidDst, unsigned __int16 wBulletIndex);
  void SendMsg_UILock_FindPW_Result(char resultCode, const char *password, char failCount);
  void SendMsg_UILock_Init_Request_ToAccount(
    unsigned int accountSerial,
    unsigned __int16 userIndex,
    const char *password,
    unsigned __int8 hintIndex,
    const char *hintAnswer);
  void SendMsg_UILock_Init_Result(char resultCode);
  void SendMsg_UILock_Login_Result(char resultCode, char failCount);
  void SendMsg_UILock_Update_Request_ToAccount(
    unsigned int accountSerial,
    unsigned __int16 userIndex,
    const char *password,
    unsigned __int8 hintIndex,
    const char *hintAnswer);
  void SendMsg_UILock_Update_Result(char resultCode);
  void SendMsg_UnitBulletFillResult(
    char byRetCode,
    char bySlotIndex,
    unsigned __int16 *pwBulletIndex,
    unsigned int *pdwConsumMoney);
  void SendMsg_UnitDeliveryResult(
    char byRetCode,
    char bySlotIndex,
    unsigned int dwParkingUnitSerial,
    unsigned int dwPayDalant);
  void SendMsg_UnitFrameBuyResult(
    char byRetCode,
    char byFrameCode,
    char byUnitSlotIndex,
    unsigned __int16 wKeyIndex,
    unsigned __int16 wKeySerial,
    unsigned int *pdwConsumMoney);
  void SendMsg_UnitFrameRepairResult(
    char byRetCode,
    char bySlotIndex,
    unsigned int dwNewGauge,
    unsigned int dwConsumDalant);
  void SendMsg_UnitLeaveResult(char byRetCode);
  void SendMsg_UnitPackFillResult(
    unsigned __int8 byRetCode,
    unsigned __int8 bySlotIndex,
    unsigned __int8 byFillNum,
    _unit_pack_fill_request_clzo::__list *pList,
    unsigned int *pdwConsumMoney);
  void SendMsg_UnitPartTuningResult(char byRetCode, char bySlotIndex, int *pnCost);
  void SendMsg_UnitSellResult(
    char byRetCode,
    char bySlotIndex,
    unsigned __int16 wKeySerial,
    int nAddMoney,
    unsigned int dwTotalNonpay,
    unsigned int dwSumDalant,
    unsigned int dwSumGold);
  void SendMsg_UnitTakeResult(char byRetCode);
  void SendMsg_VoteResult(unsigned int dwMatterVoteSynKey, unsigned __int8 byRetCode);
  void CheckGroupTargeting();
  void CheckGroupMapPoint();
  void SendMsg_StartNewPos(char byMapInMode);
  void SendMsg_BaseDownloadResult();
  void SendMsg_InvenDownloadResult();
  void SendMsg_CumDownloadResult();
  void SendMsg_ForceDownloadResult();
  void SendMsg_QuestDownloadResult();
  void SendMsg_QuestHistoryDownloadResult();
  void SendMsg_SpecialDownloadResult();
  void SendMsg_LinkBoardDownloadResult();
  void SendMsg_MacroRequest();
  void SendMsg_AMPInvenDownloadResult();
  void SendMsg_ChangeClassCommand();
  void SendMsg_SelectClassResult(char byErrCode, unsigned __int16 wSelClassIndex);
  void SendMsg_PcRoomCharClass(unsigned int dwPcRoomClassIndex);
  void SendMsg_PcRoomError(char byErrorCode);
  void SendMsg_BillingExipreInform(char byKind, unsigned __int16 wWaitSec);
  void ReservationForceClose();
  void Billing_Logout();
  void PushDQSCheatPlyerVoteInfo();
  void PushDQSUpdatePlyerVoteInfo();
  void PushDQSUpdateVoteAvilable();
  void SendMsg_Alter_Action_Point(unsigned __int8 byActCode, unsigned int dwActPoint);
  unsigned int GetInitClassCost();
  unsigned __int8 pc_InitClassRequest();
  unsigned __int8 pc_InitClass();
  void CalcAddPointByClass();
  void RewardChangeClassMastery(_class_fld *pClassFld);
  void RewardChangeClassRewardItem(_class_fld *pClassFld, unsigned __int8 bySelectRewardItem);
  void RewardChangeClass(_class_fld *pClassFld, unsigned __int8 bySelectRewardItem);
  void pc_SelectClassRequest(unsigned __int16 wSelClassIndex, unsigned __int8 bySelectRewardItem);
  unsigned __int8 pc_CanSelectClassRequest(bool *pIsRealClassUp);
  void SendMsg_StoreListResult();
  void SendMsg_BuyItemStoreResult(
    CItemStore *pStore,
    int nOfferNum,
    _buy_offer *pCard,
    unsigned __int8 byErrCode);
  void SendMsg_SellItemStoreResult(CItemStore *pStore, unsigned __int8 byErrCode);
  void SendMsg_StoreLimitItemAmountInfo(unsigned int dwStoreIndex, _limit_amount_info *pAmountInfo);
  void pc_BuyItemStore(
    CItemStore *pStore,
    unsigned __int8 byOfferNum,
    _buy_store_request_clzo::_list *pList,
    int bUseNPCLinkIntem);
  void pc_SellItemStore(
    CItemStore *pStore,
    unsigned __int8 byOfferNum,
    _sell_store_request_clzo::_list *pList,
    int bUseNPCLinkIntem);
  void pc_ExchangeDalantForGold(unsigned int dwDalant);
  void pc_ExchangeGoldForDalant(unsigned int dwGold);
  void pc_LimitItemNumRequest(unsigned int dwStoreIndex);
  void pc_PvpCashRecorver(unsigned __int16 dwItemSerial, unsigned __int8 byItemCnt);
  void SendMsg_RadarDelayTime(unsigned int dwDelay);
  void SendMsg_Init_Action_Point();
  void SendMsg_BuyCashItemMode();
  void SendMsg_ExitWorldResult(unsigned __int8 byRetCode);
  void SendMsg_Revival(char byRet, bool bEquialZone);
  void SendMsg_OtherShapeAll(CPlayer *pDst);
  void SendMsg_OtherShapePart(CPlayer *pDst);
  void SendMsg_OtherShapePartEx_CashChange(
    CPlayer *pDst,
    CashChangeStateFlag ServerData,
    CashChangeStateFlag ClinetData);
  void SendMsg_OtherShapeError(CPlayer *pDst, unsigned __int8 byErrCode);
  void SendMsg_DamageResult(_STORAGE_LIST::_db_con *pItem);
  void SendMsg_MaxHFSP();
  void SendMsg_ItemStorageRefresh(unsigned __int8 byStorageCode);
  void SendMsg_UsPotionResultOther(char byRetcode, unsigned __int16 wPotionIndex, CPlayer *pUsePlayer, bool bCircle);
  void SendMsg_UnitReturnResult(char byRetCode, unsigned int dwPayDalant);
  void SendMsg_UnitBulletReplaceResult(char byRetCode);
  void SendMsg_UnitDestroy(char bySlotIndex);
  void SendMsg_AnimusHPInform();
  void SendMsg_AnimusFPInform();
  void SendMsg_AnimusExpInform();
  void SendMsg_AnimusModeInform(char byMode);
  void SendMsg_AnimusRecallWaitTimeFree(char bFree);
  void SendMsg_MineCompleteResult(
    char byErrCode,
    unsigned __int8 byNewOreIndex,
    unsigned __int16 dwOreSerial,
    unsigned __int8 byOreDur,
    unsigned __int16 dwBatteryLeftDurPoint);
  void SendData_PartyMemberHP();
  void SendData_PartyMemberFP();
  void SendData_PartyMemberSP();
  void SendData_PartyMemberMaxHFSP();
  void SendData_PartyMemberEffect(unsigned __int8 byAlterCode, unsigned __int16 wEffectCode, unsigned __int8 byLv);
  void SendMsg_AlterHPInform();
  void SendMsg_HSKQuestSucc(char byQuestCode, char bSucc);
  void SendMsg_InsertQuestItemInform(_STORAGE_LIST::_db_con *pItem);
  void SendMsg_AddEffect(
    unsigned __int16 wEffectCode,
    unsigned __int8 byLv,
    unsigned __int16 wDurSec,
    unsigned int dwPlayerSerial,
    char *wszPlayerName);
  void SendMsg_DelEffect(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex, unsigned __int8 byLv);
  void SFContInsertMessage(
    unsigned __int8 byContCode,
    unsigned __int8 byListIndex,
    bool bAuraSkill,
    CPlayer *pPlayerAct);
  void SFContDelMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, bool bSend, bool bAura) override;
  void SFContUpdateTimeMessage(unsigned __int8 byContCode, unsigned __int8 byListIndex, int nLeftTime) override;
  void SendMsg_DTradeAskInform(CPlayer *pAsker);
  void SendMsg_DTradeAnswerResult(char byErrCode);
  void SendMsg_DTradeStartInform(CPlayer *pAsker, CPlayer *pAnswer, unsigned int *pdwKey);
  void SendMsg_DTradeLockResult(char byErrCode);
  void SendMsg_DTradeLockInform();
  void SendMsg_DTradeAddResult(char byErrCode);
  void SendMsg_DTradeAddInform(char bySlotIndex, _STORAGE_LIST::_db_con *pItem, unsigned __int8 byAmount);
  void SendMsg_DTradeDelResult(char byErrCode);
  void SendMsg_DTradeDelInform(char bySlotIndex);
  void SendMsg_DTradeBetResult(char byErrCode);
  void SendMsg_DTradeBetInform(char byUnitCode, unsigned int dwAmount);
  void SendMsg_DTradeUnitInfoInform(char byTradeSlotIndex, _UNIT_DB_BASE::_LIST *pUnitData);
  void SendMsg_DTradeUnitAddInform(unsigned __int16 wUnitKeySerial, _UNIT_DB_BASE::_LIST *pUnitData);
  void SendMsg_StartShopping();
  void SendMsg_TargetObjectHPInform();
  void SendMsg_RefeshGroupTargetPosition(char byGroupType);
  void SendMsg_CastVoteResult(char byRetCode);
  void SendMsg_ExtTrunkExtendResult(char byRetCode, unsigned __int8 bySlotNum, unsigned __int8 byLackSlotNum);
  void SendMsg_RemainTimeInform(__int16 iType, int lRemainTime, _SYSTEMTIME *pstEndDate);
  void SendMsg_BillingTypeChangeInform(
    __int16 iType,
    int lRemainTime,
    _SYSTEMTIME *pstEndDate,
    unsigned __int8 byReason);
  void SendTargetMonsterSFContInfo();
  void SendTargetPlayerDamageContInfo();
  void SendMsg_TeleportError(char byErrorCode, unsigned int dwMapIndex);
  void SendMsg_RevivalOfJade(unsigned __int16 wSuccRate);
  void SendMsg_UseJadeResult(unsigned __int8 byErrCode, unsigned __int16 wItemSerial);
  void SendMsg_BuddyNameReNewal(unsigned int dwSerial, char *wszName);
  void SendMsg_NotifyEffectForGetItem(
    char byBoxType,
    unsigned int dwCharSerial,
    char *szCharName,
    _STORAGE_LIST::_db_con *pItem,
    bool bCircle);
  void SendMsg_ApexInform(unsigned __int16 dwRecvSize, char *pMsg);
  void SendMsg_MaxPvpPointInform(int nMax);
  void SendMsg_NewMovePotionResult();
  void SendMsg_UnitRideChange(bool bTake, CParkingUnit *pUnit);
  void SendMsg_Die();
  void SendMsg_BreakdownEquipItem(unsigned __int8 byPartIndex, unsigned __int16 wSerial);
  void SendMsg_Notify_Gravity_Stone_Owner_Die();
  char _pre_check_normal_attack(
    CCharacter *pDst,
    unsigned __int16 wBulletSerial,
    bool bCount,
    _STORAGE_LIST::_db_con **ppBulletProp,
    _BulletItem_fld **ppfldBullet,
    unsigned __int16 wEffBtSerial,
    _STORAGE_LIST::_db_con **ppEffBtProp,
    _BulletItem_fld **ppfldEffBt);
  char _pre_check_skill_attack(
    CCharacter *pDst,
    float *pfAttackPos,
    unsigned __int8 byEffectCode,
    _skill_fld *pSkillFld,
    unsigned __int16 wBulletSerial,
    _STORAGE_LIST::_db_con **ppBulletProp,
    _BulletItem_fld **ppfldBullet,
    int nEffectGroup,
    unsigned __int16 *pdwDecPoint,
    unsigned __int16 wEffBtSerial,
    _STORAGE_LIST::_db_con **ppEffBtProp,
    _BulletItem_fld **ppfldEffBt);
  char _pre_check_force_attack(
    CCharacter *pDst,
    float *pfTarPos,
    unsigned __int16 wForceItemSerial,
    _force_fld **ppForceFld,
    _STORAGE_LIST::_db_con **ppForceItem,
    unsigned __int16 *pdwDecPoint,
    unsigned __int16 wEffBtSerial,
    _STORAGE_LIST::_db_con **ppEffBtProp,
    _BulletItem_fld **ppfldEffBt);
  char _pre_check_unit_attack(
    CCharacter *pDst,
    unsigned __int8 byWeaponPart,
    _UnitPart_fld **ppWeaponFld,
    _UnitBullet_fld **ppBulletFld,
    _unit_bullet_param **ppBulletParam);
  char _pre_check_siege_attack(
    CCharacter *pDst,
    float *pfAttackPos,
    unsigned __int16 wBulletSerial,
    _STORAGE_LIST::_db_con **ppBulletProp,
    _BulletItem_fld **ppfldBullet,
    unsigned __int16 wEffBtSerial,
    _STORAGE_LIST::_db_con **ppEffBulletProp,
    _BulletItem_fld **ppfldEffBullet);
  char _pre_check_wpactive_skill_attack(
    unsigned __int8 byEffectCode,
    _skill_fld *pSkillFld,
    unsigned __int16 wBulletSerial,
    _STORAGE_LIST::_db_con **ppBulletProp,
    _BulletItem_fld **ppfldBullet);
  bool _pre_check_wpactive_force_attack();
  int _pre_check_in_guild_battle(CPlayer *pDst);
  bool _pre_check_in_guild_battle_race(CPlayer *pDst, bool bEqueal);
  _STORAGE_LIST::_db_con *IsBulletValidity(unsigned __int16 wBulletSerial);
  _STORAGE_LIST::_db_con *IsEffBulletValidity(unsigned __int16 wEffBulletSerial);
  char IsSFUseableRace(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex);
  char IsSFUsableGauge(unsigned __int8 byEffectCode, unsigned __int16 wEffectIndex, unsigned __int16 *pwDelPoint);
  char IsSFUsableSFMastery(unsigned __int8 byMasteryCode, int nMasteryIndex);
  char IsSFActableByClass(unsigned __int8 byEffectCode, _base_fld *pSFFld);
  unsigned __int8 _GetPartyMemberInCircle(CPlayer **out_ppMember, int nMax, bool bOne);
  bool _pre_check_skill_enable(_skill_fld *pSkillFld);
  char _pre_check_skill_gradelimit(_skill_fld *pSkillFld);
  unsigned __int8 skill_process(
    int nEffectCode,
    int nSkillIndex,
    _CHRID *pidDst,
    unsigned __int16 *pConsumeSerial,
    int *pnLv);
  void skill_process_for_aura(int nSkillIndex);
  unsigned __int8 skill_process_for_item(int nSkillIndex, _CHRID *pidDst, int *pnLv);
  void make_gen_attack_param(
    CCharacter *pDst,
    unsigned __int8 byPart,
    _BulletItem_fld *pBulletFld,
    float fAddBulletFc,
    _attack_param *pAP,
    _BulletItem_fld *pEffBtFld,
    float fAddEffBtFc);
  void make_skill_attack_param(
    CCharacter *pDst,
    float *pfAttackPos,
    unsigned __int8 byEffectCode,
    _skill_fld *pSkillFld,
    int nAttType,
    _STORAGE_LIST::_db_con *pBulletItem,
    float fAddBulletFc,
    _attack_param *pAP,
    _STORAGE_LIST::_db_con *pEffBulletItem,
    float fAddEffBulletFc);
  void make_force_attack_param(
    CCharacter *pDst,
    _force_fld *pForceFld,
    _STORAGE_LIST::_db_con *pForceItem,
    float *pTar,
    _attack_param *pAP,
    _STORAGE_LIST::_db_con *pEffBulletItem,
    float fAddEffBtFc);
  void make_unit_attack_param(CCharacter *pDst, _UnitPart_fld *pWeaponFld, float fAddBulletFc, _attack_param *pAP);
  void make_siege_attack_param(
    CCharacter *pDst,
    float *pfAttackPos,
    unsigned __int8 byPart,
    _BulletItem_fld *pBulletFld,
    float fAddBulletFc,
    _attack_param *pAP,
    _BulletItem_fld *pEffBulletFld,
    float fAddEffBtFc);
  void make_wpactive_skill_attack_param(
    CCharacter *pDst,
    _skill_fld *pSkillFld,
    float *pfAttackPos,
    unsigned __int8 byEffectCode,
    int nAttType,
    _STORAGE_LIST::_db_con *pBulletItem,
    float fAddBulletFc,
    _attack_param *pAP,
    int *nShotNum);
  void make_wpactive_force_attack_param(CCharacter *pDst, _force_fld *pForceFld, float *pfAttackPos, _attack_param *pAP);
  char WeaponSFActive(
    _be_damaged_char *pDamList,
    int *nDamagedObjNum,
    int *nShotNum,
    unsigned __int16 wBulletSerial);
  char pc_WPActiveAttack_Skill(
    _be_damaged_char *pDamList,
    int *nDamagedObjNum,
    int *nShotNum,
    _skill_fld *pSkillFld,
    unsigned __int8 byEffectCode,
    unsigned __int16 wBulletSerial);
  char pc_WPActiveAttack_Force(_be_damaged_char *pDamList, int *nDamagedObjNum, _force_fld *pForceFld);
  bool WPActiveSkill(_be_damaged_char *pDamList, int nDamagedObjNum, _skill_fld *pSkillFld, int nEffectCode);
  bool WPActiveForce(_be_damaged_char *pDamList, int nDamagedObjNum, _force_fld *pForceFld);
  void pc_PlayAttack_Gen(
    CCharacter *pDst,
    unsigned __int8 byAttPart,
    unsigned __int16 wBulletSerial,
    unsigned __int16 wEffBtSerial,
    bool bCount);
  void pc_PlayAttack_Skill(
    CCharacter *pDst,
    float *pfAttackPos,
    unsigned __int8 byEffectCode,
    unsigned __int16 wSkillIndex,
    unsigned __int16 wBulletSerial,
    unsigned __int16 *pConsumeSerial,
    unsigned __int16 wEffBtSerial);
  void pc_PlayAttack_Force(
    CCharacter *pDst,
    float *pfAreaPos,
    unsigned __int16 wForceSerial,
    unsigned __int16 *pConsumeSerial,
    unsigned __int16 wEffBtSerial);
  void pc_PlayAttack_Unit(CCharacter *pDst, unsigned __int8 byWeaponPart);
  void pc_PlayAttack_Siege(
    CCharacter *pDst,
    float *pfAttackPos,
    unsigned __int8 byAttPart,
    unsigned __int16 wBulletSerial,
    unsigned __int16 wEffBtSerial);
  void pc_PlayAttack_Test(
    unsigned __int8 byEffectCode,
    unsigned __int8 byEffectIndex,
    unsigned __int16 wBulletItemSerial,
    unsigned __int8 byWeaponPart,
    __int16 *pzTar);
  void pc_PlayAttack_SelfDestruction();
  unsigned int _check_exp_after_attack(
    int nDamagedObjNum,
    _be_damaged_char *pList,
    CPartyModeKillMonsterExpNotify *kPartyExpNotify);
  void _check_dst_param_after_attack(int nTotalDam, CCharacter *pTarget);
  bool IsPassExpLimitLvDiff(int iDstLevel, bool *bGetAttackExp);
  float GetPartyExpDistributionRate(int iPartyMemberLevel, int iMaxLevel, int i2ndLevel);
  void CalcExp(CCharacter *pDst, int nDam, CPartyModeKillMonsterExpNotify *kPartyExpNotify);
  void SendMsg_AttackResult_Gen(CAttack *pAt, unsigned __int16 wBulletIndex);
  void SendMsg_AttackResult_Skill(unsigned __int8 byEffectCode, CPlayerAttack *pAt, unsigned __int16 wBulletIndex);
  void SendMsg_AttackResult_Force(CAttack *pAt);
  void SendMsg_AttackResult_Unit(CAttack *pAt, unsigned __int8 byWeaponPart, unsigned __int16 wBulletIndex);
  void SendMsg_AttackResult_Siege(CAttack *pAt, unsigned __int16 wBulletIndex);
  void SendMsg_AttackResult_Count(CAttack *pAt);
  void SendMsg_AttackResult_SelfDestruction(CAttack *pAt);
  void SendMsg_AttackResult_Error(char nErrCode);
  void SendMsg_TestAttackResult(
    unsigned __int8 byEffectCode,
    unsigned __int8 byEffectIndex,
    unsigned __int16 wBulletItemIndex,
    unsigned __int8 byEffectLv,
    unsigned __int8 byWeaponPart,
    __int16 *pzTar);
  void SendMsg_AlterWeaponBulletInform(unsigned __int16 wItemSerial, unsigned __int16 wLeftNum);
  void SendMsg_AlterUnitBulletInform(char byPart, unsigned __int16 wLeftNum);
  void SendMsg_Recover();
  static void OnLoop_Static();
  void UpdatePvpPointLimiter(__int64 tCurTime);
  void UpdatePvpOrderView(__int64 tCurTime);
  void AutoRecover();
  void AutoRecover_Animus();
  void AutoCharge_Booster();
  char GetUseConsumeItem(
    _consume_item_list *pConsumeList,
    unsigned __int16 *pItemSerials,
    _STORAGE_LIST::_db_con **ppConsumeItems,
    int *pnConsume,
    bool *pbOverLap);
  char DeleteUseConsumeItem(_STORAGE_LIST::_db_con **ppConsumeItems, int *pnConsume, bool *pbOverLap);
  int GetGauge(int nParamCode);
  void SetGauge(int nParamCode, int nValue, bool bOver);
  __int64 CalcEquipAttackDelay();
  __int64 GetMasteryCumAfterAttack(int nDstLv);
  bool IsPassMasteryLimitLvDiff(int iDstLevel);
  void SetBattleMode(bool bAttack);
  void AlterSec() override;
  __int64 AttackableHeight();
  __int64 GetAttackDP();
  __int64 GetAttackLevel();
  float GetAttackRange();
  __int64 GetAvoidRate() override;
  __int64 GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  __int64 GetDefSkill(bool bBackAttackDamage) override;
  __int64 GetFireTol() override;
  char *GetObjName() override;
  __int64 GetObjRace() override;
  __int64 GetSoilTol() override;
  __int64 GetWaterTol() override;
  float GetWeaponAdjust() override;
  __int64 GetWeaponClass() override;
  float GetWidth() override;
  __int64 GetWindTol() override;
  __int64 GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack);
  bool IsBeAttackedAble(bool bFirst);
  char IsBeDamagedAble(CCharacter *pAtter) override;
  char IsRecvableContEffect() override;
  bool Is_Battle_Mode() override;
  void Loop() override;
  void OutOfSec() override;
  void RecvKillMessage(CCharacter *pDier) override;
  void SendMsg_FixPosition(int n) override;
  void SendMsg_RealMovePoint(int n) override;
  void SendMsg_SetHPInform() override;
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  void SetAttackPart(int nAttactPart);
  char Load(CUserDB *pUser, bool bFirstStart);
  char dev_after_effect();
  char dev_all_kill();
  char dev_animus_recall_time_free(bool bFree);
  char dev_avator_copy(char *pwszDstName);
  bool dev_change_class(char *pszClassCode);
  char dev_cont_effect_del();
  char dev_cont_effect_time(unsigned int dwSec);
  char dev_dalant(unsigned int dwDalant);
  char dev_die();
  char dev_drop_item(char *pszItemCode, int nNum, char *pszUpTalCode);
  char dev_free_sf_by_class();
  char dev_full_animus_gauge();
  bool dev_full_force();
  char dev_full_point();
  char dev_gold(unsigned int dwGold);
  char dev_goto_monster(CMonster *pMon);
  char dev_goto_npc(CMerchant *pNpc);
  char dev_half_inven_amount(unsigned __int64 dwAmount);
  char dev_half_point();
  char dev_init_monster();
  char dev_inven_empty();
  char dev_item_make_no_use_matrial(bool noUsingMatrial);
  char dev_loot_bag();
  char dev_loot_free(bool bFree);
  char dev_loot_fullitem(unsigned __int8 byLv);
  bool dev_loot_item(char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum);
  char dev_loot_material();
  char dev_loot_mine();
  char dev_loot_tower();
  char dev_lv(int nLv);
  char dev_make_succ(bool bSucc);
  char dev_max_level_ext(unsigned __int8 byMaxLevel);
  char dev_never_die(bool bSet);
  char dev_quest_complete();
  char dev_quest_complete_other(char *pwszCharName);
  char dev_set_animus_exp(unsigned __int64 dwExpPoint);
  char dev_set_animus_lv(int nAnimusLv);
  char dev_set_hp(float prob);
  char dev_SetGuildGrade(unsigned __int8 byGrade);
  char dev_SetGuildGradeByGuildSerial(unsigned int dwGuildSerial, unsigned __int8 byGrade);
  char dev_SetGuildGradeByName(char *uszGuildName, unsigned __int8 byGrade);
  char dev_trap_attack_grade(int nPoint);
  char dev_up_all(int nCum);
  char dev_up_all_pt(int nLv);
  char dev_up_cashbag(long double dPoint);
  char dev_up_forceitem(int nCum);
  char dev_up_forcemastery(unsigned int nCum);
  char dev_up_mastery(int nMasteryCode, unsigned int nMasteryIndex, int nLv);
  char dev_up_pvp(long double dPoint);
  char dev_up_skill(char *pszSkillCode, unsigned int nCum);
  char dev_view_boss();
  char dev_view_method(char *pwszDstName);
  char mgr_all_item_muzi(unsigned int nLv);
  char mgr_change_degree(unsigned __int8 nDegree);
  char mgr_defense_item_grace(unsigned __int8 byItemCode, unsigned int nLv);
  char mgr_destroy_system_tower();
  char mgr_dungeon_pass();
  bool mgr_exit_keeper();
  bool mgr_exit_stone();
  char mgr_free_ride_ship();
  char mgr_goto_mine();
  char mgr_goto_shipport(int nRaceCode, int nPort);
  char mgr_goto_stone(unsigned __int8 byRaceCode);
  char mgr_goto_store(unsigned __int8 nRaceCode, char *pszNPCName);
  char mgr_gotoCoordinates(char *pszMapCode, float fX, float fY, float fZ);
  char mgr_gotoDstCoordinates(char *pwszDstName, char *pszMapCode, float fX, float fY, float fZ);
  bool mgr_holykeeper_start(int nRace);
  bool mgr_holystone_start(int nNumOfTime);
  char mgr_item_telekinesis();
  char mgr_kick(char *pwszCharName);
  char mgr_make_system_tower(char *pszTowerCode);
  char mgr_matchless(bool bMatchless);
  char mgr_MaxAttackPoint(int nMax);
  char mgr_pass_sch_one_step();
  char mgr_recall_guild_player(char *wszDestCharName);
  char mgr_recall_mon(char *pszMonCode, int nCreateNum);
  char mgr_recall_party_player(char *wszDestCharName);
  char mgr_recall_player(char *pwszCharName);
  bool mgr_resurrect_player(char *pwszCharName);
  char mgr_set_animus_attack_point(int nPoint);
  char mgr_TrunkInit();
  char mgr_user_ban(char *uszCharName, int iPeriod, char *uszReason, char byBlockType);
  char mgr_whisper(char *pwszMsg);

  bool m_bLoad;
  bool m_bOper;
  bool m_bPostLoad;
  bool m_bFullMode;
  unsigned __int8 m_byUserDgr;
  unsigned __int8 m_bySubDgr;
  bool m_bFirstStart;
  bool m_bOutOfMap;
  unsigned __int8 m_byMoveDirect;
  unsigned __int8 m_byPlusKey;
  unsigned __int16 m_wXorKey;
  unsigned int m_dwMoveCount;
  unsigned int m_dwTargetCount;
  unsigned int m_dwAttackCount;
  bool m_bBaseDownload;
  bool m_bInvenDownload;
  bool m_bForceDownload;
  bool m_bCumDownload;
  bool m_bQuestDownload;
  bool m_bSpecialDownload;
  bool m_bLinkBoardDownload;
  bool m_bAMPInvenDownload;
  bool m_bGuildListDownload;
  bool m_bGuildDownload;
  bool m_bBuddyListDownload;
  bool m_bBlockParty;
  bool m_bBlockWhisper;
  bool m_bBlockTrade;
  bool m_bCreateComplete;
  unsigned int m_dwSelfDestructionTime;
  float m_fSelfDestructionDamage;
  bool m_bTakeGravityStone;
  bool m_bBlockGuildBattleMsg;
  bool m_bInGuildBattle;
  bool m_bNotifyPosition;
  unsigned __int8 m_byGuildBattleColorInx;
  bool m_bTakeSoccerBall;
  _STORAGE_LIST::_db_con *m_pSoccerItem;
  CUserDB *m_pUserDB;
  CPartyPlayer *m_pPartyMgr;
  CPlayerDB m_Param;
  _CLID m_id;
  unsigned __int8 m_byMoveType;
  unsigned __int8 m_byModeType;
  unsigned __int8 m_byStandType;
  CRealMoveRequestDelayChecker m_kMoveDelayChecker;
  _WEAPON_PARAM m_pmWpn;
  _DTRADE_PARAM m_pmTrd;
  _MASTERY_PARAM m_pmMst;
  _TOWER_PARAM m_pmTwr;
  _TRAP_PARAM m_pmTrp;
  _BUDDY_LIST m_pmBuddy;
  CQuestMgr m_QuestMgr;
  ItemCombineMgr m_ItemCombineMgr;
  unsigned __int8 m_byMapInModeBuffer;
  int m_nVoteSerial;
  unsigned int m_dwLastCheckRegionTime;
  unsigned __int16 m_wRegionMapIndex;
  unsigned __int16 m_wRegionIndex;
  unsigned __int8 m_byHSKQuestCode;
  MiningTicket m_MinigTicket;
  int m_nHSKPvpPoint;
  unsigned __int16 m_wKillPoint;
  unsigned __int8 m_byHSKTime;
  unsigned __int16 m_wDiePoint;
  unsigned __int8 m_byCristalBattleDBInfo;
  CSetItemEffect m_clsSetItem;
  CDarkHoleChannel *m_pDHChannel;
  float m_fTalik_DefencePoint;
  float m_fTalik_AvoidPoint;
  bool m_bCheat_100SuccMake;
  bool m_bCheat_makeitem_no_use_matrial;
  bool m_bCheat_Matchless;
  bool m_bFreeRecallWaitTime;
  bool m_bFreeSFByClass;
  bool m_bLootFree;
  bool m_bNeverDie;
  int m_nMaxAttackPnt;
  bool m_bSpyGM;
  int m_nAnimusAttackPnt;
  int m_nTrapMaxAttackPnt;
  unsigned __int8 m_byDamagePart;
  bool m_bRecvMapChat;
  bool m_bRecvAllChat;
  CEquipItemSFAgent EquipItemSFAgent;
  _CRYMSG_LIST m_pmCryMsg;
  bool m_bSnowMan;
  unsigned __int8 m_byStoneMapMoveInfo;
  unsigned int m_dwPatriarchAppointTime;
  unsigned __int8 m_byPatriarchAppointPropose;
  bool m_bAfterEffect;
  bool m_bSFDelayNotCheck;
  _RENAME_POTION_USE_INFO m_ReNamePotionUseInfo;
  CPlayer::CashChangeStateFlag m_CashChangeStateFlag;
  _NPCQuestIndexTempData m_NPCQuestIndexTempData;
  unsigned __int16 m_wVisualVer;
  int m_nLastBeatenPart;
  unsigned __int64 m_dwLastState;
  unsigned int m_dwExpRate;
  int m_nAddDfnMstByClass;
  int m_nAddPointByClass[4];
  int m_nMaxPoint[4];
  int m_nTolValue[4];
  int m_nMaxDP;
  __int16 m_zLastTol[4];
  float m_fEquipSpeed;
  int m_nOldPoint[4];
  int m_nOldMaxDP;
  float m_fSendTarPos[2];
  unsigned __int8 m_byLastDirect;
  float m_fLastRecvPos[3];
  unsigned __int8 m_byLastRecvMapIndex;
  unsigned int m_dwLastTakeItemTime;
  int m_nCheckMovePacket;
  bool m_bCheckMovePacket;
  unsigned __int8 m_byDefMatCount;
  _100_per_random_table m_MakeRandTable;
  CMapData *m_pBindMapData;
  _dummy_position *m_pBindDummyData;
  unsigned int m_dwNextTimeDungeonDie;
  unsigned int m_dwLastTimeCheckUnitViewOver;
  unsigned int m_dwUnitViewOverTime;
  _UNIT_DB_BASE::_LIST *m_pUsingUnit;
  CParkingUnit *m_pParkingUnit;
  unsigned __int8 m_byUsingWeaponPart;
  int m_nUnitDefFc;
  _STORAGE_LIST::_db_con *m_pSiegeItem;
  bool m_bIsSiegeActing;
  CMyTimer m_tmrSiegeTime;
  _STORAGE_LIST::_db_con *m_pRecalledAnimusItem;
  CAnimus *m_pRecalledAnimusChar;
  unsigned int m_dwLastRecallTime;
  unsigned __int8 m_byNextRecallReturn;
  unsigned __int16 m_wTimeFreeRecallSerial;
  CMyTimer m_tmrIntervalSec;
  unsigned int m_dwLastSetPointTime;
  CMyTimer m_tmrBilling;
  float m_fBeforeDungeonPos[3];
  CMapData *m_pBeforeDungeonMap;
  _MEM_PAST_WHISPER m_PastWhiper[10];
  unsigned int m_dwContItemEffEndTime;
  CPotionParam m_PotionParam;
  CExtPotionBuf m_PotionBufUse;
  int m_bCntEnable;
  _SYSTEMTIME m_tmLoginTime;
  _SYSTEMTIME m_tmCalcTime;
  CMyTimer m_tmrAccumPlayingTime;
  bool m_bUpCheckEquipEffect;
  bool m_bDownCheckEquipEffect;
  unsigned __int8 m_byEffectEquipCode[15];
  unsigned __int8 m_byPosRaceTown;
  CMapData *m_pBeforeTownCheckMap;
  float m_fBeforeTownCheckPos[2];
  CPlayer::__target m_TargetObject;
  CPlayer::__target m_GroupTargetObject[3];
  CMyTimer m_tmrGroupTargeting;
  unsigned __int16 m_wPointRate_PartySend[4];
  bool m_bMineMode;
  unsigned int m_dwMineNextTime;
  unsigned __int16 m_wBatterySerialTmp;
  unsigned __int8 m_bySelectOreIndex;
  unsigned __int8 m_byDelaySec;
  __int16 m_zMinePos[2];
  _ATTACK_DELAY_CHECKER m_AttDelayChker;
  float m_fUnitPv_AttFc;
  float m_fUnitPv_DefFc;
  float m_fUnitPv_RepPr;
  CPvpPointLimiter m_kPvpPointLimiter;
  int m_nChaosMode;
  unsigned int m_dwChaosModeTime10Per;
  unsigned int m_dwChaosModeEndTime;
  int m_nCashAmount;
  float m_fGroupMapPoint[3][2];
  unsigned __int8 m_byGroupMapPointMapCode[3];
  unsigned __int16 m_wGroupMapPointLayerIndex[3];
  unsigned int m_dwLastGroupMapPointTime[3];
  CPvpOrderView m_kPvpOrderView;
  unsigned __int8 m_byBattleMode;
  unsigned int m_dwBattleTime;
  CMyTimer m_tmrAuraSkill;
  CPvpCashPoint m_kPvpCashPoint;
  CCouponMgr m_kPcBangCoupon;
  CMyTimer m_tmrEffectStartTime;
  CMyTimer m_tmrEffectEndTime;
  unsigned __int8 m_byBattleTournamentGrade;
  _NameChangeBuddyInfo m_NameChangeBuddyInfo;
  unsigned int m_dwPcBangGiveItemListIndex;
  MiningTicket::_AuthKeyTicket m_dwRaceBuffClearKey;
  CMyTimer m_tmrPremiumPVPInform;
  char m_szItemHistoryFileName[64];
  char m_szLvHistoryFileName[64];
  unsigned int m_dwUMWHLastTime;
  _other_shape_all_zocl m_bufShapeAll;
  _other_shape_part_zocl m_bufSpapePart;
};

bool LoadMasteryLimFile(char *pszErrMsg);
bool DTradeEqualPerson(CPlayer *lp_pOne, CPlayer **lpp_pDst);
void wa_EnterWorld(_WA_AVATOR_CODE *pData, unsigned __int16 wZoneIndex);
void wa_PartyJoin(_CLID *pidBoss, _CLID *pidJoiner);
void wa_PartyForceLeave(_CLID *pidBoss, _CLID *pidLeaver);
void wa_PartyDisjoint(_CLID *pidBoss);
void wa_PartySuccession(_CLID *pidBoss, _CLID *pidSuccessor);
void wa_PartyLock(_CLID *pidBoss, bool bLock);
void wa_PartyLootShareSystem(_CLID *pidBoss, unsigned __int8 byLootShareMode);
void wa_PartySelfLeave(_CLID *pidLeaver);
void wa_ExitWorld(_CLID *pidWorld);
CPlayer *GetPtrPlayerFromSerial(CPlayer *pData, int nNum, unsigned int dwSerial);
CPlayer *GetPtrPlayerFromAccount(CPlayer *pData, int nNum, char *szAccount);
CPlayer *GetPtrPlayerFromAccountSerial(CPlayer *pData, int nNum, unsigned int dwSerial);

#pragma pack(pop)

