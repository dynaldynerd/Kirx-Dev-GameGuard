#pragma once

#include "IdaCompat.h"
#include "base_fld.h"

#pragma pack(push, 1)
struct __cppobj _SetItemEff_fld : _base_fld
{
  char m_strCivil[64];
  char m_strset_head[64];
  char m_strset_upper[64];
  char m_strset_lower[64];
  char m_strset_shoes[64];
  char m_strset_gauntlet[64];
  char m_strset_weapon[64];
  char m_strset_shield[64];
  char m_strset_amulet1[64];
  char m_strset_amulet2[64];
  char m_strset_ring1[64];
  char m_strset_ring2[64];
  char m_strset_cloak[64];
  char m_strEffrule[64];
  int m_nEff1Code;
  float m_fEff1Unit;
  int m_nEff2Code;
  float m_fEff2Unit;
  int m_nEff3Code;
  float m_fEff3Unit;
  int m_nEff4Code;
  float m_fEff4Unit;
  int m_nEff5Code;
  float m_fEff5Unit;
  int m_nEff6Code;
  float m_fEff6Unit;
  int m_nEff7Code;
  float m_fEff7Unit;
  int m_nEff8Code;
  float m_fEff8Unit;
};
#pragma pack(pop)
