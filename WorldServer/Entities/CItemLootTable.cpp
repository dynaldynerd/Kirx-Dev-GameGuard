#include "pch.h"

#include "base_fld.h"

#include "CItemLootTable.h"

#include <mmsystem.h>

#include "CEventLootTable.h"
#include "WorldServerUtil.h"

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

  if (m_pTblEvent)
  {
    m_pTblEvent->ReadRecord();
  }

  return Indexing(itemTables, errCode);
}

bool CItemLootTable::Indexing(CRecordData *itemTables, char *errCode)
{
  const DWORD startTick = timeGetTime();

  for (int lootIndex = 0; lootIndex < m_nLootNum; ++lootIndex)
  {
    _base_fld *lootRecord = m_tblLoot.GetRecord(lootIndex);
    m_ppLinkCode[lootIndex] = nullptr;
    if (!lootRecord)
    {
      continue;
    }

    int *entryCountPtr = reinterpret_cast<int *>(&lootRecord[1].m_strCode[8]);
    if (*entryCountPtr <= 0)
    {
      continue;
    }

    int validEntryCount = 0;
    for (int entryIndex = 0; entryIndex < *entryCountPtr; ++entryIndex)
    {
      char *itemCode = &lootRecord[1].m_strCode[entryIndex * 8 + 12];
      if (!strcmp_0(itemCode, "0"))
      {
        break;
      }
      ++validEntryCount;
    }
    *entryCountPtr = validEntryCount;

    if (validEntryCount <= 0)
    {
      continue;
    }

    m_ppLinkCode[lootIndex] = new _linker_code[validEntryCount]();
    for (int entryIndex = 0; entryIndex < validEntryCount; ++entryIndex)
    {
      char *itemCode = &lootRecord[1].m_strCode[entryIndex * 8 + 12];
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
      m_ppLinkCode[lootIndex][entryIndex].bExist = static_cast<int>(static_cast<signed char>(itemRecord[1].m_dwIndex));
    }
  }

  __trace("loot load time :  %d", timeGetTime() - startTick);
  return true;
}
