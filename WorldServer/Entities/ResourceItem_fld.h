#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct EffectData
{
  int nEffCode;
  float fEffUnit;
  float fEffUnitMax;
};

struct  _ResourceItem_fld : _base_fld
{
  int m_bExist;
  char m_strModel[64];
  int m_nIconIDX;
  char m_strName[64];
  int m_nKindClt;
  int m_nFixPart;
  int m_nItemSerise;
  char m_strCivil[64];
  int m_nMixNum;
  float m_fNeedNum;
  char m_strLastItem[64];
  int m_nMoney;
  int m_nAncStdPrice;
  int m_nExStdPrice;
  int m_nMecaStdPrice;
  int m_nStdPoint;
  int m_nGoldPoint;
  int m_nKillPoint;
  int m_nProcPoint;
  int m_nStoragePrice;
  int m_nStartTime;
  int m_nEffContTime;
  int m_nEffType1;
  int m_nUIPrint;
  int m_nItemUiTip;
  int m_nEffectDataNum;
  EffectData m_EffectData[5];
  int m_bSell;
  int m_bExchange;
  int m_bGround;
  int m_bStoragePossible;
  int m_bUseableNormalAcc;
  char m_strTooltipIndex[64];
  int m_bIsCash;
  int m_nUsePCCash;
  int m_bIsTime;
};
#pragma pack(pop)
