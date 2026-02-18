#pragma once

#include "IdaCompat.h"
#include "CRecordData.h"
#include "PotionItem_fld.h"

class CRFWorldDatabase;
class CCharacter;
class CMapData;
class CPlayer;
struct _CheckPotion_fld;
struct _skill_fld;
struct _ContPotionData;

class  CPotionMgr
{
public:
  CPotionMgr();
  ~CPotionMgr();

  bool DatafileInit();
  bool SetPotionDataName();
  unsigned __int8 UsePotion(
    CPlayer *pUsePlayer,
    CCharacter *pTargetCharacter,
    _PotionItem_fld *pfB,
    unsigned int nCurTime);
  unsigned int PreCheckPotion(
    CPlayer *pUsePlayer,
    CCharacter **pTargetCharacter,
    const _PotionItem_fld *pfB,
    unsigned int nCurTime,
    _skill_fld *pFld,
    bool bCheckDist);
  unsigned int ApplyPotion(
    CPlayer *pUsePlayer,
    CPlayer *pApplyPlayer,
    _skill_fld *pEffecFld,
    _CheckPotion_fld *pCheckFld,
    const _PotionItem_fld *pfB,
    bool bCommonPotion);
  unsigned int RemovePotionContEffect(CPlayer *pApplyPlayer, _ContPotionData *contPotionData);
  unsigned int InsertPotionContEffect(
    CPlayer *pApplyPlayer,
    _ContPotionData *contPotionData,
    _skill_fld *pEffecFld,
    unsigned int dwDurTime);
  void UpdatePotionContEffect(CPlayer *pPlayer);
  unsigned int SelectDeleteBuf(CPlayer *pOne, bool bUse, bool bRemove);
  char CheckPotionUsableMap(const _PotionItem_fld *pPotionFld, CMapData *pMap);
  void InsertMovePotionStoneEffect(CPlayer *pApplyPlayer);
  bool InsertRenamePotion(CRFWorldDatabase *pkWorldDB, char *pData);
  bool IsPotionDelayUseIndex(unsigned int nIndex);
  void Complete_RenameChar_DB_Select(unsigned __int8 byRet, char *p);
  void Complete_RenameChar_DB_Update(unsigned __int8 byRet, char *p);
  void PushRenamePotionDBLog(char *pInfo);

  struct  PotionInnerData
  {
    PotionInnerData();
    ~PotionInnerData();
    void Init();

    bool m_bPotionDelayIndexList[38];
  };

  struct __declspec(align(8))  _ParamData
  {
    float fValue;
    _PotionItem_fld *pPotionFld;
    unsigned int dwCurTime;
    unsigned int dwEndTime;
    unsigned int dwNextTime;
  };

  PotionInnerData m_PotionInnerData;
  CRecordData m_tblPotionEffectData;
  CRecordData m_tblPotionCheckData;
};

extern CPotionMgr g_PotionMgr;

bool __fastcall DE_Potion_CharReName(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Class_Refine(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Race_Debuff_Clear_One(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
bool __fastcall DE_Potion_Race_Debuff_Clear_Two(
  CCharacter *pActChar,
  CCharacter *pTargetChar,
  float fEffectValue,
  unsigned __int8 *byRet);
