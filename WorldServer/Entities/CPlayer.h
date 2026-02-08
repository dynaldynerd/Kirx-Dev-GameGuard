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
#include <vector>



struct _RTL_CRITICAL_SECTION_DEBUG;
struct _PCBANG_PLAY_TIME;
struct _DB_QRY_SYN_DATA;
struct _base_fld;
struct _monster_sp_fld;
struct _skill_fld;
struct _ResourceItem_fld;
struct _ITEM_EFFECT;
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
struct _effect_parameter::__param_data;
class CUserDB;
class CRecordData;
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
};

/* 1697 */
struct __cppobj _equip_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[8];
};

/* 1698 */
struct __cppobj __unaligned __declspec(align(2)) _embellish_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[7];
};

/* 1699 */
struct __cppobj _force_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[88];
};

/* 1700 */
struct __cppobj _animus_db_load : _STORAGE_LIST
{
  _STORAGE_LIST::_db_con m_List[4];
};

/* 1701 */
struct __cppobj _trunk_db_load : _STORAGE_LIST
{
  unsigned __int8 m_byItemSlotRace[100];
  _STORAGE_LIST::_db_con m_List[100];
};

/* 1702 */
struct __cppobj _Exttrunk_db_load : _STORAGE_LIST
{
  unsigned __int8 m_byItemSlotRace[40];
  _STORAGE_LIST::_db_con m_ExtList[40];
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
  unsigned __int8 GetMasteryPerMast(unsigned __int8 byCode, unsigned __int8 byMast);
  unsigned int GetCumPerMast(unsigned __int8 byCode, unsigned __int8 byMast);
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
};

/* 1749 */
struct __cppobj __declspec(align(8)) _TRAP_PARAM
{
  struct __cppobj __declspec(align(8)) _param
  {
    CTrap *pItem;
    unsigned int dwSerial;

    bool isLoad();
  };

  _param m_Item[20];
  int m_nCount;
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

/* 1754 */
struct __cppobj __declspec(align(8)) _happen_event_cont
{
  _happen_event_node *m_pEvent;
  QUEST_HAPPEN m_QtHpType;
  int m_nIndexInType;
  int m_nRaceCode;
};

/* 1755 */
#include "CQuestMgr.h"

/* 1756 */
struct __cppobj ItemCombineMgr
{
  static bool LoadData();
  static bool CheckLoadData();
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
  };

  _LIST m_List[10];
};

/* 1766 */
#include "StoragePos.h"

/* 1768 */
struct __cppobj _RENAME_POTION_USE_INFO
{
  _STORAGE_POS_INDIV ItemInfo;
  char wszChangeName[17];
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
  };

  struct _mas_list
  {
    unsigned __int8 byEffectCode;
    unsigned __int8 byMastery;
    unsigned int dwNextTime;
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
};

/* 1310 */
class __cppobj __declspec(align(8)) CPlayer : public CCharacter
{
  union CashChangeStateFlag
  {
    $621D0DDFB6A4DE55506A65C7CCDC95CE __s0;
    unsigned __int8 m_byStateFlag;

    CashChangeStateFlag(char cashrename = 0);
  };

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
  static unsigned int s_dwTotalCloseCount;
  static unsigned int s_dwAbnormalCloseCount;
  static void SetStaticMember();
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
  bool OutOfMap(CMapData *pIntoMap, unsigned __int16 wLayerIndex, unsigned __int8 byMapOutType, float *pfStartPos);
  void SendMsg_GotoRecallResult(
    char byErrCode,
    char byMapCode,
    float *pfStartPos,
    unsigned __int8 byMapInType);
  void SendMsg_GotoBasePortalResult(char byErrCode);
  void SendMsg_MapEnvInform(char byMapCode, unsigned int dwMapEnvCode);
  void SendMsg_MapOut(unsigned __int8 byMapOutCode, unsigned __int8 byNextMapCode);
  void SendMsg_MineCancle();
  void SendMsg_DeleteStorageInform(char byStorageCode, unsigned __int16 wSerial);
  void SendMsg_BuddhaEventMsg(char byErrorCode);
  void SendMsg_AlterItemDurInform(char byStorageCode, unsigned __int16 wItemSerial, unsigned __int64 dwDur);
  void SendMsg_EquipPartChange(unsigned __int8 byPart);
  unsigned __int16 GetVisualVer();
  void UpdateVisualVer(CashChangeStateFlag byChangeFlagMask);
  unsigned __int8 GetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex);
  void SetEffectEquipCode(unsigned __int8 byStorageCode, unsigned __int8 bySlotIndex, unsigned __int8 byCode);
  void SetEquipEffect(_STORAGE_LIST::_db_con *pItem, bool bEquip);
  char ApplyEquipItemEffect(int iItemEffectCode, bool bEquip);
  char IsEffectableEquip(_STORAGE_LIST::_storage_con *pCon);
  unsigned int _check_equipmastery_lim(int EquipMasteryCode);
  __int64 _check_mastery_lim(unsigned __int8 byMasteryClass, unsigned __int8 byIndex);
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
    void SendMsg_AlterEquipSPInform();
    void CalcEquipMaxDP(int bSendMsg);
    bool IsSiegeMode();
    bool IsChaosMode();
    void SetSiege(_STORAGE_LIST::_db_con *pItem);
  void SetHaveEffect(char bSet);
  void SetMstHaveEffect(_ResourceItem_fld *pFld, _STORAGE_LIST::_db_con *pItem, int a3, int a4);
  void SetHaveEffectUseTime(_STORAGE_LIST::_db_con *pItem, bool bAdd);
  bool IsUsableAccountType(int nCashType);
  bool IsApplyPcbangPrimium();
  void SendMsg_PremiumCashItemUse(unsigned __int16 wSerial);
  int GetCashAmount();
  void SetCashAmount(int nAmount);
  void DeleteCouponItem(_STORAGE_POS_INDIV *CouponItem, int n);
  void DTradeInit();
  void SendMsg_DTradeCancleInform();
  void SendMsg_DTradeCloseInform(char byCloseCode);
  void NetClose(bool bMoveOutLobby);
  void _TowerAllReturn(unsigned __int8 byDestroyType, bool bForceReturn);
  void _TowerDestroy(CGuardTower *pTowerObj);
  void ExitUpdateDataToWorld();
  void _AnimusReturn(unsigned __int8 byReturnType);
  void SendMsg_AnimusReturnResult(char byRetCode, unsigned __int16 wAnimusItemSerial, unsigned __int8 byReturnType);
  CAnimus *GetRecallAnimus();
  bool IsHaveMentalTicket();
  bool IsUseReleaseRaceBuffPotion();
  void SetUseReleaseRaceBuffPotion();
  char mgr_tracing(int bOper);
  void SendMsg_AlterTowerHP(unsigned __int16 wItemSerial, unsigned __int16 wLeftHP);
  void SendMsg_Destroy();
  void SendMsg_NewViewOther(unsigned __int8 byViewType);
  void SetLastAttBuff(bool bSet);
  bool IsLastAttBuff();
  void SetShapeAllBuffer();
  __int64 GetLevel();
  __int64 GetHP();
  __int64 GetMaxHP();
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
  char SetDP(int nDP, bool bOver);
  void SendMsg_SetDPInform();
  void SendMsg_AlterMaxDP();
  void SendMsg_TransformSiegeModeResult(char byRetCode);
  void SendMsg_ReleaseSiegeModeResult(char byRetCode);
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
  void SendMsg_RecvHSKQuest();
  void AlterPvPPoint(double dAlter, int AlterType, unsigned int dwDstSerial);
  void SetGrade(unsigned __int8 byGrade);
  void SetRankRate(unsigned __int16 wRankRate, unsigned int dwRank);
  void SendMsg_AlterGradeInform();
  void SendMsg_AlterPvPRank(unsigned __int16 wPvpRate, unsigned int dwPvpRank);
  void SendMsg_PvpRankListVersionUp(char byVersion);
  void SendMsg_AlterPvPPoint();
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
    void SendMsg_AlterContEffectTime(unsigned __int8 byContType);
    void SendMsg_TLStatusPenalty(char byErrCode);
    void SendMsg_MonsterAggroData(CCharacter *pCharacter);
    int Emb_UpdateStat(unsigned int dwStatIndex, unsigned int dwNewData, unsigned int dwOldData);
    void SendMsg_StatInform(unsigned __int8 byStatIndex, unsigned int dwNewStat, unsigned __int8 byReason);
    CGameObject *GetTargetObj();
    __target *GetGroupTarget(unsigned __int8 byGroupType);
  void SendMsg_ReleaseGroupTargetObjectResult(unsigned __int8 byGroupType);
  void SendMsg_BuddyLogoffInform(unsigned int dwObjSerial);
  void SendMsg_BuddyLoginInform(unsigned int dwObjSerial, unsigned __int8 byMapIndex, unsigned __int8 byPosCode);
  void SendMsg_GuildMasterEffect(
    char byState,
    char byGrade,
    char byEffSubAttack,
    unsigned __int8 byEffSubDefence,
    unsigned __int8 byEffAddAttack,
    unsigned __int8 byEffAddDefence);
  void pc_PartyLeaveSelfReqeuest();
  void SendMsg_PartyLeaveSelfResult(CPartyPlayer *pLeaver, bool bWorldExit);
  void SendMsg_PartySuccessResult(CPartyPlayer *pSuccessor);
  void ForcePullUnit(bool bLogout);
  void _UpdateUnitDebt(unsigned __int8 bySlotIndex, unsigned int dwPull);
  bool _LockUnitKey(unsigned __int8 bySlotIndex, bool bLock);
  void SendMsg_UnitForceReturnInform(char bySlotIndex, unsigned int dwDebt);
  void SendMsg_UnitAlterFeeInform(char bySlotIndex, unsigned int dwPullingFee);
  void SubDalant(unsigned int dwSub);
  void SendMsg_RemainOreRate();
  void SendMsg_OreTransferCount();
  void SendMsg_LendItemTimeExpired(char byStorageCode, unsigned __int16 wSerial);

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
void wa_PartySelfLeave(_CLID *pidLeaver);
void wa_ExitWorld(_CLID *pidWorld);
CPlayer *GetPtrPlayerFromSerial(CPlayer *pData, int nNum, unsigned int dwSerial);

#pragma pack(pop)

