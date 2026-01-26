#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _UnitFrame_fld : _base_fld
{
  int m_nFRAType;
  int m_nUnit_HP;
  float m_fDefGap;
  float m_fDefFacing;
  float m_fMoveRate_Seed;
  float m_fHeight;
  float m_fWidth;
  int m_nMoney;
  int m_nStdPrice;
  int m_nStdPoint;
  int m_nGoldPoint;
  int m_nKillPoint;
  int m_nProcPoint;
  int m_nStoragePrice;
  int m_nRepPrice;
  int m_bDestroy;
  int m_bRepair;
  char m_strDFHead[64];
  char m_strDFUpper[64];
  char m_strDFLower[64];
  char m_strDFArms[64];
  char m_strDFShoulder[64];
  char m_strDFBack[64];
  char m_strDFBtA[64];
  char m_strDFBtS[64];
};
#pragma pack(pop)
