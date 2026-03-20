#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "ObjectCreateSetData.h"

struct _monster_fld;
struct _dummy_position;
class CMapData;

struct  _stone_create_setdata : _character_create_setdata
{
  _dummy_position *pDumPosition;
  unsigned char byMasterRace;

  _stone_create_setdata()
  {
    pDumPosition = nullptr;
    byMasterRace = 0;
  }
};

class  CHolyStone : public CCharacter
{
public:
  CHolyStone();
  ~CHolyStone();

  int GetAttackDP() override;
  int GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  int GetDefSkill(bool bBackAttackDamage) override;
  int GetFireTol() override;
  int GetLevel() override;
  int GetMaxHP() override;
  char *GetObjName() override;
  int GetObjRace() override;
  int GetSoilTol() override;
  int GetWaterTol() override;
  float GetWeaponAdjust() override;
  float GetWidth() override;
  int GetWindTol() override;
  bool IsBeAttackedAble(bool bFirst) override;
  bool IsBeDamagedAble(CCharacter *pAtter) override;
  void Loop() override;
  void AutoRecover();
  unsigned __int16 GetAddCountWithPlayer();
  void SetDropItem();
  void DropItem();
  bool Destroy(unsigned __int8 byDestroyCode, CCharacter *pAtter);
  void OutOfSec() override;
  void SendMsg_FixPosition(int n) override;

  bool Create(_stone_create_setdata *pData);
  static unsigned int GetNewStoneSerial();
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDestroyCode, unsigned int dwDestroySerial);
  void SetOper(bool bOper, float fHPRate);
  void SendMsg_StoneAlterOper();
  unsigned __int16 CalcCurHPRate() override;
  char IsChangedHP(unsigned __int16 wAlterRate);
  int GetHP();
  bool SetHP(int nHP, bool bOver = false);
  int SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  int SetDamage(int nDam, CCharacter *pDst, int nDstLv);

  static int s_nLiveNum;
  static unsigned int s_dwSerialCnt;

  bool m_bOper;
  int m_nHP;
  int m_nMaxHP;
  unsigned int m_dwLastDestroyTime;
  _monster_fld *m_pRec;
  _dummy_position *m_pDum;
  unsigned __int8 m_byMasterRaceCode;
  int m_nDefPart[5];
  int m_nOldRate;
  unsigned int m_dwLastRecoverTime;
  int m_nCurrLootIndex;
  int m_nEndLootIndex;
  int m_nCurrDropIndex;
  unsigned __int16 m_wMagnifications;
  unsigned __int16 m_wRange;
  unsigned __int16 m_wDropCntOnce;
  unsigned __int16 m_wAddCountWithPlayer;
  CMyTimer m_tmrDropTime;

  bool Init(_object_id *pID);
};

