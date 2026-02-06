# Static Data Log
Only unresolved static strings/arrays/data from IDA go here. Remove entries once recovered.

## Unknown string labels (bytes not recovered)
- aRpkAaiAo (CMergeFileManager::InitMergeFile when .rpk count > 255)
- aA_14 / aA_15 / aA_16 (CNuclearBombMgr::LoadIni error messages for zero times)
- aGetyposinleafu / byte_140883769 (CBsp::GetYposInLeafUseEdgeNormal Warning message; old source: "GetYposInLeafUseEdgeNormal()함수는 서버에서 쓸겁니다.", "")
- aPathfinderStac (CPathFinder::PopPathStack error message; current guess: "pathfinder stack error")
- pathfinder stack overflow (CPathFinder::PushPathStack error message)
- L"pMon->m_pRecordSet" (DfAIMgr::Atp_SearchStart_OnLoop / DfAIMgr::CheckEmotionBad _wassert expression)
- L"G:\\00_ZoneServer_Source\\03_Temp_Source\\2009_05_13_Source_Oversea\\zoneserver\\GameMain\\NewMonster\\NewMonsterAI_Df_Handler.cpp" (DfAIMgr::Atp_SearchStart_OnLoop / DfAIMgr::CheckEmotionBad _wassert file path)
- "Birth %s >> %s Map(%s) Pos(%d, %d, %d)" (CMonster::_BossBirthWriteLog)
- "Death %s >> %s Map(%s) Pos(%d, %d, %d)" (CMonster::_BossDieWriteLog_Start)
- " " (CMonster::_BossDieWriteLog_Start separator)
- "%s >> %s CreatTime: %d/%d %d:%d DeadTime: %d/%d %d:%d (Map:%s, Pos:%d %d %d)" (CMonster::_BossDieWriteLog_Start)
- "%d * %d = %.0f" (CMonster::_BossDieWriteLog_Start)
- "%d * %d * 1.2 = %.0f" (CMonster::_BossDieWriteLog_Start)
- "\t system guard tower : %s" (CMonster::_BossDieWriteLog_Start)
- "\t %s (%d) : %s" (CMonster::_BossDieWriteLog_Start)
- "\t disconnect!! (%d) : %s" (CMonster::_BossDieWriteLog_Start)
- "\t <<TAKER>> %s (%d) Sec: %d" (CMonster::_BossDieWriteLog_Start)
- "<<PARTY>> " (CMonster::_BossDieWriteLog_Start)
- " %d " (CMonster::_BossDieWriteLog_Start)
- "\t %s" (CMonster::_BossDieWriteLog_Start)
- "\t NOBODY LOOT" (CMonster::_BossDieWriteLog_Start)
- "\r\n" (CMonster::_BossDieWriteLog_End)
- "\t LootItem : %s" (CMonster::_LootItem_Std/_LootItem_Rwp)
- "Event Set Item >> %s, %d" (CMonster::_LootItem_EventSet)
- "Skill Attack Error : Skill(%s), SIndex(%d), l_fConst(%f), l_nLvConst(%d), nMastery(%d), nMaxAF(%d), nMinAF(%d)" (CMonsterAttack::_CalcMonSkillAttPnt)
- "FlashDamage Error AttackTarget Map : Attack Obj( %s : %s ) Dst Obj( %s : %s )" (CAttack::FlashDamageProc)

## Unknown data tables (bytes not recovered)
- UnLockDDS XOR tables (unk_1409788D4 / unk_1409788D8); current code uses old-source constants
- EmotionType table (EMOTYPE[] used by DfAIMgr::CheckEmotionBad; values not recovered, currently zeroed)
- DfAIMgr::Mon_SearchStart_OnLoop static NearMon[_NEAR_DATA x20]
- DfAIMgr::Mv_Go_OnLoop static fNextTarPos[3]
- DfAIMgr::SearchPatrollPath static vTargetPos_0[3]
- DfAIMgr::SearchCharacterPath static vTargetPos[3]
- DfAIMgr::ChangeTargetPos static NewTar[3] / fBeforeTar[3]
- Attack limit tables s_Mon_nLimitDist / s_Mon_nLimitRadius / s_Mon_nLimitAngle (used by CMonsterAttack; values not recovered, currently zeroed)
- Attack limit tables s_nLimitDist / s_nLimitRadius / s_nLimitAngle (used by CPlayerAttack; values not recovered, currently zeroed)
- Effect limit tables s_nLimitRadius_0 / s_nLimitAngle_0 / s_nLimitAngle_1 (used by CCharacter effect targeting; values not recovered, currently zeroed)

## Unknown constants (values not recovered)
- CMoveMapLimitEnviromentValues::ELAN_1TH_LIMIT_NPC_RECORD_INDEX (used by RequestElanMapUserForceMoveHQ)
- Attack scaling constants fR / fRLf / fRMf / fR_0 / fRLf_0 / fRMf_0 / fR_1 / fRLf_1 / fRMf_1 (used by CAttack/CMonsterAttack/CPlayerAttack; values not recovered, currently zeroed)
- "17" (CCharacter::InsertSFContEffect GetRecord key for effect code 3)
- "-1" (CCharacter::InsertSFContEffect compares effectRecord[12].m_strCode)
