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
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRFWorldDatabase.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "pt_inform_appoint_zocl.h"
#include "pt_result_appoint_zocl.h"
#include "pt_result_code_zocl.h"
#include "qry_case_request_refund.h"
#include "sel_patriarch_elect_state.h"

#include <cstdio>
#include <cstring>
#include <mmsystem.h>

PatriarchElectProcessor *PatriarchElectProcessor::Instance()
{
  static PatriarchElectProcessor s_instance;
  return &s_instance;
}

bool PatriarchElectProcessor::Initialize()
{
  char buffer[260];

  CreateDirectoryA("..\\ZoneServerLog\\Systemlog\\Patriarch", nullptr);
  clear_file("..\\ZoneServerLog\\Systemlog\\Patriarch", 0x1E);

  memset(buffer, 0, 256);
  const unsigned int now = GetKorLocalTime();
  sprintf_s(buffer, 0x100u, "..\\ZoneServerLog\\SystemLog\\Patriarch\\PatriarchElect_%d.log", now);
  _kSysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog\\Patriarch", nullptr);
  clear_file("..\\ZoneServerLog\\ServiceLog\\Patriarch", 0x1E);

  CandidateRegister *candidate = static_cast<CandidateRegister *>(operator new(0x11438uLL));
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
    static_cast<SecondCandidateCrystallizer *>(operator new(0xC8uLL));
  if (secondCandidate)
  {
    new (secondCandidate) SecondCandidateCrystallizer();
  }
  else
  {
    secondCandidate = nullptr;
  }
  _kProcessor[1] = secondCandidate;

  Voter *voter = static_cast<Voter *>(operator new(0x648uLL));
  if (voter)
  {
    new (voter) Voter();
  }
  else
  {
    voter = nullptr;
  }
  _kProcessor[2] = voter;

  FinalDecisionProcessor *finalDecision = static_cast<FinalDecisionProcessor *>(operator new(0x1C8uLL));
  if (finalDecision)
  {
    new (finalDecision) FinalDecisionProcessor();
  }
  else
  {
    finalDecision = nullptr;
  }
  _kProcessor[3] = finalDecision;

  FinalDecisionApplyer *finalApplyer = static_cast<FinalDecisionApplyer *>(operator new(0xC8uLL));
  if (finalApplyer)
  {
    new (finalApplyer) FinalDecisionApplyer();
  }
  else
  {
    finalApplyer = nullptr;
  }
  _kProcessor[4] = finalApplyer;

  ClassOrderProcessor *classOrder = static_cast<ClassOrderProcessor *>(operator new(0x210uLL));
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

unsigned int PatriarchElectProcessor::GetCurrPatriarchElectSerial()
{
  return _dwCurrPatriarchElectSerial;
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

void PatriarchElectProcessor::SendMsg_ResultCode(unsigned int n, unsigned __int8 byCode)
{
  _pt_result_code_zocl msg{};
  msg.byRetCode = byCode;

  unsigned __int8 type[2]{56, static_cast<unsigned __int8>(-1)};
  const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), len);
}

unsigned int PatriarchElectProcessor::GetElectSerial()
{
  return _dwElectSerial;
}

ElectProcessor::ProcessorType PatriarchElectProcessor::GetProcessorType()
{
  return _eProcessType;
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
  char buffer[272]{};
  sprintf(
    buffer,
    "{ CALL pUpdate_Patriarch_PayRefund ( %d, %d ) }",
    static_cast<unsigned __int8>(*pData),
    *reinterpret_cast<unsigned int *>(pData + 4));

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

__int64 PatriarchElectProcessor::Insert_PatrirchItemChargeRefund(char *pData)
{
  char buffer[272]{};
  sprintf(
    buffer,
    "insert [dbo].[tbl_itemcharge] (nAvatorSerial, nItemCode_K, nItemCode_D, nItemCode_U, dtGiveDate, dtTakeDate, Type) "
    "values (%d, 0, %d, default, default, default, 1)",
    *reinterpret_cast<unsigned int *>(pData + 4),
    *reinterpret_cast<unsigned long long *>(pData + 8));

  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, true) ? 0 : 24;
}

void PatriarchElectProcessor::CompleteInsertElect()
{
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x7Au, nullptr, 0);
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
        SendMsg_ResultCode(player->m_id.wIndex, 0xDu);

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
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x7Cu, reinterpret_cast<char *>(qryData), size);
}

void PatriarchElectProcessor::CompleteItemChargeRefund(_DB_QRY_SYN_DATA *pData)
{
  if (pData->m_byResult)
  {
    char *data = pData->m_sData;
    _kSysLog.Write(
      "FAILED DB_RET(%s_%d):%d-Cost:%d",
      "Item Cahrge for Refund",
      _dwElectSerial,
      *reinterpret_cast<unsigned int *>(data + 4),
      *reinterpret_cast<unsigned long long *>(data + 8));
  }
}

void PatriarchElectProcessor::CompleteInsertPatriarch(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  CandidateMgr::_candidate_info *candidate =
    candidateMgr->GetPatriarchGroupBySerial(static_cast<unsigned __int8>(*data), *reinterpret_cast<unsigned int *>(data + 8));

  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int raceBossSerial =
    ranking->GetCurrentRaceBossSerial(static_cast<unsigned __int8>(*data), 0);
  CPlayer *bossPlayer = GetPtrPlayerFromSerial(g_Player, 2532, raceBossSerial);

  if (pData->m_byResult)
  {
    _kSysLog.Write(
      "FAILED DB_RET(%s_%d):%d-Cost:%d",
      "Appoint Patriarch Insert",
      _dwElectSerial,
      *reinterpret_cast<unsigned int *>(data + 8),
      static_cast<unsigned __int8>(*data));

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
