#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include <vector>



struct _RTL_CRITICAL_SECTION_DEBUG;
struct _PCBANG_PLAY_TIME;
struct _DB_QRY_SYN_DATA;
struct _base_fld;
struct _monster_sp_fld;
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
struct _effect_parameter::__param_data;
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

/* 1535 */
struct __cppobj __unaligned __declspec(align(4)) _STORAGE_LIST
{
  struct __cppobj __unaligned __declspec(align(1)) _storage_con
  {
    unsigned __int8 m_bLoad;
    unsigned __int8 m_byTableCode;
    unsigned __int8 m_byClientIndex;
    unsigned __int16 m_wItemIndex;
    unsigned __int64 m_dwDur;
    unsigned int m_dwLv;
    unsigned __int16 m_wSerial;
    bool m_bLock;
    unsigned int m_dwETSerialNumber;
    unsigned __int64 m_lnUID;
    unsigned __int8 m_byCsMethod;
    unsigned int m_dwT;
    unsigned int m_dwLendRegdTime;
  };

  struct __cppobj __unaligned __declspec(align(2)) _db_con : _storage_con
  {
    _STORAGE_LIST *m_pInList;
    unsigned __int8 m_byStorageIndex;
  };

  int m_nListNum;
  int m_nUsedNum;
  int m_nListCode;
  _db_con *m_pStorageList;
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
  };

  _LIST m_List[4];
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
  };

  struct __cppobj __unaligned __declspec(align(1)) _NPC_QUEST_HISTORY
  {
    char szQuestCode[8];
    int nEventNo;
    unsigned __int8 byLevel;
    unsigned int dwEventEndTime;
  };

  struct __cppobj __unaligned __declspec(align(1)) _START_NPC_QUEST_HISTORY
  {
    char szQuestCode[64];
    unsigned __int8 byLevel;
    _SYSTEMTIME tmStartTime;
    __int64 nEndTime;
  };

  _LIST m_List[30];
  _NPC_QUEST_HISTORY m_History[70];
  unsigned int dwListCnt;
  _START_NPC_QUEST_HISTORY *m_StartHistory;
};

/* 1566 */
struct __cppobj _SFCONT_DB_BASE
{
  struct _LIST
  {
    unsigned int dwKey;
  };

  _LIST m_List[2][8];
};

/* 1573 */
struct _COMBINEKEY
{
  unsigned __int8 byRewardIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;
};

/* 1575 */
struct __cppobj _ITEMCOMBINE_DB_BASE
{
  struct _LIST
  {
    _COMBINEKEY Key;
    unsigned int dwDur;
    unsigned int dwUpt;
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
};

/* 1197 */
struct __cppobj _INVENKEY
{
  unsigned __int8 bySlotIndex;
  unsigned __int8 byTableCode;
  unsigned __int16 wItemIndex;
};

/* 1215 */
struct __cppobj __declspec(align(8)) CPostData
{
  int m_nNumber;
  unsigned __int8 m_byState;
  unsigned __int8 m_byErrCode;
  unsigned __int8 m_bySendRace;
  unsigned __int8 m_bySenderDgr;
  unsigned int m_dwSenderSerial;
  char m_wszSendName[17];
  char m_wszRecvName[17];
  char m_wszTitle[21];
  char m_wszContent[201];
  _INVENKEY m_Key;
  unsigned __int64 m_dwDur;
  unsigned int m_dwUpt;
  unsigned __int64 m_lnUID;
  unsigned int m_dwGold;
  unsigned int m_dwPSSerial;
  bool m_bContentLoad;
  bool m_bUpdateIndex;
};

/* 1703 */
struct __cppobj __declspec(align(4)) CPostStorage
{
  CPostData m_PostData[50];
  int m_nSize;
  bool m_bUpdate;
};

/* 1704 */
struct __cppobj __declspec(align(8)) CPostReturnStorage
{
  CPostData m_PostData[10];
  int m_nSize;
};

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
};

/* 1730 */
struct __cppobj CPlayerDB
{
  unsigned __int8 m_byPvPGrade;
  __unaligned __declspec(align(1)) _character_db_load m_dbChar;
  __unaligned __declspec(align(1)) _bag_db_load m_dbInven;
  __unaligned __declspec(align(1)) _equip_db_load m_dbEquip;
  __unaligned __declspec(align(1)) _embellish_db_load m_dbEmbellish;
  __unaligned __declspec(align(1)) _force_db_load m_dbForce;
  __unaligned __declspec(align(1)) _animus_db_load m_dbAnimus;
  __unaligned __declspec(align(1)) _trunk_db_load m_dbTrunk;
  __unaligned __declspec(align(1)) _Exttrunk_db_load m_dbExtTrunk;
  _STORAGE_LIST *m_pStoragePtr[8];
  _UNIT_DB_BASE m_UnitDB;
  _QUEST_DB_BASE m_QuestDB;
  _SFCONT_DB_BASE m_SFContDB;
  _ITEMCOMBINE_DB_BASE m_ItemCombineDB;
  __declspec(align(8)) CPostStorage m_PostStorage;
  CPostReturnStorage m_ReturnPostStorage;
  bool m_bPersonalAmineInven;
  AutominePersonal *m_pAPM;
  _personal_amine_inven_db_load m_dbPersonalAmineInven;
  unsigned __int16 *m_wCuttingResBuffer;
  unsigned __int8 m_byNameLen;
  unsigned int m_dwAlterMastery[80];
  _class_fld *m_pClassData;
  _class_fld *m_pClassHistory[3];
  _class_fld **m_ppHistoryEffect[4];
  _quick_link m_QLink[50];
  CGuild *m_pGuild;
  _guild_member_info *m_pGuildMemPtr;
  unsigned __int8 m_byClassInGuild;
  CGuild *m_pApplyGuild;
  bool m_bGuildLock;
  bool m_bTrunkOpen;
  char m_wszTrunkPasswd[13];
  long double m_dTrunkDalant;
  long double m_dTrunkGold;
  unsigned __int8 m_byTrunkSlotNum;
  unsigned __int8 m_byTrunkHintIndex;
  char m_wszTrunkHintAnswer[17];
  unsigned __int8 m_byExtTrunkSlotNum;
  int m_nMakeTrapMaxNum;
  long double m_dPvpPointLeak;
  bool m_bLastAttBuff;
  unsigned __int16 m_wSerialCount;
  CPlayer *m_pThis;
  char m_aszName[17];
};

/* 1276 */
struct __unaligned __declspec(align(2)) _CLID
{
  unsigned __int16 wIndex;
  unsigned int dwSerial;
};

/* 1692 */
struct __cppobj CSyncCS
{
  bool m_bUse;
  _RTL_CRITICAL_SECTION m_cs;
  virtual ~CSyncCS() = default;
};

/* 1689 */
struct __cppobj __declspec(align(8)) CIndexList
{
  struct _index_node
  {
    bool m_bLoad;
    unsigned int m_dwIndex;
    unsigned int m_dwInfoDataSize;
    char *m_pInfo;
    _index_node *m_pPrev;
    _index_node *m_pNext;
  };

  _index_node m_Head;
  _index_node m_Tail;
  _index_node m_BufHead;
  _index_node m_BufTail;
  _index_node *m_pBufNode;
  CSyncCS m_csList;
  unsigned int m_dwCount;
  unsigned int m_dwBufCount;
  unsigned int m_dwMaxBufNum;
  virtual ~CIndexList() = default;
};


/* 1735 */
struct __cppobj __declspec(align(8)) CRealMoveRequestDelayChecker
{
  CIndexList m_kNodeInxOrderList;
  std::vector<unsigned long> m_vecDelayList;
  unsigned __int16 m_wTotalMissCount;
  bool m_bPrevRet;
  unsigned __int16 m_wContinueMissCount;
  unsigned __int16 m_wSingleMissCount;
  unsigned __int16 m_wContinueValiedCount;
  unsigned __int16 m_wTotalContinueValiedCount;
};

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
};

/* 1740 */
struct __declspec(align(2)) _skill_lv_up_data
{
  bool bUpdate;
  unsigned __int16 wIndex;
  unsigned __int8 byLv;
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
};

/* 1745 */
struct _TOWER_PARAM
{
  struct _list
  {
    _STORAGE_LIST::_db_con *m_pTowerItem;
    unsigned __int16 m_wItemSerial;
    CGuardTower *m_pTowerObj;
  };

  int m_nCount;
  _list m_List[6];
};

/* 1749 */
struct __cppobj __declspec(align(8)) _TRAP_PARAM
{
  struct __cppobj __declspec(align(8)) _param
  {
    CTrap *pItem;
    unsigned int dwSerial;
  };

  _param m_Item[20];
  int m_nCount;
};

/* 1131 */
struct __cppobj CNetCriticalSection
{
  _RTL_CRITICAL_SECTION m_cs;
};

/* 1279 */
struct __cppobj __declspec(align(8)) CNetIndexList
{
  struct _index_node
  {
    unsigned int m_dwIndex;
    _index_node *m_pPrev;
    _index_node *m_pNext;
  };

  _index_node m_Head;
  _index_node m_Tail;
  _index_node m_BufHead;
  _index_node m_BufTail;
  _index_node *m_pBufNode;
  CNetCriticalSection m_csList;
  unsigned int m_dwCount;
  unsigned int m_dwBufCount;
  unsigned int m_dwMaxBufNum;
};

/* 1751 */
struct __cppobj _BUDDY_LIST
{
  struct __list
  {
    unsigned int dwSerial;
    char wszName[17];
    CPlayer *pPtr;
  };

  __list m_List[50];
  CNetIndexList m_LastApply;
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

/* 1754 */
struct __cppobj __declspec(align(8)) _happen_event_cont
{
  _happen_event_node *m_pEvent;
  QUEST_HAPPEN m_QtHpType;
  int m_nIndexInType;
  int m_nRaceCode;
};

/* 1755 */
struct __cppobj __declspec(align(8)) CQuestMgr
{
  CPlayer *m_pMaster;
  _QUEST_DB_BASE *m_pQuestData;
  _happen_event_cont m_LastHappenEvent;
  _happen_event_cont *m_pTempHappenEvent;
  unsigned int m_dwOldTimeoutChecktime;
};

/* 1756 */
struct __cppobj ItemCombineMgr
{
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
  };

  _AuthKeyTicket m_dwTakeLastMentalTicket;
  _AuthKeyTicket m_dwTakeLastCriTicket;
};

/* 1762 */
struct __cppobj __declspec(align(4)) CSetItemEffect
{
  struct __declspec(align(4)) set_effect_info
  {
    bool m_bCheckSetEffect;
    unsigned int m_dwSetItem;
    unsigned __int8 m_bySetItemNum;
    unsigned __int8 m_bySetEffectNum;
  };

  set_effect_info m_setCount[6];
  set_effect_info m_resetInfo;
  unsigned __int8 m_byTotalSetCount;
};

/* 1763 */
struct __cppobj CEquipItemSFAgent
{
  CPlayer *m_pMaster;
  _sf_continous *m_pContSF[8];
};

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
struct _STORAGE_POS
{
  unsigned __int8 byStorageCode;
};

/* 1767 */
struct __cppobj _STORAGE_POS_INDIV : _STORAGE_POS
{
  __unaligned __declspec(align(1)) unsigned __int16 wItemSerial;
  unsigned __int8 byNum;
};

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
struct __cppobj CPotionParam
{
  _ContPotionData m_ContCommonPotionData[2];
  _ContPotionData m_StoneOfMovePotionData;
  unsigned int m_dwNextUseTime[38];
  CPlayer *m_pMaster;
};

/* 1784 */
struct __cppobj CExtPotionBuf
{
  bool m_bExtPotionBufUse;
  bool m_bDayChange;
  unsigned int m_dwEndPotionTime;
};

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
};

/* 1793 */
struct __cppobj CPvpPointLimiter
{
  _PVPPOINT_LIMIT_DB_BASE *m_pkInfo;
};

/* 1794 */
struct CPvpOrderView
{
  unsigned int m_dwLastAttackTime;
  unsigned int m_dwLastDamagedTime;
  int m_nKillCnt;
  int m_nDeahtCnt;
  long double m_dTodayPvpPoint;
  long double m_dOriginalPvpPoint;
  long double m_dPvpPoint;
  long double m_dPvpTempCash;
  long double m_dPvpCash;
  bool m_bAttack;
  bool m_bDamaged;
  _PVP_ORDER_VIEW_DB_BASE *m_pkInfo;
};

/* 1795 */
struct __cppobj __declspec(align(8)) CPvpCashPoint
{
  CIndexList m_KillerList;
  unsigned __int8 m_byContPvpHave;
  unsigned __int8 m_byContPvpLose;
  bool m_bRaceWarRecvr;
};

/* 1796 */
struct CouponInfo
{
  unsigned __int8 byTableCode;
  unsigned int dwIndex;
};

/* 1797 */
struct __cppobj CCouponMgr
{
  CouponInfo m_Coupon[5];
  unsigned __int8 m_byRemainTime;
  unsigned __int8 m_byReceiveCoupon;
  unsigned int m_dwContTime;
  unsigned __int8 m_byInitTime;
  bool m_bTimeReset;
  CMyTimer m_tmrCheckConnMin;
  CMyTimer m_tmrCouponEnableTime;
  _PCBANG_PLAY_TIME *m_pkInfo;
};

/* 1798 */
struct __cppobj _NameChangeBuddyInfo
{
  bool bNameChange;
  int nSendNum;
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
struct __cppobj __declspec(align(8)) CPlayer : CCharacter
{
  union CashChangeStateFlag
  {
    $621D0DDFB6A4DE55506A65C7CCDC95CE __s0;
    unsigned __int8 m_byStateFlag;
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
  };

  virtual ~CPlayer();

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
