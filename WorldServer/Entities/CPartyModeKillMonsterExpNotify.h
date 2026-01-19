#pragma once

#include "IdaCompat.h"

class CPlayer;

class __cppobj CPartyModeKillMonsterExpNotify
{
public:
  struct __cppobj __declspec(align(8)) CExpInfo
  {
    float m_fExp;
    CPlayer *m_pkMember;
  };

  bool m_bKillMonster;
  unsigned char m_byMemberCnt;
  CExpInfo m_kInfo[8];
};
