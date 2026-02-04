#include "pch.h"

#include "CRaceBossWinRate.h"

#include "CNetworkEX.h"
#include "notify_race_boss_winrate_zocl.h"
#include "GlobalObjects.h"

CRaceBossWinRate *CRaceBossWinRate::m_Inst = nullptr;

CRaceBossWinRate::CRaceBossWinRate()
{
  m_byTotalBattleCnt = 0;
  memset_0(m_byWinCnt, 0, sizeof(m_byWinCnt));
  memset_0(m_dwAccBattleCnt, 0, sizeof(m_dwAccBattleCnt));
  memset_0(m_dwAccWinCnt, 0, sizeof(m_dwAccWinCnt));
}

CRaceBossWinRate *CRaceBossWinRate::Instance()
{
  if (!m_Inst)
  {
    m_Inst = new CRaceBossWinRate();
  }
  return m_Inst;
}

void CRaceBossWinRate::UpdateTotalCnt()
{
  ++m_byTotalBattleCnt;
  Notify();
}

void CRaceBossWinRate::UpdateWinCnt(unsigned __int8 byRace)
{
  ++m_byWinCnt[byRace];
  ++m_byTotalBattleCnt;
  Notify();
}

void CRaceBossWinRate::Notify()
{
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bOper && player->m_bLive)
    {
      const unsigned __int8 race = static_cast<unsigned __int8>(CPlayerDB::GetRaceCode(&player->m_Param));
      unsigned __int8 type[2]{};
      type[0] = 13;
      type[1] = 123;

      _notify_race_boss_winrate_zocl msg{};
      msg.byTotalCnt = m_byTotalBattleCnt;
      msg.byCurWinCnt = m_byWinCnt[race];
      msg.dwAccTotalCnt = m_dwAccBattleCnt[race];
      msg.dwAccWinCnt = m_dwAccWinCnt[race];

      const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
      g_Network.m_pProcess[0]->LoadSendMsg(index, type, reinterpret_cast<char *>(&msg), len);
    }
  }
}
