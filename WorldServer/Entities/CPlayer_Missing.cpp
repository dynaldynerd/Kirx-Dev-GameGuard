#include "pch.h"

#include "CPlayer.h"
#include "CQuestMgr.h"
#include "CRecordData.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "CUserDB.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"
#include "CGuildBattleController.h"
#include "CGuildMasterEffect.h"
#include "CGuildRoomSystem.h"
#include "CMgrGuildHistory.h"
#include "CMgrAvatorItemHistory.h"
#include "CPartyPlayer.h"
#include "LendItemMng.h"
#include "CUnmannedTraderController.h"
#include "cStaticMember_Player.h"
#include "CTransportShip.h"
#include "CLogFile.h"
#include "CItemUpgradeTable.h"
#include "CMoneySupplyMgr.h"
#include "QuestHappenEvent_fld.h"
#include "equip_up_item_lv_limit_zocl.h"
#include "insert_new_quest_inform_zocl.h"
#include "insert_next_quest_inform_zocl.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "TimeItem.h"
#include "trans_gm_msg_inform_zocl.h"
#include "qry_case_disjointguild.h"

namespace
{
unsigned int s_ExpNotifyIndex = 0;
}


char CPlayer::Load(CUserDB *pUser, bool bFirstStart)
{
  _AVATOR_DATA pData{};
  memcpy_0(&pData, &pUser->m_AvatorData, sizeof(pData));

  this->m_id.dwSerial = pData.dbAvator.m_dwRecordNum;
  this->m_pUserDB = nullptr;
  this->m_byUserDgr = pUser->m_byUserDgr;
  this->m_bySubDgr = pUser->m_bySubDgr;
  this->m_bFirstStart = bFirstStart;
  this->m_Param.InitPlayerDB(this);
  this->m_bMapLoading = 0;
  this->m_bOper = 0;
  this->m_bFullMode = pUser->m_bWndFullMode;
  this->m_bCheat_100SuccMake = 0;
  this->m_bCheat_makeitem_no_use_matrial = 0;
  this->m_bCheat_Matchless = 0;
  this->m_nMaxAttackPnt = 0;
  this->m_nAnimusAttackPnt = 0;
  this->m_nTrapMaxAttackPnt = 0;
  this->m_byDamagePart = static_cast<unsigned __int8>(-1);
  this->m_bFreeRecallWaitTime = 0;
  this->m_bFreeSFByClass = 0;
  this->m_bLootFree = 0;
  this->m_bOutOfMap = 0;
  this->m_dwNextTimeDungeonDie = 0;
  this->m_bNeverDie = 0;
  this->m_nVoteSerial = -1;
  this->m_bBlockParty = 0;
  this->m_bBlockWhisper = 0;
  this->m_bBlockTrade = 0;
  this->m_bSpyGM = 0;
  this->m_bTakeGravityStone = 0;
  this->m_bBlockGuildBattleMsg = 0;
  this->m_bInGuildBattle = 0;
  this->m_bNotifyPosition = 0;
  this->m_byGuildBattleColorInx = static_cast<unsigned __int8>(-1);
  this->m_bTakeSoccerBall = 0;
  this->m_pSoccerItem = nullptr;
  this->m_dwSelfDestructionTime = 0;
  this->m_fSelfDestructionDamage = 0.0f;
  this->m_fTalik_DefencePoint = 0.0f;
  this->m_fTalik_AvoidPoint = 0.0f;
  this->m_bCheckMovePacket = 0;
  this->m_bLinkBoardDownload = 0;
  this->m_bSpecialDownload = 0;
  this->m_bQuestDownload = 0;
  this->m_bCumDownload = 0;
  this->m_bForceDownload = 0;
  this->m_bInvenDownload = 0;
  this->m_bBaseDownload = 0;
  this->m_bAMPInvenDownload = 0;
  this->m_bBuddyListDownload = 0;
  this->m_bGuildDownload = 0;
  this->m_bGuildListDownload = 0;
  this->m_byMapInModeBuffer = 0;
  this->m_pBeforeTownCheckMap = nullptr;
  this->m_bCreateComplete = 0;
  this->m_bUpCheckEquipEffect = 1;
  this->m_bDownCheckEquipEffect = 0;
  memset_0(this->m_byEffectEquipCode, 0, sizeof(this->m_byEffectEquipCode));
  this->m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);

  CMapData *map = g_MapOper.GetMap(pData.dbAvator.m_byMapCode);
  const int raceIndex = pData.dbAvator.m_byRaceSexCode / 2;
  if (g_TransportShip[raceIndex].IsMemberBeforeLogoff(pData.dbAvator.m_dwRecordNum))
  {
    map = g_TransportShip[raceIndex].m_pLinkShipMap;
    pData.dbAvator.m_byMapCode = map->m_pMapSet->m_dwIndex;
    g_TransportShip[raceIndex].GetStartPosInShip(pData.dbAvator.m_fStartPos);
  }
  else
  {
    if (!map || !map->IsMapIn(pData.dbAvator.m_fStartPos) || map->m_pMapSet->m_nMapType)
    {
      CMapData *posStartMap = g_MapOper.GetPosStartMap(
        pData.dbAvator.m_byRaceSexCode / 2,
        1,
        pData.dbAvator.m_fStartPos);
      if (!posStartMap)
      {
        g_Main.m_logSystemError.Write(
          "Load() : failure : %s.. no find start position(race:%d, x:%d, y:%d, z:%d)..",
          pUser->m_aszAvatorName,
          pData.dbAvator.m_byRaceSexCode / 2,
          static_cast<int>(pData.dbAvator.m_fStartPos[0]),
          static_cast<int>(pData.dbAvator.m_fStartPos[1]),
          static_cast<int>(pData.dbAvator.m_fStartPos[2]));
        return 0;
      }
      pData.dbAvator.m_byMapCode = posStartMap->m_pMapSet->m_dwIndex;
    }
    map = g_MapOper.GetMap(pData.dbAvator.m_byMapCode);
    if (!map->m_bUse)
    {
      g_Main.m_logSystemError.Write(
        "Load() : failure : %s.. no used map(%s)..",
        pUser->m_aszAvatorName,
        map->m_pMapSet->m_strCode);
      return 0;
    }
  }

  LendItemMng::Instance()->Release(this->m_ObjID.m_wIndex);
  if (this->m_Param.ConvertAvatorDB(&pData)
      && this->m_Param.ConvertGeneralDB(&pData, &pUser->m_AvatorData))
  {
    CUnmannedTraderController::Instance()->Load(
      this->m_ObjID.m_wIndex,
      this->m_id.dwSerial,
      &pData.dbTrade);
    this->m_bLoad = 1;
    this->m_pUserDB = pUser;
    this->m_bMapLoading = 1;
    this->m_dwLastCheckRegionTime = 0;
    this->m_wRegionIndex = -1;
    this->m_wRegionMapIndex = -1;

    const long double exp = this->m_Param.GetExp();
    const int level = this->m_Param.GetLevel();
    const long double limitExp = cStaticMember_Player::Instance()->GetLimitExp(level);
    this->m_dwExpRate = static_cast<int>(exp / limitExp * 1000000.0);

    const unsigned __int8 raceCode = this->m_Param.GetRaceCode();
    this->m_pmMst.Init(&pUser->m_AvatorData.dbStat, raceCode);
    this->m_pmTrd.Init();
    this->m_pmWpn.Init();
    this->m_pmTwr.Init();
    this->m_pmTrp.Init();
    this->m_pmBuddy.Init();
    this->m_pmCryMsg.Init();

    this->CalcAddPointByClass();
    this->m_nMaxPoint[0] = this->_CalcMaxHP();
    this->m_nMaxPoint[1] = this->_CalcMaxFP();
    this->m_nMaxPoint[2] = this->_CalcMaxSP();
    this->CalcEquipMaxDP(1);
    memcpy_0(this->m_nOldPoint, this->m_nMaxPoint, sizeof(this->m_nOldPoint));
    this->m_nOldMaxDP = this->GetMaxDP();

    if (bFirstStart)
    {
      this->SetHP(this->m_nMaxPoint[0], 0);
      this->SetFP(this->m_nMaxPoint[1], 0);
      this->SetSP(this->m_nMaxPoint[2], 0);
      this->SetDP(this->GetMaxDP(), 0);
    }

    this->m_fEquipSpeed = 0.0;
    for (int j = 0; j < 50; ++j)
    {
      if (this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].IsFilled())
      {
        if (this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].wszName[0] == 42)
        {
          this->m_pUserDB->Update_DelBuddy(j);
        }
        this->m_pmBuddy.m_List[j].dwSerial = this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].dwSerial;
        strcpy_0(this->m_pmBuddy.m_List[j].wszName, this->m_pUserDB->m_AvatorData.dbBuddy.m_List[j].wszName);
      }
    }

    this->m_NameChangeBuddyInfo.Init();

    const unsigned int mapEnvCode = map->m_Level.GetEnvironment();
    this->SendMsg_MapEnvInform(map->m_pMapSet->m_dwIndex, mapEnvCode);
    this->m_pDHChannel = nullptr;
    if (this->m_pUserDB->m_byUserDgr == 2)
    {
      this->m_bSpyGM = !strncmp(&this->m_pUserDB->m_szAccountID[1], "SPY", 3u);
    }

    _PVPPOINT_LIMIT_DB_BASE *pkInfo = &this->m_pUserDB->m_AvatorData.dbPvpPointLimit;
    const long double pvpPoint = this->m_Param.GetPvPPoint();
    if (this->m_kPvpPointLimiter.Set(pvpPoint, pkInfo))
    {
      this->m_pUserDB->UpdateContUserSave(1);
    }

    for (int k = 0; k < 10; ++k)
    {
      strcpy_0(this->m_pmCryMsg.m_List[k].wszCryMsg, this->m_pUserDB->m_AvatorData.dbBossCry.m_List[k].wszCryMsg);
    }

    _PVP_ORDER_VIEW_DB_BASE *p_dbPvpOrderView = &this->m_pUserDB->m_AvatorData.dbPvpOrderView;
    const long double pvpPointView = this->m_Param.GetPvPPoint();
    if (this->m_kPvpOrderView.SetPvpOrderView(pvpPointView, p_dbPvpOrderView, this))
    {
      this->m_pUserDB->UpdateContUserSave(1);
    }

    for (int m = 0; m < 3; ++m)
    {
      this->m_fGroupMapPoint[m][0] = 0.0;
      this->m_fGroupMapPoint[m][1] = 0.0;
      this->m_byGroupMapPointMapCode[m] = 0;
      this->m_wGroupMapPointLayerIndex[m] = 0;
      this->m_dwLastGroupMapPointTime[m] = 0;
    }

    this->m_byPlusKey = rand();
    this->m_wXorKey = rand();
    this->m_dwMoveCount = 0;
    this->m_dwTargetCount = 0;
    this->m_dwAttackCount = 0;
    this->m_bSFDelayNotCheck = 0;

    this->m_kPvpCashPoint.Init(&this->m_pUserDB->m_AvatorData.dbPvpOrderView);

    _PCBANG_PLAY_TIME *p_dbPlayTimeInPcbang = &this->m_pUserDB->m_AvatorData.dbPlayTimeInPcbang;
    CUserDB *m_pUserDB = this->m_pUserDB;
    this->m_kPcBangCoupon.LoadData(m_pUserDB->m_dwAccountSerial, p_dbPlayTimeInPcbang);

    this->SetLastAttBuff(this->m_pUserDB->m_AvatorData.dbSupplement.bLastAttBuff);

    for (int n = 0; n < 38; ++n)
    {
      this->m_PotionParam.SetPotionActDelay(
        n,
        0,
        this->m_pUserDB->m_AvatorData.dbPotionNextUseTime.dwPotionNextUseTime[n]);
    }

    this->m_bCntEnable = 0;
    return 1;
  }

  g_Main.m_logSystemError.Write(
    "Load() : failure : %d:%s",
    pData.dbAvator.m_dwRecordNum,
    pUser->m_aszAvatorName);
  return 0;
}

void CPlayer::SendMsg_Level(char nLevel)
{
  char msg[5]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_dwObjSerial;
  msg[4] = nLevel;

  unsigned __int8 type[2] = {11, 1};
  CircleReport(type, msg, 5, true);
}

void CPlayer::SendData_PartyMemberLv()
{
  if (!this->m_pPartyMgr)
  {
    return;
  }

  CPartyPlayer **partyMembers = this->m_pPartyMgr->GetPtrPartyMember();
  if (!partyMembers)
  {
    return;
  }

  char msg[5]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_dwObjSerial;
  msg[4] = static_cast<char>(this->m_Param.GetLevel());

  const int memberCount = this->m_pPartyMgr->GetPopPartyMember();
  unsigned __int8 type[2] = {16, 23};

  for (int i = 0; i < memberCount; ++i)
  {
    if (partyMembers[i] != this->m_pPartyMgr)
    {
      g_Network.m_pProcess[0]->LoadSendMsg(partyMembers[i]->m_wZoneIndex, type, msg, 5u);
    }
  }
}

void CPlayer::LimLvNpcQuestDelete(unsigned __int8 byQuestDBSlot)
{
  SendMsg_QuestFailure(6u, byQuestDBSlot);
  this->m_QuestMgr.DeleteQuestData(byQuestDBSlot);
  this->m_pUserDB->Update_QuestDelete(byQuestDBSlot);
}

void CPlayer::IncCriEffPvPCashBag(double dAlter)
{
  if (dAlter > 0.0 && this->m_byHSKQuestCode != 100)
  {
    CHolyStone *stone = &g_Stone[static_cast<int>(this->m_Param.GetRaceCode())];
    if (stone->m_bOper && stone->m_pCurMap == this->m_pCurMap)
    {
      this->m_nHSKPvpPoint = static_cast<int>(static_cast<double>(this->m_nHSKPvpPoint) + dAlter);
      SendMsg_HSKQuestActCum();
      this->m_pUserDB->m_AvatorData.m_iPvpPoint = this->m_nHSKPvpPoint;
    }
  }
}

void CPlayer::SendMsg_AlterExpInform()
{
  char msg[4]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_dwExpRate;

  unsigned __int8 type[2] = {11, 4};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 4u);
}

void CPlayer::SendMsg_NotifyGetExpInfo(long double dOldExp, long double dAlterExp, long double dCurExp)
{
  if (!g_Main.IsReleaseServiceMode())
  {
    _trans_gm_msg_inform_zocl msg{};
    memset_0(&msg, 0, sizeof(msg));
    msg.wMsgSize = _snprintf(
      msg.wszChatData,
      0xFFu,
      "%u Old Exp     : %f\n   Alter Exp   : %f\n   Cur Exp     : %f\n",
      ++s_ExpNotifyIndex,
      static_cast<double>(dOldExp),
      static_cast<double>(dAlterExp),
      static_cast<double>(dCurExp));

    unsigned __int8 type[2] = {2, 13};
    const unsigned __int16 len = static_cast<unsigned __int16>(msg.size());
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, reinterpret_cast<char *>(&msg), len);
  }
}

void CPlayer::SendMsg_EquipItemLevelLimit(int nCurPlayerLv)
{
  _equip_up_item_lv_limit_zocl msg{};

  for (int i = 0; i < 8; ++i)
  {
    _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbEquip.m_pStorageList[i];
    if (!item->m_bLoad)
    {
      continue;
    }

    const int limitLevel = GetItemEquipUpLevel(item->m_byTableCode, item->m_wItemIndex);
    if (limitLevel <= 0 || nCurPlayerLv <= limitLevel)
    {
      continue;
    }

    switch (item->m_byTableCode)
    {
      case 0:
        msg.byEquipItemNum[1] = 1;
        break;
      case 1:
        msg.byEquipItemNum[2] = 1;
        break;
      case 2:
        msg.byEquipItemNum[7] = 1;
        break;
      case 3:
        msg.byEquipItemNum[4] = 1;
        break;
      case 4:
        msg.byEquipItemNum[0] = 1;
        break;
      case 5:
        msg.byEquipItemNum[6] = 1;
        break;
      case 6:
        msg.byEquipItemNum[3] = 1;
        break;
      case 7:
        msg.byEquipItemNum[5] = 1;
        break;
      default:
        break;
    }
  }

  unsigned __int8 type[2] = {11, 35};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    type,
    reinterpret_cast<char *>(&msg),
    8u);
}

CPvpPointLimiter *CPlayer::GetPvpPointLimiter(CPvpPointLimiter *result)
{
  *result = m_kPvpPointLimiter;
  return result;
}

CPvpOrderView *CPlayer::GetPvpOrderView()
{
  return &m_kPvpOrderView;
}

void CPlayer::AlterExp(
  double dAlterExp,
  bool bReward,
  bool bUseExpRecoverItem,
  bool bUseExpAdditionItem)
{
  const double playerPenalty = static_cast<double>(
    g_Main.m_pTimeLimitMgr->GetPlayerPenalty(this->m_id.wIndex));
  const double baseAlterExp = dAlterExp;

  if (!((this->m_byUserDgr && this->m_byUserDgr != 1)
        || this->m_Param.GetLevel() < this->m_Param.GetMaxLevel()))
  {
    return;
  }

  const int level = this->m_Param.GetLevel();
  const int maxLevel = this->m_Param.GetMaxLevel();
  if (level >= maxLevel || dAlterExp == 0.0)
  {
    return;
  }

  double adjustedAlterExp = 0.0;
  if (!IsApplyPcbangPrimium() || bReward)
  {
    const float rate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PLAYER_EXP_RATE;
    adjustedAlterExp = dAlterExp + baseAlterExp * static_cast<double>(rate - 1.0f);
  }
  else
  {
    const float rate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PCBANG_PRIMIUM_FAVOR::PLAYER_EXP;
    adjustedAlterExp = dAlterExp + baseAlterExp * static_cast<double>(rate - 1.0f);
  }

  if (adjustedAlterExp > 0.0 && !bUseExpRecoverItem && !bReward && !this->m_EP.m_bLock)
  {
    float extraRate = 0.0f;
    extraRate += this->m_EP.GetEff_Have(2) - 1.0f;
    extraRate += this->m_EP.GetEff_Have(5) - 1.0f;
    extraRate += this->m_EP.GetEff_Have(9) - 1.0f;

    if (IsApplyPcbangPrimium())
    {
      const float baseRate = (baseAlterExp < 0.0) ? PLAYER_LOST_EXP : PCBANG_PRIMIUM_FAVOR::PLAYER_EXP;
      const float eff71 = this->m_EP.GetEff_Have(71);
      if (eff71 > baseRate)
      {
        extraRate += eff71 - baseRate;
      }
      extraRate += this->m_EP.GetEff_Have(72) - 1.0f;
    }
    else
    {
      extraRate += this->m_EP.GetEff_Have(71) - 1.0f;
      extraRate += this->m_EP.GetEff_Have(72) - 1.0f;
    }

    if (extraRate > 0.0f)
    {
      adjustedAlterExp += baseAlterExp * static_cast<double>(extraRate);
    }
  }

  double finalAlterExp = 0.0;
  if (bUseExpRecoverItem)
  {
    finalAlterExp = baseAlterExp;
    this->m_Param.SetLossExp(0.0);
    this->m_pUserDB->Update_LossExp(0.0);
  }
  else if (bUseExpAdditionItem)
  {
    finalAlterExp = baseAlterExp;
    if (baseAlterExp < 0.0)
    {
      this->m_Param.SetLossExp(-baseAlterExp);
      this->m_pUserDB->Update_LossExp(-finalAlterExp);
    }
  }
  else
  {
    const float rate34 = this->m_EP.GetEff_Rate(34);
    const double withRate34 = adjustedAlterExp + baseAlterExp * static_cast<double>(rate34 - 1.0f);
    const float rate38 = this->m_EP.GetEff_Rate(38);
    finalAlterExp = withRate34 + baseAlterExp * static_cast<double>(rate38 - 1.0f);
    if (baseAlterExp < 0.0)
    {
      this->m_Param.SetLossExp(-finalAlterExp);
      this->m_pUserDB->Update_LossExp(-finalAlterExp);
    }
  }

  const long double penalizedAlter = static_cast<long double>(finalAlterExp * playerPenalty);
  const long double oldExp = this->m_Param.GetExp();
  const long double curExp = oldExp + penalizedAlter;
  SendMsg_NotifyGetExpInfo(oldExp, penalizedAlter, curExp);

  const int curLevel = this->m_Param.GetLevel();
  const long double limitExp = cStaticMember_Player::Instance()->GetLimitExp(curLevel);
  if (curExp < limitExp)
  {
    this->m_Param.SetExp(curExp);
    const long double exp = this->m_Param.GetExp();
    const int newLevel = this->m_Param.GetLevel();
    const long double newLimit = cStaticMember_Player::Instance()->GetLimitExp(newLevel);
    this->m_dwExpRate = static_cast<int>(exp / newLimit * 1000000.0);
    SendMsg_AlterExpInform();
  }
  else
  {
    const unsigned __int8 oldLevel = static_cast<unsigned __int8>(GetLevel());
    const unsigned __int8 nextLevel = static_cast<unsigned __int8>(oldLevel + 1);
    SetLevel(nextLevel);

    if (bReward)
    {
      if (this->m_Param.GetMaxLevel() > this->m_Param.GetLevel())
      {
        const long double oldLimitExp = cStaticMember_Player::Instance()->GetLimitExp(oldLevel);
        this->m_Param.SetExp(curExp - oldLimitExp);
        const long double exp = this->m_Param.GetExp();
        const long double newLimit = cStaticMember_Player::Instance()->GetLimitExp(nextLevel);
        this->m_dwExpRate = static_cast<int>(exp / newLimit * 1000000.0);
      }
      else
      {
        this->m_Param.SetExp(0.0);
        this->m_dwExpRate = 0;
      }
      SendMsg_AlterExpInform();
    }
    else
    {
      this->m_Param.SetExp(0.0);
      this->m_dwExpRate = 0;
      SendMsg_AlterExpInform();
    }

    this->m_bDownCheckEquipEffect = 1;
    SendMsg_EquipItemLevelLimit(nextLevel);
  }

  if (this->m_pUserDB)
  {
    const long double exp = this->m_Param.GetExp();
    this->m_pUserDB->Update_Exp(exp);
  }
}

void CPlayer::SendMsg_GuildEstablishFail(char byRetCode)
{
  char msg[1]{};
  msg[0] = byRetCode;

  unsigned __int8 type[2] = {27, 2};
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, type, msg, 1u);
}

void CPlayer::SendMsg_GuildJoinOtherInform()
{
  char msg[10]{};
  *reinterpret_cast<unsigned int *>(msg) = this->m_id.dwSerial;
  *reinterpret_cast<unsigned int *>(msg + 4) = this->m_Param.m_pGuild ? this->m_Param.m_pGuild->m_dwSerial
                                                                     : static_cast<unsigned int>(-1);
  *reinterpret_cast<unsigned __int16 *>(msg + 8) = this->m_wVisualVer;

  unsigned __int8 type[2] = {27, 39};
  CircleReport(type, msg, 10, false);
}


#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::SendMsg_SelectQuestReward(char byQuestDBSlot)
{
  char szMsg[32]; // [rsp+34h] [rbp-44h] BYREF
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  szMsg[0] = byQuestDBSlot;
  pbyType[0] = 24;
  pbyType[1] = 8;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
}
#endif

void CPlayer::SendMsg_AlterUnitHPInform(char bySlotIndex, unsigned int dwGauge)
{
  char szMsg; // [rsp+34h] [rbp-44h] BYREF
  unsigned int v7; // [rsp+35h] [rbp-43h]
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  szMsg = bySlotIndex;
  v7 = dwGauge;
  pbyType[0] = 5;
  pbyType[1] = 23;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, &szMsg, 5u);
}

void CPlayer::SendMsg_FcitemInform(unsigned __int16 wItemSerial, unsigned int dwNewStat)
{
  char szMsg[2]; // [rsp+34h] [rbp-44h] BYREF
  unsigned int v7; // [rsp+36h] [rbp-42h]
  unsigned __int8 pbyType[36]; // [rsp+54h] [rbp-24h] BYREF

  *(_WORD *)szMsg = wItemSerial;
  v7 = dwNewStat;
  pbyType[0] = 3;
  pbyType[1] = 44;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 6u);
}

#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::SendMsg_QuestFailure(char byFailCode, char byQuestDBSlot)
{
  char szMsg[32]{}; // [rsp+34h] [rbp-44h] BYREF
  unsigned __int8 pbyType[36]{}; // [rsp+54h] [rbp-24h] BYREF

  szMsg[0] = byFailCode;
  szMsg[1] = byQuestDBSlot;
  pbyType[0] = 24;
  pbyType[1] = 7;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 2u);
}
#endif

void CPlayer::SendMsg_InsertQuestFailure(char byEventType, unsigned int dwEventIndex, unsigned __int8 byEventNodeIndex)
{
  char szMsg; // [rsp+34h] [rbp-44h] BYREF
  unsigned int dwIndex; // [rsp+35h] [rbp-43h]
  unsigned __int8 byNode; // [rsp+39h] [rbp-3Fh]
  unsigned __int8 pbyType[36]{}; // [rsp+54h] [rbp-24h] BYREF

  szMsg = byEventType;
  dwIndex = dwEventIndex;
  byNode = byEventNodeIndex;
  pbyType[0] = 24;
  pbyType[1] = 4;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, &szMsg, 6u);
}

#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::SendMsg_InsertNextQuest(unsigned __int8 bySlotIndex, _QUEST_DB_BASE::_LIST *pQuestDB)
{
  _insert_next_quest_inform_zocl msg{};
  unsigned __int8 pbyType[28]{};

  msg.byQuestDBSlot = bySlotIndex;
  memcpy_0(&msg.NewQuestData, pQuestDB, sizeof(msg.NewQuestData));
  pbyType[0] = 24;
  pbyType[1] = 111;
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    0xEu);
}
#endif

void CPlayer::SetLevel(unsigned __int8 byNewLevel)
{
  if ((this->m_byUserDgr && this->m_byUserDgr != 1) || byNewLevel <= this->m_Param.GetMaxLevel())
  {
    const unsigned __int8 level = this->m_Param.GetLevel();
    if (level < byNewLevel)
    {
      if (this->m_pUserDB)
      {
        const long double exp = this->m_Param.GetExp();
        this->m_pUserDB->Update_Level(byNewLevel, exp);
      }
      this->m_Param.SetLevel(byNewLevel);
      this->SetGrade(CPlayerDB::CalcCharGrade(byNewLevel, this->m_Param.m_dbChar.m_wRankRate));

      char *levelStr = cvt_string(byNewLevel);
      if (!this->Emb_CreateQuestEvent(quest_happen_type_lv, levelStr))
      {
        char *levelStr2 = cvt_string(byNewLevel);
        this->Emb_CheckActForQuest(6, levelStr2, 1u, 0);
      }

      if (byNewLevel == this->m_Param.GetMaxLevel())
      {
        char *levelStr3 = cvt_string(byNewLevel);
        this->Emb_CreateQuestEvent(quest_happen_type_maxlevel, levelStr3);
      }

      this->m_bUpCheckEquipEffect = 1;
      this->ReCalcMaxHFSP(1, 0);
      this->SetHP(this->GetMaxHP(), 1);
      this->SetFP(this->GetMaxFP(), 1);
      this->SetSP(this->GetMaxSP(), 1);
      this->SendMsg_Level(byNewLevel);
      this->SendData_PartyMemberLv();
      if (this->m_Param.IsClassChangeableLv())
      {
        this->SendMsg_ChangeClassCommand();
      }

      CPlayer::s_MgrLvHistory.upgrade_lv(
        this->m_ObjID.m_wIndex,
        byNewLevel,
        this->m_Param.m_byPvPGrade,
        this->m_nMaxPoint,
        this->m_szLvHistoryFileName);

      _quest_fail_result *lim = this->m_QuestMgr.CheckLimLv(byNewLevel);
      if (lim)
      {
        this->LimLvNpcQuestDelete(lim->m_List[0].byQuestDBSlot);
      }
    }
  }
}

void CPlayer::SetLevelD(unsigned __int8 byDownLevel)
{
  const unsigned __int8 level = this->m_Param.GetLevel();
  if (level > byDownLevel)
  {
    if (this->m_pUserDB)
    {
      const long double exp = this->m_Param.GetExp();
      this->m_pUserDB->Update_Level(byDownLevel, exp);
    }
    this->m_Param.SetLevel(byDownLevel);
    this->SetGrade(CPlayerDB::CalcCharGrade(byDownLevel, this->m_Param.m_dbChar.m_wRankRate));
    this->ReCalcMaxHFSP(1, 0);
    this->SetHP(this->GetMaxHP(), 1);
    this->SetFP(this->GetMaxFP(), 1);
    this->SetSP(this->GetMaxSP(), 1);
    this->SendMsg_Level(byDownLevel);
    this->SendData_PartyMemberLv();
    this->m_bDownCheckEquipEffect = 1;
    CPlayer::s_MgrLvHistory.downgrade_lv(
      this->m_ObjID.m_wIndex,
      byDownLevel,
      this->m_Param.m_byPvPGrade,
      this->m_nMaxPoint,
      this->m_szLvHistoryFileName);
  }
}

void CPlayer::AlterPvPCashBag(long double dAlter, int IOCode)
{
  double PvpCash; // [rsp+20h] [rbp-18h]
  long double dAltera; // [rsp+28h] [rbp-10h]

  PvpCash = this->m_kPvpOrderView.GetPvpCash();
  dAltera = PvpCash + dAlter;
  if ( PvpCash + dAlter > 999999.0 )
    dAltera = 999999.0;
  if ( dAltera < -999999.0 )
    dAltera = -999999.0;
  if ( this->GetLevel() > 39 && this->m_Param.m_pClassData->m_nGrade >= 1 )
  {
    this->m_kPvpOrderView.SetPvpCash(dAltera);
    if ( dAltera > PvpCash && dAlter > 0.0 )
      this->IncCriEffPvPCashBag(dAlter);
    const long double currentCash = this->m_kPvpOrderView.GetPvpCash();
    if ( PvpCash != currentCash )
    {
      this->m_kPvpOrderView.UpdatePvpCash(dAltera);
      this->SendMsg_AlterPvPCash(IOCode);
    }
  }
}

#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::Emb_ItemUpgrade(
  unsigned __int8 byUpgradeType,
  unsigned __int8 byStorageCode,
  unsigned __int8 byStorageIndex,
  unsigned int dwGradeInfo)
{
  _STORAGE_LIST::_storage_con *pItem = &this->m_Param.m_pStoragePtr[byStorageCode]->m_pStorageList[byStorageIndex];
  if (byStorageCode == 1)
  {
    if (this->GetEffectEquipCode(byStorageCode, byStorageIndex) == 1)
    {
      this->SetEquipEffect(static_cast<_STORAGE_LIST::_db_con *>(pItem), 0);
    }
    this->SetEffectEquipCode(byStorageCode, byStorageIndex, 0);
  }

  if (byUpgradeType)
  {
    if (byUpgradeType == 1)
    {
      this->m_Param.m_pStoragePtr[byStorageCode]->GradeDown(byStorageIndex, dwGradeInfo);
    }
    else if (byUpgradeType == 2)
    {
      const unsigned __int8 limSocket = GetItemUpgLimSocket(pItem->m_dwLv);
      const unsigned int dwUptInfo = GetBitAfterSetLimSocket(limSocket);
      this->m_Param.m_pStoragePtr[byStorageCode]->SetGrade(byStorageIndex, 0, dwUptInfo);
    }
  }
  else
  {
    this->m_Param.m_pStoragePtr[byStorageCode]->GradeUp(byStorageIndex, dwGradeInfo);
  }

  if (byStorageCode == 1)
  {
    if (this->IsEffectableEquip(pItem))
    {
      this->SetEquipEffect(static_cast<_STORAGE_LIST::_db_con *>(pItem), 1);
      this->SetEffectEquipCode(byStorageCode, byStorageIndex, 1u);
    }
    else
    {
      this->SetEffectEquipCode(byStorageCode, byStorageIndex, 2u);
    }
    if (pItem->m_byTableCode == 6)
    {
      this->m_pmWpn.FixWeapon(static_cast<_STORAGE_LIST::_db_con *>(pItem));
    }
  }

  if (byStorageCode == 1)
  {
    CPlayer::CashChangeStateFlag changeFlag(0);
    this->UpdateVisualVer(changeFlag);
    this->SendMsg_EquipPartChange(pItem->m_byTableCode);
  }

  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_ItemUpgrade(byStorageCode, byStorageIndex, dwGradeInfo, 1);
  }
}
#endif

#if 0 // duplicate implementation exists in CPlayer.cpp
void CPlayer::Emb_CompleteQuest(
  unsigned __int8 byQuestDBSlot,
  unsigned __int8 byRewardItemIndex,
  unsigned __int8 byLinkQuestIndex)
{
  __int64 *v4; // rdi
  __int64 i; // rcx
  unsigned int Level; // eax
  int m_wIndex; // ecx
  unsigned int v8; // eax
  int v9; // ecx
  unsigned __int8 MapCode; // al
  __int64 v11; // [rsp+0h] [rbp-198h] BYREF
  int nGrade; // [rsp+30h] [rbp-168h]
  int *pnMaxPoint; // [rsp+38h] [rbp-160h]
  _MASTERY_PARAM *pData; // [rsp+40h] [rbp-158h]
  unsigned int *pdwAlter; // [rsp+48h] [rbp-150h]
  char *pszFileName; // [rsp+50h] [rbp-148h]
  unsigned __int8 byLogType; // [rsp+58h] [rbp-140h]
  char *pszTitle; // [rsp+60h] [rbp-138h]
  void *Src; // [rsp+70h] [rbp-128h]
  _QUEST_DB_BASE::_LIST v20; // [rsp+88h] [rbp-110h] BYREF
  _Quest_fld *Record; // [rsp+A8h] [rbp-F0h]
  int j; // [rsp+B0h] [rbp-E8h]
  _Quest_fld *pQuestFld; // [rsp+B8h] [rbp-E0h]
  unsigned __int8 inserted; // [rsp+C0h] [rbp-D8h]
  _QUEST_DB_BASE::_NPC_QUEST_HISTORY *pHisData; // [rsp+C8h] [rbp-D0h]
  float pNewPos[8]; // [rsp+D8h] [rbp-C0h] BYREF
  _dummy_position *pPos; // [rsp+F8h] [rbp-A0h]
  int v28; // [rsp+100h] [rbp-98h]
  unsigned __int8 v29; // [rsp+104h] [rbp-94h]
  _QUEST_DB_BASE::_LIST *pSlotData; // [rsp+108h] [rbp-90h]
  int k; // [rsp+110h] [rbp-88h]
  int m; // [rsp+114h] [rbp-84h]
  char *m_szLvHistoryFileName; // [rsp+120h] [rbp-78h]
  unsigned int *m_dwAlterMastery; // [rsp+128h] [rbp-70h]
  _MASTERY_PARAM *p_m_pmMst; // [rsp+130h] [rbp-68h]
  int *m_nMaxPoint; // [rsp+138h] [rbp-60h]
  int m_byPvPGrade; // [rsp+140h] [rbp-58h]
  long double dExp; // [rsp+148h] [rbp-50h]
  char *m_strCode; // [rsp+150h] [rbp-48h]
  char *v40; // [rsp+158h] [rbp-40h]
  unsigned int *v41; // [rsp+160h] [rbp-38h]
  _MASTERY_PARAM *v42; // [rsp+168h] [rbp-30h]
  int *v43; // [rsp+170h] [rbp-28h]
  int v44; // [rsp+178h] [rbp-20h]
  long double Exp; // [rsp+180h] [rbp-18h]

  if ( this->m_pUserDB )
  {
    Src = &this->m_Param.m_QuestDB.m_List[byQuestDBSlot];
    memcpy_0(&v20, Src, sizeof(v20));
    Record = 0LL;
    if ( *(unsigned __int8 *)Src != 255 )
    {
      for ( j = 0; j < 3; ++j )
      {
        if ( *(unsigned __int16 *)((char *)Src + 2 * j + 3) != 0xFFFF )
          return;
      }
      m_szLvHistoryFileName = this->m_szLvHistoryFileName;
      m_dwAlterMastery = this->m_Param.m_dwAlterMastery;
      p_m_pmMst = &this->m_pmMst;
      m_nMaxPoint = this->m_nMaxPoint;
      m_byPvPGrade = this->m_Param.m_byPvPGrade;
      dExp = this->m_Param.GetExp();
      Level = this->m_Param.GetLevel();
      m_wIndex = this->m_ObjID.m_wIndex;
      pszTitle = 0LL;
      byLogType = 0;
      pszFileName = m_szLvHistoryFileName;
      pdwAlter = m_dwAlterMastery;
      pData = p_m_pmMst;
      pnMaxPoint = m_nMaxPoint;
      nGrade = m_byPvPGrade;
      CPlayer::s_MgrLvHistory.update_mastery(
        m_wIndex,
        this->m_byUserDgr,
        Level,
        dExp,
        this->m_dwExpRate,
        m_byPvPGrade,
        m_nMaxPoint,
        p_m_pmMst,
        m_dwAlterMastery,
        m_szLvHistoryFileName,
        0,
        nullptr);
      this->m_Param.InitAlterMastery();
      pQuestFld = reinterpret_cast<_Quest_fld *>(
        CQuestMgr::s_tblQuest
          ? CQuestMgr::s_tblQuest->GetRecord(*(unsigned __int16 *)((char *)Src + 1))
          : nullptr);
      Record = this->_Reward_Quest(pQuestFld, byRewardItemIndex);
      this->SendMsg_QuestComplete(byQuestDBSlot);
      if ( *(_BYTE *)Src == 1 )
      {
        inserted = this->m_QuestMgr.InsertNpcQuestHistory(
          pQuestFld->m_strCode,
          pQuestFld->m_nDifficultyLevel,
          pQuestFld->m_dRepeatTime);
        pHisData = &this->m_Param.m_QuestDB.m_History[inserted];
        this->m_pUserDB->Update_NPCQuestHistory( inserted, pHisData);
        this->SendMsg_NpcQuestHistoryInform(inserted);
      }
      this->m_QuestMgr.DeleteQuestData(byQuestDBSlot);
      this->m_pUserDB->Update_QuestDelete( byQuestDBSlot);
      m_strCode = pQuestFld->m_strCode;
      v40 = this->m_szLvHistoryFileName;
      v41 = this->m_Param.m_dwAlterMastery;
      v42 = &this->m_pmMst;
      v43 = this->m_nMaxPoint;
      v44 = this->m_Param.m_byPvPGrade;
      Exp = this->m_Param.GetExp();
      v8 = this->m_Param.GetLevel();
      v9 = this->m_ObjID.m_wIndex;
      pszTitle = m_strCode;
      byLogType = 2;
      pszFileName = v40;
      pdwAlter = v41;
      pData = v42;
      pnMaxPoint = v43;
      nGrade = v44;
      CPlayer::s_MgrLvHistory.update_mastery(
        v9,
        this->m_byUserDgr,
        v8,
        Exp,
        this->m_dwExpRate,
        v44,
        v43,
        v42,
        v41,
        v40,
        2u,
        m_strCode);
      this->m_Param.InitAlterMastery();
      this->m_dwUMWHLastTime = GetLoopTime();
      if ( pQuestFld->m_nLinkDummyCond == 1 )
      {
        if ( strcmp_0(pQuestFld->m_strLinkDummyCode, "-1") )
        {
          pPos = this->m_pCurMap->GetDummyPostion(pQuestFld->m_strLinkDummyCode);
          if ( pPos )
          {
            if ( this->m_pCurMap->GetRandPosInDummy(pPos, pNewPos, 1) )
            {
              this->OutOfMap(this->m_pCurMap, this->m_wMapLayerIndex, 3u, pNewPos);
              MapCode = this->m_Param.GetMapCode();
              this->SendMsg_GotoRecallResult(0, MapCode, pNewPos, 4u);
            }
          }
        }
      }
      if ( !Record )
      {
        v28 = 0;
        for ( j = 0; j < 5; ++j )
        {
          if ( strncmp(pQuestFld->m_strLinkQuest[(__int64)j], "-1", 2uLL) )
            ++v28;
        }
        if ( v28 > 0 )
        {
          v29 = byLinkQuestIndex;
          if ( byLinkQuestIndex == 255 )
            v29 = rand() % v28;
          Record = reinterpret_cast<_Quest_fld *>(
            CQuestMgr::s_tblQuest
              ? CQuestMgr::s_tblQuest->GetRecord(pQuestFld->m_strLinkQuest[static_cast<unsigned __int64>(v29)])
              : nullptr);
        }
      }
      if ( Record )
      {
        pSlotData = &this->m_Param.m_QuestDB.m_List[byQuestDBSlot];
        pSlotData->byQuestType = v20.byQuestType;
        pSlotData->wIndex = Record->m_dwIndex;
        pSlotData->dwPassSec = 0;
        for ( k = 0; k < 3; ++k )
        {
          if ( Record->m_ActionNode[k].m_nActType != -1 )
            pSlotData->wNum[k] = 0;
        }
        this->m_pUserDB->Update_QuestInsert( byQuestDBSlot, pSlotData);
        this->SendMsg_InsertNextQuest(byQuestDBSlot, pSlotData);
      }
      for ( m = 0; m < 3; ++m )
      {
        if (this->m_QuestMgr.m_pTempHappenEvent[m].isset())
        {
          memcpy_0(
            &this->m_QuestMgr.m_LastHappenEvent,
            &this->m_QuestMgr.m_pTempHappenEvent[m],
            sizeof(this->m_QuestMgr.m_LastHappenEvent));
          this->Emb_StartQuest(0xFFu, &this->m_QuestMgr.m_pTempHappenEvent[m]);
          if ( this->m_QuestMgr.m_pTempHappenEvent[m].m_QtHpType == quest_happen_type_maxlevel )
          {
            this->m_Param.SetMaxLevel( 50);
            if ( this->m_pUserDB )
              this->m_pUserDB->Update_MaxLevel( 0x32u);
          }
          this->m_QuestMgr.m_pTempHappenEvent[m].init();
        }
      }
    }
  }
}
#endif

void CPlayer::Guild_Insert_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int EstConsumeDalant; // eax
  unsigned int v4; // eax
  CPlayer::CashChangeStateFlag *v5; // rax
  CGuildBattleController *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-4C8h] BYREF
  int v8; // [rsp+30h] [rbp-498h]
  unsigned int dwSerial; // [rsp+34h] [rbp-494h]
  char Destination[28]; // [rsp+48h] [rbp-480h] BYREF
  unsigned __int8 v11; // [rsp+64h] [rbp-464h]
  unsigned __int8 v12; // [rsp+65h] [rbp-463h]
  unsigned int v13[16]; // [rsp+78h] [rbp-450h] BYREF
  unsigned int dwMemberSerial[18]; // [rsp+B8h] [rbp-410h] BYREF
  char Source[168]; // [rsp+100h] [rbp-3C8h] BYREF
  _BYTE v16[32]; // [rsp+1A8h] [rbp-320h] BYREF
  _BYTE v17[32]; // [rsp+1C8h] [rbp-300h] BYREF
  _DWORD v18[11]; // [rsp+1E8h] [rbp-2E0h] BYREF
  int v19; // [rsp+214h] [rbp-2B4h]
  int v20; // [rsp+218h] [rbp-2B0h]
  char *m_sData; // [rsp+220h] [rbp-2A8h]
  CGuild *v22; // [rsp+228h] [rbp-2A0h]
  int j; // [rsp+230h] [rbp-298h]
  CPlayer *v24; // [rsp+238h] [rbp-290h]
  CPlayer *v25; // [rsp+240h] [rbp-288h]
  char szTran[136]; // [rsp+260h] [rbp-268h] BYREF
  CPlayer *v27; // [rsp+2E8h] [rbp-1E0h]
  _guild_member_info __t[8]; // [rsp+300h] [rbp-1C8h] BYREF
  CPlayer *pPtr; // [rsp+488h] [rbp-40h]
  _guild_member_info *pMem; // [rsp+490h] [rbp-38h]
  CPlayer::CashChangeStateFlag v31{0}; // [rsp+4A0h] [rbp-28h] BYREF
  char *pszFileName; // [rsp+4A8h] [rbp-20h]
  unsigned int dwLeftDalant; // [rsp+4B0h] [rbp-18h]

  v8 = -1;
  dwSerial = -1;
  v11 = 0;
  v12 = 0;
  v19 = -1;
  v20 = -1;
  m_sData = pData->m_sData;
  v8 = *(_DWORD *)pData->m_sData;
  dwSerial = *(_DWORD *)&pData->m_sData[272];
  v19 = *(_DWORD *)&pData->m_sData[276];
  v20 = *(_DWORD *)&pData->m_sData[280];
  v11 = pData->m_sData[21];
  v12 = pData->m_sData[22];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(v13, m_sData + 24, 0x20uLL);
  memcpy_0(dwMemberSerial, m_sData + 56, 0x20uLL);
  memcpy_0(v18, m_sData + 240, 0x20uLL);
  memcpy_0(v16, m_sData + 224, 8uLL);
  memcpy_0(v17, m_sData + 232, 8uLL);
  memcpy_0(Source, m_sData + 88, 0x88uLL);
  v22 = &g_Guild[v8];
  v22->ReleaseTemp();
  for ( j = 0; j < v12; ++j )
  {
    v24 = &g_Player[v13[j]];
    if ( v24->m_bLive && v24->m_dwObjSerial == dwMemberSerial[j] )
      v24->m_Param.m_bGuildLock = 0;
  }
  if ( pData->m_byResult || v22->IsFill() )
  {
    v25 = &g_Player[v19];
    if ( v25->m_bLive && v25->m_dwObjSerial == v20 )
    {
      v25->SendMsg_GuildEstablishFail( 0xFFu);
      EstConsumeDalant = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
      v25->AddDalant( EstConsumeDalant, 0);
      W2M(Destination, szTran, 0x80u);
      pszFileName = v25->m_szItemHistoryFileName;
      dwLeftDalant = v25->m_Param.GetDalant();
      v4 = g_Main.m_GuildCreateEventInfo.GetEstConsumeDalant();
      CPlayer::s_MgrItemHistory.guild_est_money_rollback(
        v25->m_ObjID.m_wIndex,
        szTran,
        v4,
        dwLeftDalant,
        pszFileName);
    }
  }
  else
  {
    v27 = &g_Player[v19];
    if ( v27->m_bLive && v27->m_dwObjSerial == v20 )
      v27->SendMsg_AlterMoneyInform( 0);
    for ( j = 0; j < v12; ++j )
    {
      __t[j].dwSerial = dwMemberSerial[j];
      strcpy_0(__t[j].wszName, &Source[17 * j]);
      __t[j].byLv = v16[j];
      __t[j].dwPvpPoint = v18[j];
      __t[j].byClassInGuild = 0;
      __t[j].byGrade = v17[j];
    }
    v22->EstGuild( dwSerial, Destination, v11, v12, __t);
    for ( j = 0; j < v12; ++j )
    {
      pPtr = &g_Player[v13[j]];
      if ( pPtr->m_bLive && pPtr->m_dwObjSerial == dwMemberSerial[j] )
      {
        pMem = v22->LoginMember( dwMemberSerial[j], pPtr);
        v22->SendMsg_DownPacket( 0, pMem);
        pPtr->m_pUserDB->m_AvatorData.dbAvator.m_dwGuildSerial = dwSerial;
        pPtr->m_Param.m_pGuild = v22;
        pPtr->m_Param.m_pGuildMemPtr = pMem;
        pPtr->m_Param.SetClassInGuild( 0);
        pPtr->UpdateVisualVer(v31);
        pPtr->SendMsg_GuildJoinOtherInform();
      }
    }
    v6 = CGuildBattleController::Instance();
    v6->UpdatePossibleBattleGuildList();
  }
}

void CPlayer::Guild_Join_Accept_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  const char *CharNameW; // rax
  CPlayer::CashChangeStateFlag *v4; // rax
  char *CharNameA; // rax
  CGuildBattleController *v6; // rax
  __int64 v7; // [rsp+0h] [rbp-198h] BYREF
  int v8; // [rsp+40h] [rbp-158h]
  int v9; // [rsp+44h] [rbp-154h]
  int v10; // [rsp+48h] [rbp-150h]
  unsigned int dwApplierSerial; // [rsp+4Ch] [rbp-14Ch]
  unsigned int dwMemberSerial; // [rsp+50h] [rbp-148h]
  char *m_sData; // [rsp+58h] [rbp-140h]
  CPlayer *pPlayer; // [rsp+60h] [rbp-138h]
  CGuild *v15; // [rsp+68h] [rbp-130h]
  _guild_member_info *MemberFromSerial; // [rsp+70h] [rbp-128h]
  _guild_applier_info *ApplierFromSerial; // [rsp+78h] [rbp-120h]
  _guild_member_info pSheet; // [rsp+88h] [rbp-110h] BYREF
  _guild_member_info *p; // [rsp+C8h] [rbp-D0h]
  char szTran[144]; // [rsp+E0h] [rbp-B8h] BYREF
  CPlayer::CashChangeStateFlag v21{0}; // [rsp+170h] [rbp-28h] BYREF
  char *pszFileName; // [rsp+178h] [rbp-20h]
  int n; // [rsp+180h] [rbp-18h]

  v8 = -1;
  v9 = -1;
  v10 = -1;
  dwApplierSerial = -1;
  dwMemberSerial = -1;
  m_sData = pData->m_sData;
  v8 = *(_DWORD *)pData->m_sData;
  v9 = *(_DWORD *)&pData->m_sData[4];
  v10 = *(_DWORD *)&pData->m_sData[8];
  dwApplierSerial = *(_DWORD *)&pData->m_sData[12];
  dwMemberSerial = *(_DWORD *)&pData->m_sData[16];
  pPlayer = &g_Player[v10];
  if ( pPlayer->m_bLive && pPlayer->m_dwObjSerial == dwApplierSerial )
    pPlayer->m_Param.m_bGuildLock = 0;
  v15 = &g_Guild[v8];
  if ( v15->m_dwSerial == v9 )
  {
    --v15->m_nTempMemberNum;
    if ( v15->m_nMemberNum < 50 && !pData->m_byResult )
    {
      MemberFromSerial = v15->GetMemberFromSerial( dwMemberSerial);
      if ( MemberFromSerial )
      {
        ApplierFromSerial = v15->GetApplierFromSerial( dwApplierSerial);
        if ( ApplierFromSerial )
        {
          pPlayer = ApplierFromSerial->pPlayer;
          pPlayer->m_Param.SetClassInGuild( 0);
          pPlayer->m_Param.m_pApplyGuild = 0LL;
          v15->PopApplier( dwApplierSerial, 0);
          pSheet.dwSerial = pPlayer->m_dwObjSerial;
          CharNameW = pPlayer->m_Param.GetCharNameW();
          strcpy_0(pSheet.wszName, CharNameW);
          pSheet.byLv = pPlayer->GetLevel();
          pSheet.dwPvpPoint = (int)pPlayer->m_Param.GetPvPPoint();
          pSheet.byClassInGuild = pPlayer->m_Param.GetClassInGuild();
          pSheet.pPlayer = pPlayer;
          p = v15->PushMember( &pSheet);
          if ( p )
            v15->SendMsg_GuildJoinAcceptInform( p, dwMemberSerial);
          pPlayer->m_Param.m_pGuild = v15;
          pPlayer->m_Param.m_pGuildMemPtr = p;
          pPlayer->UpdateVisualVer(v21);
          pPlayer->SendMsg_GuildJoinOtherInform();
          
            v15->SendMsg_GuildMemberLogin(
            pPlayer->m_dwObjSerial,
            pPlayer->m_wRegionMapIndex,
            pPlayer->m_wRegionIndex);
          W2M(MemberFromSerial->wszName, szTran, 0x80u);
          pszFileName = v15->m_szHistoryFileName;
          CharNameA = pPlayer->m_Param.GetCharNameA();
          CGuild::s_MgrHistory.join_member(
            CharNameA,
            pPlayer->m_dwObjSerial,
            szTran,
            MemberFromSerial->dwSerial,
            v15->m_nMemberNum,
            pszFileName);
          n = pPlayer->m_ObjID.m_wIndex;
          v6 = CGuildBattleController::Instance();
          v6->JoinGuild(n, v15->m_dwSerial, pPlayer->m_dwObjSerial);
        }
      }
    }
  }
}

void CPlayer::Guild_Self_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildRoomSystem *Instance; // rax
  CGuildRoomSystem *v4; // rax
  CGuildBattleController *v5; // rax
  CGuildMasterEffect *v6; // rax
  CPlayer::CashChangeStateFlag *v7; // rax
  char *CharNameA; // rax
  int v9; // eax
  __int64 v10; // [rsp+0h] [rbp-E8h] BYREF
  char v11; // [rsp+40h] [rbp-A8h]
  CPlayer *pkPlayer; // [rsp+48h] [rbp-A0h]
  CGuild *v13; // [rsp+50h] [rbp-98h]
  int v14; // [rsp+58h] [rbp-90h]
  unsigned int dwMemberSerial; // [rsp+5Ch] [rbp-8Ch]
  int v16; // [rsp+60h] [rbp-88h]
  int v17; // [rsp+64h] [rbp-84h]
  char *m_sData; // [rsp+68h] [rbp-80h]
  char v19; // [rsp+70h] [rbp-78h]
  _qry_case_disjointguild v20; // [rsp+88h] [rbp-60h] BYREF
  CPlayer::CashChangeStateFlag v21{0}; // [rsp+94h] [rbp-54h] BYREF
  CUserDB *m_pUserDB; // [rsp+98h] [rbp-50h]
  int n; // [rsp+A0h] [rbp-48h]
  CGuild *m_pGuild; // [rsp+A8h] [rbp-40h]
  CUserDB *v25; // [rsp+B0h] [rbp-38h]
  int m_wIndex; // [rsp+B8h] [rbp-30h]
  CGuild *v27; // [rsp+C0h] [rbp-28h]
  unsigned __int8 Grade; // [rsp+C8h] [rbp-20h]
  char *pszFileName; // [rsp+D0h] [rbp-18h]

  v11 = 0;
  pkPlayer = 0LL;
  v13 = 0LL;
  v14 = -1;
  dwMemberSerial = -1;
  v16 = -1;
  v17 = -1;
  m_sData = pData->m_sData;
  v14 = *(_DWORD *)&pData->m_sData[8];
  dwMemberSerial = *(_DWORD *)pData->m_sData;
  v16 = *(_DWORD *)&pData->m_sData[12];
  v17 = *(_DWORD *)&pData->m_sData[16];
  v19 = 0;
  pkPlayer = &g_Player[v14];
  if ( pkPlayer->m_bLive && pkPlayer->m_dwObjSerial == dwMemberSerial )
  {
    pkPlayer->m_Param.m_bGuildLock = 0;
    v19 = 1;
  }
  if ( !v19 )
    pkPlayer = 0LL;
  v19 = 0;
  v13 = &g_Guild[v16];
  if ( v13->IsFill() && v13->m_dwSerial == v17 )
    v19 = 1;
  if ( v19 )
  {
    if ( !pkPlayer )
      goto LABEL_22;
    if ( !pkPlayer->m_pUserDB )
      goto LABEL_22;
    if ( !pkPlayer->m_Param.m_pGuild )
      goto LABEL_22;
    m_pUserDB = pkPlayer->m_pUserDB;
    n = pkPlayer->m_ObjID.m_wIndex;
    m_pGuild = pkPlayer->m_Param.m_pGuild;
    Instance = CGuildRoomSystem::GetInstance();
    if (!Instance->IsGuildRoomMemberIn(m_pGuild->m_dwSerial, n, m_pUserDB->m_dwSerial))
      goto LABEL_22;
    v25 = pkPlayer->m_pUserDB;
    m_wIndex = pkPlayer->m_ObjID.m_wIndex;
    v27 = pkPlayer->m_Param.m_pGuild;
    v4 = CGuildRoomSystem::GetInstance();
    if (v4->SetPlayerOut(v27->m_dwSerial, m_wIndex, v25->m_dwSerial))
    {
      pkPlayer->SendMsg_GuildSelfLeaveResult( 0xFFu);
    }
    else
    {
LABEL_22:
      v5 = CGuildBattleController::Instance();
      v5->LeaveGuild(pkPlayer);
      if ( pkPlayer && pkPlayer->m_Param.m_byClassInGuild == 2 )
      {
        Grade = v13->GetGrade();
        v6 = CGuildMasterEffect::GetInstance();
        v6->out_player(pkPlayer, Grade);
      }
      v13->SendMsg_LeaveMember( dwMemberSerial, 1, 0);
      v13->PopMember( dwMemberSerial);
      if ( pkPlayer )
      {
        pkPlayer->m_Param.m_pGuild = 0LL;
        pkPlayer->SendMsg_GuildSelfLeaveResult( 0);
        pkPlayer->UpdateVisualVer(v21);
        pkPlayer->SendMsg_GuildJoinOtherInform();
        pkPlayer->SetLastAttBuff( 0);
        pszFileName = v13->m_szHistoryFileName;
        CharNameA = pkPlayer->m_Param.GetCharNameA();
        CGuild::s_MgrHistory.leave_member(
          CharNameA,
          pkPlayer->m_dwObjSerial,
          1,
          v13->m_nMemberNum,
          pszFileName,
          0);
      }
      if ( v13->GetMemberNum() <= 0 )
      {
        v20.in_guildserial = v13->m_dwSerial;
        v20.tmp_guildindex = v13->m_nIndex;
        v9 = v20.size();
        g_Main.PushDQSData(0xFFFFFFFF, 0LL, 0x16u, (char *)&v20, v9);
      }
    }
  }
  else if ( pkPlayer )
  {
    pkPlayer->SendMsg_GuildSelfLeaveResult( 0xFFu);
  }
}

void CPlayer::Guild_Force_Leave_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  CGuildBattleController *v3; // rax
  CGuildRoomSystem *Instance; // rax
  CGuildRoomSystem *v5; // rax
  CPlayer::CashChangeStateFlag *v6; // rax
  char *CharNameA; // rax
  __int64 v8; // [rsp+0h] [rbp-C8h] BYREF
  CGuild *v9; // [rsp+40h] [rbp-88h]
  unsigned int dwMemberSerial; // [rsp+48h] [rbp-80h]
  int v11; // [rsp+4Ch] [rbp-7Ch]
  int v12; // [rsp+50h] [rbp-78h]
  char *m_sData; // [rsp+58h] [rbp-70h]
  char v14; // [rsp+60h] [rbp-68h]
  _guild_member_info *MemberFromSerial; // [rsp+68h] [rbp-60h]
  CPlayer *pPlayer; // [rsp+70h] [rbp-58h]
  CPlayer::CashChangeStateFlag v17{0}; // [rsp+78h] [rbp-50h] BYREF
  CUserDB *m_pUserDB; // [rsp+80h] [rbp-48h]
  int n; // [rsp+88h] [rbp-40h]
  CGuild *m_pGuild; // [rsp+90h] [rbp-38h]
  CUserDB *v21; // [rsp+98h] [rbp-30h]
  int m_wIndex; // [rsp+A0h] [rbp-28h]
  CGuild *v23; // [rsp+A8h] [rbp-20h]
  char *pszFileName; // [rsp+B0h] [rbp-18h]
  int nMemNum; // [rsp+B8h] [rbp-10h]

  v9 = 0LL;
  dwMemberSerial = -1;
  v11 = -1;
  v12 = -1;
  m_sData = pData->m_sData;
  dwMemberSerial = *(_DWORD *)pData->m_sData;
  v11 = *(_DWORD *)&pData->m_sData[4];
  v12 = *(_DWORD *)&pData->m_sData[8];
  v14 = 0;
  v9 = &g_Guild[v11];
  if ( v9->IsFill() && v9->m_dwSerial == v12 )
    v14 = 1;
  if ( v14 )
  {
    MemberFromSerial = v9->GetMemberFromSerial( dwMemberSerial);
    if ( MemberFromSerial )
    {
      if ( MemberFromSerial->pPlayer )
      {
        v3 = CGuildBattleController::Instance();
        v3->LeaveGuild(MemberFromSerial->pPlayer);
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer->m_Param.m_pGuild )
        {
          m_pUserDB = pPlayer->m_pUserDB;
          n = pPlayer->m_ObjID.m_wIndex;
          m_pGuild = pPlayer->m_Param.m_pGuild;
          Instance = CGuildRoomSystem::GetInstance();
          if (Instance->IsGuildRoomMemberIn(m_pGuild->m_dwSerial, n, m_pUserDB->m_dwSerial))
          {
            v21 = pPlayer->m_pUserDB;
            m_wIndex = pPlayer->m_ObjID.m_wIndex;
            v23 = pPlayer->m_Param.m_pGuild;
            v5 = CGuildRoomSystem::GetInstance();
            v5->SetPlayerOut(v23->m_dwSerial, m_wIndex, v21->m_dwSerial);
          }
        }
        MemberFromSerial->pPlayer->m_Param.m_bGuildLock = 0;
        MemberFromSerial->pPlayer->m_Param.m_pGuild = 0LL;
        MemberFromSerial->pPlayer->m_Param.m_pGuildMemPtr = 0LL;
        MemberFromSerial->pPlayer->UpdateVisualVer(v17);
        MemberFromSerial->pPlayer->SendMsg_GuildJoinOtherInform();
        MemberFromSerial->pPlayer->SetLastAttBuff( 0);
        if ( m_sData[24] )
          MemberFromSerial->pPlayer->SendMsg_GuildForceLeaveBoradori();
        pszFileName = v9->m_szHistoryFileName;
        nMemNum = v9->m_nMemberNum - 1;
        CharNameA = MemberFromSerial->pPlayer->m_Param.GetCharNameA();
        CGuild::s_MgrHistory.leave_member(
          CharNameA,
          MemberFromSerial->dwSerial,
          0,
          nMemNum,
          pszFileName,
          m_sData[24]);
      }
      v9->SendMsg_LeaveMember( dwMemberSerial, 0, m_sData[24]);
      v9->PopMember( dwMemberSerial);
    }
  }
}

void CPlayer::Guild_Push_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int Dalant; // eax
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  int v5; // [rsp+50h] [rbp-B8h]
  int v6; // [rsp+54h] [rbp-B4h]
  unsigned int dwIOerSerial; // [rsp+58h] [rbp-B0h]
  char Destination[28]; // [rsp+68h] [rbp-A0h] BYREF
  unsigned int dwPush; // [rsp+84h] [rbp-84h]
  unsigned int dwPopDalant; // [rsp+88h] [rbp-80h]
  long double dTotalGold; // [rsp+90h] [rbp-78h]
  long double dTotalDalant; // [rsp+98h] [rbp-70h]
  unsigned __int8 pbyDate[20]; // [rsp+A4h] [rbp-64h] BYREF
  char *m_sData; // [rsp+B8h] [rbp-50h]
  CGuild *v15; // [rsp+C0h] [rbp-48h]
  _guild_member_info *MemberFromSerial; // [rsp+C8h] [rbp-40h]
  CPlayer *pPlayer; // [rsp+D0h] [rbp-38h]
  char *pszFileName; // [rsp+E0h] [rbp-28h]
  unsigned int dwLeftGold; // [rsp+E8h] [rbp-20h]

  v5 = -1;
  v6 = -1;
  dwIOerSerial = -1;
  dwPush = 0;
  dwPopDalant = 0;
  dTotalGold = 0.0;
  dTotalDalant = 0.0;
  m_sData = pData->m_sData;
  v5 = *(_DWORD *)&pData->m_sData[24];
  v6 = *(_DWORD *)&pData->m_sData[28];
  dwIOerSerial = *(_DWORD *)pData->m_sData;
  dwPopDalant = *(_DWORD *)&pData->m_sData[36];
  dwPush = *(_DWORD *)&pData->m_sData[32];
  dTotalDalant = *(long double *)&pData->m_sData[56];
  dTotalGold = *(long double *)&pData->m_sData[48];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(pbyDate, m_sData + 40, 4uLL);
  v15 = &g_Guild[v5];
  if ( v15->m_dwSerial == v6 )
  {
    v15->m_bIOWait = 0;
    if ( pData->m_byResult )
    {
      MemberFromSerial = v15->GetMemberFromSerial( dwIOerSerial);
      if ( MemberFromSerial )
      {
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer )
        {
          pPlayer->AddGold( dwPush, 1);
          pPlayer->AddDalant( dwPopDalant, 1);
          pszFileName = pPlayer->m_szItemHistoryFileName;
          dwLeftGold = pPlayer->m_Param.GetGold();
          Dalant = pPlayer->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.guild_pop_money_rollback(
            pPlayer->m_ObjID.m_wIndex,
            v15->m_aszName,
            dwPopDalant,
            dwPush,
            Dalant,
            dwLeftGold,
            pszFileName);
        }
      }
    }
    else
    {
      v15->m_byMoneyOutputKind = 0;
      
        v15->IOMoney(
        Destination,
        dwIOerSerial,
        (double)(int)dwPopDalant,
        (double)(int)dwPush,
        dTotalDalant,
        dTotalGold,
        pbyDate,
        1);
    }
  }
}

void CPlayer::Guild_Pop_Money_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  unsigned int Dalant; // eax
  __int64 v4; // [rsp+0h] [rbp-108h] BYREF
  __int64 dTotalDalant; // [rsp+28h] [rbp-E0h]
  __int64 dTotalGold; // [rsp+30h] [rbp-D8h]
  __int64 bInPut; // [rsp+40h] [rbp-C8h]
  __int64 v8; // [rsp+48h] [rbp-C0h]
  int v9; // [rsp+50h] [rbp-B8h]
  unsigned int v10; // [rsp+54h] [rbp-B4h]
  unsigned int dwIOerSerial; // [rsp+58h] [rbp-B0h]
  char Destination[28]; // [rsp+68h] [rbp-A0h] BYREF
  unsigned int dwPopGold; // [rsp+84h] [rbp-84h]
  unsigned int dwPush; // [rsp+88h] [rbp-80h]
  long double v15; // [rsp+90h] [rbp-78h]
  long double v16; // [rsp+98h] [rbp-70h]
  unsigned __int8 pbyDate[20]; // [rsp+A4h] [rbp-64h] BYREF
  char *m_sData; // [rsp+B8h] [rbp-50h]
  CGuild *v19; // [rsp+C0h] [rbp-48h]
  _guild_member_info *MemberFromSerial; // [rsp+C8h] [rbp-40h]
  CPlayer *pPlayer; // [rsp+D0h] [rbp-38h]
  char *pszFileName; // [rsp+E0h] [rbp-28h]
  unsigned int dwLeftGold; // [rsp+E8h] [rbp-20h]

  v9 = -1;
  v10 = -1;
  dwIOerSerial = -1;
  dwPopGold = 0;
  dwPush = 0;
  v15 = 0.0;
  v16 = 0.0;
  m_sData = pData->m_sData;
  v9 = *(_DWORD *)&pData->m_sData[24];
  v10 = *(_DWORD *)&pData->m_sData[28];
  dwIOerSerial = *(_DWORD *)pData->m_sData;
  dwPush = *(_DWORD *)&pData->m_sData[36];
  dwPopGold = *(_DWORD *)&pData->m_sData[32];
  v16 = *(double *)&pData->m_sData[56];
  v15 = *(double *)&pData->m_sData[48];
  strcpy_0(Destination, &pData->m_sData[4]);
  memcpy_0(pbyDate, m_sData + 40, 4uLL);
  v19 = &g_Guild[v9];
  if ( v19->m_dwSerial == v10 )
  {
    v19->m_bIOWait = 0;
	    if ( m_sData[64] )
	    {
	      const unsigned __int8 qryRet = static_cast<unsigned __int8>(m_sData[64]);
	      const unsigned int ioerSerialForLog = *reinterpret_cast<unsigned int *>(m_sData);
	      const unsigned int subGold = dwPopGold;
	      const unsigned int subDalant = dwPush;
	      g_Main.m_logSystemError.Write(
	        
	        "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
	        "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%u) SubG(%u) %s(%u)\r\n"
	        "\t\t_qry_case_outputgmoney Ret(%u) Fail!",
	        v10,
	        v16,
	        v15,
	        subDalant,
	        subGold,
	        Destination,
	        ioerSerialForLog,
	        qryRet);
	    }
    else if ( !pData->m_byResult )
    {
      
        v19->IOMoney(
        Destination,
        dwIOerSerial,
        -0.0 - (double)(int)dwPush,
        -0.0 - (double)(int)dwPopGold,
        v16,
        v15,
        pbyDate,
        0);
      MemberFromSerial = v19->GetMemberFromSerial( dwIOerSerial);
      if ( MemberFromSerial )
      {
        pPlayer = MemberFromSerial->pPlayer;
        if ( pPlayer )
        {
          pPlayer->AddDalant( dwPush, 1);
          pPlayer->AddGold( dwPopGold, 1);
          pszFileName = pPlayer->m_szItemHistoryFileName;
          dwLeftGold = pPlayer->m_Param.GetGold();
          Dalant = pPlayer->m_Param.GetDalant();
          CPlayer::s_MgrItemHistory.guild_pop_money(
            pPlayer->m_ObjID.m_wIndex,
            v19->m_aszName,
            dwPush,
            dwPopGold,
            Dalant,
            dwLeftGold,
            pszFileName);
          pPlayer->SendMsg_AlterMoneyInform( 0);
        }
        else
        {
          if (dwPush)
            g_Main.Push_ChargeItem(dwIOerSerial, 0xFFFFFFFF, dwPush, 0xFFFFFFFu, 1u);
          if (dwPopGold)
            g_Main.Push_ChargeItem(dwIOerSerial, 0xFFFFFFFF, dwPopGold, 0xFFFFFFFu, 2u);
          g_Main.m_logSystemError.Write(
            "CPlayer::Guild_Pop_Money_Complete(...) : \r\n"
            "\t\tPush Charge Money BECAUSE Poper Connection Closed : Poper Serial(%u) Pop Dalant(%u) Pop Gold(%u)\r\n",
            dwIOerSerial,
            dwPush,
            dwPopGold);
        }
      }
    }
  }
}

void CPlayer::Guild_Buy_Emblem_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-F8h] BYREF
  __int64 dTotalDalant; // [rsp+28h] [rbp-D0h]
  unsigned __int8 *pbyDate; // [rsp+38h] [rbp-C0h]
  bool bInPut[16]; // [rsp+40h] [rbp-B8h]
  int v7; // [rsp+50h] [rbp-A8h]
  unsigned int v8; // [rsp+54h] [rbp-A4h]
  unsigned int dwEmblemBack; // [rsp+58h] [rbp-A0h]
  unsigned int dwEmblemMark; // [rsp+5Ch] [rbp-9Ch]
  unsigned int dwIOerSerial; // [rsp+60h] [rbp-98h]
  char Destination[28]; // [rsp+78h] [rbp-80h] BYREF
  int v13; // [rsp+94h] [rbp-64h]
  int v14; // [rsp+98h] [rbp-60h]
  long double dTotalGold; // [rsp+A0h] [rbp-58h]
  long double v16; // [rsp+A8h] [rbp-50h]
  unsigned __int8 v17[20]; // [rsp+B4h] [rbp-44h] BYREF
  char *m_sData; // [rsp+C8h] [rbp-30h]
  CGuild *v19; // [rsp+D0h] [rbp-28h]

  v7 = -1;
  v8 = -1;
  dwEmblemBack = -1;
  dwEmblemMark = -1;
  dwIOerSerial = -1;
  v13 = 0;
  v14 = 0;
  dTotalGold = 0.0;
  v16 = 0.0;
  memset(v17, 0, 4);
  m_sData = pData->m_sData;
  v7 = *(_DWORD *)&pData->m_sData[24];
  v8 = *(_DWORD *)pData->m_sData;
  dwEmblemBack = *(_DWORD *)&pData->m_sData[4];
  dwEmblemMark = *(_DWORD *)&pData->m_sData[8];
  dwIOerSerial = *(_DWORD *)&pData->m_sData[16];
  v14 = *(_DWORD *)&pData->m_sData[12];
  v16 = *(double *)&pData->m_sData[56];
  dTotalGold = *(double *)&pData->m_sData[48];
  memcpy_0(v17, &pData->m_sData[20], 4uLL);
  strcpy_0(Destination, m_sData + 28);
  v19 = &g_Guild[v7];
  if ( v19->m_dwSerial == v8 )
  {
    v19->m_bIOWait = 0;
	    if ( m_sData[64] )
	    {
	      const unsigned __int8 qryRet = static_cast<unsigned __int8>(m_sData[64]);
	      const unsigned int ioerSerialForLog = *((_DWORD *)m_sData + 4);
	      const int subDalant = v14;
	      g_Main.m_logSystemError.Write(
	        
	        "CPlayer::Guild_Buy_Emblem_Complete(...) : \r\n"
	        "\t\tGuild(%u) TotD(%f) TotG(%f) SubD(%d) %s(%u)\r\n"
	        "\t\tqry_case_buyemblem Ret(%u) Fail!",
	        v8,
	        v16,
	        dTotalGold,
	        subDalant,
	        m_sData + 28,
	        ioerSerialForLog,
	        qryRet);
	    }
    else if ( !pData->m_byResult )
    {
      v19->m_byMoneyOutputKind = 1;
      v19->IOMoney( Destination, dwIOerSerial, (double)v14, 0.0, v16, dTotalGold, v17, 0);
      v19->UpdateEmblem( dwEmblemBack, dwEmblemMark);
    }
  }
}

void CPlayer::Guild_Disjoint_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-48h] BYREF
  int v4; // [rsp+20h] [rbp-28h]
  int v5; // [rsp+24h] [rbp-24h]
  char *m_sData; // [rsp+28h] [rbp-20h]
  CGuild *v7; // [rsp+30h] [rbp-18h]

  v4 = -1;
  v5 = -1;
  m_sData = pData->m_sData;
  v4 = *(_DWORD *)&pData->m_sData[4];
  v5 = *(_DWORD *)pData->m_sData;
  v7 = &g_Guild[v4];
  if ( v7 && v7->m_dwSerial == v5 && !pData->m_byResult )
  {
    if ( v7->m_nApplierNum > 0 )
      v7->SendMsg_GuildDisjointInform();
    v7->Release();
  }
}

void CPlayer::Guild_Update_GuildMater_Complete(_DB_QRY_SYN_DATA *pData)
{
  __int64 *v1; // rdi
  __int64 i; // rcx
  __int64 v3; // [rsp+0h] [rbp-68h] BYREF
  int v4; // [rsp+30h] [rbp-38h]
  int v5; // [rsp+34h] [rbp-34h]
  unsigned int in_guild_prev_masterSerial; // [rsp+38h] [rbp-30h]
  unsigned __int8 v7; // [rsp+3Ch] [rbp-2Ch]
  unsigned int in_guild_new_masterSerial; // [rsp+40h] [rbp-28h]
  unsigned __int8 in_guild_new_masterPrevGrade; // [rsp+44h] [rbp-24h]
  char *m_sData; // [rsp+48h] [rbp-20h]
  CGuild *v11; // [rsp+50h] [rbp-18h]

  v4 = -1;
  v5 = -1;
  m_sData = pData->m_sData;
  v4 = *(_DWORD *)&pData->m_sData[4];
  v5 = *(_DWORD *)pData->m_sData;
  in_guild_prev_masterSerial = *(_DWORD *)&pData->m_sData[8];
  v7 = pData->m_sData[12];
  in_guild_new_masterSerial = *(_DWORD *)&pData->m_sData[16];
  in_guild_new_masterPrevGrade = pData->m_sData[20];
  v11 = &g_Guild[v4];
  if ( v11 && v11->m_dwSerial == v5 && !pData->m_byResult )
    
      v11->DB_Update_GuildMaster_Complete(
      in_guild_prev_masterSerial,
      v7,
      in_guild_new_masterSerial,
      in_guild_new_masterPrevGrade);
}
