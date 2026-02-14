#include "pch.h"

#include "FinalDecisionProcessor.h"

#include "CandidateMgr.h"
#include "CNetProcess.h"
#include "PatriarchElectProcessor.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"

#include <cstring>
#include <cstdio>

FinalDecisionProcessor::FinalDecisionProcessor()
  : ElectProcessor(_eFinalDecisionProcessor)
{
  memset(_Send, 0, sizeof(_Send));
}

bool FinalDecisionProcessor::Initialize()
{
  char logPath[256]{};
  std::snprintf(
    logPath,
    sizeof(logPath),
    "..\\ZoneServerLog\\SystemLog\\Patriarch\\FinalDecisionProcessor_%d.log",
    GetKorLocalTime());
  _kSysLog.SetWriteLogFile(logPath, true, false, true, true);

  ElectProcessor::Initialize();
  PatriarchElectProcessor::Instance()->PushDQSCheckInvalidChar();
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x78u, nullptr, false);
  return true;
}

int FinalDecisionProcessor::Doit(Cmd cmd, CPlayer *player, char *data)
{
if (cmd == _eReqQryFinalDecision)
  {
    _FinalDecision();
  }
  else if (cmd == _eReqQrySetWinner)
  {
    _SetWinner();
  }
  else if (cmd == _eReqNetFinalDecision)
  {
    _ReqNetFinalDecision(player);
  }
  else
  {
    return 255;
  }

  return 0;
}

void FinalDecisionProcessor::_FinalDecision()
{
  CandidateMgr::Instance()->FinalDecision();
  _SetWinner();

  unsigned __int8 packetType[2]{56, 7};
  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (!player->m_bOper)
    {
      continue;
    }

    _pt_notify_final_decision *payload = &_Send[player->m_Param.GetRaceCode()];
    const unsigned __int16 packetLength = static_cast<unsigned __int16>(payload->size());
    g_Network.m_pProcess[0]->LoadSendMsg(
      player->m_ObjID.m_wIndex,
      packetType,
      reinterpret_cast<char *>(payload),
      packetLength);
  }
}

void FinalDecisionProcessor::_SetWinner()
{
  for (int race = 0; race < 3; ++race)
  {
    _pt_notify_final_decision *sendData = &_Send[race];
    for (int index = 0; index < 5; ++index)
    {
      const CandidateMgr::_candidate_info *leader =
        CandidateMgr::Instance()->GetLeader(static_cast<unsigned __int8>(race), static_cast<unsigned int>(index));
      if (leader != nullptr)
      {
        strcpy_s(sendData->wszAvatorName[index], sizeof(sendData->wszAvatorName[index]), leader->wszName);
      }
    }
  }
}

void FinalDecisionProcessor::_ReqNetFinalDecision(CPlayer *player)
{
  _pt_notify_final_decision *payload = &_Send[player->m_Param.GetRaceCode()];
  unsigned __int8 packetType[2]{56, 7};
  const unsigned __int16 packetLength = static_cast<unsigned __int16>(payload->size());
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    packetType,
    reinterpret_cast<char *>(payload),
    packetLength);
}
