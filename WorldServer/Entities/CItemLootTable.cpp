#include "pch.h"

#include "ItemExist_fld.h"
#include "ItemLooting_fld.h"

#include "CItemLootTable.h"

#include <mmsystem.h>

#include "CEventLootTable.h"
#include "WorldServerUtil.h"

CItemLootTable::CItemLootTable()
  : m_pTblEvent(nullptr),
    m_ppLinkCode(nullptr),
    m_nLootNum(0)
{
  m_pTblEvent = new CEventLootTable();
}

CItemLootTable::~CItemLootTable()
{
  if (m_ppLinkCode)
  {
    for (int index = 0; index < m_nLootNum; ++index)
    {
      if (m_ppLinkCode[index])
      {
        operator delete[](m_ppLinkCode[index]);
        m_ppLinkCode[index] = nullptr;
      }
    }

    operator delete[](m_ppLinkCode);
    m_ppLinkCode = nullptr;
  }

  if (m_pTblEvent)
  {
    delete m_pTblEvent;
    m_pTblEvent = nullptr;
  }
}

bool CItemLootTable::ReadRecord(const char *fileName, CRecordData *itemTables, char *errCode)
{
  if (!m_tblLoot.ReadRecord(fileName, 0x694, errCode))
  {
    return false;
  }

  m_nLootNum = m_tblLoot.GetRecordNum();
  if (m_nLootNum <= 0)
  {
    return false;
  }

  m_ppLinkCode = new _linker_code *[m_nLootNum];
  for (int index = 0; index < m_nLootNum; ++index)
  {
    m_ppLinkCode[index] = nullptr;
  }

  m_pTblEvent->ReadRecord();

  return Indexing(itemTables, errCode);
}

bool CItemLootTable::Indexing(CRecordData *itemTables, char *errCode)
{
  const DWORD startTick = timeGetTime();

  for (int lootIndex = 0; lootIndex < m_nLootNum; ++lootIndex)
  {
    _ItemLooting_fld *lootRecord = static_cast<_ItemLooting_fld *>(m_tblLoot.GetRecord(lootIndex));
    m_ppLinkCode[lootIndex] = nullptr;
    if (lootRecord->m_nLootListCount <= 0)
    {
      continue;
    }

    int validEntryCount = 0;
    for (int entryIndex = 0; entryIndex < lootRecord->m_nLootListCount; ++entryIndex)
    {
      char *itemCode = lootRecord->m_itmLootCodeKey[entryIndex];
      if (!strcmp_0(itemCode, "0"))
      {
        break;
      }
      ++validEntryCount;
    }
    lootRecord->m_nLootListCount = validEntryCount;

    if (lootRecord->m_nLootListCount <= 0)
    {
      continue;
    }

    m_ppLinkCode[lootIndex] = new _linker_code[lootRecord->m_nLootListCount];
    memset_0(m_ppLinkCode[lootIndex], 0, sizeof(_linker_code) * lootRecord->m_nLootListCount);

    for (int entryIndex = 0; entryIndex < lootRecord->m_nLootListCount; ++entryIndex)
    {
      char *itemCode = lootRecord->m_itmLootCodeKey[entryIndex];
      const int itemTableCode = GetItemTableCode(itemCode);
      if (itemTableCode == -1)
      {
        if (errCode)
        {
          std::sprintf(errCode, "CItemLootTable.. %d rec %s item no search table", lootIndex, itemCode);
        }
        return false;
      }

      _base_fld *itemRecord = itemTables[itemTableCode].GetRecordByHash(itemCode, 2, 5);
      if (!itemRecord)
      {
        if (errCode)
        {
          std::sprintf(errCode, "CItemLootTable.. %d rec %s item no search index", lootIndex, itemCode);
        }
        return false;
      }

      m_ppLinkCode[lootIndex][entryIndex].byTableCode = static_cast<unsigned __int8>(itemTableCode);
      m_ppLinkCode[lootIndex][entryIndex].wItemIndex = static_cast<unsigned __int16>(itemRecord->m_dwIndex);
      _ItemExist_fld *itemExistRecord = reinterpret_cast<_ItemExist_fld *>(itemRecord);
      m_ppLinkCode[lootIndex][entryIndex].bExist = static_cast<signed char>(itemExistRecord->m_bExist);
    }
  }

  __trace("loot load time :  %d", timeGetTime() - startTick);
  return true;
}
