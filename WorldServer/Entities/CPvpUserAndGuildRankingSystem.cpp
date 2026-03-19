#include "pch.h"

#include "GlobalObjects.h"

#include "CPvpUserAndGuildRankingSystem.h"

#include "KorLocalTime.h"
#include "WorldServerUtil.h"

#include <cstdarg>
#include <cstdio>

CPvpUserAndGuildRankingSystem *CPvpUserAndGuildRankingSystem::ms_Instance = nullptr;

CPvpUserAndGuildRankingSystem::CPvpUserAndGuildRankingSystem() = default;

CPvpUserAndGuildRankingSystem::~CPvpUserAndGuildRankingSystem() = default;

CPvpUserAndGuildRankingSystem *CPvpUserAndGuildRankingSystem::Instance()
{
  if (!ms_Instance)
  {
    ms_Instance = new CPvpUserAndGuildRankingSystem();
  }
  return ms_Instance;
}

void CPvpUserAndGuildRankingSystem::Destroy()
{
  if (ms_Instance)
  {
    delete ms_Instance;
    ms_Instance = nullptr;
  }
}

void CPvpUserAndGuildRankingSystem::Loop()
{
  if (g_Main.m_bWorldOpen)
  {
    m_kUserRankingProcess.Loop();
  }
}

bool CPvpUserAndGuildRankingSystem::InitLogger()
{
  m_pkLogger = new CLogFile();
  if (m_pkLogger == nullptr)
  {
    g_Main.m_logLoadingError.Write(
      "CPvpUserAndGuildRankingSystem::InitLogger() NULL == new CLogFile!\r\n");
    return false;
  }

  char path[1024]{};
  const unsigned int now = static_cast<unsigned int>(GetKorLocalTime());
  sprintf_s(path, sizeof(path), "..\\ZoneServerLog\\Systemlog\\RankJob%u.log", now);
  m_pkLogger->SetWriteLogFile(path, 1, 0, 1, 1);
  m_kUserRankingProcess.SetLogger(m_pkLogger);
  return true;
}

bool CPvpUserAndGuildRankingSystem::Init()
{
  return InitLogger() && m_kUserRankingProcess.Init();
}

bool CPvpUserAndGuildRankingSystem::Load()
{
  return m_kUserRankingProcess.Load();
}

bool CPvpUserAndGuildRankingSystem::IsCurrentRaceBossGroup(unsigned __int8 byRace, unsigned int dwSerial)
{
  return m_kUserRankingProcess.IsCurrentRaceBossGroup(byRace, dwSerial);
}

unsigned int CPvpUserAndGuildRankingSystem::GetCurrentRaceBossSerial(unsigned __int8 byRace, unsigned __int8 byNth)
{
  return m_kUserRankingProcess.GetCurrentRaceBossSerial(byRace, byNth);
}

void CPvpUserAndGuildRankingSystem::SetCurrentRaceBossSerial(
  unsigned __int8 byRace,
  unsigned __int8 byNth,
  unsigned int dwSerial)
{
  m_kUserRankingProcess.SetCurrentRaceBossSerial(byRace, byNth, dwSerial);
}

unsigned int CPvpUserAndGuildRankingSystem::FindRank(unsigned __int8 byRaceCode, unsigned int dwAvatorSerial)
{
  return m_kUserRankingProcess.FindRank(byRaceCode, dwAvatorSerial);
}

const _PVP_RANK_DATA *CPvpUserAndGuildRankingSystem::GetCurrentPvpRankData(unsigned __int8 byRace, unsigned __int8 byNth)
{
  return m_kUserRankingProcess.GetCurrentPvpRankData(byRace, byNth);
}

void CPvpUserAndGuildRankingSystem::PvpRankListRequest(
  unsigned __int16 wIndex,
  unsigned __int8 byRace,
  unsigned __int8 byVersion,
  unsigned __int8 byPage)
{
  m_kUserRankingProcess.PvpRankListRequest(wIndex, byRace, byVersion, byPage);
}

bool CPvpUserAndGuildRankingSystem::IsRaceViceBoss(unsigned __int8 byRace, unsigned int dwSerial)
{
  return m_kUserRankingProcess.IsRaceViceBoss(byRace, dwSerial);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::GetBossType(unsigned __int8 byRace, unsigned int dwSerial)
{
  return m_kUserRankingProcess.GetBossType(byRace, dwSerial);
}

void CPvpUserAndGuildRankingSystem::Log(const char *fmt, ...)
{
  if (!m_pkLogger)
  {
    return;
  }

  va_list va;
  va_start(va, fmt);
  m_pkLogger->WriteFromArg(fmt, va);
  va_end(va);
}

void CPvpUserAndGuildRankingSystem::SetUpdateRaceBossSerial(
  unsigned __int8 byRace,
  unsigned __int8 byNth,
  unsigned int dwSerial)
{
  m_kUserRankingProcess.SetUpdateRaceBossSerial(byRace, byNth, dwSerial);
}

void CPvpUserAndGuildRankingSystem::PvpRankDataPacking()
{
  m_kUserRankingProcess.PvpRankDataPacking();
}

void CPvpUserAndGuildRankingSystem::ApplyUpdatedBossInfo()
{
  m_kUserRankingProcess.FlipPvPRankTop();
  m_kUserRankingProcess.PvpRankDataPacking();
  m_kUserRankingProcess.IncreaseVesion();
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep1(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep1(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep2(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep2(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep3(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep3(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep4(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep4(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep5(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep5(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep6(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep6(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep7(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep7(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep8(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep8(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep9(char *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep9(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep10(_qry_case_rank_racerank_guildrank *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep10(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRaceRankStep11(_qry_case_rank_racerank_guildrank *szData)
{
  return m_kUserRankingProcess.UpdateRaceRankStep11(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateGuildRankStep1(char *szData)
{
  return m_kUserRankingProcess.UpdateGuildRankStep1(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateGuildRankStep2(char *szData)
{
  return m_kUserRankingProcess.UpdateGuildRankStep2(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateGuildRankStep3(char *szData)
{
  return m_kUserRankingProcess.UpdateGuildRankStep3(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateGuildRankStep4(char *szData)
{
  return m_kUserRankingProcess.UpdateGuildRankStep4(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRankinGuildStep1(char *szData)
{
  return m_kUserRankingProcess.UpdateRankinGuildStep1(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRankinGuildStep2(char *szData)
{
  return m_kUserRankingProcess.UpdateRankinGuildStep2(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRankinGuildStep3(char *szData)
{
  return m_kUserRankingProcess.UpdateRankinGuildStep3(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRankinGuildStep4(char *szData)
{
  return m_kUserRankingProcess.UpdateRankinGuildStep4(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRankinGuildStep5(char *szData)
{
  return m_kUserRankingProcess.UpdateRankinGuildStep5(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateRankinGuildStep6(char *szData)
{
  return m_kUserRankingProcess.UpdateRankinGuildStep6(szData);
}

unsigned __int8 CPvpUserAndGuildRankingSystem::UpdateAndSelectGuildGrade(char *szData)
{
  return m_kUserRankingProcess.UpdateAndSelectGuildGrade(szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep1(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep1(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep2(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep2(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep3(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep3(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep4(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep4(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep5(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep5(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep6(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep6(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep7(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep7(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep8(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep8(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep9(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep9(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep10(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep10(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRaceRankStep11(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteRaceRankStep11(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteGuildRankStep1(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteGuildRankStep1(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteGuildRankStep2(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteGuildRankStep2(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteGuildRankStep3(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteGuildRankStep3(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteGuildRankStep4(unsigned __int8 byRet, char *szData)
{
  m_kUserRankingProcess.CompleteGuildRankStep4(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankInGuildStep1(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankInGuildStep1(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankInGuildStep2(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankInGuildStep2(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankInGuildStep3(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankInGuildStep3(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankInGuildStep4(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankInGuildStep4(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankInGuildStep5(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankInGuildStep5(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankInGuildStep6(unsigned __int8 byRet, _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankInGuildStep6(byRet, szData);
}

void CPvpUserAndGuildRankingSystem::CompleteRankUpdateAndSelectGarde(
  unsigned __int8 byRet,
  _qry_case_rank_racerank_guildrank *szData)
{
  m_kUserRankingProcess.CompleteRankUpdateAndSelectGarde(byRet, szData);
}
