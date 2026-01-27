# GetRecord Casting logic Findings

During the implementation of `CMapOperation::Init` and its sub-flows, the following `GetRecord` casting behaviors were observed in the IDA source:

| File | Class::Method | Table Used | Cast Type | Notes |
|---|---|---|---|---|
| `CMapOperation.cpp` | `LoadMaps` | `m_tblMapData` | `_map_fld *` | Direct cast to specific type. |
| `CWorldSchedule.cpp` | `Init` | `m_tblSch` | `_WorldSchedule_fld *` | Decompiled code shows access like `Record[1].m_dwIndex`. This implies `Record` is treated as an array of structs where the base type and specific fields might have known offsets. |

> [!NOTE]
> Rule 9: `CRecordData::GetRecord` usually returns `_base_fld`. Re-casters should be careful with structure packing and offsets.
