#pragma once

#include "IdaCompat.h"

/* 1730 */
class CPlayer;
struct _class_fld;
struct _AVATOR_DATA;

class __cppobj CPlayerDB
{
public:
  int GetRaceCode();
  int GetRaceSexCode();
  int GetMapCode();
  void SetMapCode(unsigned __int8 byCode);
  char *GetCharNameA();
  char *GetCharNameW();
  unsigned int GetCharSerial();
  long double GetExp();
  long double GetLossExp();
  void SetExp(long double dExp);
  void SetLossExp(long double dLossExp);
  float *GetCurPos();
  unsigned int GetPvpRank();
  long double GetPvPPoint();
  void SetPvPPoint(long double dPoint);
  unsigned int GetGuildSerial();
  unsigned __int8 GetClassInGuild();
  void SetClassInGuild(unsigned __int8 byClassInGuild);
  unsigned int GetLevel();
  void SetLevel(unsigned __int8 nLv);
  unsigned int GetMaxLevel();
  bool IsClassChangeableLv();
  unsigned __int8 GetTrunkSlotNum();
  unsigned __int8 GetExtTrunkSlotNum();
  _class_fld *GetPtrCurClass();
  _class_fld *GetPtrBaseClass();
  unsigned int GetHP();
  void SetHP(unsigned int dwHP);
  unsigned int GetFP();
  void SetFP(unsigned int dwFP);
  unsigned int GetSP();
  void SetSP(unsigned int dwSP);
  unsigned int GetDP();
  void SetDP(unsigned int dwDP);
  unsigned int GetDalant();
  void SetDalant(unsigned int dwDt);
  unsigned int GetGold();
  void SetGold(unsigned int dwGold);
  void SetCurPos(float *fPos);
  unsigned __int8 GetBagNum();
  void SetBagNum(unsigned __int8 byNum);
  int GetUseSlot();
  unsigned __int16 GetNewItemSerial();
  static char CalcCharGrade(unsigned __int8 byLv, unsigned __int16 wRankRate);
  bool BeHaveBoxOfAMP();
  void SetHaveBoxOfAMP(bool bFlag);
  void InitPlayerDB(CPlayer *pThis);
  void InitResBuffer();
  void InitAlterMastery();
  void InitClass();
  void SelectClass(unsigned __int8 byHistoryRecordNum, _class_fld *pSelectClass);
  bool IsActableClassSkill(char *pszSkillCode, int *pnClassGrade);
  void SetMaxLevel(unsigned __int8 nLv);
  char ConvertAvatorDB(_AVATOR_DATA *pData);
  char ConvertGeneralDB(_AVATOR_DATA *pData, _AVATOR_DATA *pOutData);
  void AppointSerialStorageItem();
  char PushLink(int nLinkIndex, unsigned __int16 wSerial, bool bInit);
  _STORAGE_LIST::_db_con *GetPtrItemStorage(unsigned __int16 wSerial, unsigned __int8 *pbyStorageCode);
  void PopLink(int nLinkIndex);

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

