#pragma once

#include "IdaCompat.h"

#include "CCharacter.h"
#include "nuclear_create_setdata.h"

class CPlayer;

struct __cppobj _be_damaged_player
{
  CCharacter *m_pChar;
  unsigned int m_dwDamCharSerial;
  int m_nDamage;
};

#ifndef BE_DAMAGED_CHAR_DEFINED
#define BE_DAMAGED_CHAR_DEFINED
struct __cppobj __declspec(align(8)) _be_damaged_char
{
  CCharacter *m_pChar;
  int m_nDamage;
  bool m_bActiveSucc;
  int m_nActiveDamage;
};
#endif

class __cppobj CNuclearBomb : public CCharacter
{
public:
  __int64 GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack) override;
  void Loop() override;
  bool Init(_object_id *pID);
  bool Create(_nuclear_create_setdata *pData);
  bool GetUse();
  unsigned __int8 GetBombStatus();
  __int64 GetDamagedObjNum();
  void SetBombStatus();
  void Attack(int startNum, int objCount);
  float *GetMissilePos();
  unsigned __int16 GetItemIndex();
  void SetNuclearIndex(unsigned __int16 wItemIndex);
  unsigned __int16 GetControlSerial();
  void SetControlSerial(unsigned __int16 wControlSerial);
  unsigned __int8 GetMasterClass();
  void SendMsg_NuclearFind(unsigned int n, unsigned __int8 race);

  static __int64 GetNewSerial();

  static unsigned int s_dwSerialCnt;

  unsigned __int16 m_wItemIndex;
  unsigned __int16 m_wControlSerial;
  bool m_bUse;
  bool m_bIsLive;
  float m_fDropPos[3];
  unsigned int m_dwStartTime;
  unsigned int m_dwDurTime;
  unsigned int m_dwDelayTime;
  unsigned int m_dwWarnTime;
  unsigned int m_dwAttInformTime;
  unsigned int m_dwAttStartTime;
  unsigned __int8 m_byBombState;
  _be_damaged_player m_DamList[300];
  _be_damaged_char m_EffList[400];
  int m_nDamagedObjNum;
  int m_nEffObjNum;
  int m_nStartDmLoop;
  CPlayer *m_pMaster;
};
