#include "pch.h"

#include "CPartyModeKillMonsterExpNotify.h"

#include "CNetProcess.h"
#include "CPartyPlayer.h"
#include "GlobalObjects.h"

CPartyModeKillMonsterExpNotify::CExpInfo::CExpInfo()
{
  m_fExp = 0.0f;
  m_pkMember = nullptr;
}

CPartyModeKillMonsterExpNotify::CExpInfo::~CExpInfo() = default;

void CPartyModeKillMonsterExpNotify::CExpInfo::SetGetExp(CPlayer *pkMember, float fExp)
{
  m_pkMember = pkMember;
  m_fExp = fExp;
}

void CPartyModeKillMonsterExpNotify::CExpInfo::Notify()
{
  if (m_pkMember && m_pkMember->m_bOper && m_fExp != 0.0f)
  {
    char msg[4]{};
    *reinterpret_cast<float *>(msg) = m_fExp;

    unsigned __int8 type[2]{11, 30};
    CNetProcess::LoadSendMsg(g_Network.m_pProcess[0], m_pkMember->m_ObjID.m_wIndex, type, msg, 4u);
  }
}

CPartyModeKillMonsterExpNotify::CPartyModeKillMonsterExpNotify()
  : m_bKillMonster(false),
    m_byMemberCnt(0)
{
  for (auto &info : m_kInfo)
  {
    info = CExpInfo();
  }
}

CPartyModeKillMonsterExpNotify::~CPartyModeKillMonsterExpNotify() = default;

char CPartyModeKillMonsterExpNotify::Add(CPlayer *pkMember, float fExp)
{
  if (!pkMember || !CPartyPlayer::IsPartyMode(pkMember->m_pPartyMgr))
  {
    return 0;
  }

  if (m_byMemberCnt < 8u)
  {
    m_kInfo[m_byMemberCnt].SetGetExp(pkMember, fExp);
    ++m_byMemberCnt;
    return 1;
  }

  m_byMemberCnt = 8;
  return 0;
}

void CPartyModeKillMonsterExpNotify::Notify()
{
  if (m_bKillMonster && m_byMemberCnt)
  {
    if (m_byMemberCnt >= 8u)
    {
      m_byMemberCnt = 8;
    }

    for (unsigned __int8 index = 0; index < m_byMemberCnt; ++index)
    {
      m_kInfo[index].Notify();
    }
  }
}

void CPartyModeKillMonsterExpNotify::SetKillMonsterFlag()
{
  m_bKillMonster = true;
}
