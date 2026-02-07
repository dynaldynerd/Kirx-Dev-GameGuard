#include "pch.h"

#include "CExchangeEvent.h"

#include "CRecordData.h"
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
