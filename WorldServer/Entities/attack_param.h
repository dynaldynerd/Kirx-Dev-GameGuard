#pragma once

#include "IdaCompat.h"

class CCharacter;
struct _base_fld;

#pragma pack(push, 1)
struct _attack_param
{
  CCharacter *pDst;
  int nPart;
  int nTol;
  int nClass;
  int nMinAF;
  int nMaxAF;
  int nMinSel;
  int nMaxSel;
  int nExtentRange;
  int nShotNum;
  int nAddAttPnt;
  int nWpType;
  unsigned __int8 byEffectCode;
  _base_fld *pFld;
  float fArea[3];
  int nLevel;
  int nMastery;
  bool bPassCount;
  int nAttactType;
  bool bMatchless;
  int nMaxAttackPnt;
  bool bBackAttack;
  int nMinAFPlus;
  int nMaxAFPlus;
  int nEffShotNum;

  _attack_param();
};
#pragma pack(pop)

