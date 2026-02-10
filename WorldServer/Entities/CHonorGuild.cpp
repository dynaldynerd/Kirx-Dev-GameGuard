#include "pch.h"

#include "CHonorGuild.h"

#include <cstdio>
#include <cstring>

#include "CAsyncLogger.h"
#include "CGuild.h"
#include "CMoneySupplyMgr.h"
#include "CMgrGuildHistory.h"
#include "CRFNewDatabase.h"
#include "GlobalObjects.h"
#include "qry_case_in_atrade_tax.h"

_guild_honor_list_result_zocl::_guild_honor_list_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

int _guild_honor_list_result_zocl::size() const
{
  return sizeof(*this);
}

CHonorGuild *CHonorGuild::Instance()
{
  static CHonorGuild s_instance;
  return &s_instance;
}

bool CHonorGuild::Init()
{
  for (int j = 0; j < 3; ++j)
  {
    m_pCurrHonorGuild[j] = new _guild_honor_list_result_zocl();
    if (m_pCurrHonorGuild[j] == nullptr)
    {
      return false;
    }

    m_pNextHonorGuild[j] = new _guild_honor_list_result_zocl();
    if (m_pNextHonorGuild[j] == nullptr)
    {
      return false;
    }

    m_bNext[j] = false;
    m_bSendInform[j] = false;
  }
  return true;
}

unsigned __int8 CHonorGuild::FindHonorGuildRank(unsigned __int8 byRace, unsigned int dwGuildSerial)
{
  if (byRace >= 3u)
  {
    return static_cast<unsigned __int8>(-1);
  }

  for (int j = 0; j < m_pCurrHonorGuild[byRace]->byListNum; ++j)
  {
    if (m_pCurrHonorGuild[byRace]->GuildList[j].dwGuildSerial == dwGuildSerial)
    {
      return static_cast<unsigned __int8>(j);
    }
  }
  return static_cast<unsigned __int8>(-1);
}

char CHonorGuild::CheckHonorGuild(unsigned __int8 byRace, unsigned int dwSerial)
{
  for (int j = 0; j < m_pCurrHonorGuild[byRace]->byListNum; ++j)
  {
    if (m_pCurrHonorGuild[byRace]->GuildList[j].dwGuildSerial == dwSerial)
    {
      return 1;
    }
  }
  return 0;
}

unsigned __int8 CHonorGuild::UpdateNextHonorGuild(unsigned __int8 byRace)
{
  char buffer[264]{};
  sprintf(buffer, "{ CALL pUpdate_HonorGuild ( %d, %d ) }", byRace, 1);
  if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, 0))
  {
    return 24;
  }

  _guild_honor_list_result_zocl *nextList = m_pNextHonorGuild[byRace];
  if (nextList->byListNum)
  {
    for (int index = 0; index < nextList->byListNum; ++index)
    {
      sprintf(
        buffer,
        "{ CALL pInsert_HonorGuild ( %d, %d, %d, %d ) }",
        byRace,
        nextList->GuildList[index].dwGuildSerial,
        nextList->GuildList[index].byTaxRate,
        1);
      if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, 1))
      {
        return 24;
      }
    }
  }
  else
  {
    sprintf(buffer, "{ CALL pInsert_HonorGuild ( %d, %d, %d, %d ) }", byRace, -1, 0, 1);
    if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, 1))
    {
      return 24;
    }
  }

  return 0;
}

unsigned __int8 CHonorGuild::UpdateChangeHonorGuild(unsigned __int8 byRace)
{
  char buffer[272]{};
  sprintf(
    buffer,
    "UPDATE tbl_honor_guild SET DCK=1, dtDisable = getdate() WHERE GuildSerial=0xFFFFFFFF AND Race=%d AND IsNext=1 "
    "AND DCK=0",
    byRace);
  if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, 0))
  {
    return 24;
  }

  sprintf(buffer, "{ CALL pUpdate_HonorGuild ( %d, %d ) }", byRace, 0);
  if (!g_Main.m_pWorldDB->ExecUpdateQuery(buffer, 0))
  {
    return 24;
  }

  sprintf(buffer, "{ CALL pUpdate_ChangeHonorGuild ( %d )}", byRace);
  return g_Main.m_pWorldDB->ExecUpdateQuery(buffer, 0) ? 0 : 24;
}

void CHonorGuild::SetGuildMaintainMoney(unsigned __int8 byRace, unsigned int dwTax, unsigned int dwSeller)
{
  _guild_honor_list_result_zocl *honorList = m_pCurrHonorGuild[byRace];
  for (int j = 0; j < honorList->byListNum; ++j)
  {
    unsigned int amount = honorList->GuildList[j].byTaxRate * dwTax / 0x64u;
    if (!amount)
    {
      continue;
    }

    CGuild *guild = GetGuildDataFromSerial(g_Guild, 500, honorList->GuildList[j].dwGuildSerial);
    if (!guild)
    {
      continue;
    }

    const long double totalDalant = guild->GetTotalDalant();
    if (totalDalant + static_cast<double>(static_cast<int>(amount)) > 1000000000.0)
    {
      amount = static_cast<int>(1000000000.0 - guild->GetTotalDalant());
    }
    if (!amount)
    {
      continue;
    }

    _qry_case_in_atrade_tax qry{};
    qry.byRace = byRace;
    qry.in_seller = dwSeller;
    qry.dwGuildSerial = honorList->GuildList[j].dwGuildSerial;
    qry.out_totalgold = 0.0;
    qry.out_totaldalant = 0.0;
    qry.in_dalant = amount;

    const int size = static_cast<int>(qry.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x88u, reinterpret_cast<char *>(&qry), size);
    CMoneySupplyMgr::Instance()->UpdateHonorGuildMoneyData(0, byRace, amount);
  }
}

void CHonorGuild::DQSCompleteInAtradTaxMoney(char *pdata)
{
  CGuild *guild = GetGuildDataFromSerial(g_Guild, MAX_GUILD, *reinterpret_cast<unsigned int *>(pdata + 4));
  if (guild)
  {
    guild->m_dTotalDalant = *reinterpret_cast<long double *>(pdata + 16);
    guild->MakeDownMemberPacket();
    guild->m_byMoneyOutputKind = 6;
    guild->SendMsg_IOMoney(
      *reinterpret_cast<unsigned int *>(pdata + 8),
      static_cast<double>(*reinterpret_cast<int *>(pdata + 12)),
      0.0,
      true,
      reinterpret_cast<unsigned __int8 *>(pdata + 32));

    CGuild::s_MgrHistory.push_money(
      "Auto Trade Tax",
      *reinterpret_cast<unsigned int *>(pdata + 8),
      *reinterpret_cast<unsigned int *>(pdata + 12),
      0,
      *reinterpret_cast<long double *>(pdata + 16),
      *reinterpret_cast<long double *>(pdata + 24),
      guild->m_szHistoryFileName);
  }
  else
  {
    CAsyncLogger *logger = CAsyncLogger::Instance();
    logger->FormatLog(
      8,
      "Faild CHonorGuild::DQSCompleteInAtradTaxMoney(GuildSerial:%d,seller:%d,in:@%d,total:@%.0f)",
      *reinterpret_cast<unsigned int *>(pdata + 4),
      *reinterpret_cast<unsigned int *>(pdata + 8),
      *reinterpret_cast<unsigned int *>(pdata + 12),
      *reinterpret_cast<double *>(pdata + 16));
  }
}
