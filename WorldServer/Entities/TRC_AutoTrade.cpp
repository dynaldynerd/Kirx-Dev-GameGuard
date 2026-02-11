#include "pch.h"

#include "TRC_AutoTrade.h"
#include "KorLocalTime.h"
#include "GlobalObjects.h"
#include "CMgrGuildHistory.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "DqsDbStructs.h"
#include "pt_inform_tax_rate_zocl.h"

_suggested_matter_change_taxrate::_suggested_matter_change_taxrate()
{
  init();
}

void _suggested_matter_change_taxrate::init()
{
  memset_0(this, 0, sizeof(_suggested_matter_change_taxrate));
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
  sprintf(buffer, "..\\ZoneServerLog\\ServiceLog\\ATradeTax\\atrade_earn_%d_%d.log", this->m_byRace, korLocalTime);
  this->m_serviceLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  const unsigned int korLocalTime2 = GetKorLocalTime();
  sprintf(buffer, "..\\ZoneServerLog\\SystemLog\\Concession\\system_TRC_%d_%d.log", this->m_byRace, korLocalTime2);
  this->m_sysLog.SetWriteLogFile(buffer, 1, 0, 1, 1);

  this->m_Controller.setLimitTaxRate(0.050000001f, 0.2f);
  return true;
}

bool TRC_AutoTrade::IsOwnerGuild(unsigned int nGuildSerial)
{
  return m_pOwnerGuild && nGuildSerial == m_pOwnerGuild->m_dwSerial;
}

float TRC_AutoTrade::get_taxrate()
{
  return m_Controller.getCurTaxRate();
}

float TRC_AutoTrade::get_next_tax()
{
  return static_cast<float>(static_cast<int>(m_suggested.dwNext));
}

unsigned int TRC_AutoTrade::getSuggestedTime()
{
  return m_suggested.dwSuggestedTime;
}

void TRC_AutoTrade::AddGDalant(char *pdata)
{
  char *data = pdata;
  if (m_pOwnerGuild && m_pOwnerGuild->m_dwSerial == *reinterpret_cast<unsigned int *>(data + 4))
  {
    m_pOwnerGuild->m_dTotalDalant = *reinterpret_cast<long double *>(data + 16);
    m_pOwnerGuild->MakeDownMemberPacket();
    m_pOwnerGuild->m_byMoneyOutputKind = 6;
    m_pOwnerGuild->SendMsg_IOMoney(
      *reinterpret_cast<unsigned int *>(data + 8),
      static_cast<double>(*reinterpret_cast<int *>(data + 12)),
      0.0,
      true,
      reinterpret_cast<unsigned __int8 *>(data + 32));
    CGuild::s_MgrHistory.push_money(
      "Auto Trade Tax",
      *reinterpret_cast<unsigned int *>(data + 8),
      *reinterpret_cast<unsigned int *>(data + 12),
      0,
      *reinterpret_cast<long double *>(data + 16),
      *reinterpret_cast<long double *>(data + 24),
      m_pOwnerGuild->m_szHistoryFileName);
  }
  else
  {
    m_sysLog.Write(
      "Failed TRC_AutoTrade::AddGDalant(GuildSerial:%d,seller:%d,in:@%d,total:@%.0f)",
      *reinterpret_cast<unsigned int *>(data + 4),
      *reinterpret_cast<unsigned int *>(data + 8),
      *reinterpret_cast<unsigned int *>(data + 12),
      *reinterpret_cast<double *>(data + 16));
  }
}

unsigned int TRC_AutoTrade::CalcPrice(unsigned int nGuildSerial, unsigned int nPrice)
{
  (void)nGuildSerial;
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

void TRC_AutoTrade::SetPatriarchTaxMoney(int dwTax)
{
  _qry_case_insert_patriarch_comm qry{};
  qry.dwDalant = static_cast<unsigned int>(dwTax);
  CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
  qry.dwSerial = ranking->GetCurrentRaceBossSerial(m_byRace, 0);
  GetTodayStr(qry.szDepDate);
  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x83u, reinterpret_cast<char *>(&qry), size);
  ++m_dwTrade;
  m_dIncomeMoney = m_dIncomeMoney + static_cast<double>(dwTax);
}

void TRC_AutoTrade::set_suggested(
  unsigned __int8 byMatterType,
  unsigned int dwMatterDst,
  char *wszMatterDst,
  unsigned int dwNext)
{
  m_suggested.init();
  m_suggested.byMatterType = byMatterType;
  m_suggested.dwMatterDst = dwMatterDst;
  strcpy_0(m_suggested.wszMatterDst, wszMatterDst);
  if (dwNext < 5 || dwNext > 0x14)
  {
    dwNext = 5;
  }
  m_suggested.dwNext = dwNext;
  m_bChangeTaxRate = true;
  m_suggested.dwSuggestedTime = GetKorLocalTime();
  m_serviceLog.Write("[Suggest Change Tax Rate]:[SUBPATRIARCH:%s] - %d(%%)", wszMatterDst, dwNext);
  PushDQSData();
}

void TRC_AutoTrade::SendMsg_PatriarchTaxRate(unsigned int n)
{
  _pt_inform_tax_rate_zocl msg{};
  msg.byCurrTax = static_cast<unsigned __int8>(get_taxrate() * 100.0f);
  msg.byNextTax = static_cast<unsigned __int8>(get_next_tax());

  unsigned __int8 pbyType[2] = {13, 119};
  g_Network.m_pProcess[0]->LoadSendMsg(n, pbyType, reinterpret_cast<char *>(&msg), msg.size());
}

void TRC_AutoTrade::PushDQSData()
{
  _insert_trc_info qry{};
  qry.byRace = m_byRace;
  qry.byMatterType = m_suggested.byMatterType;
  qry.dwMatterDst = m_suggested.dwMatterDst;
  strcpy_0(qry.wszMatterDst, m_suggested.wszMatterDst);
  qry.dwGSerial = static_cast<unsigned int>(-1);
  strcpy_0(qry.szGuildName, "*");
  if (m_suggested.dwNext < 5 || m_suggested.dwNext > 0x14)
  {
    m_suggested.dwNext = 5;
  }
  qry.dwNext = m_suggested.dwNext;
  qry.byCurrTax = static_cast<unsigned __int8>(get_taxrate() * 100.0f);

  const int size = static_cast<int>(qry.size());
  g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x33u, reinterpret_cast<char *>(&qry), size);
}

unsigned __int8 TRC_AutoTrade::_insert_info(char *pdata)
{
  if (g_Main.m_pWorldDB->insert_atrade_taxrate(
        *pdata,
        *reinterpret_cast<unsigned int *>(pdata + 1),
        pdata + 5,
        *reinterpret_cast<unsigned int *>(pdata + 23),
        pdata + 27,
        pdata[95],
        *reinterpret_cast<unsigned int *>(pdata + 91)))
  {
    return 0;
  }
  return 24;
}
