#include "pch.h"

#include "CExchangeEvent.h"

#include "CRecordData.h"
#include "CPlayer.h"
#include "WorldServerUtil.h"
#include "base_fld.h"

CExchangeEvent::CExchangeEvent()
  : m_bEnable(false),
    m_bDelete(false),
    m_byState(0),
    m_bGiveItem(false),
    m_bWait(false),
    m_bModifyEnable(false),
    m_bModifyDelete(false),
    m_dwPlayerArrayNo(0)
{
  memset_0(m_EventItemCode, 0, sizeof(m_EventItemCode));
  for (int j = 0; j < 4; ++j)
  {
    m_EventItemInfo[j].byTableCode = static_cast<unsigned __int8>(-1);
    m_EventItemInfo[j].dwIndex = static_cast<unsigned int>(-1);
  }
  memset_0(m_ModifyItemCode, 0, sizeof(m_ModifyItemCode));
}

CExchangeEvent *CExchangeEvent::Instance()
{
  static CExchangeEvent s_instance;
  return &s_instance;
}

bool CExchangeEvent::Initialzie()
{
  if (!GetLastWriteFileTime(".\\Initialize\\WorldSystem.ini", &m_ftWrite))
  {
    return false;
  }

  ReadBuddhaEventInfo();
  m_bEnable = m_bModifyEnable;
  m_bDelete = m_bModifyDelete;
  memcpy_0(m_EventItemCode, m_ModifyItemCode, sizeof(m_EventItemCode));

  if (m_bEnable || m_bDelete)
  {
    for (int j = 0; j < 4; ++j)
    {
      const unsigned __int8 itemTableCode =
        static_cast<unsigned __int8>(GetItemTableCode(m_EventItemCode[static_cast<__int64>(j)]));
      if (itemTableCode != static_cast<unsigned __int8>(-1))
      {
        _base_fld *record =
          g_Main.m_tblItemData[itemTableCode].GetRecord(m_EventItemCode[static_cast<__int64>(j)]);
        if (record)
        {
          m_EventItemInfo[j].byTableCode = itemTableCode;
          m_EventItemInfo[j].dwIndex = record->m_dwIndex;
        }
      }
    }
  }

  m_tmDataFileCheckTime.BeginTimer(0x2710u);
  return true;
}

bool CExchangeEvent::IsEnable()
{
  return m_bEnable;
}

bool CExchangeEvent::IsDelete()
{
  return m_bDelete;
}

bool CExchangeEvent::IsWait()
{
  return m_bWait;
}

EventItemInfo *CExchangeEvent::GetEventItemInfo(unsigned int nInfoType)
{
  if (nInfoType < 4)
  {
    return &m_EventItemInfo[nInfoType];
  }
  return nullptr;
}

void CExchangeEvent::DeleteExchangeEventItem(CPlayer *pOne)
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
          && ((item->m_byTableCode == m_EventItemInfo[0].byTableCode
               && item->m_wItemIndex == m_EventItemInfo[0].dwIndex)
              || (item->m_byTableCode == m_EventItemInfo[1].byTableCode
                  && item->m_wItemIndex == m_EventItemInfo[1].dwIndex)
              || (item->m_byTableCode == m_EventItemInfo[2].byTableCode
                  && item->m_wItemIndex == m_EventItemInfo[2].dwIndex)
              || (item->m_byTableCode == m_EventItemInfo[3].byTableCode
                  && item->m_wItemIndex == m_EventItemInfo[3].dwIndex)))
      {
        if (!pOne->Emb_DelStorage(0, item->m_byStorageIndex, false, true, nullptr))
        {
          return;
        }
        pOne->SendMsg_DeleteStorageInform(0, item->m_wSerial);
        pOne->SendMsg_BuddhaEventMsg(2);
      }
    }
  }
}

void CExchangeEvent::GiveEventItem(CPlayer *pOne)
{
  if (pOne && pOne->m_bOper)
  {
    if (pOne->m_Param.m_dbInven.GetIndexEmptyCon() == 0xFF)
    {
      CPlayer::SendMsg_BuddhaEventMsg(pOne, 1u);
      return;
    }

    EventItemInfo *itemInfo = GetEventItemInfo(0);
    if (itemInfo && itemInfo->byTableCode != 0xFF)
    {
      _STORAGE_LIST::_db_con *lootItem = MakeLoot(itemInfo->byTableCode, itemInfo->dwIndex);
      if (lootItem)
      {
        _STORAGE_LIST::_db_con itemCopy{};
        memcpy_0(&itemCopy, lootItem, sizeof(itemCopy));
        itemCopy.m_wSerial = CPlayerDB::GetNewItemSerial(&pOne->m_Param);
        if (CPlayer::Emb_AddStorage(
              pOne,
              0,
              reinterpret_cast<_STORAGE_LIST::_storage_con *>(&itemCopy),
              false,
              true))
        {
          CPlayer::SendMsg_RewardAddItem(pOne, &itemCopy, 7u);
          CPlayer::SendMsg_BuddhaEventMsg(pOne, 0);
        }
      }
    }
  }
}

void CExchangeEvent::ReadBuddhaEventInfo()
{
  char enableStr[32]{};
  char deleteStr[24]{};

  GetPrivateProfileStringA(
    "Buddha Event",
    "Enable",
    "FALSE",
    enableStr,
    8u,
    ".\\Initialize\\WorldSystem.ini");
  GetPrivateProfileStringA(
    "Buddha Event",
    "Delete",
    "TRUE",
    deleteStr,
    8u,
    ".\\Initialize\\WorldSystem.ini");
  GetPrivateProfileStringA(
    "Buddha Event",
    "GiveItem",
    "NULL",
    m_ModifyItemCode[0],
    0x40u,
    ".\\Initialize\\WorldSystem.ini");
  GetPrivateProfileStringA(
    "Buddha Event",
    "ExchangeItem",
    "NULL",
    m_ModifyItemCode[1],
    0x40u,
    ".\\Initialize\\WorldSystem.ini");
  GetPrivateProfileStringA(
    "Buddha Event",
    "DeleteItem1",
    "NULL",
    m_ModifyItemCode[2],
    0x40u,
    ".\\Initialize\\WorldSystem.ini");
  GetPrivateProfileStringA(
    "Buddha Event",
    "DeleteItem2",
    "NULL",
    m_ModifyItemCode[3],
    0x40u,
    ".\\Initialize\\WorldSystem.ini");

  m_bModifyEnable = strcmp_0(enableStr, "TRUE") == 0;
  m_bModifyDelete = strcmp_0(deleteStr, "TRUE") == 0;
}
