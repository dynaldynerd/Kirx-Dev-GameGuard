#pragma once

#include "IdaCompat.h"

/* 1730 */
class __cppobj CPlayerDB
{
public:
  static int GetRaceCode(CPlayerDB *self);
  static int GetRaceSexCode(CPlayerDB *self);
  static int GetMapCode(CPlayerDB *self);
  static void SetMapCode(CPlayerDB *self, unsigned __int8 byCode);
  static char *GetCharNameA(CPlayerDB *self);
  static const char *GetCharNameW(CPlayerDB *self);
  static unsigned int GetCharSerial(CPlayerDB *self);
  static long double GetPvPPoint(CPlayerDB *self);
  static void SetPvPPoint(CPlayerDB *self, long double dPoint);
  static unsigned int GetGuildSerial(CPlayerDB *self);
  static unsigned int GetLevel(CPlayerDB *self);
  static unsigned int GetHP(CPlayerDB *self);
  static void SetHP(CPlayerDB *self, unsigned int dwHP);
  static unsigned int GetFP(CPlayerDB *self);
  static void SetFP(CPlayerDB *self, unsigned int dwFP);
  static unsigned int GetSP(CPlayerDB *self);
  static void SetSP(CPlayerDB *self, unsigned int dwSP);
  static unsigned int GetDP(CPlayerDB *self);
  static void SetDP(CPlayerDB *self, unsigned int dwDP);
  static unsigned int GetDalant(CPlayerDB *self);
  static void SetDalant(CPlayerDB *self, unsigned int dwDt);
  static unsigned int GetGold(CPlayerDB *self);
  static void SetCurPos(CPlayerDB *self, float *fPos);
  static unsigned __int8 GetBagNum(CPlayerDB *self);

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

