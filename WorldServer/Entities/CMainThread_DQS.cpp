#include "pch.h"

#include "CMainThread.h"

#include <ctime>
#include <cstring>

#include "AutoMineMachineMng.h"
#include "AutominePersonalMgr.h"
#include "CandidateMgr.h"
#include "CashItemRemoteStore.h"
#include "CGoldenBoxItemMgr.h"
#include "CGuildBattleController.h"
#include "CGuild.h"
#include "GuildBattle.h"
#include "CHonorGuild.h"
#include "CItemStoreManager.h"
#include "CLogFile.h"
#include "CMgrAccountLobbyHistory.h"
#include "CNetProcess.h"
#include "CNetworkEX.h"
#include "CPostSystemManager.h"
#include "CPotionMgr.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CRaceBossMsgController.h"
#include "CRaceBossWinRate.h"
#include "CTotalGuildRankManager.h"
#include "CUnmannedTraderController.h"
#include "CUnmannedTraderEnvironmentValue.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CUserDB.h"
#include "CWeeklyGuildRankManager.h"
#include "CVoteSystem.h"
#include "DqsDbStructs.h"
#include "GlobalObjects.h"
#include "PatriarchElectProcessor.h"
#include "RFEvent_ClassRefine.h"
#include "TRC_AutoTrade.h"
#include "qry_case_cash_limsale.h"
#include "qry_logout.h"

void CMainThread::OnDQSRun()
{
  m_DBFrameRate.CalcSpeedPerFrame();
  if (m_pWorldDB)
  {
    m_pWorldDB->CheckLogFileHour();
  }

  unsigned int outIndex[5]{};
  while (m_listDQSData.PopNode_Front(outIndex))
  {
    _time32(reinterpret_cast<__time32_t *>(&g_Main.m_tmDbUpdate));

    _DB_QRY_SYN_DATA *entry = &m_DBQrySynData[outIndex[0]];
    entry->m_byResult = 0;
    char *base = reinterpret_cast<char *>(&entry->m_bUse);

    unsigned __int8 qryCase = static_cast<unsigned __int8>(base[14]);
    switch (qryCase)
    {
      case 0:
      {
        entry->m_byResult = db_Reged_Avator(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_REGED *>(base + 20),
          reinterpret_cast<_NOT_ARRANGED_AVATOR_DB *>(base + 827),
          base + 4277);
        break;
      }
      case 1:
      {
        entry->m_byResult = db_Insert_Avator(
          *reinterpret_cast<unsigned int *>(base + 16),
          base + 20,
          reinterpret_cast<_REGED_AVATOR_DB *>(base + 33),
          reinterpret_cast<unsigned int *>(base + 50));
        break;
      }
      case 2:
      {
        entry->m_byResult = db_Delete_Avator(
          *reinterpret_cast<unsigned int *>(base + 20),
          static_cast<unsigned __int8>(base[17]));
        break;
      }
      case 3:
      {
        entry->m_byResult = db_Load_Avator(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 4),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<bool *>(base + 37235),
          reinterpret_cast<unsigned int *>(base + 37336),
          reinterpret_cast<unsigned int *>(base + 37340),
          reinterpret_cast<bool *>(base + 37348),
          reinterpret_cast<unsigned __int8 *>(base + 37448),
          reinterpret_cast<long double *>(base + 37456),
          reinterpret_cast<long double *>(base + 37464),
          reinterpret_cast<bool *>(base + 37472),
          reinterpret_cast<bool *>(base + 37481),
          reinterpret_cast<unsigned __int8 *>(base + 37521),
          false,
          reinterpret_cast<unsigned int *>(base + 37344));
        if (entry->m_byResult ||
            (CUserDB::ReRangeClientIndex(reinterpret_cast<_AVATOR_DATA *>(base + 20)),
             CUserDB::CheckDQSLoadCharacterData(reinterpret_cast<_AVATOR_DATA *>(base + 20))))
        {
          if (_db_load_event_classrefine(
                *reinterpret_cast<unsigned int *>(base + 16),
                reinterpret_cast<unsigned __int8 *>(base + 37480),
                reinterpret_cast<unsigned int *>(base + 37476)))
          {
            entry->m_byResult = 72;
          }
        }
        else
        {
          entry->m_byResult = 38;
        }
        break;
      }
      case 5:
      {
        entry->m_byResult = db_Update_Avator(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<_AVATOR_DATA *>(base + 37235),
          static_cast<bool>(base[74450]));
        db_Update_PostStorage(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<_AVATOR_DATA *>(base + 37219));
        if (base[74451])
        {
          entry->m_byResult = _db_update_event_classrefine(
            *reinterpret_cast<unsigned __int16 *>(base + 8),
            *reinterpret_cast<unsigned int *>(base + 16),
            static_cast<unsigned __int8>(base[74452]),
            *reinterpret_cast<unsigned int *>(base + 74456));
        }
        _db_Update_SFDelayData(*reinterpret_cast<unsigned int *>(base + 16), reinterpret_cast<_AVATOR_DATA *>(base + 20));
        break;
      }
      case 6:
      {
        entry->m_byResult = db_Update_Avator(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<_AVATOR_DATA *>(base + 37235),
          true);
        db_Update_PostStorage(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<_AVATOR_DATA *>(base + 37219));
        if (base[74450])
        {
          entry->m_byResult = _db_update_event_classrefine(
            *reinterpret_cast<unsigned __int16 *>(base + 8),
            *reinterpret_cast<unsigned int *>(base + 16),
            static_cast<unsigned __int8>(base[74451]),
            *reinterpret_cast<unsigned int *>(base + 74452));
        }
        _db_Update_SFDelayData(*reinterpret_cast<unsigned int *>(base + 16), reinterpret_cast<_AVATOR_DATA *>(base + 20));
        break;
      }
      case 7:
      {
        db_Log_AvatorLevel(
          *reinterpret_cast<unsigned int *>(base + 24),
          *reinterpret_cast<unsigned int *>(base + 16),
          static_cast<unsigned __int8>(base[20]));
        break;
      }
      case 8:
      {
        db_Log_UserNum(*reinterpret_cast<unsigned int *>(base + 16), *reinterpret_cast<unsigned int *>(base + 20));
        break;
      }
      case 9:
      {
        _worlddb_economy_history_info pEconomyData{};
        unsigned int *payload = reinterpret_cast<unsigned int *>(base + 16);
        pEconomyData.dwManageValue = *reinterpret_cast<unsigned int *>(base + 72);
        memcpy_0(pEconomyData.dTradeDalant, base + 48, sizeof(pEconomyData.dTradeDalant));
        memcpy_0(&pEconomyData, payload + 2, 0x18uLL);
        memcpy_0(pEconomyData.dMineOre, payload + 16, sizeof(pEconomyData.dMineOre));
        memcpy_0(pEconomyData.dCutOre, payload + 34, sizeof(pEconomyData.dCutOre));
        db_Insert_Economy_History(*payload, &pEconomyData);
        break;
      }
      case 10:
      {
        db_Insert_CharacSelect_Log(
          *reinterpret_cast<unsigned int *>(base + 32),
          base + 16,
          *reinterpret_cast<unsigned int *>(base + 56),
          base + 36,
          *reinterpret_cast<unsigned __int16 *>(base + 60),
          static_cast<unsigned __int8>(base[62]),
          static_cast<unsigned __int8>(base[63]),
          static_cast<unsigned __int8>(base[64]),
          static_cast<unsigned __int8>(base[65]),
          static_cast<unsigned __int8>(base[66]));
        break;
      }
      case 11:
      {
        db_Update_PvpInfo(
          *reinterpret_cast<unsigned int *>(base + 16),
          static_cast<unsigned __int8>(base[20]),
          reinterpret_cast<__int16 *>(base + 22),
          *reinterpret_cast<long double *>(base + 32));
        break;
      }
      case 12:
      {
        entry->m_byResult = db_Update_Avator(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<_AVATOR_DATA *>(base + 37235),
          true);
        if (base[74450])
        {
          entry->m_byResult = _db_update_event_classrefine(
            *reinterpret_cast<unsigned __int16 *>(base + 8),
            *reinterpret_cast<unsigned int *>(base + 16),
            static_cast<unsigned __int8>(base[74451]),
            *reinterpret_cast<unsigned int *>(base + 74452));
        }
        db_Update_PostStorage(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AVATOR_DATA *>(base + 20),
          reinterpret_cast<_AVATOR_DATA *>(base + 37219));
        break;
      }
      case 13:
      {
        entry->m_byResult = db_Add_PvpPoint(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 24));
        break;
      }
      case 14:
      {
        entry->m_byResult = db_Insert_Item(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 24),
          *reinterpret_cast<unsigned int *>(base + 28),
          static_cast<unsigned __int8>(base[32]));
        break;
      }
      case 15:
      {
        entry->m_byResult = db_Insert_guild(
          reinterpret_cast<unsigned int *>(base + 72),
          base + 20,
          static_cast<unsigned __int8>(base[37]),
          reinterpret_cast<unsigned int *>(base + 288));
        break;
      }
      case 16:
      {
        entry->m_byResult = db_update_guildmember_add(
          *reinterpret_cast<unsigned int *>(base + 28),
          *reinterpret_cast<unsigned int *>(base + 20),
          static_cast<unsigned __int8>(base[36]),
          *reinterpret_cast<unsigned int *>(base + 40));
        break;
      }
      case 17:
      {
        entry->m_byResult = db_update_guildmember_del(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 32),
          *reinterpret_cast<unsigned int *>(base + 20));
        break;
      }
      case 18:
      {
        entry->m_byResult = db_update_guildmember_del(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 24),
          *reinterpret_cast<unsigned int *>(base + 36));
        break;
      }
      case 19:
      {
        entry->m_byResult = db_input_guild_money(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 44),
          *reinterpret_cast<unsigned int *>(base + 52),
          *reinterpret_cast<unsigned int *>(base + 48),
          reinterpret_cast<long double *>(base + 144),
          reinterpret_cast<long double *>(base + 128),
          reinterpret_cast<unsigned __int8 *>(base + 56),
          base + 20);
        break;
      }
      case 20:
      {
        entry->m_byResult = db_output_guild_money(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 44),
          *reinterpret_cast<unsigned int *>(base + 52),
          *reinterpret_cast<unsigned int *>(base + 48),
          reinterpret_cast<long double *>(base + 144),
          reinterpret_cast<long double *>(base + 128),
          reinterpret_cast<unsigned __int8 *>(base + 56),
          base + 20,
          reinterpret_cast<unsigned __int8 *>(base + 80));
        break;
      }
      case 21:
      {
        entry->m_byResult = db_buy_emblem(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 28),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 24),
          *reinterpret_cast<unsigned int *>(base + 32),
          reinterpret_cast<long double *>(base + 144),
          reinterpret_cast<long double *>(base + 128),
          reinterpret_cast<unsigned __int8 *>(base + 36),
          base + 44,
          reinterpret_cast<unsigned __int8 *>(base + 80));
        break;
      }
      case 22:
      {
        entry->m_byResult = db_disjoint_guild(*reinterpret_cast<unsigned int *>(base + 16));
        break;
      }
      case 23:
      {
        entry->m_byResult = db_char_set_alive(
          *reinterpret_cast<unsigned int *>(base + 4),
          static_cast<unsigned __int8>(base[16]),
          *reinterpret_cast<unsigned int *>(base + 20),
          base + 24,
          static_cast<unsigned __int8>(base[41]),
          reinterpret_cast<_REGED *>(base + 42));
        break;
      }
      case 24:
      {
        db_Insert_ChangeClass_AfterInitClass(
          *reinterpret_cast<unsigned int *>(base + 16),
          static_cast<unsigned __int8>(base[20]),
          base + 21,
          base + 26,
          *reinterpret_cast<unsigned int *>(base + 32),
          static_cast<unsigned __int8>(base[36]),
          *reinterpret_cast<unsigned __int16 *>(base + 38),
          static_cast<unsigned __int8>(base[40]),
          static_cast<unsigned __int8>(base[41]),
          static_cast<unsigned __int8>(base[42]),
          static_cast<unsigned __int8>(base[43]),
          static_cast<unsigned __int8>(base[44]));
        break;
      }
      case 25:
      {
        entry->m_byResult = db_sendwebracebosssms(reinterpret_cast<_qry_case_sendwebracebosssms *>(base + 16));
        break;
      }
      case 30:
      {
        CGuildBattleController *controller = CGuildBattleController::Instance();
        entry->m_byResult = controller->AddSchedule( base + 16);
        break;
      }
      case 31:
      {
        entry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateWinLose(
              *reinterpret_cast<unsigned __int8 *>(base + 16),
              *reinterpret_cast<unsigned int *>(base + 20),
              static_cast<unsigned __int8>(base[24]),
              *reinterpret_cast<unsigned int *>(base + 28)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 32:
      {
        entry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateDraw(
              *reinterpret_cast<unsigned __int8 *>(base + 16),
              *reinterpret_cast<unsigned int *>(base + 20),
              static_cast<unsigned __int8>(base[24]),
              *reinterpret_cast<unsigned int *>(base + 28)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 33:
      {
        entry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateRank(
              *reinterpret_cast<unsigned __int8 *>(base + 16),
              reinterpret_cast<unsigned __int8 *>(base + 17)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 34:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->CreateGuildBattleRankTable( base + 16))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 35:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->UpdateClearGuildBattleRank())
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 36:
      {
        entry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        unsigned int *payload = reinterpret_cast<unsigned int *>(base + 16);
        if (!controller->UpdateClearRerservedDayInfo( payload[0], payload[1], payload[2], payload[3]))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 37:
      {
        entry->m_byResult = db_input_guild_money(
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 28),
          *reinterpret_cast<unsigned int *>(base + 24),
          reinterpret_cast<long double *>(base + 48),
          reinterpret_cast<long double *>(base + 40),
          reinterpret_cast<unsigned __int8 *>(base + 32),
          "Scramble Cost");
        break;
      }
      case 38:
      {
        entry->m_byResult = db_output_guild_money(
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 28),
          *reinterpret_cast<unsigned int *>(base + 24),
          reinterpret_cast<long double *>(base + 48),
          reinterpret_cast<long double *>(base + 40),
          reinterpret_cast<unsigned __int8 *>(base + 32),
          "Scramble Cost",
          reinterpret_cast<unsigned __int8 *>(base + 72));
        break;
      }
      case 39:
      {
        entry->m_byResult = db_input_guild_money(
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 28),
          *reinterpret_cast<unsigned int *>(base + 24),
          reinterpret_cast<long double *>(base + 48),
          reinterpret_cast<long double *>(base + 40),
          reinterpret_cast<unsigned __int8 *>(base + 32),
          "Scramble Cost");
        break;
      }
      case 40:
      {
        entry->m_byResult = 0;
        break;
      }
      case 41:
      {
        entry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateReservedGuildBattleSchedule(
              *reinterpret_cast<unsigned int *>(base + 20),
              reinterpret_cast<unsigned __int8 *>(base + 24)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 43:
      {
        entry->m_byResult = 0;
        CTotalGuildRankManager *manager = CTotalGuildRankManager::Instance();
        if (!manager->Update(reinterpret_cast<_total_guild_rank_info *>(base + 16)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 44:
      {
        entry->m_byResult = 0;
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        if (!manager->UpdateTodayRank(reinterpret_cast<_pvppoint_guild_rank_info *>(base + 16)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 45:
      {
        entry->m_byResult = 0;
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        if (!manager->UpdateWeeklyOwner(reinterpret_cast<_weeklyguildrank_owner_info *>(base + 16)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 46:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_ClearWeeklyPvpPointSum())
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 47:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_IncreaseWeeklyGuildGuildBattlePvpPointSum(
              *reinterpret_cast<unsigned int *>(base + 16),
              *reinterpret_cast<long double *>(base + 24)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 49:
      {
        entry->m_byResult = db_update_guildmaster(reinterpret_cast<_qry_case_update_guildmaster *>(base + 16));
        break;
      }
      case 50:
      {
        entry->m_byResult = _db_Update_MacroData(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_AIOC_A_MACRODATA *>(base + 20),
          reinterpret_cast<_AIOC_A_MACRODATA *>(base + 2724))
                               ? 0
                               : 24;
        break;
      }
      case 51:
      {
        entry->m_byResult = TRC_AutoTrade::_insert_info(base + 16);
        break;
      }
      case 52:
      {
        entry->m_byResult = db_input_guild_money_atradetax(
          *reinterpret_cast<unsigned int *>(base + 24),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 28),
          reinterpret_cast<long double *>(base + 32),
          reinterpret_cast<long double *>(base + 40),
          reinterpret_cast<unsigned __int8 *>(base + 48));
        break;
      }
      case 53:
      {
        AutoMineMachineMng *manager = AutoMineMachineMng::Instance();
        entry->m_byResult = manager->request_db_query( base + 16);
        break;
      }
      case 56:
      {
        AutominePersonalMgr *manager = AutominePersonalMgr::instance();
        entry->m_byResult = manager->request_query( base + 16);
        break;
      }
      case 57:
      {
        _guildbattle_totalrecord *info = reinterpret_cast<_guildbattle_totalrecord *>(base + 16);
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Select_GuildBattleRecord( info->dwGuildSerial, info))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 58:
      {
        unsigned __int8 result = g_Main.m_pWorldDB->Select_UnmannedTraderReservedSchedule(
          CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt,
          reinterpret_cast<_unmannedtrader_reserved_schedule_info *>(base + 16));
        if (result != 2 && result)
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 59:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateItemState( base + 16);
        break;
      }
      case 60:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateRegistItem( base + 16);
        break;
      }
      case 61:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateCancelRegist( base + 16);
        break;
      }
      case 62:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateTimeOutCancelRegist( base + 16);
        break;
      }
      case 63:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateRePrice( base + 16);
        break;
      }
      case 64:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->SelectBuy( base + 16);
        break;
      }
      case 65:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateBuy( base + 16);
        break;
      }
      case 66:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateBuyRollBack( base + 16);
        break;
      }
      case 67:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateLazyClean( base + 16);
        break;
      }
      case 68:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateBuyComplete( base + 16);
        break;
      }
      case 71:
      {
        entry->m_byResult = _db_GuildRoom_Insert(reinterpret_cast<_qry_case_guildroom_insert *>(base + 16));
        break;
      }
      case 72:
      {
        entry->m_byResult =
          _db_GuildRoom_Update(reinterpret_cast<_qry_case_guildroom_update *>(base + 16)) ? 0 : 24;
        break;
      }
      case 74:
      {
        entry->m_byResult = db_GM_GreetingMsg(reinterpret_cast<_qry_case_gm_greetingmsg *>(base + 16));
        break;
      }
      case 75:
      {
        entry->m_byResult = db_RACE_GreetingMsg(reinterpret_cast<_qry_case_race_greetingmsg *>(base + 16));
        break;
      }
      case 76:
      {
        entry->m_byResult = db_GUILD_GreetingMsg(reinterpret_cast<_qry_case_guild_greetingmsg *>(base + 16));
        break;
      }
      case 77:
      {
        CPostSystemManager *manager = CPostSystemManager::Instace();
        entry->m_byResult = manager->UpdateRegist( base + 16);
        break;
      }
      case 78:
      {
        CPostSystemManager *manager = CPostSystemManager::Instace();
        entry->m_byResult = manager->PostSend( base + 16);
        break;
      }
      case 79:
      {
        entry->m_byResult = db_Load_PostStorage(base + 16);
        break;
      }
      case 80:
      {
        entry->m_byResult = db_Load_ReturnPost(base + 16);
        break;
      }
      case 82:
      {
        entry->m_byResult = db_Load_Content(base + 16);
        break;
      }
      case 83:
      {
        entry->m_byResult = _db_init_classrefine_count();
        break;
      }
      case 84:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Insert_GuildBatlleResultLog(
              base + 16,
              base + 33,
              *reinterpret_cast<unsigned int *>(base + 52),
              base + 56,
              *reinterpret_cast<unsigned int *>(base + 76),
              base + 80,
              *reinterpret_cast<unsigned int *>(base + 100),
              *reinterpret_cast<unsigned int *>(base + 104),
              *reinterpret_cast<unsigned int *>(base + 108),
              *reinterpret_cast<unsigned int *>(base + 112),
              *reinterpret_cast<unsigned int *>(base + 116),
              *reinterpret_cast<unsigned int *>(base + 120),
              *reinterpret_cast<unsigned int *>(base + 124),
              *reinterpret_cast<unsigned int *>(base + 128),
              static_cast<unsigned __int8>(base[132]),
              *reinterpret_cast<unsigned int *>(base + 136),
              base + 140,
              *reinterpret_cast<unsigned int *>(base + 160),
              base + 164,
              static_cast<unsigned __int8>(base[181]),
              *reinterpret_cast<unsigned int *>(base + 184),
              base + 188))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 85:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep1( base + 16);
        break;
      }
      case 86:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep2( base + 16);
        break;
      }
      case 87:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep3( base + 16);
        break;
      }
      case 88:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep4( base + 16);
        break;
      }
      case 89:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep5( base + 16);
        break;
      }
      case 90:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep6( base + 16);
        break;
      }
      case 91:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep7( base + 16);
        break;
      }
      case 92:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep8( base + 16);
        break;
      }
      case 93:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep9( base + 16);
        break;
      }
      case 94:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep10( base + 16);
        break;
      }
      case 95:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRaceRankStep11( base + 16);
        break;
      }
      case 97:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateGuildRankStep1( base + 16);
        break;
      }
      case 98:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateGuildRankStep2( base + 16);
        break;
      }
      case 99:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateGuildRankStep3( base + 16);
        break;
      }
      case 100:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateGuildRankStep4( base + 16);
        break;
      }
      case 101:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRankinGuildStep1( base + 16);
        break;
      }
      case 102:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRankinGuildStep2( base + 16);
        break;
      }
      case 103:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRankinGuildStep3( base + 16);
        break;
      }
      case 104:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRankinGuildStep4( base + 16);
        break;
      }
      case 105:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRankinGuildStep5( base + 16);
        break;
      }
      case 106:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateRankinGuildStep6( base + 16);
        break;
      }
      case 107:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        entry->m_byResult = system->UpdateAndSelectGuildGrade( base + 16);
        break;
      }
      case 111:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateLogInComplete( base + 16);
        break;
      }
      case 112:
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        entry->m_byResult = manager->UpdateStoreLimitItem();
        break;
      }
      case 113:
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        entry->m_byResult = manager->UpdateDisableInstanceStore( base + 16);
        break;
      }
      case 114:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Update_RegistCandidate_2st();
        break;
      }
      case 115:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Update_Score();
        break;
      }
      case 116:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Update_ClassType();
        break;
      }
      case 117:
      case 125:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Insert_Candidate( base);
        break;
      }
      case 118:
      {
        entry->m_byResult = db_output_guild_money(
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 28),
          *reinterpret_cast<unsigned int *>(base + 24),
          reinterpret_cast<long double *>(base + 48),
          reinterpret_cast<long double *>(base + 40),
          reinterpret_cast<unsigned __int8 *>(base + 32),
          "Scramble Cost",
          reinterpret_cast<unsigned __int8 *>(base + 72));
        break;
      }
      case 119:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        entry->m_byResult = processor->Insert_Elect();
        break;
      }
      case 120:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        entry->m_byResult = processor->Update_Elect();
        break;
      }
      case 121:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Update_Refund();
        break;
      }
      case 122:
      {
        entry->m_byResult = 0;
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        if (!processor->LoadElectState())
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 123:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        entry->m_byResult = processor->Request_Refund( base + 16);
        break;
      }
      case 124:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        entry->m_byResult = processor->Insert_PatrirchItemChargeRefund( base + 16);
        break;
      }
      case 126:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Update_DischargePatriarch( base + 16);
        break;
      }
      case 127:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_Punishment( base + 16))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 128:
      {
        entry->m_byResult = 0;
        unsigned __int8 result = g_Main.m_pWorldDB->Select_GetCharSerialByNameRace(
          base + 24,
          *reinterpret_cast<unsigned __int8 *>(base + 16),
          reinterpret_cast<unsigned int *>(base + 20));
        if (result != 2 && result)
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 129:
      {
        CPostSystemManager *manager = CPostSystemManager::Instace();
        entry->m_byResult = manager->PostReceiverCheck( base + 16);
        break;
      }
      case 130:
      {
        entry->m_byResult = _db_Load_PatriarchComm(base + 16);
        break;
      }
      case 131:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Insert_PatriarchComm(
              *reinterpret_cast<unsigned int *>(base + 16),
              *reinterpret_cast<unsigned int *>(base + 20),
              base + 24))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 134:
      {
        CHonorGuild *honor = CHonorGuild::Instance();
        entry->m_byResult = honor->UpdateNextHonorGuild( *reinterpret_cast<unsigned __int8 *>(base + 16));
        break;
      }
      case 135:
      {
        CHonorGuild *honor = CHonorGuild::Instance();
        entry->m_byResult = honor->UpdateChangeHonorGuild( *reinterpret_cast<unsigned __int8 *>(base + 16));
        break;
      }
      case 136:
      {
        entry->m_byResult = db_input_guild_money_atradetax(
          *reinterpret_cast<unsigned int *>(base + 24),
          *reinterpret_cast<unsigned int *>(base + 20),
          *reinterpret_cast<unsigned int *>(base + 28),
          reinterpret_cast<long double *>(base + 32),
          reinterpret_cast<long double *>(base + 40),
          reinterpret_cast<unsigned __int8 *>(base + 48));
        break;
      }
      case 137:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Insert_RaceBattleLog( reinterpret_cast<_race_battle_log_info *>(base + 16)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 138:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->CheckDBValidCharacter( *reinterpret_cast<unsigned __int8 *>(base + 16));
        break;
      }
      case 139:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        entry->m_byResult = manager->Update_VoteTime( *reinterpret_cast<unsigned int *>(base + 16));
        break;
      }
      case 140:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateReRegist( base + 16);
        break;
      }
      case 141:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateCheatRegistTime( base + 16);
        break;
      }
      case 142:
      {
        CRaceBossWinRate *mgr = CRaceBossWinRate::Instance();
        entry->m_byResult =
          mgr->LoadBossAccmulationWinRate( reinterpret_cast<_qry_case_raceboss_accumulation_winrate *>(base + 16));
        break;
      }
      case 143:
      {
        entry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_UserGuildData(
              *reinterpret_cast<unsigned int *>(base + 16),
              *reinterpret_cast<unsigned int *>(base + 24),
              static_cast<unsigned __int8>(base[28])))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 144:
      {
        unsigned __int8 result = g_Main.m_pWorldDB->Select_GuildMasterLastConn(
          *reinterpret_cast<unsigned int *>(base + 16),
          *reinterpret_cast<unsigned int *>(base + 28),
          reinterpret_cast<unsigned int *>(base + 32));
        entry->m_byResult = (result == 2 || result == 0) ? 0 : 24;
        break;
      }
      case 145:
      {
        entry->m_byResult =
          g_Main.m_pWorldDB->Select_CharacterReName( base + 28, reinterpret_cast<unsigned int *>(base + 20))
            ? 0
            : 24;
        break;
      }
      case 146:
      {
        entry->m_byResult =
          g_Main.m_pWorldDB->Update_CharacterReName( base + 28, *reinterpret_cast<unsigned int *>(base + 16))
            ? 0
            : 24;
        break;
      }
      case 147:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        entry->m_byResult = controller->UpdateReRegistRollBack( base + 16);
        break;
      }
      case 149:
      {
        entry->m_byResult =
          g_Main.m_pWorldDB->Updatet_Account_Vote_Available(
            *reinterpret_cast<unsigned int *>(base + 20),
            reinterpret_cast<unsigned __int8 *>(base + 16))
            ? 0
            : 24;
        break;
      }
      case 150:
      {
        entry->m_byResult =
          g_Main.m_pWorldDB->Update_Player_Vote_Info(
            *reinterpret_cast<unsigned int *>(base + 36),
            *reinterpret_cast<unsigned int *>(base + 16),
            static_cast<unsigned __int8>(base[20]),
            static_cast<unsigned __int8>(base[21]),
            *reinterpret_cast<unsigned int *>(base + 24))
            ? 0
            : 24;
        break;
      }
      case 151:
      {
        entry->m_byResult = g_Main.m_pWorldDB->UpdateVotedReset_Cheat( *reinterpret_cast<unsigned int *>(base + 20))
                              ? 0
                              : 24;
        break;
      }
      case 152:
      {
        entry->m_byResult = g_Main.m_pWorldDB->UpdateServerResetToken(
                              *reinterpret_cast<unsigned int *>(base + 16),
                              *reinterpret_cast<unsigned __int16 *>(base + 24),
                              *reinterpret_cast<unsigned int *>(base + 20))
                              ? 0
                              : 24;
        break;
      }
      case 153:
      {
        unsigned __int8 result = g_Main.m_pWorldDB->Select_Player_Last_LogoutTime(
          *reinterpret_cast<unsigned int *>(base + 20),
          reinterpret_cast<unsigned int *>(base + 24));
        entry->m_byResult = (result == 1) ? 24 : 0;
        if (result == 2)
        {
          if (m_pWorldDB->Insert_PlayerTimeLimitInfo( *reinterpret_cast<unsigned int *>(base + 20)))
          {
            if (m_pWorldDB->Select_Player_Last_LogoutTime(
                  *reinterpret_cast<unsigned int *>(base + 20),
                  reinterpret_cast<unsigned int *>(base + 24)))
            {
              entry->m_byResult = 24;
            }
          }
          else
          {
            entry->m_byResult = 24;
          }
        }
        break;
      }
      case 155:
      {
        entry->m_byResult = g_Main.m_pWorldDB->Update_Player_TimeLimit_Info(
                              *reinterpret_cast<unsigned int *>(base + 20),
                              *reinterpret_cast<unsigned int *>(base + 28),
                              static_cast<unsigned __int8>(base[32]))
                              ? 0
                              : 24;
        break;
      }
      case 157:
      {
        qry_case_cash_limsale *sheet = reinterpret_cast<qry_case_cash_limsale *>(base + 16);
        entry->m_byResult = _db_Load_Cash_LimSale(sheet);
        if (!entry->m_byResult)
        {
          CashItemRemoteStore *store = CashItemRemoteStore::Instance();
          store->Set_FROMDB_LimitedSale_Event( &sheet->NewSale);
        }
        break;
      }
      case 158:
      {
        entry->m_byResult = _db_Update_Cash_LimSale(
          reinterpret_cast<_db_cash_limited_sale *>(base + 16),
          reinterpret_cast<_db_cash_limited_sale *>(base + 180));
        break;
      }
      case 161:
      {
        entry->m_byResult = _db_Update_Set_Limit_Run();
        break;
      }
      case 165:
      {
        entry->m_byResult = _db_Update_GoldBoxItem(
          *reinterpret_cast<unsigned int *>(base + 16),
          reinterpret_cast<_db_golden_box_item *>(base + 20),
          reinterpret_cast<_db_golden_box_item *>(base + 1644));
        break;
      }
      case 171:
      {
        entry->m_byResult = _db_Select_RegeAvator_For_Lobby_Logout(base + 16);
        break;
      }
      case 172:
      {
        entry->m_byResult = 0;
        if (!m_pWorldDB->Insert_OreReset_Log(
              *reinterpret_cast<unsigned __int8 *>(base + 16),
              *reinterpret_cast<unsigned int *>(base + 36),
              *reinterpret_cast<unsigned int *>(base + 40),
              *reinterpret_cast<unsigned int *>(base + 44)))
        {
          entry->m_byResult = 24;
        }
        break;
      }
      case 175:
      {
        entry->m_byResult = _db_Update_Data_For_Post_Send(base + 16);
        break;
      }
      case 176:
      {
        entry->m_byResult = _db_Update_Data_For_Trade(base + 16);
        break;
      }
      default:
        break;
    }

    if (entry->m_byResult && entry->m_byResult != 25 && entry->m_byResult != 49)
    {
      m_logDQS.Write(
        "DQSThread.. DB fail.. account: %d, case: %d, ret: %d",
        *reinterpret_cast<unsigned int *>(base + 4),
        qryCase,
        entry->m_byResult);
    }

    base[1] = 1;
    m_listDQSDataComplete.PushNode_Back(outIndex[0]);
  }
}

void CMainThread::DQSCompleteProcess()
{
  unsigned int outIndex[5]{};
  while (m_listDQSDataComplete.PopNode_Front(outIndex))
  {
    _DB_QRY_SYN_DATA *pData = &m_DBQrySynData[outIndex[0]];
    int qryCase = pData->m_byQryCase;
    switch (qryCase)
    {
      case 0:
        Reged_Avator_Complete(pData);
        break;
      case 1:
        Insert_Avator_Complete(pData);
        break;
      case 2:
        Delete_Avator_Complete(pData);
        break;
      case 3:
        Select_Avator_Complete(pData);
        break;
      case 5:
        Logout_Account_Complete(pData);
        break;
      case 6:
        Lobby_Account_Complete(pData);
        break;
      case 12:
        Cont_UserSave_Complete(pData);
        break;
      case 15:
        CPlayer::Guild_Insert_Complete(pData);
        break;
      case 16:
        CPlayer::Guild_Join_Accept_Complete(pData);
        break;
      case 17:
        CPlayer::Guild_Self_Leave_Complete(pData);
        break;
      case 18:
        CPlayer::Guild_Force_Leave_Complete(pData);
        break;
      case 19:
        CPlayer::Guild_Push_Money_Complete(pData);
        break;
      case 20:
        CPlayer::Guild_Pop_Money_Complete(pData);
        break;
      case 21:
        CPlayer::Guild_Buy_Emblem_Complete(pData);
        break;
      case 22:
        CPlayer::Guild_Disjoint_Complete(pData);
        break;
      case 23:
        Alive_Char_Complete(pData);
        break;
      case 25:
        SendWebRaceBossSMS(pData);
        break;
      case 30:
        AddGuildBattleSchdule(pData);
        break;
      case 31:
        UpdateGuildBattleWinLoseRankInfo(pData);
        break;
      case 32:
        UpdateGuildBattleDrawRankInfo(pData);
        break;
      case 33:
        UpdateLoadGuildBattleRank(pData);
        break;
      case 34:
      {
        CGuildBattleController *controller = CGuildBattleController::Instance();
        controller->CompleteCreateGuildBattleRankTable( pData->m_byResult);
        break;
      }
      case 35:
      {
        CGuildBattleController *controller = CGuildBattleController::Instance();
        controller->CompleteClearGuildBattleRank( pData->m_byResult);
        break;
      }
      case 37:
        InGuildbattleCost(pData);
        break;
      case 38:
        OutSrcGuildbattleCost(pData);
        break;
      case 39:
        InGuildbattleRewardMoney(pData);
        break;
      case 41:
        UpdateReservedGuildBattleSchedule(pData);
        break;
      case 43:
      {
        CTotalGuildRankManager *manager = CTotalGuildRankManager::Instance();
        manager->UpdateComlete(pData->m_byResult, reinterpret_cast<_total_guild_rank_info *>(pData->m_sData));
        break;
      }
      case 44:
      {
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        manager->CompleteLoadeTodayRank(
          pData->m_byResult,
          reinterpret_cast<_pvppoint_guild_rank_info *>(pData->m_sData));
        break;
      }
      case 45:
      {
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        manager->CompleteUpdateWeeklyOwner(
          pData->m_byResult,
          reinterpret_cast<_weeklyguildrank_owner_info *>(pData->m_sData));
        break;
      }
      case 46:
      {
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        manager->CompleteUpdateClear( pData->m_byResult);
        break;
      }
      case 49:
        CPlayer::Guild_Update_GuildMater_Complete(pData);
        break;
      case 52:
        InAtradTaxMoney(pData);
        break;
      case 53:
      {
        AutoMineMachineMng *manager = AutoMineMachineMng::Instance();
        manager->result_db_query( pData->m_byResult, pData->m_sData);
        break;
      }
      case 56:
      {
        AutominePersonalMgr *manager = AutominePersonalMgr::instance();
        manager->result_query( pData->m_byResult, pData->m_sData);
        break;
      }
      case 57:
        CompleteLoadGuildBattleTotalRecord(pData->m_byResult, pData->m_sData);
        break;
      case 58:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteSelectReservedSchedule(
          pData->m_byResult,
          reinterpret_cast<_unmannedtrader_reserved_schedule_info *>(pData->m_sData));
        break;
      }
      case 59:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteUpdateState( pData->m_byResult, pData->m_sData);
        break;
      }
      case 60:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteRegistItem( pData->m_byResult, pData->m_sData);
        break;
      }
      case 61:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteCancelRegist( pData->m_byResult, pData->m_sData);
        break;
      }
      case 62:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteTimeOutCancelRegist( pData->m_byResult, pData->m_sData);
        break;
      }
      case 63:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteReprice( pData->m_byResult, pData->m_sData);
        break;
      }
      case 64:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteSelectBuyInfo( pData->m_byResult, pData->m_sData);
        break;
      }
      case 65:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteBuy( pData->m_byResult, pData->m_sData);
        break;
      }
      case 66:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteBuyRollBack( pData->m_byResult, pData->m_sData);
        break;
      }
      case 67:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->ComleteLazyClean( pData->m_sData);
        break;
      }
      case 68:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteBuyComplete( pData->m_sData);
        break;
      }
      case 77:
      {
        CPostSystemManager *manager = CPostSystemManager::Instace();
        manager->CompleteRegist( pData->m_sData);
        break;
      }
      case 78:
      {
        CPostSystemManager *manager = CPostSystemManager::Instace();
        manager->CompleteSend( pData->m_sData);
        break;
      }
      case 79:
        Load_PostStorage_Complete(pData->m_sData);
        break;
      case 80:
        Load_ReturnPost_Complete(pData->m_sData);
        break;
      case 82:
        Load_Content_Complete(pData->m_sData);
        break;
      case 85:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep1( pData->m_byResult, pData->m_sData);
        break;
      }
      case 86:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep2( pData->m_byResult, pData->m_sData);
        break;
      }
      case 87:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep3( pData->m_byResult, pData->m_sData);
        break;
      }
      case 88:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep4( pData->m_byResult, pData->m_sData);
        break;
      }
      case 89:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep5( pData->m_byResult, pData->m_sData);
        break;
      }
      case 90:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep6( pData->m_byResult, pData->m_sData);
        break;
      }
      case 91:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep7( pData->m_byResult, pData->m_sData);
        break;
      }
      case 92:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep8( pData->m_byResult, pData->m_sData);
        break;
      }
      case 93:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep9( pData->m_byResult, pData->m_sData);
        break;
      }
      case 94:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep10( pData->m_byResult, pData->m_sData);
        break;
      }
      case 95:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep11( pData->m_byResult, pData->m_sData);
        break;
      }
      case 97:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep1( pData->m_byResult, pData->m_sData);
        break;
      }
      case 98:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep2( pData->m_byResult, pData->m_sData);
        break;
      }
      case 99:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep3( pData->m_byResult, pData->m_sData);
        break;
      }
      case 100:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep4( pData->m_byResult, pData->m_sData);
        break;
      }
      case 101:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep1( pData->m_byResult, pData->m_sData);
        break;
      }
      case 102:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep2( pData->m_byResult, pData->m_sData);
        break;
      }
      case 103:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep3( pData->m_byResult, pData->m_sData);
        break;
      }
      case 104:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep4( pData->m_byResult, pData->m_sData);
        break;
      }
      case 105:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep5( pData->m_byResult, pData->m_sData);
        break;
      }
      case 106:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep6( pData->m_byResult, pData->m_sData);
        break;
      }
      case 107:
      {
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankUpdateAndSelectGarde( pData->m_byResult, pData->m_sData);
        break;
      }
      case 111:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteLogInCompete( pData->m_sData);
        break;
      }
      case 112:
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        manager->CompleteStoreLimitItem();
        break;
      }
      case 113:
      {
        CItemStoreManager *manager = CItemStoreManager::Instance();
        manager->CompleteDisableInstanceStore( pData->m_sData);
        break;
      }
      case 117:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        manager->CompleteInsertCandidate( pData->m_byResult, pData->m_sData);
        break;
      }
      case 118:
        OutDestGuildbattleCost(pData);
        break;
      case 119:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteInsertElect();
        break;
      }
      case 122:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteSelectElect();
        break;
      }
      case 123:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteRequestRefund( pData);
        break;
      }
      case 124:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteItemChargeRefund( pData);
        break;
      }
      case 125:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteInsertPatriarch( pData);
        break;
      }
      case 128:
        g_VoteSys[static_cast<unsigned __int8>(pData->m_sData[0])].CompleteSelectCharSerial( pData->m_sData);
        break;
      case 129:
      {
        CPostSystemManager *manager = CPostSystemManager::Instace();
        manager->CompletePostReceiverCheck( pData->m_sData);
        break;
      }
      case 136:
      {
        CHonorGuild *honor = CHonorGuild::Instance();
        honor->DQSCompleteInAtradTaxMoney( pData->m_sData);
        break;
      }
      case 138:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteCheckInvalidChar( static_cast<unsigned __int8>(pData->m_sData[0]));
        break;
      }
      case 140:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteReRegist( pData->m_sData);
        break;
      }
      case 141:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteUpdateCheatRegistTime( pData->m_sData);
        break;
      }
      case 142:
      {
        CRaceBossWinRate *mgr = CRaceBossWinRate::Instance();
        mgr->CompleteBossAccmulationWinRate( reinterpret_cast<_qry_case_raceboss_accumulation_winrate *>(pData->m_sData));
        break;
      }
      case 143:
      {
        if (!pData->m_byResult)
        {
          CGuild *guild = &g_Guild[*reinterpret_cast<unsigned int *>(&pData->m_sData[4])];
          if (guild->m_dwSerial == *reinterpret_cast<unsigned int *>(&pData->m_sData[8]))
          {
            guild->Complete_DB_Update_Committee(pData->m_sData);
          }
        }
        break;
      }
      case 144:
      {
        CGuild *guild = &g_Guild[*reinterpret_cast<unsigned int *>(&pData->m_sData[4])];
        if (guild->m_dwSerial == *reinterpret_cast<unsigned int *>(&pData->m_sData[8]))
        {
          guild->CompleteSelectMasterLastConn(*reinterpret_cast<unsigned int *>(&pData->m_sData[16]));
        }
        break;
      }
      case 145:
        g_PotionMgr.Complete_RenameChar_DB_Select(pData->m_byResult, pData->m_sData);
        break;
      case 146:
        g_PotionMgr.Complete_RenameChar_DB_Update(pData->m_byResult, pData->m_sData);
        break;
      case 147:
      {
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteReRegistRollBack( pData->m_sData);
        break;
      }
      case 149:
        CompleteUpdateVoteAvailable(pData->m_sData);
        break;
      case 150:
        CompleteUpdatePlayerVoteInfo(pData->m_sData);
        break;
      case 151:
      {
        CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(&pData->m_sData[4]));
        if (player && player->m_bOper)
        {
          player->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote = 0;
        }
        break;
      }
      case 152:
        CompleteUpdateServerToken(pData->m_sData);
        break;
      case 153:
      {
        unsigned int logoutSeconds = m_pTimeLimitMgr->ClacLastLogoutTimeSec(
          *reinterpret_cast<unsigned int *>(&pData->m_sData[8]));
        BOOL isLimit = 1;
        unsigned __int16 endPlayTime = m_pTimeLimitMgr->GetEndPlayTime();
        isLimit = logoutSeconds >= 60u * static_cast<unsigned int>(endPlayTime);
        unsigned char payload[8]{};
        *reinterpret_cast<unsigned int *>(payload) = static_cast<unsigned int>(isLimit);
        *reinterpret_cast<unsigned int *>(payload + 4) = logoutSeconds;
        unsigned __int8 type[16]{};
        type[0] = 13;
        type[1] = static_cast<unsigned __int8>(-110);
        g_Network.m_pProcess[0]->LoadSendMsg( *reinterpret_cast<unsigned __int16 *>(pData->m_sData), type, reinterpret_cast<char *>(payload), 8u);
        break;
      }
      case 155:
      {
        if (!m_pTimeLimitMgr->UpdatePlayerStatus(
              *reinterpret_cast<unsigned __int16 *>(pData->m_sData),
              *reinterpret_cast<unsigned int *>(&pData->m_sData[12]),
              static_cast<unsigned __int8>(pData->m_sData[16])))
        {
          m_logSystemError.Write( "Error_DQSComplete_qry_case_insert_timelimit_info");
        }
        m_pTimeLimitMgr->ReSetPercent( *reinterpret_cast<unsigned __int16 *>(pData->m_sData));
        break;
      }
      case 161:
        CompleteUpdateSetLimitRun(pData->m_byResult, pData->m_sData);
        break;
      case 165:
      {
        CGoldenBoxItemMgr *mgr = CGoldenBoxItemMgr::Instance();
        mgr->BoxItemDataCopy();
        break;
      }
      case 171:
        Complete_Select_RegeAvator_For_Lobby_Logout(reinterpret_cast<_qry_case_lobby_logout *>(pData->m_sData));
        break;
      case 175:
        Complete_db_Update_Data_For_Post_Send(pData->m_sData);
        break;
      case 176:
        Complete_db_Update_Data_For_Trade(pData->m_sData);
        break;
      default:
        break;
    }

    pData->m_bUse = false;
    pData->m_bLoad = false;
    m_listDQSDataEmpty.PushNode_Back(outIndex[0]);
  }
}

void CMainThread::Reged_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    user->Reged_Char_Complete(
      pData->m_byResult,
      reinterpret_cast<_REGED *>(&pData->m_sData[4]),
      reinterpret_cast<_NOT_ARRANGED_AVATOR_DB *>(&pData->m_sData[811]));
  }
}

void CMainThread::Insert_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    user->Insert_Char_Complete(pData->m_byResult, reinterpret_cast<_REGED_AVATOR_DB *>(&pData->m_sData[17]));
  }
}

void CMainThread::Delete_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    user->Delete_Char_Complete(pData->m_byResult, pData->m_sData[0]);
  }
}

void CMainThread::Select_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    char *data = pData->m_sData;
    unsigned __int8 extTrunkOldSlot = static_cast<unsigned __int8>(data[37505]);
    user->Select_Char_Complete(
      pData->m_byResult,
      reinterpret_cast<_AVATOR_DATA *>(data + 4),
      reinterpret_cast<bool *>(data + 37219),
      *reinterpret_cast<unsigned int *>(data + 37320),
      *reinterpret_cast<unsigned int *>(data + 37324),
      *reinterpret_cast<unsigned int *>(data + 37328),
      reinterpret_cast<bool *>(data + 37332),
      static_cast<unsigned __int8>(data[37432]),
      *reinterpret_cast<long double *>(data + 37440),
      *reinterpret_cast<long double *>(data + 37448),
      static_cast<unsigned __int8>(data[37456]),
      reinterpret_cast<bool *>(data + 37465),
      extTrunkOldSlot);
    _db_complete_event_classrefine(
      pData->m_idWorld.wIndex,
      pData->m_idWorld.dwSerial,
      static_cast<unsigned __int8>(data[37464]),
      *reinterpret_cast<unsigned int *>(data + 37460));
  }
}
void CMainThread::Logout_Account_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    user->Exit_Account_Complete(pData->m_byResult);
  }
}

void CMainThread::Lobby_Account_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    if (pData->m_byResult)
    {
      char buffer[144]{};
      sprintf(
        buffer,
        "Lobby_Account_Complete Database Error(%d) account(%s , %d), char(%s, %d)",
        pData->m_byResult,
        user->m_szAccountID,
        user->m_dwAccountSerial,
        user->m_aszAvatorName,
        user->m_dwSerial);
      g_Network.Close(0, pData->m_idWorld.wIndex, false, buffer);
    }
    else
    {
      user->Lobby_Char_Complete(pData->m_byResult);
    }
  }
}

void CMainThread::Cont_UserSave_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    char *data = pData->m_sData;
    if (data[34814])
    {
      CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, pData->m_dwAccountSerial);
      if (player && player->m_bOper)
      {
        for (int nIndex = 0; nIndex < 50; ++nIndex)
        {
          char *postEntry = data + (297 * nIndex) + 19964;
          if (data[297 * nIndex + 19993] && postEntry[31] && postEntry[30])
          {
            CPostData *postData = player->m_Param.m_PostStorage.GetPostDataFromInx(nIndex);
            if (postData && !user->m_AvatorData.dbPostData.dbPost.m_PostList[nIndex].bNew)
            {
              postData->m_dwPSSerial = *reinterpret_cast<unsigned int *>(postEntry);
            }
          }
        }
      }
    }

    if (user->m_AvatorData.m_byHSKTime <= 2u && user->m_AvatorData.m_byCristalBattleDBInfo != 3
        && !user->m_AvatorData.m_bCristalBattleDateUpdate)
    {
      user->m_AvatorData.m_bCristalBattleDateUpdate = 1;
      user->m_AvatorData_bk.m_bCristalBattleDateUpdate = 1;
    }

    user->m_AvatorData.dbCutting.ReSetOldDataLoad();

    if (pData->m_byResult)
    {
      char buffer[144]{};
      sprintf(
        buffer,
        "Cont_UserSave_Complete Database Error(%d) account(%s , %d), char(%s, %d)",
        pData->m_byResult,
        user->m_szAccountID,
        user->m_dwAccountSerial,
        user->m_aszAvatorName,
        user->m_dwSerial);
      g_Network.Close(0, pData->m_idWorld.wIndex, 0, buffer);
    }
    else
    {
      user->Cont_UserSave_Complete(pData->m_byResult, reinterpret_cast<_AVATOR_DATA *>(data + 4));
      if (data[74434])
      {
        _db_complete_update_event_classrefine(pData->m_idWorld.wIndex, pData->m_idWorld.dwSerial);
      }
    }
  }
}

void CMainThread::Alive_Char_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    user->Alive_Char_Complete(
      pData->m_byResult,
      static_cast<unsigned __int8>(pData->m_sData[0]),
      *reinterpret_cast<unsigned int *>(&pData->m_sData[4]),
      reinterpret_cast<_REGED *>(&pData->m_sData[26]));
  }
}
void CMainThread::SendWebRaceBossSMS(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  if (pData->m_byResult)
  {
    *reinterpret_cast<unsigned int *>(data + 276) = 0;
  }

  char *msg = data + 4;
  char *name = data + 259;
  CRaceBossMsgController *controller = CRaceBossMsgController::Instance();
  controller->Send(
    static_cast<unsigned __int8>(data[280]),
    *reinterpret_cast<unsigned int *>(data + 276),
    name,
    msg,
    *reinterpret_cast<unsigned int *>(data));
}

void CMainThread::AddGuildBattleSchdule(_DB_QRY_SYN_DATA *pData)
{
  unsigned int *payload = reinterpret_cast<unsigned int *>(pData->m_sData);
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->AddComplete(pData->m_byResult, payload[3], payload[0], payload[5]);
}

void CMainThread::UpdateGuildBattleWinLoseRankInfo(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  if (pData->m_byResult)
  {
    GUILD_BATTLE::CGuildBattleLogger *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
    logger->Log(
      "void CMainThread::UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA* pData) : Win R(%u) Serial(%u) Lose R(%u) Serial(%u) Fail!",
      static_cast<unsigned __int8>(data[0]),
      *reinterpret_cast<unsigned int *>(data + 4),
      static_cast<unsigned __int8>(data[8]),
      *reinterpret_cast<unsigned int *>(data + 12));
  }
  else
  {
    _qry_case_loadguildbattlerank qry{};
    qry.byRace = static_cast<unsigned __int8>(data[0]);
    int size = static_cast<int>(qry.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x21u, reinterpret_cast<char *>(&qry), size);
    if (data[0] != data[8])
    {
      qry.byRace = static_cast<unsigned __int8>(data[8]);
      size = static_cast<int>(qry.size());
      PushDQSData(0xFFFFFFFF, nullptr, 0x21u, reinterpret_cast<char *>(&qry), size);
    }

    _qry_case_load_guildbattle_totalrecord total{};
    total.dwGuildSerial = *reinterpret_cast<unsigned int *>(data + 4);
    size = static_cast<int>(total.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x39u, reinterpret_cast<char *>(&total), size);

    total.dwGuildSerial = *reinterpret_cast<unsigned int *>(data + 12);
    size = static_cast<int>(total.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x39u, reinterpret_cast<char *>(&total), size);
  }
}

void CMainThread::UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  if (pData->m_byResult)
  {
    GUILD_BATTLE::CGuildBattleLogger *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
    logger->Log(
      "void CMainThread::UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA* pData) : 1P R(%u) Serial(%u) 2P R(%u) Serial(%u) Fail!",
      static_cast<unsigned __int8>(data[0]),
      *reinterpret_cast<unsigned int *>(data + 4),
      static_cast<unsigned __int8>(data[8]),
      *reinterpret_cast<unsigned int *>(data + 12));
  }
  else
  {
    _qry_case_loadguildbattlerank qry{};
    qry.byRace = static_cast<unsigned __int8>(data[0]);
    int size = static_cast<int>(qry.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x21u, reinterpret_cast<char *>(&qry), size);
    if (data[0] != data[8])
    {
      qry.byRace = static_cast<unsigned __int8>(data[8]);
      size = static_cast<int>(qry.size());
      PushDQSData(0xFFFFFFFF, nullptr, 0x21u, reinterpret_cast<char *>(&qry), size);
    }

    _qry_case_load_guildbattle_totalrecord total{};
    total.dwGuildSerial = *reinterpret_cast<unsigned int *>(data + 4);
    size = static_cast<int>(total.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x39u, reinterpret_cast<char *>(&total), size);

    total.dwGuildSerial = *reinterpret_cast<unsigned int *>(data + 12);
    size = static_cast<int>(total.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x39u, reinterpret_cast<char *>(&total), size);
  }
}

void CMainThread::UpdateLoadGuildBattleRank(_DB_QRY_SYN_DATA *pData)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(pData->m_sData);
  unsigned __int8 *loadData = reinterpret_cast<unsigned __int8 *>(&pData->m_sData[1]);
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->CompleteUpdateRank( pData->m_byResult, data[0], loadData);
}
void CMainThread::InGuildbattleCost(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  CGuild *guild = &g_Guild[*reinterpret_cast<unsigned int *>(data)];
  if (guild->m_dwSerial == *reinterpret_cast<unsigned int *>(data + 4))
  {
    guild->m_bIOWait = 0;
    if (!pData->m_byResult)
    {
      guild->m_byMoneyOutputKind = 4;
      long double ioGold = static_cast<long double>(*reinterpret_cast<int *>(data + 8));
      long double ioDalant = static_cast<long double>(*reinterpret_cast<int *>(data + 12));
      guild->IOMoney(
        "Scramble Cost",
        *reinterpret_cast<unsigned int *>(data + 4),
        ioDalant,
        ioGold,
        *reinterpret_cast<long double *>(data + 32),
        *reinterpret_cast<long double *>(data + 24),
        reinterpret_cast<unsigned __int8 *>(data + 16),
        true);
    }
  }
}

void CMainThread::OutSrcGuildbattleCost(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  CGuild *guild = &g_Guild[*reinterpret_cast<unsigned int *>(data)];
  if (guild->m_dwSerial == *reinterpret_cast<unsigned int *>(data + 4))
  {
    guild->m_bIOWait = 0;
    if (data[56])
    {
      guild->SendMsg_GuildBattleSuggestResult(0xA6u, guild->m_GuildBattleSugestMatter.pkDest->m_wszName);
      guild->m_GuildBattleSugestMatter.Clear();
    }
    else if (!pData->m_byResult)
    {
      guild->m_byMoneyOutputKind = 4;
      long double ioGold = static_cast<long double>(*reinterpret_cast<int *>(data + 8));
      long double ioDalant = static_cast<long double>(*reinterpret_cast<int *>(data + 12));
      guild->IOMoney(
        "Scramble Cost",
        *reinterpret_cast<unsigned int *>(data + 4),
        ioDalant,
        ioGold,
        *reinterpret_cast<long double *>(data + 32),
        *reinterpret_cast<long double *>(data + 24),
        reinterpret_cast<unsigned __int8 *>(data + 16),
        false);
      guild->CompleteOutGuildbattleCost(
        *reinterpret_cast<unsigned int *>(data + 40),
        *reinterpret_cast<unsigned int *>(data + 44),
        *reinterpret_cast<unsigned int *>(data + 48),
        *reinterpret_cast<unsigned int *>(data + 52));
    }
  }
}

void CMainThread::OutDestGuildbattleCost(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  CGuild *guild = &g_Guild[*reinterpret_cast<unsigned int *>(data)];
  if (guild->m_dwSerial == *reinterpret_cast<unsigned int *>(data + 4))
  {
    guild->m_bIOWait = 0;
    if (pData->m_byResult)
    {
      const int dTotalDalant = 5000;
      g_Main.m_logSystemError.Write(
        "OutDestGuildbattleCost(...) : (%d)Guld Cur Dal(%f) Gold(%f) GuildBattleCost(%d)Gold _qry_case_out_guildbattlecost Fail!",
        *reinterpret_cast<unsigned int *>(data + 4),
        guild->m_dTotalDalant,
        guild->m_dTotalGold,
        dTotalDalant);
    }
    else
    {
      guild->m_byMoneyOutputKind = 4;
      long double ioGold = static_cast<long double>(*reinterpret_cast<int *>(data + 8));
      long double ioDalant = static_cast<long double>(*reinterpret_cast<int *>(data + 12));
      guild->IOMoney(
        "Scramble Cost",
        *reinterpret_cast<unsigned int *>(data + 4),
        ioDalant,
        ioGold,
        *reinterpret_cast<long double *>(data + 32),
        *reinterpret_cast<long double *>(data + 24),
        reinterpret_cast<unsigned __int8 *>(data + 16),
        false);
    }
  }
}

void CMainThread::InGuildbattleRewardMoney(_DB_QRY_SYN_DATA *pData)
{
  char *data = pData->m_sData;
  CGuild *guild = &g_Guild[*reinterpret_cast<unsigned int *>(data)];
  if (guild->m_dwSerial == *reinterpret_cast<unsigned int *>(data + 4))
  {
    guild->m_bIOWait = 0;
    if (!pData->m_byResult)
    {
      guild->m_byMoneyOutputKind = 5;
      long double ioGold = static_cast<long double>(*reinterpret_cast<int *>(data + 8));
      long double ioDalant = static_cast<long double>(*reinterpret_cast<int *>(data + 12));
      guild->IOMoney(
        "Scramble Reward",
        *reinterpret_cast<unsigned int *>(data + 4),
        ioDalant,
        ioGold,
        *reinterpret_cast<long double *>(data + 32),
        *reinterpret_cast<long double *>(data + 24),
        reinterpret_cast<unsigned __int8 *>(data + 16),
        true);
    }
  }
}

void CMainThread::UpdateReservedGuildBattleSchedule(_DB_QRY_SYN_DATA *pData)
{
  unsigned int *data = reinterpret_cast<unsigned int *>(pData->m_sData);
  unsigned __int8 *loadData = reinterpret_cast<unsigned __int8 *>(&pData->m_sData[8]);
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->CompleteUpdateReservedSchedule( *data, loadData);
}

void CMainThread::CompleteLoadGuildBattleTotalRecord(unsigned __int8 byRet, char *pLoadData)
{
  unsigned int *payload = reinterpret_cast<unsigned int *>(pLoadData);
  if (!byRet)
  {
    CGuild *guild = GetGuildDataFromSerial(g_Guild, 500, *payload);
    if (guild)
    {
      guild->UpdateGuildBattleWinCnt(payload[1], payload[3], payload[2]);
    }
    else
    {
      m_logSystemError.Write(
        "CMainThread::CompleteLoadGuildBattleTotalRecord( BYTE byRet(%u), char * pLoadData ) :GetGuildDataFromSerial( g_Guild, MAX_GUILD, pSheet->dwGuildSerial(%u) ) == NULL!",
        *payload,
        *payload);
    }
  }
}

void CMainThread::InAtradTaxMoney(_DB_QRY_SYN_DATA *pData)
{
  unsigned __int8 *data = reinterpret_cast<unsigned __int8 *>(pData->m_sData);
  CUnmannedTraderTaxRateManager *manager = CUnmannedTraderTaxRateManager::Instance();
  manager->DQSCompleteInAtradTaxMoney(*data, pData->m_sData);
}
void CMainThread::Load_PostStorage_Complete(char *pData)
{
  if (pData[4] == 1)
  {
    m_logSystemError.Write(
      "Load_PostStorage_Complete() : Select_PostStorageList(Serial:%d) => failed",
      *reinterpret_cast<unsigned int *>(pData));
  }

  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pData));
  if (player && player->m_bLoad && pData[4] != 1)
  {
    CPostStorage *postStorage = &player->m_Param.m_PostStorage;
    unsigned int count = *reinterpret_cast<unsigned int *>(pData + 8);

    for (unsigned int n = 0; n < count; ++n)
    {
      char *entry = pData + 80 * n + 16;
      if (*reinterpret_cast<unsigned __int16 *>(pData + 80 * n + 92) != 255)
      {
        unsigned __int8 type = static_cast<unsigned __int8>(entry[4]);
        unsigned __int16 storageIndex = *reinterpret_cast<unsigned __int16 *>(entry + 76);
        postStorage->AddPostTitleDataByStorageIndex(
          storageIndex,
          type,
          *reinterpret_cast<unsigned int *>(entry),
          static_cast<unsigned __int8>(entry[5]),
          entry + 6,
          entry + 23,
          *reinterpret_cast<_INVENKEY *>(entry + 44),
          *reinterpret_cast<unsigned long long *>(entry + 48),
          *reinterpret_cast<unsigned int *>(entry + 56),
          *reinterpret_cast<unsigned int *>(entry + 72),
          *reinterpret_cast<unsigned long long *>(entry + 64));
      }
    }

    for (unsigned int n = 0; n < count; ++n)
    {
      char *entry = pData + 80 * n + 16;
      if (*reinterpret_cast<unsigned __int16 *>(pData + 80 * n + 92) == 255)
      {
        postStorage->AddPostTitleData(
          static_cast<unsigned __int8>(entry[4]),
          *reinterpret_cast<unsigned int *>(entry),
          static_cast<unsigned __int8>(entry[5]),
          entry + 6,
          entry + 23,
          *reinterpret_cast<_INVENKEY *>(entry + 44),
          *reinterpret_cast<unsigned long long *>(entry + 48),
          *reinterpret_cast<unsigned int *>(entry + 56),
          *reinterpret_cast<unsigned int *>(entry + 72),
          *reinterpret_cast<unsigned long long *>(entry + 64));
      }
    }

    for (int n = 0; n < 50; ++n)
    {
      CPostData *postData = &postStorage->m_PostData[n];
      if (postData->m_byState != 255)
      {
        int itemKey = postData->m_Key.CovDBKey();
        player->m_pUserDB->SetDBPostData(
          n,
          postData->m_dwPSSerial,
          postData->m_nNumber,
          postData->m_byState,
          itemKey,
          postData->m_dwDur,
          postData->m_dwUpt,
          postData->m_dwGold,
          postData->m_bUpdateIndex,
          postData->m_lnUID);
      }
    }

    if (count)
    {
      bool hasItem = false;
      bool hasGold = false;
      int size = postStorage->GetSize();
      for (int n = 0; n < 50; ++n)
      {
        if (postStorage->m_PostData[n].GetState() != 255)
        {
          hasItem = postStorage->m_PostData[n].m_Key.IsFilled();
          hasGold = postStorage->m_PostData[n].m_dwGold != 0;
          unsigned __int8 state = postStorage->m_PostData[n].GetState();
          player->SendMsg_PostDelivery(
            postStorage->m_PostData[n].m_nNumber,
            n,
            postStorage->m_PostData[n].m_wszSendName,
            postStorage->m_PostData[n].m_wszTitle,
            hasItem,
            hasGold,
            state);
          if (!postStorage->m_PostData[n].GetState())
          {
            if (hasItem || hasGold)
            {
              CPlayer::s_MgrItemHistory.post_receive(
                &postStorage->m_PostData[n],
                player->m_szItemHistoryFileName);
            }
            postStorage->m_PostData[n].SetState(2u);
            player->UpdatePost(n);
          }
          if (--size <= 0)
          {
            break;
          }
        }
      }
    }

    CPlayer::s_MgrItemHistory.post_storage(
      &player->m_Param.m_PostStorage,
      player->m_szItemHistoryFileName);

    _qry_case_post_return_list_get qry{};
    qry.dwMasterSerial = player->m_pUserDB->m_dwSerial;
    int size = static_cast<int>(qry.size());
    PushDQSData(0xFFFFFFFF, nullptr, 0x50u, reinterpret_cast<char *>(&qry), size);
  }
}
void CMainThread::Load_ReturnPost_Complete(char *pData)
{
  if (pData[8] == 1)
  {
    m_logSystemError.Write(
      "Load_ReturnPost_Complete() : Select_ReturnPost(Serial:%d) => failed",
      *reinterpret_cast<unsigned int *>(pData + 4));
  }

  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pData + 4));
  if (player && player->m_bLoad && pData[8] != 1)
  {
    CPostReturnStorage *returnStorage = &player->m_Param.m_ReturnPostStorage;
    unsigned int count = *reinterpret_cast<unsigned int *>(pData + 12);
    for (unsigned int j = 0; j < count; ++j)
    {
      char *entry = pData + 280 * j + 16;
      CPostData *post = returnStorage->AddReturnPost(
        pData[280 * j + 292],
        *reinterpret_cast<unsigned int *>(entry),
        static_cast<unsigned __int8>(pData[280 * j + 20]),
        &pData[280 * j + 21],
        &pData[280 * j + 38],
        &pData[280 * j + 59],
        *reinterpret_cast<_INVENKEY *>(&pData[280 * j + 260]),
        *reinterpret_cast<unsigned long long *>(&pData[280 * j + 264]),
        *reinterpret_cast<unsigned int *>(&pData[280 * j + 272]),
        *reinterpret_cast<unsigned int *>(&pData[280 * j + 288]),
        *reinterpret_cast<unsigned long long *>(&pData[280 * j + 280]));
      if (post->m_Key.IsFilled() || post->m_dwGold)
      {
        CPlayer::s_MgrItemHistory.post_returnreceive(post, player->m_szItemHistoryFileName);
      }
    }

    CPlayer::s_MgrItemHistory.return_post_storage(
      &player->m_Param.m_ReturnPostStorage,
      player->m_szItemHistoryFileName);
    player->m_bPostLoad = 1;

    if (count)
    {
      int size = returnStorage->GetSize();
      for (int j = 0; j < 10; ++j)
      {
        if (returnStorage->m_PostData[j].GetState() != 255)
        {
          CPostData *post = &returnStorage->m_PostData[j];
          player->SendMsg_PostReturn(
            post->m_byErrCode,
            post->m_dwPSSerial,
            post->m_wszRecvName,
            post->m_wszTitle,
            post->m_wszContent,
            post->m_Key.byTableCode,
            post->m_Key.wItemIndex,
            post->m_dwDur,
            post->m_dwUpt,
            post->m_dwGold);
          if (--size <= 0)
          {
            break;
          }
        }
      }
    }
  }
}

void CMainThread::Load_Content_Complete(char *pData)
{
  if (pData[4] == 1)
  {
    m_logSystemError.Write(
      "Load_Content_Complete() : Select_PostContent() : MasterSerial=%d ,PostSerial=%d",
      *reinterpret_cast<unsigned int *>(pData + 8),
      *reinterpret_cast<unsigned int *>(pData));
  }

  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pData + 8));
  if (player && player->m_bOper && pData[4] != 1)
  {
    CPostData *post = player->m_Param.m_PostStorage.GetPostDataFromInx(
      *reinterpret_cast<unsigned int *>(pData + 12));
    if (post)
    {
      post->SetPostContent(pData + 16);
      player->SendMsg_PostContent(
        0,
        *reinterpret_cast<unsigned int *>(pData + 12),
        post->m_wszContent,
        post->m_Key.byTableCode,
        post->m_Key.wItemIndex,
        post->m_dwDur,
        post->m_dwUpt,
        post->m_dwGold);
    }
  }
}
void CMainThread::CompleteUpdateVoteAvailable(char *pData)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pData + 8));
  if (player && player->m_bOper)
  {
    player->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable = static_cast<unsigned __int8>(*pData);
  }
}

void CMainThread::CompleteUpdatePlayerVoteInfo(char *pData)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pData + 20));
  if (player && player->m_bOper)
  {
    player->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable = pData[5];
    player->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime = *reinterpret_cast<unsigned int *>(pData);
    player->m_pUserDB->m_AvatorData.dbSupplement.byVoted = pData[4];
    player->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt = *reinterpret_cast<unsigned __int16 *>(pData + 6);
    player->m_pUserDB->m_AvatorData.dbSupplement.dwScanerGetDate = *reinterpret_cast<unsigned int *>(pData + 8);
  }
}

void CMainThread::CompleteUpdateServerToken(char *pData)
{
  g_Main.m_dwServerResetToken = *reinterpret_cast<unsigned int *>(pData);
}

void CMainThread::CompleteUpdateSetLimitRun(char byRet, char *pData)
{
  unsigned char payload[8]{};
  payload[0] = static_cast<unsigned __int8>(byRet);
  std::memcpy(payload + 1, pData, 6);
  payload[7] = static_cast<unsigned __int8>(pData[6]);

  unsigned __int8 type[16]{};
  type[0] = 1;
  type[1] = 26;
  g_Network.m_pProcess[1]->LoadSendMsg( 0, type, reinterpret_cast<char *>(payload), 8u);
}
void CMainThread::Complete_Select_RegeAvator_For_Lobby_Logout(_qry_case_lobby_logout *pSheet)
{
  CUserDB::s_MgrLobbyHistory.lobby_disconnect(pSheet, pSheet->szLobbyHistoryFileName);
}

void CMainThread::Complete_db_Update_Data_For_Post_Send(char *pSheet)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(pSheet));
  if (player && player->m_bOper)
  {
    player->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwGold = player->m_pUserDB->m_AvatorData.dbAvator.m_dwGold;
    player->m_pUserDB->m_AvatorData_bk.dbInven = player->m_pUserDB->m_AvatorData.dbInven;
    player->SendMsg_PostSendReply(0);
  }
}

void CMainThread::Complete_db_Update_Data_For_Trade(char *pSheet)
{
  for (int i = 0; i < 2; ++i)
  {
    CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, *reinterpret_cast<unsigned int *>(&pSheet[32 * i]));
    if (player && player->m_bOper)
    {
      player->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwDalant = player->m_pUserDB->m_AvatorData.dbAvator.m_dwDalant;
      player->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwGold = player->m_pUserDB->m_AvatorData.dbAvator.m_dwGold;
      player->m_pUserDB->m_AvatorData_bk.dbInven = player->m_pUserDB->m_AvatorData.dbInven;
    }
  }
}

void CMainThread::_db_complete_event_classrefine(
  unsigned __int16 wSock,
  unsigned int dwAvatorSerial,
  unsigned __int8 byRefinedCnt,
  unsigned int dwRefineDate)
{
  _event_participant_classrefine state{};
  state.nSock = wSock;
  state.nAvatorSerial = dwAvatorSerial;
  state.nCurRefineCnt = byRefinedCnt;
  state.dwRefineDate = dwRefineDate;
  int size = state.size();
  g_Main.m_pRFEvent_ClassRefine->SetPlayerState(&state, size);
}

void CMainThread::_db_complete_update_event_classrefine(unsigned __int16 wSock, unsigned int dwAvatorSerial)
{
  auto *state = g_Main.m_pRFEvent_ClassRefine->GetPlayerState(wSock, dwAvatorSerial);
  if (state)
  {
    state->bChange = false;
  }
}
