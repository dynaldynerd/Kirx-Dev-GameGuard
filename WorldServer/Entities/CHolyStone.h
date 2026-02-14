#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "ObjectCreateSetData.h"

struct _monster_fld;
struct _dummy_position;
class CMapData;

struct __cppobj __declspec(align(8)) _stone_create_setdata : _character_create_setdata
{
  _dummy_position *pDumPosition;
  unsigned char byMasterRace;

  _stone_create_setdata()
  {
    pDumPosition = nullptr;
    byMasterRace = 0;
  }
};

class __cppobj CHolyStone : public CCharacter
{
public:
  CHolyStone();
  ~CHolyStone();

  __int64 GetAttackDP() override;
  __int64 GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart) override;
  float GetDefFacing(int nPart) override;
  float GetDefGap(int nPart) override;
  __int64 GetDefSkill(bool bBackAttackDamage) override;
  __int64 GetFireTol() override;
  __int64 GetLevel() override;
  __int64 GetMaxHP() override;
  char *GetObjName() override;
  __int64 GetObjRace() override;
  __int64 GetSoilTol() override;
  __int64 GetWaterTol() override;
  float GetWeaponAdjust() override;
  float GetWidth() override;
  __int64 GetWindTol() override;
  bool IsBeAttackedAble(bool bFirst) override;
  char IsBeDamagedAble(CCharacter *pAtter) override;
  void Loop() override;
  void OutOfSec() override;
  void SendMsg_FixPosition(int n) override;

  bool Create(_stone_create_setdata *pData);
  static unsigned int GetNewStoneSerial();
  void SendMsg_Create();
  void SendMsg_Destroy(unsigned __int8 byDestroyCode, unsigned int dwDestroySerial);
  void SetOper(bool bOper, float fHPRate);
  void SendMsg_StoneAlterOper();
  unsigned int CalcCurHPRate();
  char IsChangedHP(unsigned __int16 wAlterRate);
  __int64 GetHP();
  char SetHP(int nHP, bool bOver = false);
  __int64 SetDamage(
    int nDam,
    CCharacter *pDst,
    int nDstLv,
    bool bCrt,
    int nAttackType,
    unsigned int dwAttackSerial,
    bool bJadeReturn) override;
  __int64 SetDamage(int nDam, CCharacter *pDst, int nDstLv);

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
