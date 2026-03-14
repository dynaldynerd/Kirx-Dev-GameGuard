#include "pch.h"

#include "CPcBangFavor.h"

#include <array>

#include "CPlayer.h"
#include "CUserDB.h"
#include "GlobalObjects.h"
#include "PcRoom_fld.h"
#include "WorldServerUtil.h"

CPcBangFavor *CPcBangFavor::ms_pInstance = nullptr;

CPcBangFavor::CPcBangFavor()
{
  m_bEnable = 0;
}

CPcBangFavor::~CPcBangFavor() = default;

CPcBangFavor *CPcBangFavor::Instance()
{
  if (!ms_pInstance)
  {
    ms_pInstance = new CPcBangFavor();
  }
  return ms_pInstance;
}

bool CPcBangFavor::Initialzie()
{
  char returnedString[40]{};
  std::memset(returnedString, 0, 10);
  GetPrivateProfileStringA(
    "PcBang Favor",
    "USE",
    "FALSE",
    returnedString,
    10,
    ".\\Initialize\\WorldSystem.ini");
  m_bEnable = std::strcmp(returnedString, "FALSE") != 0;
  return true;
}

bool CPcBangFavor::LoadPcBangData()
{
  char pszErrMsg[144] = {};
  if (m_tblPcRoomData.ReadRecord(".\\script\\PcRoom.dat", 676, pszErrMsg))
  {
    return true;
  }
  MyMessageBox("DatafileInit", pszErrMsg);
  return false;
}

bool CPcBangFavor::IsEnable()
{
  return m_bEnable != 0;
}

int CPcBangFavor::ClassCodePasing(_AVATOR_DATA *pData, CPlayer *pOne)
{
  if (!m_bEnable || !pData || !pOne)
  {
    return -1;
  }

  char codeBuffer[32]{};
  char classCode[32]{};
  char classPrefix[20]{};

  _base_fld *record = g_Main.m_tblClass.GetRecord(pData->dbAvator.m_szClassCode);
  if (!record)
  {
    return -1;
  }

  std::strcpy(classCode, record->m_strCode);
  if (static_cast<unsigned int>(pOne->m_Param.GetRaceCode()) == 2
      && classCode[1] == 'R'
      && classCode[3] == '1')
  {
    strncpy(classPrefix, record->m_strCode, 1u);
    const int level = static_cast<int>(pOne->m_Param.GetLevel());
    sprintf_s(codeBuffer, "%sf%02d", classPrefix, level);
  }
  else
  {
    strncpy(classPrefix, record->m_strCode, 2u);
    const int level = static_cast<int>(pOne->m_Param.GetLevel());
    sprintf_s(codeBuffer, "%s%02d", classPrefix, level);
  }

  _strlwr(codeBuffer);
  _PcRoom_fld *pcRoomRecord = static_cast<_PcRoom_fld *>(m_tblPcRoomData.GetRecord(codeBuffer));
  if (!pcRoomRecord)
  {
    return -1;
  }

  pOne->m_dwPcBangGiveItemListIndex = pcRoomRecord->m_dwIndex;
  pOne->SendMsg_PcRoomCharClass(pcRoomRecord->m_dwIndex);
  return static_cast<int>(pcRoomRecord->m_dwIndex);
}

bool CPcBangFavor::PcBangGiveItem(
  CPlayer *pOne,
  int dwRecIndex,
  unsigned __int8 *bySeletItemIndex,
  int nSelectCount)
{
  if (!pOne || !pOne->m_bOper)
  {
    return false;
  }
  if (nSelectCount > 5)
  {
    return false;
  }
  if (dwRecIndex != static_cast<int>(pOne->m_dwPcBangGiveItemListIndex)
      && pOne->m_dwPcBangGiveItemListIndex != static_cast<unsigned int>(-1))
  {
    return false;
  }

  pOne->m_dwPcBangGiveItemListIndex = static_cast<unsigned int>(-1);
  _PcRoom_fld *record = static_cast<_PcRoom_fld *>(m_tblPcRoomData.GetRecord(dwRecIndex));
  if (!record)
  {
    return false;
  }

  std::array<_STORAGE_LIST::_db_con, 15> makeItems{};
  int makeItemCount = 0;

  for (int j = 0; j < 5; ++j)
  {
    const int itemCount = record->m_SelectSupply[j].m_nSelectNumber;
    if (itemCount < 0)
    {
      continue;
    }

    const unsigned __int8 selectedItemIndex = bySeletItemIndex[j];
    if (selectedItemIndex >= 10u)
    {
      continue;
    }

    const char *itemCode = record->m_SelectSupply[j].m_itmSelect[selectedItemIndex];
    const int tableCode = GetItemTableCode(itemCode);
    if (tableCode == -1)
    {
      continue;
    }

    _base_fld *itemRecord = g_Main.m_tblItemData[tableCode].GetRecordByHash(itemCode, 2, 5);
    if (!itemRecord)
    {
      continue;
    }

    _STORAGE_LIST::_db_con *loot = MakeLoot(static_cast<unsigned __int8>(tableCode), itemRecord->m_dwIndex);
    if (!loot)
    {
      continue;
    }

    if (IsOverLapItem(tableCode))
    {
      loot->m_dwDur = static_cast<unsigned __int64>(itemCount);
    }

    makeItems[makeItemCount++] = *loot;
  }

  for (int j = 0; j < 10; ++j)
  {
    const int itemCount = record->m_FixSupply[j].m_nFixNumber;
    if (itemCount < 0)
    {
      continue;
    }

    const char *itemCode = record->m_FixSupply[j].m_itmFix;
    const int tableCode = GetItemTableCode(itemCode);
    if (tableCode == -1)
    {
      continue;
    }

    _base_fld *itemRecord = g_Main.m_tblItemData[tableCode].GetRecordByHash(itemCode, 2, 5);
    if (!itemRecord)
    {
      continue;
    }

    _STORAGE_LIST::_db_con *loot = MakeLoot(static_cast<unsigned __int8>(tableCode), itemRecord->m_dwIndex);
    if (!loot)
    {
      continue;
    }

    if (IsOverLapItem(tableCode))
    {
      loot->m_dwDur = static_cast<unsigned __int64>(itemCount);
    }

    makeItems[makeItemCount++] = *loot;
  }

  if (makeItemCount <= 0)
  {
    return false;
  }

  const int emptySlotCount = pOne->m_Param.m_dbInven.GetNumEmptyCon();
  if (makeItemCount >= emptySlotCount)
  {
    pOne->SendMsg_PcRoomError(1);
    return false;
  }

  for (int j = 0; j < makeItemCount; ++j)
  {
    _STORAGE_LIST::_db_con *makeItem = &makeItems[j];
    makeItem->m_wSerial = pOne->m_Param.GetNewItemSerial();
    _STORAGE_LIST::_db_con *addedItem = pOne->Emb_AddStorage(0, makeItem, false, true);
    pOne->SendMsg_RewardAddItem(makeItem, 9u);
    if (addedItem)
    {
      pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.InsertItem(addedItem);
    }
  }

  pOne->SendMsg_PcRoomError(0);
  return true;
}

void CPcBangFavor::PcBangDeleteItem(CPlayer *pOne)
{
  if (pOne && pOne->m_bLive)
  {
    for (int index = 0; ; ++index)
    {
      const unsigned __int8 bagNum = pOne->m_Param.GetBagNum();
      if (index >= 20 * bagNum)
      {
        break;
      }

      _STORAGE_LIST::_db_con *item = &pOne->m_Param.m_dbInven.m_pStorageList[index];
      if (item
          && item->m_bLoad
          && pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.IsDeleteItem(item))
      {
        pOne->SendMsg_DeleteStorageInform(0, item->m_wSerial);
        pOne->Emb_DelStorage(0, item->m_byStorageIndex, false, true, nullptr);
      }
    }

    for (int index = 0; index < 8; ++index)
    {
      _STORAGE_LIST::_db_con *item = &pOne->m_Param.m_dbEquip.m_pStorageList[index];
      if (item
          && item->m_bLoad
          && pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.IsDeleteItem(item))
      {
        pOne->SendMsg_DeleteStorageInform(1u, item->m_wSerial);
        pOne->Emb_DelStorage(1u, item->m_byStorageIndex, false, true, nullptr);
      }
    }

    for (int index = 0; index < 7; ++index)
    {
      _STORAGE_LIST::_db_con *item = &pOne->m_Param.m_dbEmbellish.m_pStorageList[index];
      if (item
          && item->m_bLoad
          && pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.IsDeleteItem(item))
      {
        pOne->SendMsg_DeleteStorageInform(2u, item->m_wSerial);
        pOne->Emb_DelStorage(2u, item->m_byStorageIndex, false, true, nullptr);
      }
    }

    pOne->m_pUserDB->m_AvatorData.dbPcBangFavorItem.Init();
  }
}

