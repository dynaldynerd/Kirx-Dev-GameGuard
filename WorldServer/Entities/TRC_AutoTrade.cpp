#include "pch.h"

#include "TRC_AutoTrade.h"
#include "atrade_taxrate_result_zocl.h"
#include "insert_trc_info.h"
#include "pt_inform_tax_rate_zocl.h"
#include "CNetProcess.h"
#include "KorLocalTime.h"
#include "GlobalObjects.h"
#include "CMgrGuildHistory.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CNotifyNotifyRaceLeaderSownerUTaxrate.h"
#include "CPlayer.h"
#include "CHonorGuild.h"
#include "DqsDbStructs.h"
#include "qry_case_in_atrade_tax.h"

_suggested_matter_change_taxrate::_suggested_matter_change_taxrate()
{
  init();
}

void _suggested_matter_change_taxrate::init()
{
  std::memset(this, 0, sizeof(_suggested_matter_change_taxrate));
  this->dwNext = 5;
}

TRC_AutoTrade::TRC_AutoTrade()
  : m_bInit(false),
    m_byCurDay(0),
    m_wCurMonth(0),
    m_wCurYear(0),
    m_bChangeTaxRate(false),
    m_fCommonTaxRate(0.050000001f),
    m_pOwnerGuild(nullptr),
    m_Controller(),
    m_sysLog(),
    m_serviceLog(),
    m_dwTrade(0),
    m_dIncomeMoney(0.0),
    m_byRace(0),
    m_suggested()
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  this->m_byCurDay = systemTime.wDay;
  this->m_wCurMonth = systemTime.wMonth;
  this->m_wCurYear = systemTime.wYear;
}

TRC_AutoTrade::TRC_AutoTrade(unsigned char byRace)
  : m_bInit(false),
    m_byCurDay(0),
    m_wCurMonth(0),
    m_wCurYear(0),
    m_bChangeTaxRate(false),
    m_fCommonTaxRate(0.050000001f),
    m_pOwnerGuild(nullptr),
    m_Controller(),
    m_sysLog(),
    m_serviceLog(),
    m_dwTrade(0),
    m_dIncomeMoney(0.0),
    m_byRace(byRace),
    m_suggested()
{
  SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  this->m_byCurDay = systemTime.wDay;
  this->m_wCurMonth = systemTime.wMonth;
  this->m_wCurYear = systemTime.wYear;
}

bool TRC_AutoTrade::Initialzie()
{
  CreateDirectoryA("..\\ZoneServerLog\\ServiceLog\\ATradeTax", nullptr);
  CreateDirectoryA("..\\ZoneServerLog\\SystemLog\\Concession", nullptr);

  char buffer[144]{};
  const unsigned int korLocalTime = GetKorLocalTime();
  sprintf_s(buffer, "..\\ZoneServerLog\\ServiceLog\\ATradeTax\\atrade_earn_%d_%u.log", this->m_byRace, korLocalTime);
  this->m_serviceLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  const unsigned int korLocalTime2 = GetKorLocalTime();
  sprintf_s(buffer, "..\\ZoneServerLog\\SystemLog\\Concession\\system_TRC_%d_%u.log", this->m_byRace, korLocalTime2);
  this->m_sysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  this->m_Controller.setLimitTaxRate(0.050000001f, 0.2f);
  return true;
}

__int64 TRC_AutoTrade::check(unsigned int dwAvatorSerial, unsigned int dwGuildSerial)
{
  if (!IsOwnerGuild(dwGuildSerial))
  {
    return 1;
  }

  if (IsMaster(dwAvatorSerial))
  {
    return 0;
  }

  return 2;
}

bool TRC_AutoTrade::IsOwnerGuild(unsigned int nGuildSerial)
{
  return m_pOwnerGuild && nGuildSerial == m_pOwnerGuild->m_dwSerial;
}

bool TRC_AutoTrade::IsMaster(unsigned int dwSerial)
{
  return m_pOwnerGuild && dwSerial == m_pOwnerGuild->m_MasterData.dwSerial;
}

float TRC_AutoTrade::get_taxrate()
{
  return m_Controller.getCurTaxRate();
}

float TRC_AutoTrade::get_next_tax()
{
  return static_cast<float>(static_cast<int>(this->m_suggested.dwNext));
}

unsigned __int8 TRC_AutoTrade::get_race()
{
  return m_pOwnerGuild->m_byRace;
}

char *TRC_AutoTrade::get_guidlname()
{
  if (m_pOwnerGuild)
  {
    return m_pOwnerGuild->m_wszName;
  }

  return nullptr;
}

CGuild *TRC_AutoTrade::getOwnerGuild()
{
  return m_pOwnerGuild;
}

unsigned int TRC_AutoTrade::getSuggestedTime()
{
  return this->m_suggested.dwSuggestedTime;
}

void TRC_AutoTrade::set_suggested(
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *wszMatterDst,
  unsigned int dwNext)
{
  this->m_suggested.init();
  this->m_suggested.byMatterType = byMatterType;
  this->m_suggested.dwMatterDst = dwMatterDst;
  std::strcpy(this->m_suggested.wszMatterDst, wszMatterDst);

  if (dwNext < 5 || dwNext > 20)
  {
    dwNext = 5;
  }

  this->m_suggested.dwNext = dwNext;
  this->m_bChangeTaxRate = true;
  this->m_suggested.dwSuggestedTime = GetKorLocalTime();
  this->m_serviceLog.Write("[Suggest Change Tax Rate]:[SUBPATRIARCH:%s] - %d(%%)", wszMatterDst, dwNext);
  this->PushDQSData();
}

void TRC_AutoTrade::ChangeTaxRate()
{
  if (!m_bInit)
  {
    return;
  }

  _SYSTEMTIME systemTime{};
  GetLocalTime(&systemTime);
  if (m_byCurDay < systemTime.wDay || m_wCurMonth < systemTime.wMonth || m_wCurYear < systemTime.wYear)
  {
    if (m_bChangeTaxRate)
    {
      ChangeTaxRate(static_cast<float>(static_cast<int>(m_suggested.dwNext)) / 100.0f);
    }

    m_byCurDay = systemTime.wDay;
    m_wCurMonth = systemTime.wMonth;
    m_wCurYear = systemTime.wYear;
    his_income_money();

    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned int serial = ranking->GetCurrentRaceBossSerial(m_byRace, 0);
    CPlayer *raceBoss = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, serial);
    if (raceBoss && raceBoss->m_bOper)
    {
      char payload[4]{};
      *reinterpret_cast<unsigned int *>(payload) = raceBoss->m_Param.GetCharSerial();
      g_Main.PushDQSData(-1, nullptr, 130, payload, sizeof(payload));
    }

    CHonorGuild::Instance()->ChangeHonorGuild(m_byRace);
  }
}

__int64 TRC_AutoTrade::ChangeTaxRate(float fNewTaxRate)
{
  if (!m_Controller.checkLimitTaxRate(fNewTaxRate))
  {
    m_serviceLog.Write("[ChangeTaxRate]:Invalid value :: (%.2f)", fNewTaxRate);
    return 3;
  }

  if (m_Controller.getCurTaxRate() != fNewTaxRate)
  {
    const float currentTaxRate = m_Controller.getCurTaxRate();
    m_Controller.setCurTaxRate(fNewTaxRate);
    m_serviceLog.Write("[ChangeTaxRate]:Current(%.2f) ==> Next(%.2f)", currentTaxRate, fNewTaxRate);
  }

  PushDQSData();
  m_bChangeTaxRate = false;

  const float taxRate = get_taxrate();
  g_Main.m_kEtcNotifyInfo.UpdateTaxRate(m_byRace, static_cast<int>(taxRate * 100.0f));
  g_Main.m_kEtcNotifyInfo.Notify(m_byRace);

  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int patriarchSerial = ranking->GetCurrentRaceBossSerial(m_byRace, 5u);
  CPlayer *patriarch = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, patriarchSerial);
  if (patriarch)
  {
    SendMsg_PatriarchTaxRate(patriarch->m_ObjID.m_wIndex);
  }

  return 0;
}

void TRC_AutoTrade::his_income_money()
{
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  const unsigned int serial = ranking->GetCurrentRaceBossSerial(m_byRace, 0);
  const float currentTaxRate = m_Controller.getCurTaxRate();
  m_serviceLog.Write(
    "PS:%d tax:%.2f tcnt:%d money:%.0f",
    serial,
    currentTaxRate,
    m_dwTrade,
    static_cast<double>(m_dIncomeMoney));
  m_dIncomeMoney = 0.0;
  m_dwTrade = 0;
}

void TRC_AutoTrade::PushDQSData()
{
  _insert_trc_info query{};
  query.byRace = this->m_byRace;
  query.byMatterType = this->m_suggested.byMatterType;
  query.dwMatterDst = this->m_suggested.dwMatterDst;
  std::strcpy(query.wszMatterDst, this->m_suggested.wszMatterDst);
  query.dwGSerial = static_cast<unsigned int>(-1);
  std::strcpy(query.szGuildName, "*");

  if (this->m_suggested.dwNext < 5 || this->m_suggested.dwNext > 20)
  {
    this->m_suggested.dwNext = 5;
  }

  query.dwNext = this->m_suggested.dwNext;
  query.byCurrTax = static_cast<unsigned __int8>(this->get_taxrate() * 100.0f);
  const int size = static_cast<int>(query.size());
  g_Main.PushDQSData(-1, nullptr, 51, reinterpret_cast<char *>(&query), size);
}

void TRC_AutoTrade::PushDQSData_GuildInMoney(unsigned int dwRetPrice, unsigned int dwSeller)
{
  if (!m_pOwnerGuild)
  {
    return;
  }

  _qry_case_in_atrade_tax query{};
  query.byRace = m_byRace;
  query.dwGuildSerial = m_pOwnerGuild->m_dwSerial;
  query.in_seller = dwSeller;
  query.out_totalgold = 0.0;
  query.out_totaldalant = 0.0;
  query.in_dalant = dwRetPrice;
  g_Main.PushDQSData(
    -1,
    nullptr,
    52,
    reinterpret_cast<char *>(&query),
    static_cast<int>(query.size()));
}

void TRC_AutoTrade::sendmsg_taxrate(unsigned int n, unsigned __int8 byRet)
{
_atrade_taxrate_result_zocl result{};
  result.byTaxRate = static_cast<unsigned __int8>(get_taxrate() * 100.0f);

  if (m_pOwnerGuild)
  {
    result.dwEmblemBack = m_pOwnerGuild->m_dwEmblemBack;
    result.dwEmblemMark = m_pOwnerGuild->m_dwEmblemMark;
    std::memcpy(result.wszGuildName, m_pOwnerGuild->m_wszName, sizeof(result.wszGuildName));
  }
  else
  {
    result.dwEmblemBack = static_cast<unsigned int>(-1);
    result.dwEmblemMark = static_cast<unsigned int>(-1);
  }

  unsigned __int8 type[2]{30, 24};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), len);
}

void TRC_AutoTrade::SendMsg_PatriarchTaxRate(unsigned int n)
{
  _pt_inform_tax_rate_zocl result{};
  result.byCurrTax = static_cast<unsigned __int8>(this->get_taxrate() * 100.0f);
  result.byNextTax = static_cast<unsigned __int8>(this->get_next_tax());

  unsigned __int8 type[2]{13, 119};
  const unsigned __int16 len = static_cast<unsigned __int16>(result.size());
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&result), len);
}

void TRC_AutoTrade::SendMsg_UserLogInNotifyTaxRate(unsigned int n)
{
  _unmannedtrader_taxrate_inform_zocl msg{};
  msg.byTaxRate = 1;
  unsigned __int8 type[2] = {30, 39};
  g_Network.m_pProcess[0]->LoadSendMsg(
    n,
    type,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
}

void TRC_AutoTrade::AddGDalant(char *pdata)
{
  const auto *query = reinterpret_cast<const _qry_case_in_atrade_tax *>(pdata);
  if (m_pOwnerGuild && m_pOwnerGuild->m_dwSerial == query->dwGuildSerial)
  {
    m_pOwnerGuild->m_dTotalDalant = query->out_totaldalant;
    m_pOwnerGuild->MakeDownMemberPacket();
    m_pOwnerGuild->m_byMoneyOutputKind = 6;
    m_pOwnerGuild->SendMsg_IOMoney(
      query->in_seller,
      static_cast<double>(query->in_dalant),
      0.0,
      true,
      const_cast<unsigned __int8 *>(query->byDate));
    CGuild::s_MgrHistory.push_money(
      "Auto Trade Tax",
      query->in_seller,
      query->in_dalant,
      0,
      query->out_totaldalant,
      query->out_totalgold,
      m_pOwnerGuild->m_szHistoryFileName);
  }
  else
  {
    m_sysLog.Write(
      "Failed TRC_AutoTrade::AddGDalant(GuildSerial:%d,seller:%d,in:@%d,total:@%.0f)",
      query->dwGuildSerial,
      query->in_seller,
      query->in_dalant,
      static_cast<double>(query->out_totaldalant));
  }
}

unsigned int TRC_AutoTrade::CalcPrice(unsigned int nGuildSerial, unsigned int nPrice)
{
return m_Controller.calcTaxRate(nPrice);
}

int TRC_AutoTrade::ChangeOwner(CGuild *pGuild)
{
  if (!pGuild)
  {
    return 4;
  }
  if (m_pOwnerGuild && m_pOwnerGuild->m_dwSerial == pGuild->m_dwSerial)
  {
    return 0;
  }

  CGuild *prevOwner = m_pOwnerGuild;
  m_pOwnerGuild = pGuild;
  if (m_pOwnerGuild)
  {
    m_pOwnerGuild->MakeDownMemberPacket();
  }
  if (prevOwner)
  {
    prevOwner->MakeDownMemberPacket();
  }
  return 0;
}

void TRC_AutoTrade::history_used_cheet_changetaxrate(unsigned int dwProb, char *pName)
{
  m_serviceLog.Write("[ChangeTaxRate]:Used cheat code(changetaxrate) :: %s(%d)", pName, dwProb);
}

void TRC_AutoTrade::SetGuildMaintainMoney(int dwTax, unsigned int dwSeller)
{
  if (m_pOwnerGuild)
  {
    PushDQSData_GuildInMoney(static_cast<unsigned int>(dwTax), dwSeller);
  }

  ++m_dwTrade;
  m_dIncomeMoney = m_dIncomeMoney + static_cast<double>(dwTax);
}

void TRC_AutoTrade::SetPatriarchTaxMoney(int dwTax)
{
  _qry_case_insert_patriarch_comm qry{};
  qry.dwDalant = static_cast<unsigned int>(dwTax);
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  qry.dwSerial = ranking->GetCurrentRaceBossSerial(m_byRace, 0);
  GetTodayStr(qry.szDepDate);
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(-1, nullptr, 131, reinterpret_cast<char *>(&qry), size);
  ++m_dwTrade;
  m_dIncomeMoney = m_dIncomeMoney + static_cast<double>(dwTax);
}

void TRC_AutoTrade::set_owner(CGuild *pGuild)
{
  m_pOwnerGuild = pGuild;
}

char TRC_AutoTrade::_db_load(unsigned __int8 byRace)
{
  unsigned __int8 byCurrTax[32]{};
  unsigned __int8 byNextTax[36]{};
  char pwszName[28]{};

  const int result = g_Main.m_pWorldDB->select_atrade_taxrate(byRace, pwszName, byCurrTax, byNextTax);
  if (result == 1)
  {
    return 0;
  }

  if (byCurrTax[0] < 5u || byCurrTax[0] > 20)
  {
    byCurrTax[0] = 5;
  }
  if (byNextTax[0] < 5u || byNextTax[0] > 20)
  {
    byNextTax[0] = 5;
  }

  m_Controller.setCurTaxRate(static_cast<float>(byCurrTax[0]) / 100.0f);
  m_serviceLog.Write(
    "Init tax rate : Suggester:%s tax: %.2f",
    pwszName,
    static_cast<float>(byCurrTax[0]) / 100.0f);

  m_suggested.dwNext = byNextTax[0];
  std::strcpy(m_suggested.wszMatterDst, pwszName);
  g_Main.m_kEtcNotifyInfo.UpdateTaxRate(byRace, byCurrTax[0]);
  m_bInit = true;
  return 1;
}

unsigned __int8 TRC_AutoTrade::_insert_info(char *pdata)
{
  const auto *info = reinterpret_cast<const _insert_trc_info *>(pdata);
  if (g_Main.m_pWorldDB->insert_atrade_taxrate(
        info->byRace,
        info->dwGSerial,
        const_cast<char *>(info->szGuildName),
        info->dwMatterDst,
        const_cast<char *>(info->wszMatterDst),
        info->byCurrTax,
        info->dwNext))
  {
    return 0;
  }
  return 24;
}

