# Static Data Log
Only unresolved static strings/arrays/data from IDA go here. Remove entries once recovered.

## Unknown string labels (bytes not recovered)
- aRpkAaiAo (CMergeFileManager::InitMergeFile when .rpk count > 255)
- aA_14 / aA_15 / aA_16 (CNuclearBombMgr::LoadIni error messages for zero times)

## Unknown data tables (bytes not recovered)
- UnLockDDS XOR tables (unk_1409788D4 / unk_1409788D8); current code uses old-source constants

## Unknown constants (values not recovered)
- CMoveMapLimitEnviromentValues::ELAN_1TH_LIMIT_NPC_RECORD_INDEX (used by RequestElanMapUserForceMoveHQ)
