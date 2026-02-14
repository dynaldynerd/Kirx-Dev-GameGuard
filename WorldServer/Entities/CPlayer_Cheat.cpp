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
#include "ItemUpgrade_fld.h"
#include "TimeItem.h"

#include <mmsystem.h>

char loot_item(CPlayer *pOwner, char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum);

static float fPos[3] = {13.0f, -108.0f, 37.0f};
static float fTarPos[3] = {-249.0f, -1737.0f, -242.0f};

char CPlayer::dev_after_effect()
{
  _base_fld *record = g_Main.m_tblEffectData[3].GetRecord( "17");
  if (!record)
  {
    return 0;
  }

  int unusedFlag = 1;
  unsigned int curTime = _sf_continous::GetSFContCurTime();
  unsigned __int16 durSec = *reinterpret_cast<unsigned __int16 *>(&record[16].m_strCode[24]);
  bool upMty = false;
  if (!this->InsertSFContEffect(
        static_cast<unsigned __int8>(record[13].m_strCode[32]),
        3,
        record->m_dwIndex,
        durSec,
        1,
        &upMty,
        nullptr))
  {
    this->m_bAfterEffect = 1;
  }
return 1;
}

char CPlayer::dev_all_kill()
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

  return 1;
}

char CPlayer::dev_animus_recall_time_free(bool bFree)
{
  if (this->m_bFreeRecallWaitTime == bFree)
  {
    return 0;
  }

  this->m_bFreeRecallWaitTime = bFree;
  char szMsg[32]{};
  szMsg[0] = static_cast<char>(bFree);

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 22;
  pbyType[1] = 13;
  g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, pbyType, szMsg, 1u);
  return 1;
}

char CPlayer::dev_avator_copy(char *pwszDstName)
{
  if (!this->m_pUserDB)
  {
    return 0;
  }

  CUserDB *dstUser = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszDstName);
  if (!dstUser)
  {
    return 0;
  }

  CPlayer *dstPlayer = &g_Player[dstUser->m_idWorld.wIndex];
  if (!dstPlayer->m_bLive)
  {
    return 0;
  }

  const int dstRaceSex = dstPlayer->m_Param.GetRaceSexCode();
  const int srcRaceSex = this->m_Param.GetRaceSexCode();
  if (dstRaceSex != srcRaceSex)
  {
    return 0;
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
  return 1;
}

bool CPlayer::dev_change_class(char *pszClassCode)
{
  return this->m_pUserDB && this->m_pUserDB->Setting_Class(pszClassCode);
}

char CPlayer::dev_cont_effect_del()
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
  return 1;
}

char CPlayer::dev_cont_effect_time(unsigned int dwSec)
{

    if ( dwSec )
    {
      if ( dwSec <= 0x2328 )
        this->m_nContEffectSec = dwSec;
      else
        this->m_nContEffectSec = 9000;
    }
    else
    {
      this->m_nContEffectSec = -1;
    }
    return 1;
}

char CPlayer::dev_dalant(unsigned int dwDalant)
{
  unsigned int dalant = dwDalant;
  if (dwDalant == static_cast<unsigned int>(-1))
  {
    dalant = 2000000000;
  }
  else if (dwDalant > 0x77359400)
  {
    dalant = 2000000000;
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
  return 1;
}

char CPlayer::dev_die()
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
  return 1;
}

char CPlayer::dev_drop_item(char *pszItemCode, int nNum, char *pszUpTalCode)
{
  _STORAGE_LIST::_db_con items[100];
  unsigned __int8 itemCount = 0;

  const int tableCode = GetItemTableCode(pszItemCode);
  if (tableCode == -1 || tableCode != 18)
  {
    return 0;
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

    if (!strcmp_0(record->m_strCode, pszItemCode))
    {
      this->SendMsg_DeleteStorageInform(0, src->m_wSerial);
      this->Emb_DelStorage(0, j, 0, 1, "CPlayer::dev_inven_empty()");
    }
    memcpy_0(&items[itemCount++], src, sizeof(_STORAGE_LIST::_db_con));
  }

  CPlayer::s_MgrItemHistory.cheat_del_item(
    this->m_ObjID.m_wIndex,
    items,
    itemCount,
    this->m_szItemHistoryFileName);
  return 1;
}

char CPlayer::dev_free_sf_by_class()
{

    this->m_bFreeSFByClass = 1;
    return 1;
}

char CPlayer::dev_full_animus_gauge()
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
      return 0;
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
    unsigned __int8 msgHp[4]{};
    unsigned __int8 msgFp[4]{};
    *reinterpret_cast<unsigned __int16 *>(msgHp) = animus->m_wSerial;
    *reinterpret_cast<unsigned __int16 *>(msgHp + 2) = *reinterpret_cast<unsigned __int16 *>(levelPtr);
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, typeHp, reinterpret_cast<char *>(msgHp), 4u);

    *reinterpret_cast<unsigned __int16 *>(msgFp) = animus->m_wSerial;
    *reinterpret_cast<unsigned __int16 *>(msgFp + 2) = *(reinterpret_cast<unsigned __int16 *>(levelPtr) + 1);
    g_Network.m_pProcess[0]->LoadSendMsg(this->m_ObjID.m_wIndex, typeFp, reinterpret_cast<char *>(msgFp), 4u);
  }
  return 1;
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
      src.m_dwDur = 0xFFFFFFuLL;
      src.m_wSerial = this->m_Param.GetNewItemSerial();
      src.m_dwLv = 0xFFFFFFF;
      if (this->Emb_AddStorage(3u, &src, 1, 1))
      {
        memcpy_0(&items[count++], &src, sizeof(_STORAGE_LIST::_db_con));
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

char CPlayer::dev_full_point()
{
  const int maxHp = this->GetMaxHP();
  this->SetHP(maxHp, false);
  const int maxFp = this->GetMaxFP();
  this->SetFP(maxFp, false);
  const int maxSp = this->GetMaxSP();
  this->SetSP(maxSp, false);
  this->SendMsg_Recover();
  return 1;
}

char CPlayer::dev_gold(unsigned int dwGold)
{
  unsigned int gold = dwGold;
  if (dwGold == static_cast<unsigned int>(-1))
  {
    gold = 500000;
  }
  else if (dwGold > 0x7A120)
  {
    gold = 500000;
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
  return 1;
}

char CPlayer::dev_goto_monster(CMonster *pMon)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    unsigned __int8 MapCode; // al
    __int64 v6; // [rsp+0h] [rbp-48h] BYREF
    float *m_fCurPos; // [rsp+30h] [rbp-18h]
  if ( !pMon || !pMon->m_bLive || !pMon->m_bOper )
      return 0;
    this->OutOfMap( pMon->m_pCurMap, pMon->m_wMapLayerIndex, 3u, pMon->m_fCurPos);
    m_fCurPos = pMon->m_fCurPos;
    MapCode = this->m_Param.GetMapCode();
    this->SendMsg_GotoRecallResult(0, MapCode, m_fCurPos, 4u);
    return 1;
}

char CPlayer::dev_goto_npc(CMerchant *pNpc)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    unsigned __int8 MapCode; // al
    __int64 v6; // [rsp+0h] [rbp-48h] BYREF
    float *m_fCurPos; // [rsp+30h] [rbp-18h]
  if ( !pNpc || !pNpc->m_bLive || pNpc->m_bCorpse )
      return 0;
    this->OutOfMap( pNpc->m_pCurMap, pNpc->m_wMapLayerIndex, 3u, pNpc->m_fCurPos);
    m_fCurPos = pNpc->m_fCurPos;
    MapCode = this->m_Param.GetMapCode();
    this->SendMsg_GotoRecallResult(0, MapCode, m_fCurPos, 4u);
    return 1;
}

char CPlayer::dev_half_inven_amount(unsigned __int64 dwAmount)
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
    if (IsOverLapItem(item->m_byTableCode) && amount > 0x63)
    {
      amount = 99;
    }

    item->m_dwDur = amount;
    this->SendMsg_AlterItemDurInform(0, item->m_wSerial, amount);
    if (this->m_pUserDB)
    {
      this->m_pUserDB->Update_ItemDur(0, j, amount, true);
    }
  }
  return 1;
}

char CPlayer::dev_half_point()
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
  return 1;
}

char CPlayer::dev_init_monster()
{
  g_Main.gm_MonsterInit(static_cast<CMonster *>(this->m_TargetObject.pObject));
  return 1;
}

char CPlayer::dev_inven_empty()
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
      memcpy_0(&items[count++], src, sizeof(_STORAGE_LIST::_db_con));
    }
  }

  CPlayer::s_MgrItemHistory.cheat_del_item(
    this->m_ObjID.m_wIndex,
    items,
    count,
    this->m_szItemHistoryFileName);
  return 1;
}

char CPlayer::dev_item_make_no_use_matrial(bool noUsingMatrial)
{
  if (this->m_bCheat_makeitem_no_use_matrial == noUsingMatrial)
  {
    return 0;
  }

  this->m_bCheat_makeitem_no_use_matrial = noUsingMatrial;
  this->SendMsg_MakeItemCheatSendButtonEnable(this->m_bCheat_makeitem_no_use_matrial);
  return 1;
}

char CPlayer::dev_loot_bag()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    int RecordNum; // eax
    int RaceSexCode; // eax
    __int64 v6; // [rsp+0h] [rbp-58h] BYREF
    int nItemIndex; // [rsp+30h] [rbp-28h]
    char *ItemEquipCivil; // [rsp+38h] [rbp-20h]
    _base_fld *Record; // [rsp+40h] [rbp-18h]
  for ( nItemIndex = 0; ; ++nItemIndex )
    {
      RecordNum = g_Main.m_tblItemData[12].GetRecordNum();
      if ( nItemIndex >= RecordNum )
        break;
      ItemEquipCivil = GetItemEquipCivil(12, nItemIndex);
      RaceSexCode = this->m_Param.GetRaceSexCode();
      if ( ItemEquipCivil[RaceSexCode] == 49 )
      {
        Record = g_Main.m_tblItemData[12].GetRecord( nItemIndex);
        loot_item(this, Record->m_strCode, 4, 0LL, 0);
        return 1;
      }
    }
    return 1;
}

char CPlayer::dev_loot_free(bool bFree)
{

    if ( this->m_bLootFree == bFree )
      return 0;
    this->m_bLootFree = bFree;
    return 1;
}

char CPlayer::dev_loot_fullitem(unsigned __int8 byLv)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-88h] BYREF
    char Buffer[32]; // [rsp+38h] [rbp-50h] BYREF
    _base_fld *Record; // [rsp+58h] [rbp-30h]
    int j; // [rsp+60h] [rbp-28h]
  memset(Buffer, 0, 9);
    sprintf(Buffer, "%s_%d", this->m_Param.m_pClassData->m_strCode, byLv);
    Record = g_Main.m_tblEditData.GetRecord( Buffer);
    if ( !Record )
      return 0;
    for ( j = 0; j < 30; ++j )
    {
      if ( *(int *)&Record[j + 1].m_strCode[60] > 0 )
        this->dev_loot_item( (char *)&Record[j + 1], *(_DWORD *)&Record[j + 1].m_strCode[60], 0LL, 0);
    }
    return 1;
}

bool CPlayer::dev_loot_item(char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum)
{

    __int64 *v5; // rdi
    __int64 i; // rcx
    __int64 v8; // [rsp+0h] [rbp-38h] BYREF
  return loot_item(this, pszItemCode, nNum, pszUpTalCode, nUpNum);
}

char CPlayer::dev_loot_material()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    int RecordNum; // eax
    int v5; // eax
    int v6; // eax
    __int64 v7; // [rsp+0h] [rbp-158h] BYREF
    int nIndex; // [rsp+50h] [rbp-108h]
    _STORAGE_LIST::_db_con pItem; // [rsp+68h] [rbp-F0h] BYREF
    _STORAGE_LIST::_db_con v10; // [rsp+B8h] [rbp-A0h] BYREF
    _STORAGE_LIST::_db_con v11; // [rsp+108h] [rbp-50h] BYREF
  for ( nIndex = 0; ; ++nIndex )
    {
      RecordNum = g_Main.m_tblItemData[20].GetRecordNum();
      if ( nIndex >= RecordNum )
        break;
      pItem.Init();
      pItem.m_byTableCode = 20;
      pItem.m_wItemIndex = nIndex;
      pItem.m_dwDur = 99LL;
      pItem.m_dwLv = 0xFFFFFFF;
      if ( !CreateItemBox(
              &pItem,
              this,
              0xFFFFFFFF,
              0,
              0LL,
              2u,
              this->m_pCurMap,
              this->m_wMapLayerIndex,
              this->m_fCurPos,
              1) )
        return 1;
    }
    for ( nIndex = 0; ; ++nIndex )
    {
      v5 = g_Main.m_tblItemData[18].GetRecordNum();
      if ( nIndex >= v5 )
        break;
      v10.Init();
      v10.m_byTableCode = 18;
      v10.m_wItemIndex = nIndex;
      v10.m_dwDur = 99LL;
      v10.m_dwLv = 0xFFFFFFF;
      if ( !CreateItemBox(&v10, this, 0xFFFFFFFF, 0, 0LL, 2u, this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, 1) )
        return 1;
    }
    for ( nIndex = 0; ; ++nIndex )
    {
      v6 = g_Main.m_tblItemData[11].GetRecordNum();
      if ( nIndex >= v6 )
        break;
      v11.Init();
      v11.m_byTableCode = 11;
      v11.m_wItemIndex = nIndex;
      v11.m_dwDur = GetItemDurPoint(11, nIndex);
      v11.m_dwLv = 0xFFFFFFF;
      if ( !CreateItemBox(&v11, this, 0xFFFFFFFF, 0, 0LL, 2u, this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, 1) )
        return 1;
    }
    return 1;
}

char CPlayer::dev_loot_mine()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    int RecordNum; // eax
    int RaceSexCode; // eax
    int v5; // eax
    int v6; // eax
    __int64 v8; // [rsp+0h] [rbp-68h] BYREF
    int n; // [rsp+30h] [rbp-38h]
    _base_fld *Record; // [rsp+38h] [rbp-30h]
    char *ItemEquipCivil; // [rsp+40h] [rbp-28h]
    _base_fld *v12; // [rsp+48h] [rbp-20h]
    char *v13; // [rsp+50h] [rbp-18h]
  for ( n = 0; ; ++n )
    {
      RecordNum = g_Main.m_tblItemData[6].GetRecordNum();
      if ( n >= RecordNum )
        break;
      Record = g_Main.m_tblItemData[6].GetRecord( n);
      if ( Record[1].m_dwIndex && *(_DWORD *)&Record[6].m_strCode[8] == 10 )
      {
        ItemEquipCivil = GetItemEquipCivil(6, n);
        RaceSexCode = this->m_Param.GetRaceSexCode();
        if ( ItemEquipCivil[RaceSexCode] == 49 )
          loot_item(this, Record->m_strCode, 1, 0LL, 0);
      }
    }
    for ( n = 0; ; ++n )
    {
      v5 = g_Main.m_tblItemData[16].GetRecordNum();
      if ( n >= v5 )
        break;
      v12 = g_Main.m_tblItemData[16].GetRecord( n);
      if ( v12[1].m_dwIndex )
      {
        v13 = GetItemEquipCivil(16, n);
        if ( v13 )
        {
          v6 = this->m_Param.GetRaceSexCode();
          if ( v13[v6] == 49 )
            loot_item(this, v12->m_strCode, 1, 0LL, 0);
        }
      }
    }
    return 1;
}

char CPlayer::dev_loot_tower()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    __int64 v4; // [rsp+0h] [rbp-B8h] BYREF
    int RecordNum; // [rsp+50h] [rbp-68h]
    int n; // [rsp+54h] [rbp-64h]
    _base_fld *Record; // [rsp+58h] [rbp-60h]
    _STORAGE_LIST::_db_con pItem; // [rsp+68h] [rbp-50h] BYREF
    int j; // [rsp+A4h] [rbp-14h]
  RecordNum = g_Main.m_tblItemData[25].GetRecordNum();
    for ( n = 0; n < RecordNum; ++n )
    {
      Record = g_Main.m_tblItemData[25].GetRecord( n);
      if ( IsExistItem(25, n) && IsGroundableItem(25, n) )
      {
        pItem.Init();
        pItem.m_byTableCode = 25;
        pItem.m_wItemIndex = n;
        pItem.m_dwDur = GetItemDurPoint(25, n);
        pItem.m_dwLv = 0xFFFFFFF;
        if ( !CreateItemBox(
                &pItem,
                this,
                0xFFFFFFFF,
                0,
                0LL,
                2u,
                this->m_pCurMap,
                this->m_wMapLayerIndex,
                this->m_fCurPos,
                1) )
          return 1;
        for ( j = 0; j < 3 && loot_item(this, &Record[j + 7].m_strCode[4], 1, 0LL, 0); ++j )
          ;
      }
    }
    return 1;
}

char CPlayer::dev_lv(int nLv)
{
  if (nLv <= 0)
  {
    return 0;
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
  return 1;
}

char CPlayer::dev_make_succ(bool bSucc)
{

    if ( this->m_bCheat_100SuccMake == bSucc )
      return 0;
    this->m_bCheat_100SuccMake = bSucc;
    return 1;
}

char CPlayer::dev_max_level_ext(unsigned __int8 byMaxLevel)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    int Level; // eax
    __int64 v6; // [rsp+0h] [rbp-38h] BYREF
    int v7; // [rsp+20h] [rbp-18h]
  this->m_Param.SetMaxLevel( byMaxLevel);
    if ( this->m_pUserDB )
      this->m_pUserDB->Update_MaxLevel( byMaxLevel);
    v7 = byMaxLevel;
    Level = this->m_Param.GetLevel();
    if ( v7 < Level )
    {
      this->m_Param.SetExp( 0.0);
      this->m_dwExpRate = 0;
      if ( byMaxLevel )
        this->SetLevelD( byMaxLevel);
      else
        this->SetLevelD( 0x32u);
    }
    return 1;
}

char CPlayer::dev_never_die(bool bSet)
{

    if ( this->m_bNeverDie == bSet )
      return 0;
    this->m_bNeverDie = bSet;
    return 1;
}

char CPlayer::dev_quest_complete()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    __int64 v4; // [rsp+0h] [rbp-48h] BYREF
    int j; // [rsp+20h] [rbp-28h]
    _QUEST_DB_BASE::_LIST *pSlotData; // [rsp+28h] [rbp-20h]
    int k; // [rsp+30h] [rbp-18h]
    _base_fld *Record; // [rsp+38h] [rbp-10h]
  if ( !this->m_pUserDB )
      return 0;
    for ( j = 0; ; ++j )
    {
      if ( j >= 30 )
        return 0;
      pSlotData = &this->m_Param.m_QuestDB.m_List[j];
      if ( pSlotData->byQuestType != 255 )
        break;
    }
    for ( k = 0; k < 3; ++k )
      pSlotData->wNum[k] = -1;
    pSlotData->dwPassSec = -1;
    this->m_pUserDB->Update_QuestUpdate( j, pSlotData, 1);
    Record = CQuestMgr::s_tblQuest->GetRecord( pSlotData->wIndex);
    if ( *(_DWORD *)&Record[13].m_strCode[60] || *(_DWORD *)&Record[1].m_strCode[24] )
      this->SendMsg_SelectQuestReward( j);
    else
      this->Emb_CompleteQuest( j, 0xFFu, 0xFFu);
    return 1;
}

char CPlayer::dev_quest_complete_other(char *pwszCharName)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-58h] BYREF
    CUserDB *v6; // [rsp+20h] [rbp-38h]
    CPlayer *v7; // [rsp+28h] [rbp-30h]
    int j; // [rsp+30h] [rbp-28h]
    _QUEST_DB_BASE::_LIST *pSlotData; // [rsp+38h] [rbp-20h]
    int k; // [rsp+40h] [rbp-18h]
    _base_fld *Record; // [rsp+48h] [rbp-10h]
  v6 = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
    if ( !v6 )
      return 0;
    v7 = &g_Player[v6->m_idWorld.wIndex];
    if ( !v7 )
      return 0;
    for ( j = 0; ; ++j )
    {
      if ( j >= 30 )
        return 0;
      pSlotData = &v7->m_Param.m_QuestDB.m_List[j];
      if ( pSlotData->byQuestType != 255 )
        break;
    }
    for ( k = 0; k < 3; ++k )
      pSlotData->wNum[k] = -1;
    pSlotData->dwPassSec = -1;
    v6->Update_QuestUpdate( j, pSlotData, 1);
    Record = CQuestMgr::s_tblQuest->GetRecord( pSlotData->wIndex);
    if ( *(_DWORD *)&Record[13].m_strCode[60] || *(_DWORD *)&Record[1].m_strCode[24] )
      v7->SendMsg_SelectQuestReward( j);
    else
      v7->Emb_CompleteQuest( j, 0xFFu, 0xFFu);
    return 1;
}

char CPlayer::dev_set_animus_exp(unsigned __int64 dwExpPoint)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-98h] BYREF
    int j; // [rsp+30h] [rbp-68h]
    char szMsg[2]; // [rsp+48h] [rbp-50h] BYREF
    unsigned __int64 v8; // [rsp+4Ah] [rbp-4Eh]
    unsigned __int8 pbyType[36]; // [rsp+74h] [rbp-24h] BYREF
  if ( !this->m_pUserDB )
      return 0;
    for ( j = 0; j < 4; ++j )
    {
      if ( this->m_Param.m_dbAnimus.m_pStorageList[j].m_bLoad
        && !this->m_Param.m_dbAnimus.m_pStorageList[j].m_bLock
        && this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwDur != dwExpPoint )
      {
        this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwDur = dwExpPoint;
        if ( this->m_pUserDB )
          this->m_pUserDB->Update_ItemDur( 4u, j, dwExpPoint, 0);
        *(_WORD *)szMsg = this->m_Param.m_dbAnimus.m_pStorageList[j].m_wSerial;
        v8 = dwExpPoint;
        pbyType[0] = 22;
        pbyType[1] = 11;
        g_Network.m_pProcess[0]->LoadSendMsg( this->m_ObjID.m_wIndex, pbyType, szMsg, 0xAu);
      }
    }
    return 1;
}

char CPlayer::dev_set_animus_lv(int nAnimusLv)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-128h] BYREF
    _animus_fld *v6; // [rsp+30h] [rbp-F8h]
    _animus_fld *AnimusFldFromLv; // [rsp+38h] [rbp-F0h]
    unsigned int *p_m_dwLv; // [rsp+40h] [rbp-E8h]
    int j; // [rsp+48h] [rbp-E0h]
    char szMsg[2]; // [rsp+58h] [rbp-D0h] BYREF
    unsigned __int64 m_dwDur; // [rsp+5Ah] [rbp-CEh]
    unsigned __int8 pbyType[32]; // [rsp+84h] [rbp-A4h] BYREF
    char v13[2]; // [rsp+A4h] [rbp-84h] BYREF
    __int16 v14; // [rsp+A6h] [rbp-82h]
    unsigned __int8 v15[32]; // [rsp+C4h] [rbp-64h] BYREF
    char v16[2]; // [rsp+E4h] [rbp-44h] BYREF
    __int16 v17; // [rsp+E6h] [rbp-42h]
    unsigned __int8 v18[36]; // [rsp+104h] [rbp-24h] BYREF
  if ( !this->m_pUserDB )
      return 0;
    if ( nAnimusLv > this->m_Param.GetMaxLevel() || !nAnimusLv )
      return 0;
    v6 = 0LL;
    AnimusFldFromLv = 0LL;
    p_m_dwLv = 0LL;
    for ( j = 0; j < 4; ++j )
    {
      if ( this->m_Param.m_dbAnimus.m_pStorageList[j].m_bLoad && !this->m_Param.m_dbAnimus.m_pStorageList[j].m_bLock )
      {
        p_m_dwLv = &this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwLv;
        if ( nAnimusLv == 1 )
        {
          this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwDur = 0LL;
          AnimusFldFromLv = GetAnimusFldFromLv(this->m_Param.m_dbAnimus.m_pStorageList[j].m_wItemIndex, 1u);
          *(_WORD *)p_m_dwLv = AnimusFldFromLv->m_nMaxHP;
          *((_WORD *)p_m_dwLv + 1) = AnimusFldFromLv->m_nMaxFP;
        }
        else
        {
          v6 = GetAnimusFldFromLv(this->m_Param.m_dbAnimus.m_pStorageList[j].m_wItemIndex, nAnimusLv - 1);
          AnimusFldFromLv = GetAnimusFldFromLv(this->m_Param.m_dbAnimus.m_pStorageList[j].m_wItemIndex, nAnimusLv);
          if ( !v6 || !AnimusFldFromLv )
            return 0;
          this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwDur = v6->m_nForLvUpExp + 1;
          *(_WORD *)p_m_dwLv = AnimusFldFromLv->m_nMaxHP;
          *((_WORD *)p_m_dwLv + 1) = AnimusFldFromLv->m_nMaxFP;
        }
        if ( this->m_pUserDB )
          this->m_pUserDB->Update_ItemDur( 4u, j, this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwDur, 0);
        *(_WORD *)szMsg = this->m_Param.m_dbAnimus.m_pStorageList[j].m_wSerial;
        m_dwDur = this->m_Param.m_dbAnimus.m_pStorageList[j].m_dwDur;
        pbyType[0] = 22;
        pbyType[1] = 11;
        g_Network.m_pProcess[0]->LoadSendMsg( this->m_ObjID.m_wIndex, pbyType, szMsg, 0xAu);
        *(_WORD *)v13 = this->m_Param.m_dbAnimus.m_pStorageList[j].m_wSerial;
        v14 = *(_WORD *)p_m_dwLv;
        v15[0] = 22;
        v15[1] = 9;
        g_Network.m_pProcess[0]->LoadSendMsg( this->m_ObjID.m_wIndex, v15, v13, 4u);
        *(_WORD *)v16 = this->m_Param.m_dbAnimus.m_pStorageList[j].m_wSerial;
        v17 = *((_WORD *)p_m_dwLv + 1);
        v18[0] = 22;
        v18[1] = 10;
        g_Network.m_pProcess[0]->LoadSendMsg( this->m_ObjID.m_wIndex, v18, v16, 4u);
      }
    }
    return 1;
}

char CPlayer::dev_set_hp(float prob)
{
  if (this->IsRidingUnit())
  {
    if (!this->m_pUsingUnit)
    {
      return 0;
    }

    const unsigned __int8 byFrame = this->m_pUsingUnit->byFrame;
    _base_fld *record = g_Main.m_tblUnitFrame.GetRecord(byFrame);
    if (!record)
    {
      return 0;
    }

    const int maxGauge = *reinterpret_cast<int *>(record[1].m_strCode);
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
  return 1;
}

char CPlayer::dev_SetGuildGrade(unsigned __int8 byGrade)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-38h] BYREF
    CGuild *m_pGuild; // [rsp+20h] [rbp-18h]
  if ( !this->m_Param.m_pGuild )
      return 0;
    m_pGuild = 0LL;
    m_pGuild = this->m_Param.m_pGuild;
    if ( !byGrade || byGrade > 8u )
      return 0;
    m_pGuild->UpdateGrade(byGrade);
    return 1;
}

char CPlayer::dev_SetGuildGradeByGuildSerial(unsigned int dwGuildSerial, unsigned __int8 byGrade)
{

    __int64 *v3; // rdi
    __int64 i; // rcx
    __int64 v6; // [rsp+0h] [rbp-38h] BYREF
    CGuild *GuildDataFromSerial; // [rsp+20h] [rbp-18h]
  GuildDataFromSerial = 0LL;
    GuildDataFromSerial = GetGuildDataFromSerial(g_Guild, 500, dwGuildSerial);
    if ( !GuildDataFromSerial )
      return 0;
    if ( !byGrade || byGrade > 8u )
      return 0;
    if ( !dwGuildSerial )
      return 0;
    GuildDataFromSerial->UpdateGrade(byGrade);
    return 1;
}

char CPlayer::dev_SetGuildGradeByName(char *uszGuildName, unsigned __int8 byGrade)
{

    __int64 *v3; // rdi
    __int64 i; // rcx
    __int64 v6; // [rsp+0h] [rbp-38h] BYREF
    CGuild *GuildPtrFromName; // [rsp+20h] [rbp-18h]
  GuildPtrFromName = 0LL;
    GuildPtrFromName = GetGuildPtrFromName(g_Guild, 500, uszGuildName);
    if ( !GuildPtrFromName )
      return 0;
    if ( !byGrade || byGrade > 8u )
      return 0;
    if ( !uszGuildName )
      return 0;
    GuildPtrFromName->UpdateGrade(byGrade);
    return 1;
}

#if 0 // duplicate implementation exists in CPlayer.cpp
char CPlayer::dev_trap_attack_grade(int nPoint)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-38h] BYREF
    int j; // [rsp+20h] [rbp-18h]
  this->m_nTrapMaxAttackPnt = nPoint;
    for ( j = 0; j < 20; ++j )
    {
      if (this->m_pmTrp.m_Item[j].isLoad())
        this->m_pmTrp.m_Item[j].pItem->m_nTrapMaxAttackPnt = nPoint;
    }
    return 1;
}
#endif

char CPlayer::dev_up_all(int nCum)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-98h] BYREF
    unsigned int dwStatIndex; // [rsp+20h] [rbp-78h]
    _DWORD v7[12]; // [rsp+38h] [rbp-60h] BYREF
    _base_fld *Record; // [rsp+68h] [rbp-30h]
    int n; // [rsp+70h] [rbp-28h]
    _base_fld *v10; // [rsp+78h] [rbp-20h]
    unsigned int dwNewCum; // [rsp+80h] [rbp-18h]
    float fVal; // [rsp+84h] [rbp-14h]
  if ( nCum < 0 )
      return 0;
    for ( dwStatIndex = 0; (int)dwStatIndex < 2; ++dwStatIndex )
    {
      this->Emb_UpdateStat( dwStatIndex, nCum, 0);
      this->m_pmMst.UpdateCumPerMast(0, dwStatIndex, nCum);
      this->SendMsg_StatInform( dwStatIndex, nCum, 0);
    }
    this->Emb_UpdateStat( 2u, nCum, 0);
    this->m_pmMst.UpdateCumPerMast(1u, 0, nCum);
    this->SendMsg_StatInform( 2u, nCum, 0);
    this->Emb_UpdateStat( 3u, nCum, 0);
    this->m_pmMst.UpdateCumPerMast(2u, 0, nCum);
    this->SendMsg_StatInform( 3u, nCum, 0);
    memset(v7, 0, 32);
    Record = 0LL;
    for ( n = 0; n < 48; ++n )
    {
      Record = _MASTERY_PARAM::s_pSkillData->GetRecord( n);
      if ( Record && *(_DWORD *)&Record[1].m_strCode[4] < 8u )
      {
        if ( strncmp(Record->m_strCode, "FF", 2uLL) )
          ++v7[*(int *)&Record[1].m_strCode[4]];
      }
    }
    for ( n = 0; n < 8; ++n )
      this->m_pmMst.m_dwSkillMasteryCum[n] = 0;
    for ( dwStatIndex = 0; (int)dwStatIndex < 48; ++dwStatIndex )
    {
      v10 = _MASTERY_PARAM::s_pSkillData->GetRecord( dwStatIndex);
      if ( v10 && strncmp(v10->m_strCode, "FF", 2uLL) )
      {
        dwNewCum = nCum;
        if ( (int)v7[*(int *)&v10[1].m_strCode[4]] > 0 )
        {
          fVal = (float)(int)dwNewCum / (float)(int)v7[*(int *)&v10[1].m_strCode[4]];
          dwNewCum = CalcRoundUp(fVal);
        }
        this->Emb_UpdateStat( dwStatIndex + 4, dwNewCum, 0);
        this->m_pmMst.UpdateCumPerMast(3u, dwStatIndex, dwNewCum);
        this->SendMsg_StatInform( dwStatIndex + 4, dwNewCum, 0);
      }
    }
    for ( dwStatIndex = 0; (int)dwStatIndex < 3; ++dwStatIndex )
    {
      this->Emb_UpdateStat( dwStatIndex + 76, nCum, 0);
      this->m_pmMst.UpdateCumPerMast(5u, dwStatIndex, nCum);
      this->SendMsg_StatInform( dwStatIndex + 76, nCum, 0);
    }
    this->Emb_UpdateStat( 0x4Fu, nCum, 0);
    this->m_pmMst.UpdateCumPerMast(6u, 0, nCum);
    this->SendMsg_StatInform( 0x4Fu, nCum, 0);
    for ( dwStatIndex = 0; (int)dwStatIndex < 24; ++dwStatIndex )
    {
      this->Emb_UpdateStat( dwStatIndex + 52, nCum, 0);
      this->m_pmMst.UpdateCumPerMast(4u, dwStatIndex, nCum);
      this->SendMsg_StatInform( dwStatIndex + 52, nCum, 0);
    }
    this->ReCalcMaxHFSP( 1, 0);
    return 1;
}

char CPlayer::dev_up_all_pt(int nLv)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    float v5; // xmm0_4
    float v6; // xmm0_4
    float v7; // xmm0_4
    float v8; // xmm1_4
    float v9; // xmm0_4
    float v10; // xmm0_4
    float v11; // xmm0_4
    float v12; // xmm0_4
    float v13; // xmm0_4
    float v14; // xmm1_4
    __int64 v15; // [rsp+0h] [rbp-A8h] BYREF
    unsigned int dwNewData; // [rsp+20h] [rbp-88h]
    unsigned int dwStatIndex; // [rsp+24h] [rbp-84h]
    _DWORD v18[12]; // [rsp+38h] [rbp-70h] BYREF
    _base_fld *Record; // [rsp+68h] [rbp-40h]
    int n; // [rsp+70h] [rbp-38h]
    _base_fld *v21; // [rsp+78h] [rbp-30h]
    unsigned int dwNewCum; // [rsp+80h] [rbp-28h]
    float fVal; // [rsp+84h] [rbp-24h]
    float v24; // [rsp+88h] [rbp-20h]
    float v25; // [rsp+8Ch] [rbp-1Ch]
    float v26; // [rsp+90h] [rbp-18h]
  if ( nLv <= 0 || nLv > 99 )
      return 0;
    v24 = pow(1000.0, 2);
    v5 = pow((float)nLv, 2);
    v6 = sqrt(v24 + (float)((float)(4.0 * v5) * 1000.0));
    dwNewData = (int)pow((float)(v6 + -1000.0) / 2.0, 2);
    for ( dwStatIndex = 0; (int)dwStatIndex < 2; ++dwStatIndex )
    {
      this->Emb_UpdateStat( dwStatIndex, dwNewData, 0);
      this->m_pmMst.UpdateCumPerMast(0, dwStatIndex, dwNewData);
      this->SendMsg_StatInform( dwStatIndex, dwNewData, 0);
    }
    this->Emb_UpdateStat( 2u, dwNewData, 0);
    this->m_pmMst.UpdateCumPerMast(1u, 0, dwNewData);
    this->SendMsg_StatInform( 2u, dwNewData, 0);
    v25 = pow(100.0, 2);
    v7 = pow((float)nLv, 2);
    v8 = sqrt(v25 + (float)((float)(4.0 * v7) * 100.0)) + -100.0;
    dwNewData = (int)pow(v8 / 2.0, 2);
    this->Emb_UpdateStat( 3u, dwNewData, 0);
    this->m_pmMst.UpdateCumPerMast(2u, 0, dwNewData);
    this->SendMsg_StatInform( 3u, dwNewData, 0);
    v9 = pow((float)nLv, 2);
    v10 = pow(v9, 2);
    dwNewData = (int)(float)((float)(CalcRoundUp(v10 / 10.0) - 1) + 0.0099999998);
    memset(v18, 0, 32);
    Record = 0LL;
    for ( n = 0; n < 48; ++n )
    {
      Record = _MASTERY_PARAM::s_pSkillData->GetRecord( n);
      if ( Record && *(_DWORD *)&Record[1].m_strCode[4] < 8u )
      {
        if ( strncmp(Record->m_strCode, "FF", 2uLL) )
          ++v18[*(int *)&Record[1].m_strCode[4]];
      }
    }
    for ( n = 0; n < 8; ++n )
      this->m_pmMst.m_dwSkillMasteryCum[n] = 0;
    for ( dwStatIndex = 0; (int)dwStatIndex < 48; ++dwStatIndex )
    {
      v21 = _MASTERY_PARAM::s_pSkillData->GetRecord( dwStatIndex);
      if ( v21 && strncmp(v21->m_strCode, "FF", 2uLL) )
      {
        dwNewCum = dwNewData;
        if ( (int)v18[*(int *)&v21[1].m_strCode[4]] > 0 )
        {
          fVal = (float)(int)dwNewCum / (float)(int)v18[*(int *)&v21[1].m_strCode[4]];
          dwNewCum = CalcRoundUp(fVal);
        }
        this->Emb_UpdateStat( dwStatIndex + 4, dwNewCum, 0);
        this->m_pmMst.UpdateCumPerMast(3u, dwStatIndex, dwNewCum);
        this->SendMsg_StatInform( dwStatIndex + 4, dwNewCum, 0);
      }
    }
    v11 = pow((float)nLv, 2);
    v12 = pow(v11, 2);
    dwNewData = (int)(float)((float)(CalcRoundUp(v12 / 14.0) - 1) + 0.0099999998);
    for ( dwStatIndex = 0; (int)dwStatIndex < 24; ++dwStatIndex )
    {
      this->Emb_UpdateStat( dwStatIndex + 52, dwNewData, 0);
      this->m_pmMst.UpdateCumPerMast(4u, dwStatIndex, dwNewData);
      this->SendMsg_StatInform( dwStatIndex + 52, dwNewData, 0);
    }
    for ( dwStatIndex = 0; (int)dwStatIndex < 3; ++dwStatIndex )
    {
      if ( dwStatIndex > 1 )
      {
        if ( dwStatIndex == 2 )
          dwNewData = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 10.0) + 0.89999998);
      }
      else
      {
        dwNewData = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 1.1) + 0.89999998);
      }
      this->Emb_UpdateStat( dwStatIndex + 76, dwNewData, 0);
      this->m_pmMst.UpdateCumPerMast(5u, dwStatIndex, dwNewData);
      this->SendMsg_StatInform( dwStatIndex + 76, dwNewData, 0);
    }
    if ( !(unsigned int)this->m_Param.GetRaceCode()
      || (unsigned int)this->m_Param.GetRaceCode() == 1 )
    {
      dwNewData = (int)(float)(pow((float)(nLv - 1), 2) * 15000.0);
    }
    else if ( (unsigned int)this->m_Param.GetRaceCode() == 2 )
    {
      v26 = pow(1000.0, 2);
      v13 = pow((float)nLv, 2);
      v14 = sqrt(v26 + (float)((float)(4.0 * v13) * 1000.0)) + -1000.0;
      dwNewData = (int)pow(v14 / 2.0, 2);
    }
    this->Emb_UpdateStat( 0x4Fu, dwNewData, 0);
    this->m_pmMst.UpdateCumPerMast(6u, 0, dwNewData);
    this->SendMsg_StatInform( 0x4Fu, dwNewData, 0);
    this->ReCalcMaxHFSP( 1, 0);
    return 1;
}

char CPlayer::dev_up_cashbag(long double dPoint)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-28h] BYREF
  if (this->GetLevel() < 39)
      return 0;
    if ( this->m_Param.m_pClassData->m_nGrade < 1 )
      return 0;
    this->AlterPvPCashBag( dPoint, pm_kill);
    return 1;
}

char CPlayer::dev_up_forceitem(int nCum)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-48h] BYREF
    int n; // [rsp+30h] [rbp-18h]
    _STORAGE_LIST::_db_con *v7; // [rsp+38h] [rbp-10h]
    int nUpdate; // [rsp+58h] [rbp+10h]

    nUpdate = nCum;
  if ( nCum < 0 )
      return 0;
    if ( !this->m_pUserDB )
      return 0;
    if ( nCum > 0xFFFFFF )
      nUpdate = 0xFFFFFF;
    for ( n = 0; n < 88; ++n )
    {
      v7 = &this->m_Param.m_dbForce.m_pStorageList[n];
      if ( v7->m_bLoad )
      {
        if ( !v7->m_bLock )
        {
          this->m_pUserDB->Update_ItemDur( 3u, n, nUpdate, 0);
          this->m_Param.m_pStoragePtr[3]->UpdateCurDur(n, nUpdate);
          this->SendMsg_FcitemInform( v7->m_wSerial, nUpdate);
        }
      }
    }
    return 1;
}

char CPlayer::dev_up_forcemastery(unsigned int nCum)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-38h] BYREF
    int j; // [rsp+20h] [rbp-18h]
  for ( j = 0; j < 24; ++j )
      this->Emb_UpdateStat( j + 52, nCum, 0);
    return 1;
}

char CPlayer::dev_up_mastery(int nMasteryCode, unsigned int nMasteryIndex, int nLv)
{

    __int64 *v4; // rdi
    __int64 i; // rcx
    float v7; // xmm0_4
    float v8; // xmm1_4
    float v9; // xmm0_4
    float v10; // xmm1_4
    float v11; // xmm0_4
    float v12; // xmm1_4
    float v13; // xmm0_4
    float v14; // xmm0_4
    float v15; // xmm0_4
    float v16; // xmm0_4
    float v17; // xmm0_4
    float v18; // xmm1_4
    __int64 v19; // [rsp+0h] [rbp-58h] BYREF
    unsigned int dwNewData; // [rsp+20h] [rbp-38h]
    unsigned int dwStatIndex; // [rsp+24h] [rbp-34h]
    unsigned int dwNewStat; // [rsp+28h] [rbp-30h]
    int j; // [rsp+2Ch] [rbp-2Ch]
    int v24; // [rsp+30h] [rbp-28h]
    unsigned int StatIndex; // [rsp+34h] [rbp-24h]
    float v26; // [rsp+38h] [rbp-20h]
    float v27; // [rsp+3Ch] [rbp-1Ch]
    float v28; // [rsp+40h] [rbp-18h]
    float v29; // [rsp+44h] [rbp-14h]
    unsigned __int64 v30; // [rsp+48h] [rbp-10h]
  if ( nMasteryCode == 3 )
    {
      if ( (int)nMasteryIndex >= 8 )
        return 0;
    }
    else if ( !_STAT_DB_BASE::IsRangePerMastery(nMasteryCode, nMasteryIndex) )
    {
      return 0;
    }
    if ( nLv > 99 )
      return 0;
    if ( nLv < 1 )
      return 0;
    dwNewData = 0;
    if ( nMasteryCode )
    {
      switch ( nMasteryCode )
      {
        case 1:
          v27 = pow(1000.0, 2);
          v9 = pow((float)nLv, 2);
          v10 = sqrt(v27 + (float)((float)(4.0 * v9) * 1000.0)) + -1000.0;
          dwNewData = (int)pow(v10 / 2.0, 2);
          break;
        case 2:
          v28 = pow(1000.0, 2);
          v11 = pow((float)nLv, 2);
          v12 = sqrt(v28 + (float)((float)(4.0 * v11) * 1000.0)) + -1000.0;
          dwNewData = (int)pow(v12 / 2.0, 2);
          break;
        case 4:
          v13 = pow((float)nLv, 2);
          v14 = pow(v13, 2);
          dwNewData = (int)(float)((float)(CalcRoundUp(v14 / 14.0) - 1) + 0.0099999998);
          break;
        case 3:
          v15 = pow((float)nLv, 2);
          v16 = pow(v15, 2);
          dwNewData = (int)(float)((float)(CalcRoundUp(v16 / 10.0) - 1) + 0.0099999998);
          break;
        case 5:
          if ( nMasteryIndex > 1 )
          {
            if ( nMasteryIndex == 2 )
              dwNewData = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 10.0) + 0.89999998);
          }
          else
          {
            dwNewData = (int)(float)((float)((float)((float)(pow((float)nLv, 2) - 1.0) / 3.0) * 1.1) + 0.89999998);
          }
          break;
        case 6:
          if ( !(unsigned int)this->m_Param.GetRaceCode()
            || (unsigned int)this->m_Param.GetRaceCode() == 1 )
          {
            dwNewData = (int)(float)(pow((float)nLv - 1.0, 2) * 15000.0);
          }
          else if ( (unsigned int)this->m_Param.GetRaceCode() == 2 )
          {
            v29 = pow(1000.0, 2);
            v17 = pow((float)nLv, 2);
            v18 = sqrt(v29 + (float)((float)(4.0 * v17) * 1000.0)) + -1000.0;
            dwNewData = (int)pow(v18 / 2.0, 2);
          }
          break;
      }
    }
    else
    {
      v26 = pow(1000.0, 2);
      v7 = pow((float)nLv, 2);
      v8 = sqrt(v26 + (float)((float)(4.0 * v7) * 1000.0)) + -1000.0;
      dwNewData = (int)pow(v8 / 2.0, 2);
    }
    if ( !dwNewData )
      return 0;
    if ( nMasteryCode == 3 )
    {
      v30 = 196LL * (int)nMasteryIndex;
      dwNewStat = dwNewData / CPlayer::s_SkillIndexPerMastery[v30 / 0xC4].m_nSkillIndexNum + 1;
      for ( j = 0; j < CPlayer::s_SkillIndexPerMastery[nMasteryIndex].m_nSkillIndexNum; ++j )
      {
        v24 = CPlayer::s_SkillIndexPerMastery[nMasteryIndex].m_nSkillIndex[j];
        if ( dwNewStat )
        {
          StatIndex = _STAT_DB_BASE::GetStatIndex(3u, v24);
          this->Emb_UpdateStat( StatIndex, dwNewStat, 0);
          this->SendMsg_StatInform( StatIndex, dwNewStat, 0);
        }
      }
    }
    else
    {
      dwStatIndex = _STAT_DB_BASE::GetStatIndex(nMasteryCode, nMasteryIndex);
      this->Emb_UpdateStat( dwStatIndex, dwNewData, 0);
      this->SendMsg_StatInform( dwStatIndex, dwNewData, 0);
    }
    return 1;
}

char CPlayer::dev_up_pvp(long double dPoint)
{
  this->AlterPvPPoint(dPoint, cheat, 0xFFFFFFFF);
  return 1;
}

char CPlayer::dev_up_skill(char *pszSkillCode, unsigned int nCum)
{

    __int64 *v3; // rdi
    __int64 i; // rcx
    __int64 v6; // [rsp+0h] [rbp-38h] BYREF
    _base_fld *Record; // [rsp+20h] [rbp-18h]
  Record = g_Main.m_tblEffectData[0].GetRecord( pszSkillCode);
    if ( !Record )
      return 0;
    this->Emb_UpdateStat( Record->m_dwIndex + 4, nCum, 0);
    return 1;
}

char CPlayer::dev_view_boss()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    __int64 v4; // [rsp+0h] [rbp-98h] BYREF
    unsigned __int64 m_dwLastState; // [rsp+30h] [rbp-68h]
    char szMsg[4]; // [rsp+48h] [rbp-50h] BYREF
    unsigned __int64 v7; // [rsp+4Ch] [rbp-4Ch]
    unsigned __int8 pbyType[36]; // [rsp+74h] [rbp-24h] BYREF
  m_dwLastState = this->m_dwLastState;
    m_dwLastState |= 0x10000uLL;
    *(_DWORD *)szMsg = this->m_dwObjSerial;
    v7 = m_dwLastState;
    pbyType[0] = 4;
    pbyType[1] = 25;
    g_Network.m_pProcess[0]->LoadSendMsg( this->m_ObjID.m_wIndex, pbyType, szMsg, 0xCu);
    return 1;
}

char CPlayer::dev_view_method(char *pwszDstName)
{
  CPlayer *target = nullptr;
  if (pwszDstName)
  {
    const unsigned __int8 nameLen = static_cast<unsigned __int8>(strlen_0(pwszDstName));
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
    return 0;
  }

  CLogFile log;
  const int currentSec = GetCurrentSec();
  const unsigned int korTime = GetKorLocalTime();
  const char *charNameA = target->m_Param.GetCharNameA();

  char buffer[152]{};
  sprintf(buffer, "..\\ZoneServerLog\\CharLog\\%s_Method_%d_%d.log", charNameA, korTime, currentSec);
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
  for (int index = 0; index < 62; ++index)
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
  for (int index = 0; index < 83; ++index)
  {
    const float have = target->m_EP.GetEff_Have(index);
    log.Write("%d\t : %f", index, have);
  }

  return 1;
}

char CPlayer::mgr_all_item_muzi(unsigned int nLv)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    unsigned __int8 BagNum; // al
    __int64 v6; // [rsp+0h] [rbp-58h] BYREF
    int j; // [rsp+30h] [rbp-28h]
    _STORAGE_LIST::_db_con *pItem; // [rsp+38h] [rbp-20h]
    unsigned __int8 ItemUpgLimSocket; // [rsp+40h] [rbp-18h]
    unsigned int dwCurBit; // [rsp+44h] [rbp-14h]
    signed int k; // [rsp+48h] [rbp-10h]
  if ( nLv >= 8 )
      return 0;
    for ( j = 0; ; ++j )
    {
      BagNum = this->m_Param.GetBagNum();
      if ( j >= 20 * BagNum )
        break;
      if ( this->m_Param.m_dbInven.m_pStorageList[j].m_bLoad )
      {
        pItem = &this->m_Param.m_dbInven.m_pStorageList[j];
        if ( pItem->m_bLoad )
        {
          if ( pItem->m_byTableCode == 6 )
          {
            ItemUpgLimSocket = GetItemUpgLimSocket(pItem->m_dwLv);
            if ( ItemUpgLimSocket >= (int)nLv )
            {
              dwCurBit = GetBitAfterSetLimSocket(ItemUpgLimSocket);
              for ( k = 0; k < (int)nLv; ++k )
                dwCurBit = GetBitAfterUpgrade(dwCurBit, 0, k);
              this->Emb_ItemUpgrade( 0, 0, j, dwCurBit);
              this->SendMsg_DeleteStorageInform( 0, pItem->m_wSerial);
              this->SendMsg_RewardAddItem( pItem, 0);
            }
          }
        }
      }
    }
    return 1;
}

char CPlayer::mgr_change_degree(unsigned __int8 nDegree)
{

    unsigned __int8 *v2; // rdi
    __int64 i; // rcx
    unsigned __int8 m_byUserDgr; // [rsp+0h] [rbp-18h] BYREF
  m_byUserDgr = this->m_byUserDgr;
    this->m_byUserDgr = nDegree;
    this->m_pUserDB->m_byUserDgr = nDegree;
    return 1;
}

char CPlayer::mgr_defense_item_grace(unsigned __int8 byItemCode, unsigned int nLv)
{

    __int64 *v3; // rdi
    __int64 i; // rcx
    unsigned __int8 BagNum; // al
    __int64 v7; // [rsp+0h] [rbp-58h] BYREF
    int j; // [rsp+30h] [rbp-28h]
    _STORAGE_LIST::_db_con *pItem; // [rsp+38h] [rbp-20h]
    unsigned __int8 ItemUpgLimSocket; // [rsp+40h] [rbp-18h]
    unsigned int dwCurBit; // [rsp+44h] [rbp-14h]
    signed int k; // [rsp+48h] [rbp-10h]
  if ( nLv >= 8 )
      return 0;
    pItem = 0LL;
    for ( j = 0; ; ++j )
    {
      BagNum = this->m_Param.GetBagNum();
      if ( j >= 20 * BagNum )
        break;
      pItem = &this->m_Param.m_dbInven.m_pStorageList[j];
      if ( pItem && pItem->m_bLoad )
      {
        if ( byItemCode == 37 )
        {
          if ( pItem->m_byTableCode
            && pItem->m_byTableCode != 1
            && pItem->m_byTableCode != 2
            && pItem->m_byTableCode != 3
            && pItem->m_byTableCode != 4
            && pItem->m_byTableCode != 5
            && pItem->m_byTableCode != 7 )
          {
            continue;
          }
        }
        else if ( pItem->m_byTableCode != byItemCode )
        {
          continue;
        }
        ItemUpgLimSocket = GetItemUpgLimSocket(pItem->m_dwLv);
        if ( ItemUpgLimSocket >= (int)nLv )
        {
          dwCurBit = GetBitAfterSetLimSocket(ItemUpgLimSocket);
          for ( k = 0; k < (int)nLv; ++k )
            dwCurBit = GetBitAfterUpgrade(dwCurBit, 5u, k);
          this->Emb_ItemUpgrade( 0, 0, j, dwCurBit);
          this->SendMsg_DeleteStorageInform( 0, pItem->m_wSerial);
          this->SendMsg_RewardAddItem( pItem, 0);
        }
      }
    }
    return 1;
}

char CPlayer::mgr_destroy_system_tower()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    int CurSecNum; // eax
    _sec_info *SecInfo; // rax
    __int64 v6; // [rsp+0h] [rbp-208h] BYREF
    _pnt_rect pRect; // [rsp+28h] [rbp-1E0h] BYREF
    int j; // [rsp+44h] [rbp-1C4h]
    int k; // [rsp+48h] [rbp-1C0h]
    unsigned int dwSecIndex; // [rsp+4Ch] [rbp-1BCh]
    CObjectList *SectorListObj; // [rsp+50h] [rbp-1B8h]
    _object_list_point *m_pNext; // [rsp+58h] [rbp-1B0h]
    CGuardTower *m_pItem; // [rsp+60h] [rbp-1A8h]
    _object_id *p_m_ObjID; // [rsp+68h] [rbp-1A0h]
    CGuardTower *v15; // [rsp+70h] [rbp-198h]
    char wszTran[152]; // [rsp+90h] [rbp-178h] BYREF
    LPCSTR lpAppName[7]; // [rsp+128h] [rbp-E0h]
    char Buffer[144]; // [rsp+160h] [rbp-A8h] BYREF
  CurSecNum = this->GetCurSecNum();
    this->m_pCurMap->GetRectInRadius( &pRect, 1, CurSecNum);
    for ( j = pRect.nStarty; j <= pRect.nEndy; ++j )
    {
      for ( k = pRect.nStartx; k <= pRect.nEndx; ++k )
      {
        SecInfo = this->m_pCurMap->GetSecInfo();
        dwSecIndex = SecInfo->m_nSecNumW * j + k;
        SectorListObj = this->m_pCurMap->GetSectorListObj(this->m_wMapLayerIndex, dwSecIndex);
        if ( SectorListObj )
        {
          m_pNext = SectorListObj->m_Head.m_pNext;
          while ( m_pNext != &SectorListObj->m_Tail )
          {
            m_pItem = (CGuardTower *)m_pNext->m_pItem;
            m_pNext = m_pNext->m_pNext;
            p_m_ObjID = &m_pItem->m_ObjID;
            if ( !m_pItem->m_ObjID.m_byKind && p_m_ObjID->m_byID == 4 )
            {
              v15 = m_pItem;
              if ( m_pItem->m_bSystemStruct
                && abs(m_pItem->m_fCurPos[1] - this->m_fCurPos[1]) <= 100.0
                && GetSqrt(m_pItem->m_fCurPos, this->m_fCurPos) < 20.0 )
              {
                M2W(v15->m_pRecordSet->m_strCode, wszTran, 0x80u);
                if ( v15->m_nIniIndex != -1 )
                {
                  lpAppName[0] = "BELLATO";
                  lpAppName[1] = "CORA";
                  lpAppName[2] = "ACCRETIA";
                  sprintf(Buffer, "Map%d", v15->m_nIniIndex);
                  WritePrivateProfileStringA(
                    lpAppName[v15->m_byRaceCode],
                    Buffer,
                    "NULL",
                    ".\\Script\\SystemGuardTower.ini");
                }
                v15->Destroy(0, true);
                return 1;
              }
            }
          }
        }
      }
    }
    return 0;
}

char CPlayer::mgr_dungeon_pass()
{
  if (this->m_pCurMap->m_pMapSet->m_nMapType != 1)
  {
    return 0;
  }
  if (this->m_bMapLoading)
  {
    return 0;
  }
  if (this->m_pDHChannel)
  {
    for (int j = 0; j < this->m_pDHChannel->m_MissionMgr.pCurMssionPtr->nEmbJobSetupNum; ++j)
    {
      this->m_pDHChannel->m_MissionMgr.Count[j].bPass = 1;
    }
    this->m_pDHChannel->m_MissionMgr.OpenPortal(-1);
  }
  return 1;
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

char CPlayer::mgr_free_ride_ship()
{
  CMapData *pIntoMap = g_MapOper.GetMap("Transport01");
  if (!pIntoMap)
  {
    return 0;
  }
  if (!pIntoMap->IsMapIn(fPos))
  {
    return 0;
  }

  const unsigned __int16 raceCode = this->m_Param.GetRaceCode();
  this->OutOfMap(pIntoMap, raceCode, 4u, fPos);
  const unsigned __int8 mapCode = this->m_Param.GetMapCode();
  this->SendMsg_GotoRecallResult(0, mapCode, fPos, 4u);
  return 1;
}

char CPlayer::mgr_goto_mine()
{
  CMapData *pIntoMap = g_MapOper.GetMap("resources");
  if (!pIntoMap)
  {
    return 0;
  }
  this->OutOfMap(pIntoMap, 0, 4u, fTarPos);
  this->SendMsg_GotoRecallResult(0, pIntoMap->m_pMapSet->m_dwIndex, fTarPos, 4u);
  return 1;
}

char CPlayer::mgr_goto_shipport(int nRaceCode, int nPort)
{

    __int64 *v3; // rdi
    __int64 i; // rcx
    unsigned __int8 MapCode; // al
    __int64 v7; // [rsp+0h] [rbp-88h] BYREF
    CTransportShip *v8; // [rsp+30h] [rbp-58h]
    _portal_dummy *Portal; // [rsp+38h] [rbp-50h]
    CMapData *pIntoMap; // [rsp+40h] [rbp-48h]
    _portal_dummy *v11; // [rsp+48h] [rbp-40h]
    float pNewPos[12]; // [rsp+58h] [rbp-30h] BYREF
  if ( nRaceCode >= 3 )
      return 0;
    if ( this->GetCurSecNum() == -1 || this->m_bMapLoading )
      return 0;
    v8 = &g_TransportShip[nRaceCode];
    Portal = v8->m_pLinkShipMap->GetPortal( 3 * nPort + nRaceCode);
    if ( !Portal )
      return 0;
    if ( !Portal->m_pPortalRec )
      return 0;
    pIntoMap = g_MapOper.GetMap( Portal->m_pPortalRec->m_strLinkMapCode);
    if ( !pIntoMap )
      return 0;
    v11 = pIntoMap->GetPortal( Portal->m_pPortalRec->m_strLinkPortalCode);
    if ( !v11 )
      return 0;
    if ( !pIntoMap->GetRandPosInDummy( v11->m_pDumPos, pNewPos, 1) )
      memcpy_0(pNewPos, v11->m_pDumPos->m_fCenterPos, 0xCuLL);
    this->OutOfMap( pIntoMap, 0, 4u, pNewPos);
    MapCode = this->m_Param.GetMapCode();
    this->SendMsg_GotoRecallResult( 0, MapCode, pNewPos, 4u);
    return 1;
}

char CPlayer::mgr_goto_stone(unsigned __int8 byRaceCode)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-48h] BYREF
    __holy_stone_data *v6; // [rsp+30h] [rbp-18h]
  if ( byRaceCode >= 3u )
      return 0;
    v6 = &g_HolySys.m_HolyStoneData[byRaceCode];
    this->OutOfMap( v6->pCreateMap, 0, 4u, v6->CreateDummy.m_fCenterPos);
    this->SendMsg_GotoRecallResult( 0, v6->pCreateMap->m_pMapSet->m_dwIndex, v6->CreateDummy.m_fCenterPos, 4u);
    return 1;
}

char CPlayer::mgr_goto_store(unsigned __int8 nRaceCode, char *pszNPCName)
{
  if (this->m_pmTrd.bDTradeMode)
  {
    return 0;
  }
  if (this->GetCurSecNum() == -1 || this->m_bMapLoading)
  {
    return 0;
  }

  CMapData *pIntoMap = g_MapOper.GetStartMap(nRaceCode);
  if (!pIntoMap)
  {
    return 0;
  }

  float *startPos = nullptr;
  const int mapSerial = pIntoMap->GetMapCode();
  CItemStoreManager *manager = CItemStoreManager::Instance();
  CMapItemStoreList *storeList = manager->GetMapItemStoreListBySerial(mapSerial);
  if (!storeList)
  {
    return 0;
  }

  const size_t nameLen = strlen_0(pszNPCName);
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
    return 0;
  }

  this->OutOfMap(pIntoMap, 0, 4u, startPos);
  const unsigned __int8 mapCode = this->m_Param.GetMapCode();
  this->SendMsg_GotoRecallResult(0, mapCode, startPos, 4u);
  return 1;
}

char CPlayer::mgr_gotoCoordinates(char *pszMapCode, float fX, float fY, float fZ)
{

    __int64 *v5; // rdi
    __int64 i; // rcx
    __int64 v8; // [rsp+0h] [rbp-78h] BYREF
    CMapData *pIntoMap; // [rsp+30h] [rbp-48h]
    float fPos[12]; // [rsp+48h] [rbp-30h] BYREF
  pIntoMap = g_MapOper.GetMap( pszMapCode);
    if ( !pIntoMap )
      return 0;
    if ( pIntoMap->m_pMapSet->m_nMapType )
      return 0;
    fPos[0] = fX;
    fPos[1] = fY;
    fPos[2] = fZ;
    if ( !pIntoMap->IsMapIn( fPos) )
      return 0;
    this->OutOfMap( pIntoMap, 0, 4u, fPos);
    this->SendMsg_GotoRecallResult( 0, pIntoMap->m_pMapSet->m_dwIndex, fPos, 4u);
    return 1;
}

char CPlayer::mgr_gotoDstCoordinates(char *pwszDstName, char *pszMapCode, float fX, float fY, float fZ)
{

    __int64 *v6; // rdi
    __int64 i; // rcx
    __int64 v9; // [rsp+0h] [rbp-78h] BYREF
    CPlayer *PtrPlayerFromName; // [rsp+30h] [rbp-48h]
    CMapData *pIntoMap; // [rsp+38h] [rbp-40h]
    float fPos[12]; // [rsp+48h] [rbp-30h] BYREF
  PtrPlayerFromName = 0LL;
    if ( pwszDstName )
    {
    PtrPlayerFromName = GetPtrPlayerFromName(g_Player, MAX_PLAYER, pwszDstName);
    }
    else
    {
      if ( !this->m_TargetObject.pObject )
        return 0;
      if ( !this->m_TargetObject.pObject->m_bLive )
        return 0;
      if ( this->m_TargetObject.pObject->m_ObjID.m_byKind || this->m_TargetObject.pObject->m_ObjID.m_byID )
        return 0;
      if ( this->m_TargetObject.byKind != this->m_TargetObject.pObject->m_ObjID.m_byKind
        || this->m_TargetObject.byID != this->m_TargetObject.pObject->m_ObjID.m_byID )
      {
        return 0;
      }
      PtrPlayerFromName = (CPlayer *)this->m_TargetObject.pObject;
    }
    if ( !PtrPlayerFromName )
      return 0;
    pIntoMap = g_MapOper.GetMap( pszMapCode);
    if ( !pIntoMap )
      return 0;
    if ( pIntoMap->m_pMapSet->m_nMapType )
      return 0;
    fPos[0] = fX;
    fPos[1] = fY;
    fPos[2] = fZ;
    if ( !pIntoMap->IsMapIn( fPos) )
      return 0;
    PtrPlayerFromName->OutOfMap( pIntoMap, 0, 4u, fPos);
    PtrPlayerFromName->SendMsg_GotoRecallResult( 0, pIntoMap->m_pMapSet->m_dwIndex, fPos, 4u);
    return 1;
}

bool CPlayer::mgr_holykeeper_start(int nRace)
{

    return 0;
}

bool CPlayer::mgr_holystone_start(int nNumOfTime)
{

    return 0;
}

char CPlayer::mgr_item_telekinesis()
{
  if (!this->m_pUserDB)
  {
    return 0;
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
        if (this->m_EP.GetEff_State(20) || this->m_EP.GetEff_State(28))
        {
          continue;
        }

        _STORAGE_LIST::_db_con item;
        memcpy_0(&item, &itemBox->m_Item, sizeof(item));
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
              item.m_dwDur,
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
                const DWORD nowSec = timeGetTime() / 0x3E8;
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

  return result;
}

char CPlayer::mgr_kick(char *pwszCharName)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-48h] BYREF
    CUserDB *v6; // [rsp+30h] [rbp-18h]
  if ( pwszCharName )
    {
      v6 = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
      if ( v6 )
      {
        v6->ForceCloseCommand( 0, 0, 1, "Kick By GM");
        return 1;
      }
      else
      {
        return 0;
      }
    }
    else if ( this->m_TargetObject.pObject )
    {
      if ( this->m_TargetObject.pObject->m_bLive )
      {
        if ( this->m_TargetObject.pObject->m_ObjID.m_byKind || this->m_TargetObject.pObject->m_ObjID.m_byID )
        {
          return 0;
        }
        else if ( this->m_TargetObject.byKind == this->m_TargetObject.pObject->m_ObjID.m_byKind
               && this->m_TargetObject.byID == this->m_TargetObject.pObject->m_ObjID.m_byID )
        {
          CUserDB *targetUser = *reinterpret_cast<CUserDB **>(&this->m_TargetObject.pObject[10].m_ObjID.m_byKind);
          if (targetUser)
          {
            targetUser->ForceCloseCommand(0, 0, true, "Kick By GM");
          }
          return 1;
        }
        else
        {
          return 0;
        }
      }
      else
      {
        return 0;
      }
    }
    else
    {
      return 0;
    }
}

char CPlayer::mgr_make_system_tower(char *pszTowerCode)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-3D8h] BYREF
    char wszTran[136]; // [rsp+40h] [rbp-398h] BYREF
    _base_fld *Record; // [rsp+C8h] [rbp-310h]
    int v8; // [rsp+D0h] [rbp-308h]
    int j; // [rsp+D4h] [rbp-304h]
    LPCSTR lpAppName[9]; // [rsp+E8h] [rbp-2F0h]
    int nIniIndex; // [rsp+134h] [rbp-2A4h]
    char ReturnedString[160]; // [rsp+150h] [rbp-288h] BYREF
    char Buffer[160]; // [rsp+1F0h] [rbp-1E8h] BYREF
    char KeyName[160]; // [rsp+290h] [rbp-148h] BYREF
    char String[144]; // [rsp+330h] [rbp-A8h] BYREF
  M2W(pszTowerCode, wszTran, 0x80u);
    Record = g_Main.m_tblItemData[25].GetRecord( pszTowerCode);
    if ( !Record )
      return 0;
    if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return 0;
    v8 = -1;
    for ( j = 0; j < 3; ++j )
    {
      if ( Record[3].m_strCode[2 * j + 52] == 49 )
      {
        v8 = j;
        break;
      }
    }
    if ( v8 == -1 )
      return 0;
    lpAppName[0] = "BELLATO";
    lpAppName[1] = "CORA";
    lpAppName[2] = "ACCRETIA";
    lpAppName[6] = "BELLATO";
    lpAppName[7] = "CORA";
    lpAppName[8] = "ACCRETIA";
    nIniIndex = -1;
    for ( j = 0; j < 50; ++j )
    {
      sprintf(Buffer, "Map%d", j);
      GetPrivateProfileStringA(lpAppName[v8], Buffer, "NULL", ReturnedString, 0x80u, ".\\Script\\SystemGuardTower.ini");
      if ( !strcmp_0(ReturnedString, "NULL") )
      {
        nIniIndex = j;
        break;
      }
    }
    if ( nIniIndex == -1 )
      return 0;
    if ( !CreateSystemTower(this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, Record->m_dwIndex, v8, nIniIndex) )
      return 0;
    sprintf(KeyName, "Map%d", nIniIndex);
    WritePrivateProfileStringA(
      lpAppName[v8],
      KeyName,
      this->m_pCurMap->m_pMapSet->m_strCode,
      ".\\Script\\SystemGuardTower.ini");
    sprintf(KeyName, "Pos%d_x", nIniIndex);
    _itoa((int)this->m_fCurPos[0], String, 10);
    WritePrivateProfileStringA(lpAppName[v8], KeyName, String, ".\\Script\\SystemGuardTower.ini");
    sprintf(KeyName, "Pos%d_y", nIniIndex);
    _itoa((int)this->m_fCurPos[1], String, 10);
    WritePrivateProfileStringA(lpAppName[v8], KeyName, String, ".\\Script\\SystemGuardTower.ini");
    sprintf(KeyName, "Pos%d_z", nIniIndex);
    _itoa((int)this->m_fCurPos[2], String, 10);
    WritePrivateProfileStringA(lpAppName[v8], KeyName, String, ".\\Script\\SystemGuardTower.ini");
    sprintf(KeyName, "Code%d", nIniIndex);
    WritePrivateProfileStringA(lpAppName[v8], KeyName, Record->m_strCode, ".\\Script\\SystemGuardTower.ini");
    return 1;
}

char CPlayer::mgr_matchless(bool bMatchless)
{

    if ( this->m_bCheat_Matchless == bMatchless )
      return 0;
    this->m_bCheat_Matchless = bMatchless;
    return 1;
}

#if 0 // duplicate implementation exists in CPlayer.cpp
char CPlayer::mgr_MaxAttackPoint(int nMax)
{

    if ( this->m_nMaxAttackPnt == nMax )
      return 0;
    this->m_nMaxAttackPnt = nMax;
    return 1;
}
#endif

char CPlayer::mgr_pass_sch_one_step()
{
  g_WorldSch.PassOneStep();
  return 1;
}

char CPlayer::mgr_recall_guild_player(char *wszDestCharName)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-58h] BYREF
    CPlayer *PtrPlayerFromName; // [rsp+30h] [rbp-28h]
    CPlayer *pPlayer; // [rsp+38h] [rbp-20h]
    int j; // [rsp+40h] [rbp-18h]
    _guild_member_info *v9; // [rsp+48h] [rbp-10h]
  if ( !wszDestCharName )
      return 0;
    if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return 0;
    PtrPlayerFromName = GetPtrPlayerFromName(g_Player, MAX_PLAYER, wszDestCharName);
    if ( !PtrPlayerFromName )
      return 0;
    if ( PtrPlayerFromName->m_Param.m_pGuild )
    {
      pPlayer = 0LL;
      for ( j = 0; j < 50; ++j )
      {
        v9 = &PtrPlayerFromName->m_Param.m_pGuild->m_MemberData[j];
        if ( v9->IsFill() )
        {
          if ( v9->pPlayer )
          {
            pPlayer = v9->pPlayer;
            if ( pPlayer->m_bLive && this->m_dwObjSerial != pPlayer->m_dwObjSerial )
              pPlayer->RecallRandomPositionInRange( this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, 200);
          }
        }
      }
      return 1;
    }
    else if ( this->m_dwObjSerial == PtrPlayerFromName->m_dwObjSerial )
    {
      return 0;
    }
    else
    {
      PtrPlayerFromName->RecallRandomPositionInRange(
        
        this->m_pCurMap,
        this->m_wMapLayerIndex,
        this->m_fCurPos,
        200);
      return 1;
    }
}

char CPlayer::mgr_recall_mon(char *pszMonCode, int nCreateNum)
{

    __int64 *v3; // rdi
    __int64 i; // rcx
    __int64 v6; // [rsp+0h] [rbp-68h] BYREF
    int v7; // [rsp+50h] [rbp-18h]
    int j; // [rsp+54h] [rbp-14h]
    int v11; // [rsp+80h] [rbp+18h]

    v11 = nCreateNum;
  if ( this->m_pCurMap->m_pMapSet->m_nMapType == 1 )
      return 0;
    if ( nCreateNum > 100 )
      v11 = 100;
    v7 = 0;
    for ( j = 0;
          j < v11
       && CreateRepMonster(this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, pszMonCode, 0LL, 0, 1, 0, 0, 1);
          ++j )
    {
      ++v7;
    }
    return 1;
}

char CPlayer::mgr_recall_party_player(char *wszDestCharName)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-58h] BYREF
    CPlayer *PtrPlayerFromName; // [rsp+30h] [rbp-28h]
    CPartyPlayer **PtrPartyMember; // [rsp+38h] [rbp-20h]
    CPlayer *v8; // [rsp+40h] [rbp-18h]
    int j; // [rsp+48h] [rbp-10h]
  if ( !wszDestCharName )
      return 0;
    if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return 0;
    PtrPlayerFromName = GetPtrPlayerFromName(g_Player, MAX_PLAYER, wszDestCharName);
    if ( !PtrPlayerFromName )
      return 0;
    if ( PtrPlayerFromName->m_pPartyMgr->IsPartyMode() )
    {
      PtrPartyMember = PtrPlayerFromName->m_pPartyMgr->GetPtrPartyMember();
      if ( PtrPartyMember )
      {
        v8 = 0LL;
        for ( j = 0; j < 8 && PtrPartyMember[j]; ++j )
        {
          v8 = &g_Player[PtrPartyMember[j]->m_wZoneIndex];
          if ( v8->m_bLive && this->m_dwObjSerial != v8->m_dwObjSerial )
            v8->RecallRandomPositionInRange( this->m_pCurMap, this->m_wMapLayerIndex, this->m_fCurPos, 120);
        }
        return 1;
      }
      else
      {
        return 0;
      }
    }
    else if ( this->m_dwObjSerial == PtrPlayerFromName->m_dwObjSerial )
    {
      return 0;
    }
    else
    {
      PtrPlayerFromName->RecallRandomPositionInRange(
        
        this->m_pCurMap,
        this->m_wMapLayerIndex,
        this->m_fCurPos,
        120);
      return 1;
    }
}

char CPlayer::mgr_recall_player(char *pwszCharName)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    const char *CharNameW; // rax
    __int64 v6; // [rsp+0h] [rbp-38h] BYREF
    CUserDB *v7; // [rsp+20h] [rbp-18h]
    CPlayer *v8; // [rsp+28h] [rbp-10h]
  if ( this->m_pCurMap->m_pMapSet->m_nMapType )
      return 0;
    v7 = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
    if ( !v7 )
      return 0;
    v8 = &g_Player[v7->m_idWorld.wIndex];
    if ( !v8->m_bLive )
      return 0;
    CharNameW = this->m_Param.GetCharNameW();
    v8->pc_GotoAvatorRequest( CharNameW);
    return 1;
}

bool CPlayer::mgr_resurrect_player(char *pwszCharName)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    __int64 v5; // [rsp+0h] [rbp-38h] BYREF
    CUserDB *v6; // [rsp+20h] [rbp-18h]
    CPlayer *v7; // [rsp+28h] [rbp-10h]
  v6 = SearchAvatorWithName(g_UserDB, MAX_PLAYER, pwszCharName);
    if ( !v6 )
      return 0;
    v7 = &g_Player[v6->m_idWorld.wIndex];
    return v7->m_bLive && v7->pc_Resurrect( 0);
}

#if 0 // duplicate implementation exists in CPlayer.cpp
char CPlayer::mgr_set_animus_attack_point(int nPoint)
{

    if ( !this->m_pUserDB )
      return 0;
    if ( this->m_nAnimusAttackPnt == nPoint )
      return 0;
    this->m_nAnimusAttackPnt = nPoint;
    if ( this->m_pRecalledAnimusChar )
      this->m_pRecalledAnimusChar->m_nMaxAttackPnt = nPoint;
    return 1;
}
#endif

char CPlayer::mgr_TrunkInit()
{

    __int64 *v1; // rdi
    __int64 i; // rcx
    unsigned __int8 TrunkSlotNum; // al
    unsigned __int8 v5; // al
    unsigned __int8 ExtTrunkSlotNum; // al
    unsigned __int8 v7; // al
    __int64 v8; // [rsp+0h] [rbp-48h] BYREF
    int j; // [rsp+30h] [rbp-18h]
    int k; // [rsp+34h] [rbp-14h]
  for ( j = 0; ; ++j )
    {
      TrunkSlotNum = this->m_Param.GetTrunkSlotNum();
      if ( j >= TrunkSlotNum )
        break;
      if ( this->m_Param.m_pStoragePtr[5]->m_pStorageList[j].m_bLoad
        && !this->Emb_DelStorage( 5u, j, 0, 0, "CPlayer::mgr_TrunkInit()") )
      {
        return 0;
      }
    }
    this->m_Param.m_byTrunkSlotNum = 0;
    this->m_Param.m_dbTrunk.SetUseListNum(0);
    v5 = this->m_Param.GetTrunkSlotNum();
    this->m_pUserDB->Update_TrunkSlotNum( v5);
    for ( k = 0; ; ++k )
    {
      ExtTrunkSlotNum = this->m_Param.GetExtTrunkSlotNum();
      if ( k >= ExtTrunkSlotNum )
        break;
      if ( this->m_Param.m_pStoragePtr[7]->m_pStorageList[k].m_bLoad )
        this->Emb_DelStorage( 7u, k, 0, 0, "CPlayer::mgr_TrunkInit()");
    }
    this->m_Param.m_byExtTrunkSlotNum = 0;
    this->m_Param.m_dbExtTrunk.SetUseListNum(0);
    v7 = this->m_Param.GetExtTrunkSlotNum();
    this->m_pUserDB->Update_ExtTrunkSlotNum( v7);
    return 1;
}

char CPlayer::mgr_user_ban(char *uszCharName, int iPeriod, char *uszReason, char byBlockType)
{

    __int64 *v5; // rdi
    __int64 i; // rcx
    const char *CharNameW; // rax
    __int64 v9; // [rsp+0h] [rbp-1A8h] BYREF
    CUserDB *v10; // [rsp+40h] [rbp-168h]
    char szMsg; // [rsp+60h] [rbp-148h] BYREF
    int v12; // [rsp+61h] [rbp-147h]
    _BYTE v13[8]; // [rsp+65h] [rbp-143h] BYREF
    _BYTE v14[6]; // [rsp+6Dh] [rbp-13Bh] BYREF
    char Destination[32]; // [rsp+73h] [rbp-135h] BYREF
    char v16[32]; // [rsp+93h] [rbp-115h] BYREF
    unsigned int m_dwAccountSerial; // [rsp+B3h] [rbp-F5h]
    unsigned __int8 pbyType[44]; // [rsp+D4h] [rbp-D4h] BYREF
    char _Dest[128]; // [rsp+100h] [rbp-A8h] BYREF
  v10 = SearchAvatorWithName(g_UserDB, MAX_PLAYER, uszCharName);
    if ( v10 )
    {
      v10->SetChatLock( 1);
      szMsg = byBlockType;
      v12 = iPeriod;
      m_dwAccountSerial = v10->m_dwAccountSerial;
      memcpy_0(v13, &v10->m_gidGlobal, sizeof(v13));
      memcpy_0(v14, &this->m_pUserDB->m_idWorld, sizeof(v14));
      strncpy(Destination, uszReason, 0x1FuLL);
      CharNameW = this->m_Param.GetCharNameW();
      strncpy(v16, CharNameW, 0x1FuLL);
      memcpy_0(pbyType, "2n", 2);
      g_Network.m_pProcess[1]->LoadSendMsg( 0, pbyType, &szMsg, 0x57u);
      return 1;
    }
    else
    {
      sprintf_s(_Dest, sizeof(_Dest), "%s is not connected", uszCharName);
      this->SendData_ChatTrans( 0, 0xFFFFFFFF, 0xFFu, 0, _Dest, 0xFFu, 0LL);
      return 0;
    }
}

char CPlayer::mgr_whisper(char *pwszMsg)
{

    __int64 *v2; // rdi
    __int64 i; // rcx
    unsigned __int8 RaceCode; // al
    __int64 v6; // [rsp+0h] [rbp-58h] BYREF
    int j; // [rsp+40h] [rbp-18h]
    char *pwszSender; // [rsp+48h] [rbp-10h]
  for ( j = 0; j < MAX_PLAYER; ++j )
    {
      if ( g_UserDB[j].m_bActive && g_UserDB[j].m_byUserDgr >= 2u )
      {
        pwszSender = this->m_Param.GetCharNameW();
        RaceCode = this->m_Param.GetRaceCode();
        g_Player[j].SendData_ChatTrans(
          
          5u,
          this->m_dwObjSerial,
          RaceCode,
          0,
          pwszMsg,
          this->m_Param.m_byPvPGrade,
          pwszSender);
      }
    }
    return 1;
}

char loot_item(CPlayer *pOwner, char *pszItemCode, int nNum, char *pszUpTalCode, int nUpNum)
{

    __int64 *v5; // rdi
    __int64 i; // rcx
    __int64 v8; // [rsp+0h] [rbp-108h] BYREF
    int nTableCode; // [rsp+50h] [rbp-B8h]
    _base_fld *Record; // [rsp+58h] [rbp-B0h]
    unsigned int dwExp; // [rsp+60h] [rbp-A8h]
    unsigned int dwExp_4; // [rsp+64h] [rbp-A4h]
    unsigned __int8 DefItemUpgSocketNum; // [rsp+68h] [rbp-A0h]
    unsigned __int8 ItemKindCode; // [rsp+69h] [rbp-9Fh]
    _ItemUpgrade_fld *RecordFromRes; // [rsp+70h] [rbp-98h]
    int ItemTableCode; // [rsp+78h] [rbp-90h]
    _base_fld *v17; // [rsp+80h] [rbp-88h]
    int j; // [rsp+88h] [rbp-80h]
    _STORAGE_LIST::_db_con pItem; // [rsp+98h] [rbp-70h] BYREF
    const _TimeItem_fld *TimeRec; // [rsp+D8h] [rbp-30h]
    __time32_t Time[4]; // [rsp+E4h] [rbp-24h] BYREF
    int k; // [rsp+F4h] [rbp-14h]
  nTableCode = GetItemTableCode(pszItemCode);
    if ( nTableCode == -1 )
      return 0;
    if ( nTableCode == 19 )
      return 0;
    Record = g_Main.m_tblItemData[nTableCode].GetRecord( pszItemCode);
    if ( !Record )
      return 0;
    dwExp = 0;
    if ( IsOverLapItem(nTableCode) )
      dwExp = 99;
    else
      dwExp = GetItemDurPoint(nTableCode, Record->m_dwIndex);
    if ( nNum > 100 )
      nNum = 100;
    ItemKindCode = GetItemKindCode(nTableCode);
    if ( ItemKindCode )
    {
      if ( ItemKindCode != 1 )
        return 0;
      dwExp_4 = GetMaxParamFromExp(Record->m_dwIndex, dwExp);
    }
    else
    {
      DefItemUpgSocketNum = GetDefItemUpgSocketNum(nTableCode, Record->m_dwIndex);
      dwExp_4 = GetBitAfterSetLimSocket(DefItemUpgSocketNum);
    }
    RecordFromRes = 0LL;
    if ( !DefItemUpgSocketNum || !pszUpTalCode )
      goto LABEL_29;
    ItemTableCode = GetItemTableCode(pszUpTalCode);
    if ( ItemTableCode != 18 )
      return 0;
    v17 = g_Main.m_tblItemData[18].GetRecord( pszUpTalCode);
    if ( !v17 )
      return 0;
    RecordFromRes = g_Main.m_tblItemUpgrade.GetRecordFromRes(v17->m_dwIndex);
    if ( !RecordFromRes || RecordFromRes->m_dwIndex >= 0xD )
      return 0;
  LABEL_29:
    for ( j = 0; j < nNum; ++j )
    {
      pItem.Init();
      pItem.m_byTableCode = nTableCode;
      pItem.m_wItemIndex = Record->m_dwIndex;
      pItem.m_dwDur = dwExp;
      TimeRec = TimeItem::FindTimeRec(nTableCode, Record->m_dwIndex);
      if ( TimeRec && TimeRec->m_nCheckType )
      {
        _time32(Time);
        pItem.m_byCsMethod = TimeRec->m_nCheckType;
        pItem.m_dwT = TimeRec->m_nUseTime + Time[0];
        pItem.m_dwLendRegdTime = Time[0];
      }
      if ( RecordFromRes )
      {
        if ( nUpNum > DefItemUpgSocketNum )
          nUpNum = DefItemUpgSocketNum;
        for ( k = 0; k < nUpNum; ++k )
          dwExp_4 = GetBitAfterUpgrade(dwExp_4, RecordFromRes->m_dwIndex, k);
      }
      pItem.m_dwLv = dwExp_4;
      if ( !CreateItemBox(
              &pItem,
              pOwner,
              0xFFFFFFFF,
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
