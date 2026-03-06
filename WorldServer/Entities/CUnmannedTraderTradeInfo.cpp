#include "pch.h"

#include "CUnmannedTraderTradeInfo.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CandidateMgr.h"
#include "CNationSettingManager.h"
#include "CPlayer.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"

#include <new>
#include <windows.h>

CUnmannedTraderTradeInfo::CUnmannedTraderTradeInfo()
  : m_pkTimer(nullptr),
    m_iOldDay(0),
    m_bNeedUpdateSave(false),
    m_ui64TotalOldIncome(0),
    m_ui64TotalCurrentIncome(0)
{
}

CUnmannedTraderTradeInfo::~CUnmannedTraderTradeInfo()
{
  if (m_pkTimer)
  {
    delete m_pkTimer;
    m_pkTimer = nullptr;
  }
  m_ui64TotalOldIncome = 0;
  m_ui64TotalCurrentIncome = 0;
}

char CUnmannedTraderTradeInfo::Init()
{
  m_pkTimer = new (std::nothrow) CMyTimer();
  if (!m_pkTimer)
  {
    return 0;
  }

  m_pkTimer->BeginTimer(CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_LOOP_DELAY);
  m_iOldDay = GetCurrentDay();
  LoadINI();
  return 1;
}

void CUnmannedTraderTradeInfo::Loop()
{
  if (m_pkTimer && m_pkTimer->CountingTimer())
  {
    if (IsDayChanged(&m_iOldDay))
    {
      UpdateIncome();
      NotifyIncome();
    }
    SaveINI();
  }
}

void CUnmannedTraderTradeInfo::LoadINI()
{
  char buffer[5136]{};
  GetPrivateProfileStringA(
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_SECTION_NAME,
    "OldIncome",
    "0",
    buffer,
    0x1400u,
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_FILE_NAME);
  m_ui64TotalOldIncome = _strtoui64(buffer, nullptr, 10);

  GetPrivateProfileStringA(
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_SECTION_NAME,
    "CurrentIncome",
    "0",
    buffer,
    0x1400u,
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_FILE_NAME);
  m_ui64TotalCurrentIncome = _strtoui64(buffer, nullptr, 10);
}

void CUnmannedTraderTradeInfo::SaveINI()
{
  if (!m_bNeedUpdateSave)
  {
    return;
  }

  char buffer[5136]{};
  _ui64toa_s(m_ui64TotalOldIncome, buffer, 0x1400u, 10);
  WritePrivateProfileStringA(
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_SECTION_NAME,
    "OldIncome",
    buffer,
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_FILE_NAME);

  _ui64toa_s(m_ui64TotalCurrentIncome, buffer, 0x1400u, 10);
  WritePrivateProfileStringA(
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_SECTION_NAME,
    "CurrentIncome",
    buffer,
    CUnmannedTraderEnvironmentValue::UNMANNEDTRADETRADEINFO_SAVE_INI_FILE_NAME);

  m_bNeedUpdateSave = false;
}

void CUnmannedTraderTradeInfo::UpdateIncome()
{
  m_ui64TotalOldIncome = m_ui64TotalCurrentIncome;
  if (GetCurwDay() == 1)
  {
    m_ui64TotalCurrentIncome = 0;
  }
  m_bNeedUpdateSave = true;
}

void CUnmannedTraderTradeInfo::NotifyIncome()
{
  unsigned __int8 type[2] = {13, 102};
  _notify_unmanned_trader_weekly_daily_income_zocl msg[3]{};

  for (int j = 0; j < 3; ++j)
  {
    const CandidateMgr::_candidate_info *patriarchInfo = nullptr;
    CandidateMgr *candidateMgr = CandidateMgr::Instance();
    patriarchInfo = candidateMgr->GetPatriarchGroup(static_cast<unsigned __int8>(j), CandidateMgr::_candidate_info::patriarch);

    const char *source = nullptr;
    if (patriarchInfo)
    {
      source = patriarchInfo->wszName;
    }
    else
    {
      CNationSettingManager *settingManager = CTSingleton<CNationSettingManager>::Instance();
      source = settingManager->GetNoneString();
    }

    msg[j].ui64TotalIncome = m_ui64TotalOldIncome;
    strncpy_s(msg[j].wszRaceLeaderName, sizeof(msg[j].wszRaceLeaderName), source, 0x10u);
  }

  for (int k = 0; k < MAX_PLAYER; ++k)
  {
    CPlayer *player = &g_Player[k];
    if (player->m_bLive)
    {
      const int raceCode = player->m_Param.GetRaceCode();
      g_Network.m_pProcess[0]->LoadSendMsg(
        player->m_ObjID.m_wIndex,
        type,
        reinterpret_cast<char *>(&msg[raceCode]),
        static_cast<unsigned __int16>(sizeof(_notify_unmanned_trader_weekly_daily_income_zocl)));
    }
  }
}

void CUnmannedTraderTradeInfo::NotifyIncome(unsigned __int8 byRace, unsigned __int16 wIndex)
{
  unsigned __int8 type[2] = {13, 102};
  _notify_unmanned_trader_weekly_daily_income_zocl msg{};

  CandidateMgr *candidateMgr = CandidateMgr::Instance();
  const CandidateMgr::_candidate_info *patriarchInfo =
    candidateMgr->GetPatriarchGroup(byRace, CandidateMgr::_candidate_info::patriarch);

  const char *source = nullptr;
  if (patriarchInfo)
  {
    source = patriarchInfo->wszName;
  }
  else
  {
    CNationSettingManager *settingManager = CTSingleton<CNationSettingManager>::Instance();
    source = settingManager->GetNoneString();
  }

  msg.ui64TotalIncome = m_ui64TotalOldIncome;
  strncpy_s(msg.wszRaceLeaderName, sizeof(msg.wszRaceLeaderName), source, 0x10u);

  g_Network.m_pProcess[0]->LoadSendMsg(
    wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void CUnmannedTraderTradeInfo::AddIncome(unsigned int dwOriPrice)
{
  this->m_ui64TotalCurrentIncome += dwOriPrice;
  this->m_bNeedUpdateSave = true;
}
