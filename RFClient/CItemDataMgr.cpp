#include "CItemDataMgr.h"

#include <cstring>

#include "ClientDataPath.h"

CItemDataMgr *g_pItemDataMgr = NULL;

namespace
{
constexpr DWORD kClientItemRestDataSize = 0x168;
constexpr DWORD kClientItemHeaderKindPrefix = 0xF100;
constexpr DWORD kItemRecordMeshIDOffset = 72;
constexpr DWORD kItemRecordIconIDOffset = 76;
constexpr DWORD kItemRecordMinDataSize = 80;

const DWORD kClientItemRecordSize[47] =
{
  156, 240, 240, 240, 240, 240, 276, 240, 244, 228,
  228, 232, 156, 204, 156, 160, 168, 292, 160, 220,
  156, 188, 228, 192, 156, 296, 192, 240, 224, 156,
  168, 168, 156, 188, 200, 180, 176, 224, 224, 224,
  256, 256, 232, 240, 104, 108, 406
};

BOOL ReadExact(CDataString *pi_pData, void *po_pBuffer, int pi_nSize)
{
  if (!pi_pData || !po_pBuffer || pi_nSize <= 0)
  {
    return FALSE;
  }

  return pi_pData->Read(po_pBuffer, pi_nSize, 1) == static_cast<DWORD>(pi_nSize);
}

template <typename T>
T ReadRecordValue(const std::vector<BYTE> &pi_vecRecord, DWORD pi_dwOffset)
{
  T l_Value = T();
  if (pi_dwOffset + sizeof(T) <= pi_vecRecord.size())
  {
    memcpy(&l_Value, &pi_vecRecord[pi_dwOffset], sizeof(T));
  }

  return l_Value;
}

BOOL LoadNationFile(CEdfFile &po_rFile, const char *pi_pFileName)
{
  char l_szFileName[MAX_PATH];
  if (!GetNationDataFileName(pi_pFileName, l_szFileName, sizeof(l_szFileName)))
  {
    return FALSE;
  }

  return po_rFile.Load(l_szFileName);
}
} // namespace

CItemDataMgr::CItemDataMgr()
  : m_bCreated(FALSE),
    m_bLoaded(FALSE)
{
}

CItemDataMgr::~CItemDataMgr()
{
  Destroy();
}

BOOL CItemDataMgr::Create(void)
{
  if (m_bCreated)
  {
    return TRUE;
  }

  m_bCreated = TRUE;
  return TRUE;
}

BOOL CItemDataMgr::Destroy(void)
{
  UnloadData();
  m_bCreated = FALSE;
  return TRUE;
}

BOOL CItemDataMgr::LoadData(void)
{
  if (!m_bCreated)
  {
    return FALSE;
  }

  UnloadData();

  if (!m_cItemData.Load(".\\DataTable\\item.edf") ||
      !ParseItemData() ||
      !m_cItemCashData.Load(".\\DataTable\\ItemCash.edf") ||
      !m_cItemCombineData.Load(".\\DataTable\\ItemCombine.edf") ||
      !m_cItemPremiumData.Load(".\\DataTable\\ItemPremium.edf") ||
      !m_cItemSetEffectData.Load(".\\DataTable\\ItemSetEffect.edf") ||
      !m_cItemTimerData.Load(".\\DataTable\\ItemTimer.edf") ||
      !LoadNationFile(m_cNationItemData, "NDItem.edf") ||
      !LoadNationFile(m_cNationItemCashData, "NDItemCash.edf"))
  {
    UnloadData();
    return FALSE;
  }

  m_bLoaded = TRUE;
  return TRUE;
}

void CItemDataMgr::UnloadData(void)
{
  for (int i = 0; i < CLIENT_ITEM_SECTION_NUM; ++i)
  {
    m_vecItemRecords[i].clear();
  }

  m_cItemData.Unload();
  m_cItemCashData.Unload();
  m_cItemCombineData.Unload();
  m_cItemPremiumData.Unload();
  m_cItemSetEffectData.Unload();
  m_cItemTimerData.Unload();
  m_cNationItemData.Unload();
  m_cNationItemCashData.Unload();
  m_bLoaded = FALSE;
}

BOOL CItemDataMgr::IsLoaded(void) const
{
  return m_bLoaded;
}

BOOL CItemDataMgr::ParseItemData(void)
{
  CDataString *l_pSourceData = m_cItemData.GetSourceData();
  if (!l_pSourceData || !l_pSourceData->GetString())
  {
    return FALSE;
  }

  DWORD l_dwCurrentOffset = 0;
  for (int i = 0; i < CLIENT_ITEM_SECTION_NUM; ++i)
  {
    const DWORD l_dwHeaderOffset = l_dwCurrentOffset;
    WORD l_wKind = 0;
    LONG l_nTotalSize = 0;
    LONG l_nStartOffset = 0;
    LONG l_nRecordCount = 0;
    LONG l_nRecordSize = 0;

    if (!ReadExact(l_pSourceData, &l_wKind, sizeof(l_wKind)) ||
        !ReadExact(l_pSourceData, &l_nTotalSize, sizeof(l_nTotalSize)) ||
        !ReadExact(l_pSourceData, &l_nStartOffset, sizeof(l_nStartOffset)) ||
        !ReadExact(l_pSourceData, &l_nRecordCount, sizeof(l_nRecordCount)) ||
        !ReadExact(l_pSourceData, &l_nRecordSize, sizeof(l_nRecordSize)))
    {
      return FALSE;
    }
    l_dwCurrentOffset += sizeof(WORD) + (sizeof(LONG) * 4);

    const BYTE l_byKind = static_cast<BYTE>(l_wKind & 0x00FF);
    if ((l_wKind & 0xFF00) != kClientItemHeaderKindPrefix ||
        (i < CLIENT_ITEM_SECTION_NUM - 1 && l_byKind != static_cast<BYTE>(i)) ||
        (i == CLIENT_ITEM_SECTION_NUM - 1 && l_byKind != 47) ||
        l_nStartOffset != static_cast<LONG>(l_dwHeaderOffset) ||
        l_nRecordCount < 0 ||
        l_nRecordSize != static_cast<LONG>(kClientItemRecordSize[i]))
    {
      return FALSE;
    }

    const LONG l_nExpectedTotalSize =
      static_cast<LONG>(sizeof(LONG) * 2) +
      ((i == CLIENT_ITEM_SECTION_NUM - 1)
         ? static_cast<LONG>(kClientItemRestDataSize)
         : (l_nRecordCount * l_nRecordSize));
    if (l_nTotalSize != l_nExpectedTotalSize)
    {
      return FALSE;
    }

    m_vecItemRecords[i].clear();
    if (i <= IEPT_CLOAK)
    {
      m_vecItemRecords[i].reserve(static_cast<size_t>(l_nRecordCount));
    }

    for (LONG j = 0; j < l_nRecordCount; ++j)
    {
      std::vector<BYTE> l_vecRecord(static_cast<size_t>(l_nRecordSize));
      if (!l_vecRecord.empty() &&
          l_pSourceData->Read(&l_vecRecord[0], l_nRecordSize, 1) != static_cast<DWORD>(l_nRecordSize))
      {
        return FALSE;
      }
      l_dwCurrentOffset += static_cast<DWORD>(l_nRecordSize);

      if (i <= IEPT_CLOAK && l_vecRecord.size() >= kItemRecordMinDataSize)
      {
        ITEM_CLIENT_RECORD l_Record = {};
        l_Record.dwDTIndex = ReadRecordValue<DWORD>(l_vecRecord, 0);
        l_Record.dwDTCode = ReadRecordValue<DWORD>(l_vecRecord, 4);
        l_Record.dwMeshID = ReadRecordValue<DWORD>(l_vecRecord, kItemRecordMeshIDOffset);
        l_Record.dwIconID = ReadRecordValue<DWORD>(l_vecRecord, kItemRecordIconIDOffset);
        l_Record.byItemType = static_cast<BYTE>(i);
        m_vecItemRecords[i].push_back(l_Record);
      }
    }
  }

  BYTE l_abyRestData[kClientItemRestDataSize];
  if (!ReadExact(l_pSourceData, l_abyRestData, sizeof(l_abyRestData)))
  {
    return FALSE;
  }

  l_dwCurrentOffset += kClientItemRestDataSize;
  return l_dwCurrentOffset == l_pSourceData->GetStringLength();
}

const ITEM_CLIENT_RECORD *CItemDataMgr::GetClientRecord(BYTE pi_byItemType, DWORD pi_dwDTIndex) const
{
  if (pi_byItemType >= CLIENT_ITEM_SECTION_NUM)
  {
    return NULL;
  }

  const std::vector<ITEM_CLIENT_RECORD> &l_vecRecords = m_vecItemRecords[pi_byItemType];
  if (pi_dwDTIndex < l_vecRecords.size())
  {
    const ITEM_CLIENT_RECORD &l_Record = l_vecRecords[pi_dwDTIndex];
    if (l_Record.dwDTIndex == pi_dwDTIndex)
    {
      return &l_Record;
    }
  }

  for (std::vector<ITEM_CLIENT_RECORD>::const_iterator it = l_vecRecords.begin();
       it != l_vecRecords.end();
       ++it)
  {
    if (it->dwDTIndex == pi_dwDTIndex)
    {
      return &(*it);
    }
  }

  return NULL;
}

DWORD CItemDataMgr::GetClientMeshID(BYTE pi_byItemType, DWORD pi_dwDTIndex) const
{
  const ITEM_CLIENT_RECORD *l_pRecord = GetClientRecord(pi_byItemType, pi_dwDTIndex);
  return l_pRecord ? l_pRecord->dwMeshID : static_cast<DWORD>(-1);
}
