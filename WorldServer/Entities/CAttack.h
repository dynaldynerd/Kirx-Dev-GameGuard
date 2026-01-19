#pragma once

#include "IdaCompat.h"

class CCharacter;
struct _attack_param;

/* 2358 */
struct __cppobj __declspec(align(8)) _be_damaged_char
{
  CCharacter *m_pChar;
  int m_nDamage;
  bool m_bActiveSucc;
  int m_nActiveDamage;
};

/* 2359 */
class __cppobj __declspec(align(8)) CAttack
{
  _attack_param *m_pp;
  CCharacter *m_pAttChar;
  bool m_bIsCrtAtt;
  bool m_bActiveSucc;
  int m_nDamagedObjNum;
  _be_damaged_char m_DamList[30];
  bool m_bFailure;
};
