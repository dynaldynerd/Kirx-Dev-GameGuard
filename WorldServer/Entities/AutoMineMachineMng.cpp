#include "pch.h"

#include "AutoMineMachineMng.h"

#include "CMainThread.h"
#include "CNetProcess.h"
#include "CPlayer.h"
#include "CGuild.h"
#include "GlobalObjects.h"

static SYSTEMTIME s_tmLocal{};

AutoMineMachineMng *AutoMineMachineMng::m_pInstance = nullptr;

__int64 _pt_automine_charge_money_db_update_fail_zocl::size()
{
  return 4;
}

AutoMineMachine::AutoMineMachine()
{
  m_bInit = false;
  m_bOpenUI = false;
  m_bRunning = false;
  m_bySelectedOre = 0;
  m_pOwnerGuild = nullptr;
  m_dwMiningTime = 0;
  m_dwMiningTerm = 30;
  m_dwDQSPushTime = 0;
  std::memset(&s_tmLocal, 0, sizeof(s_tmLocal));
}

AutoMineMachine::~AutoMineMachine() = default;

void AutoMineMachine::SubChargeCost(unsigned __int8 byRet, char *pdata)
{
  if (pdata == nullptr)
  {
    return;
  }

  if (byRet || pdata[45])
  {
    CPlayer *player = &g_Player[*reinterpret_cast<unsigned __int16 *>(pdata + 11)];
    if (player->m_dwObjSerial != *reinterpret_cast<unsigned int *>(pdata + 13))
    {
      player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, *reinterpret_cast<unsigned int *>(pdata + 13));
    }

    if (player && player->m_bLive)
    {
      _pt_automine_charge_money_db_update_fail_zocl payload{};
      payload.nCharge = *reinterpret_cast<int *>(pdata + 17);

      unsigned __int8 type[36]{};
      type[0] = 14;
      type[1] = 28;

      const unsigned __int16 len = static_cast<unsigned __int16>(payload.size());
      g_Network.m_pProcess[0]->LoadSendMsg(player->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&payload), len);
    }
    return;
  }

  m_Battery.m_nCurGage += *reinterpret_cast<int *>(pdata + 17);
  if (m_Battery.m_nCurGage > m_Battery.m_nMaxGage)
  {
    m_Battery.m_nCurGage = m_Battery.m_nMaxGage;
  }

  m_pOwnerGuild->m_byMoneyOutputKind = 7;
  const long double outDalant = static_cast<long double>(-*reinterpret_cast<int *>(pdata + 21));
  CGuild *ownerGuild = m_pOwnerGuild;
  unsigned __int8 *date = reinterpret_cast<unsigned __int8 *>(pdata + 41);
  m_pOwnerGuild->IOMoney(
    "AutoMine Charge",
    ownerGuild->m_MasterData.dwSerial,
    0.0,
    outDalant,
    *reinterpret_cast<long double *>(pdata + 25),
    *reinterpret_cast<long double *>(pdata + 33),
    date,
    0);

  m_Log.Write(
    "Cost : serial:%d, out:%d, total:%.0f)",
    *reinterpret_cast<int *>(pdata + 3),
    *reinterpret_cast<int *>(pdata + 21),
    *reinterpret_cast<double *>(pdata + 33));
}

AutoMineMachineMng::AutoMineMachineMng() = default;

AutoMineMachineMng *AutoMineMachineMng::Instance()
{
  if (!m_pInstance)
  {
    m_pInstance = new AutoMineMachineMng();
  }
  return m_pInstance;
}

unsigned __int8 AutoMineMachineMng::request_db_query(char *pdata)
{
  if (pdata == nullptr)
  {
    return 24;
  }

  switch (*pdata)
  {
    case 0:
      return _db_qry_insert_newowner(pdata);
    case 1:
      return _db_qry_update_battery_charge(pdata);
    case 2:
      return _db_qry_update_mineore(pdata);
    case 3:
      return _db_qry_update_workstate(pdata);
    case 4:
      return _db_qry_update_selore(pdata);
    case 5:
      return _db_qry_update_battery_discharge(pdata);
    case 6:
      return _db_qry_update_moveore(pdata);
    default:
      return 24;
  }
}

void AutoMineMachineMng::result_db_query(unsigned __int8 byRet, char *pdata)
{
  if (pdata == nullptr)
  {
    return;
  }

  if (*pdata == 1)
  {
    AutoMineMachine *machine = GetMachine(pdata[2], pdata[1]);
    machine->SubChargeCost(byRet, pdata);
  }
}

AutoMineMachine *AutoMineMachineMng::GetMachine(unsigned __int8 byRace, unsigned __int8 byCollisionType)
{
  return &m_Machine[byRace][byCollisionType];
}

unsigned __int8 AutoMineMachineMng::_db_qry_insert_newowner(char *pdata)
{
  g_Main.m_pWorldDB->SetAutoCommitMode(false);
  if (g_Main.m_pWorldDB->update_amine_dck(pdata[1], pdata[2], *reinterpret_cast<unsigned int *>(pdata + 3))
      && g_Main.m_pWorldDB->insert_amine_newowner(pdata[1], pdata[2], *reinterpret_cast<unsigned int *>(pdata + 3)))
  {
    g_Main.m_pWorldDB->CommitTransaction();
    g_Main.m_pWorldDB->SetAutoCommitMode(true);
    return 0;
  }

  g_Main.m_pWorldDB->RollbackTransaction();
  g_Main.m_pWorldDB->SetAutoCommitMode(true);
  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_battery_charge(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_battery(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        *reinterpret_cast<unsigned int *>(pdata + 7)))
  {
    return g_Main.db_output_guild_money(
      *reinterpret_cast<unsigned int *>(pdata + 13),
      *reinterpret_cast<unsigned int *>(pdata + 3),
      0,
      *reinterpret_cast<unsigned int *>(pdata + 21),
      reinterpret_cast<long double *>(pdata + 25),
      reinterpret_cast<long double *>(pdata + 33),
      reinterpret_cast<unsigned __int8 *>(pdata + 41),
      "AutoMine Charge",
      reinterpret_cast<unsigned __int8 *>(pdata + 45));
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_mineore(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_mineore(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7],
        *reinterpret_cast<unsigned int *>(pdata + 8),
        pdata[12],
        *reinterpret_cast<unsigned int *>(pdata + 13)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_workstate(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_workstate(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7]))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_selore(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_selore(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7]))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_battery_discharge(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_battery(
        pdata[2],
        pdata[1],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        *reinterpret_cast<unsigned int *>(pdata + 7)))
  {
    return 0;
  }

  return 24;
}

unsigned __int8 AutoMineMachineMng::_db_qry_update_moveore(char *pdata)
{
  if (g_Main.m_pWorldDB->update_amine_moveore(
        pdata[1],
        pdata[2],
        *reinterpret_cast<unsigned int *>(pdata + 3),
        pdata[7],
        *reinterpret_cast<unsigned int *>(pdata + 8),
        pdata[12],
        pdata[13],
        *reinterpret_cast<unsigned int *>(pdata + 14),
        pdata[18]))
  {
    return 0;
  }

  return 24;
}
