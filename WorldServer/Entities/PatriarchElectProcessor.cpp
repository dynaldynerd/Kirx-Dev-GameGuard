#include "pch.h"

#include "PatriarchElectProcessor.h"

#include "CandidateMgr.h"
#include "CandidateRegister.h"
#include "SecondCandidateCrystallizer.h"
#include "Voter.h"
#include "FinalDecisionProcessor.h"
#include "FinalDecisionApplyer.h"
#include "ClassOrderProcessor.h"
#include "CMainThread.h"
#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"
#include "CPlayer.h"
#include "CUserDB.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRFWorldDatabase.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "pt_inform_appoint_zocl.h"
#include "pt_result_appoint_zocl.h"
#include "pt_result_code_zocl.h"
#include "qry_case_insert_candidate.h"
#include "qry_case_request_refund.h"
#include "qry_case_candidate_scalar_payloads.h"
#include "sel_patriarch_elect_state.h"
#include "DqsDbStructs.h"

#include <cstdio>
#include <cstring>
#include <mmsystem.h>

static_assert(sizeof(CandidateRegister) == 70712, "CandidateRegister size mismatch");
static_assert(sizeof(SecondCandidateCrystallizer) == 200, "SecondCandidateCrystallizer size mismatch");
static_assert(sizeof(Voter) == 1608, "Voter size mismatch");
static_assert(sizeof(FinalDecisionProcessor) == 456, "FinalDecisionProcessor size mismatch");
static_assert(sizeof(FinalDecisionApplyer) == 200, "FinalDecisionApplyer size mismatch");
static_assert(sizeof(ClassOrderProcessor) == 528, "ClassOrderProcessor size mismatch");

PatriarchElectProcessor *PatriarchElectProcessor::_pkInstance = nullptr;

PatriarchElectProcessor::PatriarchElectProcessor()
{
  _eProcessType = ElectProcessor::_eNonProcessor;
  _kRunningProcessor = nullptr;
  _bTimeCheck = true;
  _bInitProce = false;
  _dwNextCheckTime = 0;
  _dwNextCheckDay = 0;
  _dwNextScoreUpdateTime = 0;
  _dwElectSerial = 0;
  _dwCurrPatriarchElectSerial = 0;
  for (int index = 0; index < 6; ++index)
  {
    _kProcessor[index] = nullptr;
  }
  for (int race = 0; race < 3; ++race)
  {
    m_dwNonvoteCnt[race] = 0;
    m_dwTotalVoteCnt[race] = 0;
    m_dwHighGradeNum[race] = 0;
  }
}

PatriarchElectProcessor *PatriarchElectProcessor::Instance()
{
  if (!_pkInstance)
  {
    _pkInstance = new PatriarchElectProcessor();
  }
  return _pkInstance;
}

void PatriarchElectProcessor::Destroy()
{
  if (_pkInstance)
  {
    delete _pkInstance;
    _pkInstance = nullptr;
  }
}

char PatriarchElectProcessor::LoadDatabae()
{
  if (!LoadElectState())
  {
    return 0;
  }
  if (!CandidateMgr::Instance()->LoadDatabase())
  {
    return 0;
  }
  if (!CandidateMgr::Instance()->LoadPatriarchGroup())
  {
    return 0;
  }

  for (int race = 0; race < 3; ++race)
  {
    for (
      CandidateMgr::_candidate_info::ClassType classType = CandidateMgr::_candidate_info::patriarch;
      classType < CandidateMgr::_candidate_info::patriarch_group_num;
      classType = static_cast<CandidateMgr::_candidate_info::ClassType>(classType + 1))
    {
      CandidateMgr::_candidate_info *group = CandidateMgr::Instance()->GetPatriarchGroup(race, classType);
      if (!group)
      {
        continue;
      }

      if (group->eStatus != CandidateMgr::_candidate_info::candidate_discharge)
      {
        CPvpUserAndGuildRankingSystem::Instance()->SetUpdateRaceBossSerial(
          race,
          static_cast<unsigned __int8>(classType),
          group->dwAvatorSerial);
        CPvpUserAndGuildRankingSystem::Instance()->SetCurrentRaceBossSerial(
          race,
          static_cast<unsigned __int8>(classType),
          group->dwAvatorSerial);
        g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(
          race,
          static_cast<unsigned __int8>(classType),
          group->wszName);
      }

      if (group->eClassType >= 5)
      {
        ClassOrderProcessor::Instance()->UpdatePacket(
          static_cast<unsigned __int8>(race),
          static_cast<unsigned __int8>(group->eClassType));
      }
    }
  }

  CPvpUserAndGuildRankingSystem::Instance()->PvpRankDataPacking();
  return 1;
}

bool PatriarchElectProcessor::Initialize()
{
  char buffer[260];

  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\Patriarch", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog\\Patriarch", 30);

  memset(buffer, 0, 256);
  const unsigned int now = GetKorLocalTime();
  sprintf_s(buffer, 256, "..\\ZoneServerLog\\SystemLog\\Patriarch\\PatriarchElect_%u.log", now);
  _kSysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog\\Patriarch", nullptr);
  clear_file("..\\ZoneServerLog\\ServiceLog\\Patriarch", 30);

  CandidateRegister *candidate = static_cast<CandidateRegister *>(operator new(70712));
  if (candidate)
  {
    new (candidate) CandidateRegister();
  }
  else
  {
    candidate = nullptr;
  }
  _kProcessor[0] = candidate;

  SecondCandidateCrystallizer *secondCandidate =
    static_cast<SecondCandidateCrystallizer *>(operator new(200));
  if (secondCandidate)
  {
    new (secondCandidate) SecondCandidateCrystallizer();
  }
  else
  {
    secondCandidate = nullptr;
  }
  _kProcessor[1] = secondCandidate;

  Voter *voter = static_cast<Voter *>(operator new(1608));
  if (voter)
  {
    new (voter) Voter();
  }
  else
  {
    voter = nullptr;
  }
  _kProcessor[2] = voter;

  FinalDecisionProcessor *finalDecision = static_cast<FinalDecisionProcessor *>(operator new(456));
  if (finalDecision)
  {
    new (finalDecision) FinalDecisionProcessor();
  }
  else
  {
    finalDecision = nullptr;
  }
  _kProcessor[3] = finalDecision;

  FinalDecisionApplyer *finalApplyer = static_cast<FinalDecisionApplyer *>(operator new(200));
  if (finalApplyer)
  {
    new (finalApplyer) FinalDecisionApplyer();
  }
  else
  {
    finalApplyer = nullptr;
  }
  _kProcessor[4] = finalApplyer;

  ClassOrderProcessor *classOrder = static_cast<ClassOrderProcessor *>(operator new(528));
  if (classOrder)
  {
    new (classOrder) ClassOrderProcessor();
  }
  else
  {
    classOrder = nullptr;
  }
  _kProcessor[5] = classOrder;

  for (int index = 0; index < 6; ++index)
  {
    if (_kProcessor[index]->GetProcessorType() != index)
    {
      return false;
    }
  }

  _eProcessType = ElectProcessor::_eNonProcessor;
  _dwNextCheckTime = timeGetTime() + 60000;
  return true;
}

void PatriarchElectProcessor::Loop()
{
  if (!g_Main.m_bWorldOpen)
  {
    return;
  }

  if (!_bInitProce)
  {
    InitProcess();
  }

  const DWORD now = timeGetTime();
  if (now > _dwNextCheckTime)
  {
    _dwNextCheckTime = timeGetTime() + 60000;
    SYSTEMTIME localTime{};
    GetLocalTime(&localTime);
    if (_bTimeCheck)
    {
      TimeCheck(localTime.wDayOfWeek, localTime.wHour);
    }
  }
}

bool PatriarchElectProcessor::InitProcess()
{
  if (_eProcessType < ElectProcessor::_eProcessorNum)
  {
    if (!_kProcessor[_eProcessType])
    {
      return false;
    }

    _kRunningProcessor = _kProcessor[_eProcessType];
    if (_eProcessType == ElectProcessor::_eCandidateRegister)
    {
      if (!_kRunningProcessor->Initialize())
      {
        return false;
      }
      _kRunningProcessor->Doit(_eRequestInitCandidate, nullptr, nullptr);
    }
    else if (_eProcessType == ElectProcessor::_eVoter)
    {
      if (!_kRunningProcessor->Initialize())
      {
        return false;
      }
    }
    else if (_eProcessType == ElectProcessor::_eFinalDecisionProcessor)
    {
      _kRunningProcessor->Doit(_eReqQrySetWinner, nullptr, nullptr);
    }
  }

  _bInitProce = true;
  return true;
}

void PatriarchElectProcessor::TimeCheck(unsigned __int16 wDayOfWeek, unsigned __int16 wHour)
{
  bool changed = false;
  if (wDayOfWeek == 1)
  {
    if (_eProcessType == ElectProcessor::_eFinalDecisionApplyer
        || _eProcessType == ElectProcessor::_eClassOrderProcessor)
    {
      if (_eProcessType != ElectProcessor::_eClassOrderProcessor)
      {
        _eProcessType = ElectProcessor::_eClassOrderProcessor;
        changed = true;
      }
    }
    else
    {
      _eProcessType = ElectProcessor::_eFinalDecisionApplyer;
      changed = true;
    }
  }
  else if (wDayOfWeek > 1)
  {
    if (wDayOfWeek <= 5)
    {
      if (_eProcessType != ElectProcessor::_eCandidateRegister)
      {
        _eProcessType = ElectProcessor::_eCandidateRegister;
        changed = true;
      }
    }
    else if (wDayOfWeek == 6)
    {
      if (_eProcessType != ElectProcessor::_eCandidateRegister && wHour < 17)
      {
        _eProcessType = ElectProcessor::_eCandidateRegister;
        changed = true;
      }
      else if (_eProcessType == ElectProcessor::_eSecondCandidateCrystallizer || wHour != 17)
      {
        if (_eProcessType != ElectProcessor::_eVoter && wHour >= 18)
        {
          _eProcessType = ElectProcessor::_eVoter;
          changed = true;
        }
      }
      else
      {
        _eProcessType = ElectProcessor::_eSecondCandidateCrystallizer;
        changed = true;
      }
    }
  }
  else if (_eProcessType != ElectProcessor::_eFinalDecisionProcessor && wHour >= 22 && wHour < 24)
  {
    _eProcessType = ElectProcessor::_eFinalDecisionProcessor;
    changed = true;
  }

  if (changed)
  {
    if (_eProcessType == ElectProcessor::_eCandidateRegister)
    {
      CandidateMgr::Instance()->InitCandidate();
      g_Main.PushDQSData(-1, nullptr, 119, nullptr, 0);
    }

    _kRunningProcessor = _kProcessor[_eProcessType];
    if (!_kRunningProcessor || !_kRunningProcessor->Initialize())
    {
      _kSysLog.Write(
        "PatriarchElectProcessor::TimeCheck() Process Initialize Error (_eProcessType:%d)",
        _eProcessType);
      return;
    }

    if (_eProcessType == ElectProcessor::_eFinalDecisionProcessor)
    {
      g_Main.PushResetServerToken();

      for (int playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex)
      {
        CPlayer *player = &g_Player[playerIndex];
        if (player && player->m_bOper)
        {
          player->m_pUserDB->Update_RaceVoteInfoInit();
        }
      }
    }
  }

  if (_kRunningProcessor
      && _kRunningProcessor->GetProcessorType() == ElectProcessor::_eVoter)
  {
    g_Main.PushDQSData(-1, nullptr, 115, nullptr, 0);
    g_Main.PushDQSData(-1, nullptr, 120, nullptr, 0);
  }
}

unsigned int PatriarchElectProcessor::GetCurrPatriarchElectSerial()
{
  return _dwCurrPatriarchElectSerial;
}

void PatriarchElectProcessor::SetCurrPatriarchElectSerial(unsigned int dwSerial)
{
  _dwCurrPatriarchElectSerial = dwSerial;
}

char PatriarchElectProcessor::Doit(Cmd eCmd, CPlayer *pOne, char *pdata)
{
  if (eCmd == _eReqDischarge || eCmd == _eReqPatriarchInform)
  {
    ClassOrderProcessor *orderProcessor = ClassOrderProcessor::Instance();
    const int result = static_cast<int>(orderProcessor->Doit(eCmd, pOne, pdata));
    if (!result)
    {
      return 1;
    }
    if (pOne)
    {
      SendMsg_ResultCode(pOne->m_id.wIndex, static_cast<unsigned __int8>(result));
    }
    return 1;
  }

  int result = 0;
  if (!this->_kRunningProcessor
      || (result = static_cast<int>(this->_kRunningProcessor->Doit(eCmd, pOne, pdata)), result == 255))
  {
    if (!pOne || !pOne->m_bOper)
    {
      return 1;
    }

    if (eCmd < _eRequestCandidateList)
    {
      result = 2;
    }
    else if (eCmd <= _eRegCandidate)
    {
      result = 2;
    }
    else if (eCmd == _eVote)
    {
      result = 9;
    }
    else if (eCmd <= _eReqNetFinalDecision || eCmd > _eReqPatriarchInform)
    {
      result = 2;
    }
    else
    {
      result = 21;
    }

    SendMsg_ResultCode(pOne->m_id.wIndex, static_cast<unsigned __int8>(result));
    return 1;
  }

  if (!result)
  {
    return 1;
  }
  if (pOne)
  {
    SendMsg_ResultCode(pOne->m_id.wIndex, static_cast<unsigned __int8>(result));
  }
  return 1;
}

void PatriarchElectProcessor::PushDQSCheckInvalidChar()
{
  _qry_case_check_invalid_character queryData{};
  queryData.byProc = static_cast<unsigned __int8>(_eProcessType);
  g_Main.PushDQSData(
    -1,
    nullptr,
    138,
    reinterpret_cast<char *>(&queryData),
    static_cast<int>(queryData.size()));
}

void PatriarchElectProcessor::SendMsg_ResultCode(unsigned int n, unsigned __int8 byCode)
{
  _pt_result_code_zocl msg{};
  msg.byRetCode = byCode;

  unsigned __int8 type[2]{56, static_cast<unsigned __int8>(-1)};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), len);
}

void PatriarchElectProcessor::SendMsg_ConnectNewUser(CPlayer *pOne)
{
  _AVATOR_DATA *avatorData = &pOne->m_pUserDB->m_AvatorData;
  if (avatorData->dbAvator.m_dwGivebackCount)
  {
    const unsigned __int64 addMoney = 10000000ULL * avatorData->dbAvator.m_dwGivebackCount;
    const unsigned int dalant = pOne->m_Param.GetDalant();
    if (CanAddMoneyForMaxLimMoney(addMoney, dalant))
    {
      const unsigned int charSerial = pOne->m_Param.GetCharSerial();
      const unsigned __int8 raceCode = pOne->m_Param.GetRaceCode();
      _qry_case_request_refund query(raceCode, pOne->m_id.wIndex, charSerial, addMoney);
      const int size = query.size();
      g_Main.PushDQSData(static_cast<unsigned __int8>(-1), nullptr, 123, reinterpret_cast<char *>(&query), size);
    }
    else
    {
      SendMsg_ResultCode(pOne->m_id.wIndex, 15);
    }
  }

  const ElectProcessor::ProcessorType processType = _eProcessType;
  if (processType == ElectProcessor::_eVoter)
  {
    if (_kRunningProcessor)
    {
      _kRunningProcessor->Doit(_eSendVotePaper, pOne, nullptr);
      _kRunningProcessor->Doit(_eVoteScore, pOne, nullptr);
    }
  }
  else if (processType == ElectProcessor::_eFinalDecisionProcessor)
  {
    if (_kRunningProcessor)
    {
      _kRunningProcessor->Doit(_eReqNetFinalDecision, pOne, nullptr);
    }
  }
}

unsigned int PatriarchElectProcessor::GetElectSerial()
{
  return _dwElectSerial;
}

ElectProcessor::ProcessorType PatriarchElectProcessor::GetProcessorType()
{
  return _eProcessType;
}

bool PatriarchElectProcessor::GetTimeCheck()
{
  return _bTimeCheck;
}

void PatriarchElectProcessor::SetTimeCheck(bool bFlag)
{
  _bTimeCheck = bFlag;
}

bool PatriarchElectProcessor::ForceChangeProcessor(ElectProcessor::ProcessorType eProc)
{
  if (_bTimeCheck)
  {
    return false;
  }

  if (eProc == ElectProcessor::_eNonProcessor)
  {
    _eProcessType = ElectProcessor::_eNonProcessor;
    _kRunningProcessor = nullptr;
    return true;
  }

  if (eProc < ElectProcessor::_eCandidateRegister || eProc >= ElectProcessor::_eProcessorNum)
  {
    return false;
  }

  bool allowedTransition = false;
  if (_eProcessType == ElectProcessor::_eNonProcessor)
  {
    allowedTransition = (eProc == ElectProcessor::_eCandidateRegister);
  }
  else
  {
    switch (_eProcessType)
    {
      case ElectProcessor::_eCandidateRegister:
        allowedTransition = (eProc == ElectProcessor::_eSecondCandidateCrystallizer);
        break;
      case ElectProcessor::_eSecondCandidateCrystallizer:
        allowedTransition = (eProc == ElectProcessor::_eVoter);
        break;
      case ElectProcessor::_eVoter:
        allowedTransition = (eProc == ElectProcessor::_eFinalDecisionProcessor);
        break;
      case ElectProcessor::_eFinalDecisionProcessor:
        allowedTransition = (eProc == ElectProcessor::_eFinalDecisionApplyer);
        break;
      case ElectProcessor::_eFinalDecisionApplyer:
        allowedTransition = (eProc == ElectProcessor::_eClassOrderProcessor);
        break;
      case ElectProcessor::_eClassOrderProcessor:
        allowedTransition = (eProc == ElectProcessor::_eCandidateRegister);
        break;
      default:
        allowedTransition = false;
        break;
    }
  }

  if (!allowedTransition)
  {
    return false;
  }

  ElectProcessor *nextProcessor = _kProcessor[static_cast<int>(eProc)];
  if (!nextProcessor)
  {
    return false;
  }

  _eProcessType = eProc;
  _kRunningProcessor = nextProcessor;
  return _kRunningProcessor->Initialize();
}

__int64 PatriarchElectProcessor::Insert_Elect()
{
  char buffer[1040]{};
  sprintf_s(
    buffer,
    sizeof(buffer),
    "{ CALL pInsert_Patriarch_Elect ( '%s', %d ) }",
    g_Main.m_szWorldName,
    _eProcessType);

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

__int64 PatriarchElectProcessor::Update_Elect()
{
  char buffer[272]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_Patriarch_Elect_20080611 ( %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d ) }",
    _eProcessType,
    m_dwNonvoteCnt[0],
    m_dwTotalVoteCnt[0],
    m_dwHighGradeNum[0],
    m_dwNonvoteCnt[1],
    m_dwTotalVoteCnt[1],
    m_dwHighGradeNum[1],
    m_dwNonvoteCnt[2],
    m_dwTotalVoteCnt[2],
    m_dwHighGradeNum[2],
    _dwElectSerial);

  if (g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true))
  {
    return 0;
  }

  _kSysLog.Write("Update_Elect DB Error : %s", buffer);
  return 24;
}

char PatriarchElectProcessor::LoadElectState()
{
  _sel_patriarch_elect_state sheet{};
  strcpy_s(sheet.szWorldName, sizeof(sheet.szWorldName), g_Main.m_szWorldName);

  const int result = g_Main.m_pWorldDB->Select_PatriarchElectState(&sheet);
  if (result == 1)
  {
    return 0;
  }

  if (result != 2)
  {
    _dwElectSerial = sheet.dwSerial;
    _eProcessType = static_cast<ElectProcessor::ProcessorType>(sheet.byProcType);
    for (int i = 0; i < 3; ++i)
    {
      m_dwNonvoteCnt[i] = sheet.dwNonVoteCnt[i];
      m_dwTotalVoteCnt[i] = sheet.dwVoteCnt[i];
      m_dwHighGradeNum[i] = sheet.dwHighGradeNum[i];
    }
    g_Main.m_dwServerResetToken = sheet.dwResetServerToken;
  }

  return 1;
}

__int64 PatriarchElectProcessor::Request_Refund(char *pData)
{
  const auto *query = reinterpret_cast<const _qry_case_request_refund *>(pData);
  char buffer[272]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_Patriarch_PayRefund ( %d, %d ) }",
    query->byRace,
    query->dwAvatorSerial);

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

__int64 PatriarchElectProcessor::Insert_PatrirchItemChargeRefund(char *pData)
{
  const auto *query = reinterpret_cast<const _qry_case_request_refund *>(pData);
  char buffer[272]{};
  sprintf(
    buffer,
    "insert [dbo].[tbl_itemcharge] (nAvatorSerial, nItemCode_K, nItemCode_D, nItemCode_U, dtGiveDate, dtTakeDate, Type) "
    "values (%d, 0, %d, default, default, default, 1)",
    query->dwAvatorSerial,
    query->dwRefund);

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

bool PatriarchElectProcessor::CheatClearPatriarch()
{
  char buffer[260]{};
  sprintf(
    buffer,
    "Delete [dbo].[tbl_patriarch_candidate] Where\teSerial = %d And\tClassType >= %d ",
    _dwElectSerial,
    5);
  if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true))
  {
    return false;
  }

  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  ClassOrderProcessor *orderProcessor = ClassOrderProcessor::Instance();
  char emptyName[1]{};

  for (int race = 0; race < 3; ++race)
  {
    for (int classType = 5; classType < CandidateMgr::_candidate_info::patriarch_group_num; ++classType)
    {
      CandidateMgr::_candidate_info *patriarchGroup =
        candidateMgr->GetPatriarchGroup(race, static_cast<CandidateMgr::_candidate_info::ClassType>(classType));
      if (!patriarchGroup || !patriarchGroup->bLoad)
      {
        continue;
      }

      ranking->SetUpdateRaceBossSerial(
        patriarchGroup->byRace,
        static_cast<unsigned __int8>(patriarchGroup->eClassType),
        0);
      g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(
        patriarchGroup->byRace,
        static_cast<unsigned __int8>(patriarchGroup->eClassType),
        emptyName);
      ranking->ApplyUpdatedBossInfo();
      patriarchGroup->_Init();
      orderProcessor->UpdatePacket(static_cast<unsigned __int8>(race), static_cast<unsigned __int8>(classType));
    }
  }

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bOper)
    {
      const unsigned __int8 raceCode = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
      g_Main.m_kEtcNotifyInfo.Notify(raceCode, player->m_ObjID.m_wIndex);
    }
  }

  return true;
}

bool PatriarchElectProcessor::CheatSetPatriarch(CPlayer *pOne, int eClass)
{
  const int raceCode = pOne->m_Param.GetRaceCode();
  if (raceCode < 0 || raceCode >= 3)
  {
    return false;
  }

  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  CandidateMgr::_candidate_info *patriarchGroup =
    candidateMgr->GetPatriarchGroup(raceCode, static_cast<CandidateMgr::_candidate_info::ClassType>(eClass));
  if (!patriarchGroup)
  {
    for (int index = 0; index < 9; ++index)
    {
      CandidateMgr::_candidate_info *candidate = &candidateMgr->m_kPatriarchGroup[raceCode][index];
      if (!candidate->bLoad)
      {
        patriarchGroup = candidate;
        break;
      }
    }

    if (!patriarchGroup)
    {
      return false;
    }
  }

  patriarchGroup->bLoad = true;
  patriarchGroup->byRace = static_cast<unsigned __int8>(raceCode);
  patriarchGroup->dwAvatorSerial = pOne->m_Param.GetCharSerial();
  patriarchGroup->dwRank = pOne->m_Param.GetPvpRank();
  patriarchGroup->dPvpPoint = pOne->m_Param.GetPvPPoint();
  patriarchGroup->byLevel = static_cast<unsigned __int8>(pOne->m_Param.GetLevel());
  patriarchGroup->eClassType = static_cast<CandidateMgr::_candidate_info::ClassType>(eClass);
  if (eClass >= 5)
  {
    patriarchGroup->eStatus = CandidateMgr::_candidate_info::candidate_appoint;
  }
  else
  {
    patriarchGroup->eStatus = CandidateMgr::_candidate_info::candidate_2st;
  }

  strcpy_s(patriarchGroup->wszName, sizeof(patriarchGroup->wszName), pOne->m_Param.GetCharNameW());
  if (pOne->m_Param.m_pGuild)
  {
    patriarchGroup->dwGuildSerial = pOne->m_Param.m_pGuild->m_dwSerial;
    strcpy_s(
      patriarchGroup->wszGuildName,
      sizeof(patriarchGroup->wszGuildName),
      pOne->m_Param.m_pGuild->m_wszName);
  }
  else
  {
    patriarchGroup->dwGuildSerial = static_cast<unsigned int>(-1);
    memset_0(patriarchGroup->wszGuildName, 0, sizeof(patriarchGroup->wszGuildName));
  }

  if (pOne->m_Param.GetPvpRank() == static_cast<unsigned int>(-1))
  {
    patriarchGroup->dwRank = 255;
  }

  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  ranking->SetUpdateRaceBossSerial(
    patriarchGroup->byRace,
    static_cast<unsigned __int8>(patriarchGroup->eClassType),
    patriarchGroup->dwAvatorSerial);
  g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(
    patriarchGroup->byRace,
    static_cast<unsigned __int8>(patriarchGroup->eClassType),
    patriarchGroup->wszName);
  ranking->ApplyUpdatedBossInfo();

  if (eClass >= 5)
  {
    ClassOrderProcessor *orderProcessor = ClassOrderProcessor::Instance();
    orderProcessor->UpdatePacket(patriarchGroup->byRace, static_cast<unsigned __int8>(eClass));
  }

  for (int index = 0; index < MAX_PLAYER; ++index)
  {
    CPlayer *player = &g_Player[index];
    if (player->m_bOper && player->m_Param.GetRaceCode() == raceCode)
    {
      const unsigned __int8 playerRace = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
      g_Main.m_kEtcNotifyInfo.Notify(playerRace, player->m_ObjID.m_wIndex);
    }
  }

  if (!eClass)
  {
    _qry_case_raceboss_accumulation_winrate qryData{};
    const int size = static_cast<int>(sizeof(qryData));
    g_Main.PushDQSData(-1, nullptr, 142, reinterpret_cast<char *>(&qryData), size);
  }

  return true;
}

void PatriarchElectProcessor::CompleteInsertElect()
{
  g_Main.PushDQSData(-1, nullptr, 122, nullptr, 0);
}

void PatriarchElectProcessor::CompleteSelectElect()
{
  _kRunningProcessor->Doit(_eRequestInitCandidate, nullptr, nullptr);
}

void PatriarchElectProcessor::CompleteRequestRefund(_DB_QRY_SYN_DATA *pData)
{
  auto *qryData = reinterpret_cast<_qry_case_request_refund *>(pData->m_sData);
  if (pData->m_byResult)
  {
    _kSysLog.Write(
      "FAILED DB_RET(%s_%d):%d",
      "Request_Refund",
      _dwElectSerial,
      qryData->dwAvatorSerial);
    return;
  }

  CPlayer *player = &g_Player[qryData->wIndex];
  if (player->m_bOper)
  {
    const unsigned int charSerial = player->m_Param.GetCharSerial();
    if (charSerial == qryData->dwAvatorSerial)
    {
      const unsigned int curDalant = player->m_Param.GetDalant();
      if (CanAddMoneyForMaxLimMoney(qryData->dwRefund, curDalant))
      {
        const signed __int64 refund = static_cast<signed __int64>(qryData->dwRefund);
        double refundValue = static_cast<double>(static_cast<int>(refund));
        if (refund < 0)
        {
          refundValue = refundValue + 1.844674407370955e19;
        }

        player->AlterDalant(refundValue);
        player->SendMsg_AlterMoneyInform(0);
        SendMsg_ResultCode(player->m_id.wIndex, 13);

        const unsigned int serial = player->m_Param.GetCharSerial();
        CPlayer::s_MgrItemHistory.raceboss_giveback(
          serial,
          qryData->dwRefund,
          player->m_szItemHistoryFileName);
        return;
      }
    }
  }

  const int size = static_cast<int>(qryData->size());
  g_Main.PushDQSData(-1, nullptr, 124, reinterpret_cast<char *>(qryData), size);
}

void PatriarchElectProcessor::CompleteItemChargeRefund(_DB_QRY_SYN_DATA *pData)
{
  if (pData->m_byResult)
  {
    auto *query = reinterpret_cast<_qry_case_request_refund *>(pData->m_sData);
    _kSysLog.Write(
      "FAILED DB_RET(%s_%d):%d-Cost:%d",
      "Item Cahrge for Refund",
      _dwElectSerial,
      query->dwAvatorSerial,
      query->dwRefund);
  }
}

void PatriarchElectProcessor::CompleteInsertPatriarch(_DB_QRY_SYN_DATA *pData)
{
  auto *query = reinterpret_cast<_qry_case_insert_candidate *>(pData->m_sData);
  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  CandidateMgr::_candidate_info *candidate =
    candidateMgr->GetPatriarchGroupBySerial(query->byRace, query->dwAvatorSerial);

  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int raceBossSerial = ranking->GetCurrentRaceBossSerial(query->byRace, 0);
  CPlayer *bossPlayer = GetPtrPlayerFromSerial(g_Player, 2532, raceBossSerial);

  if (pData->m_byResult)
  {
    _kSysLog.Write(
      "FAILED DB_RET(%s_%d):%d-Cost:%d",
      "Appoint Patriarch Insert",
      _dwElectSerial,
      query->dwAvatorSerial,
      query->byRace);

    if (candidate && bossPlayer && bossPlayer->m_bOper)
    {
      ClassOrderProcessor *order = ClassOrderProcessor::Instance();
      order->SendMsg_QueryAppointResult(
        bossPlayer->m_id.wIndex,
        6u,
        candidate->eClassType,
        candidate->wszName);
      candidate->_Init();
    }
    return;
  }

  if (!candidate)
  {
    return;
  }

  ranking->SetUpdateRaceBossSerial(candidate->byRace, candidate->eClassType, candidate->dwAvatorSerial);
  g_Main.m_kEtcNotifyInfo.UpdateRaceLeader(
    candidate->byRace,
    candidate->eClassType,
    candidate->wszName);
  ranking->ApplyUpdatedBossInfo();

  ClassOrderProcessor *order = ClassOrderProcessor::Instance();
  order->UpdatePacket(candidate->byRace, candidate->eClassType);

  for (int i = 0; i < 2532; ++i)
  {
    CPlayer *player = &g_Player[i];
    if (player->m_bOper)
    {
      if (player->m_Param.GetRaceCode() == candidate->byRace)
      {
        unsigned __int8 type[2]{56, 14};
        _pt_inform_appoint_zocl inform;
        inform.byClassType = static_cast<unsigned __int8>(candidate->eClassType) - 5;
        strcpy_0(inform.wszAvatorName, candidate->wszName);
        const unsigned __int16 size = static_cast<unsigned __int16>(inform.size());
        g_Network.m_pProcess[0]->LoadSendMsg(
          player->m_id.wIndex,
          type,
          reinterpret_cast<char *>(&inform),
          size);

        const unsigned __int8 raceCode = static_cast<unsigned __int8>(player->m_Param.GetRaceCode());
        g_Main.m_kEtcNotifyInfo.Notify(raceCode, player->m_ObjID.m_wIndex);
      }
    }
  }

  if (bossPlayer)
  {
    unsigned __int8 type[2]{56, 13};
    _pt_result_appoint_zocl result;
    result.byLevel = candidate->byLevel;
    result.byClassType = static_cast<unsigned __int8>(candidate->eClassType) - 5;
    result.dPvpPoint = candidate->dPvpPoint;
    strcpy_0(result.wszAvatorName, candidate->wszName);
    const unsigned __int16 size = static_cast<unsigned __int16>(result.size());
    g_Network.m_pProcess[0]->LoadSendMsg(
      bossPlayer->m_id.wIndex,
      type,
      reinterpret_cast<char *>(&result),
      size);
  }
}

void PatriarchElectProcessor::CompleteCheckInvalidChar(unsigned __int8 byProc)
{
  if (byProc == 1)
  {
    CandidateMgr::Instance()->ChangeState_1to2();
  }
  else if (byProc == 2)
  {
    PatriarchElectProcessor::Instance()->Doit(_eMakeVotePaper, nullptr, nullptr);
  }
  else if (byProc == 3)
  {
    PatriarchElectProcessor::Instance()->Doit(_eReqQryFinalDecision, nullptr, nullptr);
  }
}
