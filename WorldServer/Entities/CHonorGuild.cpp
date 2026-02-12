#include "pch.h"

#include "CHonorGuild.h"

#include <cstdio>
#include <cstring>

#include "CAsyncLogger.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CPlayer.h"
#include "CGuild.h"
#include "CMoneySupplyMgr.h"
#include "CMgrGuildHistory.h"
#include "CRFNewDatabase.h"
#include "CRFWorldDatabase.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "qry_case_in_atrade_tax.h"

_guild_honor_list_result_zocl::_guild_honor_list_result_zocl()
{
  std::memset(this, 0, sizeof(*this));
}

int _guild_honor_list_result_zocl::size() const
{
  if (byListNum > 5u)
  {
    return 2;
  }
  return 237 - 47 * (5 - byListNum);
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

void CHonorGuild::Loop()
{
  for (unsigned __int8 race = 0; race < 3; ++race)
  {
    if (m_bChageInform[race])
    {
      LoopSubProcSendInform(race);
    }
  }
}

void CHonorGuild::LoopSubProcSendInform(unsigned __int8 byRace)
{
  const unsigned int processEnd = m_uiProccessIndex[byRace] + 50;
  unsigned int processIndex = m_uiProccessIndex[byRace];
  for (; processIndex < processEnd && processIndex < MAX_PLAYER; ++processIndex)
  {
    CPlayer *player = &g_Player[processIndex];
    if (player->m_bOper && player->m_Param.GetRaceCode() == byRace)
    {
      SendCurrHonorGuildList(player->m_ObjID.m_wIndex, byRace, 0xFFu);
    }
  }

  m_uiProccessIndex[byRace] = processIndex;
  if (m_uiProccessIndex[byRace] >= MAX_PLAYER)
  {
    m_uiProccessIndex[byRace] = 0;
    m_bChageInform[byRace] = false;
  }
}

void CHonorGuild::SendCurrHonorGuildList(unsigned __int16 wIndex, unsigned __int8 byRace, unsigned __int8 byUI)
{
  unsigned __int8 type[2]{27, 112};
  m_pCurrHonorGuild[byRace]->byUI = byUI;
  const unsigned __int16 len = static_cast<unsigned __int16>(m_pCurrHonorGuild[byRace]->size());
  g_Network.m_pProcess[0]->LoadSendMsg(wIndex, type, reinterpret_cast<char *>(m_pCurrHonorGuild[byRace]), len);
}

void CHonorGuild::SendNextHonorGuildList(unsigned __int16 wIndex, unsigned __int8 byRace)
{
  unsigned __int8 type[2]{27, 119};
  if (m_bNext[byRace])
  {
    const unsigned __int16 len = static_cast<unsigned __int16>(m_pNextHonorGuild[byRace]->size());
    g_Network.m_pProcess[0]->LoadSendMsg(wIndex, type, reinterpret_cast<char *>(m_pNextHonorGuild[byRace]), len);
  }
  else
  {
    const unsigned __int16 len = static_cast<unsigned __int16>(m_pCurrHonorGuild[byRace]->size());
    g_Network.m_pProcess[0]->LoadSendMsg(wIndex, type, reinterpret_cast<char *>(m_pCurrHonorGuild[byRace]), len);
  }
}

char CHonorGuild::LoadDB()
{
  for (int j = 0; j < 3; ++j)
  {
    char result = g_Main.m_pWorldDB->Select_HonorGuild(j, m_pCurrHonorGuild[j], false);
    if (result != 2 && result)
    {
      CAsyncLogger::Instance()->FormatLog(8, "Faild Current CHonorGuild::LoadDB(RACE:%d)", j);
      return 0;
    }

    result = g_Main.m_pWorldDB->Select_HonorGuild(j, m_pNextHonorGuild[j], true);
    if (result == 1)
    {
      CAsyncLogger::Instance()->FormatLog(8, "Faild Next CHonorGuild::LoadDB(RACE:%d)", j);
      return 0;
    }

    if (m_pNextHonorGuild[j]->byListNum)
    {
      m_bNext[j] = true;
    }
    else
    {
      unsigned int dwSerial[9]{};
      result = g_Main.m_pWorldDB->Select_ClearHonorGuild(j, dwSerial);
      if (result == 1)
      {
        CAsyncLogger::Instance()->FormatLog(8, "Faild Load ClearHonorGuild(RACE:%d)", j);
        return 0;
      }

      if (dwSerial[0] == 0xFFFFFFFFu)
      {
        memset_0(m_pNextHonorGuild[j], 0, sizeof(_guild_honor_list_result_zocl));
        m_bNext[j] = true;
      }
    }
  }

  CAsyncLogger::Instance()->FormatLog(8, "Success CHonorGuild::LoadDB()");
  return 1;
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

void CHonorGuild::ChangeHonorGuild(unsigned __int8 byRace)
{
  if (m_bNext[byRace])
  {
    _guild_honor_list_result_zocl prevList;
    std::memcpy(&prevList, m_pCurrHonorGuild[byRace], sizeof(prevList));
    std::memcpy(m_pCurrHonorGuild[byRace], m_pNextHonorGuild[byRace], sizeof(_guild_honor_list_result_zocl));
    std::memset(m_pNextHonorGuild[byRace], 0, sizeof(_guild_honor_list_result_zocl));
    m_bNext[byRace] = false;

    _qry_case_update_honor_guild query{};
    query.byRace = byRace;
    const int size = static_cast<int>(query.size());
    g_Main.PushDQSData(0xFFFFFFFF, nullptr, 0x87u, reinterpret_cast<char *>(&query), size);

    m_bSendInform[byRace] = true;
    SendInformChange(byRace, 0xFFFF);
    UpdateHonorGuildMark(&prevList, 0);
    UpdateHonorGuildMark(m_pCurrHonorGuild[byRace], 1);
    m_bChageInform[byRace] = true;

    for (int j = 0; j < m_pCurrHonorGuild[byRace]->byListNum; ++j)
    {
      const int taxRate = m_pCurrHonorGuild[byRace]->GuildList[j].byTaxRate;
      char *guildName = m_pCurrHonorGuild[byRace]->GuildList[j].wszGuildName;
      CAsyncLogger::Instance()->FormatLog(8, "Changed Honor Guild (RACE:%d) : (%s:%d%%)", byRace, guildName, taxRate);
    }
  }
}

void CHonorGuild::SendInformChange(unsigned __int8 byRace, unsigned __int16 wIndex)
{
  if (!m_bSendInform[byRace])
  {
    return;
  }

  unsigned __int16 targetIndex = wIndex;
  if (wIndex == 0xFFFF)
  {
    CPvpUserAndGuildRankingSystem *ranking = CPvpUserAndGuildRankingSystem::Instance();
    const unsigned int bossSerial = ranking->GetCurrentRaceBossSerial(byRace, 0);
    CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, bossSerial);
    if (!player || !player->m_bOper)
    {
      return;
    }
    targetIndex = player->m_ObjID.m_wIndex;
  }

  unsigned __int8 type[2]{};
  type[0] = 27;
  type[1] = 117;
  char msg{};
  g_Network.m_pProcess[0]->LoadSendMsg(targetIndex, type, &msg, 1u);
  m_bSendInform[byRace] = false;
}

void CHonorGuild::UpdateHonorGuildMark(_guild_honor_list_result_zocl *pList, int bSet)
{
  for (int j = 0; j < pList->byListNum; ++j)
  {
    CGuild *guild = GetGuildDataFromSerial(g_Guild, MAX_GUILD, pList->GuildList[j].dwGuildSerial);
    if (!guild)
    {
      continue;
    }

    for (int k = 0; k < 50; ++k)
    {
      _guild_member_info *member = &guild->m_MemberData[k];
      if (member->IsFill() && member->pPlayer)
      {
        CPlayer::CashChangeStateFlag changeFlag(0);
        member->pPlayer->UpdateVisualVer(changeFlag);
        if (bSet)
        {
          member->pPlayer->SendMsg_HonorGuildMark(static_cast<char>(j));
        }
        else
        {
          member->pPlayer->SendMsg_HonorGuildMark(static_cast<char>(0xFFu));
        }
      }
    }
  }
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
