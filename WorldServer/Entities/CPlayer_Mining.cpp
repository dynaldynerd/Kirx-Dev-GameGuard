#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CBsp.h"
#include "CObjectList.h"
#include "CMonster.h"
#include "pnt_rect.h"
#include "CMainThread.h"
#include "CItemStore.h"
#include "CNetworkEX.h"
#include "StoreList_fld.h"
#include "CActionPointSystemMgr.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "CGuild.h"
#include "CUserDB.h"
#include "CNetProcess.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CUnmannedTraderTaxRateManager.h"
#include "CNationSettingManager.h"
#include "CNationSettingData.h"
#include "GlobalObjects.h"
#include "CGuildBattleController.h"
#include "CGuildMasterEffect.h"
#include "CGuildRoomSystem.h"
#include "CHonorGuild.h"
#include "CHolyStoneSystem.h"
#include "CPotionMgr.h"
#include "guild_honor_set_request_clzo.h"
#include "CMgrGuildHistory.h"
#include "CMgrAvatorItemHistory.h"
#include "CPostSystemManager.h"
#include "CPartyPlayer.h"
#include "LendItemMng.h"
#include "CUnmannedTraderController.h"
#include "cStaticMember_Player.h"
#include "CTransportShip.h"
#include "CLogFile.h"
#include "CItemUpgradeTable.h"
#include "CMoneySupplyMgr.h"
#include "COreAmountMgr.h"
#include "CGoldenBoxItemMgr.h"
#include "COreCuttingTable.h"
#include "CDarkHole.h"
#include "CDarkHoleChannel.h"
#include "CDarkHoleDungeonQuest.h"
#include "CDarkHoleDungeonQuestSetup.h"
#include "TicketItem_fld.h"
#include "darkhole_create_setdata.h"
#include "ENTER_DUNGEON_NEW_POS.h"
#include "QuestHappenEvent_fld.h"
#include "announ_message_receipt_udp.h"
#include "equip_up_item_lv_limit_zocl.h"
#include "sf_delay_download_result_zocl.h"
#include "darkhole_ask_reenter_inform_zocl.h"
#include "darkhole_answer_reenter_result_zocl.h"
#include "darkhole_clear_out_result_zocl.h"
#include "darkhole_enter_result_zocl.h"
#include "darkhole_giveup_out_result_zocl.h"
#include "darkhole_open_result_zocl.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "move_to_own_stonemap_inform_zocl.h"
#include "move_to_own_stonemap_result_zocl.h"
#include "pt_result_change_tax_rate_zocl.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "economy_history_data.h"
#include "DqsDbStructs.h"
#include "EconomySystemFunctions.h"
#include "TimeItem.h"
#include "trans_ship_renew_ticket_result_zocl.h"
#include "trans_gm_msg_inform_zocl.h"
#include "trunk_change_passwd_result_zocl.h"
#include "trunk_download_result_zocl.h"
#include "trunk_est_result_zocl.h"
#include "trunk_extend_result_zocl.h"
#include "trunk_hint_answer_result_zocl.h"
#include "trunk_io_money_result_zocl.h"
#include "trunk_io_result_zocl.h"
#include "trunk_potionsocket_division_result_zocl.h"
#include "trunk_pw_hint_index_result_zocl.h"
#include "trunk_res_division_result_zocl.h"
#include "guildroom_rent_request_clzo.h"
#include "guildroom_enter_request_clzo.h"
#include "guildroom_out_request_clzo.h"
#include "GuardTowerItem_fld.h"
#include "UnitKeyItem_fld.h"
#include "TrapItem_fld.h"
#include "UnitFrame_fld.h"
#include "UnitPart_fld.h"
#include "UnitBullet_fld.h"
#include "BulletItem_fld.h"
#include "ItemCombine_exp_fld.h"
#include "combine_ex_item_result_zocl.h"
#include "combine_ex_item_accept_request_clzo.h"
#include "combine_ex_item_accept_result_zocl.h"
#include "qry_case_disjointguild.h"
#include "WorldServerUtil.h"
#include "NetCheckPackets.h"
#include "GlobalObjectDefs.h"

#include <ctime>
#include <mmsystem.h>
#include <cstdlib>
#include <cmath>
#include <vector>

namespace
{

int GetResDummyDelayForMining(const _res_dummy &resDummy, int sectorIndex, bool isPcBang)
{
  if (sectorIndex < 0 || sectorIndex >= 3)
  {
    return 0;
  }

  const int gradeIndex = isPcBang ? 1 : 0;
  return static_cast<int>(resDummy.m_dwDelay[sectorIndex][gradeIndex]);
}

}

void CPlayer::SendMsg_MineCompleteResult(
  char byErrCode,
  unsigned __int8 byNewOreIndex,
  unsigned __int16 dwOreSerial,
  unsigned __int8 byOreDur,
  unsigned __int16 dwBatteryLeftDurPoint)
{
  char payload[9]{};
  payload[0] = byErrCode;
  *reinterpret_cast<__int16 *>(payload + 1) = static_cast<__int16>(this->m_Param.m_dbEquip.m_pStorageList[6].m_dwDur);
  *reinterpret_cast<unsigned __int16 *>(payload + 3) = dwBatteryLeftDurPoint;
  payload[5] = static_cast<char>(byNewOreIndex);
  *reinterpret_cast<unsigned __int16 *>(payload + 6) = dwOreSerial;
  payload[8] = static_cast<char>(byOreDur);

  unsigned __int8 type[2] = {14, 6};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 9u);
}

void CPlayer::SendMsg_MineStartResult(unsigned __int8 resultCode)
{
  char payload = static_cast<char>(resultCode);
  unsigned __int8 type[2] = {14, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, &payload, 1u);
}

void CPlayer::SendMsg_OreIntoBagResult(char resultCode, unsigned __int16 newSerial, unsigned __int8 lendType, unsigned int lendTime)
{
  char payload[8] = {};
  payload[0] = resultCode;
  *reinterpret_cast<unsigned __int16 *>(&payload[1]) = newSerial;
  payload[3] = static_cast<char>(lendType);
  *reinterpret_cast<unsigned int *>(&payload[4]) = lendTime;

  unsigned __int8 type[2] = {14, 12};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, 8u);
}

void CPlayer::SendMsg_OreCuttingResult(unsigned __int8 resultCode, unsigned __int8 leftOreCount, unsigned int consumedDalant)
{
  char payload[11 + 100 * 3] = {};
  payload[0] = static_cast<char>(resultCode);

  if (!resultCode)
  {
    payload[1] = static_cast<char>(leftOreCount);
    *reinterpret_cast<unsigned int *>(&payload[2]) = this->m_Param.GetDalant();
    *reinterpret_cast<unsigned int *>(&payload[6]) = consumedDalant;

    unsigned __int8 resultCount = 0;
    const int maxResKind = GetMaxResKind();
    for (int resIndex = 0; resIndex < maxResKind && resultCount < 100; ++resIndex)
    {
      const unsigned __int16 addedAmount = this->m_Param.m_wCuttingResBuffer[resIndex];
      if (!addedAmount)
      {
        continue;
      }

      const int baseOffset = 11 + resultCount * 3;
      *reinterpret_cast<unsigned __int16 *>(&payload[baseOffset]) = static_cast<unsigned __int16>(resIndex);
      payload[baseOffset + 2] = static_cast<char>(addedAmount);
      ++resultCount;
    }
    payload[10] = static_cast<char>(resultCount);
  }

  const unsigned __int8 resultCount = static_cast<unsigned __int8>(payload[10]);
  const unsigned __int16 payloadLength = static_cast<unsigned __int16>(11 + 3 * resultCount);
  unsigned __int8 type[2] = {14, 10};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, payload, payloadLength);
}

void CPlayer::pc_MineCancle()
{
  if (this->m_bMineMode)
  {
    this->m_bMineMode = false;
    this->m_dwMineNextTime = static_cast<unsigned int>(-1);
    this->SendMsg_MineCancle();
  }
}

void CPlayer::pc_MineComplete()
{
  unsigned __int8 resultCode = 0;
  unsigned __int8 targetSlot = 0xFF;
  _STORAGE_LIST::_db_con *batteryItem = nullptr;

  if (!COreAmountMgr::Instance()->IsOreRemain())
  {
    resultCode = 27;
  }
  else
  {
    batteryItem = m_Param.m_dbInven.GetPtrFromSerial(m_wBatterySerialTmp);
    if (!batteryItem)
    {
      resultCode = 8;
    }
    else if (batteryItem->m_bLock)
    {
      resultCode = 12;
    }
    else
    {
      bool isGoldenBoxItem = false;
      unsigned __int8 selectedGoldenBoxIndex = 0;
      unsigned __int8 targetOreIndex = 0;

      if (CGoldenBoxItemMgr::Instance()->Get_Event_Status() == 2)
      {
        for (unsigned int loopIndex = 0; ; ++loopIndex)
        {
          const unsigned __int8 loopCount = CGoldenBoxItemMgr::Instance()->GetLoopCount();
          if (loopIndex >= static_cast<unsigned int>(loopCount))
          {
            break;
          }

          const unsigned __int16 oreIndex = CGoldenBoxItemMgr::Instance()->GetGoldBoxItemIndex(
            static_cast<unsigned __int16>(loopIndex));
          _base_fld *oreRecord = g_Main.m_tblItemData[17].GetRecord(oreIndex);
          if (oreRecord)
          {
            const unsigned int rate = *reinterpret_cast<unsigned int *>(&oreRecord[3].m_strCode[4]);
            if (rate)
            {
              const int randomValue = rand();
              const int randomHigh = randomValue << 16;
              const unsigned int lottery = static_cast<unsigned int>((rand() + randomHigh) % 0x1770 + 1);
              if (rate <= lottery
                  && CGoldenBoxItemMgr::Instance()->Get_Box_Count(static_cast<unsigned __int8>(loopIndex)))
              {
                targetOreIndex = static_cast<unsigned __int8>(
                  CGoldenBoxItemMgr::Instance()->GetGoldBoxItemIndex(static_cast<unsigned __int16>(loopIndex)));
                isGoldenBoxItem = true;
                selectedGoldenBoxIndex = static_cast<unsigned __int8>(loopIndex);
                break;
              }
            }
          }
        }
      }

      if (!isGoldenBoxItem)
      {
        targetOreIndex = static_cast<unsigned __int8>(m_bySelectOreIndex + rand() % 3);
      }

      const int targetRecordIndex = targetOreIndex;
      const int recordCount = g_Main.m_tblItemData[17].GetRecordNum();
      if (targetRecordIndex >= recordCount)
      {
        resultCode = 9;
      }
      else
      {
        for (int slotIndex = 0; slotIndex < m_Param.GetUseSlot(); ++slotIndex)
        {
          _STORAGE_LIST::_db_con *item = &m_Param.m_dbInven.m_pStorageList[slotIndex];
          if (item->m_bLoad
              && item->m_byTableCode == 17
              && item->m_wItemIndex == targetOreIndex
              && item->m_dwDur < 0x63
              && !item->m_bLock)
          {
            targetSlot = static_cast<unsigned __int8>(slotIndex);
            break;
          }
        }

        if (targetSlot == 0xFF && m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
        {
          resultCode = 10;
        }
        else if (m_zMinePos[0] != static_cast<__int16>(static_cast<int>(m_fCurPos[0]))
                 || m_zMinePos[1] != static_cast<__int16>(static_cast<int>(m_fCurPos[2])))
        {
          resultCode = 13;
        }
      }

      if (!resultCode)
      {
        unsigned __int8 oreDur = 0;
        _STORAGE_LIST::_db_con minedItem{};
        unsigned __int16 oreSerial = 0;

        if (targetSlot == 0xFF)
        {
          _STORAGE_LIST::_db_con newItem{};
          newItem.m_byTableCode = 17;
          newItem.m_dwDur = 1;
          newItem.m_wItemIndex = targetOreIndex;
          newItem.m_dwLv = 0xFFFFFFF;
          newItem.m_wSerial = m_Param.GetNewItemSerial();

          if (!Emb_AddStorage(0, &newItem, false, true))
          {
            s_MgrItemHistory.add_storage_fail(
              m_ObjID.m_wIndex,
              &newItem,
              "CPlayer::pc_MineComplete() - Emb_AddStorage() Fail",
              m_szItemHistoryFileName);
          }

          memcpy_0(&minedItem, &newItem, sizeof(minedItem));
          minedItem.m_dwDur = 1;
          oreSerial = newItem.m_wSerial;
          oreDur = 1;
        }
        else
        {
          _STORAGE_LIST::_db_con *inventoryItem = &m_Param.m_dbInven.m_pStorageList[targetSlot];
          oreDur = static_cast<unsigned __int8>(Emb_AlterDurPoint(0, inventoryItem->m_byStorageIndex, 1, false, false));
          oreSerial = inventoryItem->m_wSerial;
          memset_0(&minedItem, 0, sizeof(minedItem));
          memcpy_0(&minedItem, inventoryItem, sizeof(minedItem));
        }

        const unsigned __int16 batteryLeftDur = static_cast<unsigned __int16>(
          Emb_AlterDurPoint(0, batteryItem->m_byStorageIndex, -static_cast<int>(m_byDelaySec), false, false));
        _base_fld *oreRecord = g_Main.m_tblItemData[17].GetRecord(targetOreIndex);
        Emb_CheckActForQuest(12, oreRecord->m_strCode, 1u, false);
        SendMsg_MineCompleteResult(resultCode, targetOreIndex, oreSerial, oreDur, batteryLeftDur);

        if (isGoldenBoxItem && !resultCode)
        {
          if (CGoldenBoxItemMgr::Instance()->Get_Event_Status() == 2)
          {
            CGoldenBoxItemMgr::Instance()->Set_Box_Count(selectedGoldenBoxIndex);
            minedItem.m_dwDur = 1;
            SendMsg_Notify_Get_Golden_Box(4u, m_Param.GetCharSerial(), m_Param.GetCharNameA(), &minedItem, false);
          }
        }

        eAddMineOre(m_Param.GetRaceCode(), oreRecord[3].m_strCode[0], 1);
        COreAmountMgr::Instance()->DecreaseOre(1u);
      }
    }
  }

  if (resultCode)
  {
    SendMsg_MineCompleteResult(resultCode, 0xFFu, 0xFFFFu, 0xFFu, 0xFFFFu);
  }

  m_dwMineNextTime = static_cast<unsigned int>(-1);
  m_bMineMode = false;
}

void CPlayer::SendMsg_CuttingCompleteResult(unsigned __int8 byRet)
{
  char msg[5]{};
  const unsigned int gold = this->m_Param.GetGold();
  std::memcpy(msg, &gold, sizeof(gold));
  msg[4] = static_cast<char>(byRet);

  unsigned __int8 type[2] = {14, 14};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 5u);
}

void CPlayer::pc_CuttingComplete(unsigned __int8 byNpcRace)
{
unsigned __int64 addGold = 0;
  const unsigned int texRate = 10000 - eGetTexRate(this->m_Param.GetRaceCode());
  bool hasResource = false;

  for (int resourceIndex = 0; resourceIndex < GetMaxResKind(); ++resourceIndex)
  {
    if (this->m_Param.m_wCuttingResBuffer[resourceIndex])
    {
      _base_fld *record = g_Main.m_tblItemData[18].GetRecord(resourceIndex);
      const int unitPrice = *reinterpret_cast<int *>(&record[5].m_strCode[12]);
      addGold += static_cast<unsigned __int64>(this->m_Param.m_wCuttingResBuffer[resourceIndex]) * unitPrice;
      hasResource = true;
    }
  }

  addGold = addGold * texRate / 10000u;

  if (hasResource)
  {
    if (!CanAddMoneyForMaxLimGold(addGold, this->m_Param.GetGold()))
    {
      this->SendMsg_CuttingCompleteResult(1u);
      return;
    }

    this->AddGold(static_cast<int>(addGold), true);
    CPlayer::s_MgrItemHistory.cut_clear_item(
      this->m_ObjID.m_wIndex,
      this->m_Param.m_wCuttingResBuffer,
      static_cast<unsigned int>(addGold),
      this->m_Param.GetGold(),
      this->m_szItemHistoryFileName);
  }

  this->m_Param.InitResBuffer();

  if (!this->m_byUserDgr)
  {
    eAddGold(this->m_Param.GetRaceCode(), static_cast<int>(addGold));
  }

  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_CuttingEmpty();
  }

  this->SendMsg_CuttingCompleteResult(0);
}

void CPlayer::pc_MineStart(
  unsigned __int8 byMineIndex,
  unsigned __int8 byOreIndex,
  unsigned __int16 wBatterySerial)
{
  unsigned __int8 resultCode = 0;
  _base_fld *oreRecord = g_Main.m_tblItemData[17].GetRecord(byOreIndex);
  _res_dummy *resDummy = nullptr;
  int resDummySector = -1;
  _STORAGE_LIST::_db_con *toolItem = nullptr;
  _STORAGE_LIST::_db_con *batteryItem = nullptr;

  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus(m_id.wIndex) == 99)
  {
    resultCode = 33;
  }
  else if (m_Param.m_pAPM && m_Param.m_pAPM->is_installed())
  {
    resultCode = 25;
  }
  else if (m_dwMineNextTime != static_cast<unsigned int>(-1))
  {
    resultCode = 7;
  }
  else if (!oreRecord || *reinterpret_cast<unsigned int *>(&oreRecord[3].m_strCode[0]) > 2u)
  {
    resultCode = 9;
  }
  else if (byMineIndex >= m_pCurMap->m_nResDumNum)
  {
    resultCode = 1;
  }
  else
  {
    resDummy = &m_pCurMap->m_pResDummy[byMineIndex];
    if (!resDummy || resDummy->m_byQualityGrade == 2)
    {
      resultCode = 28;
    }
    else if (resDummy->m_byQualityGrade == 2 || IsMiningByMinigTicket())
    {
      resDummySector = m_pCurMap->GetResDummySector(byMineIndex, m_fCurPos);
      if (resDummySector == -1)
      {
        resultCode = 2;
      }
      else
      {
        toolItem = &m_Param.m_dbEquip.m_pStorageList[6];
        if (!toolItem->m_bLoad)
        {
          resultCode = 4;
        }
        else if (toolItem->m_bLock)
        {
          resultCode = 12;
        }
        else
        {
          _base_fld *toolRecord = g_Main.m_tblItemData[6].GetRecord(toolItem->m_wItemIndex);
          if (!toolRecord || *reinterpret_cast<int *>(&toolRecord[6].m_strCode[8]) != 10)
          {
            resultCode = 5;
          }
          else
          {
            batteryItem = m_Param.m_dbInven.GetPtrFromSerial(wBatterySerial);
            if (!batteryItem)
            {
              resultCode = 8;
            }
            else if (batteryItem->m_byTableCode != 16)
            {
              resultCode = 8;
            }
            else if (batteryItem->m_wItemIndex >= 6u && batteryItem->m_wItemIndex <= 9u)
            {
              resultCode = 8;
            }
            else if (batteryItem->m_bLock)
            {
              resultCode = 12;
            }
            else if (!COreAmountMgr::Instance()->IsOreRemain())
            {
              resultCode = 27;
            }
          }
        }
      }
    }
    else
    {
      resultCode = 26;
    }
  }

  if (!resultCode)
  {
    m_bMineMode = true;

    const bool isPcBang = m_pUserDB->m_BillingInfo.IsPcBangType();
    int delayMs = GetResDummyDelayForMining(m_pCurMap->m_pResDummy[byMineIndex], resDummySector, isPcBang);

    const float mineSpeedBuff = m_EP.GetEff_Have(7);
    if (mineSpeedBuff > 1.0f)
    {
      delayMs = static_cast<int>(static_cast<float>(delayMs) / mineSpeedBuff);
    }

    const float mineSpeedRate = m_EP.GetEff_Rate(37);
    if (mineSpeedRate > 1.0f)
    {
      delayMs = static_cast<int>(static_cast<float>(delayMs) / mineSpeedRate);
    }

    int penaltyScale = 0;
    const double penalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(m_id.wIndex);
    if (penalty != 0.0)
    {
      penaltyScale = static_cast<int>(1.0 / penalty);
    }
    delayMs *= penaltyScale;

    m_dwMineNextTime = static_cast<unsigned int>(delayMs) + GetLoopTime();
    m_byDelaySec = static_cast<unsigned __int8>(delayMs / 1000);
    m_wBatterySerialTmp = wBatterySerial;
    m_bySelectOreIndex = byOreIndex;
    m_zMinePos[0] = static_cast<int>(m_fCurPos[0]);
    m_zMinePos[1] = static_cast<int>(m_fCurPos[2]);

    if (m_bMove)
    {
      const bool outExtraStopPos = IsOutExtraStopPos(m_fCurPos);
      SendMsg_Stop(outExtraStopPos);
      Stop();
    }
    BreakStealth();
  }

  this->SendMsg_MineStartResult(resultCode);
}

void CPlayer::pc_OreCutting(unsigned __int16 wOreSerial, unsigned __int8 byProcessNum)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *oreItem = nullptr;
  _base_fld *oreRecord = nullptr;
  unsigned int leftOreCount = 0;
  unsigned int consumedDalant = 0;

  const int raceCode = m_Param.GetRaceCode();
  const unsigned int texRate = eGetTexRate(raceCode) + 10000;

  bool hasBufferedRes = false;
  const int maxResKind = GetMaxResKind();
  for (int index = 0; index < maxResKind; ++index)
  {
    if (m_Param.m_wCuttingResBuffer[index])
    {
      hasBufferedRes = true;
      break;
    }
  }

  if (hasBufferedRes)
  {
    resultCode = 4;
  }
  else
  {
    oreItem = m_Param.m_dbInven.GetPtrFromSerial(wOreSerial);
    if (!oreItem)
    {
      resultCode = 1;
    }
    else if (oreItem->m_byTableCode != 17)
    {
      resultCode = 1;
    }
    else if (oreItem->m_bLock)
    {
      resultCode = 11;
    }
    else
    {
      oreRecord = g_Main.m_tblItemData[17].GetRecord(oreItem->m_wItemIndex);
      if (!oreRecord)
      {
        resultCode = 1;
      }
      else if (byProcessNum > oreItem->m_dwDur)
      {
        resultCode = 2;
      }
      else
      {
        const int baseDalant = *reinterpret_cast<int *>(&oreRecord[4].m_strCode[4]);
        const float managerScale = static_cast<float>(eGetMgrValue());
        const float unitDalant = static_cast<float>(baseDalant)
                               + static_cast<float>(baseDalant) / 1000.0f * managerScale;
        unsigned long long totalDalant = static_cast<unsigned long long>(
          static_cast<int>(unitDalant * static_cast<float>(byProcessNum)));
        totalDalant = totalDalant * texRate / 10000u;
        consumedDalant = static_cast<unsigned int>(totalDalant);
        if (totalDalant > m_Param.GetDalant())
        {
          resultCode = 3;
        }
      }
    }
  }

  if (!resultCode)
  {
    const int minCutCount = static_cast<int>(oreRecord[4].m_dwIndex);
    const int maxCutCount = *reinterpret_cast<int *>(&oreRecord[4].m_strCode[0]);
    for (int processIndex = 0; processIndex < byProcessNum; ++processIndex)
    {
      int cutCount = minCutCount;
      if (maxCutCount >= minCutCount)
      {
        cutCount += rand() % (maxCutCount - minCutCount + 1);
      }

      for (int cutIndex = 0; cutIndex < cutCount; ++cutIndex)
      {
        const unsigned int rate = static_cast<unsigned int>(rand() + (rand() << 16));
        const unsigned int resIndex = g_Main.m_tblOreCutting.GetOreIndexFromRate(
          oreRecord->m_dwIndex,
          rate);
        if (resIndex != static_cast<unsigned int>(-1)
            && resIndex < static_cast<unsigned int>(maxResKind)
            && m_Param.m_wCuttingResBuffer[resIndex] < 0xFFu)
        {
          ++m_Param.m_wCuttingResBuffer[resIndex];
        }
      }
    }

    leftOreCount = Emb_AlterDurPoint(0, oreItem->m_byStorageIndex, -byProcessNum, false, false);
    SubDalant(consumedDalant);

    if (!m_byUserDgr)
    {
      const int managerDivisor = eGetMgrValue() + 1000;
      if (managerDivisor > 0)
      {
        const int dalantDrain = static_cast<int>(consumedDalant / static_cast<unsigned int>(managerDivisor));
        if (dalantDrain > 0)
        {
          eAddDalant(m_Param.GetRaceCode(), -dalantDrain);
        }
      }
    }

    const int level = m_Param.GetLevel();
    if (level == 30 || level == 40 || level == 50 || level == 60)
    {
      CMoneySupplyMgr::Instance()->UpdateFeeMoneyData(m_Param.GetRaceCode(), level, consumedDalant);
    }

    if (m_pUserDB && CActionPointSystemMgr::Instance()->GetEventStatus(0) == 2)
    {
      const unsigned int currentPoint = m_pUserDB->GetActPoint(0);
      const unsigned int pointPerCut = *reinterpret_cast<unsigned int *>(&oreRecord[3].m_strCode[36]);
      const unsigned int nextPoint = currentPoint + byProcessNum * pointPerCut;
      m_pUserDB->Update_User_Action_Point(0, nextPoint);
      SendMsg_Alter_Action_Point(0, nextPoint);
    }
  }

  this->SendMsg_OreCuttingResult(resultCode, static_cast<unsigned __int8>(leftOreCount), consumedDalant);
}

void CPlayer::pc_OreIntoBag(
  unsigned __int16 wResIndex,
  unsigned __int16 wSerial,
  unsigned __int8 byAddAmount)
{
  unsigned __int8 resultCode = 0;
  _STORAGE_LIST::_db_con *targetItem = nullptr;
  unsigned __int16 newSerial = wSerial;
  _STORAGE_LIST::_db_con newItem;
  const _TimeItem_fld *timeRecord = TimeItem::FindTimeRec(18, wResIndex);

  if (byAddAmount > m_Param.m_wCuttingResBuffer[wResIndex])
  {
    resultCode = 2;
  }
  else if (wSerial == 0xFFFF && m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
  {
    resultCode = 4;
  }
  else if (wSerial != 0xFFFF)
  {
    targetItem = m_Param.m_dbInven.GetPtrFromSerial(wSerial);
    if (!targetItem)
    {
      resultCode = 5;
    }
    else if (targetItem->m_byTableCode != 18 || targetItem->m_wItemIndex != wResIndex)
    {
      resultCode = 7;
    }
    else if (targetItem->m_bLock)
    {
      resultCode = 11;
    }
    else if (targetItem->m_dwDur + byAddAmount > 99)
    {
      resultCode = 6;
    }
  }

  if (!resultCode)
  {
    m_Param.m_wCuttingResBuffer[wResIndex] -= byAddAmount;
    if (!targetItem || timeRecord)
    {
      newItem = _STORAGE_LIST::_db_con();
      newItem.m_byTableCode = 18;
      newItem.m_wItemIndex = wResIndex;
      newItem.m_dwDur = byAddAmount;
      newItem.m_dwLv = 0xFFFFFFF;
      newItem.m_wSerial = m_Param.GetNewItemSerial();

      if (timeRecord && timeRecord->m_nCheckType)
      {
        __time32_t currentTime = 0;
        _time32(&currentTime);
        newItem.m_byCsMethod = static_cast<unsigned __int8>(timeRecord->m_nCheckType);
        newItem.m_dwT = timeRecord->m_nUseTime + currentTime;
        newItem.m_dwLendRegdTime = currentTime;
      }

      if (!Emb_AddStorage(0, &newItem, false, false))
      {
        this->SendMsg_OreIntoBagResult(static_cast<char>(0xFF), newSerial, 0, 0);
        return;
      }
      newSerial = newItem.m_wSerial;
    }
    else
    {
      Emb_AlterDurPoint(0, targetItem->m_byStorageIndex, byAddAmount, false, false);
    }

    _base_fld *resRecord = g_Main.m_tblItemData[18].GetRecord(wResIndex);
    if (resRecord)
    {
      Emb_CheckActForQuest(13, resRecord->m_strCode, byAddAmount, false);
    }
  }

  this->SendMsg_OreIntoBagResult(static_cast<char>(resultCode), newSerial, newItem.m_byCsMethod, newItem.m_dwT);
}

