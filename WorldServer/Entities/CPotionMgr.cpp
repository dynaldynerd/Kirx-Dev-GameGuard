#include "pch.h"

#include "CPotionMgr.h"

#include "CNationSettingManager.h"
#include "CRFWorldDatabase.h"
#include "CLogTypeDBTaskManager.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "CheckPotion_fld.h"
#include "GlobalObjects.h"
#include "NameTxt_fld.h"
#include "skill_fld.h"

#include <mmsystem.h>

CPotionMgr g_PotionMgr;

bool CPotionMgr::DatafileInit()
{
    char pszErrMsg[144]; // [rsp+30h] [rbp-A8h] BYREF

    if (m_tblPotionEffectData.ReadRecord(".\\script\\PotionItemEffect.dat", 0x490u, pszErrMsg))
    {
        if (m_tblPotionCheckData.ReadRecord(".\\script\\CheckPotionEffect.dat", 0xDCu, pszErrMsg))
        {
            return true;
        }
        else
        {
            MyMessageBox("DatafileInit", pszErrMsg);
            return false;
        }
    }
    else
    {
        MyMessageBox("DatafileInit", pszErrMsg);
        return false;
    }
}

bool CPotionMgr::SetPotionDataName()
{
  if (!m_tblPotionEffectData.IsTableOpen() || !m_tblPotionCheckData.IsTableOpen())
  {
    MyMessageBox("CPotionMgr::SetPotionDataName()", "PotionItemEffectData Or CheckPotionEffectData Not Load!");
    return false;
  }

  char errMsg[160]{};
  CRecordData potionEffectNames;
  if (!potionEffectNames.ReadRecord(".\\script\\PotionItemEffect_str.dat", 0x304, errMsg))
  {
    MyMessageBox("CPotionMgr::SetPotionDataName()", "PotionItemEffect Name Data Load Error, ErrorMsg(%s)", errMsg);
    return false;
  }

  int recordNum = static_cast<int>(m_tblPotionEffectData.GetRecordNum());
  for (int n = 0; n < recordNum; ++n)
  {
    _skill_fld *record = reinterpret_cast<_skill_fld *>(m_tblPotionEffectData.GetRecord(n));
    if (!record)
    {
      MyMessageBox("CPotionMgr::SetPotionDataName()", "Get Potion Effect Data Error, PotionEffectIndex(%d)", n);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(potionEffectNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "CPotionMgr::SetPotionDataName()",
        "Get Potion Effect Name Data Error, PotionEffectCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strKorName, 0x40, name);
  }

  CRecordData checkEffectNames;
  if (!checkEffectNames.ReadRecord(".\\script\\CheckPotionEffect_str.dat", 0x304, errMsg))
  {
    MyMessageBox(
      "CPotionMgr::SetPotionDataName()",
      "CheckPotionEffect Name Data Load Error, ErrorMsg(%s)",
      errMsg);
    return false;
  }

  recordNum = static_cast<int>(m_tblPotionCheckData.GetRecordNum());
  for (int n = 0; n < recordNum; ++n)
  {
    _CheckPotion_fld *record = reinterpret_cast<_CheckPotion_fld *>(m_tblPotionCheckData.GetRecord(n));
    if (!record)
    {
      MyMessageBox(
        "CPotionMgr::SetPotionDataName()",
        "Get Potion Effect Check Data Error, PotionEffectCheckIndex(%d)",
        n);
      return false;
    }
    _NameTxt_fld *nameRecord = reinterpret_cast<_NameTxt_fld *>(potionEffectNames.GetRecord(record->m_strCode));
    if (!nameRecord)
    {
      MyMessageBox(
        "CPotionMgr::SetPotionDataName()",
        "Get Potion Effect Check Name Data Error, PotionEffectCheckCode(%s)",
        record->m_strCode);
      return false;
    }
    const char *name = CNationSettingManager::Instance()->GetItemName(nameRecord);
    strcpy_s(record->m_strKOR_name, 0x40, name);
  }

  return true;
}

bool CPotionMgr::InsertRenamePotion(CRFWorldDatabase *pkWorldDB, char *pData)
{
  return pkWorldDB->Insert_RenamePotionLog(
    *reinterpret_cast<unsigned int *>(pData),
    pData + 4,
    pData + 21);
}

bool CPotionMgr::IsPotionDelayUseIndex(unsigned int nIndex)
{
  return nIndex < 0x26 && m_PotionInnerData.m_bPotionDelayIndexList[nIndex];
}

void CPotionMgr::Complete_RenameChar_DB_Select(unsigned __int8 byRet, char *p)
{
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, *reinterpret_cast<unsigned int *>(p));
  if (player && player->m_bOper)
  {
    if (byRet)
    {
      player->SendMsg_CharacterRenameCashResult(0, 0x1Au);
    }
    else if (*reinterpret_cast<unsigned int *>(p + 4) == static_cast<unsigned int>(-1))
    {
      memcpy_s(&player->m_ReNamePotionUseInfo, 4u, p + 8, 4u);
      strcpy_s(player->m_ReNamePotionUseInfo.wszChangeName, 0x11u, p + 12);
      player->SendMsg_CharacterRenameCashResult(0, 0);
    }
    else
    {
      player->SendMsg_CharacterRenameCashResult(0, 0xAu);
    }
  }
}

void CPotionMgr::Complete_RenameChar_DB_Update(unsigned __int8 byRet, char *p)
{
  unsigned __int8 resultCode = 0;
  CPlayer *player = GetPtrPlayerFromSerial(g_Player, MAX_PLAYER, *reinterpret_cast<unsigned int *>(p));

  if (byRet)
  {
    if (player && player->m_bOper)
    {
      _STORAGE_LIST *storageList = player->m_Param.m_pStoragePtr[static_cast<unsigned __int8>(p[8])];
      _STORAGE_LIST::_storage_con *storage = storageList
        ? storageList->GetPtrFromSerial(*reinterpret_cast<unsigned __int16 *>(p + 9))
        : nullptr;
      if (storage)
      {
        storage->lock(false);
      }
    }
    resultCode = 26;
  }
  else
  {
    if (player && player->m_bOper)
    {
      _STORAGE_LIST *storageList = player->m_Param.m_pStoragePtr[static_cast<unsigned __int8>(p[8])];
      _STORAGE_LIST::_db_con *useItem =
        storageList ? storageList->GetPtrFromSerial(*reinterpret_cast<unsigned __int16 *>(p + 9)) : nullptr;

      if (!useItem)
      {
        player->m_ReNamePotionUseInfo.Init();
        const char *charName = player->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "CPotionMgr::Complete_RenameChar_DB_Update : %u(%s) pUseItem NULL!",
          player->m_dwObjSerial,
          charName);
        return;
      }

      _base_fld *record = g_Main.m_tblItemData[13].GetRecord(useItem->m_wItemIndex);
      if (!record)
      {
        const char *charName = player->m_Param.GetCharNameA();
        g_Main.m_logSystemError.Write(
          "CPotionMgr::Complete_RenameChar_DB_Update : %u(%s) pPotionFld NULL!",
          player->m_dwObjSerial,
          charName);
      }

      const unsigned __int8 adjustResult =
        player->Emb_AlterDurPoint(0, useItem->m_byStorageIndex, -1, 0, 0);
      player->SendMsg_AdjustAmountInform(0, useItem->m_wSerial, adjustResult);

      if (record && *reinterpret_cast<unsigned int *>(&record[9].m_strCode[36]) == 1)
      {
        CPlayer::s_MgrItemHistory.cash_item_use(
          player->m_ObjID.m_wIndex,
          useItem,
          player->m_szItemHistoryFileName);
      }

      if (record)
      {
        const unsigned int actDelay = static_cast<unsigned int>(*reinterpret_cast<float *>(record[6].m_strCode));
        const DWORD now = timeGetTime();
        player->SetPotionActDelay(record[6].m_strCode[4], now / 1000, actDelay);
      }

      strcpy_0(player->m_Param.m_dbChar.m_wszCharID, p + 12);
      strcpy_0(player->m_pUserDB->m_wszAvatorName, p + 12);
      CPlayer::CashChangeStateFlag changeFlag(1);
      player->UpdateVisualVer(changeFlag);

      unsigned int objSerial = player->m_dwObjSerial;
      char nameBuffer[38]{};
      strcpy_0(nameBuffer, player->m_pUserDB->m_wszAvatorName);
      unsigned __int8 type[2]{3, 61};
      unsigned __int8 report[23]{};
      memcpy_0(report + 2, &objSerial, sizeof(objSerial));
      memcpy_0(report + 6, nameBuffer, 17);
      player->CircleReport(type, reinterpret_cast<char *>(report), 23, false);

      if (player->m_pPartyMgr->IsPartyMode())
      {
        player->pc_PartyLeaveSelfReqeuest();
      }
      if (player->m_Param.m_pGuild)
      {
        player->pc_GuildSelfLeaveRequest();
      }
      player->m_NameChangeBuddyInfo.bNameChange = 1;
    }

    PushRenamePotionDBLog(p);
  }

  if (player && player->m_bOper)
  {
    player->m_ReNamePotionUseInfo.Init();
    player->SendMsg_CharacterRenameCashResult(1, byRet);
  }
}

void CPotionMgr::PushRenamePotionDBLog(char *pInfo)
{
  char data[40]{};
  *reinterpret_cast<unsigned int *>(data) = *reinterpret_cast<unsigned int *>(pInfo);
  strcpy_s(data + 4, 0x11u, pInfo + 29);
  strcpy_s(data + 21, 0x11u, pInfo + 12);

  CLogTypeDBTaskManager *mgr = CLogTypeDBTaskManager::Instance();
  mgr->Push(2, data, 0x28u);
  g_Main.m_logRename.Write("CPotionMgr::PushRenamePotionDBLog()!");
}
