#include "pch.h"

#include "CPlayer.h"
#include "cStaticMember_Player.h"
#include "CRecordData.h"
#include "CGuild.h"
#include "CPlayerDB.h"
#include "CMainThread.h"
#include "CMapOperation.h"
#include "CMapData.h"
#include "pnt_rect.h"
#include "CMonster.h"
#include "CMerchant.h"
#include "CAnimus.h"
#include "CTrap.h"
#include "CUnmannedTraderController.h"
#include "CMgrAvatorItemHistory.h"
#include "COreAmountMgr.h"
#include "CGravityStoneRegener.h"
#include "CGravityStone.h"
#include "CPcBangFavor.h"
#include "CPvpUserAndGuildRankingSystem.h"
#include "CPvpPointLimiter.h"
#include "CPvpOrderView.h"
#include "CPvpCashPoint.h"
#include "CGameStatistics.h"
#include "CTransportShip.h"
#include "CMoneySupplyMgr.h"
#include "CLogFile.h"
#include "CItemStore.h"
#include "CItemStoreManager.h"
#include "CMapItemStoreList.h"
#include "StoreList_fld.h"
#include "CDarkHoleChannel.h"
#include "WorldServerUtil.h"
#include "GlobalObjects.h"
#include "ITEM_EFFECT.h"
#include "BatteryItem_fld.h"
#include "EditData_fld.h"
#include "GuardTowerItem_fld.h"
#include "ItemUpgrade_fld.h"
#include "Quest_fld.h"
#include "TimeItem.h"
#include "UnitFrame_fld.h"
#include "WeaponItem_fld.h"
#include "force_fld.h"
#include "skill_fld.h"

#include <mmsystem.h>

char loot_item(CPlayer *pOwner, char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum);

static float fPos[3];
static float fTarPos[3];

bool CPlayer::dev_after_effect()
{
  _skill_fld *record = static_cast<_skill_fld *>(g_Main.m_tblEffectData[3].GetRecord("17"));
  if (!record)
  {
    return false;
  }

  int unusedFlag = 1;
  unsigned int curTime = _sf_continous::GetSFContCurTime();
  unsigned __int16 durSec = static_cast<unsigned __int16>(record->m_nContEffectSec[0]);
  bool upMty = false;
  if (!this->InsertSFContEffect(
        static_cast<unsigned __int8>(record->m_nContEffectType),
        3,
        record->m_dwIndex,
        durSec,
        1,
        &upMty,
        nullptr))
  {
    this->m_bAfterEffect = 1;
  }
return true;
}

bool CPlayer::dev_all_kill()
{
  _sec_info *secInfo = this->m_pCurMap->GetSecInfo();
  const int curSecNum = this->GetCurSecNum();
  _pnt_rect rect{};
  this->m_pCurMap->GetRectInRadius(&rect, 1, curSecNum);

  for (int y = rect.nStarty; y <= rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x <= rect.nEndx; ++x)
    {
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = this->m_pCurMap->GetSectorListObj(this->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *object = node->m_pItem;
        if (!object)
        {
          continue;
        }
        if (!object->m_ObjID.m_byKind && object->m_ObjID.m_byID == 1)
        {
          const int hp = object->GetHP();
          const int level = this->GetLevel();
          object->SetDamage(hp, this, level, false, -1, 0, true);
        }
      }
    }
  }

  return true;
}

bool CPlayer::dev_animus_recall_time_free(bool bFree)
{
  if (this->m_bFreeRecallWaitTime == bFree)
  {
    return false;
  }

  this->m_bFreeRecallWaitTime = bFree;
  _animus_recall_wait_time_free_inform_zocl msg{};
  msg.bFree = bFree;

  unsigned __int8 pbyType[2]{22, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return true;
}

bool CPlayer::dev_avator_copy(char *pwszDstName)
{
  if (!this->m_pUserDB)
  {
    return false;
  }

  CUserDB *dstUser = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszDstName);
  if (!dstUser)
  {
    return false;
  }

  CPlayer *dstPlayer = &g_Player[dstUser->m_idWorld.wIndex];
  if (!dstPlayer->m_bLive)
  {
    return false;
  }

  const int dstRaceSex = static_cast<int>(dstPlayer->m_Param.GetRaceSexCode());
  const int srcRaceSex = static_cast<int>(this->m_Param.GetRaceSexCode());
  if (dstRaceSex != srcRaceSex)
  {
    return false;
  }

  this->m_pUserDB->Update_CopyAll(&dstUser->m_AvatorData);
  CPlayer::s_MgrItemHistory.char_copy(
    this->m_ObjID.m_wIndex,
    dstUser->m_aszAvatorName,
    dstUser->m_dwSerial,
    this->m_szItemHistoryFileName);
  CPlayer::s_MgrLvHistory.char_copy(
    this->m_ObjID.m_wIndex,
    dstUser->m_aszAvatorName,
    dstUser->m_dwSerial,
    this->m_szLvHistoryFileName);
  g_Network.Close(0, this->m_ObjID.m_wIndex, false, nullptr);
  return true;
}

bool CPlayer::dev_change_class(char *pszClassCode)
{
  return this->m_pUserDB && this->m_pUserDB->Setting_Class(pszClassCode);
}

bool CPlayer::dev_cont_effect_del()
{
  for (unsigned __int8 cont = 0; cont < 2; ++cont)
  {
    for (unsigned __int16 idx = 0; idx < 8; ++idx)
    {
      if (this->m_SFContAura[cont][idx].m_bExist)
      {
        this->RemoveSFContEffect(cont, idx, false, true);
      }
      if (this->m_SFCont[cont][idx].m_bExist)
      {
        this->RemoveSFContEffect(cont, idx, false, false);
      }
    }
  }
  return true;
}

bool CPlayer::dev_cont_effect_time(unsigned int dwSec)
{

    if ( dwSec )
    {
      if ( dwSec <= 9000 )
        this->m_nContEffectSec = dwSec;
      else
        this->m_nContEffectSec = 9000;
    }
    else
    {
      this->m_nContEffectSec = -1;
    }
    return true;
}

bool CPlayer::dev_dalant(unsigned int dwDalant)
{
  unsigned int dalant = dwDalant;
  if (dwDalant == static_cast<unsigned int>(-1))
  {
    dalant = MAX_DALANT;
  }
  else if (dwDalant > MAX_DALANT)
  {
    dalant = MAX_DALANT;
  }

  this->m_Param.SetDalant(dalant);
  const unsigned int gold = this->m_Param.GetGold();
  const unsigned int curDalant = this->m_Param.GetDalant();
  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_Money(curDalant, gold);
  }
  this->SendMsg_ExchangeMoneyResult(0);

  const unsigned int newGold = this->m_Param.GetGold();
  const unsigned int newDalant = this->m_Param.GetDalant();
  CPlayer::s_MgrItemHistory.cheat_alter_money(
    this->m_ObjID.m_wIndex,
    newDalant,
    newGold,
    this->m_szItemHistoryFileName);
  return true;
}

bool CPlayer::dev_die()
{
  this->m_nLastBeatenPart = 0;
  if (this->IsRidingUnit())
  {
    const int damage = static_cast<int>(this->m_pUsingUnit->dwGauge);
    this->SetDamage(damage, nullptr, 0, false, -1, 0, true);
  }
  else
  {
    const int hp = static_cast<int>(this->m_Param.GetHP());
    this->SetDamage(hp, nullptr, 0, false, -1, 0, true);
  }
  return true;
}

bool CPlayer::dev_drop_item(char *pszItemCode, int nNum, char *pszUpTalCode)
{
  _STORAGE_LIST::_db_con items[100];
  unsigned __int8 itemCount = 0;

  const int tableCode = GetItemTableCode(pszItemCode);
  if (tableCode == -1 || tableCode != 18)
  {
    return false;
  }

  for (int j = 0;; ++j)
  {
    const unsigned __int8 bagNum = this->m_Param.GetBagNum();
    if (j >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *src = &this->m_Param.m_dbInven.m_pStorageList[j];
    if (!src->m_bLoad)
    {
      continue;
    }
    if (src->m_byTableCode != 18)
    {
      continue;
    }

    _base_fld *record = g_Main.m_tblItemData[src->m_byTableCode].GetRecord( src->m_wItemIndex);
    if (!record)
    {
      continue;
    }

    if (!std::strcmp(record->m_strCode, pszItemCode))
    {
      this->SendMsg_DeleteStorageInform(0, src->m_wSerial);
      this->Emb_DelStorage(0, j, 0, 1, "CPlayer::dev_inven_empty()");
    }
    std::memcpy(&items[itemCount++], src, sizeof(_STORAGE_LIST::_db_con));
  }

  CPlayer::s_MgrItemHistory.cheat_del_item(
    this->m_ObjID.m_wIndex,
    items,
    itemCount,
    this->m_szItemHistoryFileName);
  return true;
}

bool CPlayer::dev_free_sf_by_class()
{

    this->m_bFreeSFByClass = 1;
    return true;
}

bool CPlayer::dev_full_animus_gauge()
{
  for (int j = 0; j < 4; ++j)
  {
    _STORAGE_LIST::_db_con *animus = &this->m_Param.m_dbAnimus.m_pStorageList[j];
    if (!animus->m_bLoad || animus->m_bLock)
    {
      continue;
    }

    const int animusClass = animus->m_wItemIndex;
    _animus_fld *animusFld = GetAnimusFldFromExp(animusClass, animus->m_dwDur);
    if (!animusFld)
    {
      return false;
    }

    unsigned int *levelPtr = &animus->m_dwLv;
    *reinterpret_cast<unsigned __int16 *>(levelPtr) = animusFld->m_nMaxHP;
    *(reinterpret_cast<unsigned __int16 *>(levelPtr) + 1) = animusFld->m_nMaxFP;

    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_ItemUpgrade(4u, animus->m_byStorageIndex, animus->m_dwLv, 0);
    }

    unsigned __int8 typeHp[2]{22, 9};
    unsigned __int8 typeFp[2]{22, 10};
    _animus_hp_inform_zocl msgHp{};
    msgHp.wAnimusItemSerial = animus->m_wSerial;
    msgHp.wLeftHP = *reinterpret_cast<unsigned __int16 *>(levelPtr);
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      typeHp,
      reinterpret_cast<char *>(&msgHp),
      static_cast<unsigned __int16>(sizeof(msgHp)));

    _animus_fp_inform_zocl msgFp{};
    msgFp.wAnimusItemSerial = animus->m_wSerial;
    msgFp.wLeftFP = *(reinterpret_cast<unsigned __int16 *>(levelPtr) + 1);
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      typeFp,
      reinterpret_cast<char *>(&msgFp),
      static_cast<unsigned __int16>(sizeof(msgFp)));
  }
  return true;
}

bool CPlayer::dev_full_force()
{
  _STORAGE_LIST::_db_con items[88];
  unsigned __int8 count = 0;

  const int recordNum = g_Main.m_tblItemData[15].GetRecordNum();
  for (int n = 0; n < recordNum; ++n)
  {
    _base_fld *record = g_Main.m_tblItemData[15].GetRecord(n);
    if (!this->m_Param.m_dbForce.GetNumEmptyCon())
    {
      break;
    }

    bool alreadyHave = false;
    for (int j = 0; j < 88; ++j)
    {
      _STORAGE_LIST::_db_con *force = &this->m_Param.m_dbForce.m_pStorageList[j];
      if (force->m_bLoad
          && CPlayer::s_pnLinkForceItemToEffect[force->m_wItemIndex]
               == CPlayer::s_pnLinkForceItemToEffect[record->m_dwIndex])
      {
        alreadyHave = true;
        break;
      }
    }

    if (!alreadyHave)
    {
      _STORAGE_LIST::_db_con src;
      src.m_byTableCode = 15;
      src.m_wItemIndex = static_cast<unsigned __int16>(n);
      src.m_dwDur = 16777215;
      src.m_wSerial = this->m_Param.GetNewItemSerial();
      src.m_dwLv = 268435455;
      if (this->Emb_AddStorage(3u, &src, 1, 1))
      {
        std::memcpy(&items[count++], &src, sizeof(_STORAGE_LIST::_db_con));
      }
    }
  }

  CPlayer::s_MgrItemHistory.cheat_add_item(
    this->m_ObjID.m_wIndex,
    items,
    count,
    this->m_szItemHistoryFileName);
  return count != 0;
}

bool CPlayer::dev_full_point()
{
  const int maxHp = this->GetMaxHP();
  this->SetHP(maxHp, false);
  const int maxFp = this->GetMaxFP();
  this->SetFP(maxFp, false);
  const int maxSp = this->GetMaxSP();
  this->SetSP(maxSp, false);
  this->SendMsg_Recover();
  return true;
}

bool CPlayer::dev_gold(unsigned int dwGold)
{
  unsigned int gold = dwGold;
  if (dwGold == static_cast<unsigned int>(-1))
  {
    gold = MAX_GOLD;
  }
  else if (dwGold > MAX_GOLD)
  {
    gold = MAX_GOLD;
  }

  this->m_Param.SetGold(gold);
  const unsigned int curGold = this->m_Param.GetGold();
  const unsigned int curDalant = this->m_Param.GetDalant();
  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_Money(curDalant, curGold);
  }
  this->SendMsg_ExchangeMoneyResult(0);

  const unsigned int newGold = this->m_Param.GetGold();
  const unsigned int newDalant = this->m_Param.GetDalant();
  CPlayer::s_MgrItemHistory.cheat_alter_money(
    this->m_ObjID.m_wIndex,
    newDalant,
    newGold,
    this->m_szItemHistoryFileName);
  return true;
}

bool CPlayer::dev_goto_monster(CMonster *pMon)
{
  if ( !pMon || !pMon->m_bLive || !pMon->m_bOper )
      return false;
    this->OutOfMap( pMon->m_pCurMap, pMon->m_wMapLayerIndex, 3u, pMon->m_fCurPos);
    float *targetPos = pMon->m_fCurPos;
    const unsigned __int8 mapCode = this->m_Param.GetMapCode();
    this->SendMsg_GotoRecallResult(0, mapCode, targetPos, 4u);
    return true;
}

bool CPlayer::dev_goto_npc(CMerchant *pNpc)
{
  if ( !pNpc || !pNpc->m_bLive || pNpc->m_bCorpse )
      return false;
    this->OutOfMap( pNpc->m_pCurMap, pNpc->m_wMapLayerIndex, 3u, pNpc->m_fCurPos);
    float *targetPos = pNpc->m_fCurPos;
    const unsigned __int8 mapCode = this->m_Param.GetMapCode();
    this->SendMsg_GotoRecallResult(0, mapCode, targetPos, 4u);
    return true;
}

bool CPlayer::dev_half_inven_amount(unsigned __int64 dwAmount)
{
  for (int j = 0;; ++j)
  {
    const unsigned __int8 bagNum = this->m_Param.GetBagNum();
    if (j >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *item = &this->m_Param.m_dbInven.m_pStorageList[j];
    if (!item->m_bLoad)
    {
      continue;
    }
    if (item->m_bLock)
    {
      continue;
    }
    if (!IsAbrItem(item->m_byTableCode, item->m_wItemIndex))
    {
      continue;
    }

    unsigned __int64 amount = dwAmount;
    if (IsOverLapItem(item->m_byTableCode) && amount > MAX_ITEM_OVERLAP)
    {
      amount = MAX_ITEM_OVERLAP;
    }

    item->m_dwDur = amount;
    this->SendMsg_AlterItemDurInform(0, item->m_wSerial, amount);
    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_ItemDur(0, j, amount, true);
    }
  }
  return true;
}

bool CPlayer::dev_half_point()
{
  const int hp = this->GetHP();
  this->SetHP(hp / 2, true);
  const int fp = this->GetFP();
  this->SetFP(fp / 2, true);
  const int sp = this->GetSP();
  this->SetSP(sp / 2, true);
  const int dp = this->GetDP();
  this->SetDP(dp / 2, true);
  this->SendMsg_Recover();
  return true;
}

bool CPlayer::dev_init_monster()
{
  g_Main.gm_MonsterInit(static_cast<CMonster *>(this->m_TargetObject.pObject));
  return true;
}

bool CPlayer::dev_inven_empty()
{
  _STORAGE_LIST::_db_con items[100];
  unsigned __int8 count = 0;

  for (int j = 0;; ++j)
  {
    const unsigned __int8 bagNum = this->m_Param.GetBagNum();
    if (j >= 20 * bagNum)
    {
      break;
    }

    _STORAGE_LIST::_db_con *src = &this->m_Param.m_dbInven.m_pStorageList[j];
    if (!src->m_bLoad)
    {
      continue;
    }
    if (src->m_bLock || src->m_byTableCode == 19)
    {
      continue;
    }

    this->SendMsg_DeleteStorageInform(0, src->m_wSerial);
    if (this->Emb_DelStorage(0, j, 0, 1, "CPlayer::dev_inven_empty()"))
    {
      std::memcpy(&items[count++], src, sizeof(_STORAGE_LIST::_db_con));
    }
  }

  CPlayer::s_MgrItemHistory.cheat_del_item(
    this->m_ObjID.m_wIndex,
    items,
    count,
    this->m_szItemHistoryFileName);
  return true;
}

bool CPlayer::dev_item_make_no_use_matrial(bool noUsingMatrial)
{
  if (this->m_bCheat_makeitem_no_use_matrial == noUsingMatrial)
  {
    return false;
  }

  this->m_bCheat_makeitem_no_use_matrial = noUsingMatrial;
  this->SendMsg_MakeItemCheatSendButtonEnable(this->m_bCheat_makeitem_no_use_matrial);
  return true;
}

bool CPlayer::dev_loot_bag()
{
    int itemIndex; // [rsp+30h] [rbp-28h]
    char *itemEquipCivil; // [rsp+38h] [rbp-20h]
    _base_fld *itemRecord; // [rsp+40h] [rbp-18h]
  for ( itemIndex = 0; ; ++itemIndex )
    {
      const int recordCount = g_Main.m_tblItemData[12].GetRecordNum();
      if ( itemIndex >= recordCount )
        break;
      itemEquipCivil = GetItemEquipCivil(12, itemIndex);
      const int raceSexCode = static_cast<int>(this->m_Param.GetRaceSexCode());
      if ( itemEquipCivil[raceSexCode] == '1' )
      {
        itemRecord = g_Main.m_tblItemData[12].GetRecord( itemIndex);
        loot_item(this, itemRecord->m_strCode, 4, 0LL, 0);
        return true;
      }
    }
    return true;
}

bool CPlayer::dev_loot_free(bool bFree)
{

    if ( this->m_bLootFree == bFree )
      return false;
    this->m_bLootFree = bFree;
    return true;
}

bool CPlayer::dev_loot_fullitem(unsigned __int8 byLv)
{
    char Buffer[32]; // [rsp+38h] [rbp-50h] BYREF
    _EditData_fld *editRecord; // [rsp+58h] [rbp-30h]
    int nodeIndex; // [rsp+60h] [rbp-28h]
  memset(Buffer, 0, 9);
    sprintf_s(Buffer, "%s_%d", this->m_Param.m_pClassData->m_strCode, byLv);
    editRecord = static_cast<_EditData_fld *>(g_Main.m_tblEditData.GetRecord(Buffer));
    if ( !editRecord )
      return false;
    for ( nodeIndex = 0; nodeIndex < 30; ++nodeIndex )
    {
      if (editRecord->m_Node[nodeIndex].m_nEditItemcount > 0)
        this->dev_loot_item(
          editRecord->m_Node[nodeIndex].m_strEditItemCode,
          editRecord->m_Node[nodeIndex].m_nEditItemcount,
          0LL,
          0);
    }
    return true;
}

bool CPlayer::dev_loot_item(char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum)
{
  return loot_item(this, pszItemCode, nNum, pszUpTalCode, nUpNum);
}

bool CPlayer::dev_loot_material()
{
  int recordIndex = 0;
  _STORAGE_LIST::_db_con materialItem{};
  _STORAGE_LIST::_db_con upgradeItem{};
  _STORAGE_LIST::_db_con resourceItem{};
  for (recordIndex = 0; ; ++recordIndex)
  {
    const int recordCount = g_Main.m_tblItemData[20].GetRecordNum();
    if (recordIndex >= recordCount)
      break;
    materialItem.Init();
    materialItem.m_byTableCode = 20;
    materialItem.m_wItemIndex = recordIndex;
    materialItem.m_dwDur = 99LL;
    materialItem.m_dwLv = 268435455;
    if (!CreateItemBox(
          &materialItem,
          this,
          -1,
          0,
          0LL,
          2u,
          this->m_pCurMap,
          this->m_wMapLayerIndex,
          this->m_fCurPos,
          1))
      return true;
  }
  for (recordIndex = 0; ; ++recordIndex)
  {
    const int recordCount = g_Main.m_tblItemData[18].GetRecordNum();
    if (recordIndex >= recordCount)
      break;
    upgradeItem.Init();
    upgradeItem.m_byTableCode = 18;
    upgradeItem.m_wItemIndex = recordIndex;
    upgradeItem.m_dwDur = 99LL;
    upgradeItem.m_dwLv = 268435455;
    if (!CreateItemBox(
          &upgradeItem,
          this,
          -1,
          0,
          0LL,
          2u,
          this->m_pCurMap,
          this->m_wMapLayerIndex,
          this->m_fCurPos,
          1))
      return true;
  }
  for (recordIndex = 0; ; ++recordIndex)
  {
    const int recordCount = g_Main.m_tblItemData[11].GetRecordNum();
    if (recordIndex >= recordCount)
      break;
    resourceItem.Init();
    resourceItem.m_byTableCode = 11;
    resourceItem.m_wItemIndex = recordIndex;
    resourceItem.m_dwDur = GetItemDurPoint(11, recordIndex);
    resourceItem.m_dwLv = 268435455;
    if (!CreateItemBox(
          &resourceItem,
          this,
          -1,
          0,
          0LL,
          2u,
          this->m_pCurMap,
          this->m_wMapLayerIndex,
          this->m_fCurPos,
          1))
      return true;
  }
  return true;
}

bool CPlayer::dev_loot_mine()
{
  for (int recordIndex = 0; ; ++recordIndex)
  {
    const int recordCount = g_Main.m_tblItemData[6].GetRecordNum();
    if (recordIndex >= recordCount)
      break;
    _WeaponItem_fld *weaponRecord = static_cast<_WeaponItem_fld *>(g_Main.m_tblItemData[6].GetRecord(recordIndex));
    if (weaponRecord && weaponRecord->m_bExist && weaponRecord->m_nType == 10)
    {
      char *equipCivil = GetItemEquipCivil(6, recordIndex);
      const int raceSexCode = static_cast<int>(this->m_Param.GetRaceSexCode());
      if (equipCivil[raceSexCode] == '1')
        loot_item(this, weaponRecord->m_strCode, 1, 0LL, 0);
    }
  }
  for (int recordIndex = 0; ; ++recordIndex)
  {
    const int recordCount = g_Main.m_tblItemData[16].GetRecordNum();
    if (recordIndex >= recordCount)
      break;
    _BatteryItem_fld *batteryRecord = static_cast<_BatteryItem_fld *>(g_Main.m_tblItemData[16].GetRecord(recordIndex));
    if (batteryRecord && batteryRecord->m_bExist)
    {
      char *equipCivil = GetItemEquipCivil(16, recordIndex);
      if (equipCivil)
      {
        const int raceSexCode = static_cast<int>(this->m_Param.GetRaceSexCode());
        if (equipCivil[raceSexCode] == '1')
          loot_item(this, batteryRecord->m_strCode, 1, 0LL, 0);
      }
    }
  }
  return true;
}

bool CPlayer::dev_loot_tower()
{
  const int recordNum = g_Main.m_tblItemData[25].GetRecordNum();
  for (int n = 0; n < recordNum; ++n)
  {
    _GuardTowerItem_fld *record = static_cast<_GuardTowerItem_fld *>(g_Main.m_tblItemData[25].GetRecord(n));
    if (!record || !IsExistItem(25, n) || !IsGroundableItem(25, n))
    {
      continue;
    }

    _STORAGE_LIST::_db_con item{};
    item.Init();
    item.m_byTableCode = 25;
    item.m_wItemIndex = static_cast<unsigned __int16>(n);
    item.m_dwDur = GetItemDurPoint(25, n);
    item.m_dwLv = 268435455;
    if (!CreateItemBox(
          &item,
          this,
          -1,
          0,
          nullptr,
          2u,
          this->m_pCurMap,
          this->m_wMapLayerIndex,
          this->m_fCurPos,
          1))
    {
      return true;
    }

    for (int j = 0; j < 3 && loot_item(this, record->m_Material[j].strMaterialCode, 1, nullptr, 0); ++j)
    {
    }
  }

  return true;
}

bool CPlayer::dev_lv(int nLv)
{
  if (nLv <= 0)
  {
    return false;
  }

  const int currentLevel = static_cast<int>(this->m_Param.GetLevel());
  if (nLv <= currentLevel)
  {
    if (nLv < currentLevel)
    {
      this->m_Param.SetExp(0.0);
      this->m_dwExpRate = 0;
      this->SetLevelD(static_cast<unsigned __int8>(nLv));
    }
  }
  else
  {
    this->m_Param.SetExp(0.0);
    this->m_dwExpRate = 0;
    this->SetLevel(static_cast<unsigned __int8>(nLv));
  }

  int newMaxLevel = nLv;
  _base_fld *happenRecord = nullptr;
  while (true)
  {
    cStaticMember_Player *staticPlayer = cStaticMember_Player::Instance();
    const int maxLevel = staticPlayer ? staticPlayer->GetMaxLv() : 0;
    if (newMaxLevel >= maxLevel)
    {
      break;
    }

    const char *levelStr = cvt_string(newMaxLevel);
    CRecordData *happenTable = CQuestMgr::s_tblQuestHappenEvent + 8;
    happenRecord = happenTable->GetRecord(levelStr);
    if (happenRecord)
    {
      break;
    }
    ++newMaxLevel;
  }

  if (!happenRecord)
  {
    cStaticMember_Player *staticPlayer = cStaticMember_Player::Instance();
    if (staticPlayer)
    {
      newMaxLevel = staticPlayer->GetMaxLv();
    }
  }

  this->m_Param.SetMaxLevel(static_cast<unsigned __int8>(newMaxLevel));
  if (this->m_pUserDB)
  {
    this->m_pUserDB->Update_MaxLevel(static_cast<unsigned __int8>(newMaxLevel));
  }
  return true;
}

bool CPlayer::dev_make_succ(bool bSucc)
{

    if ( this->m_bCheat_100SuccMake == bSucc )
      return false;
    this->m_bCheat_100SuccMake = bSucc;
    return true;
}

bool CPlayer::dev_max_level_ext(unsigned __int8 byMaxLevel)
{
  this->m_Param.SetMaxLevel( byMaxLevel);
    if ( this->m_pUserDB )
      this->m_pUserDB->Update_MaxLevel( byMaxLevel);
    const int maxLevel = byMaxLevel;
    const int currentLevel = this->m_Param.GetLevel();
    if ( maxLevel < currentLevel )
    {
      this->m_Param.SetExp( 0.0);
      this->m_dwExpRate = 0;
      if ( byMaxLevel )
        this->SetLevelD( byMaxLevel);
      else
        this->SetLevelD( 50);
    }
    return true;
}

bool CPlayer::dev_never_die(bool bSet)
{

    if ( this->m_bNeverDie == bSet )
      return false;
    this->m_bNeverDie = bSet;
    return true;
}

bool CPlayer::dev_quest_complete()
{
    int slotIndex; // [rsp+20h] [rbp-28h]
    _QUEST_DB_BASE::_LIST *questSlot; // [rsp+28h] [rbp-20h]
    int objectiveIndex; // [rsp+30h] [rbp-18h]
    _Quest_fld *questRecord; // [rsp+38h] [rbp-10h]
  if ( !this->m_pUserDB )
      return false;
    for ( slotIndex = 0; ; ++slotIndex )
    {
      if ( slotIndex >= 30 )
        return false;
      questSlot = &this->m_Param.m_QuestDB.m_List[slotIndex];
      if ( questSlot->byQuestType != 255 )
        break;
    }
    for ( objectiveIndex = 0; objectiveIndex < 3; ++objectiveIndex )
      questSlot->wNum[objectiveIndex] = -1;
    questSlot->dwPassSec = -1;
    this->m_pUserDB->Update_QuestUpdate( slotIndex, questSlot, 1);
    questRecord = static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(questSlot->wIndex));
    if (questRecord->m_bSelectConsITMenual
         || questRecord->m_bSelectQuestMenual)
      this->SendMsg_SelectQuestReward( slotIndex);
    else
      this->Emb_CompleteQuest( slotIndex, 255, 255);
    return true;
}

bool CPlayer::dev_quest_complete_other(char *pwszCharName)
{
    CUserDB *targetUserDb; // [rsp+20h] [rbp-38h]
    CPlayer *targetPlayer; // [rsp+28h] [rbp-30h]
    int slotIndex; // [rsp+30h] [rbp-28h]
    _QUEST_DB_BASE::_LIST *questSlot; // [rsp+38h] [rbp-20h]
    int objectiveIndex; // [rsp+40h] [rbp-18h]
    _Quest_fld *questRecord; // [rsp+48h] [rbp-10h]
  targetUserDb = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
    if ( !targetUserDb )
      return false;
    targetPlayer = &g_Player[targetUserDb->m_idWorld.wIndex];
    if ( !targetPlayer )
      return false;
    for ( slotIndex = 0; ; ++slotIndex )
    {
      if ( slotIndex >= 30 )
        return false;
      questSlot = &targetPlayer->m_Param.m_QuestDB.m_List[slotIndex];
      if ( questSlot->byQuestType != 255 )
        break;
    }
    for ( objectiveIndex = 0; objectiveIndex < 3; ++objectiveIndex )
      questSlot->wNum[objectiveIndex] = -1;
    questSlot->dwPassSec = -1;
    targetUserDb->Update_QuestUpdate( slotIndex, questSlot, 1);
    questRecord = static_cast<_Quest_fld *>(CQuestMgr::s_tblQuest->GetRecord(questSlot->wIndex));
    if (questRecord->m_bSelectConsITMenual
         || questRecord->m_bSelectQuestMenual)
      targetPlayer->SendMsg_SelectQuestReward( slotIndex);
    else
      targetPlayer->Emb_CompleteQuest( slotIndex, 255, 255);
    return true;
}

bool CPlayer::dev_set_animus_exp(unsigned __int64 dwExpPoint)
{
  if (!this->m_pUserDB)
  {
    return false;
  }

  unsigned __int8 pbyType[2]{22, 11};
  for (int j = 0; j < 4; ++j)
  {
    _STORAGE_LIST::_db_con &animus = this->m_Param.m_dbAnimus.m_pStorageList[j];
    if (!animus.m_bLoad || animus.m_bLock || animus.m_dwDur == dwExpPoint)
    {
      continue;
    }

    animus.m_dwDur = dwExpPoint;
    this->m_pUserDB->Update_ItemDur(4u, j, dwExpPoint, 0);

    _animus_exp_inform_zocl msg{};
    msg.wAnimusItemSerial = animus.m_wSerial;
    msg.dwExp = dwExpPoint;
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      pbyType,
      reinterpret_cast<char *>(&msg),
      static_cast<unsigned __int16>(sizeof(msg)));
  }

  return true;
}

bool CPlayer::dev_set_animus_lv(int nAnimusLv)
{
  if (!this->m_pUserDB)
  {
    return false;
  }
  if (nAnimusLv > this->m_Param.GetMaxLevel() || nAnimusLv == 0)
  {
    return false;
  }

  unsigned __int8 typeExp[2]{22, 11};
  unsigned __int8 typeHp[2]{22, 9};
  unsigned __int8 typeFp[2]{22, 10};
  for (int j = 0; j < 4; ++j)
  {
    _STORAGE_LIST::_db_con &animus = this->m_Param.m_dbAnimus.m_pStorageList[j];
    if (!animus.m_bLoad || animus.m_bLock)
    {
      continue;
    }

    unsigned int *p_m_dwLv = &animus.m_dwLv;
    _animus_fld *animusFldFromLv = nullptr;
    if (nAnimusLv == 1)
    {
      animus.m_dwDur = 0;
      animusFldFromLv = GetAnimusFldFromLv(animus.m_wItemIndex, 1u);
      *reinterpret_cast<unsigned __int16 *>(p_m_dwLv) = static_cast<unsigned __int16>(animusFldFromLv->m_nMaxHP);
      *(reinterpret_cast<unsigned __int16 *>(p_m_dwLv) + 1) =
        static_cast<unsigned __int16>(animusFldFromLv->m_nMaxFP);
    }
    else
    {
      _animus_fld *prevAnimusFld = GetAnimusFldFromLv(animus.m_wItemIndex, nAnimusLv - 1);
      animusFldFromLv = GetAnimusFldFromLv(animus.m_wItemIndex, nAnimusLv);
      if (!prevAnimusFld || !animusFldFromLv)
      {
        return false;
      }

      animus.m_dwDur = prevAnimusFld->m_nForLvUpExp + 1;
      *reinterpret_cast<unsigned __int16 *>(p_m_dwLv) = static_cast<unsigned __int16>(animusFldFromLv->m_nMaxHP);
      *(reinterpret_cast<unsigned __int16 *>(p_m_dwLv) + 1) =
        static_cast<unsigned __int16>(animusFldFromLv->m_nMaxFP);
    }

    this->m_pUserDB->Update_ItemDur(4u, j, animus.m_dwDur, 0);

    _animus_exp_inform_zocl msgExp{};
    msgExp.wAnimusItemSerial = animus.m_wSerial;
    msgExp.dwExp = animus.m_dwDur;
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      typeExp,
      reinterpret_cast<char *>(&msgExp),
      static_cast<unsigned __int16>(sizeof(msgExp)));

    _animus_hp_inform_zocl msgHp{};
    msgHp.wAnimusItemSerial = animus.m_wSerial;
    msgHp.wLeftHP = *reinterpret_cast<unsigned __int16 *>(p_m_dwLv);
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      typeHp,
      reinterpret_cast<char *>(&msgHp),
      static_cast<unsigned __int16>(sizeof(msgHp)));

    _animus_fp_inform_zocl msgFp{};
    msgFp.wAnimusItemSerial = animus.m_wSerial;
    msgFp.wLeftFP = *(reinterpret_cast<unsigned __int16 *>(p_m_dwLv) + 1);
    g_Network.m_pProcess[0]->LoadSendMsg(
      this->m_ObjID.m_wIndex,
      typeFp,
      reinterpret_cast<char *>(&msgFp),
      static_cast<unsigned __int16>(sizeof(msgFp)));
  }

  return true;
}

bool CPlayer::dev_set_hp(float prob)
{
  if (this->IsRidingUnit())
  {
    if (!this->m_pUsingUnit)
    {
      return false;
    }

    const unsigned __int8 byFrame = this->m_pUsingUnit->byFrame;
    _UnitFrame_fld *record = static_cast<_UnitFrame_fld *>(g_Main.m_tblUnitFrame.GetRecord(byFrame));
    if (!record)
    {
      return false;
    }

    const int maxGauge = record->m_nUnit_HP;
    int newGauge = static_cast<int>(static_cast<float>(maxGauge) * prob);
    if (newGauge == 0)
    {
      newGauge = 1;
    }
    if (newGauge > maxGauge)
    {
      newGauge = maxGauge;
    }

    this->m_pUsingUnit->dwGauge = static_cast<unsigned int>(newGauge);
    this->SendMsg_AlterUnitHPInform(this->m_pUsingUnit->bySlotIndex, this->m_pUsingUnit->dwGauge);
  }
  else
  {
    const int maxHp = static_cast<int>(this->GetMaxHP());
    int newHp = static_cast<int>(static_cast<float>(maxHp) * prob);
    if (newHp == 0)
    {
      newHp = 1;
    }
    if (newHp > maxHp)
    {
      newHp = maxHp;
    }

    this->SetHP(newHp, false);
    this->SendMsg_SetHPInform();
  }
  return true;
}

bool CPlayer::dev_SetGuildGrade(unsigned __int8 byGrade)
{
    CGuild *guild; // [rsp+20h] [rbp-18h]
  if ( !this->m_Param.m_pGuild )
      return false;
    guild = this->m_Param.m_pGuild;
    if ( !byGrade || byGrade > 8u )
      return false;
    guild->UpdateGrade(byGrade);
    return true;
}

bool CPlayer::dev_SetGuildGradeByGuildSerial(unsigned int dwGuildSerial, unsigned __int8 byGrade)
{
    CGuild *guildFromSerial; // [rsp+20h] [rbp-18h]
  guildFromSerial = GetGuildDataFromSerial(g_Guild, 500, dwGuildSerial);
    if ( !guildFromSerial )
      return false;
    if ( !byGrade || byGrade > 8u )
      return false;
    if ( !dwGuildSerial )
      return false;
    guildFromSerial->UpdateGrade(byGrade);
    return true;
}

bool CPlayer::dev_SetGuildGradeByName(char *uszGuildName, unsigned __int8 byGrade)
{
    CGuild *guildFromName; // [rsp+20h] [rbp-18h]
  guildFromName = GetGuildPtrFromName(g_Guild, 500, uszGuildName);
    if ( !guildFromName )
      return false;
    if ( !byGrade || byGrade > 8u )
      return false;
    if ( !uszGuildName )
      return false;
    guildFromName->UpdateGrade(byGrade);
    return true;
}

#if 0 // duplicate implementation exists in CPlayer.cpp
bool CPlayer::dev_trap_attack_grade(int nPoint)
{
  this->m_nTrapMaxAttackPnt = nPoint;
    for ( int trapIndex = 0; trapIndex < 20; ++trapIndex )
    {
      if (this->m_pmTrp.m_Item[trapIndex].isLoad())
        this->m_pmTrp.m_Item[trapIndex].pItem->m_nTrapMaxAttackPnt = nPoint;
    }
    return true;
}
#endif

bool CPlayer::dev_up_all(int nCum)
{
    unsigned int statIndex; // [rsp+20h] [rbp-78h]
    unsigned int skillCountByMastery[12]; // [rsp+38h] [rbp-60h] BYREF
    _skill_fld *skillRecord; // [rsp+68h] [rbp-30h]
    int skillIndex; // [rsp+70h] [rbp-28h]
    _skill_fld *masterySkillRecord; // [rsp+78h] [rbp-20h]
    unsigned int newMasteryCum; // [rsp+80h] [rbp-18h]
    float masteryShare; // [rsp+84h] [rbp-14h]
  if ( nCum < 0 )
      return false;
    for ( statIndex = 0; (int)statIndex < 2; ++statIndex )
    {
      this->Emb_UpdateStat( statIndex, nCum, 0);
      this->m_pmMst.UpdateCumPerMast(0, statIndex, nCum);
      this->SendMsg_StatInform( statIndex, nCum, 0);
    }
    this->Emb_UpdateStat( 2u, nCum, 0);
    this->m_pmMst.UpdateCumPerMast(1u, 0, nCum);
    this->SendMsg_StatInform( 2u, nCum, 0);
    this->Emb_UpdateStat( 3u, nCum, 0);
    this->m_pmMst.UpdateCumPerMast(2u, 0, nCum);
    this->SendMsg_StatInform( 3u, nCum, 0);
    memset(skillCountByMastery, 0, 32);
    skillRecord = 0LL;
    for ( skillIndex = 0; skillIndex < 48; ++skillIndex )
    {
      skillRecord = static_cast<_skill_fld *>(_MASTERY_PARAM::s_pSkillData->GetRecord(skillIndex));
      if (skillRecord && static_cast<unsigned int>(skillRecord->m_nMastIndex) < 8u)
      {
        if ( strncmp(skillRecord->m_strCode, "FF", 2uLL) )
          ++skillCountByMastery[skillRecord->m_nMastIndex];
      }
    }
    for ( skillIndex = 0; skillIndex < 8; ++skillIndex )
      this->m_pmMst.m_dwSkillMasteryCum[skillIndex] = 0;
    for ( statIndex = 0; (int)statIndex < 48; ++statIndex )
    {
      masterySkillRecord = static_cast<_skill_fld *>(_MASTERY_PARAM::s_pSkillData->GetRecord(statIndex));
      if ( masterySkillRecord && strncmp(masterySkillRecord->m_strCode, "FF", 2uLL) )
      {
        newMasteryCum = nCum;
        if (masterySkillRecord->m_nMastIndex >= 0
          && masterySkillRecord->m_nMastIndex < 8
          && (int)skillCountByMastery[masterySkillRecord->m_nMastIndex] > 0)
        {
          masteryShare =
            (float)(int)newMasteryCum / (float)(int)skillCountByMastery[masterySkillRecord->m_nMastIndex];
          newMasteryCum = CalcRoundUp(masteryShare);
        }
        this->Emb_UpdateStat( statIndex + 4, newMasteryCum, 0);
        this->m_pmMst.UpdateCumPerMast(3u, statIndex, newMasteryCum);
        this->SendMsg_StatInform( statIndex + 4, newMasteryCum, 0);
      }
    }
    for ( statIndex = 0; (int)statIndex < 3; ++statIndex )
    {
      this->Emb_UpdateStat( statIndex + 76, nCum, 0);
      this->m_pmMst.UpdateCumPerMast(5u, statIndex, nCum);
      this->SendMsg_StatInform( statIndex + 76, nCum, 0);
    }
    this->Emb_UpdateStat( 79, nCum, 0);
    this->m_pmMst.UpdateCumPerMast(6u, 0, nCum);
    this->SendMsg_StatInform( 79, nCum, 0);
    for ( statIndex = 0; (int)statIndex < 24; ++statIndex )
    {
      this->Emb_UpdateStat( statIndex + 52, nCum, 0);
      this->m_pmMst.UpdateCumPerMast(4u, statIndex, nCum);
      this->SendMsg_StatInform( statIndex + 52, nCum, 0);
    }
    this->ReCalcMaxHFSP( 1, 0);
    return true;
}

bool CPlayer::dev_up_all_pt(int nLv)
{
    unsigned int newStatValue; // [rsp+20h] [rbp-88h]
    unsigned int statIndex; // [rsp+24h] [rbp-84h]
    unsigned int skillCountByMastery[12]; // [rsp+38h] [rbp-70h] BYREF
    _skill_fld *skillRecord; // [rsp+68h] [rbp-40h]
    int skillIndex; // [rsp+70h] [rbp-38h]
    _skill_fld *masterySkillRecord; // [rsp+78h] [rbp-30h]
    unsigned int newMasteryCum; // [rsp+80h] [rbp-28h]
    float masteryShare; // [rsp+84h] [rbp-24h]
    float baseOffsetSquared; // [rsp+88h] [rbp-20h]
    float forceOffsetSquared; // [rsp+8Ch] [rbp-1Ch]
    float raceOffsetSquared; // [rsp+90h] [rbp-18h]
  if ( nLv <= 0 || nLv > 99 )
      return false;
    baseOffsetSquared = static_cast<float>(pow(1000.0, 2));
    const float baseLevelSquared = static_cast<float>(pow((float)nLv, 2));
    const float baseRoot = sqrt(baseOffsetSquared + (float)((float)(4.0 * baseLevelSquared) * 1000.0));
    newStatValue = (int)pow((float)(baseRoot + -1000.0) / 2.0, 2);
    for ( statIndex = 0; (int)statIndex < 2; ++statIndex )
    {
      this->Emb_UpdateStat( statIndex, newStatValue, 0);
      this->m_pmMst.UpdateCumPerMast(0, statIndex, newStatValue);
      this->SendMsg_StatInform( statIndex, newStatValue, 0);
    }
    this->Emb_UpdateStat( 2u, newStatValue, 0);
    this->m_pmMst.UpdateCumPerMast(1u, 0, newStatValue);
    this->SendMsg_StatInform( 2u, newStatValue, 0);
    forceOffsetSquared = static_cast<float>(pow(100.0, 2));
    const float forceLevelSquared = static_cast<float>(pow((float)nLv, 2));
    const float forceRoot = static_cast<float>(sqrt(forceOffsetSquared + (float)((float)(4.0 * forceLevelSquared) * 100.0)) + -100.0);
    newStatValue = (int)pow(forceRoot / 2.0, 2);
    this->Emb_UpdateStat( 3u, newStatValue, 0);
    this->m_pmMst.UpdateCumPerMast(2u, 0, newStatValue);
    this->SendMsg_StatInform( 3u, newStatValue, 0);
    const float skillLevelSquared = static_cast<float>(pow((float)nLv, 2));
    const float skillLevelPow4 = static_cast<float>(pow(skillLevelSquared, 2));
    static_cast<float>(newStatValue = (int)(float)((float)(CalcRoundUp(skillLevelPow4 / 10.0f) - 1) + 0.0099999998f));
    memset(skillCountByMastery, 0, 32);
    skillRecord = 0LL;
    for ( skillIndex = 0; skillIndex < 48; ++skillIndex )
    {
      skillRecord = static_cast<_skill_fld *>(_MASTERY_PARAM::s_pSkillData->GetRecord(skillIndex));
      if (skillRecord && static_cast<unsigned int>(skillRecord->m_nMastIndex) < 8u)
      {
        if ( strncmp(skillRecord->m_strCode, "FF", 2uLL) )
          ++skillCountByMastery[skillRecord->m_nMastIndex];
      }
    }
    for ( skillIndex = 0; skillIndex < 8; ++skillIndex )
      this->m_pmMst.m_dwSkillMasteryCum[skillIndex] = 0;
    for ( statIndex = 0; (int)statIndex < 48; ++statIndex )
    {
      masterySkillRecord = static_cast<_skill_fld *>(_MASTERY_PARAM::s_pSkillData->GetRecord(statIndex));
      if ( masterySkillRecord && strncmp(masterySkillRecord->m_strCode, "FF", 2uLL) )
      {
        newMasteryCum = newStatValue;
        if (masterySkillRecord->m_nMastIndex >= 0
          && masterySkillRecord->m_nMastIndex < 8
          && (int)skillCountByMastery[masterySkillRecord->m_nMastIndex] > 0)
        {
          masteryShare =
            (float)(int)newMasteryCum / (float)(int)skillCountByMastery[masterySkillRecord->m_nMastIndex];
          newMasteryCum = CalcRoundUp(masteryShare);
        }
        this->Emb_UpdateStat( statIndex + 4, newMasteryCum, 0);
        this->m_pmMst.UpdateCumPerMast(3u, statIndex, newMasteryCum);
        this->SendMsg_StatInform( statIndex + 4, newMasteryCum, 0);
      }
    }
    const float forceItemLevelSquared = static_cast<float>(pow((float)nLv, 2));
    const float forceItemLevelPow4 = static_cast<float>(pow(forceItemLevelSquared, 2));
    static_cast<float>(newStatValue = (int)(float)((float)(CalcRoundUp(forceItemLevelPow4 / 14.0f) - 1) + 0.0099999998f));
    for ( statIndex = 0; (int)statIndex < 24; ++statIndex )
    {
      this->Emb_UpdateStat( statIndex + 52, newStatValue, 0);
      this->m_pmMst.UpdateCumPerMast(4u, statIndex, newStatValue);
      this->SendMsg_StatInform( statIndex + 52, newStatValue, 0);
    }
    for ( statIndex = 0; (int)statIndex < 3; ++statIndex )
    {
      if ( statIndex > 1 )
      {
        if ( statIndex == 2 )
          newStatValue = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 10.0) + 0.89999998);
      }
      else
      {
        newStatValue = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 1.1) + 0.89999998);
      }
      this->Emb_UpdateStat( statIndex + 76, newStatValue, 0);
      this->m_pmMst.UpdateCumPerMast(5u, statIndex, newStatValue);
      this->SendMsg_StatInform( statIndex + 76, newStatValue, 0);
    }
    if ( !(unsigned int)this->m_Param.GetRaceCode()
      || (unsigned int)this->m_Param.GetRaceCode() == 1 )
    {
      newStatValue = (int)(float)(pow((float)(nLv - 1), 2) * 15000.0);
    }
    else if ( (unsigned int)this->m_Param.GetRaceCode() == 2 )
    {
      raceOffsetSquared = static_cast<float>(pow(1000.0, 2));
      const float raceLevelSquared = static_cast<float>(pow((float)nLv, 2));
      const float raceRoot = static_cast<float>(sqrt(raceOffsetSquared + (float)((float)(4.0 * raceLevelSquared) * 1000.0)) + -1000.0);
      newStatValue = (int)pow(raceRoot / 2.0, 2);
    }
    this->Emb_UpdateStat( 79, newStatValue, 0);
    this->m_pmMst.UpdateCumPerMast(6u, 0, newStatValue);
    this->SendMsg_StatInform( 79, newStatValue, 0);
    this->ReCalcMaxHFSP( 1, 0);
    return true;
}

bool CPlayer::dev_up_cashbag(long double dPoint)
{
  if (this->GetLevel() < 39)
      return false;
    if ( this->m_Param.m_pClassData->m_nGrade < 1 )
      return false;
    this->AlterPvPCashBag( dPoint, pm_kill);
    return true;
}

bool CPlayer::dev_up_forceitem(int nCum)
{
    int slotIndex; // [rsp+30h] [rbp-18h]
    _STORAGE_LIST::_db_con *forceItem; // [rsp+38h] [rbp-10h]
    int clampedCum; // [rsp+58h] [rbp+10h]

    clampedCum = nCum;
  if ( nCum < 0 )
      return false;
    if ( !this->m_pUserDB )
      return false;
    if ( nCum > 16777215 )
      clampedCum = 16777215;
    for ( slotIndex = 0; slotIndex < 88; ++slotIndex )
    {
      forceItem = &this->m_Param.m_dbForce.m_pStorageList[slotIndex];
      if ( forceItem->m_bLoad )
      {
        if ( !forceItem->m_bLock )
        {
          this->m_pUserDB->Update_ItemDur( 3u, slotIndex, clampedCum, 0);
          this->m_Param.m_pStoragePtr[3]->UpdateCurDur(slotIndex, clampedCum);
          this->SendMsg_FcitemInform( forceItem->m_wSerial, clampedCum);
        }
      }
    }
    return true;
}

bool CPlayer::dev_up_forcemastery(unsigned int nCum)
{
  for ( int forceMasteryIndex = 0; forceMasteryIndex < 24; ++forceMasteryIndex )
      this->Emb_UpdateStat( forceMasteryIndex + 52, nCum, 0);
    return true;
}

bool CPlayer::dev_up_mastery(int nMasteryCode, unsigned int nMasteryIndex, int nLv)
{
    unsigned int newStatValue; // [rsp+20h] [rbp-38h]
    unsigned int statIndex; // [rsp+24h] [rbp-34h]
    unsigned int newSkillStatValue; // [rsp+28h] [rbp-30h]
    int skillIndexOffset; // [rsp+2Ch] [rbp-2Ch]
    int masterySkillIndex; // [rsp+30h] [rbp-28h]
    unsigned int skillStatIndex; // [rsp+34h] [rbp-24h]
    float baseOffsetSquared; // [rsp+38h] [rbp-20h]
    float weaponOffsetSquared; // [rsp+3Ch] [rbp-1Ch]
    float classOffsetSquared; // [rsp+40h] [rbp-18h]
    float raceOffsetSquared; // [rsp+44h] [rbp-14h]
    unsigned __int64 masterySkillGroupOffset; // [rsp+48h] [rbp-10h]
  if ( nMasteryCode == 3 )
    {
      if ( (int)nMasteryIndex >= 8 )
        return false;
    }
    else if ( !_STAT_DB_BASE::IsRangePerMastery(nMasteryCode, nMasteryIndex) )
    {
      return false;
    }
    if ( nLv > 99 )
      return false;
    if ( nLv < 1 )
      return false;
    newStatValue = 0;
    if ( nMasteryCode )
    {
      switch ( nMasteryCode )
      {
        case 1:
        {
          weaponOffsetSquared = static_cast<float>(pow(1000.0, 2));
          const float weaponLevelSquared = static_cast<float>(pow((float)nLv, 2));
          const float weaponRoot = static_cast<float>(sqrt(weaponOffsetSquared + (float)((float)(4.0 * weaponLevelSquared) * 1000.0)) + -1000.0);
          newStatValue = (int)pow(weaponRoot / 2.0, 2);
          break;
        }
        case 2:
        {
          classOffsetSquared = static_cast<float>(pow(1000.0, 2));
          const float classLevelSquared = static_cast<float>(pow((float)nLv, 2));
          const float classRoot = static_cast<float>(sqrt(classOffsetSquared + (float)((float)(4.0 * classLevelSquared) * 1000.0)) + -1000.0);
          newStatValue = (int)pow(classRoot / 2.0, 2);
          break;
        }
        case 4:
        {
          const float forceItemLevelSquared = static_cast<float>(pow((float)nLv, 2));
          const float forceItemLevelPow4 = static_cast<float>(pow(forceItemLevelSquared, 2));
          static_cast<float>(newStatValue = (int)(float)((float)(CalcRoundUp(forceItemLevelPow4 / 14.0f) - 1) + 0.0099999998f));
          break;
        }
        case 3:
        {
          const float skillLevelSquared = static_cast<float>(pow((float)nLv, 2));
          const float skillLevelPow4 = static_cast<float>(pow(skillLevelSquared, 2));
          static_cast<float>(newStatValue = (int)(float)((float)(CalcRoundUp(skillLevelPow4 / 10.0f) - 1) + 0.0099999998f));
          break;
        }
        case 5:
          if ( nMasteryIndex > 1 )
          {
            if ( nMasteryIndex == 2 )
              newStatValue = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 10.0) + 0.89999998);
          }
          else
          {
            newStatValue = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 1.1) + 0.89999998);
          }
          break;
        case 6:
        {
          if ( !(unsigned int)this->m_Param.GetRaceCode()
            || (unsigned int)this->m_Param.GetRaceCode() == 1 )
          {
            newStatValue = (int)(float)(pow((float)nLv - 1.0, 2) * 15000.0);
          }
          else if ( (unsigned int)this->m_Param.GetRaceCode() == 2 )
          {
            raceOffsetSquared = static_cast<float>(pow(1000.0, 2));
            const float raceLevelSquared = static_cast<float>(pow((float)nLv, 2));
            const float raceRoot = static_cast<float>(sqrt(raceOffsetSquared + (float)((float)(4.0 * raceLevelSquared) * 1000.0)) + -1000.0);
            newStatValue = (int)pow(raceRoot / 2.0, 2);
          }
          break;
        }
      }
    }
    else
    {
      baseOffsetSquared = static_cast<float>(pow(1000.0, 2));
      const float baseLevelSquared = static_cast<float>(pow((float)nLv, 2));
      const float baseRoot = static_cast<float>(sqrt(baseOffsetSquared + (float)((float)(4.0 * baseLevelSquared) * 1000.0)) + -1000.0);
      newStatValue = (int)pow(baseRoot / 2.0, 2);
    }
    if ( !newStatValue )
      return false;
    if ( nMasteryCode == 3 )
    {
      masterySkillGroupOffset = 196LL * (int)nMasteryIndex;
      newSkillStatValue =
        newStatValue / CPlayer::s_SkillIndexPerMastery[masterySkillGroupOffset / 196].m_nSkillIndexNum + 1;
      for ( skillIndexOffset = 0; skillIndexOffset < CPlayer::s_SkillIndexPerMastery[nMasteryIndex].m_nSkillIndexNum; ++skillIndexOffset )
      {
        masterySkillIndex = CPlayer::s_SkillIndexPerMastery[nMasteryIndex].m_nSkillIndex[skillIndexOffset];
        if ( newSkillStatValue )
        {
          skillStatIndex = _STAT_DB_BASE::GetStatIndex(3u, masterySkillIndex);
          this->Emb_UpdateStat( skillStatIndex, newSkillStatValue, 0);
          this->SendMsg_StatInform( skillStatIndex, newSkillStatValue, 0);
        }
      }
    }
    else
    {
      statIndex = _STAT_DB_BASE::GetStatIndex(nMasteryCode, nMasteryIndex);
      this->Emb_UpdateStat( statIndex, newStatValue, 0);
      this->SendMsg_StatInform( statIndex, newStatValue, 0);
    }
    return true;
}

bool CPlayer::dev_up_pvp(long double dPoint)
{
  this->AlterPvPPoint(dPoint, cheat, -1);
  return true;
}

bool CPlayer::dev_up_skill(char *pszSkillCode, unsigned int nCum)
{
    _base_fld *skillRecord; // [rsp+20h] [rbp-18h]
  skillRecord = g_Main.m_tblEffectData[0].GetRecord( pszSkillCode);
    if ( !skillRecord )
      return false;
    this->Emb_UpdateStat( skillRecord->m_dwIndex + 4, nCum, 0);
    return true;
}

bool CPlayer::dev_view_boss()
{
  const unsigned __int64 stateWithBossFlag = this->m_dwLastState | 0x10000uLL;

  _state_inform_zocl msg{};
  msg.dwSerial = this->m_dwObjSerial;
  msg.dwState = stateWithBossFlag;

  unsigned __int8 pbyType[2]{4, 25};
  g_Network.m_pProcess[0]->LoadSendMsg(
    this->m_ObjID.m_wIndex,
    pbyType,
    reinterpret_cast<char *>(&msg),
    static_cast<unsigned __int16>(sizeof(msg)));
  return true;
}

bool CPlayer::dev_view_method(char *pwszDstName)
{
  CPlayer *target = nullptr;
  if (pwszDstName)
  {
    const unsigned __int8 nameLen = static_cast<unsigned __int8>(std::strlen(pwszDstName));
    for (int j = 0; j < MAX_PLAYER; ++j)
    {
      CPlayer *player = &g_Player[j];
      if (player->m_bLive && player->m_Param.m_byNameLen == nameLen)
      {
        const char *name = player->m_Param.GetCharNameW();
        if (!strncmp(name, pwszDstName, nameLen))
        {
          target = player;
          break;
        }
      }
    }
  }
  else
  {
    target = this;
  }

  if (!target)
  {
    return false;
  }

  CLogFile log;
  const int currentSec = GetCurrentSec();
  const unsigned int korTime = static_cast<unsigned int>(GetKorLocalTime());
  const char *charNameA = target->m_Param.GetCharNameA();

  char buffer[152]{};
  sprintf_s(buffer, "..\\ZoneServerLog\\CharLog\\%s_Method_%u_%d.log", charNameA, korTime, currentSec);
  log.SetWriteLogFile(buffer, 1, 0, 0, 0);
  log.Write(">> PARAM ###########");

  const int maxHp = target->GetMaxHP();
  const int curHp = target->GetHP();
  log.Write("HP cur: %d, max: %d, lim: %d", curHp, target->m_nMaxPoint[0], maxHp);

  const int maxFp = target->GetMaxFP();
  const int curFp = target->GetFP();
  log.Write("FP cur: %d, max: %d, lim: %d", curFp, target->m_nMaxPoint[1], maxFp);

  const int maxSp = target->GetMaxSP();
  const int curSp = target->GetSP();
  log.Write("SP cur: %d, max: %d, lim: %d", curSp, target->m_nMaxPoint[2], maxSp);

  const char *stateNames[] = {"stop", "move"};
  const char *moveNames[] = {"walk", "run"};
  const char *standNames[] = {"stand", "sit"};

  log.Write(">> POSTION #########");
  log.Write("MAP: %s", target->m_pCurMap->m_pMapSet->m_strCode);
  log.Write("POS: %d, %d, %d", (int)target->m_fCurPos[0], (int)target->m_fCurPos[1], (int)target->m_fCurPos[2]);
  log.Write("TOWN: %d", target->m_byPosRaceTown);
  log.Write(
    "STATE: %s, MODE: %s, STAND: %s",
    stateNames[static_cast<unsigned __int8>(target->m_bMove)],
    moveNames[target->m_byMoveType],
    standNames[target->m_byStandType]);

  log.Write(">> RATE ############");
  for (int index = 0; index < 66; ++index)
  {
    const float rate = target->m_EP.GetEff_Rate(index);
    log.Write("%d\t : %f", index, rate);
  }

  log.Write(">> PLUS ############");
  for (int index = 0; index < 42; ++index)
  {
    const float plus = target->m_EP.GetEff_Plus(index);
    log.Write("%d\t : %f", index, plus);
  }

  log.Write(">> STATE ###########");
  for (int index = 0; index < 29; ++index)
  {
    const bool state = target->m_EP.GetEff_State(index);
    log.Write("%d\t : %d", index, state);
  }

  log.Write(">> HAVE ############");
  for (int index = 0; index < 92; ++index)
  {
    const float have = target->m_EP.GetEff_Have(index);
    log.Write("%d\t : %f", index, have);
  }

  return true;
}

bool CPlayer::mgr_all_item_muzi(unsigned int nLv)
{
    int slotIndex; // [rsp+30h] [rbp-28h]
    _STORAGE_LIST::_db_con *item; // [rsp+38h] [rbp-20h]
    unsigned __int8 upgradeSocketLimit; // [rsp+40h] [rbp-18h]
    unsigned int upgradeState; // [rsp+44h] [rbp-14h]
    signed int upgradeStep; // [rsp+48h] [rbp-10h]
  if ( nLv >= 8 )
      return false;
    for ( slotIndex = 0; ; ++slotIndex )
    {
      const unsigned __int8 bagNum = this->m_Param.GetBagNum();
      if ( slotIndex >= 20 * bagNum )
        break;
      if ( this->m_Param.m_dbInven.m_pStorageList[slotIndex].m_bLoad )
      {
        item = &this->m_Param.m_dbInven.m_pStorageList[slotIndex];
        if ( item->m_bLoad )
        {
          if ( item->m_byTableCode == 6 )
          {
            upgradeSocketLimit = GetItemUpgLimSocket(item->m_dwLv);
            if ( upgradeSocketLimit >= (int)nLv )
            {
              upgradeState = GetBitAfterSetLimSocket(upgradeSocketLimit);
              for ( upgradeStep = 0; upgradeStep < (int)nLv; ++upgradeStep )
                upgradeState = GetBitAfterUpgrade(upgradeState, 0, upgradeStep);
              this->Emb_ItemUpgrade( 0, 0, slotIndex, upgradeState);
              this->SendMsg_DeleteStorageInform( 0, item->m_wSerial);
              this->SendMsg_RewardAddItem( item, 0);
            }
          }
        }
      }
    }
    return true;
}

bool CPlayer::mgr_change_degree(unsigned __int8 nDegree)
{
    this->m_byUserDgr = nDegree;
    this->m_pUserDB->m_byUserDgr = nDegree;
    return true;
}

bool CPlayer::mgr_defense_item_grace(unsigned __int8 byItemCode, unsigned int nLv)
{
    int slotIndex; // [rsp+30h] [rbp-28h]
    _STORAGE_LIST::_db_con *item; // [rsp+38h] [rbp-20h]
    unsigned __int8 upgradeSocketLimit; // [rsp+40h] [rbp-18h]
    unsigned int upgradeState; // [rsp+44h] [rbp-14h]
    signed int upgradeStep; // [rsp+48h] [rbp-10h]
  if ( nLv >= 8 )
      return false;
    item = 0LL;
    for ( slotIndex = 0; ; ++slotIndex )
    {
      const unsigned __int8 bagNum = this->m_Param.GetBagNum();
      if ( slotIndex >= 20 * bagNum )
        break;
      item = &this->m_Param.m_dbInven.m_pStorageList[slotIndex];
      if ( item && item->m_bLoad )
      {
        if ( byItemCode == 37 )
        {
          if ( item->m_byTableCode
            && item->m_byTableCode != 1
            && item->m_byTableCode != 2
            && item->m_byTableCode != 3
            && item->m_byTableCode != 4
            && item->m_byTableCode != 5
            && item->m_byTableCode != 7 )
          {
            continue;
          }
        }
        else if ( item->m_byTableCode != byItemCode )
        {
          continue;
        }
        upgradeSocketLimit = GetItemUpgLimSocket(item->m_dwLv);
        if ( upgradeSocketLimit >= (int)nLv )
        {
          upgradeState = GetBitAfterSetLimSocket(upgradeSocketLimit);
          for ( upgradeStep = 0; upgradeStep < (int)nLv; ++upgradeStep )
            upgradeState = GetBitAfterUpgrade(upgradeState, 5u, upgradeStep);
          this->Emb_ItemUpgrade( 0, 0, slotIndex, upgradeState);
          this->SendMsg_DeleteStorageInform( 0, item->m_wSerial);
          this->SendMsg_RewardAddItem( item, 0);
        }
      }
    }
    return true;
}

bool CPlayer::mgr_destroy_system_tower()
{
    _pnt_rect pRect; // [rsp+28h] [rbp-1E0h] BYREF
    int secY; // [rsp+44h] [rbp-1C4h]
    int secX; // [rsp+48h] [rbp-1C0h]
    unsigned int dwSecIndex; // [rsp+4Ch] [rbp-1BCh]
    CObjectList *sectorList; // [rsp+50h] [rbp-1B8h]
    _object_list_point *node; // [rsp+58h] [rbp-1B0h]
    CGuardTower *guardTower; // [rsp+60h] [rbp-1A8h]
    _object_id *objectId; // [rsp+68h] [rbp-1A0h]
    CGuardTower *systemTower; // [rsp+70h] [rbp-198h]
    char wszTran[152]; // [rsp+90h] [rbp-178h] BYREF
    LPCSTR appNameByRace[7]; // [rsp+128h] [rbp-E0h]
    char Buffer[144]; // [rsp+160h] [rbp-A8h] BYREF
  const int curSecNum = this->GetCurSecNum();
    this->m_pCurMap->GetRectInRadius( &pRect, 1, curSecNum);
    for ( secY = pRect.nStarty; secY <= pRect.nEndy; ++secY )
    {
      for ( secX = pRect.nStartx; secX <= pRect.nEndx; ++secX )
      {
        _sec_info *secInfo = this->m_pCurMap->GetSecInfo();
        dwSecIndex = secInfo->m_nSecNumW * secY + secX;
        sectorList = this->m_pCurMap->GetSectorListObj(this->m_wMapLayerIndex, dwSecIndex);
        if ( sectorList )
        {
          node = sectorList->m_Head.m_pNext;
          while ( node != &sectorList->m_Tail )
          {
            guardTower = (CGuardTower *)node->m_pItem;
            node = node->m_pNext;
            objectId = &guardTower->m_ObjID;
            if ( !guardTower->m_ObjID.m_byKind && objectId->m_byID == 4 )
            {
              systemTower = guardTower;
              if ( guardTower->m_bSystemStruct
                && abs(guardTower->m_fCurPos[1] - this->m_fCurPos[1]) <= 100.0
                && GetSqrt(guardTower->m_fCurPos, this->m_fCurPos) < 20.0 )
              {
                M2W(systemTower->m_pRecordSet->m_strCode, wszTran, 128);
                if ( systemTower->m_nIniIndex != -1 )
                {
                  appNameByRace[0] = "BELLATO";
                  appNameByRace[1] = "CORA";
                  appNameByRace[2] = "ACCRETIA";
                  sprintf_s(Buffer, "Map%d", systemTower->m_nIniIndex);
                  WritePrivateProfileStringA(
                    appNameByRace[systemTower->m_byRaceCode],
                    Buffer,
                    "NULL",
                    ".\\Script\\SystemGuardTower.ini");
                }
                systemTower->Destroy(0, true);
                return true;
              }
            }
          }
        }
      }
    }
    return false;
}

bool CPlayer::mgr_dungeon_pass()
{
  if (this->m_pCurMap->m_pMapSet->m_nMapType != 1)
  {
    return false;
  }
  if (this->m_bMapLoading)
  {
    return false;
  }
  if (this->m_pDHChannel)
  {
    for (int j = 0; j < this->m_pDHChannel->m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum; ++j)
    {
      this->m_pDHChannel->m_MissionMgr.Count[j].bPass = 1;
    }
    this->m_pDHChannel->m_MissionMgr.OpenPortal(-1);
  }
  return true;
}

bool CPlayer::mgr_exit_keeper()
{
  // this is not a stub
  return 0;
}

bool CPlayer::mgr_exit_stone()
{
  // this is not a stub
  return 0;
}

bool CPlayer::mgr_free_ride_ship()
{
  CMapData *pIntoMap = g_MapOper.GetMap("Transport01");
  if (!pIntoMap)
  {
    return false;
  }
  if (!pIntoMap->IsMapIn(fPos))
  {
    return false;
  }

  const unsigned __int16 raceCode = static_cast<unsigned short>(this->m_Param.GetRaceCode());
  this->OutOfMap(pIntoMap, raceCode, 4u, fPos);
  const unsigned __int8 mapCode = this->m_Param.GetMapCode();
  this->SendMsg_GotoRecallResult(0, mapCode, fPos, 4u);
  return true;
}

bool CPlayer::mgr_goto_mine()
{
  CMapData *pIntoMap = g_MapOper.GetMap("resources");
  if (!pIntoMap)
  {
    return false;
  }
  this->OutOfMap(pIntoMap, 0, 4u, fTarPos);
  this->SendMsg_GotoRecallResult(0, pIntoMap->m_pMapSet->m_dwIndex, fTarPos, 4u);
  return true;
}

bool CPlayer::mgr_goto_shipport(int nRaceCode, int nPort)
{
    CTransportShip *transportShip; // [rsp+30h] [rbp-58h]
    _portal_dummy *sourcePortal; // [rsp+38h] [rbp-50h]
    CMapData *destinationMap; // [rsp+40h] [rbp-48h]
    _portal_dummy *destinationPortal; // [rsp+48h] [rbp-40h]
    float newPos[12]; // [rsp+58h] [rbp-30h] BYREF
  if ( nRaceCode >= 3 )
      return false;
    if ( this->GetCurSecNum() == -1 || this->m_bMapLoading )
      return false;
    transportShip = &g_TransportShip[nRaceCode];
    sourcePortal = transportShip->m_pLinkShipMap->GetPortal( 3 * nPort + nRaceCode);
    if ( !sourcePortal )
      return false;
    if ( !sourcePortal->m_pPortalRec )
      return false;
    destinationMap = g_MapOper.GetMap( sourcePortal->m_pPortalRec->m_strLinkMapCode);
    if ( !destinationMap )
      return false;
    destinationPortal = destinationMap->GetPortal( sourcePortal->m_pPortalRec->m_strLinkPortalCode);
    if ( !destinationPortal )
      return false;
    if ( !destinationMap->GetRandPosInDummy( destinationPortal->m_pDumPos, newPos, 1) )
      std::memcpy(newPos, destinationPortal->m_pDumPos->m_fCenterPos, 12);
    this->OutOfMap( destinationMap, 0, 4u, newPos);
    const unsigned __int8 mapCode = this->m_Param.GetMapCode();
    this->SendMsg_GotoRecallResult( 0, mapCode, newPos, 4u);
    return true;
}

bool CPlayer::mgr_goto_stone(unsigned __int8 byRaceCode)
{
    __holy_stone_data *holyStoneData; // [rsp+30h] [rbp-18h]
  if ( byRaceCode >= 3u )
      return false;
    holyStoneData = &g_HolySys.m_HolyStoneData[byRaceCode];
    this->OutOfMap( holyStoneData->pCreateMap, 0, 4u, holyStoneData->CreateDummy.m_fCenterPos);
    this->SendMsg_GotoRecallResult(
      0,
      holyStoneData->pCreateMap->m_pMapSet->m_dwIndex,
      holyStoneData->CreateDummy.m_fCenterPos,
      4u);
    return true;
}

bool CPlayer::mgr_goto_store(unsigned __int8 nRaceCode, char *pszNPCName)
{
  if (this->m_pmTrd.bDTradeMode)
  {
    return false;
  }
  if (this->GetCurSecNum() == -1 || this->m_bMapLoading)
  {
    return false;
  }

  CMapData *pIntoMap = g_MapOper.GetStartMap(nRaceCode);
  if (!pIntoMap)
  {
    return false;
  }

  float *startPos = nullptr;
  const int mapSerial = pIntoMap->GetMapCode();
  CItemStoreManager *manager = CItemStoreManager::Instance();
  CMapItemStoreList *storeList = manager->GetMapItemStoreListBySerial(mapSerial);
  if (!storeList)
  {
    return false;
  }

  const size_t nameLen = std::strlen(pszNPCName);
  for (int j = 0; j < storeList->m_nItemStoreNum; ++j)
  {
    CItemStore *store = &storeList->m_ItemStore[j];
    if (store->m_pRec && !strncmp(store->m_pRec->m_strStore_NPCname, pszNPCName, nameLen))
    {
      if (store->m_pDum && store->m_pDum->m_pDumPos)
      {
        startPos = store->m_pDum->m_pDumPos->m_fCenterPos;
      }
      break;
    }
  }

  if (!startPos)
  {
    return false;
  }

  this->OutOfMap(pIntoMap, 0, 4u, startPos);
  const unsigned __int8 mapCode = this->m_Param.GetMapCode();
  this->SendMsg_GotoRecallResult(0, mapCode, startPos, 4u);
  return true;
}

bool CPlayer::mgr_gotoCoordinates(char *pszMapCode, float fX, float fY, float fZ)
{
    CMapData *destinationMap; // [rsp+30h] [rbp-48h]
    float targetPos[12]; // [rsp+48h] [rbp-30h] BYREF
  destinationMap = g_MapOper.GetMap( pszMapCode);
    if ( !destinationMap )
      return false;
    if ( destinationMap->m_pMapSet->m_nMapType )
      return false;
    targetPos[0] = fX;
    targetPos[1] = fY;
    targetPos[2] = fZ;
    if ( !destinationMap->IsMapIn( targetPos) )
      return false;
    this->OutOfMap( destinationMap, 0, 4u, targetPos);
    this->SendMsg_GotoRecallResult( 0, destinationMap->m_pMapSet->m_dwIndex, targetPos, 4u);
    return true;
}

bool CPlayer::mgr_gotoDstCoordinates(char *pwszDstName, char *pszMapCode, float fX, float fY, float fZ)
{
    CPlayer *targetPlayer; // [rsp+30h] [rbp-48h]
    CMapData *destinationMap; // [rsp+38h] [rbp-40h]
    float targetPos[12]; // [rsp+48h] [rbp-30h] BYREF
  targetPlayer = 0LL;
    if ( pwszDstName )
    {
    targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, pwszDstName);
    }
    else
    {
      if ( !this->m_TargetObject.pObject )
        return false;
      if ( !this->m_TargetObject.pObject->m_bLive )
        return false;
      if ( this->m_TargetObject.pObject->m_ObjID.m_byKind || this->m_TargetObject.pObject->m_ObjID.m_byID )
        return false;
      if ( this->m_TargetObject.byKind != this->m_TargetObject.pObject->m_ObjID.m_byKind
        || this->m_TargetObject.byID != this->m_TargetObject.pObject->m_ObjID.m_byID )
      {
        return false;
      }
      targetPlayer = (CPlayer *)this->m_TargetObject.pObject;
    }
    if ( !targetPlayer )
      return false;
    destinationMap = g_MapOper.GetMap( pszMapCode);
    if ( !destinationMap )
      return false;
    if ( destinationMap->m_pMapSet->m_nMapType )
      return false;
    targetPos[0] = fX;
    targetPos[1] = fY;
    targetPos[2] = fZ;
    if ( !destinationMap->IsMapIn( targetPos) )
      return false;
    targetPlayer->OutOfMap( destinationMap, 0, 4u, targetPos);
    targetPlayer->SendMsg_GotoRecallResult( 0, destinationMap->m_pMapSet->m_dwIndex, targetPos, 4u);
    return true;
}

bool CPlayer::mgr_holykeeper_start(int nRace)
{

    return 0;
}

bool CPlayer::mgr_holystone_start(int nNumOfTime)
{

    return 0;
}

bool CPlayer::mgr_item_telekinesis()
{
  if (!this->m_pUserDB)
  {
    return false;
  }

  char result = 0;
  CMapData *map = this->m_pCurMap;
  const int curSecNum = this->GetCurSecNum();
  _pnt_rect rect{};
  map->GetRectInRadius(&rect, 100, curSecNum);

  for (int y = rect.nStarty; y < rect.nEndy; ++y)
  {
    for (int x = rect.nStartx; x < rect.nEndx; ++x)
    {
      _sec_info *secInfo = map->GetSecInfo();
      const unsigned int secIndex = secInfo->m_nSecNumW * y + x;
      CObjectList *sectorList = map->GetSectorListObj(this->m_wMapLayerIndex, secIndex);
      if (!sectorList)
      {
        continue;
      }

      for (_object_list_point *node = sectorList->m_Head.m_pNext; node != &sectorList->m_Tail; node = node->m_pNext)
      {
        CGameObject *object = node->m_pItem;
        if (!object || object->m_ObjID.m_byKind != 1)
        {
          continue;
        }

        CItemBox *itemBox = static_cast<CItemBox *>(object);
        if (!itemBox->IsTakeRight(this))
        {
          continue;
        }
        if (this->m_EP.GetEff_State(EFF_STATE_INSUPERABLE) || this->m_EP.GetEff_State(EFF_STATE_INSUPERABLE_MOVE))
        {
          continue;
        }

        _STORAGE_LIST::_db_con item;
        std::memcpy(&item, &itemBox->m_Item, sizeof(item));
        if (item.m_byTableCode == 19)
        {
          continue;
        }

        auto processReward = [&]() {
          bool darkHoleEvent = false;
          if (itemBox->m_byCreateCode == 5 && this->m_pDHChannel)
          {
            darkHoleEvent = this->m_pDHChannel->CheckEvent(
              dh_event_take,
              item.m_byTableCode,
              item.m_wItemIndex,
              static_cast<int>(item.m_dwDur),
              itemBox);
          }

          if (itemBox->m_byThrowerID == 1)
          {
            if (!darkHoleEvent)
            {
              _base_fld *record = g_Main.m_tblItemData[item.m_byTableCode].GetRecord( item.m_wItemIndex);
              this->Emb_CheckActForQuest(4, record->m_strCode, 1u, 0);
              this->CheckMentalTakeAndUpdateLastMetalTicket(record->m_strCode);
            }

            if (this->m_pPartyMgr->IsPartyMode()
                && itemBox->m_dwPartyBossSerial == this->m_pPartyMgr->m_pPartyBoss->m_id.dwSerial)
            {
              unsigned __int8 amount = 1;
              if (IsOverLapItem(item.m_byTableCode))
              {
                amount = static_cast<unsigned __int8>(item.m_dwDur);
              }
              this->SendMsg_PartyLootItemInform(
                this->m_dwObjSerial,
                item.m_byTableCode,
                item.m_wItemIndex,
                amount);
            }
          }

          this->SendMsg_RewardAddItem(&item, 0);
          itemBox->Destroy();
        };

        if (!IsSaveItem(item.m_byTableCode))
        {
          processReward();
          continue;
        }

        if (this->m_Param.m_dbInven.GetIndexEmptyCon() != 255)
        {
          item.m_wSerial = this->m_Param.GetNewItemSerial();
          if (this->Emb_AddStorage(0, &item, 0, 1))
          {
            result = 1;
            bool logItem = true;
            if (!IsProtectItem(item.m_byTableCode) && itemBox->m_byThrowerID == 1)
            {
              logItem = false;
            }
            if (logItem)
            {
              if (itemBox->m_wMonRecIndex != 0xFFFF && itemBox->m_bBossMob)
              {
                _base_fld *monRec = g_Main.m_tblMonster.GetRecord( itemBox->m_wMonRecIndex);
                _base_fld *itemRec = g_Main.m_tblItemData[item.m_byTableCode].GetRecord( item.m_wItemIndex);
                const DWORD nowSec = timeGetTime() / 1000;
                const char *charNameA = this->m_Param.GetCharNameA();
                CMonster::s_logTrace_Boss_Looting.Write(
                  "\t Mob: %s Item: %s => %s ( %d ), Sec: %d",
                  monRec->m_strCode,
                  itemRec->m_strCode,
                  charNameA,
                  this->m_dwObjSerial,
                  nowSec);
              }

              CPlayer::s_MgrItemHistory.take_ground_item(
                this->m_ObjID.m_wIndex,
                itemBox->m_byCreateCode,
                &item,
                itemBox->m_aszThrowerName,
                itemBox->m_dwThrowerCharSerial,
                itemBox->m_szThrowerID,
                itemBox->m_wMonRecIndex,
                this->m_pCurMap->m_pMapSet->m_strCode,
                this->m_fCurPos,
                this->m_szItemHistoryFileName);

              if (itemBox->m_dwThrowerCharSerial != static_cast<unsigned int>(-1)
                  && itemBox->m_dwThrowerCharSerial != this->m_dwObjSerial)
              {
                const char *takerId = this->m_pUserDB->m_szAccountID;
                const char *takerName = this->m_Param.GetCharNameA();
                CPlayer::s_MgrItemHistory.trans_ground_item(
                  &item,
                  takerName,
                  this->m_dwObjSerial,
                  takerId,
                  itemBox->m_szThrowerItemHistoryFileName);
              }
            }

            processReward();
          }
        }
      }
    }
  }

  return (result) != 0;
}

bool CPlayer::mgr_kick(char *pwszCharName)
{
    CUserDB *targetUserDb; // [rsp+30h] [rbp-18h]
  if ( pwszCharName )
    {
      targetUserDb = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
      if ( targetUserDb )
      {
        targetUserDb->ForceCloseCommand( 0, 0, 1, "Kick By GM");
        return true;
      }
      else
      {
        return false;
      }
    }
    else if ( this->m_TargetObject.pObject )
    {
      if ( this->m_TargetObject.pObject->m_bLive )
      {
        if ( this->m_TargetObject.pObject->m_ObjID.m_byKind || this->m_TargetObject.pObject->m_ObjID.m_byID )
        {
          return false;
        }
        else if ( this->m_TargetObject.byKind == this->m_TargetObject.pObject->m_ObjID.m_byKind
               && this->m_TargetObject.byID == this->m_TargetObject.pObject->m_ObjID.m_byID )
        {
          CPlayer *targetPlayer = static_cast<CPlayer *>(this->m_TargetObject.pObject);
          CUserDB *targetUser = targetPlayer->m_pUserDB;
          if (targetUser)
          {
            targetUser->ForceCloseCommand(0, 0, true, "Kick By GM");
          }
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
}

bool CPlayer::mgr_make_system_tower(char *pszTowerCode)
{
    char wszTran[136]; // [rsp+40h] [rbp-398h] BYREF
    _GuardTowerItem_fld *towerRecord; // [rsp+C8h] [rbp-310h]
    int raceIndex; // [rsp+D0h] [rbp-308h]
    int iniSlotIndex; // [rsp+D4h] [rbp-304h]
    LPCSTR appNameByRace[9]; // [rsp+E8h] [rbp-2F0h]
    int nIniIndex; // [rsp+134h] [rbp-2A4h]
    char ReturnedString[160]; // [rsp+150h] [rbp-288h] BYREF
    char Buffer[160]; // [rsp+1F0h] [rbp-1E8h] BYREF
    char KeyName[160]; // [rsp+290h] [rbp-148h] BYREF
    char String[144]; // [rsp+330h] [rbp-A8h] BYREF
  M2W(pszTowerCode, wszTran, 128);
    towerRecord = static_cast<_GuardTowerItem_fld *>(g_Main.m_tblItemData[25].GetRecord(pszTowerCode));
    if ( !towerRecord )
      return false;
    if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return false;
    raceIndex = -1;
    for ( iniSlotIndex = 0; iniSlotIndex < 3; ++iniSlotIndex )
    {
      if (towerRecord->m_strCivil[2 * iniSlotIndex] == '1')
      {
        raceIndex = iniSlotIndex;
        break;
      }
    }
    if ( raceIndex == -1 )
      return false;
    appNameByRace[0] = "BELLATO";
    appNameByRace[1] = "CORA";
    appNameByRace[2] = "ACCRETIA";
    appNameByRace[6] = "BELLATO";
    appNameByRace[7] = "CORA";
    appNameByRace[8] = "ACCRETIA";
    nIniIndex = -1;
    for ( iniSlotIndex = 0; iniSlotIndex < 50; ++iniSlotIndex )
    {
      sprintf_s(Buffer, "Map%d", iniSlotIndex);
      GetPrivateProfileStringA(
        appNameByRace[raceIndex],
        Buffer,
        "NULL",
        ReturnedString,
        128,
        ".\\Script\\SystemGuardTower.ini");
      if ( !std::strcmp(ReturnedString, "NULL") )
      {
        nIniIndex = iniSlotIndex;
        break;
      }
    }
    if ( nIniIndex == -1 )
      return false;
    if ( !CreateSystemTower(this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, towerRecord->m_dwIndex, raceIndex, nIniIndex) )
      return false;
    sprintf_s(KeyName, "Map%d", nIniIndex);
    WritePrivateProfileStringA(
      appNameByRace[raceIndex],
      KeyName,
      this->m_pCurMap->m_pMapSet->m_strCode,
      ".\\Script\\SystemGuardTower.ini");
    sprintf_s(KeyName, "Pos%d_x", nIniIndex);
    _itoa((int)this->m_fCurPos[0], String, 10);
    WritePrivateProfileStringA(appNameByRace[raceIndex], KeyName, String, ".\\Script\\SystemGuardTower.ini");
    sprintf_s(KeyName, "Pos%d_y", nIniIndex);
    _itoa((int)this->m_fCurPos[1], String, 10);
    WritePrivateProfileStringA(appNameByRace[raceIndex], KeyName, String, ".\\Script\\SystemGuardTower.ini");
    sprintf_s(KeyName, "Pos%d_z", nIniIndex);
    _itoa((int)this->m_fCurPos[2], String, 10);
    WritePrivateProfileStringA(appNameByRace[raceIndex], KeyName, String, ".\\Script\\SystemGuardTower.ini");
    sprintf_s(KeyName, "Code%d", nIniIndex);
    WritePrivateProfileStringA(appNameByRace[raceIndex], KeyName, towerRecord->m_strCode, ".\\Script\\SystemGuardTower.ini");
    return true;
}

bool CPlayer::mgr_matchless(bool bMatchless)
{

    if ( this->m_bCheat_Matchless == bMatchless )
      return false;
    this->m_bCheat_Matchless = bMatchless;
    return true;
}

#if 0 // duplicate implementation exists in CPlayer.cpp
bool CPlayer::mgr_MaxAttackPoint(int nMax)
{

    if ( this->m_nMaxAttackPnt == nMax )
      return false;
    this->m_nMaxAttackPnt = nMax;
    return true;
}
#endif

bool CPlayer::mgr_pass_sch_one_step()
{
  g_WorldSch.PassOneStep();
  return true;
}

bool CPlayer::mgr_recall_guild_player(char *wszDestCharName)
{
    CPlayer *targetPlayer; // [rsp+30h] [rbp-28h]
    CPlayer *guildMemberPlayer; // [rsp+38h] [rbp-20h]
    int memberIndex; // [rsp+40h] [rbp-18h]
    _guild_member_info *memberInfo; // [rsp+48h] [rbp-10h]
  if ( !wszDestCharName )
      return false;
    if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return false;
    targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, wszDestCharName);
    if ( !targetPlayer )
      return false;
    if ( targetPlayer->m_Param.m_pGuild )
    {
      guildMemberPlayer = 0LL;
      for ( memberIndex = 0; memberIndex < 50; ++memberIndex )
      {
        memberInfo = &targetPlayer->m_Param.m_pGuild->m_MemberData[memberIndex];
        if ( memberInfo->IsFill() )
        {
          if ( memberInfo->pPlayer )
          {
            guildMemberPlayer = memberInfo->pPlayer;
            if ( guildMemberPlayer->m_bLive && this->m_dwObjSerial != guildMemberPlayer->m_dwObjSerial )
              guildMemberPlayer->RecallRandomPositionInRange(
                this->m_pCurMap,
                this->m_wMapLayerIndex,
                this->m_fCurPos,
                200);
          }
        }
      }
      return true;
    }
    else if ( this->m_dwObjSerial == targetPlayer->m_dwObjSerial )
    {
      return false;
    }
    else
    {
      targetPlayer->RecallRandomPositionInRange(
        
        this->m_pCurMap,
        this->m_wMapLayerIndex,
        this->m_fCurPos,
        200);
      return true;
    }
}

bool CPlayer::mgr_recall_mon(char *pszMonCode, int nCreateNum)
{
    int createIndex; // [rsp+54h] [rbp-14h]
    int createCount; // [rsp+80h] [rbp+18h]

    createCount = nCreateNum;
  if ( this->m_pCurMap->m_pMapSet->m_nMapType == 1 )
      return false;
    if ( nCreateNum > 100 )
      createCount = 100;
    for ( createIndex = 0;
          createIndex < createCount
       && CreateRepMonster(this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, pszMonCode, 0LL, 0, 1, 0, 0, 1);
          ++createIndex )
    {
    }
    return true;
}

bool CPlayer::mgr_recall_party_player(char *wszDestCharName)
{
    CPlayer *targetPlayer; // [rsp+30h] [rbp-28h]
    CPartyPlayer **partyMembers; // [rsp+38h] [rbp-20h]
    CPlayer *partyMemberPlayer; // [rsp+40h] [rbp-18h]
    int memberIndex; // [rsp+48h] [rbp-10h]
  if ( !wszDestCharName )
      return false;
    if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return false;
    targetPlayer = GetPtrPlayerFromName(g_Player, MAX_PLAYER, wszDestCharName);
    if ( !targetPlayer )
      return false;
    if ( targetPlayer->m_pPartyMgr->IsPartyMode() )
    {
      partyMembers = targetPlayer->m_pPartyMgr->GetPtrPartyMember();
      if ( partyMembers )
      {
        partyMemberPlayer = 0LL;
        for ( memberIndex = 0; memberIndex < 8 && partyMembers[memberIndex]; ++memberIndex )
        {
          partyMemberPlayer = &g_Player[partyMembers[memberIndex]->m_wZoneIndex];
          if ( partyMemberPlayer->m_bLive && this->m_dwObjSerial != partyMemberPlayer->m_dwObjSerial )
            partyMemberPlayer->RecallRandomPositionInRange(
              this->m_pCurMap,
              this->m_wMapLayerIndex,
              this->m_fCurPos,
              120);
        }
        return true;
      }
      else
      {
        return false;
      }
    }
    else if ( this->m_dwObjSerial == targetPlayer->m_dwObjSerial )
    {
      return false;
    }
    else
    {
      targetPlayer->RecallRandomPositionInRange(
        
        this->m_pCurMap,
        this->m_wMapLayerIndex,
        this->m_fCurPos,
        120);
      return true;
    }
}

bool CPlayer::mgr_recall_player(char *pwszCharName)
{
    const char *sourceCharacterName; // rax
    CUserDB *targetUserDb; // [rsp+20h] [rbp-18h]
    CPlayer *targetPlayer; // [rsp+28h] [rbp-10h]
  // non-IDA: allow recall regardless of current map type.
    targetUserDb = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
    if ( !targetUserDb )
      return false;
    targetPlayer = &g_Player[targetUserDb->m_idWorld.wIndex];
    if ( !targetPlayer->m_bLive )
      return false;
    sourceCharacterName = this->m_Param.GetCharNameW();
    targetPlayer->pc_GotoAvatorRequest( sourceCharacterName);
    return true;
}

bool CPlayer::mgr_resurrect_player(char *pwszCharName)
{
    CUserDB *targetUserDb; // [rsp+20h] [rbp-18h]
    CPlayer *targetPlayer; // [rsp+28h] [rbp-10h]
  targetUserDb = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
    if ( !targetUserDb )
      return 0;
    targetPlayer = &g_Player[targetUserDb->m_idWorld.wIndex];
    return targetPlayer->m_bLive && targetPlayer->pc_Resurrect( 0);
}

#if 0 // duplicate implementation exists in CPlayer.cpp
bool CPlayer::mgr_set_animus_attack_point(int nPoint)
{

    if ( !this->m_pUserDB )
      return false;
    if ( this->m_nAnimusAttackPnt == nPoint )
      return false;
    this->m_nAnimusAttackPnt = nPoint;
    if ( this->m_pRecalledAnimusChar )
      this->m_pRecalledAnimusChar->m_nMaxAttackPnt = nPoint;
    return true;
}
#endif

bool CPlayer::mgr_TrunkInit()
{
    int trunkSlotIndex; // [rsp+30h] [rbp-18h]
    int extTrunkSlotIndex; // [rsp+34h] [rbp-14h]
  for ( trunkSlotIndex = 0; ; ++trunkSlotIndex )
    {
      const unsigned __int8 trunkSlotCount = this->m_Param.GetTrunkSlotNum();
      if ( trunkSlotIndex >= trunkSlotCount )
        break;
      if ( this->m_Param.m_pStoragePtr[5]->m_pStorageList[trunkSlotIndex].m_bLoad
        && !this->Emb_DelStorage( 5u, trunkSlotIndex, 0, 0, "CPlayer::mgr_TrunkInit()") )
      {
        return false;
      }
    }
    this->m_Param.m_byTrunkSlotNum = 0;
    this->m_Param.m_dbTrunk.SetUseListNum(0);
    this->m_pUserDB->Update_TrunkSlotNum(this->m_Param.GetTrunkSlotNum());
    for ( extTrunkSlotIndex = 0; ; ++extTrunkSlotIndex )
    {
      const unsigned __int8 extTrunkSlotCount = this->m_Param.GetExtTrunkSlotNum();
      if ( extTrunkSlotIndex >= extTrunkSlotCount )
        break;
      if ( this->m_Param.m_pStoragePtr[7]->m_pStorageList[extTrunkSlotIndex].m_bLoad )
        this->Emb_DelStorage( 7u, extTrunkSlotIndex, 0, 0, "CPlayer::mgr_TrunkInit()");
    }
    this->m_Param.m_byExtTrunkSlotNum = 0;
    this->m_Param.m_dbExtTrunk.SetUseListNum(0);
    this->m_pUserDB->Update_ExtTrunkSlotNum(this->m_Param.GetExtTrunkSlotNum());
    return true;
}

bool CPlayer::mgr_user_ban(char *uszCharName, int iPeriod, char *uszReason, char byBlockType)
{
    const char *gmCharacterName; // rax
    CUserDB *targetUserDb; // [rsp+40h] [rbp-168h]
    char blockType; // [rsp+60h] [rbp-148h] BYREF
    int blockPeriod; // [rsp+61h] [rbp-147h]
    unsigned __int8 targetGlobalId[8]; // [rsp+65h] [rbp-143h] BYREF
    unsigned __int8 gmWorldId[6]; // [rsp+6Dh] [rbp-13Bh] BYREF
    char reasonText[32]; // [rsp+73h] [rbp-135h] BYREF
    char gmName[32]; // [rsp+93h] [rbp-115h] BYREF
    unsigned int accountSerial; // [rsp+B3h] [rbp-F5h]
    unsigned __int8 messageType[44]; // [rsp+D4h] [rbp-D4h] BYREF
    char chatMessage[128]; // [rsp+100h] [rbp-A8h] BYREF
  targetUserDb = SearchAvatorWithName(g_UserDB, MAX_PLAYER, uszCharName);
    if ( targetUserDb )
    {
      targetUserDb->SetChatLock( 1);
      blockType = byBlockType;
      blockPeriod = iPeriod;
      accountSerial = targetUserDb->m_dwAccountSerial;
      std::memcpy(targetGlobalId, &targetUserDb->m_gidGlobal, sizeof(targetGlobalId));
      std::memcpy(gmWorldId, &this->m_pUserDB->m_idWorld, sizeof(gmWorldId));
      strncpy(reasonText, uszReason, 31);
      gmCharacterName = this->m_Param.GetCharNameW();
      strncpy(gmName, gmCharacterName, 31);
      std::memcpy(messageType, "2n", 2);
      g_Network.m_pProcess[1]->LoadSendMsg( 0, messageType, &blockType, 87);
      return true;
    }
    else
    {
      sprintf_s(chatMessage, sizeof(chatMessage), "%s is not connected", uszCharName);
      this->SendData_ChatTrans( 0, 0xFFFFFFFF, 0xFFu, 0, chatMessage, 0xFFu, 0LL);
      return false;
    }
}

bool CPlayer::mgr_whisper(char *pwszMsg)
{
    int playerIndex; // [rsp+40h] [rbp-18h]
    char *senderName; // [rsp+48h] [rbp-10h]
  for ( playerIndex = 0; playerIndex < MAX_PLAYER; ++playerIndex )
    {
      if ( g_UserDB[playerIndex].m_bActive && g_UserDB[playerIndex].m_byUserDgr >= 2u )
      {
        senderName = this->m_Param.GetCharNameW();
        const unsigned __int8 raceCode = static_cast<unsigned char>(this->m_Param.GetRaceCode());
        g_Player[playerIndex].SendData_ChatTrans(
          
          5u,
          this->m_dwObjSerial,
          raceCode,
          0,
          pwszMsg,
          this->m_Param.m_byPvPGrade,
          senderName);
      }
    }
    return true;
}

char loot_item(CPlayer *pOwner, char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum)
{
    int nTableCode; // [rsp+50h] [rbp-B8h]
    _base_fld *itemRecord; // [rsp+58h] [rbp-B0h]
    unsigned int itemDurability; // [rsp+60h] [rbp-A8h]
    unsigned int itemUpgradeState; // [rsp+64h] [rbp-A4h]
    unsigned __int8 upgradeSocketLimit; // [rsp+68h] [rbp-A0h]
    unsigned __int8 itemKindCode; // [rsp+69h] [rbp-9Fh]
    _ItemUpgrade_fld *upgradeRecord; // [rsp+70h] [rbp-98h]
    int upgradeTableCode; // [rsp+78h] [rbp-90h]
    _base_fld *upgradeItemRecord; // [rsp+80h] [rbp-88h]
    int createIndex; // [rsp+88h] [rbp-80h]
    _STORAGE_LIST::_db_con pItem; // [rsp+98h] [rbp-70h] BYREF
    const _TimeItem_fld *timeRecord; // [rsp+D8h] [rbp-30h]
    __time32_t currentTime[4]; // [rsp+E4h] [rbp-24h] BYREF
    int upgradeStep; // [rsp+F4h] [rbp-14h]
  nTableCode = GetItemTableCode(pszItemCode);
    if ( nTableCode == -1 )
      return 0;
    if ( nTableCode == 19 )
      return 0;
    itemRecord = g_Main.m_tblItemData[nTableCode].GetRecord( pszItemCode);
    if ( !itemRecord )
      return 0;
    itemDurability = 0;
    upgradeSocketLimit = 0;
    if ( IsOverLapItem(nTableCode) )
      itemDurability = MAX_ITEM_OVERLAP;
    else
      itemDurability = GetItemDurPoint(nTableCode, itemRecord->m_dwIndex);
    if ( nNum > 100 )
      nNum = 100;
    itemKindCode = GetItemKindCode(nTableCode);
    if ( itemKindCode )
    {
      if ( itemKindCode != 1 )
        return 0;
      itemUpgradeState = GetMaxParamFromExp(itemRecord->m_dwIndex, itemDurability);
    }
    else
    {
      upgradeSocketLimit = GetDefItemUpgSocketNum(nTableCode, itemRecord->m_dwIndex);
      itemUpgradeState = GetBitAfterSetLimSocket(upgradeSocketLimit);
    }
    upgradeRecord = 0LL;
    if ( upgradeSocketLimit && pszUpTalCode )
    {
      upgradeTableCode = GetItemTableCode(pszUpTalCode);
      if ( upgradeTableCode != 18 )
        return 0;
      upgradeItemRecord = g_Main.m_tblItemData[18].GetRecord( pszUpTalCode);
      if ( !upgradeItemRecord )
        return 0;
      upgradeRecord = g_Main.m_tblItemUpgrade.GetRecordFromRes(upgradeItemRecord->m_dwIndex);
      if ( !upgradeRecord || upgradeRecord->m_dwIndex >= 13 )
        return 0;
    }
    for ( createIndex = 0; createIndex < nNum; ++createIndex )
    {
      pItem.Init();
      pItem.m_byTableCode = nTableCode;
      pItem.m_wItemIndex = itemRecord->m_dwIndex;
      pItem.m_dwDur = itemDurability;
      timeRecord = TimeItem::FindTimeRec(nTableCode, itemRecord->m_dwIndex);
      if ( timeRecord && timeRecord->m_nCheckType )
      {
        _time32(currentTime);
        pItem.m_byCsMethod = timeRecord->m_nCheckType;
        pItem.m_dwT = timeRecord->m_nUseTime + currentTime[0];
        pItem.m_dwLendRegdTime = currentTime[0];
      }
      if ( upgradeRecord )
      {
        if ( nUpNum > upgradeSocketLimit )
          nUpNum = upgradeSocketLimit;
        for ( upgradeStep = 0; upgradeStep < nUpNum; ++upgradeStep )
          itemUpgradeState = GetBitAfterUpgrade(itemUpgradeState, upgradeRecord->m_dwIndex, upgradeStep);
      }
      pItem.m_dwLv = itemUpgradeState;
      if ( !CreateItemBox(
              &pItem,
              pOwner,
              -1,
              0,
              0LL,
              2u,
              pOwner->m_pCurMap,
              pOwner->m_wMapLayerIndex,
              pOwner->m_fCurPos,
              1) )
        return 0;
    }
    return 1;
}

