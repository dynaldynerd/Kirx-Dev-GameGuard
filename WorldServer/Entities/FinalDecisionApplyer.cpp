#include "pch.h"

#include "FinalDecisionApplyer.h"

#include "CandidateMgr.h"
#include "ClassOrderProcessor.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "DqsDbStructs.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include <cstdio>

FinalDecisionApplyer::FinalDecisionApplyer()
  : ElectProcessor(_eFinalDecisionApplyer)
{
}

bool FinalDecisionApplyer::Initialize()
{
  char logPath[256]{};
  std::snprintf(
    logPath,
    sizeof(logPath),
    "..\\ZoneServerLog\\SystemLog\\Patriarch\\CandidateRegister_%d.log",
    GetKorLocalTime());
  _kSysLog.SetWriteLogFile(logPath, true, false, true, true);

  ElectProcessor::Initialize();
  Doit(_eReqQryFinalApplyer, nullptr, nullptr);
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x78u, nullptr, false);
  return true;
}

int FinalDecisionApplyer::Doit(Cmd cmd, CPlayer *player, char *data)
{
  (void)player;
  (void)data;

  if (cmd == _eReqQryFinalApplyer)
  {
    _FinalDecisionApply();
    return 0;
  }

  return 255;
}

void FinalDecisionApplyer::_FinalDecisionApply()
{
  static char kEmptyLeaderName[1] = {'\0'};

  for (int race = 0; race < 3; ++race)
  {
    for (int leaderIndex = 0; leaderIndex < 5; ++leaderIndex)
    {
      const CandidateMgr::_candidate_info *leader =
        CandidateMgr::Instance()->GetLeader(static_cast<unsigned __int8>(race), static_cast<unsigned int>(leaderIndex));
      if (leader != nullptr)
      {
        CPvpUserAndGuildRankingSystem::Instance()->SetUpdateRaceBossSerial(
          leader->byRace,
          static_cast<unsigned __int8>(leaderIndex),
          leader->dwAvatorSerial);
        g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(
          leader->byRace,
          static_cast<unsigned __int8>(leaderIndex),
          const_cast<char *>(leader->wszName));
      }
    }

    for (int classIndex = 5; classIndex < 9; ++classIndex)
    {
      CPvpUserAndGuildRankingSystem::Instance()->SetUpdateRaceBossSerial(
        static_cast<unsigned __int8>(race),
        static_cast<unsigned __int8>(classIndex),
        0);
      g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(
        static_cast<unsigned __int8>(race),
        static_cast<unsigned __int8>(classIndex),
        kEmptyLeaderName);
    }
  }

  CPvpUserAndGuildRankingSystem::Instance()->ApplyUpdatedBossInfo();

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bOper)
    {
      const unsigned __int8 raceCode = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
      g_Main.m_kEtcNotifyInfo.Notify(raceCode, player->m_ObjID.m_wIndex);
    }
  }

  CandidateMgr::Instance()->ApplyPatriarchGroup();

  for (int race = 0; race < 3; ++race)
  {
    for (int classType = 5; classType < 9; ++classType)
    {
      ClassOrderProcessor::Instance()->UpdatePacket(
        static_cast<unsigned __int8>(race),
        static_cast<unsigned __int8>(classType));
    }
  }

  _qry_case_raceboss_accumulation_winrate query{};
  const int querySize = static_cast<int>(query.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x8Eu, reinterpret_cast<char *>(&query), querySize);
}
