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
#include "log_case_charselect.h"
#include "qry_case_cash_limsale.h"
#include "qry_sheet_delete.h"
#include "qry_sheet_insert.h"
#include "qry_sheet_load.h"
#include "qry_sheet_reged.h"
#include "qry_logout.h"






void CMainThread::OnDQSRun()
{
  unsigned __int8 v3; // al
  unsigned __int8 inserted; // al
  unsigned __int8 v5; // al
  unsigned __int8 Avator; // al
  unsigned __int8 updated; // al
  unsigned __int8 v8; // al
  unsigned __int8 v9; // al
  unsigned __int8 v10; // al
  unsigned __int8 v11; // al
  unsigned __int8 v12; // al
  unsigned __int8 v13; // al
  unsigned __int8 v14; // al
  unsigned __int8 v15; // al
  unsigned __int8 v16; // al
  unsigned __int8 v17; // al
  unsigned __int8 v18; // al
  unsigned __int8 v19; // al
  unsigned __int8 v20; // al
  unsigned __int8 v21; // al
  unsigned __int8 v22; // al
  unsigned __int8 v23; // al
  unsigned __int8 v24; // al
  unsigned __int8 v25; // al
  unsigned __int8 v26; // al
  unsigned __int8 v27; // al
  CGuildBattleController *v28; // rax
  unsigned __int8 v29; // al
  CGuildBattleController *v30; // rax
  CGuildBattleController *v31; // rax
  CGuildBattleController *v32; // rax
  CGuildBattleController *v33; // rax
  unsigned __int8 v34; // al
  unsigned __int8 v35; // al
  unsigned __int8 v36; // al
  unsigned __int8 v37; // al
  CGuildBattleController *v38; // rax
  unsigned __int8 v39; // al
  unsigned __int8 v40; // al
  CTotalGuildRankManager *v41; // rax
  CWeeklyGuildRankManager *v42; // rax
  CWeeklyGuildRankManager *v43; // rax
  unsigned __int8 v44; // al
  AutoMineMachineMng *v45; // rax
  unsigned __int8 v46; // al
  AutominePersonalMgr *v47; // rax
  unsigned __int8 v48; // al
  CUnmannedTraderController *v49; // rax
  unsigned __int8 v50; // al
  CUnmannedTraderController *v51; // rax
  unsigned __int8 v52; // al
  CUnmannedTraderController *v53; // rax
  unsigned __int8 v54; // al
  CUnmannedTraderController *v55; // rax
  unsigned __int8 v56; // al
  CUnmannedTraderController *v57; // rax
  unsigned __int8 v58; // al
  CUnmannedTraderController *v59; // rax
  unsigned __int8 v60; // al
  CUnmannedTraderController *v61; // rax
  unsigned __int8 v62; // al
  CUnmannedTraderController *v63; // rax
  unsigned __int8 v64; // al
  CUnmannedTraderController *v65; // rax
  unsigned __int8 v66; // al
  CUnmannedTraderController *v67; // rax
  unsigned __int8 v68; // al
  CUnmannedTraderController *v69; // rax
  unsigned __int8 v70; // al
  CUnmannedTraderController *v71; // rax
  unsigned __int8 v72; // al
  CUnmannedTraderController *v73; // rax
  unsigned __int8 v74; // al
  CUnmannedTraderController *v75; // rax
  unsigned __int8 v76; // al
  unsigned __int8 v77; // al
  CPostSystemManager *v78; // rax
  unsigned __int8 v79; // al
  CPostSystemManager *v80; // rax
  unsigned __int8 v81; // al
  unsigned __int8 PostStorage; // al
  unsigned __int8 ReturnPost; // al
  unsigned __int8 Content; // al
  unsigned __int8 inited; // al
  CPvpUserAndGuildRankingSystem *v86; // rax
  unsigned __int8 v87; // al
  CPvpUserAndGuildRankingSystem *v88; // rax
  unsigned __int8 v89; // al
  CPvpUserAndGuildRankingSystem *v90; // rax
  unsigned __int8 v91; // al
  CPvpUserAndGuildRankingSystem *v92; // rax
  unsigned __int8 v93; // al
  CPvpUserAndGuildRankingSystem *v94; // rax
  unsigned __int8 v95; // al
  CPvpUserAndGuildRankingSystem *v96; // rax
  unsigned __int8 v97; // al
  CPvpUserAndGuildRankingSystem *v98; // rax
  unsigned __int8 v99; // al
  CPvpUserAndGuildRankingSystem *v100; // rax
  unsigned __int8 v101; // al
  CPvpUserAndGuildRankingSystem *v102; // rax
  unsigned __int8 v103; // al
  CPvpUserAndGuildRankingSystem *v104; // rax
  unsigned __int8 v105; // al
  CPvpUserAndGuildRankingSystem *v106; // rax
  unsigned __int8 v107; // al
  CPvpUserAndGuildRankingSystem *v108; // rax
  unsigned __int8 v109; // al
  CPvpUserAndGuildRankingSystem *v110; // rax
  unsigned __int8 v111; // al
  CPvpUserAndGuildRankingSystem *v112; // rax
  unsigned __int8 v113; // al
  CPvpUserAndGuildRankingSystem *v114; // rax
  unsigned __int8 v115; // al
  CPvpUserAndGuildRankingSystem *v116; // rax
  unsigned __int8 v117; // al
  CPvpUserAndGuildRankingSystem *v118; // rax
  unsigned __int8 v119; // al
  CPvpUserAndGuildRankingSystem *v120; // rax
  unsigned __int8 v121; // al
  CPvpUserAndGuildRankingSystem *v122; // rax
  unsigned __int8 v123; // al
  CPvpUserAndGuildRankingSystem *v124; // rax
  unsigned __int8 v125; // al
  CPvpUserAndGuildRankingSystem *v126; // rax
  unsigned __int8 v127; // al
  CPvpUserAndGuildRankingSystem *v128; // rax
  unsigned __int8 v129; // al
  CItemStoreManager *v130; // rax
  unsigned __int8 v131; // al
  CItemStoreManager *v132; // rax
  unsigned __int8 v133; // al
  CandidateMgr *v134; // rax
  unsigned __int8 v135; // al
  CandidateMgr *v136; // rax
  unsigned __int8 v137; // al
  CandidateMgr *v138; // rax
  unsigned __int8 v139; // al
  CandidateMgr *v140; // rax
  unsigned __int8 v141; // al
  PatriarchElectProcessor *v142; // rax
  unsigned __int8 v143; // al
  PatriarchElectProcessor *v144; // rax
  unsigned __int8 v145; // al
  CandidateMgr *v146; // rax
  unsigned __int8 v147; // al
  PatriarchElectProcessor *v148; // rax
  PatriarchElectProcessor *v149; // rax
  unsigned __int8 v150; // al
  PatriarchElectProcessor *v151; // rax
  unsigned __int8 v152; // al
  CandidateMgr *v153; // rax
  unsigned __int8 v154; // al
  CPostSystemManager *v155; // rax
  unsigned __int8 v156; // al
  unsigned __int8 PatriarchComm; // al
  CHonorGuild *v158; // rax
  unsigned __int8 v159; // al
  CHonorGuild *v160; // rax
  unsigned __int8 v161; // al
  unsigned __int8 v162; // al
  CandidateMgr *v163; // rax
  unsigned __int8 v164; // al
  CandidateMgr *v165; // rax
  unsigned __int8 v166; // al
  CRaceBossWinRate *v167; // rax
  unsigned __int8 BossAccmulationWinRate; // al
  unsigned __int8 Cash_LimSale; // al
  CashItemRemoteStore *v170; // rax
  unsigned __int8 v171; // al
  unsigned __int8 v172; // al
  unsigned __int8 v173; // al
  unsigned __int8 v174; // al
  unsigned __int8 v175; // al
  unsigned __int8 v176; // al
  int queryResult; // eax
  int v178; // ecx
  unsigned int queryIndex; // [rsp+C4h] [rbp-544h] BYREF
  _DB_QRY_SYN_DATA *queryEntry; // [rsp+D8h] [rbp-530h]
  char *v186; // [rsp+100h] [rbp-508h]
  char *v187; // [rsp+108h] [rbp-500h]
  char *v188; // [rsp+110h] [rbp-4F8h]
  char *v189; // [rsp+118h] [rbp-4F0h]
  unsigned int *v190; // [rsp+120h] [rbp-4E8h]
  _worlddb_economy_history_info pEconomyData; // [rsp+140h] [rbp-4C8h] BYREF
  char *v193; // [rsp+220h] [rbp-3E8h]
  char *v194; // [rsp+228h] [rbp-3E0h]
  char *v195; // [rsp+230h] [rbp-3D8h]
  char *v196; // [rsp+238h] [rbp-3D0h]
  char *v197; // [rsp+240h] [rbp-3C8h]
  char *v198; // [rsp+248h] [rbp-3C0h]
  char *v199; // [rsp+250h] [rbp-3B8h]
  char *v200; // [rsp+258h] [rbp-3B0h]
  char *v201; // [rsp+260h] [rbp-3A8h]
  char *v202; // [rsp+268h] [rbp-3A0h]
  char *v203; // [rsp+270h] [rbp-398h]
  _qry_case_gm_greetingmsg *pSheet; // [rsp+278h] [rbp-390h]
  _qry_case_race_greetingmsg *v205; // [rsp+280h] [rbp-388h]
  _qry_case_guild_greetingmsg *v206; // [rsp+288h] [rbp-380h]
  char *v207; // [rsp+290h] [rbp-378h]
  char *v208; // [rsp+298h] [rbp-370h]
  char *v209; // [rsp+2A0h] [rbp-368h]
  _qry_case_sendwebracebosssms *v210; // [rsp+2A8h] [rbp-360h]
  char *v211; // [rsp+2B0h] [rbp-358h]
  char *v212; // [rsp+2B8h] [rbp-350h]
  unsigned __int8 *v213; // [rsp+2C0h] [rbp-348h]
  unsigned int *v214; // [rsp+2C8h] [rbp-340h]
  char *v215; // [rsp+2D0h] [rbp-338h]
  char *v216; // [rsp+2D8h] [rbp-330h]
  char *v217; // [rsp+2E0h] [rbp-328h]
  char *v218; // [rsp+2E8h] [rbp-320h]
  char *v219; // [rsp+2F0h] [rbp-318h]
  char *v220; // [rsp+2F8h] [rbp-310h]
  _qry_case_update_guildmaster *v221; // [rsp+300h] [rbp-308h]
  char *v222; // [rsp+308h] [rbp-300h]
  char *v223; // [rsp+310h] [rbp-2F8h]
  _guildbattle_totalrecord *pkInfo; // [rsp+318h] [rbp-2F0h]
  _unmannedtrader_reserved_schedule_info *v225; // [rsp+320h] [rbp-2E8h]
  unsigned __int8 v226; // [rsp+328h] [rbp-2E0h]
  _qry_case_guildroom_insert *v227; // [rsp+330h] [rbp-2D8h]
  _qry_case_guildroom_update *v228; // [rsp+338h] [rbp-2D0h]
  char *szStartTime; // [rsp+340h] [rbp-2C8h]
  char *v230; // [rsp+348h] [rbp-2C0h]
  char CharSerialByNameRace; // [rsp+350h] [rbp-2B8h]
  char *v232; // [rsp+358h] [rbp-2B0h]
  unsigned __int8 *v233; // [rsp+360h] [rbp-2A8h]
  unsigned __int8 *v234; // [rsp+368h] [rbp-2A0h]
  char *v235; // [rsp+370h] [rbp-298h]
  _race_battle_log_info *pInfo; // [rsp+378h] [rbp-290h]
  unsigned __int8 *v237; // [rsp+380h] [rbp-288h]
  unsigned int *v238; // [rsp+388h] [rbp-280h]
  _qry_case_raceboss_accumulation_winrate *v239; // [rsp+390h] [rbp-278h]
  char *v240; // [rsp+398h] [rbp-270h]
  char *v241; // [rsp+3A0h] [rbp-268h]
  unsigned __int8 Conn; // [rsp+3A8h] [rbp-260h]
  char *v243; // [rsp+3B0h] [rbp-258h]
  char *v244; // [rsp+3B8h] [rbp-250h]
  unsigned __int8 *byVoteEnable; // [rsp+3C0h] [rbp-248h]
  char *v246; // [rsp+3C8h] [rbp-240h]
  char *v247; // [rsp+3D0h] [rbp-238h]
  char *v248; // [rsp+3D8h] [rbp-230h]
  qry_case_cash_limsale *pDbLimitedSale; // [rsp+3E0h] [rbp-228h]
  _db_cash_limited_sale *pNewData; // [rsp+3E8h] [rbp-220h]
  char *v251; // [rsp+3F0h] [rbp-218h]
  char *v252; // [rsp+3F8h] [rbp-210h]
  char LogoutTime; // [rsp+400h] [rbp-208h]
  char *v254; // [rsp+408h] [rbp-200h]
  char *v255; // [rsp+410h] [rbp-1F8h]
  int queryCase; // [rsp+420h] [rbp-1E8h]
  char *szData; // [rsp+428h] [rbp-1E0h]
  unsigned __int8 *byOutData; // [rsp+430h] [rbp-1D8h]
  unsigned __int8 *v259; // [rsp+438h] [rbp-1D0h]
  int v260; // [rsp+440h] [rbp-1C8h]
  char *pLoadData; // [rsp+448h] [rbp-1C0h]
  char *v262; // [rsp+450h] [rbp-1B8h]
  char *v263; // [rsp+458h] [rbp-1B0h]
  char *pdata; // [rsp+460h] [rbp-1A8h]
  char *v265; // [rsp+468h] [rbp-1A0h]
  char *pData; // [rsp+470h] [rbp-198h]
  char *v267; // [rsp+478h] [rbp-190h]
  char *v268; // [rsp+480h] [rbp-188h]
  char *v269; // [rsp+488h] [rbp-180h]
  char *v270; // [rsp+490h] [rbp-178h]
  char *v271; // [rsp+498h] [rbp-170h]
  char *v272; // [rsp+4A0h] [rbp-168h]
  char *v273; // [rsp+4A8h] [rbp-160h]
  char *v274; // [rsp+4B0h] [rbp-158h]
  char *v275; // [rsp+4B8h] [rbp-150h]
  char *v276; // [rsp+4C0h] [rbp-148h]
  char *v277; // [rsp+4C8h] [rbp-140h]
  char *v278; // [rsp+4D0h] [rbp-138h]
  char *v279; // [rsp+4D8h] [rbp-130h]
  int v280; // [rsp+4E0h] [rbp-128h]
  char *v281; // [rsp+4E8h] [rbp-120h]
  char *v282; // [rsp+4F0h] [rbp-118h]
  char *v283; // [rsp+4F8h] [rbp-110h]
  char *v284; // [rsp+500h] [rbp-108h]
  char *v285; // [rsp+508h] [rbp-100h]
  char *v286; // [rsp+510h] [rbp-F8h]
  char *v287; // [rsp+518h] [rbp-F0h]
  char *v288; // [rsp+520h] [rbp-E8h]
  char *v289; // [rsp+528h] [rbp-E0h]
  char *v290; // [rsp+530h] [rbp-D8h]
  char *v291; // [rsp+538h] [rbp-D0h]
  char *v292; // [rsp+540h] [rbp-C8h]
  char *v293; // [rsp+548h] [rbp-C0h]
  char *v294; // [rsp+550h] [rbp-B8h]
  char *v295; // [rsp+558h] [rbp-B0h]
  char *v296; // [rsp+560h] [rbp-A8h]
  char *v297; // [rsp+568h] [rbp-A0h]
  char *v298; // [rsp+570h] [rbp-98h]
  char *v299; // [rsp+578h] [rbp-90h]
  char *v300; // [rsp+580h] [rbp-88h]
  char *v301; // [rsp+588h] [rbp-80h]
  char *v302; // [rsp+590h] [rbp-78h]
  char *v303; // [rsp+598h] [rbp-70h]
  char *v304; // [rsp+5A0h] [rbp-68h]
  char *v305; // [rsp+5A8h] [rbp-60h]
  char *v306; // [rsp+5B0h] [rbp-58h]
  char *v307; // [rsp+5B8h] [rbp-50h]
  char *v308; // [rsp+5C0h] [rbp-48h]
  char *v309; // [rsp+5C8h] [rbp-40h]
  CLogFile *p_m_logDQS; // [rsp+5D0h] [rbp-38h]
  m_DBFrameRate.CalcSpeedPerFrame();
  if (m_pWorldDB )
    m_pWorldDB->CheckLogFileHour();
  while (m_listDQSData.PopNode_Front(&queryIndex) )
  {
    _time32(reinterpret_cast<__time32_t *>(&g_Main.m_tmDbUpdate));
    queryEntry = &m_DBQrySynData[queryIndex];
    m_DBQrySynData[queryIndex].m_byResult = 0;
    queryCase = queryEntry->m_byQryCase;
    switch (queryCase )
    {
      case 0:
      {
        _qry_sheet_reged *regedQuery = reinterpret_cast<_qry_sheet_reged *>(queryEntry->m_sData);
        v3 = db_Reged_Avator(
          regedQuery->dwAccountSerial,
          regedQuery->RegedData,
          regedQuery->ArrangedData,
          regedQuery->in_szIP);
        queryEntry->m_byResult = v3;
        break;
      }
      case 1:
      {
        _qry_sheet_insert *insertQuery = reinterpret_cast<_qry_sheet_insert *>(queryEntry->m_sData);
        inserted = db_Insert_Avator(
          insertQuery->dwAccountSerial,
          insertQuery->szAccountID,
          &insertQuery->InsertData,
          &insertQuery->InsertData.m_dwRecordNum);
        queryEntry->m_byResult = inserted;
        break;
      }
      case 2:
      {
        _qry_sheet_delete *deleteQuery = reinterpret_cast<_qry_sheet_delete *>(queryEntry->m_sData);
        v5 = db_Delete_Avator(deleteQuery->dwAvatorSerial, deleteQuery->byRaceCode);
        queryEntry->m_byResult = v5;
        break;
      }
      case 3:
      {
        _qry_sheet_load *loadQuery = reinterpret_cast<_qry_sheet_load *>(queryEntry->m_sData);
        Avator = db_Load_Avator(
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
        queryEntry->m_byResult = Avator;
        if (queryEntry->m_byResult
          || (CUserDB::ReRangeClientIndex(&loadQuery->LoadData), CUserDB::CheckDQSLoadCharacterData(&loadQuery->LoadData)))
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
        v186 = queryEntry->m_sData;
        updated = db_Update_Avator(*(_DWORD *)queryEntry->m_sData,
                    (_AVATOR_DATA *)&queryEntry->m_sData[4],
                    (_AVATOR_DATA *)&queryEntry->m_sData[37219],
                    queryEntry->m_sData[74434]);
        queryEntry->m_byResult = updated;
        db_Update_PostStorage(*(_DWORD *)v186,
          (_AVATOR_DATA *)(v186 + 4),
          (_AVATOR_DATA *)(v186 + 37219));
        if (v186[74435] )
        {
          v8 = _db_update_event_classrefine(queryEntry->m_idWorld.wIndex,
                 *(_DWORD *)v186,
                 v186[74436],
                 *((_DWORD *)v186 + 18610));
          queryEntry->m_byResult = v8;
        }
        _db_Update_SFDelayData(*(_DWORD *)v186, (_AVATOR_DATA *)(v186 + 4));
        break;
      case 6:
        v187 = queryEntry->m_sData;
        v9 = db_Update_Avator(*(_DWORD *)queryEntry->m_sData,
               (_AVATOR_DATA *)&queryEntry->m_sData[4],
               (_AVATOR_DATA *)&queryEntry->m_sData[37219],
               1);
        queryEntry->m_byResult = v9;
        db_Update_PostStorage(*(_DWORD *)v187,
          (_AVATOR_DATA *)(v187 + 4),
          (_AVATOR_DATA *)(v187 + 37219));
        if (v187[74434] )
        {
          v10 = _db_update_event_classrefine(queryEntry->m_idWorld.wIndex,
                  *(_DWORD *)v187,
                  v187[74435],
                  *((_DWORD *)v187 + 18609));
          queryEntry->m_byResult = v10;
        }
        _db_Update_SFDelayData(*(_DWORD *)v187, (_AVATOR_DATA *)(v187 + 4));
        break;
      case 7:
        v188 = queryEntry->m_sData;
        db_Log_AvatorLevel(*(_DWORD *)&queryEntry->m_sData[8], *(_DWORD *)queryEntry->m_sData, queryEntry->m_sData[4]);
        break;
      case 8:
        v189 = queryEntry->m_sData;
        db_Log_UserNum(*(_DWORD *)queryEntry->m_sData, *(_DWORD *)&queryEntry->m_sData[4]);
        break;
      case 9:
        v190 = (unsigned int *)queryEntry->m_sData;
        pEconomyData.dwManageValue = *(_DWORD *)&queryEntry->m_sData[56];
        memcpy_0(pEconomyData.dTradeDalant, &queryEntry->m_sData[32], sizeof(pEconomyData.dTradeDalant));
        memcpy_0(&pEconomyData, v190 + 2, 0x18uLL);
        memcpy_0(pEconomyData.dMineOre, v190 + 16, sizeof(pEconomyData.dMineOre));
        memcpy_0(pEconomyData.dCutOre, v190 + 34, sizeof(pEconomyData.dCutOre));
        db_Insert_Economy_History(*v190, &pEconomyData);
        break;
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
        v193 = queryEntry->m_sData;
        db_Update_PvpInfo(*(_DWORD *)queryEntry->m_sData,
          queryEntry->m_sData[4],
          (__int16 *)&queryEntry->m_sData[6],
          *(long double *)&queryEntry->m_sData[16]);
        break;
      case 12:
        v194 = queryEntry->m_sData;
        v11 = db_Update_Avator(*(_DWORD *)queryEntry->m_sData,
                (_AVATOR_DATA *)&queryEntry->m_sData[4],
                (_AVATOR_DATA *)&queryEntry->m_sData[37219],
                1);
        queryEntry->m_byResult = v11;
        if (v194[74434] )
        {
          v12 = _db_update_event_classrefine(queryEntry->m_idWorld.wIndex,
                  *(_DWORD *)v194,
                  v194[74435],
                  *((_DWORD *)v194 + 18609));
          queryEntry->m_byResult = v12;
        }
        db_Update_PostStorage(*(_DWORD *)v194,
          (_AVATOR_DATA *)(v194 + 4),
          (_AVATOR_DATA *)(v194 + 37219));
        break;
      case 13:
        v195 = queryEntry->m_sData;
        v13 = db_Add_PvpPoint(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[8]);
        queryEntry->m_byResult = v13;
        break;
      case 14:
        v196 = queryEntry->m_sData;
        v14 = db_Insert_Item(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[8],
                *(_DWORD *)&queryEntry->m_sData[12],
                queryEntry->m_sData[16]);
        queryEntry->m_byResult = v14;
        break;
      case 15:
        v197 = queryEntry->m_sData;
        v15 = db_Insert_guild((unsigned int *)&queryEntry->m_sData[56],
                &queryEntry->m_sData[4],
                queryEntry->m_sData[21],
                (unsigned int *)&queryEntry->m_sData[272]);
        queryEntry->m_byResult = v15;
        break;
      case 16:
        v198 = queryEntry->m_sData;
        v16 = db_update_guildmember_add(*(_DWORD *)&queryEntry->m_sData[12],
                *(_DWORD *)&queryEntry->m_sData[4],
                queryEntry->m_sData[20],
                *(_DWORD *)&queryEntry->m_sData[24]);
        queryEntry->m_byResult = v16;
        break;
      case 17:
        v199 = queryEntry->m_sData;
        v17 = db_update_guildmember_del(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[16],
                *(_DWORD *)&queryEntry->m_sData[4]);
        queryEntry->m_byResult = v17;
        break;
      case 18:
        v200 = queryEntry->m_sData;
        v18 = db_update_guildmember_del(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[8],
                *(_DWORD *)&queryEntry->m_sData[20]);
        queryEntry->m_byResult = v18;
        break;
      case 19:
        v201 = queryEntry->m_sData;
        v19 = db_input_guild_money(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[28],
                *(_DWORD *)&queryEntry->m_sData[36],
                *(_DWORD *)&queryEntry->m_sData[32],
                (long double *)&queryEntry->m_sData[56],
                (long double *)&queryEntry->m_sData[48],
                (unsigned __int8 *)&queryEntry->m_sData[40],
                &queryEntry->m_sData[4]);
        queryEntry->m_byResult = v19;
        break;
      case 20:
        v202 = queryEntry->m_sData;
        v20 = db_output_guild_money(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[28],
                *(_DWORD *)&queryEntry->m_sData[36],
                *(_DWORD *)&queryEntry->m_sData[32],
                (long double *)&queryEntry->m_sData[56],
                (long double *)&queryEntry->m_sData[48],
                (unsigned __int8 *)&queryEntry->m_sData[40],
                &queryEntry->m_sData[4],
                (unsigned __int8 *)&queryEntry->m_sData[64]);
        queryEntry->m_byResult = v20;
        break;
      case 21:
        v203 = queryEntry->m_sData;
        v21 = db_buy_emblem(*(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[12],
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[8],
                *(_DWORD *)&queryEntry->m_sData[16],
                (long double *)&queryEntry->m_sData[56],
                (long double *)&queryEntry->m_sData[48],
                (unsigned __int8 *)&queryEntry->m_sData[20],
                &queryEntry->m_sData[28],
                (unsigned __int8 *)&queryEntry->m_sData[64]);
        queryEntry->m_byResult = v21;
        break;
      case 22:
        v207 = queryEntry->m_sData;
        v25 = db_disjoint_guild(*(_DWORD *)queryEntry->m_sData);
        queryEntry->m_byResult = v25;
        break;
      case 23:
        v208 = queryEntry->m_sData;
        v26 = db_char_set_alive(queryEntry->m_dwAccountSerial,
                queryEntry->m_sData[0],
                *(_DWORD *)&queryEntry->m_sData[4],
                &queryEntry->m_sData[8],
                queryEntry->m_sData[25],
                (_REGED *)&queryEntry->m_sData[26]);
        queryEntry->m_byResult = v26;
        break;
      case 24:
        v209 = queryEntry->m_sData;
        db_Insert_ChangeClass_AfterInitClass(*(_DWORD *)queryEntry->m_sData,
          queryEntry->m_sData[4],
          &queryEntry->m_sData[5],
          &queryEntry->m_sData[10],
          *(_DWORD *)&queryEntry->m_sData[16],
          queryEntry->m_sData[20],
          *(_WORD *)&queryEntry->m_sData[22],
          queryEntry->m_sData[24],
          queryEntry->m_sData[25],
          queryEntry->m_sData[26],
          queryEntry->m_sData[27],
          queryEntry->m_sData[28]);
        break;
      case 25:
        v210 = (_qry_case_sendwebracebosssms *)queryEntry->m_sData;
        v27 = db_sendwebracebosssms((_qry_case_sendwebracebosssms *)queryEntry->m_sData);
        queryEntry->m_byResult = v27;
        break;
      case 30:
        szData = queryEntry->m_sData;
        v28 = CGuildBattleController::Instance();
        v29 = v28->AddSchedule(szData);
        queryEntry->m_byResult = v29;
        break;
      case 31:
        v211 = queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        v30 = CGuildBattleController::Instance();
        if (!v30->UpdateWinLose(*v211, *((_DWORD *)v211 + 1), v211[8], *((_DWORD *)v211 + 3)) )
          queryEntry->m_byResult = 24;
        break;
      case 32:
        v212 = queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        v31 = CGuildBattleController::Instance();
        if (!v31->UpdateDraw(*v212, *((_DWORD *)v212 + 1), v212[8], *((_DWORD *)v212 + 3)) )
          queryEntry->m_byResult = 24;
        break;
      case 33:
        v213 = (unsigned __int8 *)queryEntry->m_sData;
        byOutData = (unsigned __int8 *)&queryEntry->m_sData[1];
        v32 = CGuildBattleController::Instance();
        if (!v32->UpdateRank(*v213, byOutData) )
          queryEntry->m_byResult = 24;
        break;
      case 34:
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->CreateGuildBattleRankTable(queryEntry->m_sData) )
          queryEntry->m_byResult = 24;
        break;
      case 35:
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->UpdateClearGuildBattleRank() )
          queryEntry->m_byResult = 24;
        break;
      case 36:
        v214 = (unsigned int *)queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        v33 = CGuildBattleController::Instance();
        if (!v33->UpdateClearRerservedDayInfo(*v214, v214[1], v214[2], v214[3]) )
          queryEntry->m_byResult = 24;
        break;
      case 37:
        v215 = queryEntry->m_sData;
        v34 = db_input_guild_money(*(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[12],
                *(_DWORD *)&queryEntry->m_sData[8],
                (long double *)&queryEntry->m_sData[32],
                (long double *)&queryEntry->m_sData[24],
                (unsigned __int8 *)&queryEntry->m_sData[16],
                "Scramble Cost");
        queryEntry->m_byResult = v34;
        break;
      case 38:
        v216 = queryEntry->m_sData;
        v35 = db_output_guild_money(*(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[12],
                *(_DWORD *)&queryEntry->m_sData[8],
                (long double *)&queryEntry->m_sData[32],
                (long double *)&queryEntry->m_sData[24],
                (unsigned __int8 *)&queryEntry->m_sData[16],
                "Scramble Cost",
                (unsigned __int8 *)&queryEntry->m_sData[56]);
        queryEntry->m_byResult = v35;
        break;
      case 39:
        v218 = queryEntry->m_sData;
        v37 = db_input_guild_money(*(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[12],
                *(_DWORD *)&queryEntry->m_sData[8],
                (long double *)&queryEntry->m_sData[32],
                (long double *)&queryEntry->m_sData[24],
                (unsigned __int8 *)&queryEntry->m_sData[16],
                "Scramble Cost");
        queryEntry->m_byResult = v37;
        break;
      case 40:
        queryEntry->m_byResult = 0;
        break;
      case 41:
        queryEntry->m_byResult = 0;
        v219 = queryEntry->m_sData;
        v259 = (unsigned __int8 *)&queryEntry->m_sData[8];
        v38 = CGuildBattleController::Instance();
        if (!v38->UpdateReservedGuildBattleSchedule(*((_DWORD *)v219 + 1), v259) )
          queryEntry->m_byResult = 24;
        break;
      case 43:
        queryEntry->m_byResult = 0;
        pLoadData = queryEntry->m_sData;
        v41 = CTotalGuildRankManager::Instance();
        if (!v41->Update(reinterpret_cast<_total_guild_rank_info *>(pLoadData)) )
          queryEntry->m_byResult = 24;
        break;
      case 44:
        queryEntry->m_byResult = 0;
        v262 = queryEntry->m_sData;
        v42 = CWeeklyGuildRankManager::Instance();
        if (!v42->UpdateTodayRank(reinterpret_cast<_pvppoint_guild_rank_info *>(v262)) )
          queryEntry->m_byResult = 24;
        break;
      case 45:
        queryEntry->m_byResult = 0;
        v263 = queryEntry->m_sData;
        v43 = CWeeklyGuildRankManager::Instance();
        if (!v43->UpdateWeeklyOwner(reinterpret_cast<_weeklyguildrank_owner_info *>(v263)) )
          queryEntry->m_byResult = 24;
        break;
      case 46:
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_ClearWeeklyPvpPointSum() )
          queryEntry->m_byResult = 24;
        break;
      case 47:
        v223 = queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_IncreaseWeeklyGuildGuildBattlePvpPointSum(
                *(_DWORD *)v223,
                *((long double *)v223 + 1)) )
          queryEntry->m_byResult = 24;
        break;
      case 49:
        v221 = (_qry_case_update_guildmaster *)queryEntry->m_sData;
        v40 = db_update_guildmaster((_qry_case_update_guildmaster *)queryEntry->m_sData);
        queryEntry->m_byResult = v40;
        break;
      case 50:
        v222 = queryEntry->m_sData;
        if (_db_Update_MacroData(*(_DWORD *)queryEntry->m_sData,
               (_AIOC_A_MACRODATA *)&queryEntry->m_sData[4],
               (_AIOC_A_MACRODATA *)&queryEntry->m_sData[2708]) )
        {
          v260 = 0;
        }
        else
        {
          v260 = 24;
        }
        queryEntry->m_byResult = v260;
        break;
      case 51:
        v44 = TRC_AutoTrade::_insert_info(queryEntry->m_sData);
        queryEntry->m_byResult = v44;
        break;
      case 52:
        v220 = queryEntry->m_sData;
        v39 = db_input_guild_money_atradetax(*(_DWORD *)&queryEntry->m_sData[8],
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[12],
                (long double *)&queryEntry->m_sData[16],
                (long double *)&queryEntry->m_sData[24],
                (unsigned __int8 *)&queryEntry->m_sData[32]);
        queryEntry->m_byResult = v39;
        break;
      case 53:
        pdata = queryEntry->m_sData;
        v45 = AutoMineMachineMng::Instance();
        v46 = v45->request_db_query(pdata);
        queryEntry->m_byResult = v46;
        break;
      case 56:
        v265 = queryEntry->m_sData;
        v47 = AutominePersonalMgr::instance();
        v48 = v47->request_query(v265);
        queryEntry->m_byResult = v48;
        break;
      case 57:
        pkInfo = (_guildbattle_totalrecord *)queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Select_GuildBattleRecord(pkInfo->dwGuildSerial, pkInfo) )
          queryEntry->m_byResult = 24;
        break;
      case 58:
        v225 = (_unmannedtrader_reserved_schedule_info *)queryEntry->m_sData;
        v226 = g_Main.m_pWorldDB->Select_UnmannedTraderReservedSchedule(
                 CUnmannedTraderEnvironmentValue::Unmanned_Trader_Max_Schdule_Cnt,
                 (_unmannedtrader_reserved_schedule_info *)queryEntry->m_sData);
        if (v226 != 2 && v226 )
          queryEntry->m_byResult = 24;
        break;
      case 59:
        pData = queryEntry->m_sData;
        v49 = CUnmannedTraderController::Instance();
        v50 = v49->UpdateItemState(pData);
        queryEntry->m_byResult = v50;
        break;
      case 60:
        v267 = queryEntry->m_sData;
        v51 = CUnmannedTraderController::Instance();
        v52 = v51->UpdateRegistItem(v267);
        queryEntry->m_byResult = v52;
        break;
      case 61:
        v268 = queryEntry->m_sData;
        v53 = CUnmannedTraderController::Instance();
        v54 = v53->UpdateCancelRegist(v268);
        queryEntry->m_byResult = v54;
        break;
      case 62:
        v269 = queryEntry->m_sData;
        v55 = CUnmannedTraderController::Instance();
        v56 = v55->UpdateTimeOutCancelRegist(v269);
        queryEntry->m_byResult = v56;
        break;
      case 63:
        v270 = queryEntry->m_sData;
        v57 = CUnmannedTraderController::Instance();
        v58 = v57->UpdateRePrice(v270);
        queryEntry->m_byResult = v58;
        break;
      case 64:
        v271 = queryEntry->m_sData;
        v59 = CUnmannedTraderController::Instance();
        v60 = v59->SelectBuy(v271);
        queryEntry->m_byResult = v60;
        break;
      case 65:
        v272 = queryEntry->m_sData;
        v61 = CUnmannedTraderController::Instance();
        v62 = v61->UpdateBuy(v272);
        queryEntry->m_byResult = v62;
        break;
      case 66:
        v273 = queryEntry->m_sData;
        v63 = CUnmannedTraderController::Instance();
        v64 = v63->UpdateBuyRollBack(v273);
        queryEntry->m_byResult = v64;
        break;
      case 67:
        v274 = queryEntry->m_sData;
        v65 = CUnmannedTraderController::Instance();
        v66 = v65->UpdateLazyClean(v274);
        queryEntry->m_byResult = v66;
        break;
      case 68:
        v275 = queryEntry->m_sData;
        v67 = CUnmannedTraderController::Instance();
        v68 = v67->UpdateBuyComplete(v275);
        queryEntry->m_byResult = v68;
        break;
      case 71:
        v227 = (_qry_case_guildroom_insert *)queryEntry->m_sData;
        v77 = _db_GuildRoom_Insert((_qry_case_guildroom_insert *)queryEntry->m_sData);
        queryEntry->m_byResult = v77;
        break;
      case 72:
        v228 = (_qry_case_guildroom_update *)queryEntry->m_sData;
        if (_db_GuildRoom_Update((_qry_case_guildroom_update *)queryEntry->m_sData) )
          v280 = 0;
        else
          v280 = 24;
        queryEntry->m_byResult = v280;
        break;
      case 74:
        pSheet = (_qry_case_gm_greetingmsg *)queryEntry->m_sData;
        v22 = db_GM_GreetingMsg((_qry_case_gm_greetingmsg *)queryEntry->m_sData);
        queryEntry->m_byResult = v22;
        break;
      case 75:
        v205 = (_qry_case_race_greetingmsg *)queryEntry->m_sData;
        v23 = db_RACE_GreetingMsg((_qry_case_race_greetingmsg *)queryEntry->m_sData);
        queryEntry->m_byResult = v23;
        break;
      case 76:
        v206 = (_qry_case_guild_greetingmsg *)queryEntry->m_sData;
        v24 = db_GUILD_GreetingMsg((_qry_case_guild_greetingmsg *)queryEntry->m_sData);
        queryEntry->m_byResult = v24;
        break;
      case 77:
        v281 = queryEntry->m_sData;
        v78 = CPostSystemManager::Instace();
        v79 = v78->UpdateRegist(v281);
        queryEntry->m_byResult = v79;
        break;
      case 78:
        v282 = queryEntry->m_sData;
        v80 = CPostSystemManager::Instace();
        v81 = v80->PostSend(v282);
        queryEntry->m_byResult = v81;
        break;
      case 79:
        PostStorage = db_Load_PostStorage(queryEntry->m_sData);
        queryEntry->m_byResult = PostStorage;
        break;
      case 80:
        ReturnPost = db_Load_ReturnPost(queryEntry->m_sData);
        queryEntry->m_byResult = ReturnPost;
        break;
      case 82:
        Content = db_Load_Content(queryEntry->m_sData);
        queryEntry->m_byResult = Content;
        break;
      case 83:
        inited = _db_init_classrefine_count();
        queryEntry->m_byResult = inited;
        break;
      case 84:
        queryEntry->m_byResult = 0;
        szStartTime = queryEntry->m_sData;
        if (!g_Main.m_pWorldDB->Insert_GuildBatlleResultLog(
                queryEntry->m_sData,
                &queryEntry->m_sData[17],
                *(_DWORD *)&queryEntry->m_sData[36],
                &queryEntry->m_sData[40],
                *(_DWORD *)&queryEntry->m_sData[60],
                &queryEntry->m_sData[64],
                *(_DWORD *)&queryEntry->m_sData[84],
                *(_DWORD *)&queryEntry->m_sData[88],
                *(_DWORD *)&queryEntry->m_sData[92],
                *(_DWORD *)&queryEntry->m_sData[96],
                *(_DWORD *)&queryEntry->m_sData[100],
                *(_DWORD *)&queryEntry->m_sData[104],
                *(_DWORD *)&queryEntry->m_sData[108],
                *(_DWORD *)&queryEntry->m_sData[112],
                queryEntry->m_sData[116],
                *(_DWORD *)&queryEntry->m_sData[120],
                &queryEntry->m_sData[124],
                *(_DWORD *)&queryEntry->m_sData[144],
                &queryEntry->m_sData[148],
                queryEntry->m_sData[165],
                *(_DWORD *)&queryEntry->m_sData[168],
                &queryEntry->m_sData[172]) )
          queryEntry->m_byResult = 24;
        break;
      case 85:
        v283 = queryEntry->m_sData;
        v86 = CPvpUserAndGuildRankingSystem::Instance();
        v87 = v86->UpdateRaceRankStep1(v283);
        queryEntry->m_byResult = v87;
        break;
      case 86:
        v284 = queryEntry->m_sData;
        v88 = CPvpUserAndGuildRankingSystem::Instance();
        v89 = v88->UpdateRaceRankStep2(v284);
        queryEntry->m_byResult = v89;
        break;
      case 87:
        v285 = queryEntry->m_sData;
        v90 = CPvpUserAndGuildRankingSystem::Instance();
        v91 = v90->UpdateRaceRankStep3(v285);
        queryEntry->m_byResult = v91;
        break;
      case 88:
        v286 = queryEntry->m_sData;
        v92 = CPvpUserAndGuildRankingSystem::Instance();
        v93 = v92->UpdateRaceRankStep4(v286);
        queryEntry->m_byResult = v93;
        break;
      case 89:
        v287 = queryEntry->m_sData;
        v94 = CPvpUserAndGuildRankingSystem::Instance();
        v95 = v94->UpdateRaceRankStep5(v287);
        queryEntry->m_byResult = v95;
        break;
      case 90:
        v288 = queryEntry->m_sData;
        v96 = CPvpUserAndGuildRankingSystem::Instance();
        v97 = v96->UpdateRaceRankStep6(v288);
        queryEntry->m_byResult = v97;
        break;
      case 91:
        v289 = queryEntry->m_sData;
        v98 = CPvpUserAndGuildRankingSystem::Instance();
        v99 = v98->UpdateRaceRankStep7(v289);
        queryEntry->m_byResult = v99;
        break;
      case 92:
        v290 = queryEntry->m_sData;
        v100 = CPvpUserAndGuildRankingSystem::Instance();
        v101 = v100->UpdateRaceRankStep8(v290);
        queryEntry->m_byResult = v101;
        break;
      case 93:
        v291 = queryEntry->m_sData;
        v102 = CPvpUserAndGuildRankingSystem::Instance();
        v103 = v102->UpdateRaceRankStep9(v291);
        queryEntry->m_byResult = v103;
        break;
      case 94:
        v292 = queryEntry->m_sData;
        v104 = CPvpUserAndGuildRankingSystem::Instance();
        v105 = v104->UpdateRaceRankStep10(v292);
        queryEntry->m_byResult = v105;
        break;
      case 95:
        v293 = queryEntry->m_sData;
        v106 = CPvpUserAndGuildRankingSystem::Instance();
        v107 = v106->UpdateRaceRankStep11(v293);
        queryEntry->m_byResult = v107;
        break;
      case 97:
        v294 = queryEntry->m_sData;
        v108 = CPvpUserAndGuildRankingSystem::Instance();
        v109 = v108->UpdateGuildRankStep1(v294);
        queryEntry->m_byResult = v109;
        break;
      case 98:
        v295 = queryEntry->m_sData;
        v110 = CPvpUserAndGuildRankingSystem::Instance();
        v111 = v110->UpdateGuildRankStep2(v295);
        queryEntry->m_byResult = v111;
        break;
      case 99:
        v296 = queryEntry->m_sData;
        v112 = CPvpUserAndGuildRankingSystem::Instance();
        v113 = v112->UpdateGuildRankStep3(v296);
        queryEntry->m_byResult = v113;
        break;
      case 100:
        v297 = queryEntry->m_sData;
        v114 = CPvpUserAndGuildRankingSystem::Instance();
        v115 = v114->UpdateGuildRankStep4(v297);
        queryEntry->m_byResult = v115;
        break;
      case 101:
        v298 = queryEntry->m_sData;
        v116 = CPvpUserAndGuildRankingSystem::Instance();
        v117 = v116->UpdateRankinGuildStep1(v298);
        queryEntry->m_byResult = v117;
        break;
      case 102:
        v299 = queryEntry->m_sData;
        v118 = CPvpUserAndGuildRankingSystem::Instance();
        v119 = v118->UpdateRankinGuildStep2(v299);
        queryEntry->m_byResult = v119;
        break;
      case 103:
        v300 = queryEntry->m_sData;
        v120 = CPvpUserAndGuildRankingSystem::Instance();
        v121 = v120->UpdateRankinGuildStep3(v300);
        queryEntry->m_byResult = v121;
        break;
      case 104:
        v301 = queryEntry->m_sData;
        v122 = CPvpUserAndGuildRankingSystem::Instance();
        v123 = v122->UpdateRankinGuildStep4(v301);
        queryEntry->m_byResult = v123;
        break;
      case 105:
        v302 = queryEntry->m_sData;
        v124 = CPvpUserAndGuildRankingSystem::Instance();
        v125 = v124->UpdateRankinGuildStep5(v302);
        queryEntry->m_byResult = v125;
        break;
      case 106:
        v303 = queryEntry->m_sData;
        v126 = CPvpUserAndGuildRankingSystem::Instance();
        v127 = v126->UpdateRankinGuildStep6(v303);
        queryEntry->m_byResult = v127;
        break;
      case 107:
        v304 = queryEntry->m_sData;
        v128 = CPvpUserAndGuildRankingSystem::Instance();
        v129 = v128->UpdateAndSelectGuildGrade(v304);
        queryEntry->m_byResult = v129;
        break;
      case 111:
        v276 = queryEntry->m_sData;
        v69 = CUnmannedTraderController::Instance();
        v70 = v69->UpdateLogInComplete(v276);
        queryEntry->m_byResult = v70;
        break;
      case 112:
        v130 = CItemStoreManager::Instance();
        v131 = v130->UpdateStoreLimitItem();
        queryEntry->m_byResult = v131;
        break;
      case 113:
        v305 = queryEntry->m_sData;
        v132 = CItemStoreManager::Instance();
        v133 = v132->UpdateDisableInstanceStore(v305);
        queryEntry->m_byResult = v133;
        break;
      case 114:
        v134 = CandidateMgr::Instance();
        v135 = v134->Update_RegistCandidate_2st();
        queryEntry->m_byResult = v135;
        break;
      case 115:
        v136 = CandidateMgr::Instance();
        v137 = v136->Update_Score();
        queryEntry->m_byResult = v137;
        break;
      case 116:
        v138 = CandidateMgr::Instance();
        v139 = v138->Update_ClassType();
        queryEntry->m_byResult = v139;
        break;
      case 117:
      case 125:
        v140 = CandidateMgr::Instance();
        v141 = v140->Insert_Candidate((char *)&queryEntry->m_bUse);
        queryEntry->m_byResult = v141;
        break;
      case 118:
        v217 = queryEntry->m_sData;
        v36 = db_output_guild_money(*(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[4],
                *(_DWORD *)&queryEntry->m_sData[12],
                *(_DWORD *)&queryEntry->m_sData[8],
                (long double *)&queryEntry->m_sData[32],
                (long double *)&queryEntry->m_sData[24],
                (unsigned __int8 *)&queryEntry->m_sData[16],
                "Scramble Cost",
                (unsigned __int8 *)&queryEntry->m_sData[56]);
        queryEntry->m_byResult = v36;
        break;
      case 119:
        v142 = PatriarchElectProcessor::Instance();
        v143 = v142->Insert_Elect();
        queryEntry->m_byResult = v143;
        break;
      case 120:
        v144 = PatriarchElectProcessor::Instance();
        v145 = v144->Update_Elect();
        queryEntry->m_byResult = v145;
        break;
      case 121:
        v146 = CandidateMgr::Instance();
        v147 = v146->Update_Refund();
        queryEntry->m_byResult = v147;
        break;
      case 122:
        queryEntry->m_byResult = 0;
        v148 = PatriarchElectProcessor::Instance();
        if (!v148->LoadElectState() )
          queryEntry->m_byResult = 24;
        break;
      case 123:
        v306 = queryEntry->m_sData;
        v149 = PatriarchElectProcessor::Instance();
        v150 = v149->Request_Refund(v306);
        queryEntry->m_byResult = v150;
        break;
      case 124:
        v307 = queryEntry->m_sData;
        v151 = PatriarchElectProcessor::Instance();
        v152 = v151->Insert_PatrirchItemChargeRefund(v307);
        queryEntry->m_byResult = v152;
        break;
      case 126:
        v308 = queryEntry->m_sData;
        v153 = CandidateMgr::Instance();
        v154 = v153->Update_DischargePatriarch(v308);
        queryEntry->m_byResult = v154;
        break;
      case 127:
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_Punishment(queryEntry->m_sData) )
          queryEntry->m_byResult = 24;
        break;
      case 128:
        v230 = queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        CharSerialByNameRace = g_Main.m_pWorldDB->Select_GetCharSerialByNameRace(
                                 v230 + 8,
                                 *v230,
                                 (unsigned int *)v230 + 1);
        if (CharSerialByNameRace != 2 && CharSerialByNameRace )
          queryEntry->m_byResult = 24;
        break;
      case 129:
        v309 = queryEntry->m_sData;
        v155 = CPostSystemManager::Instace();
        v156 = v155->PostReceiverCheck(v309);
        queryEntry->m_byResult = v156;
        break;
      case 130:
        PatriarchComm = _db_Load_PatriarchComm(queryEntry->m_sData);
        queryEntry->m_byResult = PatriarchComm;
        break;
      case 131:
        queryEntry->m_byResult = 0;
        v232 = queryEntry->m_sData;
        if (!g_Main.m_pWorldDB->Insert_PatriarchComm(
                *(_DWORD *)queryEntry->m_sData,
                *(_DWORD *)&queryEntry->m_sData[4],
                &queryEntry->m_sData[8]) )
          queryEntry->m_byResult = 24;
        break;
      case 134:
        v233 = (unsigned __int8 *)queryEntry->m_sData;
        v158 = CHonorGuild::Instance();
        v159 = v158->UpdateNextHonorGuild(*v233);
        queryEntry->m_byResult = v159;
        break;
      case 135:
        v234 = (unsigned __int8 *)queryEntry->m_sData;
        v160 = CHonorGuild::Instance();
        v161 = v160->UpdateChangeHonorGuild(*v234);
        queryEntry->m_byResult = v161;
        break;
      case 136:
        v235 = queryEntry->m_sData;
        v162 = db_input_guild_money_atradetax(*(_DWORD *)&queryEntry->m_sData[8],
                 *(_DWORD *)&queryEntry->m_sData[4],
                 *(_DWORD *)&queryEntry->m_sData[12],
                 (long double *)&queryEntry->m_sData[16],
                 (long double *)&queryEntry->m_sData[24],
                 (unsigned __int8 *)&queryEntry->m_sData[32]);
        queryEntry->m_byResult = v162;
        break;
      case 137:
        queryEntry->m_byResult = 0;
        pInfo = (_race_battle_log_info *)queryEntry->m_sData;
        if (!g_Main.m_pWorldDB->Insert_RaceBattleLog((_race_battle_log_info *)queryEntry->m_sData) )
          queryEntry->m_byResult = 24;
        break;
      case 138:
        v237 = (unsigned __int8 *)queryEntry->m_sData;
        v163 = CandidateMgr::Instance();
        v164 = v163->CheckDBValidCharacter(*v237);
        queryEntry->m_byResult = v164;
        break;
      case 139:
        v238 = (unsigned int *)queryEntry->m_sData;
        v165 = CandidateMgr::Instance();
        v166 = v165->Update_VoteTime(*v238);
        queryEntry->m_byResult = v166;
        break;
      case 140:
        v277 = queryEntry->m_sData;
        v71 = CUnmannedTraderController::Instance();
        v72 = v71->UpdateReRegist(v277);
        queryEntry->m_byResult = v72;
        break;
      case 141:
        v278 = queryEntry->m_sData;
        v73 = CUnmannedTraderController::Instance();
        v74 = v73->UpdateCheatRegistTime(v278);
        queryEntry->m_byResult = v74;
        break;
      case 142:
        v239 = (_qry_case_raceboss_accumulation_winrate *)queryEntry->m_sData;
        v167 = CRaceBossWinRate::Instance();
        BossAccmulationWinRate = v167->LoadBossAccmulationWinRate(v239);
        queryEntry->m_byResult = BossAccmulationWinRate;
        break;
      case 143:
        v240 = queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        if (!g_Main.m_pWorldDB->Update_UserGuildData(
                *(_DWORD *)v240,
                *((_DWORD *)v240 + 2),
                v240[12]) )
          queryEntry->m_byResult = 24;
        break;
      case 144:
        v241 = queryEntry->m_sData;
        Conn = g_Main.m_pWorldDB->Select_GuildMasterLastConn(
                 *(_DWORD *)queryEntry->m_sData,
                 *(_DWORD *)&queryEntry->m_sData[12],
                 (unsigned int *)&queryEntry->m_sData[16]);
        if (Conn == 2 || !Conn )
          queryEntry->m_byResult = 0;
        else
          queryEntry->m_byResult = 24;
        break;
      case 145:
        v243 = queryEntry->m_sData;
        if (g_Main.m_pWorldDB->Select_CharacterReName(
               &queryEntry->m_sData[12],
               (unsigned int *)&queryEntry->m_sData[4]) )
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      case 146:
        v244 = queryEntry->m_sData;
        if (g_Main.m_pWorldDB->Update_CharacterReName(&queryEntry->m_sData[12], *(_DWORD *)queryEntry->m_sData) )
          queryEntry->m_byResult = 0;
        else
          queryEntry->m_byResult = 24;
        break;
      case 147:
        v279 = queryEntry->m_sData;
        v75 = CUnmannedTraderController::Instance();
        v76 = v75->UpdateReRegistRollBack(v279);
        queryEntry->m_byResult = v76;
        break;
      case 149:
        byVoteEnable = (unsigned __int8 *)queryEntry->m_sData;
        if (g_Main.m_pWorldDB->Updatet_Account_Vote_Available(
               *(_DWORD *)&queryEntry->m_sData[4],
               (unsigned __int8 *)queryEntry->m_sData) )
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      case 150:
        v246 = queryEntry->m_sData;
        if (g_Main.m_pWorldDB->Update_Player_Vote_Info(
               *(_DWORD *)&queryEntry->m_sData[20],
               *(_DWORD *)queryEntry->m_sData,
               queryEntry->m_sData[4],
               queryEntry->m_sData[5],
               *(_DWORD *)&queryEntry->m_sData[8]) )
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      case 151:
        v247 = queryEntry->m_sData;
        if (g_Main.m_pWorldDB->UpdateVotedReset_Cheat(*(_DWORD *)&queryEntry->m_sData[4]) )
          queryEntry->m_byResult = 0;
        else
          queryEntry->m_byResult = 24;
        break;
      case 152:
        v248 = queryEntry->m_sData;
        if (g_Main.m_pWorldDB->UpdateServerResetToken(
               *(_DWORD *)queryEntry->m_sData,
               *(_WORD *)&queryEntry->m_sData[8],
               *(_DWORD *)&queryEntry->m_sData[4]) )
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      case 153:
        v252 = queryEntry->m_sData;
        LogoutTime = g_Main.m_pWorldDB->Select_Player_Last_LogoutTime(
                       *(_DWORD *)&queryEntry->m_sData[4],
                       (unsigned int *)&queryEntry->m_sData[8]);
        if (LogoutTime == 1 )
          queryEntry->m_byResult = 24;
        else
          queryEntry->m_byResult = 0;
        if (LogoutTime == 2 )
        {
          if (m_pWorldDB->Insert_PlayerTimeLimitInfo(*((_DWORD *)v252 + 1)) )
          {
            if (m_pWorldDB->Select_Player_Last_LogoutTime(
                   *((_DWORD *)v252 + 1),
                   (unsigned int *)v252 + 2) )
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
      case 155:
        v251 = queryEntry->m_sData;
        if (g_Main.m_pWorldDB->Update_Player_TimeLimit_Info(
               *(_DWORD *)&queryEntry->m_sData[4],
               *(_DWORD *)&queryEntry->m_sData[12],
               queryEntry->m_sData[16]) )
        {
          queryEntry->m_byResult = 0;
        }
        else
        {
          queryEntry->m_byResult = 24;
        }
        break;
      case 157:
        pDbLimitedSale = (qry_case_cash_limsale *)queryEntry->m_sData;
        Cash_LimSale = _db_Load_Cash_LimSale((qry_case_cash_limsale *)queryEntry->m_sData);
        queryEntry->m_byResult = Cash_LimSale;
        if (!queryEntry->m_byResult )
        {
          v170 = CashItemRemoteStore::Instance();
          v170->Set_FROMDB_LimitedSale_Event(&pDbLimitedSale->NewSale);
        }
        break;
      case 158:
        pNewData = (_db_cash_limited_sale *)queryEntry->m_sData;
        v171 = _db_Update_Cash_LimSale((_db_cash_limited_sale *)queryEntry->m_sData,
                 (_db_cash_limited_sale *)&queryEntry->m_sData[164]);
        queryEntry->m_byResult = v171;
        break;
      case 161:
        v172 = _db_Update_Set_Limit_Run();
        queryEntry->m_byResult = v172;
        break;
      case 165:
        v254 = queryEntry->m_sData;
        v173 = _db_Update_GoldBoxItem(*(_DWORD *)queryEntry->m_sData,
                 (_db_golden_box_item *)&queryEntry->m_sData[4],
                 (_db_golden_box_item *)&queryEntry->m_sData[1628]);
        queryEntry->m_byResult = v173;
        break;
      case 171:
        v174 = _db_Select_RegeAvator_For_Lobby_Logout(queryEntry->m_sData);
        queryEntry->m_byResult = v174;
        break;
      case 172:
        v255 = queryEntry->m_sData;
        queryEntry->m_byResult = 0;
        if (!m_pWorldDB->Insert_OreReset_Log(
                *v255,
                *((_DWORD *)v255 + 5),
                *((_DWORD *)v255 + 6),
                *((_DWORD *)v255 + 7)) )
          queryEntry->m_byResult = 24;
        break;
      case 175:
        v175 = _db_Update_Data_For_Post_Send(queryEntry->m_sData);
        queryEntry->m_byResult = v175;
        break;
      case 176:
        v176 = _db_Update_Data_For_Trade(queryEntry->m_sData);
        queryEntry->m_byResult = v176;
        break;
      default:
        break;
    }
    if (queryEntry->m_byResult && queryEntry->m_byResult != 25 && queryEntry->m_byResult != 49 )
    {
      queryResult = queryEntry->m_byResult;
      v178 = queryEntry->m_byQryCase;
      p_m_logDQS = &m_logDQS;
      m_logDQS.Write(
        "DQSThread.. DB fail.. account: %d, case: %d, ret: %d",
        queryEntry->m_dwAccountSerial,
        v178,
        queryResult);
    }
    queryEntry->m_bLoad = 1;
    m_listDQSDataComplete.PushNode_Back(queryIndex);
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
