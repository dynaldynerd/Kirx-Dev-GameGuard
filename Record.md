# GetRecord Casting logic Findings

During the implementation of `CMapOperation::Init` and its sub-flows, the following `GetRecord` casting behaviors were observed in the IDA source:

| File | Class::Method | Table Used | Cast Type | Notes |
|---|---|---|---|---|
| `CMapOperation.cpp` | `LoadMaps` | `m_tblMapData` | `_map_fld *` | Direct cast to specific type. |
| `CWorldSchedule.cpp` | `Init` | `m_tblSch` | `_WorldSchedule_fld *` | Decompiled code shows access like `Record[1].m_dwIndex`. This implies `Record` is treated as an array of structs where the base type and specific fields might have known offsets. |
| `CItemStore.cpp` | `CItemStore::GetNpcRaceCode` | `g_Main.m_tblNPC` | `_base_fld *` | Accesses `Record[1].m_strCode[60]` for race code. |
| `CItemStore.cpp` | `CItemStore::Init` | `g_Main.m_tblItemData[nTableCode]` | `_base_fld *` | Uses `Record[1].m_dwIndex` (SLOBYTE) for `bExist`. |
| `WorldServerUtil.cpp` | `GetItemStdPrice / GetItemStdPoint / GetItemProcPoint / GetItemKillPoint / GetItemGoldPoint` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Multiple cases access `Record[N].m_strCode[offset]` and `Record[N].m_dwIndex` depending on table code. |
| `WorldServerUtil.cpp` | `GetItemDurPoint` | `s_ptblItemData[nTableCode]` | `_base_fld *` | Accesses `Record[5].m_strCode`, `Record[9].m_strCode`, `Record[6].m_strCode[28]`, `Record[4].m_strCode[40]`, `Record[4].m_strCode[28]`, `Record[6].m_strCode[40]`, `Record[5].m_strCode[60]` (float), `Record[5].m_dwIndex`, `Record[5].m_strCode[44]`. |
| `CMapData.cpp` | `_LoadStoreDummy` | `CItemStoreManager::m_tblItemStore` | `_StoreList_fld *` | IDA compares `&Record[3].m_strCode[52]` with map code; interpreted as `_StoreList_fld::m_strStore_MAPcode`. |

> [!NOTE]
> Rule 9: `CRecordData::GetRecord` usually returns `_base_fld`. Re-casters should be careful with structure packing and offsets.
