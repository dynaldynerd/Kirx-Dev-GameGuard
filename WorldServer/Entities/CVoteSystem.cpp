#include "pch.h"

#include "WorldServerUtil.h"

#include "CVoteSystem.h"

#include "CMainThread.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CUserDB.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "PatriarchElectProcessor.h"
#include "pt_result_punishment_zocl.h"

#include <ctime>
#include <cstring>

_starting_vote_inform_zocl::_starting_vote_inform_zocl()
{
  wContentSize = 0;
}

__int64 _starting_vote_inform_zocl::size()
{
  if (wContentSize > 1280)
  {
    wContentSize = 0;
  }
  return 1289LL - (1280 - wContentSize);
}

_started_vote_inform_zocl::_started_vote_inform_zocl()
{
  wContentSize = 0;
}

__int64 _started_vote_inform_zocl::size()
{
  if (wContentSize > 1280)
  {
    wContentSize = 0;
  }
  return 1297LL - (1280 - wContentSize);
}

CVoteSystem::CVoteSystem() : m_bActive(false), m_byRaceCode(static_cast<unsigned __int8>(-1)), m_nSerial(0)
{
  m_SendStarted = _started_vote_inform_zocl();
  m_listVote.SetList(5064);
}

CVoteSystem::~CVoteSystem() = default;

char CVoteSystem::StartVote(char *pwszContent, unsigned __int8 byLimGrade, unsigned __int8 byRaceCode)
{
  if (m_bActive)
  {
    return 0;
  }

  ++m_nSerial;
  m_byRaceCode = byRaceCode;
  std::memset(m_dwPoint, 0, sizeof(m_dwPoint));
  m_listVote.ResetList();
  m_byLimGrade = byLimGrade;
  m_dwLastBroadcastTime = GetLoopTime();
  m_dwStartVoteTime = GetLoopTime();
  m_bActive = true;
  m_bHurry = false;
  m_bPunishment = false;

  _starting_vote_inform_zocl started{};
  started.nVoteSerial = m_nSerial;
  started.byLimGrade = m_byLimGrade;
  started.wLeftSec = 300;
  std::strcpy(started.wszContent, pwszContent);
  started.wContentSize = std::strlen(pwszContent) + 1;

  unsigned __int8 type[16]{};
  type[0] = 26;
  type[1] = 3;
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *player = &g_Player[clientIndex];
    if (player->m_bLive && static_cast<unsigned int>(player->m_Param.GetRaceCode()) == m_byRaceCode)
    {
      const unsigned __int16 len = static_cast<unsigned __int16>(started.size());
      g_Network.m_pProcess[0]->LoadSendMsg(clientIndex, type, reinterpret_cast<char *>(&started), len);
    }
  }

  m_SendStarted.nVoteSerial = m_nSerial;
  m_SendStarted.byLimGrade = m_byLimGrade;
  std::strcpy(m_SendStarted.wszContent, pwszContent);
  m_SendStarted.wContentSize = std::strlen(pwszContent) + 1;
  m_SendStarted.wLeftSec = 300;
  return 1;
}

char CVoteSystem::StartVote(
  unsigned __int8 byRaceCode,
  unsigned __int8 byPunishType,
  char *pwszContent,
  char *pwszName,
  unsigned int dwSerial)
{
  if (m_bActive)
  {
    return 0;
  }

  ++m_nSerial;
  m_byRaceCode = byRaceCode;
  std::memset(m_dwPoint, 0, sizeof(m_dwPoint));
  m_listVote.ResetList();
  m_byLimGrade = 0;
  m_dwLastBroadcastTime = GetLoopTime();
  m_dwStartVoteTime = GetLoopTime();
  m_bActive = true;
  m_bHurry = false;
  m_bPunishment = true;
  m_byPunishType = byPunishType;
  m_dwAvatorSerial = dwSerial;
  std::strcpy(m_wszCharName, pwszName);

  _starting_vote_inform_zocl started{};
  started.nVoteSerial = m_nSerial;
  started.byLimGrade = m_byLimGrade;
  started.wLeftSec = 300;
  std::strcpy(started.wszContent, pwszContent);
  started.wContentSize = std::strlen(pwszContent) + 1;

  unsigned __int8 type[16]{};
  type[0] = 26;
  type[1] = 3;

  m_SendStarted.nVoteSerial = m_nSerial;
  m_SendStarted.byLimGrade = m_byLimGrade;
  std::strcpy(m_SendStarted.wszContent, pwszContent);
  m_SendStarted.wContentSize = std::strlen(pwszContent) + 1;
  m_SendStarted.wLeftSec = 300;

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bLive && player->m_bOper &&
        static_cast<unsigned int>(player->m_Param.GetRaceCode()) == m_byRaceCode)
    {
      const unsigned int charSerial = player->m_Param.GetCharSerial();
      const int raceCode = player->m_Param.GetRaceCode();
      CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
      if (ranking->GetBossType(raceCode, charSerial) == 255)
      {
        const unsigned int serial = player->m_Param.GetCharSerial();
        SendMsg_StartedVoteInform(player->m_ObjID.m_wIndex, serial, true);
      }
      else
      {
        const unsigned __int16 len = static_cast<unsigned __int16>(started.size());
        g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&started), len);
      }
    }
  }

  return 1;
}

void CVoteSystem::CompleteSelectCharSerial(const _qry_case_select_charserial *query)
{
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int bossSerial = ranking->GetCurrentRaceBossSerial(query->byRace, 0);
  CPlayer *boss = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, bossSerial);

  if (query->dwAvatorSerial == static_cast<unsigned int>(-1))
  {
    if (boss)
    {
      boss->SendMsg_ProposeVoteResult(8);
    }
  }
  else if (!StartVote(
             query->byRace,
             query->byType,
             const_cast<char *>(query->wszContent),
             const_cast<char *>(query->wszCharName),
             query->dwAvatorSerial))
  {
    if (boss)
    {
      boss->SendMsg_ProposeVoteResult(7);
    }
  }
}

char CVoteSystem::ActVote(unsigned int dwAvatorSerial, unsigned __int8 byPoint)
{
  if (!m_bActive)
  {
    return 0;
  }
  if (m_listVote.IsInList(dwAvatorSerial))
  {
    return 0;
  }

  m_listVote.PushNode_Back(dwAvatorSerial);
  ++m_dwPoint[byPoint];
  return 1;
}

void CVoteSystem::Loop()
{
  if (!m_bActive)
  {
    return;
  }

  const unsigned int loopTime = GetLoopTime();
  if (m_bHurry)
  {
    if (loopTime - m_dwStartVoteTime > 300000)
    {
      StopVote();
      return;
    }
  }
  else if (loopTime - m_dwStartVoteTime > 240000)
  {
    m_bHurry = true;
  }

  m_SendStarted.wLeftSec = (300000 - (loopTime - m_dwStartVoteTime)) / 1000;
  if (loopTime - m_dwLastBroadcastTime <= 10000)
  {
    return;
  }

  m_dwLastBroadcastTime = loopTime;

  _progress_vote_inform_zocl msg{};
  msg.nVoteSerial = m_nSerial;
  for (int index = 0; index < 3; ++index)
  {
    msg.wPoint[index] = static_cast<unsigned __int16>(m_dwPoint[index]);
  }
  msg.bHurry = m_bHurry;

  unsigned __int8 type[20]{};
  type[0] = 26;
  type[1] = 4;

  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *player = &g_Player[clientIndex];
    if (player->m_bLive && static_cast<unsigned int>(player->m_Param.GetRaceCode()) == m_byRaceCode)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        clientIndex,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }
}

char CVoteSystem::StopVote()
{
  if (!m_bActive)
  {
    return 0;
  }

  m_bActive = false;
  m_listVote.ResetList();

  _complete_vote_inform_zocl msg{};
  msg.nVoteSerial = m_nSerial;
  for (int index = 0; index < 3; ++index)
  {
    msg.wPoint[index] = static_cast<unsigned __int16>(m_dwPoint[index]);
  }

  unsigned __int8 type[20]{};
  type[0] = 26;
  type[1] = 7;

  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *player = &g_Player[clientIndex];
    if (player->m_bLive && static_cast<unsigned int>(player->m_Param.GetRaceCode()) == m_byRaceCode)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(
        clientIndex,
        type,
        reinterpret_cast<char *>(&msg),
        static_cast<unsigned __int16>(sizeof(msg)));
    }
  }

  if (m_bPunishment && m_dwPoint[0] > m_dwPoint[1])
  {
    ProcessPunishment();
  }

  return 1;
}

void CVoteSystem::SendMsg_StartedVoteInform(unsigned int n, unsigned int dwAvatorSerial, bool bPunish)
{
  m_SendStarted.bActed = m_listVote.IsInList(dwAvatorSerial) || bPunish;
  for (int index = 0; index < 3; ++index)
  {
    m_SendStarted.wPoint[index] = static_cast<unsigned __int16>(m_dwPoint[index]);
  }
  m_SendStarted.bHurry = m_bHurry;

  unsigned __int8 type[36]{};
  type[0] = 26;
  type[1] = 33;
  const unsigned __int16 len = static_cast<unsigned __int16>(m_SendStarted.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&m_SendStarted), len);
}

void CVoteSystem::ProcessPunishment()
{
  const std::time_t now = std::time(nullptr);
  const unsigned int value = static_cast<unsigned int>(now / 60);

  CPlayer *player = static_cast<CPlayer *>(g_Main.GetCharW(m_wszCharName));
  if (player && player->m_pUserDB)
  {
    player->m_pUserDB->m_AvatorData.dbAvator.m_dwPunishment[m_byPunishType] = value;
    PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
    const unsigned int electSerial = processor->GetCurrPatriarchElectSerial();
    player->m_pUserDB->m_AvatorData.dbAvator.m_dwElectSerial[m_byPunishType] = electSerial;
  }

  unsigned __int8 type[36]{};
  type[0] = 13;
  type[1] = 116;

  _pt_result_punishment_zocl result{};
  result.byType = m_byPunishType;
  std::strcpy(result.wszCharName, m_wszCharName);
  for (unsigned int clientIndex = 0; clientIndex < MAX_PLAYER; ++clientIndex)
  {
    CPlayer *player = &g_Player[clientIndex];
    if (player->m_bLive && static_cast<unsigned int>(player->m_Param.GetRaceCode()) == m_byRaceCode)
    {
      const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
      g_Network.m_pProcess[0]->LoadSendMsg(clientIndex, type, reinterpret_cast<char *>(&result), len);
    }
  }

  _qry_case_update_punishment update{};
  update.byType = m_byPunishType;
  update.dwValue = value;
  update.dwAvatorSerial = m_dwAvatorSerial;
  PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
  update.dwElectSerial = processor->GetCurrPatriarchElectSerial();
  std::strcpy(update.wszCharName, m_wszCharName);

  const int size = update.size();
  g_Main.PushDQSData(-1, nullptr, 127, reinterpret_cast<char *>(&update), size);
}
