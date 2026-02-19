#pragma once

#include "IdaCompat.h"

class CPlayer;

class  CPartyModeKillMonsterExpNotify
{
public:
  struct  CExpInfo
  {
    float m_fExp;
    CPlayer *m_pkMember;

    CExpInfo();
    ~CExpInfo();
    void SetGetExp(CPlayer *pkMember, float fExp);
    void Notify();
  };

  bool m_bKillMonster;
  unsigned char m_byMemberCnt;
  CExpInfo m_kInfo[8];

  CPartyModeKillMonsterExpNotify();
  ~CPartyModeKillMonsterExpNotify();
  char Add(CPlayer *pkMember, float fExp);
  void Notify();
  void SetKillMonsterFlag();
};

