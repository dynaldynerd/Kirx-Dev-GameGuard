# Static Data Log
Only unresolved static strings/arrays/data from IDA go here. Remove entries once recovered.

## Unknown string labels (bytes not recovered)
- aCmapoperationL_0 (CMapOperation::LoadMaps FindEmptyNPC error string)
- aRpkAaiAo (CMergeFileManager::InitMergeFile when .rpk count > 255)
- R3/CBsp/Material warnings:
  - aAiAaiai_1, aAai, aAiAaiai_4, aA_3, aAiAaiaiAiau, aAiAaiai, aAiAaiai_5, aAiAaiai_2, aAiAaiai_3
- R3T/texture warnings:
  - aR3tAaia_0, aR3tAaia, aAo_1, aAo_2, aAlphaSortTrue, aAi, aAiR3tAai, aAiAaiai_0, aA_0, aA_2, aAi_1
- Vertex/Index buffer error labels: byte_140884910, byte_140884930, aAiu, aAiao
- Error suffix label: asc_140884708
- R3T lightmap size error label: byte_140883FF8

## Unknown data tables (bytes not recovered)
- UnLockDDS XOR tables (unk_1409788D4 / unk_1409788D8); current code uses old-source constants
- Default paths from old source, not confirmed in IDA:
  - byte_184A790F0 ".\\Map\\Entity\\"
  - byte_184A79270 ".\\Snd\\"
  - aTexture ".\\Texture\\"
