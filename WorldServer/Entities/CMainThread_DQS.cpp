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
#include "DqsOnRunPayloads.h"
#include "GlobalObjects.h"
#include "GuildBattleTypes.h"
#include "lt_qry_case_unmandtrader_select_list.h"
#include "Packet/ZoneAccountPacket.h"
#include "insert_trc_info.h"
#include "qry_case_inputgmoney.h"
#include "PatriarchElectProcessor.h"
#include "RFEvent_ClassRefine.h"
#include "TRC_AutoTrade.h"
#include "log_case_charselect.h"
#include "log_sheet_lv.h"
#include "qry_case_alive_char.h"
#include "qry_case_amine_header.h"
#include "qry_case_addpvppoint.h"
#include "qry_case_candidate_scalar_payloads.h"
#include "qry_case_cash_limsale.h"
#include "qry_case_contsave.h"
#include "qry_case_discharge_patriarch.h"
#include "qry_case_disjointguild.h"
#include "qry_case_guild_battel_result_log.h"
#include "qry_case_in_guildbattlerewardmoney.h"
#include "qry_case_insert_candidate.h"
#include "qry_case_insert_orelog.h"
#include "qry_case_insertitem.h"
#include "qry_case_in_atrade_tax.h"
#include "qry_case_make_storage.h"
#include "qry_case_post_list_regi.h"
#include "qry_case_post_serial_check.h"
#include "qry_case_post_send.h"
#include "qry_case_rank_racerank_guildrank.h"
#include "qry_case_request_refund.h"
#include "qry_case_select_timelimit_info.h"
#include "qry_case_selfleave.h"
#include "qry_case_unmandtrader_buy_update.h"
#include "qry_case_unmandtrader_buy_get_info.h"
#include "qry_case_unmandtrader_cancelitem.h"
#include "qry_case_unmandtrader_cheat_updateregisttime.h"
#include "qry_case_unmandtrader_lazyclean_flags.h"
#include "qry_case_unmandtrader_log_in_proc_update_complete.h"
#include "qry_case_unmandtrader_re_registsingleitem.h"
#include "qry_case_unmandtrader_re_registsingleitem_roll_back.h"
#include "qry_case_unmandtrader_registsingleitem.h"
#include "qry_case_unmandtrader_update_reprice.h"
#include "qry_case_unmandtrader_updateitemstate.h"
#include "qry_case_update_macro_data.h"
#include "qry_case_update_pvp_info.h"
#include "qry_case_updateclearguildbattleDayInfo.h"
#include "qry_case_updatedrawguildbattlerank.h"
#include "qry_case_updateweeklyguildpvppointsum.h"
#include "qry_case_updatewinloseguildbattlerank.h"
#include "qry_case_updatereservedschedule.h"
#include "qry_sheet_delete.h"
#include "qry_sheet_insert.h"
#include "qry_sheet_lobby.h"
#include "qry_sheet_load.h"
#include "qry_sheet_reged.h"
#include "log_change_class_after_init_class.h"
#include "time_limit_accum_logouttime_result_zocl.h"
#include "qry_logout.h"






void CMainThread::OnDQSRun()
{
  unsigned int queryIndex;
  m_DBFrameRate.CalcSpeedPerFrame();
  if (m_pWorldDB )
    m_pWorldDB->CheckLogFileHour();
  while (m_listDQSData.PopNode_Front(&queryIndex) )
  {
    g_Main.m_tmDbUpdate = static_cast<int>(std::time(nullptr));
    _DB_QRY_SYN_DATA *queryEntry = &m_DBQrySynData[queryIndex];
    queryEntry->m_byResult = 0;
    const int queryCase = queryEntry->m_byQryCase;
    switch (queryCase )
    {
      case 0:
      {
        _qry_sheet_reged *regedQuery = reinterpret_cast<_qry_sheet_reged *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Reged_Avator(
          regedQuery->dwAccountSerial,
          regedQuery->RegedData,
          regedQuery->ArrangedData,
          regedQuery->in_szIP);
        break;
      }
      case 1:
      {
        _qry_sheet_insert *insertQuery = reinterpret_cast<_qry_sheet_insert *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Insert_Avator(
          insertQuery->dwAccountSerial,
          insertQuery->szAccountID,
          &insertQuery->InsertData,
          &insertQuery->InsertData.m_dwRecordNum);
        break;
      }
      case 2:
      {
        _qry_sheet_delete *deleteQuery = reinterpret_cast<_qry_sheet_delete *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Delete_Avator(deleteQuery->dwAvatorSerial, deleteQuery->byRaceCode);
        break;
      }
      case 3:
      {
        _qry_sheet_load *loadQuery = reinterpret_cast<_qry_sheet_load *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Load_Avator(
          loadQuery->dwAvatorSerial,
          queryEntry->m_dwAccountSerial,
          &loadQuery->LoadData,
          loadQuery->bAddItem,
          &loadQuery->dwAddDalant,
          &loadQuery->dwAddGold,
          loadQuery->bTrunkAddItem,
          &loadQuery->byTrunkOldSlot,
          &loadQuery->dTrunkOldDalant,
          &loadQuery->dTrunkOldGold,
          &loadQuery->bCreateTrunkFree,
          loadQuery->bExtTrunkAddItem,
          &loadQuery->byExtTrunkOldSlot,
          0,
          &loadQuery->dwCheckSum);
        if (queryEntry->m_byResult
          || (CUserDB::ReRangeClientIndex(&loadQuery->LoadData),
              CUserDB::CheckDQSLoadCharacterData(&loadQuery->LoadData)))
        {
          if (_db_load_event_classrefine(
                loadQuery->dwAvatorSerial,
                &loadQuery->byRefinedCnt,
                &loadQuery->dwRefineDate) )
          {
            queryEntry->m_byResult = 72;
          }
        }
        else
        {
          queryEntry->m_byResult = 38;
        }
        break;
      }
      case 5:
      {
        auto *logoutQuery = reinterpret_cast<_qry_sheet_logout *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Update_Avator(
          logoutQuery->dwAvatorSerial,
          &logoutQuery->NewData,
          &logoutQuery->OldData,
          logoutQuery->bCheckLowHigh);
        db_Update_PostStorage(logoutQuery->dwAvatorSerial, &logoutQuery->NewData, &logoutQuery->OldData);
        if (logoutQuery->bUpdateRefineCnt)
        {
          queryEntry->m_byResult = _db_update_event_classrefine(
            queryEntry->m_idWorld.wIndex,
            logoutQuery->dwAvatorSerial,
            logoutQuery->byRefinedCnt,
            logoutQuery->dwRefineDate);
        }
        _db_Update_SFDelayData(logoutQuery->dwAvatorSerial, &logoutQuery->NewData);
        break;
      }
      case 6:
      {
        auto *lobbyQuery = reinterpret_cast<_qry_sheet_lobby *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Update_Avator(
          lobbyQuery->dwAvatorSerial,
          &lobbyQuery->NewData,
          &lobbyQuery->OldData,
          1);
        db_Update_PostStorage(lobbyQuery->dwAvatorSerial, &lobbyQuery->NewData, &lobbyQuery->OldData);
        if (lobbyQuery->bUpdateRefineCnt)
        {
          queryEntry->m_byResult = _db_update_event_classrefine(
            queryEntry->m_idWorld.wIndex,
            lobbyQuery->dwAvatorSerial,
            lobbyQuery->byRefinedCnt,
            lobbyQuery->dwRefineDate);
        }
        _db_Update_SFDelayData(lobbyQuery->dwAvatorSerial, &lobbyQuery->NewData);
        break;
      }
      case 7:
      {
        auto *levelLog = reinterpret_cast<_log_sheet_lv *>(queryEntry->m_sData);
        db_Log_AvatorLevel(levelLog->dwTotalPlayMin, levelLog->dwAvatorSerial, levelLog->byLv);
        break;
      }
      case 8:
      {
        auto *userNumLog = reinterpret_cast<_log_sheet_usernum *>(queryEntry->m_sData);
        db_Log_UserNum(userNumLog->nAveragePerHour, userNumLog->nMaxPerHour);
        break;
      }
      case 9:
      {
        auto *economyLog = reinterpret_cast<_log_sheet_economy *>(queryEntry->m_sData);
        _worlddb_economy_history_info pEconomyData{};
        pEconomyData.dwManageValue = static_cast<unsigned int>(economyLog->nMgrValue);
        std::memcpy(pEconomyData.dTradeGold, economyLog->dTradeGold, sizeof(pEconomyData.dTradeGold));
        std::memcpy(pEconomyData.dTradeDalant, economyLog->dTradeDalant, sizeof(pEconomyData.dTradeDalant));
        std::memcpy(pEconomyData.dMineOre, economyLog->dCutOre, sizeof(pEconomyData.dMineOre));
        std::memcpy(pEconomyData.dCutOre, &economyLog->dCutOre[3], sizeof(pEconomyData.dCutOre));
        db_Insert_Economy_History(economyLog->dwDate, &pEconomyData);
        break;
      }
      case 10:
      {
        _log_case_charselect *charSelectLog = reinterpret_cast<_log_case_charselect *>(queryEntry->m_sData);
        db_Insert_CharacSelect_Log(
          charSelectLog->dwIDSerial,
          charSelectLog->szID,
          charSelectLog->dwNameSerial,
          charSelectLog->wszName,
          charSelectLog->dwYear,
          charSelectLog->byMonth,
          charSelectLog->byDay,
          charSelectLog->byHour,
          charSelectLog->byMin,
          charSelectLog->bySec);
        break;
      }
      case 11:
      {
        auto *updatePvpInfoQuery = reinterpret_cast<_qry_case_update_pvp_info *>(queryEntry->m_sData);
        db_Update_PvpInfo(
          updatePvpInfoQuery->dwSerial,
          updatePvpInfoQuery->byGrade,
          updatePvpInfoQuery->zClassHistory,
          updatePvpInfoQuery->dPvpPoint);
        break;
      }
      case 12:
      {
        auto *contSaveQuery = reinterpret_cast<_qry_case_contsave *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Update_Avator(
          contSaveQuery->dwAvatorSerial,
          &contSaveQuery->NewData,
          &contSaveQuery->OldData,
          1);
        if (contSaveQuery->bUpdateRefineCnt)
        {
          queryEntry->m_byResult = _db_update_event_classrefine(
            queryEntry->m_idWorld.wIndex,
            contSaveQuery->dwAvatorSerial,
            contSaveQuery->byRefinedCnt,
            contSaveQuery->dwRefineDate);
        }
        db_Update_PostStorage(contSaveQuery->dwAvatorSerial, &contSaveQuery->NewData, &contSaveQuery->OldData);
        break;
      }
      case 13:
      {
        auto *addPvpPointQuery = reinterpret_cast<_qry_case_addpvppoint *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Add_PvpPoint(
          addPvpPointQuery->dwSerial,
          addPvpPointQuery->dwPoint,
          addPvpPointQuery->dwCashBag);
        break;
      }
      case 14:
      {
        auto *insertItemQuery = reinterpret_cast<_qry_case_insertitem *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Insert_Item(
          insertItemQuery->dwSerial,
          insertItemQuery->dwItemCodeK,
          insertItemQuery->dwItemCodeD,
          insertItemQuery->dwItemCodeU,
          insertItemQuery->byType);
        break;
      }
      case 15:
      {
        auto *insertGuildQuery = reinterpret_cast<_qry_case_insertguild_local *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Insert_guild(
          insertGuildQuery->in_memberserial,
          insertGuildQuery->in_w_guildName,
          insertGuildQuery->in_guildRace,
          &insertGuildQuery->out_guildserial);
        break;
      }
      case 16:
      {
        auto *joinAcceptQuery = reinterpret_cast<_qry_case_joinacguild_local *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_update_guildmember_add(
          joinAcceptQuery->in_applierserial,
          joinAcceptQuery->in_guildserial,
          static_cast<unsigned __int8>(joinAcceptQuery->in_Grade),
          static_cast<unsigned int>(joinAcceptQuery->in_MemberNum));
        break;
      }
      case 17:
      {
        auto *selfLeaveQuery = reinterpret_cast<_qry_case_selfleave *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_update_guildmember_del(
          selfLeaveQuery->in_leaverserial,
          selfLeaveQuery->tmp_guildserial,
          static_cast<unsigned int>(selfLeaveQuery->in_MemberNum));
        break;
      }
      case 18:
      {
        auto *forceLeaveQuery = reinterpret_cast<_qry_case_forceleave *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_update_guildmember_del(
          forceLeaveQuery->in_leaverserial,
          forceLeaveQuery->in_guildserial,
          static_cast<unsigned int>(forceLeaveQuery->in_seniornum));
        break;
      }
      case 19:
      {
        auto *pushMoneyQuery = reinterpret_cast<_qry_case_inputgmoney *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_input_guild_money(
          pushMoneyQuery->in_pusherserial,
          pushMoneyQuery->in_guildserial,
          pushMoneyQuery->dwAddDalant,
          pushMoneyQuery->dwAddGold,
          &pushMoneyQuery->out_totaldalant,
          &pushMoneyQuery->out_totalgold,
          pushMoneyQuery->in_date,
          pushMoneyQuery->in_w_pushername);
        break;
      }
      case 20:
      {
        auto *outputMoneyQuery = reinterpret_cast<_qry_case_outputgmoney *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_output_guild_money(
          outputMoneyQuery->in_poperserial,
          outputMoneyQuery->in_guildserial,
          outputMoneyQuery->dwSubDalant,
          outputMoneyQuery->dwSubGold,
          &outputMoneyQuery->out_totaldalant,
          &outputMoneyQuery->out_totalgold,
          outputMoneyQuery->in_date,
          outputMoneyQuery->in_w_popername,
          &outputMoneyQuery->byProcRet);
        break;
      }
      case 21:
      {
        auto *buyEmblemQuery = reinterpret_cast<_qry_case_buyemblem *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_buy_emblem(
          buyEmblemQuery->in_guildserial,
          buyEmblemQuery->in_emblemdlant,
          buyEmblemQuery->in_emblemback,
          buyEmblemQuery->in_emblemmark,
          buyEmblemQuery->in_suggestorSerial,
          &buyEmblemQuery->out_totaldalant,
          &buyEmblemQuery->out_totalgold,
          buyEmblemQuery->in_date,
          buyEmblemQuery->tmp_w_suggestorname,
          &buyEmblemQuery->byProcRet);
        break;
      }
      case 22:
      {
        auto *disjointGuildQuery = reinterpret_cast<_qry_case_disjointguild *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_disjoint_guild(disjointGuildQuery->in_guildserial);
        break;
      }
      case 23:
      {
        auto *aliveCharQuery = reinterpret_cast<_qry_case_alive_char_payload *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_char_set_alive(
          queryEntry->m_dwAccountSerial,
          aliveCharQuery->in_byCase,
          aliveCharQuery->in_dwSerial,
          aliveCharQuery->in_w_szName,
          aliveCharQuery->in_bySlot,
          reinterpret_cast<_REGED *>(aliveCharQuery->out_AliveAvator));
        break;
      }
      case 24:
      {
        auto *changeClassLog = reinterpret_cast<_log_change_class_after_init_class *>(queryEntry->m_sData);
        db_Insert_ChangeClass_AfterInitClass(
          changeClassLog->dwCharacSerial,
          changeClassLog->byType,
          changeClassLog->szPrevClassCode,
          changeClassLog->szNextClassCode,
          changeClassLog->nClassInitCnt,
          changeClassLog->byLastClassGrade,
          changeClassLog->dwYear,
          changeClassLog->byMonth,
          changeClassLog->byDay,
          changeClassLog->byHour,
          changeClassLog->byMin,
          changeClassLog->bySec);
        break;
      }
      case 25:
      {
        auto *raceBossSmsQuery = reinterpret_cast<_qry_case_sendwebracebosssms *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_sendwebracebosssms(raceBossSmsQuery);
        break;
      }
      case 30:
      {
        auto *addScheduleQuery = reinterpret_cast<_qry_case_addguildbattleschedule *>(queryEntry->m_sData);
        CGuildBattleController *controller = CGuildBattleController::Instance();
        queryEntry->m_byResult = controller->AddSchedule(reinterpret_cast<char *>(addScheduleQuery));
        break;
      }
      case 31:
      {
        auto *winLoseRankQuery = reinterpret_cast<_qry_case_updatewinloseguildbattlerank *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateWinLose(
              winLoseRankQuery->byWinRace,
              winLoseRankQuery->dwWinGuildSerial,
              winLoseRankQuery->byLoseRace,
              winLoseRankQuery->dwLoseGuildSerial))
          queryEntry->m_byResult = 24;
        break;
      }
      case 32:
      {
        auto *drawRankQuery = reinterpret_cast<_qry_case_updatedrawguildbattlerank *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateDraw(
              drawRankQuery->by1PRace,
              drawRankQuery->dw1PGuildSerial,
              drawRankQuery->by2PRace,
              drawRankQuery->dw2PGuildSerial))
          queryEntry->m_byResult = 24;
        break;
      }
      case 33:
      {
        auto *loadGuildBattleRankQuery = reinterpret_cast<_qry_case_loadguildbattlerank *>(queryEntry->m_sData);
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateRank(loadGuildBattleRankQuery->byRace, &loadGuildBattleRankQuery->byLoadDataStartPosition))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 34:
      {
        using GuildBattleRankDate = char[10];
        auto *guildBattleRankDate = reinterpret_cast<GuildBattleRankDate *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->CreateGuildBattleRankTable(*guildBattleRankDate))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 35:
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->UpdateClearGuildBattleRank() )
          queryEntry->m_byResult = 24;
        break;
      case 36:
      {
        auto *reservedDayInfo = reinterpret_cast<_qry_case_updateclearguildbattleDayInfo *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateClearRerservedDayInfo(
              reservedDayInfo->dwStartSLID,
              reservedDayInfo->dwEndSLID,
              reservedDayInfo->dwStartSID,
              reservedDayInfo->dwEndSID))
          queryEntry->m_byResult = 24;
        break;
      }
      case 37:
      {
        auto *inBattleCostQuery = reinterpret_cast<_qry_case_in_guildbattlecost *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_input_guild_money(
          inBattleCostQuery->dwGuildIndex,
          inBattleCostQuery->dwGuildIndex,
          inBattleCostQuery->dwAddDalant,
          inBattleCostQuery->dwAddGold,
          &inBattleCostQuery->out_totaldalant,
          &inBattleCostQuery->out_totalgold,
          inBattleCostQuery->byDate,
          "Scramble Cost");
        break;
      }
      case 38:
      {
        auto *srcGuildOutBattleCostQuery =
          reinterpret_cast<_qry_case_src_guild_out_guildbattlecost *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_output_guild_money(
          srcGuildOutBattleCostQuery->dwGuildIndex,
          srcGuildOutBattleCostQuery->dwGuildIndex,
          srcGuildOutBattleCostQuery->dwSubDalant,
          srcGuildOutBattleCostQuery->dwSubGold,
          &srcGuildOutBattleCostQuery->out_totaldalant,
          &srcGuildOutBattleCostQuery->out_totalgold,
          srcGuildOutBattleCostQuery->byDate,
          "Scramble Cost",
          &srcGuildOutBattleCostQuery->byProcRet);
        break;
      }
      case 39:
      {
        auto *inGuildBattleRewardMoneyQuery =
          reinterpret_cast<_qry_case_in_guildbattlerewardmoney *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_input_guild_money(
          inGuildBattleRewardMoneyQuery->dwGuildSerial,
          inGuildBattleRewardMoneyQuery->dwGuildSerial,
          inGuildBattleRewardMoneyQuery->dwAddDalant,
          inGuildBattleRewardMoneyQuery->dwAddGold,
          &inGuildBattleRewardMoneyQuery->out_totaldalant,
          &inGuildBattleRewardMoneyQuery->out_totalgold,
          inGuildBattleRewardMoneyQuery->byDate,
          "Scramble Cost");
        break;
      }
      case 40:
        queryEntry->m_byResult = 0;
        break;
      case 41:
      {
        auto *updateReservedScheduleQuery = reinterpret_cast<_qry_case_updatereservedschedule *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        CGuildBattleController *controller = CGuildBattleController::Instance();
        if (!controller->UpdateReservedGuildBattleSchedule(
              updateReservedScheduleQuery->dwSLID,
              &updateReservedScheduleQuery->byLoadDataStartPosition))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 43:
      {
        queryEntry->m_byResult = 0;
        char *loadData = queryEntry->m_sData;
        CTotalGuildRankManager *manager = CTotalGuildRankManager::Instance();
        if (!manager->Update(reinterpret_cast<_total_guild_rank_info *>(loadData)) )
          queryEntry->m_byResult = 24;
        break;
      }
      case 44:
      {
        queryEntry->m_byResult = 0;
        char *rankData = queryEntry->m_sData;
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        if (!manager->UpdateTodayRank(reinterpret_cast<_pvppoint_guild_rank_info *>(rankData)) )
          queryEntry->m_byResult = 24;
        break;
      }
      case 45:
      {
        queryEntry->m_byResult = 0;
        char *ownerData = queryEntry->m_sData;
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        if (!manager->UpdateWeeklyOwner(reinterpret_cast<_weeklyguildrank_owner_info *>(ownerData)) )
          queryEntry->m_byResult = 24;
        break;
      }
      case 46:
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_ClearWeeklyPvpPointSum() )
          queryEntry->m_byResult = 24;
        break;
      case 47:
      {
        auto *weeklyPvpPointSum = reinterpret_cast<_qry_case_updateweeklyguildpvppointsum *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_IncreaseWeeklyGuildGuildBattlePvpPointSum(
                weeklyPvpPointSum->dwGuildSerial,
                weeklyPvpPointSum->dPvpPoint))
          queryEntry->m_byResult = 24;
        break;
      }
      case 49:
      {
        auto *updateGuildMasterQuery = reinterpret_cast<_qry_case_update_guildmaster *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_update_guildmaster(updateGuildMasterQuery);
        break;
      }
      case 50:
      {
        auto *updateMacroDataQuery = reinterpret_cast<_qry_case_update_macro_data *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Update_MacroData(
               updateMacroDataQuery->dwSerial,
               &updateMacroDataQuery->NewData,
               &updateMacroDataQuery->OldData)
          ? 0
          : 24;
        break;
      }
      case 51:
      {
        auto *insertTrcInfo = reinterpret_cast<_insert_trc_info *>(queryEntry->m_sData);
        queryEntry->m_byResult = TRC_AutoTrade::_insert_info(reinterpret_cast<char *>(insertTrcInfo));
        break;
      }
      case 52:
      {
        auto *inATradeTaxQuery = reinterpret_cast<_qry_case_in_atrade_tax *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_input_guild_money_atradetax(
          inATradeTaxQuery->in_seller,
          inATradeTaxQuery->dwGuildSerial,
          inATradeTaxQuery->in_dalant,
          &inATradeTaxQuery->out_totaldalant,
          &inATradeTaxQuery->out_totalgold,
          inATradeTaxQuery->byDate);
        break;
      }
      case 53:
      {
        AutoMineMachineMng *manager = AutoMineMachineMng::Instance();
        auto *autoMineQuery = reinterpret_cast<_qry_case_amine_header *>(queryEntry->m_sData);
        queryEntry->m_byResult = manager->request_db_query(autoMineQuery);
        break;
      }
      case 56:
      {
        auto *makeStorageQuery = reinterpret_cast<_qry_case_make_storage *>(queryEntry->m_sData);
        AutominePersonalMgr *manager = AutominePersonalMgr::instance();
        queryEntry->m_byResult = manager->request_query(makeStorageQuery);
        break;
      }
      case 57:
      {
        auto *loadGuildBattleTotalRecordQuery =
          reinterpret_cast<_qry_case_load_guildbattle_totalrecord *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Select_GuildBattleRecord(
              loadGuildBattleTotalRecordQuery->dwGuildSerial,
              reinterpret_cast<_guildbattle_totalrecord *>(loadGuildBattleTotalRecordQuery)))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 58:
      {
        auto *reservedScheduleInfo = reinterpret_cast<_unmannedtrader_reserved_schedule_info *>(queryEntry->m_sData);
        unsigned __int8 selectResult = g_Main.m_pWorldDB->Select_UnmannedTraderReservedSchedule(
          CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt,
          reservedScheduleInfo);
        if (selectResult != 2 && selectResult)
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 59:
      {
        auto *updateItemStateQuery = reinterpret_cast<_qry_case_unmandtrader_updateitemstate *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateItemState(updateItemStateQuery);
        break;
      }
      case 60:
      {
        auto *updateRegistItemQuery = reinterpret_cast<_qry_case_unmandtrader_registsingleitem *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateRegistItem(updateRegistItemQuery);
        break;
      }
      case 61:
      {
        auto *updateCancelRegistQuery = reinterpret_cast<_qry_case_unmandtrader_cancelitem *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateCancelRegist(updateCancelRegistQuery);
        break;
      }
      case 62:
      {
        auto *timeOutCancelRegistQuery =
          reinterpret_cast<_qry_case_unmandtrader_time_out_cancelitem *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateTimeOutCancelRegist(timeOutCancelRegistQuery);
        break;
      }
      case 63:
      {
        auto *updateRePriceQuery = reinterpret_cast<_qry_case_unmandtrader_update_reprice *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateRePrice(updateRePriceQuery);
        break;
      }
      case 64:
      {
        auto *selectBuyQuery = reinterpret_cast<_qry_case_unmandtrader_buy_get_info *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->SelectBuy(selectBuyQuery);
        break;
      }
      case 65:
      {
        auto *updateBuyWaitQuery = reinterpret_cast<_qry_case_unmandtrader_buy_update_wait *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateBuy(updateBuyWaitQuery);
        break;
      }
      case 66:
      {
        auto *updateBuyRollbackQuery =
          reinterpret_cast<_qry_case_unmandtrader_buy_update_rollback *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateBuyRollBack(updateBuyRollbackQuery);
        break;
      }
      case 67:
      {
        auto *lazyCleanFlags = reinterpret_cast<_qry_case_unmandtrader_lazyclean_flags *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateLazyClean(lazyCleanFlags);
        break;
      }
      case 68:
      {
        auto *updateBuyCompleteQuery =
          reinterpret_cast<_qry_case_unmandtrader_buy_update_complete *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateBuyComplete(updateBuyCompleteQuery);
        break;
      }
      case 71:
      {
        auto *guildRoomInsertQuery = reinterpret_cast<_qry_case_guildroom_insert *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_GuildRoom_Insert(guildRoomInsertQuery);
        break;
      }
      case 72:
      {
        auto *guildRoomUpdateQuery = reinterpret_cast<_qry_case_guildroom_update *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_GuildRoom_Update(guildRoomUpdateQuery) ? 0 : 24;
        break;
      }
      case 74:
      {
        auto *gmGreetingQuery = reinterpret_cast<_qry_case_gm_greetingmsg *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_GM_GreetingMsg(gmGreetingQuery);
        break;
      }
      case 75:
      {
        auto *raceGreetingQuery = reinterpret_cast<_qry_case_race_greetingmsg *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_RACE_GreetingMsg(raceGreetingQuery);
        break;
      }
      case 76:
      {
        auto *guildGreetingQuery = reinterpret_cast<_qry_case_guild_greetingmsg *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_GUILD_GreetingMsg(guildGreetingQuery);
        break;
      }
      case 77:
      {
        auto *postListRegiQuery = reinterpret_cast<_qry_case_post_list_regi *>(queryEntry->m_sData);
        CPostSystemManager *manager = CPostSystemManager::Instace();
        queryEntry->m_byResult = manager->UpdateRegist(postListRegiQuery);
        break;
      }
      case 78:
      {
        auto *postSendQuery = reinterpret_cast<_qry_case_post_send *>(queryEntry->m_sData);
        CPostSystemManager *manager = CPostSystemManager::Instace();
        queryEntry->m_byResult = manager->PostSend(postSendQuery);
        break;
      }
      case 79:
      {
        auto *postStorageListQuery = reinterpret_cast<_qry_case_post_storage_list_get *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Load_PostStorage(postStorageListQuery);
        break;
      }
      case 80:
      {
        auto *postReturnListQuery = reinterpret_cast<_qry_case_post_return_list_get *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Load_ReturnPost(postReturnListQuery);
        break;
      }
      case 82:
      {
        auto *postContentQuery = reinterpret_cast<_qry_case_post_content_get *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_Load_Content(postContentQuery);
        break;
      }
      case 83:
        queryEntry->m_byResult = _db_init_classrefine_count();
        break;
      case 84:
      {
        auto *guildBattleResultLog = reinterpret_cast<_qry_case_guild_battel_result_log *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Insert_GuildBatlleResultLog(
                guildBattleResultLog->szStartTime,
                guildBattleResultLog->szEndTime,
                guildBattleResultLog->dwRedSerial,
                guildBattleResultLog->wszRedName,
                guildBattleResultLog->dwBlueSerial,
                guildBattleResultLog->wszBlueName,
                guildBattleResultLog->dwRedScore,
                guildBattleResultLog->dwBlueScore,
                guildBattleResultLog->dwRedMaxJoinCnt,
                guildBattleResultLog->dwBlueMaxJoinCnt,
                guildBattleResultLog->dwRedGoalCnt,
                guildBattleResultLog->dwBlueGoalCnt,
                guildBattleResultLog->dwRedKillCntSum,
                guildBattleResultLog->dwBlueKillCntSum,
                guildBattleResultLog->byBattleResult,
                guildBattleResultLog->dwMaxGoalCharacSerial,
                guildBattleResultLog->wszMaxGoalCharacName,
                guildBattleResultLog->dwMaxKillCharacSerial,
                guildBattleResultLog->wszMaxKillCharacName,
                guildBattleResultLog->byJoinLimit,
                guildBattleResultLog->dwGuildBattleCostGold,
                guildBattleResultLog->szBattleMapCode))
          queryEntry->m_byResult = 24;
        break;
      }
      case 85:
      case 86:
      case 87:
      case 88:
      case 89:
      case 90:
      case 91:
      case 92:
      case 93:
      case 94:
      case 95:
      case 97:
      case 98:
      case 99:
      case 100:
      case 101:
      case 102:
      case 103:
      case 104:
      case 105:
      case 106:
      case 107:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(queryEntry->m_sData);
        CPvpUserAndGuildRankingSystem *rankingSystem = CPvpUserAndGuildRankingSystem::Instance();
        switch (queryCase)
        {
          case 85: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep1(reinterpret_cast<char *>(rankQuery)); break;
          case 86: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep2(reinterpret_cast<char *>(rankQuery)); break;
          case 87: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep3(reinterpret_cast<char *>(rankQuery)); break;
          case 88: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep4(reinterpret_cast<char *>(rankQuery)); break;
          case 89: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep5(reinterpret_cast<char *>(rankQuery)); break;
          case 90: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep6(reinterpret_cast<char *>(rankQuery)); break;
          case 91: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep7(reinterpret_cast<char *>(rankQuery)); break;
          case 92: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep8(reinterpret_cast<char *>(rankQuery)); break;
          case 93: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep9(reinterpret_cast<char *>(rankQuery)); break;
          case 94: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep10(rankQuery); break;
          case 95: queryEntry->m_byResult = rankingSystem->UpdateRaceRankStep11(rankQuery); break;
          case 97: queryEntry->m_byResult = rankingSystem->UpdateGuildRankStep1(reinterpret_cast<char *>(rankQuery)); break;
          case 98: queryEntry->m_byResult = rankingSystem->UpdateGuildRankStep2(reinterpret_cast<char *>(rankQuery)); break;
          case 99: queryEntry->m_byResult = rankingSystem->UpdateGuildRankStep3(reinterpret_cast<char *>(rankQuery)); break;
          case 100: queryEntry->m_byResult = rankingSystem->UpdateGuildRankStep4(reinterpret_cast<char *>(rankQuery)); break;
          case 101: queryEntry->m_byResult = rankingSystem->UpdateRankinGuildStep1(reinterpret_cast<char *>(rankQuery)); break;
          case 102: queryEntry->m_byResult = rankingSystem->UpdateRankinGuildStep2(reinterpret_cast<char *>(rankQuery)); break;
          case 103: queryEntry->m_byResult = rankingSystem->UpdateRankinGuildStep3(reinterpret_cast<char *>(rankQuery)); break;
          case 104: queryEntry->m_byResult = rankingSystem->UpdateRankinGuildStep4(reinterpret_cast<char *>(rankQuery)); break;
          case 105: queryEntry->m_byResult = rankingSystem->UpdateRankinGuildStep5(reinterpret_cast<char *>(rankQuery)); break;
          case 106: queryEntry->m_byResult = rankingSystem->UpdateRankinGuildStep6(reinterpret_cast<char *>(rankQuery)); break;
          case 107: queryEntry->m_byResult = rankingSystem->UpdateAndSelectGuildGrade(reinterpret_cast<char *>(rankQuery)); break;
          default: break;
        }
        break;
      }
      case 111:
      {
        auto *loginCompleteCreateQuery =
          reinterpret_cast<_qry_case_unmandtrader_log_in_proc_update_complete *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateLogInComplete(loginCompleteCreateQuery);
        break;
      }
      case 112:
        queryEntry->m_byResult = CItemStoreManager::Instance()->UpdateStoreLimitItem();
        break;
      case 113:
      {
        auto *disableInstanceStoreData = reinterpret_cast<_qry_case_disable_instance_store *>(queryEntry->m_sData);
        CItemStoreManager *manager = CItemStoreManager::Instance();
        queryEntry->m_byResult = manager->UpdateDisableInstanceStore(disableInstanceStoreData);
        break;
      }
      case 114:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        queryEntry->m_byResult = manager->Update_RegistCandidate_2st();
        break;
      }
      case 115:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        queryEntry->m_byResult = manager->Update_Score();
        break;
      }
      case 116:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        queryEntry->m_byResult = manager->Update_ClassType();
        break;
      }
      case 117:
      case 125:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        auto *insertCandidateQuery = reinterpret_cast<_qry_case_insert_candidate *>(queryEntry->m_sData);
        queryEntry->m_byResult = manager->Insert_Candidate(queryEntry->m_byQryCase, insertCandidateQuery);
        break;
      }
      case 118:
      {
        auto *destGuildOutBattleCostQuery =
          reinterpret_cast<_qry_case_dest_guild_out_guildbattlecost *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_output_guild_money(
          destGuildOutBattleCostQuery->dwGuildIndex,
          destGuildOutBattleCostQuery->dwGuildIndex,
          destGuildOutBattleCostQuery->dwSubDalant,
          destGuildOutBattleCostQuery->dwSubGold,
          &destGuildOutBattleCostQuery->out_totaldalant,
          &destGuildOutBattleCostQuery->out_totalgold,
          destGuildOutBattleCostQuery->byDate,
          "Scramble Cost",
          &destGuildOutBattleCostQuery->byProcRet);
        break;
      }
      case 119:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        queryEntry->m_byResult = processor->Insert_Elect();
        break;
      }
      case 120:
      {
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        queryEntry->m_byResult = processor->Update_Elect();
        break;
      }
      case 121:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        queryEntry->m_byResult = manager->Update_Refund();
        break;
      }
      case 122:
      {
        queryEntry->m_byResult = 0;
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        if (!processor->LoadElectState() )
          queryEntry->m_byResult = 24;
        break;
      }
      case 123:
      {
        auto *requestRefundQuery = reinterpret_cast<_qry_case_request_refund *>(queryEntry->m_sData);
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        queryEntry->m_byResult = processor->Request_Refund(reinterpret_cast<char *>(requestRefundQuery));
        break;
      }
      case 124:
      {
        auto *requestRefundQuery = reinterpret_cast<_qry_case_request_refund *>(queryEntry->m_sData);
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        queryEntry->m_byResult = processor->Insert_PatrirchItemChargeRefund(reinterpret_cast<char *>(requestRefundQuery));
        break;
      }
      case 126:
      {
        CandidateMgr *manager = CandidateMgr::Instance();
        auto *dischargePatriarchQuery = reinterpret_cast<_qry_case_discharge_patriarch *>(queryEntry->m_sData);
        queryEntry->m_byResult = manager->Update_DischargePatriarch(dischargePatriarchQuery);
        break;
      }
      case 127:
      {
        auto *updatePunishmentQuery = reinterpret_cast<_qry_case_update_punishment *>(queryEntry->m_sData);
        queryEntry->m_byResult = g_Main.m_pWorldDB->Update_Punishment(reinterpret_cast<char *>(updatePunishmentQuery))
          ? 0
          : 24;
        break;
      }
      case 128:
      {
        auto *selectCharSerialQuery = reinterpret_cast<_qry_case_select_charserial *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        char charSerialByNameRace = g_Main.m_pWorldDB->Select_GetCharSerialByNameRace(
          selectCharSerialQuery->wszCharName,
          selectCharSerialQuery->byRace,
          &selectCharSerialQuery->dwAvatorSerial);
        if (charSerialByNameRace != 2 && charSerialByNameRace)
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 129:
      {
        auto *postSerialCheckQuery = reinterpret_cast<_qry_case_post_serial_check *>(queryEntry->m_sData);
        CPostSystemManager *manager = CPostSystemManager::Instace();
        queryEntry->m_byResult = manager->PostReceiverCheck(postSerialCheckQuery);
        break;
      }
      case 130:
      {
        auto *selectPatriarchCommQuery = reinterpret_cast<_qry_case_select_patriarch_comm *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Load_PatriarchComm(selectPatriarchCommQuery);
        break;
      }
      case 131:
      {
        auto *insertPatriarchCommQuery = reinterpret_cast<_qry_case_insert_patriarch_comm *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Insert_PatriarchComm(
              insertPatriarchCommQuery->dwSerial,
              insertPatriarchCommQuery->dwDalant,
              insertPatriarchCommQuery->szDepDate))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 134:
      {
        auto *updateHonorGuildQuery = reinterpret_cast<_qry_case_update_honor_guild *>(queryEntry->m_sData);
        CHonorGuild *honorGuild = CHonorGuild::Instance();
        queryEntry->m_byResult = honorGuild->UpdateNextHonorGuild(updateHonorGuildQuery->byRace);
        break;
      }
      case 135:
      {
        auto *updateHonorGuildQuery = reinterpret_cast<_qry_case_update_honor_guild *>(queryEntry->m_sData);
        CHonorGuild *honorGuild = CHonorGuild::Instance();
        queryEntry->m_byResult = honorGuild->UpdateChangeHonorGuild(updateHonorGuildQuery->byRace);
        break;
      }
      case 136:
      {
        auto *inATradeTaxQuery = reinterpret_cast<_qry_case_in_atrade_tax *>(queryEntry->m_sData);
        queryEntry->m_byResult = db_input_guild_money_atradetax(
          inATradeTaxQuery->in_seller,
          inATradeTaxQuery->dwGuildSerial,
          inATradeTaxQuery->in_dalant,
          &inATradeTaxQuery->out_totaldalant,
          &inATradeTaxQuery->out_totalgold,
          inATradeTaxQuery->byDate);
        break;
      }
      case 137:
      {
        auto *raceBattleLogInfo = reinterpret_cast<_race_battle_log_info *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Insert_RaceBattleLog(raceBattleLogInfo))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 138:
      {
        auto *checkInvalidCharacterQuery = reinterpret_cast<_qry_case_check_invalid_character *>(queryEntry->m_sData);
        CandidateMgr *manager = CandidateMgr::Instance();
        queryEntry->m_byResult = manager->CheckDBValidCharacter(checkInvalidCharacterQuery->byProc);
        break;
      }
      case 139:
      {
        auto *updateVoteTimeQuery = reinterpret_cast<_qry_case_update_vote_time *>(queryEntry->m_sData);
        CandidateMgr *manager = CandidateMgr::Instance();
        queryEntry->m_byResult = manager->Update_VoteTime(updateVoteTimeQuery->dwSerial);
        break;
      }
      case 140:
      {
        auto *updateReRegistQuery = reinterpret_cast<_qry_case_unmandtrader_re_registsingleitem *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateReRegist(updateReRegistQuery);
        break;
      }
      case 141:
      {
        auto *cheatUpdateRegistTimeQuery =
          reinterpret_cast<_qry_case_unmandtrader_cheat_updateregisttime *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateCheatRegistTime(cheatUpdateRegistTimeQuery);
        break;
      }
      case 142:
      {
        auto *raceBossAccumulationWinRate = reinterpret_cast<_qry_case_raceboss_accumulation_winrate *>(queryEntry->m_sData);
        CRaceBossWinRate *winRate = CRaceBossWinRate::Instance();
        queryEntry->m_byResult = winRate->LoadBossAccmulationWinRate(raceBossAccumulationWinRate);
        break;
      }
      case 143:
      {
        auto *updateUserGuildDataQuery = reinterpret_cast<_qry_case_update_user_guild_data *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_UserGuildData(
              updateUserGuildDataQuery->dwAvatorSerial,
              updateUserGuildDataQuery->dwGuildSerial,
              updateUserGuildDataQuery->byGrade))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 144:
      {
        auto *guildMasterLastConnQuery = reinterpret_cast<_qry_case_select_guild_master_lastconn *>(queryEntry->m_sData);
        unsigned __int8 conn = g_Main.m_pWorldDB->Select_GuildMasterLastConn(
          guildMasterLastConnQuery->dwSerial,
          guildMasterLastConnQuery->dwLimitConnTime,
          &guildMasterLastConnQuery->dwLastConnTime);
        queryEntry->m_byResult = (conn == 2 || !conn) ? 0 : 24;
        break;
      }
      case 145:
      {
        auto *characterRenameQuery = reinterpret_cast<_qry_case_character_rename *>(queryEntry->m_sData);
        queryEntry->m_byResult = g_Main.m_pWorldDB->Select_CharacterReName(
          characterRenameQuery->wszCharName,
          &characterRenameQuery->dwAlreadySerial)
          ? 0
          : 24;
        break;
      }
      case 146:
      {
        auto *characterRenameQuery = reinterpret_cast<_qry_case_character_rename *>(queryEntry->m_sData);
        if (g_Main.m_pWorldDB->Update_CharacterReName(
              characterRenameQuery->wszCharName,
              characterRenameQuery->dwCharSerial))
          queryEntry->m_byResult = 0;
        else
          queryEntry->m_byResult = 24;
        break;
      }
      case 147:
      {
        auto *loginProcUpdateCompleteQuery =
          reinterpret_cast<_qry_case_unmandtrader_re_registsingleitem_roll_back *>(queryEntry->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        queryEntry->m_byResult = controller->UpdateReRegistRollBack(loginProcUpdateCompleteQuery);
        break;
      }
      case 149:
      {
        auto *updateVoteAvailableQuery = reinterpret_cast<_qry_case_update_vote_available *>(queryEntry->m_sData);
        if (g_Main.m_pWorldDB->Updatet_Account_Vote_Available(
               updateVoteAvailableQuery->dwAccountSerial,
               &updateVoteAvailableQuery->byVoteEnable))
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 150:
      {
        auto *updatePlayerVoteInfoQuery = reinterpret_cast<_qry_case_update_player_vote_info *>(queryEntry->m_sData);
        queryEntry->m_byResult = g_Main.m_pWorldDB->Update_Player_Vote_Info(
          updatePlayerVoteInfoQuery->dwCharSerial,
          updatePlayerVoteInfoQuery->dwAccumPlayTime,
          updatePlayerVoteInfoQuery->byIsVoted,
          updatePlayerVoteInfoQuery->byVoteEnable,
          static_cast<unsigned int>(updatePlayerVoteInfoQuery->dwScanerData))
          ? 0
          : 24;
        break;
      }
      case 151:
      {
        auto *cheatPlayerVoteInfoQuery = reinterpret_cast<_qry_case_cheat_player_vote_info *>(queryEntry->m_sData);
        queryEntry->m_byResult = g_Main.m_pWorldDB->UpdateVotedReset_Cheat(cheatPlayerVoteInfoQuery->dwCharSerial)
          ? 0
          : 24;
        break;
      }
      case 152:
      {
        auto *updateServerResetTokenQuery = reinterpret_cast<_qry_case_update_server_reset_token *>(queryEntry->m_sData);
        if (g_Main.m_pWorldDB->UpdateServerResetToken(
               updateServerResetTokenQuery->dwServerToken,
               updateServerResetTokenQuery->wProcType,
               updateServerResetTokenQuery->dwESerial))
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 153:
      {
        auto *selectTimeLimitInfoQuery = reinterpret_cast<_qry_case_select_timelimit_info *>(queryEntry->m_sData);
        char logoutTime = g_Main.m_pWorldDB->Select_Player_Last_LogoutTime(
          selectTimeLimitInfoQuery->dwAccountSerial,
          &selectTimeLimitInfoQuery->dwLastLogoutTime);
        queryEntry->m_byResult = (logoutTime == 1) ? 24 : 0;
        if (logoutTime == 2)
        {
          if (m_pWorldDB->Insert_PlayerTimeLimitInfo(selectTimeLimitInfoQuery->dwAccountSerial))
          {
            if (m_pWorldDB->Select_Player_Last_LogoutTime(
                  selectTimeLimitInfoQuery->dwAccountSerial,
                  &selectTimeLimitInfoQuery->dwLastLogoutTime))
            {
              queryEntry->m_byResult = 24;
            }
          }
          else
          {
            queryEntry->m_byResult = 24;
          }
        }
        break;
      }
      case 155:
      {
        auto *insertTimeLimitInfoQuery = reinterpret_cast<_qry_case_insert_timelimit_info *>(queryEntry->m_sData);
        queryEntry->m_byResult = g_Main.m_pWorldDB->Update_Player_TimeLimit_Info(
          insertTimeLimitInfoQuery->dwAccSerial,
          insertTimeLimitInfoQuery->dwFatigue,
          static_cast<unsigned __int8>(insertTimeLimitInfoQuery->wStatus))
          ? 0
          : 24;
        break;
      }
      case 157:
      {
        auto *cashLimSaleQuery = reinterpret_cast<qry_case_cash_limsale *>(queryEntry->m_sData);
        unsigned __int8 loadResult = _db_Load_Cash_LimSale(cashLimSaleQuery);
        queryEntry->m_byResult = loadResult;
        if (!queryEntry->m_byResult )
        {
          CashItemRemoteStore *remoteStore = CashItemRemoteStore::Instance();
          remoteStore->Set_FROMDB_LimitedSale_Event(&cashLimSaleQuery->NewSale);
        }
        break;
      }
      case 158:
      {
        auto *cashLimSaleQuery = reinterpret_cast<qry_case_cash_limsale *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Update_Cash_LimSale(&cashLimSaleQuery->NewSale, &cashLimSaleQuery->OldSale);
        break;
      }
      case 161:
      {
        queryEntry->m_byResult = _db_Update_Set_Limit_Run();
        break;
      }
      case 165:
      {
        auto *goldenBoxItemQuery = reinterpret_cast<qry_case_golden_box_item *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Update_GoldBoxItem(
          goldenBoxItemQuery->nDBSerial,
          &goldenBoxItemQuery->NewData,
          &goldenBoxItemQuery->OldData);
        break;
      }
      case 171:
      {
        auto *lobbyLogoutQuery = reinterpret_cast<_qry_case_lobby_logout *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Select_RegeAvator_For_Lobby_Logout(lobbyLogoutQuery);
        break;
      }
      case 172:
      {
        auto *insertOreLogQuery = reinterpret_cast<_qry_case_insert_orelog *>(queryEntry->m_sData);
        queryEntry->m_byResult = 0;
        if (!m_pWorldDB->Insert_OreReset_Log(
              insertOreLogQuery->byType,
              insertOreLogQuery->nLiveUsercnt,
              insertOreLogQuery->dwOreRemain,
              insertOreLogQuery->dwTAmount))
        {
          queryEntry->m_byResult = 24;
        }
        break;
      }
      case 175:
      {
        auto *updateDataForPostSendQuery = reinterpret_cast<_qry_case_update_data_for_post_send *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Update_Data_For_Post_Send(updateDataForPostSendQuery);
        break;
      }
      case 176:
      {
        auto *updateDataForTradeQuery = reinterpret_cast<_qry_case_update_data_for_trade *>(queryEntry->m_sData);
        queryEntry->m_byResult = _db_Update_Data_For_Trade(updateDataForTradeQuery);
        break;
      }
      default:
        break;
    }
    if (queryEntry->m_byResult && queryEntry->m_byResult != 25 && queryEntry->m_byResult != 49 )
    {
      m_logDQS.Write(
        "DQSThread.. DB fail.. account: %d, case: %d, ret: %d",
        queryEntry->m_dwAccountSerial,
        queryEntry->m_byQryCase,
        queryEntry->m_byResult);
    }
    queryEntry->m_bLoad = 1;
    m_listDQSDataComplete.PushNode_Back(queryIndex);
  }
}

void CMainThread::DQSCompleteProcess()
{
  unsigned int outIndex;
  while (m_listDQSDataComplete.PopNode_Front(&outIndex))
  {
    _DB_QRY_SYN_DATA *pData = &m_DBQrySynData[outIndex];
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
        auto *totalGuildRankInfo = reinterpret_cast<_total_guild_rank_info *>(pData->m_sData);
        manager->UpdateComlete(pData->m_byResult, totalGuildRankInfo);
        break;
      }
      case 44:
      {
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        auto *todayRankInfo = reinterpret_cast<_pvppoint_guild_rank_info *>(pData->m_sData);
        manager->CompleteLoadeTodayRank(
          pData->m_byResult,
          todayRankInfo);
        break;
      }
      case 45:
      {
        CWeeklyGuildRankManager *manager = CWeeklyGuildRankManager::Instance();
        auto *weeklyOwnerInfo = reinterpret_cast<_weeklyguildrank_owner_info *>(pData->m_sData);
        manager->CompleteUpdateWeeklyOwner(
          pData->m_byResult,
          weeklyOwnerInfo);
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
        auto *autoMineQuery = reinterpret_cast<_qry_case_amine_header *>(pData->m_sData);
        manager->result_db_query(pData->m_byResult, autoMineQuery);
        break;
      }
      case 56:
      {
        AutominePersonalMgr *manager = AutominePersonalMgr::instance();
        auto *makeStorageQuery = reinterpret_cast<_qry_case_make_storage *>(pData->m_sData);
        manager->result_query(pData->m_byResult, makeStorageQuery);
        break;
      }
      case 57:
      {
        auto *loadGuildBattleTotalRecordQuery = reinterpret_cast<_qry_case_load_guildbattle_totalrecord *>(pData->m_sData);
        CompleteLoadGuildBattleTotalRecord(pData->m_byResult, reinterpret_cast<char *>(loadGuildBattleTotalRecordQuery));
        break;
      }
      case 58:
      {
        auto *reservedScheduleInfo = reinterpret_cast<_unmannedtrader_reserved_schedule_info *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteSelectReservedSchedule(pData->m_byResult, reservedScheduleInfo);
        break;
      }
      case 59:
      {
        auto *updateItemStateQuery = reinterpret_cast<_qry_case_unmandtrader_updateitemstate *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteUpdateState(pData->m_byResult, updateItemStateQuery);
        break;
      }
      case 60:
      {
        auto *updateRegistItemQuery = reinterpret_cast<_qry_case_unmandtrader_registsingleitem *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteRegistItem(pData->m_byResult, updateRegistItemQuery);
        break;
      }
      case 61:
      {
        auto *updateCancelRegistQuery = reinterpret_cast<_qry_case_unmandtrader_cancelitem *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteCancelRegist(pData->m_byResult, updateCancelRegistQuery);
        break;
      }
      case 62:
      {
        auto *timeOutCancelRegistQuery =
          reinterpret_cast<_qry_case_unmandtrader_time_out_cancelitem *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteTimeOutCancelRegist(pData->m_byResult, timeOutCancelRegistQuery);
        break;
      }
      case 63:
      {
        auto *updateRePriceQuery = reinterpret_cast<_qry_case_unmandtrader_update_reprice *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteReprice(pData->m_byResult, updateRePriceQuery);
        break;
      }
      case 64:
      {
        auto *selectBuyQuery = reinterpret_cast<_qry_case_unmandtrader_buy_get_info *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteSelectBuyInfo(pData->m_byResult, selectBuyQuery);
        break;
      }
      case 65:
      {
        auto *updateBuyWaitQuery = reinterpret_cast<_qry_case_unmandtrader_buy_update_wait *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteBuy(pData->m_byResult, updateBuyWaitQuery);
        break;
      }
      case 66:
      {
        auto *updateBuyRollbackQuery = reinterpret_cast<_qry_case_unmandtrader_buy_update_rollback *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteBuyRollBack(pData->m_byResult, updateBuyRollbackQuery);
        break;
      }
      case 67:
      {
        auto *lazyCleanFlags = reinterpret_cast<_qry_case_unmandtrader_lazyclean_flags *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->ComleteLazyClean(lazyCleanFlags);
        break;
      }
      case 68:
      {
        auto *updateBuyCompleteQuery = reinterpret_cast<_qry_case_unmandtrader_buy_update_complete *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteBuyComplete(updateBuyCompleteQuery);
        break;
      }
      case 77:
      {
        auto *postListRegiQuery = reinterpret_cast<_qry_case_post_list_regi *>(pData->m_sData);
        CPostSystemManager *manager = CPostSystemManager::Instace();
        manager->CompleteRegist(postListRegiQuery);
        break;
      }
      case 78:
      {
        auto *postSendQuery = reinterpret_cast<_qry_case_post_send *>(pData->m_sData);
        CPostSystemManager *manager = CPostSystemManager::Instace();
        manager->CompleteSend(postSendQuery);
        break;
      }
      case 79:
      {
        auto *postStorageListQuery = reinterpret_cast<_qry_case_post_storage_list_get *>(pData->m_sData);
        Load_PostStorage_Complete(reinterpret_cast<char *>(postStorageListQuery));
        break;
      }
      case 80:
      {
        auto *postReturnListQuery = reinterpret_cast<_qry_case_post_return_list_get *>(pData->m_sData);
        Load_ReturnPost_Complete(reinterpret_cast<char *>(postReturnListQuery));
        break;
      }
      case 82:
      {
        auto *postContentQuery = reinterpret_cast<_qry_case_post_content_get *>(pData->m_sData);
        Load_Content_Complete(reinterpret_cast<char *>(postContentQuery));
        break;
      }
      case 85:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep1(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 86:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep2(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 87:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep3(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 88:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep4(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 89:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep5(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 90:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep6(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 91:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep7(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 92:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep8(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 93:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep9(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 94:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep10(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 95:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRaceRankStep11(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 97:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep1(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 98:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep2(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 99:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep3(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 100:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteGuildRankStep4(pData->m_byResult, reinterpret_cast<char *>(rankQuery));
        break;
      }
      case 101:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep1(pData->m_byResult, rankQuery);
        break;
      }
      case 102:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep2(pData->m_byResult, rankQuery);
        break;
      }
      case 103:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep3(pData->m_byResult, rankQuery);
        break;
      }
      case 104:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep4(pData->m_byResult, rankQuery);
        break;
      }
      case 105:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep5(pData->m_byResult, rankQuery);
        break;
      }
      case 106:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankInGuildStep6(pData->m_byResult, rankQuery);
        break;
      }
      case 107:
      {
        auto *rankQuery = reinterpret_cast<_qry_case_rank_racerank_guildrank *>(pData->m_sData);
        CPvpUserAndGuildRankingSystem *system = CPvpUserAndGuildRankingSystem::Instance();
        system->CompleteRankUpdateAndSelectGarde(pData->m_byResult, rankQuery);
        break;
      }
      case 111:
      {
        auto *loginCompleteCreateQuery =
          reinterpret_cast<_qry_case_unmandtrader_log_in_proc_update_complete *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteLogInCompete(loginCompleteCreateQuery);
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
        auto *disableInstanceStoreData = reinterpret_cast<_qry_case_disable_instance_store *>(pData->m_sData);
        CItemStoreManager *manager = CItemStoreManager::Instance();
        manager->CompleteDisableInstanceStore(disableInstanceStoreData);
        break;
      }
      case 117:
      {
        auto *candidateData = reinterpret_cast<_qry_case_insert_candidate *>(pData->m_sData);
        CandidateMgr *manager = CandidateMgr::Instance();
        manager->CompleteInsertCandidate(pData->m_byResult, candidateData);
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
      {
        auto *selectCharSerialQuery = reinterpret_cast<_qry_case_select_charserial *>(pData->m_sData);
        g_VoteSys[selectCharSerialQuery->byRace].CompleteSelectCharSerial(selectCharSerialQuery);
        break;
      }
      case 129:
      {
        auto *postSerialCheckQuery = reinterpret_cast<_qry_case_post_serial_check *>(pData->m_sData);
        CPostSystemManager *manager = CPostSystemManager::Instace();
        manager->CompletePostReceiverCheck(postSerialCheckQuery);
        break;
      }
      case 136:
      {
        auto *inATradeTaxQuery = reinterpret_cast<_qry_case_in_atrade_tax *>(pData->m_sData);
        CHonorGuild *honor = CHonorGuild::Instance();
        honor->DQSCompleteInAtradTaxMoney(reinterpret_cast<char *>(inATradeTaxQuery));
        break;
      }
      case 138:
      {
        auto *checkInvalidCharacterQuery = reinterpret_cast<_qry_case_check_invalid_character *>(pData->m_sData);
        PatriarchElectProcessor *processor = PatriarchElectProcessor::Instance();
        processor->CompleteCheckInvalidChar(checkInvalidCharacterQuery->byProc);
        break;
      }
      case 140:
      {
        auto *updateReRegistQuery = reinterpret_cast<_qry_case_unmandtrader_re_registsingleitem *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteReRegist(updateReRegistQuery);
        break;
      }
      case 141:
      {
        auto *cheatUpdateRegistTimeQuery =
          reinterpret_cast<_qry_case_unmandtrader_cheat_updateregisttime *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteUpdateCheatRegistTime(cheatUpdateRegistTimeQuery);
        break;
      }
      case 142:
      {
        auto *raceBossAccumulationWinRate = reinterpret_cast<_qry_case_raceboss_accumulation_winrate *>(pData->m_sData);
        CRaceBossWinRate *mgr = CRaceBossWinRate::Instance();
        mgr->CompleteBossAccmulationWinRate(raceBossAccumulationWinRate);
        break;
      }
      case 143:
      {
        auto *updateUserGuildDataQuery = reinterpret_cast<_qry_case_update_user_guild_data *>(pData->m_sData);
        if (!pData->m_byResult)
        {
          CGuild *guild = &g_Guild[updateUserGuildDataQuery->dwGuildIndex];
          if (guild->m_dwSerial == updateUserGuildDataQuery->dwGuildSerial)
          {
            guild->Complete_DB_Update_Committee(reinterpret_cast<char *>(updateUserGuildDataQuery));
          }
        }
        break;
      }
      case 144:
      {
        auto *guildMasterLastConnQuery = reinterpret_cast<_qry_case_select_guild_master_lastconn *>(pData->m_sData);
        CGuild *guild = &g_Guild[guildMasterLastConnQuery->dwGuildIndex];
        if (guild->m_dwSerial == guildMasterLastConnQuery->dwGuildSerial)
        {
          guild->CompleteSelectMasterLastConn(guildMasterLastConnQuery->dwLastConnTime);
        }
        break;
      }
      case 145:
      {
        auto *characterRenameQuery = reinterpret_cast<_qry_case_character_rename *>(pData->m_sData);
        g_PotionMgr.Complete_RenameChar_DB_Select(pData->m_byResult, reinterpret_cast<char *>(characterRenameQuery));
        break;
      }
      case 146:
      {
        auto *characterRenameQuery = reinterpret_cast<_qry_case_character_rename *>(pData->m_sData);
        g_PotionMgr.Complete_RenameChar_DB_Update(pData->m_byResult, reinterpret_cast<char *>(characterRenameQuery));
        break;
      }
      case 147:
      {
        auto *loginProcUpdateCompleteQuery =
          reinterpret_cast<_qry_case_unmandtrader_re_registsingleitem_roll_back *>(pData->m_sData);
        CUnmannedTraderController *controller = CUnmannedTraderController::Instance();
        controller->CompleteReRegistRollBack(loginProcUpdateCompleteQuery);
        break;
      }
      case 149:
      {
        auto *updateVoteAvailableQuery = reinterpret_cast<_qry_case_update_vote_available *>(pData->m_sData);
        CompleteUpdateVoteAvailable(reinterpret_cast<char *>(updateVoteAvailableQuery));
        break;
      }
      case 150:
      {
        auto *updatePlayerVoteInfoQuery = reinterpret_cast<_qry_case_update_player_vote_info *>(pData->m_sData);
        CompleteUpdatePlayerVoteInfo(reinterpret_cast<char *>(updatePlayerVoteInfoQuery));
        break;
      }
      case 151:
      {
        auto *cheatPlayerVoteInfoQuery = reinterpret_cast<_qry_case_cheat_player_vote_info *>(pData->m_sData);
        CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, cheatPlayerVoteInfoQuery->dwCharSerial);
        if (player && player->m_bOper)
        {
          player->m_pUserDB->m_AvatorData.dbAvator.m_bOverlapVote = 0;
        }
        break;
      }
      case 152:
      {
        auto *updateServerResetTokenQuery = reinterpret_cast<_qry_case_update_server_reset_token *>(pData->m_sData);
        CompleteUpdateServerToken(reinterpret_cast<char *>(updateServerResetTokenQuery));
        break;
      }
      case 153:
      {
        auto *selectTimeLimitInfoQuery = reinterpret_cast<_qry_case_select_timelimit_info *>(pData->m_sData);
        unsigned int logoutSeconds = m_pTimeLimitMgr->ClacLastLogoutTimeSec(
          selectTimeLimitInfoQuery->dwLastLogoutTime);
        BOOL isLimit = 1;
        unsigned __int16 endPlayTime = m_pTimeLimitMgr->GetEndPlayTime();
        isLimit = logoutSeconds >= 60u * static_cast<unsigned int>(endPlayTime);
        _time_limit_accum_logouttime_result_zocl result{};
        result.bEnable = static_cast<int>(isLimit);
        result.dwAccumLogoutTime = logoutSeconds;
        unsigned __int8 type[16]{};
        type[0] = 13;
        type[1] = static_cast<unsigned __int8>(-110);
        g_Network.m_pProcess[0]->LoadSendMsg(
          selectTimeLimitInfoQuery->wIndex,
          type,
          reinterpret_cast<char *>(&result),
          sizeof(result));
        break;
      }
      case 155:
      {
        auto *insertTimeLimitInfoQuery = reinterpret_cast<_qry_case_insert_timelimit_info *>(pData->m_sData);
        if (!m_pTimeLimitMgr->UpdatePlayerStatus(
              insertTimeLimitInfoQuery->wIndex,
              insertTimeLimitInfoQuery->dwFatigue,
              static_cast<unsigned __int8>(insertTimeLimitInfoQuery->wStatus)))
        {
          m_logSystemError.Write( "Error_DQSComplete_qry_case_insert_timelimit_info");
        }
        m_pTimeLimitMgr->ReSetPercent(insertTimeLimitInfoQuery->wIndex);
        break;
      }
      case 161:
      {
        auto *limitRunRequest = reinterpret_cast<_manage_client_limit_run_request_acwr *>(pData->m_sData);
        CompleteUpdateSetLimitRun(pData->m_byResult, reinterpret_cast<char *>(limitRunRequest));
        break;
      }
      case 165:
      {
        CGoldenBoxItemMgr *mgr = CGoldenBoxItemMgr::Instance();
        mgr->BoxItemDataCopy();
        break;
      }
      case 171:
      {
        auto *lobbyLogoutQuery = reinterpret_cast<_qry_case_lobby_logout *>(pData->m_sData);
        Complete_Select_RegeAvator_For_Lobby_Logout(lobbyLogoutQuery);
        break;
      }
      case 175:
      {
        auto *updateDataForPostSendQuery = reinterpret_cast<_qry_case_update_data_for_post_send *>(pData->m_sData);
        Complete_db_Update_Data_For_Post_Send(updateDataForPostSendQuery);
        break;
      }
      case 176:
      {
        auto *updateDataForTradeQuery = reinterpret_cast<_qry_case_update_data_for_trade *>(pData->m_sData);
        Complete_db_Update_Data_For_Trade(updateDataForTradeQuery);
        break;
      }
      default:
        break;
    }

    pData->m_bUse = false;
    pData->m_bLoad = false;
    m_listDQSDataEmpty.PushNode_Back(outIndex);
  }
}

void CMainThread::Reged_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    auto *regedQuery = reinterpret_cast<_qry_sheet_reged *>(pData->m_sData);
    user->Reged_Char_Complete(
      pData->m_byResult,
      regedQuery->RegedData,
      regedQuery->ArrangedData);
  }
}

void CMainThread::Insert_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    auto *insertQuery = reinterpret_cast<_qry_sheet_insert *>(pData->m_sData);
    user->Insert_Char_Complete(pData->m_byResult, &insertQuery->InsertData);
  }
}

void CMainThread::Delete_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    auto *deleteQuery = reinterpret_cast<_qry_sheet_delete *>(pData->m_sData);
    user->Delete_Char_Complete(pData->m_byResult, deleteQuery->bySlotIndex);
  }
}

void CMainThread::Select_Avator_Complete(_DB_QRY_SYN_DATA *pData)
{
  CUserDB *user = &g_UserDB[pData->m_idWorld.wIndex];
  if (user->m_bActive && user->m_idWorld.dwSerial == pData->m_idWorld.dwSerial)
  {
    _qry_sheet_load *loadQuery = reinterpret_cast<_qry_sheet_load *>(pData->m_sData);
    user->Select_Char_Complete(
      pData->m_byResult,
      &loadQuery->LoadData,
      loadQuery->bAddItem,
      loadQuery->dwAddDalant,
      loadQuery->dwAddGold,
      loadQuery->dwCheckSum,
      loadQuery->bTrunkAddItem,
      loadQuery->byTrunkOldSlot,
      loadQuery->dTrunkOldDalant,
      loadQuery->dTrunkOldGold,
      loadQuery->bCreateTrunkFree,
      loadQuery->bExtTrunkAddItem,
      loadQuery->byExtTrunkOldSlot);
    _db_complete_event_classrefine(
      pData->m_idWorld.wIndex,
      pData->m_idWorld.dwSerial,
      loadQuery->byRefinedCnt,
      loadQuery->dwRefineDate);
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
      sprintf_s(buffer, sizeof(buffer),
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
    auto *contSaveQuery = reinterpret_cast<_qry_case_contsave *>(pData->m_sData);
    if (contSaveQuery->NewData.dbPostData.dbPost.m_bUpdate)
    {
      CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, pData->m_dwAccountSerial);
      if (player && player->m_bOper)
      {
        for (int nIndex = 0; nIndex < 50; ++nIndex)
        {
          _POSTSTORAGE_DB_BASE::__list *postEntry = &contSaveQuery->NewData.dbPostData.dbPost.m_PostList[nIndex];
          if (postEntry->bNew && postEntry->bRetProc && postEntry->bUpdate)
          {
            CPostData *postData = player->m_Param.m_PostStorage.GetPostDataFromInx(nIndex);
            if (postData && !user->m_AvatorData.dbPostData.dbPost.m_PostList[nIndex].bNew)
            {
              postData->m_dwPSSerial = postEntry->dwPSSerial;
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
      sprintf_s(buffer, sizeof(buffer),
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
      user->Cont_UserSave_Complete(pData->m_byResult, &contSaveQuery->NewData);
      if (contSaveQuery->bUpdateRefineCnt)
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
    auto *aliveCharQuery = reinterpret_cast<_qry_case_alive_char_payload *>(pData->m_sData);
    user->Alive_Char_Complete(
      pData->m_byResult,
      aliveCharQuery->in_byCase,
      aliveCharQuery->in_dwSerial,
      reinterpret_cast<_REGED *>(aliveCharQuery->out_AliveAvator));
  }
}
void CMainThread::SendWebRaceBossSMS(_DB_QRY_SYN_DATA *pData)
{
  auto *raceBossSmsQuery = reinterpret_cast<_qry_case_sendwebracebosssms *>(pData->m_sData);
  if (pData->m_byResult)
  {
    raceBossSmsQuery->dwCharactSerial = 0;
  }

  CRaceBossMsgController *controller = CRaceBossMsgController::Instance();
  controller->Send(
    raceBossSmsQuery->byRace,
    raceBossSmsQuery->dwCharactSerial,
    raceBossSmsQuery->szCharacterName,
    raceBossSmsQuery->wszMsg,
    raceBossSmsQuery->dwWebSendDBID);
}

void CMainThread::AddGuildBattleSchdule(_DB_QRY_SYN_DATA *pData)
{
  auto *addScheduleQuery = reinterpret_cast<_qry_case_addguildbattleschedule *>(pData->m_sData);
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->AddComplete(
    pData->m_byResult,
    addScheduleQuery->dwMapID,
    addScheduleQuery->dwID,
    addScheduleQuery->dwSLID);
}

void CMainThread::UpdateGuildBattleWinLoseRankInfo(_DB_QRY_SYN_DATA *pData)
{
  auto *winLoseRankQuery = reinterpret_cast<_qry_case_updatewinloseguildbattlerank *>(pData->m_sData);
  if (pData->m_byResult)
  {
    GUILD_BATTLE::CGuildBattleLogger *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
    logger->Log(
      "void CMainThread::UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA* pData) : Win R(%u) Serial(%u) Lose R(%u) Serial(%u) Fail!",
      winLoseRankQuery->byWinRace,
      winLoseRankQuery->dwWinGuildSerial,
      winLoseRankQuery->byLoseRace,
      winLoseRankQuery->dwLoseGuildSerial);
  }
  else
  {
    _qry_case_loadguildbattlerank qry{};
    qry.byRace = winLoseRankQuery->byWinRace;
    int size = static_cast<int>(qry.size());
    PushDQSData(-1, nullptr, 33, reinterpret_cast<char *>(&qry), size);
    if (winLoseRankQuery->byWinRace != winLoseRankQuery->byLoseRace)
    {
      qry.byRace = winLoseRankQuery->byLoseRace;
      size = static_cast<int>(qry.size());
      PushDQSData(-1, nullptr, 33, reinterpret_cast<char *>(&qry), size);
    }

    _qry_case_load_guildbattle_totalrecord total{};
    total.dwGuildSerial = winLoseRankQuery->dwWinGuildSerial;
    size = static_cast<int>(total.size());
    PushDQSData(-1, nullptr, 57, reinterpret_cast<char *>(&total), size);

    total.dwGuildSerial = winLoseRankQuery->dwLoseGuildSerial;
    size = static_cast<int>(total.size());
    PushDQSData(-1, nullptr, 57, reinterpret_cast<char *>(&total), size);
  }
}

void CMainThread::UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA *pData)
{
  auto *drawRankQuery = reinterpret_cast<_qry_case_updatedrawguildbattlerank *>(pData->m_sData);
  if (pData->m_byResult)
  {
    GUILD_BATTLE::CGuildBattleLogger *logger = GUILD_BATTLE::CGuildBattleLogger::Instance();
    logger->Log(
      "void CMainThread::UpdateGuildBattleDrawRankInfo(_DB_QRY_SYN_DATA* pData) : 1P R(%u) Serial(%u) 2P R(%u) Serial(%u) Fail!",
      drawRankQuery->by1PRace,
      drawRankQuery->dw1PGuildSerial,
      drawRankQuery->by2PRace,
      drawRankQuery->dw2PGuildSerial);
  }
  else
  {
    _qry_case_loadguildbattlerank qry{};
    qry.byRace = drawRankQuery->by1PRace;
    int size = static_cast<int>(qry.size());
    PushDQSData(-1, nullptr, 33, reinterpret_cast<char *>(&qry), size);
    if (drawRankQuery->by1PRace != drawRankQuery->by2PRace)
    {
      qry.byRace = drawRankQuery->by2PRace;
      size = static_cast<int>(qry.size());
      PushDQSData(-1, nullptr, 33, reinterpret_cast<char *>(&qry), size);
    }

    _qry_case_load_guildbattle_totalrecord total{};
    total.dwGuildSerial = drawRankQuery->dw1PGuildSerial;
    size = static_cast<int>(total.size());
    PushDQSData(-1, nullptr, 57, reinterpret_cast<char *>(&total), size);

    total.dwGuildSerial = drawRankQuery->dw2PGuildSerial;
    size = static_cast<int>(total.size());
    PushDQSData(-1, nullptr, 57, reinterpret_cast<char *>(&total), size);
  }
}

void CMainThread::UpdateLoadGuildBattleRank(_DB_QRY_SYN_DATA *pData)
{
  auto *loadGuildBattleRankQuery = reinterpret_cast<_qry_case_loadguildbattlerank *>(pData->m_sData);
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->CompleteUpdateRank(
    pData->m_byResult,
    loadGuildBattleRankQuery->byRace,
    &loadGuildBattleRankQuery->byLoadDataStartPosition);
}
void CMainThread::InGuildbattleCost(_DB_QRY_SYN_DATA *pData)
{
  auto *inBattleCostQuery = reinterpret_cast<_qry_case_in_guildbattlecost *>(pData->m_sData);
  CGuild *guild = &g_Guild[inBattleCostQuery->dwGuildIndex];
  if (guild->m_dwSerial == inBattleCostQuery->dwGuildSerial)
  {
    guild->m_bIOWait = 0;
    if (!pData->m_byResult)
    {
      guild->m_byMoneyOutputKind = 4;
      long double ioGold = static_cast<long double>(inBattleCostQuery->dwAddGold);
      long double ioDalant = static_cast<long double>(inBattleCostQuery->dwAddDalant);
      guild->IOMoney(
        "Scramble Cost",
        inBattleCostQuery->dwGuildSerial,
        ioDalant,
        ioGold,
        inBattleCostQuery->out_totaldalant,
        inBattleCostQuery->out_totalgold,
        inBattleCostQuery->byDate,
        true);
    }
  }
}

void CMainThread::OutSrcGuildbattleCost(_DB_QRY_SYN_DATA *pData)
{
  auto *srcGuildOutBattleCostQuery = reinterpret_cast<_qry_case_src_guild_out_guildbattlecost *>(pData->m_sData);
  CGuild *guild = &g_Guild[srcGuildOutBattleCostQuery->dwGuildIndex];
  if (guild->m_dwSerial == srcGuildOutBattleCostQuery->dwGuildSerial)
  {
    guild->m_bIOWait = 0;
    if (srcGuildOutBattleCostQuery->byProcRet)
    {
      guild->SendMsg_GuildBattleSuggestResult(166, guild->m_GuildBattleSugestMatter.pkDest->m_wszName);
      guild->m_GuildBattleSugestMatter.Clear();
    }
    else if (!pData->m_byResult)
    {
      guild->m_byMoneyOutputKind = 4;
      long double ioGold = static_cast<long double>(srcGuildOutBattleCostQuery->dwSubGold);
      long double ioDalant = static_cast<long double>(srcGuildOutBattleCostQuery->dwSubDalant);
      guild->IOMoney(
        "Scramble Cost",
        srcGuildOutBattleCostQuery->dwGuildSerial,
        ioDalant,
        ioGold,
        srcGuildOutBattleCostQuery->out_totaldalant,
        srcGuildOutBattleCostQuery->out_totalgold,
        srcGuildOutBattleCostQuery->byDate,
        false);
      guild->CompleteOutGuildbattleCost(
        srcGuildOutBattleCostQuery->dwSrcGuildSerial,
        srcGuildOutBattleCostQuery->dwStartTimeInx,
        srcGuildOutBattleCostQuery->dwMemberCntInx,
        srcGuildOutBattleCostQuery->dwMapInx);
    }
  }
}

void CMainThread::OutDestGuildbattleCost(_DB_QRY_SYN_DATA *pData)
{
  auto *destGuildOutBattleCostQuery = reinterpret_cast<_qry_case_dest_guild_out_guildbattlecost *>(pData->m_sData);
  CGuild *guild = &g_Guild[destGuildOutBattleCostQuery->dwGuildIndex];
  if (guild->m_dwSerial == destGuildOutBattleCostQuery->dwGuildSerial)
  {
    guild->m_bIOWait = 0;
    if (pData->m_byResult)
    {
      const int dTotalDalant = 5000;
      g_Main.m_logSystemError.Write(
        "OutDestGuildbattleCost(...) : (%d)Guld Cur Dal(%f) Gold(%f) GuildBattleCost(%d)Gold _qry_case_out_guildbattlecost Fail!",
        destGuildOutBattleCostQuery->dwGuildSerial,
        guild->m_dTotalDalant,
        guild->m_dTotalGold,
        dTotalDalant);
    }
    else
    {
      guild->m_byMoneyOutputKind = 4;
      long double ioGold = static_cast<long double>(destGuildOutBattleCostQuery->dwSubGold);
      long double ioDalant = static_cast<long double>(destGuildOutBattleCostQuery->dwSubDalant);
      guild->IOMoney(
        "Scramble Cost",
        destGuildOutBattleCostQuery->dwGuildSerial,
        ioDalant,
        ioGold,
        destGuildOutBattleCostQuery->out_totaldalant,
        destGuildOutBattleCostQuery->out_totalgold,
        destGuildOutBattleCostQuery->byDate,
        false);
    }
  }
}

void CMainThread::InGuildbattleRewardMoney(_DB_QRY_SYN_DATA *pData)
{
  auto *inGuildBattleRewardMoneyQuery = reinterpret_cast<_qry_case_in_guildbattlerewardmoney *>(pData->m_sData);
  CGuild *guild = &g_Guild[inGuildBattleRewardMoneyQuery->dwGuildIndex];
  if (guild->m_dwSerial == inGuildBattleRewardMoneyQuery->dwGuildSerial)
  {
    guild->m_bIOWait = 0;
    if (!pData->m_byResult)
    {
      guild->m_byMoneyOutputKind = 5;
      long double ioGold = static_cast<long double>(inGuildBattleRewardMoneyQuery->dwAddGold);
      long double ioDalant = static_cast<long double>(inGuildBattleRewardMoneyQuery->dwAddDalant);
      guild->IOMoney(
        "Scramble Reward",
        inGuildBattleRewardMoneyQuery->dwGuildSerial,
        ioDalant,
        ioGold,
        inGuildBattleRewardMoneyQuery->out_totaldalant,
        inGuildBattleRewardMoneyQuery->out_totalgold,
        inGuildBattleRewardMoneyQuery->byDate,
        true);
    }
  }
}

void CMainThread::UpdateReservedGuildBattleSchedule(_DB_QRY_SYN_DATA *pData)
{
  auto *updateReservedScheduleQuery = reinterpret_cast<_qry_case_updatereservedschedule *>(pData->m_sData);
  CGuildBattleController *controller = CGuildBattleController::Instance();
  controller->CompleteUpdateReservedSchedule(
    updateReservedScheduleQuery->dwMapID,
    &updateReservedScheduleQuery->byLoadDataStartPosition);
}

void CMainThread::CompleteLoadGuildBattleTotalRecord(unsigned __int8 byRet, char *pLoadData)
{
  auto *loadGuildBattleTotalRecordQuery = reinterpret_cast<_qry_case_load_guildbattle_totalrecord *>(pLoadData);
  if (!byRet)
  {
    CGuild *guild = GetGuildDataFromSerial(g_Guild, 500, loadGuildBattleTotalRecordQuery->dwGuildSerial);
    if (guild)
    {
      guild->UpdateGuildBattleWinCnt(
        loadGuildBattleTotalRecordQuery->dwTotWinCnt,
        loadGuildBattleTotalRecordQuery->dwTotDrawCnt,
        loadGuildBattleTotalRecordQuery->dwTotLoseCnt);
    }
    else
    {
      m_logSystemError.Write(
        "CMainThread::CompleteLoadGuildBattleTotalRecord( BYTE byRet(%u), char * pLoadData ) :GetGuildDataFromSerial( g_Guild, MAX_GUILD, pSheet->dwGuildSerial(%u) ) == NULL!",
        loadGuildBattleTotalRecordQuery->dwGuildSerial,
        loadGuildBattleTotalRecordQuery->dwGuildSerial);
    }
  }
}

void CMainThread::InAtradTaxMoney(_DB_QRY_SYN_DATA *pData)
{
  auto *inATradeTaxQuery = reinterpret_cast<_qry_case_in_atrade_tax *>(pData->m_sData);
  CUnmannedTraderTaxRateManager *manager = CUnmannedTraderTaxRateManager::Instance();
  manager->DQSCompleteInAtradTaxMoney(inATradeTaxQuery->byRace, reinterpret_cast<char *>(inATradeTaxQuery));
}
void CMainThread::Load_PostStorage_Complete(char *pData)
{
  auto *postStorageListQuery = reinterpret_cast<_qry_case_post_storage_list_get *>(pData);
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, postStorageListQuery->dwMasterSerial);
  if (postStorageListQuery->byProcRet == 1)
  {
    m_logSystemError.Write(
      "Load_PostStorage_Complete() : Select_PostStorageList(Serial:%d) => failed",
      postStorageListQuery->dwMasterSerial);

    if (player && player->m_bLoad)
    {
      player->m_bPostLoading = false;
    }
  }

  if (player && player->m_bLoad && postStorageListQuery->byProcRet != 1)
  {
    // Post load fix (non-IDA): ignore stale or duplicate case-79 completions.
    if (player->m_bPostLoad || !player->m_bPostLoading)
    {
      return;
    }

    CPostStorage *postStorage = &player->m_Param.m_PostStorage;
    unsigned int count = postStorageListQuery->dwCount;

    for (unsigned int n = 0; n < count; ++n)
    {
      _qry_case_post_storage_list_get::__list *entry = &postStorageListQuery->List[n];
      if (entry->wStorageIndex != 255)
      {
        postStorage->AddPostTitleDataByStorageIndex(
          entry->wStorageIndex,
          entry->byIndex,
          entry->dwSerial,
          entry->byState,
          entry->wszSendName,
          entry->wszTitle,
          entry->key,
          entry->dwDur,
          entry->dwUpt,
          entry->dwGold,
          entry->lnUID);
      }
    }

    for (unsigned int n = 0; n < count; ++n)
    {
      _qry_case_post_storage_list_get::__list *entry = &postStorageListQuery->List[n];
      if (entry->wStorageIndex == 255)
      {
        postStorage->AddPostTitleData(
          entry->byIndex,
          entry->dwSerial,
          entry->byState,
          entry->wszSendName,
          entry->wszTitle,
          entry->key,
          entry->dwDur,
          entry->dwUpt,
          entry->dwGold,
          entry->lnUID);
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
    PushDQSData(-1, nullptr, 80, reinterpret_cast<char *>(&qry), size);
  }
}
void CMainThread::Load_ReturnPost_Complete(char *pData)
{
  auto *postReturnListQuery = reinterpret_cast<_qry_case_post_return_list_get *>(pData);
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, postReturnListQuery->dwMasterSerial);
  if (postReturnListQuery->byProcRet == 1)
  {
    m_logSystemError.Write(
      "Load_ReturnPost_Complete() : Select_ReturnPost(Serial:%d) => failed",
      postReturnListQuery->dwMasterSerial);

    if (player && player->m_bLoad)
    {
      player->m_bPostLoading = false;
    }
  }

  if (player && player->m_bLoad && postReturnListQuery->byProcRet != 1)
  {
    // Post load fix (non-IDA): case 80 only completes the active load chain once.
    if (player->m_bPostLoad || !player->m_bPostLoading)
    {
      return;
    }

    CPostReturnStorage *returnStorage = &player->m_Param.m_ReturnPostStorage;
    unsigned int count = postReturnListQuery->dwCount;
    for (unsigned int j = 0; j < count; ++j)
    {
      _qry_case_post_return_list_get::__list *entry = &postReturnListQuery->List[j];
      CPostData *post = returnStorage->AddReturnPost(
        entry->byErr,
        entry->dwSerial,
        entry->byState,
        entry->wszRecvName,
        entry->wszTitle,
        entry->wszContent,
        entry->key,
        entry->dwDur,
        entry->dwUpt,
        entry->dwGold,
        entry->lnUID);
      if (post->m_Key.IsFilled() || post->m_dwGold)
      {
        CPlayer::s_MgrItemHistory.post_returnreceive(post, player->m_szItemHistoryFileName);
      }
    }

    CPlayer::s_MgrItemHistory.return_post_storage(
      &player->m_Param.m_ReturnPostStorage,
      player->m_szItemHistoryFileName);
    player->m_bPostLoad = 1;
    player->m_bPostLoading = false;

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
  auto *postContentQuery = reinterpret_cast<_qry_case_post_content_get *>(pData);
  if (postContentQuery->byProcRet == 1)
  {
    m_logSystemError.Write(
      "Load_Content_Complete() : Select_PostContent() : MasterSerial=%d ,PostSerial=%d",
      postContentQuery->dwMasterSerial,
      postContentQuery->dwSerial);
  }

  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, postContentQuery->dwMasterSerial);
  if (player && player->m_bOper && postContentQuery->byProcRet != 1)
  {
    CPostData *post = player->m_Param.m_PostStorage.GetPostDataFromInx(postContentQuery->dwIndex);
    if (post)
    {
      post->SetPostContent(postContentQuery->wszContent);
      player->SendMsg_PostContent(
        0,
        postContentQuery->dwIndex,
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
  auto *updateVoteAvailableQuery = reinterpret_cast<_qry_case_update_vote_available *>(pData);
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, updateVoteAvailableQuery->dwCharSerial);
  if (player && player->m_bOper)
  {
    player->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable = updateVoteAvailableQuery->byVoteEnable;
  }
}

void CMainThread::CompleteUpdatePlayerVoteInfo(char *pData)
{
  auto *updatePlayerVoteInfoQuery = reinterpret_cast<_qry_case_update_player_vote_info *>(pData);
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, updatePlayerVoteInfoQuery->dwCharSerial);
  if (player && player->m_bOper)
  {
    player->m_pUserDB->m_AvatorData.dbSupplement.VoteEnable = updatePlayerVoteInfoQuery->byVoteEnable;
    player->m_pUserDB->m_AvatorData.dbSupplement.dwAccumPlayTime = updatePlayerVoteInfoQuery->dwAccumPlayTime;
    player->m_pUserDB->m_AvatorData.dbSupplement.byVoted = updatePlayerVoteInfoQuery->byIsVoted;
    player->m_pUserDB->m_AvatorData.dbSupplement.wScanerCnt = updatePlayerVoteInfoQuery->wScaner;
    player->m_pUserDB->m_AvatorData.dbSupplement.dwScanerGetDate =
      static_cast<unsigned int>(updatePlayerVoteInfoQuery->dwScanerData);
  }
}

void CMainThread::CompleteUpdateServerToken(char *pData)
{
  auto *updateServerResetTokenQuery = reinterpret_cast<_qry_case_update_server_reset_token *>(pData);
  g_Main.m_dwServerResetToken = updateServerResetTokenQuery->dwServerToken;
}

void CMainThread::CompleteUpdateSetLimitRun(char byRet, char *pData)
{
  auto *limitRunRequest = reinterpret_cast<_manage_client_limit_run_request_acwr *>(pData);
  _manage_client_limit_run_result_wrac result{};
  result.byRet = static_cast<unsigned __int8>(byRet);
  std::memcpy(&result.idLocal, limitRunRequest, sizeof(result.idLocal));
  result.byLoginServerIndex = limitRunRequest->byCode;

  unsigned __int8 type[16]{};
  type[0] = 1;
  type[1] = 26;
  g_Network.m_pProcess[1]->LoadSendMsg(
    0,
    type,
    reinterpret_cast<char *>(&result),
    static_cast<unsigned __int16>(sizeof(result)));
}
void CMainThread::Complete_Select_RegeAvator_For_Lobby_Logout(_qry_case_lobby_logout *pSheet)
{
  CUserDB::s_MgrLobbyHistory.lobby_disconnect(pSheet, pSheet->szLobbyHistoryFileName);
}

void CMainThread::Complete_db_Update_Data_For_Post_Send(_qry_case_update_data_for_post_send *pSheet)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, pSheet->dwSerial);
  if (player && player->m_bOper)
  {
    player->m_pUserDB->m_AvatorData_bk.dbAvator.m_dwGold = player->m_pUserDB->m_AvatorData.dbAvator.m_dwGold;
    player->m_pUserDB->m_AvatorData_bk.dbInven = player->m_pUserDB->m_AvatorData.dbInven;
    player->SendMsg_PostSendReply(0);
  }
}

void CMainThread::Complete_db_Update_Data_For_Trade(_qry_case_update_data_for_trade *pSheet)
{
  for (int i = 0; i < 2; ++i)
  {
    CPlayer *player = GetPtrPlayerFromSerial(g_Player, 2532, pSheet->tradelist[i].dwSerial);
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

