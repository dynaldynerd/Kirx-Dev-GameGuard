#include "pch.h"

#include "ClassOrderProcessor.h"
#include "PatriarchElectProcessor.h"
#include "CandidateMgr.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CPlayer.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "Packet/ZoneClientPacket.h"

#include <cstring>
#include <mmsystem.h>

ClassOrderProcessor *ClassOrderProcessor::_pkInstance = nullptr;

ClassOrderProcessor::ClassOrderProcessor()
  : ElectProcessor(_eClassOrderProcessor)
{
  _byPtType[0] = 56;
  _byPtType[1] = 17;

  for (int raceIndex = 0; raceIndex < 3; ++raceIndex)
  {
    for (int classType = 0; classType < 4; ++classType)
    {
      _pt_appoint_inform_request_zocl::__body &entry = _kSend[raceIndex].body[classType];
      entry.byLevel = static_cast<unsigned __int8>(-1);
      entry.byClassType = static_cast<unsigned __int8>(classType);
      entry.dPvpPoint = 0.0;
      std::memset(entry.wszAvatorName, 0, sizeof(entry.wszAvatorName));
    }
  }
}

ClassOrderProcessor::~ClassOrderProcessor()
{
}

bool ClassOrderProcessor::Initialize()
{
  char logPath[256]{};
  std::snprintf(
    logPath,
    sizeof(logPath),
    "..\\ZoneServerLog\\SystemLog\\Patriarch\\ClassOrderProcessor_%llu.log",
    static_cast<unsigned long long>(GetKorLocalTime()));
  _kSysLog.SetWriteLogFile(logPath, true, false, true, true);

  ElectProcessor::Initialize();
  g_Main.PushDQSData(-1, nullptr, 120, nullptr, false);
  return true;
}

int ClassOrderProcessor::Doit(Cmd cmd, CPlayer *player, char *payload)
{
  switch (cmd)
  {
  case _eQryAppoint:
    return _QueryAppoint(player, payload);
  case _eReqAppoint:
    return _RequestAppoint(player, payload);
  case _eRespAppoint:
    return _ResponseAppoint(player, payload);
  case _eReqDischarge:
    return _RequestDischarge(player, payload);
  case _eReqPatriarchInform:
    SendMsg_PatriarchInform(player);
    return 0;
  default:
    return 255;
  }
}

ClassOrderProcessor *ClassOrderProcessor::Instance()
{
  if (!_pkInstance)
  {
    _pkInstance = new ClassOrderProcessor();
  }
  return _pkInstance;
}

int ClassOrderProcessor::_CheckUserInfo(unsigned __int8 byRace, unsigned __int8 byClassType, CPlayer *user)
{
  if (user == nullptr || !user->m_bOper)
    return 1;
  if (user->m_Param.GetRaceCode() != byRace)
    return 1;

  const CandidateMgr::_candidate_info::ClassType classType =
    static_cast<CandidateMgr::_candidate_info::ClassType>(byClassType);

  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  if (candidateMgr->GetPatriarchGroup(byRace, classType) != nullptr)
    return 2;

  const unsigned int userSerial = user->m_Param.GetCharSerial();
  if (candidateMgr->GetPatriarchGroupBySerial(byRace, userSerial) != nullptr)
    return 3;

  if (user->m_Param.m_byPvPGrade < 3u)
    return 4;

  return 0;
}

int ClassOrderProcessor::_QueryAppoint(CPlayer *player, char *data)
{
  const unsigned int serial = player->m_Param.GetCharSerial();
  const int race = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(race, 0);
  if (serial != raceBossSerial)
    return 20;

  if (static_cast<unsigned __int8>(*data) < 4u)
  {
    const unsigned __int8 classType = static_cast<unsigned __int8>(*data + 5);
    CPlayer *target = static_cast<CPlayer *>(g_Main.GetCharW(data + 1));
    const int result = _CheckUserInfo(
      static_cast<unsigned __int8>(player->m_Param.GetRaceCode()),
      classType,
      target);
    SendMsg_QueryAppointResult(player->m_id.wIndex, static_cast<unsigned __int8>(result), static_cast<unsigned __int8>(*data), data + 1);
  }

  return 0;
}

int ClassOrderProcessor::_RequestAppoint(CPlayer *player, char *data)
{
  const unsigned int serial = player->m_Param.GetCharSerial();
  const int race = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(race, 0);
  if (serial != raceBossSerial)
    return 20;

  if (static_cast<unsigned __int8>(*data) >= 4u)
    return 0;

  const unsigned __int8 classType = static_cast<unsigned __int8>(*data + 5);
  CPlayer *target = static_cast<CPlayer *>(g_Main.GetCharW(data + 1));
  const int checkResult = _CheckUserInfo(
    static_cast<unsigned __int8>(player->m_Param.GetRaceCode()),
    classType,
    target);
  if (checkResult != 0)
  {
    SendMsg_QueryAppointResult(
      player->m_id.wIndex,
      static_cast<unsigned __int8>(checkResult),
      static_cast<unsigned __int8>(*data),
      data + 1);
    return 0;
  }

  target->m_byPatriarchAppointPropose = classType;
  target->m_dwPatriarchAppointTime = timeGetTime();

  _pt_propose_appoint_zocl msg{};
  msg.byClassType = static_cast<unsigned __int8>(*data);
  const unsigned __int8 msgType[2] = {56, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(
    target->m_id.wIndex,
    const_cast<unsigned __int8 *>(msgType),
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return 0;
}

int ClassOrderProcessor::_ResponseAppoint(CPlayer *player, char *data)
{
  const int race = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(race, 0);
  CPlayer *raceBoss = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, raceBossSerial);

  if (player->m_byPatriarchAppointPropose == 255)
    return 0;

  if (*data != 0)
  {
    if (*data == 1 && raceBoss != nullptr && raceBoss->m_bOper)
    {
      SendMsg_QueryAppointResult(
        raceBoss->m_id.wIndex,
        5u,
        player->m_byPatriarchAppointPropose,
        player->m_Param.GetCharNameW());
    }

    player->m_byPatriarchAppointPropose = 255;
    player->m_dwPatriarchAppointTime = -1;
    return 0;
  }

  const int checkResult = _CheckUserInfo(
    static_cast<unsigned __int8>(player->m_Param.GetRaceCode()),
    player->m_byPatriarchAppointPropose,
    player);
  if (checkResult != 0)
  {
    if (raceBoss != nullptr && raceBoss->m_bOper)
    {
      SendMsg_QueryAppointResult(
        raceBoss->m_id.wIndex,
        static_cast<unsigned __int8>(checkResult),
        player->m_byPatriarchAppointPropose,
        player->m_Param.GetCharNameW());
    }
  }
  else
  {
    const CandidateMgr::_candidate_info::ClassType classType =
      static_cast<CandidateMgr::_candidate_info::ClassType>(player->m_byPatriarchAppointPropose);
  if (CandidateMgr::Instance()->AppointPatriarchGroup(player, classType))
    {
      const unsigned int playerSerial = player->m_Param.GetCharSerial();
      const int raceCode = static_cast<int>(player->m_Param.GetRaceCode());
      const unsigned int patriarchSerial =
        CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);
      const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetElectSerial();
      _kSysLog.Write(
        "Patriarch Appoint (ES:%d, PS:%d) >> Class(%d) Avator(%d)",
        electSerial,
        patriarchSerial,
        player->m_byPatriarchAppointPropose,
        playerSerial);
    }
    else if (raceBoss != nullptr && raceBoss->m_bOper)
    {
      SendMsg_QueryAppointResult(
        raceBoss->m_id.wIndex,
        2u,
        player->m_byPatriarchAppointPropose,
        player->m_Param.GetCharNameW());
    }
  }

  player->m_byPatriarchAppointPropose = 255;
  player->m_dwPatriarchAppointTime = -1;
  return 0;
}

int ClassOrderProcessor::_RequestDischarge(CPlayer *player, char *data)
{
  const unsigned int playerSerial = player->m_Param.GetCharSerial();
  const int playerRace = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned int raceBossSerial =
    CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(playerRace, 0);
  if (playerSerial != raceBossSerial)
    return 20;

  const CandidateMgr::_candidate_info::ClassType classType =
    static_cast<CandidateMgr::_candidate_info::ClassType>(static_cast<unsigned __int8>(*data) + 5);
  if (static_cast<unsigned __int8>(classType) < 5u)
    return 22;

  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  CandidateMgr::_candidate_info *candidate = candidateMgr->GetPatriarchGroup(playerRace, classType);
  if (candidate == nullptr)
    return 23;
  if (std::strcmp(candidate->wszName, data + 1) != 0)
    return 24;
  if (!candidateMgr->DischargePatriarchGroup(playerRace, classType))
    return 23;

  CPvpUserAndGuildRankingSystem::Instance()->SetUpdateRaceBossSerial(candidate->byRace, candidate->eClassType, 0);
  g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(candidate->byRace, candidate->eClassType, candidate->wszName);
  CPvpUserAndGuildRankingSystem::Instance()->ApplyUpdatedBossInfo();

  for (int i = 0; i < MAX_PLAYER; ++i)
  {
    CPlayer *online = &g_Player[i];
    if (!online->m_bOper)
      continue;
    if (online->m_Param.GetRaceCode() != candidate->byRace)
      continue;
    g_Main.m_kEtcNotifyInfo.Notify(static_cast<unsigned __int8>(online->m_Param.GetRaceCode()), online->m_ObjID.m_wIndex);
  }

  const int race = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned __int16 len = static_cast<unsigned __int16>(sizeof(_kSend[race]));
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    _byPtType,
    reinterpret_cast<char *>(&_kSend[race]),
    len);

  const unsigned int electSerial = PatriarchElectProcessor::Instance()->GetElectSerial();
  _kSysLog.Write(
    "Patriarch Discharge (ES:%d, PS:%d) >> Class(%d) Avator(%s)",
    electSerial,
    player->m_Param.GetCharSerial(),
    static_cast<unsigned __int8>(*data) + 5,
    data + 1);
  return 0;
}

void ClassOrderProcessor::SendMsg_PatriarchInform(CPlayer *player)
{
  const unsigned int playerSerial = player->m_Param.GetCharSerial();
  const int raceCode = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned int raceBossSerial = CPvpUserAndGuildRankingSystem::Instance()->GetCurrentRaceBossSerial(raceCode, 0);
  if (playerSerial != raceBossSerial)
    return;

  const int race = static_cast<int>(player->m_Param.GetRaceCode());
  const unsigned __int16 len = static_cast<unsigned __int16>(sizeof(_kSend[race]));
  g_Network.m_pProcess[0]->LoadSendMsg(
    player->m_ObjID.m_wIndex,
    _byPtType,
    reinterpret_cast<char *>(&_kSend[race]),
    len);
}

void ClassOrderProcessor::SendMsg_QueryAppointResult(
  unsigned __int16 wIndex,
  unsigned __int8 byRet,
  unsigned __int8 byClassType,
  char *pwszAvatorName)
{
  unsigned __int8 pbyType[36]{};
  std::memcpy(pbyType, "8\t", 2u);

  _pt_query_appoint_zocl msg{};
  msg.byRet = byRet;
  msg.byClassType = byClassType;
  std::strcpy(msg.wszAvatorName, pwszAvatorName);

  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, pbyType, reinterpret_cast<char *>(&msg), len);
}

void ClassOrderProcessor::UpdatePacket(unsigned __int8 byRace, unsigned __int8 byClassType)
{
  const unsigned int classIndex = byClassType - 5;
  if (classIndex >= 4)
  {
    return;
  }

  CandidateMgr *mgr = CandidateMgr::Instance();
  CandidateMgr::_candidate_info *group =
    mgr->GetPatriarchGroup(byRace, static_cast<CandidateMgr::_candidate_info::ClassType>(byClassType));
  if (group)
  {
    if (group->eStatus == CandidateMgr::_candidate_info::candidate_discharge)
    {
      _kSend[byRace].body[classIndex].byLevel = 0;
    }
    else
    {
      _kSend[byRace].body[classIndex].byLevel = group->byLevel;
    }

    _kSend[byRace].body[classIndex].byClassType =
      static_cast<unsigned __int8>(static_cast<unsigned __int8>(group->eClassType) - 5);
    _kSend[byRace].body[classIndex].dPvpPoint = group->dPvpPoint;
    std::strcpy(_kSend[byRace].body[classIndex].wszAvatorName, group->wszName);
  }
  else
  {
    _kSend[byRace].body[classIndex].byClassType = static_cast<unsigned __int8>(byClassType - 5);
    _kSend[byRace].body[classIndex].byLevel = static_cast<unsigned __int8>(-1);
  }
}
