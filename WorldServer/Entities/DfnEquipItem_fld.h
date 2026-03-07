#pragma once

#include "IdaCompat.h"
#include "base_fld.h"
#include "EQUIP_MASTERY_LIM.h"
#include "ITEM_EFFECT.h"

#pragma pack(push, 1)

struct  _DfnEquipItem_fld : _base_fld
{
  int m_bExist;
  char m_strModel[64];
  int m_nIconIDX;
  char m_strName[64];
  int m_nKindClt;
  int m_nItemGrade;
  int m_nFixPart;
  int m_nHelmetClass;
  char m_strCivil[64];
  int m_nLevelLim;
  int m_nUpLevelLim;
  int m_nClassGradeLim;
  union
  {
    struct
    {
      int m_nExpertID1;
      int m_nExpertLim1;
      int m_nExpertID2;
      int m_nExpertLim2;
    };
    _EQUIP_MASTERY_LIM m_MasteryLimit[2];
  };
  int m_nMoney;
  int m_nStdPrice;
  int m_nStdPoint;
  int m_nGoldPoint;
  int m_nKillPoint;
  int m_nProcPoint;
  int m_nStoragePrice;
  int m_bAbr;
  int m_nDurUnit;
  float m_fEquipSpeed;
  int m_bRepair;
  int m_nRepPrice;
  int m_nEffState;
  int m_nGASpd;
  int m_nProperty;
  float m_fFireTol;
  float m_fWaterTol;
  float m_fSoilTol;
  float m_fWindTol;
  float m_fDefFc;
  int m_nDefence_DP;
  int m_nMaxDP;
  float m_fDefGap;
  float m_fDefFacing;
  union
  {
    struct
    {
      int m_nEff1Code;
      float m_fEff1Unit;
      int m_nEff2Code;
      float m_fEff2Unit;
      int m_nEff3Code;
      float m_fEff3Unit;
      int m_nEff4Code;
      float m_fEff4Unit;
    };
    _ITEM_EFFECT m_Effect[4];
  };
  int m_nDuration;
  int m_bSell;
  int m_bExchange;
  int m_bGround;
  int m_bStoragePossible;
  int m_bUseableNormalAcc;
  int m_nUpgrade;
  char m_strTooltipIndex[64];
  int m_nDefEffType;
  int m_bIsTime;
};

#pragma pack(pop)
