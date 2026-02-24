#include "pch.h"

#include "CMonster.h"
#include "ObjectCreateSetData.h"
#include "GlobalObjects.h"
#include "WorldServerUtil.h"
#include "DummyPosition.h"
#include "CRFMonsterAIMgr.h"
#include "CPlayer.h"
#include "CAnimus.h"
#include "CLogFile.h"
#include "CMonsterSkill.h"
#include "CMonsterAttack.h"
#include "CMonsterHelper.h"
#include "CMonsterEventRespawn.h"
#include "CMonsterEventSet.h"
#include "CItemLootTable.h"
#include "ItemLooting_fld.h"
#include "CItemBox.h"
#include "CPartyPlayer.h"
#include "CPlayerDB.h"
#include "CGameStatistics.h"
#include "TimeItem.h"
#include "StorageList.h"
#include "PCBANG_PRIMIUM_FAVOR.h"
#include "CNetworkEX.h"
#include "attack_force_result_zocl.h"
#include "attack_gen_result_zocl.h"
#include "attack_skill_result_zocl.h"
#include "CMapData.h"
#include "CGuardTower.h"
#include "CTrap.h"
#include "monster_fld.h"

#include <cstring>
#include <cmath>
#include <cstdlib>
#include <cstdio>
#include <ctime>
#include <mmsystem.h>

CMonster:: _monster_loot_index *CMonster::s_idxMonsterLoot;
unsigned int CMonster::s_dwSerialCnt;
int CMonster::s_nAllocNum;
int CMonster::s_nLiveNum;
CLogFile CMonster::s_logTrace_Boss_BirthAndDeath;
CLogFile CMonster::s_logTrace_Boss_Looting;
namespace
{
char s_monsterObjectName[256]{};
}

CMonster::CMonster()
{
  m_pTargetChar = nullptr;
  m_AggroMgr.OnlyOnceInit(this);
  m_MonHierarcy.OnlyOnceInit(this);
  m_SFContDamageTolerance.OnlyOnceInit(this);
  _InitSDM();
  ++s_nAllocNum;
}

CMonster::~CMonster()
{
  --s_nAllocNum;
  _DestroySDM();
}

void CMonster::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_EP.AllocEffParam();
  m_dwLastDestroyTime = 0;
  m_bDungeon = false;
  m_bOper = false;
  m_MonsterSkillPool.Init();
  m_bApparition = false;
  m_dwDestroyNextTime = static_cast<unsigned int>(-1);
  m_dwLastRecoverTime = static_cast<unsigned int>(-1);
  m_fCreatePos[0] = 0.0f;
  m_fCreatePos[1] = 0.0f;
  m_fCreatePos[2] = 0.0f;
  m_fLookAtPos[0] = 0.0f;
  m_fLookAtPos[1] = 0.0f;
  m_fLookAtPos[2] = 0.0f;
  m_bRobExp = false;
  m_bRewardExp = false;
  m_bStdItemLoot = false;
  m_pActiveRec = nullptr;
  m_pMonRec = nullptr;
  m_pDumPosition = nullptr;
  m_nHP = 0;
  m_byCreateDate[0] = 0;
  m_byCreateDate[1] = 0;
  m_byCreateDate[2] = 0;
  m_byCreateDate[3] = 0;
  m_LifeMax = 0;
  m_LifeCicle = static_cast<unsigned int>(-1);
  m_fStartLookAtPos[0] = 0.0f;
  m_fStartLookAtPos[1] = 0.0f;
  m_fStartLookAtPos[2] = 0.0f;
  m_bRotateMonster = false;
  m_pTargetChar = nullptr;
  for (int j = 0; j < 5; ++j)
  {
    m_DefPart[j] = 0;
  }
  m_nEventItemNum = 0;
  std::memset(m_eventItem, 0, sizeof(m_eventItem));
  m_pEventRespawn = nullptr;
  m_pEventSet = nullptr;
  _InitSDM_LootTBL();
}

char CMonster::Create(_monster_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
  {
    return 0;
  }

  m_pMonRec = reinterpret_cast<_monster_fld *>(pData->m_pRecordSet);
  memcpy_0(m_fCreatePos, pData->m_fStartPos, sizeof(m_fCreatePos));
  memcpy_0(m_fTarPos, pData->m_fStartPos, sizeof(m_fTarPos));
  memcpy_0(m_fLookAtPos, pData->m_fStartPos, sizeof(m_fLookAtPos));

  m_bRotateMonster = false;
  m_EmotionPresentationCheck.ReSet();

  m_bRobExp = pData->bRobExp;
  m_bRewardExp = pData->bRewardExp;
  m_bStdItemLoot = true;
  m_pEventRespawn = nullptr;
  m_nEventItemNum = 0;
  m_pActiveRec = nullptr;
  m_pDumPosition = nullptr;
  m_pEventSet = nullptr;

  if (pData->pActiveRec)
  {
    m_pActiveRec = pData->pActiveRec;
    m_pActiveRec->SetCurMonNum(1);
    bool rotate = false;
    if (m_pActiveRec->m_pBlk)
    {
      rotate = m_pActiveRec->m_pBlk->m_bRotate;
    }
    m_bRotateMonster = rotate;
  }

  if (pData->pDumPosition)
  {
    m_pDumPosition = pData->pDumPosition;
    if (m_bRotateMonster)
    {
      float dir[3];
      dir[0] = m_pDumPosition->m_fDirection[0] - m_pDumPosition->m_fCenterPos[0];
      dir[1] = m_pDumPosition->m_fDirection[1] - m_pDumPosition->m_fCenterPos[1];
      dir[2] = m_pDumPosition->m_fDirection[2] - m_pDumPosition->m_fCenterPos[2];
      Normalize(dir);
      const float visualField = GetVisualField();
      dir[0] *= visualField;
      dir[1] *= visualField;
      dir[2] *= visualField;
      float src[3];
      src[0] = m_fCurPos[0] + dir[0];
      src[1] = m_fCurPos[1] + dir[1];
      src[2] = m_fCurPos[2] + dir[2];
      memcpy_0(m_fStartLookAtPos, src, sizeof(m_fStartLookAtPos));
      UpdateLookAtPos(src);
    }
    m_pDumPosition->SetActiveMonNum(1);
  }
  else if (m_bRotateMonster)
  {
    memcpy_0(m_fStartLookAtPos, m_fCreatePos, sizeof(m_fStartLookAtPos));
    m_fLookAtPos[2] = m_fLookAtPos[2] - 10.0f;
    memcpy_0(m_fStartLookAtPos, m_fLookAtPos, sizeof(m_fStartLookAtPos));
    UpdateLookAtPos(m_fLookAtPos);
  }

  m_bDungeon = pData->bDungeon;
  m_dwObjSerial = GetNewMonSerial();

  int nUserNode = 16;
  if (m_pMonRec->m_bMonsterCondition == 1)
  {
    nUserNode = 64;
  }
  m_LootMgr.Init(static_cast<unsigned __int8>(nUserNode));
  m_AggroMgr.Init();

  const int grade = GetMonsterGrade();
  const float tolMax = g_MonsterSetInfoData.GetMaxToleranceProbMax(grade);
  m_SFContDamageTolerance.Init(tolMax);
  m_LuaSignalReActor.Init();

  m_nHP = static_cast<int>(m_pMonRec->m_fMaxHP);
  m_dwDestroyNextTime = static_cast<unsigned int>(-1);
  m_bApparition = false;
  m_dwLastRecoverTime = GetLoopTime();
  m_LifeMax = 60000 * (rand() % 3) + 600000;
  m_LifeCicle = GetLoopTime();

  if (m_pMonRec->m_bMonsterCondition == 1 && !pData->pParent)
  {
    m_byCreateDate[0] = GetCurrentMonth();
    m_byCreateDate[1] = GetCurrentDay();
    m_byCreateDate[2] = GetCurrentHour();
    m_byCreateDate[3] = GetCurrentMin();
    _BossBirthWriteLog();
  }

  SetDefPart(m_pMonRec);
  m_MonsterSkillPool.Set(this);
  CreateAI(0);
  m_bOper = true;
  SetMoveType(0);
  CheckMonsterStateData();
  SendMsg_Create();
  m_MonHierarcy.OnChildMonsterCreate(pData);
  ++CMonster::s_nLiveNum;
  return 1;
}

bool CMonster::Destroy(unsigned __int8 byDestroyCode, CPlayer *pAttObj)
{
  m_LuaSignalReActor.SetSignalAndAction(0);
  if (m_pMonRec->m_bMonsterCondition == 1)
  {
    _BossDieWriteLog_Start(byDestroyCode, pAttObj);
  }

  m_MonHierarcy.OnChildMonsterDestroy();
  m_dwLastDestroyTime = GetLoopTime();
  if (m_pTargetChar)
  {
    m_pTargetChar->RemoveSlot(this);
  }
  m_pTargetChar = nullptr;
  CheckEmotionPresentation();

  if (m_bMove)
  {
    const float moveSpeed = GetMoveSpeed();
    MoveBreak(moveSpeed);
  }

  SendMsg_Destroy(byDestroyCode);
  m_bOper = false;
  if (!byDestroyCode)
  {
    if (pAttObj)
    {
      if (pAttObj != &sPlayerDum)
      {
        CheckLootItem(pAttObj);
        if (m_pMonRec->m_bMonsterCondition == 1)
        {
          _BossDieWriteLog_End();
        }
      }
    }
  }

  if (m_pActiveRec)
  {
    m_pActiveRec->SetCurMonNum(-1);
    if (m_pDumPosition)
    {
      m_pDumPosition->SetActiveMonNum(-1);
    }
    m_pActiveRec = nullptr;
  }

  m_bDungeon = false;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  --CMonster::s_nLiveNum;
  m_LuaSignalReActor.Free();
  return CCharacter::Destroy();
}

void CMonster::Command_ChildMonDestroy(unsigned int dwAfterKillTerm)
{
  if (m_bLive)
  {
    m_dwDestroyNextTime = dwAfterKillTerm + GetLoopTime();
    m_MonHierarcy.SetParent(nullptr);
  }
}

void CMonster::_InitSDM()
{
  char buffer[144]{};
  if (!s_logTrace_Boss_Looting.m_bInit)
  {
    const unsigned int localTime = GetKorLocalTime();
    sprintf(buffer, "..\\ZoneServerLog\\ServiceLog\\MonsterLoot%d.log", localTime);
    s_logTrace_Boss_Looting.SetWriteLogFile(buffer, 1, 0, 0, 0);
  }

  if (!s_logTrace_Boss_BirthAndDeath.m_bInit)
  {
    const unsigned int localTime = GetKorLocalTime();
    sprintf(buffer, "..\\ZoneServerLog\\ServiceLog\\MonsterBirth%d.log", localTime);
    s_logTrace_Boss_BirthAndDeath.SetWriteLogFile(buffer, 1, 0, 1, 1);
  }

  if (!sPlayerDum.m_bLive)
  {
    sPlayerDum.m_dwObjSerial = static_cast<unsigned int>(-16);
    sPlayerDum.m_bLive = true;
  }
}

void CMonster::_DestroySDM()
{
  if (s_nAllocNum <= 0 && s_idxMonsterLoot)
  {
    operator delete[](s_idxMonsterLoot);
    s_idxMonsterLoot = nullptr;
  }
}

void CMonster::_InitSDM_LootTBL()
{
  if (s_idxMonsterLoot != nullptr)
  {
    return;
  }

  const int monsterRecordCount = static_cast<int>(g_Main.GetMonsterRecordNum());
  if (monsterRecordCount <= 0)
  {
    return;
  }

  s_idxMonsterLoot = new _monster_loot_index[monsterRecordCount];
  if (s_idxMonsterLoot == nullptr)
  {
    return;
  }

  for (int monsterIndex = 0; monsterIndex < monsterRecordCount; ++monsterIndex)
  {
    _base_fld *monsterRecord = g_Main.m_tblMonster.GetRecord(monsterIndex);
    if (monsterRecord == nullptr)
    {
      s_idxMonsterLoot[monsterIndex].nStartRecIndex = -1;
      s_idxMonsterLoot[monsterIndex].nEndRecIndex = -1;
      continue;
    }

    _base_fld *lootRecord = g_Main.m_tblItemLoot.m_tblLoot.GetRecord(monsterRecord->m_strCode);
    if (lootRecord == nullptr)
    {
      s_idxMonsterLoot[monsterIndex].nStartRecIndex = -1;
      s_idxMonsterLoot[monsterIndex].nEndRecIndex = -1;
      continue;
    }

    int currentLootIndex = static_cast<int>(lootRecord->m_dwIndex);
    s_idxMonsterLoot[monsterIndex].nStartRecIndex = currentLootIndex;
    s_idxMonsterLoot[monsterIndex].nEndRecIndex = currentLootIndex;

    while (true)
    {
      ++currentLootIndex;
      _base_fld *nextLootRecord = g_Main.m_tblItemLoot.m_tblLoot.GetRecord(currentLootIndex);
      if (nextLootRecord == nullptr || strncmp(nextLootRecord->m_strCode, monsterRecord->m_strCode, 5u) != 0)
      {
        break;
      }

      s_idxMonsterLoot[monsterIndex].nEndRecIndex = currentLootIndex;
    }
  }
}

float CMonster::GetYAngle()
{
  return m_fYAngle;
}

unsigned __int8 CMonster::GetYAngleByte()
{
  return static_cast<unsigned __int8>(static_cast<int>((255.0f * GetYAngle()) / 65535.0f));
}

bool CMonster::IsRoateMonster()
{
  return m_bRotateMonster;
}

float CMonster::GetVisualField()
{
  return m_pMonRec->m_fViewExt;
}

float CMonster::GetVisualAngle()
{
  return static_cast<float>(m_pMonRec->m_nViewAngle);
}

bool CMonster::IsViewArea(CCharacter *pTarget)
{
  if (!pTarget)
  {
    return false;
  }

  float pfDist[4]{};
  const float angle = GetVisualAngle();
  const float radius = GetVisualField();

  float viewDirection[3]{};
  viewDirection[0] = m_fLookAtPos[0] - m_fCurPos[0];
  viewDirection[1] = m_fLookAtPos[1] - m_fCurPos[1];
  viewDirection[2] = m_fLookAtPos[2] - m_fCurPos[2];
  Normalize(viewDirection);

  viewDirection[0] = 3.0f * viewDirection[0];
  viewDirection[1] = 3.0f * viewDirection[1];
  viewDirection[2] = 3.0f * viewDirection[2];

  float src[3]{};
  src[0] = m_fCurPos[0] - viewDirection[0];
  src[1] = m_fCurPos[1] - viewDirection[1];
  src[2] = m_fCurPos[2] - viewDirection[2];

  return CMonsterHelper::IsInSector(pTarget->m_fCurPos, src, m_fLookAtPos, angle, radius, pfDist);
}

bool CMonster::GetViewAngleCap(int nCapKind, int *nOutValue)
{
  if (m_pMonRec->m_nViewAngleCap != nCapKind && nCapKind != 4)
  {
    return false;
  }
  *nOutValue = m_pMonRec->m_nCapacityValue;
  return true;
}

__int64 CMonster::GetMob_AsistType()
{
  return static_cast<unsigned int>(m_pMonRec->m_nAsitType);
}

__int64 CMonster::GetMob_SubRace()
{
  return static_cast<unsigned int>(m_pMonRec->m_nMobRace);
}

unsigned int CMonster::GetAggroShortTime()
{
  if (m_pMonRec)
  {
    return static_cast<unsigned int>(1000 * m_pMonRec->m_nAPTime);
  }
  return 0;
}

unsigned int CMonster::GetAggroResetTime()
{
  if (m_pMonRec)
  {
    return static_cast<unsigned int>(1000 * m_pMonRec->m_nAPReset);
  }
  return 0;
}

float CMonster::GetBonusInAreaAggro()
{
  return 80.0f;
}

void CMonster::UpdateLookAtPos()
{
  if (m_fCurPos[0] != m_fOldPos[0] || m_fCurPos[2] != m_fOldPos[2])
  {
    float dir[3];
    dir[0] = m_fCurPos[0] - m_fOldPos[0];
    dir[1] = m_fCurPos[1] - m_fOldPos[1];
    dir[2] = m_fCurPos[2] - m_fOldPos[2];
    Normalize(dir);
    const float visualField = GetVisualField();
    dir[0] *= visualField;
    dir[1] *= visualField;
    dir[2] *= visualField;
    m_fLookAtPos[0] = m_fCurPos[0] + dir[0];
    m_fLookAtPos[1] = m_fCurPos[1] + dir[1];
    m_fLookAtPos[2] = m_fCurPos[2] + dir[2];
    m_fYAngle = ::GetYAngle(m_fCurPos, m_fLookAtPos);
  }
}

void CMonster::UpdateLookAtPos(float *vLookAt)
{
  float dir[3];
  dir[0] = vLookAt[0] - m_fCurPos[0];
  dir[1] = vLookAt[1] - m_fCurPos[1];
  dir[2] = vLookAt[2] - m_fCurPos[2];
  Normalize(dir);
  const float visualField = GetVisualField();
  dir[0] *= visualField;
  dir[1] *= visualField;
  dir[2] *= visualField;
  m_fLookAtPos[0] = m_fCurPos[0] + dir[0];
  m_fLookAtPos[1] = m_fCurPos[1] + dir[1];
  m_fLookAtPos[2] = m_fCurPos[2] + dir[2];
  m_fYAngle = ::GetYAngle(m_fCurPos, m_fLookAtPos);
}

unsigned int CMonster::GetNewMonSerial()
{
  return s_dwSerialCnt++;
}

__int64 CMonster::GetMonsterGrade()
{
  return static_cast<unsigned int>(m_pMonRec->m_nMobGrade);
}

unsigned __int8 CMonster::GetMoveType()
{
  return static_cast<unsigned __int8>(___u23.m_nCommonStateChunk & 1);
}

unsigned __int8 CMonster::GetCombatState()
{
  if (GetMoveType() == 1)
  {
    return 1;
  }
  return static_cast<unsigned __int8>((___u23.m_nCommonStateChunk >> 1) & 1);
}

unsigned __int8 CMonster::GetEmotionState()
{
  return static_cast<unsigned __int8>((___u23.m_nCommonStateChunk >> 2) & 7);
}

char CMonster::CheckMonsterStateData()
{
  m_MonsterStateData.___u0.m_wSendChunkData &= 0xFFC3u;
  const unsigned __int8 moveType = GetMoveType();
  m_MonsterStateData.___u0.m_wSendChunkData =
    (moveType & 1) | (m_MonsterStateData.___u0.m_wSendChunkData & 0xFFFE);
  const bool effState = m_EP.GetEff_State(7);
  m_MonsterStateData.___u0.m_wSendChunkData =
    (static_cast<unsigned __int16>(effState) << 1) | (m_MonsterStateData.___u0.m_wSendChunkData & 0xFFFD);
  const unsigned __int8 combatState = GetCombatState();
  m_MonsterStateData.___u0.m_wSendChunkData =
    (static_cast<unsigned __int16>(combatState & 1) << 6) | (m_MonsterStateData.___u0.m_wSendChunkData & 0xFFBF);
  if (!m_MonsterStateData.operator!=(&m_BeforeMonsterStateData))
  {
    return 0;
  }
  m_BeforeMonsterStateData.___u0.m_wSendChunkData = m_MonsterStateData.___u0.m_wSendChunkData;
  return 1;
}

float CMonster::GetMoveSpeed()
{
  if (m_EP.GetEff_State(7))
  {
    return m_pMonRec->m_fMovSpd;
  }
  if (GetMoveType())
  {
    return m_pMonRec->m_fWarMovSpd;
  }
  return m_pMonRec->m_fMovSpd;
}

void CMonster::SetMoveType(unsigned __int8 bMoveType)
{
  ___u23.m_nCommonStateChunk = (bMoveType & 1) | (___u23.m_nCommonStateChunk & 0xFFFFFFFE);
}

void CMonster::SetCombatState(unsigned __int8 byCombatState)
{
  ___u23.m_nCommonStateChunk =
    (static_cast<unsigned int>(byCombatState & 1) << 1) | (___u23.m_nCommonStateChunk & 0xFFFFFFFD);
}

void CMonster::SetEmotionState(unsigned __int8 byEmotionState)
{
  ___u23.m_nCommonStateChunk =
    (static_cast<unsigned int>(byEmotionState & 7) << 2) | (___u23.m_nCommonStateChunk & 0xFFFFFFE3);
}

unsigned __int16 CMonster::GetMonStateInfo()
{
  return m_MonsterStateData.GetStateChunk();
}

__int64 CMonster::GetAttackPart()
{
  const int randValue = rand() % 100;
  if (randValue <= 20)
  {
    return 4;
  }
  if (randValue <= 43)
  {
    return 0;
  }
  if (randValue <= 65)
  {
    return 1;
  }
  if (randValue <= 83)
  {
    return 2;
  }
  if (randValue > 100)
  {
    return -1;
  }
  return 3;
}

__int64 CMonster::GetAttackDP()
{
  return static_cast<unsigned int>(m_pMonRec->m_nAttack_DP);
}

float CMonster::GetAttackRange()
{
  return m_pMonRec->m_fAttExt;
}

__int64 CMonster::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
if (m_pMonRec == nullptr)
    return 0;

  if (pAttChar != nullptr && m_pMonRec->m_nShieldBlock == 1)
  {
    int blockPercent = m_pMonRec->m_nBlockPer;
    if (blockPercent > 100)
      blockPercent = 100;
    if (rand() % 100 < blockPercent)
      return static_cast<unsigned int>(-2);
  }

  float defValue = 0.0f;
  if (nAttactPart == -1)
    defValue = static_cast<float>(m_DefPart[rand() % 5]);
  else
    defValue = static_cast<float>(m_DefPart[nAttactPart]);

  return static_cast<unsigned int>(static_cast<int>(defValue * m_EP.GetEff_Rate(6)));
}

float CMonster::GetDefFacing(int nPart)
{
  return m_pMonRec->m_fDefFacing;
}

float CMonster::GetDefGap(int nPart)
{
  return m_pMonRec->m_fDefGap;
}

__int64 CMonster::GetDefSkill(bool bBackAttackDamage)
{
  float defSkill = m_pMonRec->m_fDefSklUnit;
  if (bBackAttackDamage)
  {
    int cap = -1;
    if (GetViewAngleCap(3, &cap))
    {
      if (cap > 100)
        cap = 100;
      if (cap < 0)
        cap = 0;
      defSkill = defSkill * static_cast<float>(100 - cap) / 100.0f;
    }
  }

  return static_cast<unsigned int>(static_cast<int>(defSkill));
}

__int64 CMonster::GetFireTol()
{
  float value = m_pMonRec->m_fFireTol + m_EP.GetEff_Plus(15);
  int finalValue = static_cast<int>(value * m_EP.GetEff_Rate(25));
  if (finalValue < -200)
    finalValue = -200;
  if (finalValue > 200)
    finalValue = 200;
  if (m_EP.GetEff_State(19) && finalValue > 0)
    finalValue = -finalValue;
  return static_cast<unsigned int>(finalValue);
}

__int64 CMonster::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CMonster::GetLevel()
{
  return static_cast<unsigned int>(static_cast<int>(m_pMonRec->m_fLevel));
}

__int64 CMonster::GetMaxHP()
{
  return static_cast<unsigned int>(static_cast<int>(m_pMonRec->m_fMaxHP));
}

char *CMonster::GetObjName()
{
  std::snprintf(
    s_monsterObjectName,
    sizeof(s_monsterObjectName),
    "[MONSTER] >> %s (pos: %s {%d, %d, %d})",
    m_pMonRec->m_strName,
    m_pCurMap->m_pMapSet->m_strCode,
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]));
  return s_monsterObjectName;
}

__int64 CMonster::GetObjRace()
{
  return 16;
}

__int64 CMonster::GetSoilTol()
{
  float value = m_pMonRec->m_fSoilTol + m_EP.GetEff_Plus(17);
  int finalValue = static_cast<int>(value * m_EP.GetEff_Rate(27));
  if (finalValue < -200)
    finalValue = -200;
  if (finalValue > 200)
    finalValue = 200;
  if (m_EP.GetEff_State(19) && finalValue > 0)
    finalValue = -finalValue;
  return static_cast<unsigned int>(finalValue);
}

__int64 CMonster::GetWaterTol()
{
  float value = m_pMonRec->m_fWaterTol + m_EP.GetEff_Plus(16);
  int finalValue = static_cast<int>(value * m_EP.GetEff_Rate(26));
  if (finalValue < -200)
    finalValue = -200;
  if (finalValue > 200)
    finalValue = 200;
  if (m_EP.GetEff_State(19) && finalValue > 0)
    finalValue = -finalValue;
  return static_cast<unsigned int>(finalValue);
}

float CMonster::GetWeaponAdjust()
{
  return m_pMonRec->m_fWeakPart;
}

__int64 CMonster::GetWeaponClass()
{
  return static_cast<unsigned int>(m_pMonRec->m_bAttRangeType);
}

float CMonster::GetWidth()
{
  return m_pMonRec->m_fWidth;
}

__int64 CMonster::GetWindTol()
{
  float value = m_pMonRec->m_fWindTol + m_EP.GetEff_Plus(18);
  int finalValue = static_cast<int>(value * m_EP.GetEff_Rate(28));
  if (finalValue < -200)
    finalValue = -200;
  if (finalValue > 200)
    finalValue = 200;
  if (m_EP.GetEff_State(19) && finalValue > 0)
    finalValue = -finalValue;
  return static_cast<unsigned int>(finalValue);
}

__int64 CMonster::GetCritical_Exception_Rate()
{
  return static_cast<unsigned int>(m_pMonRec->m_nCriticalTol);
}

__int64 CMonster::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
if (!IsValidPlayer())
  {
    return 0;
  }

  const float attSkillUnit = m_pMonRec->m_fAttSklUnit;
  const float targetLevel = static_cast<float>(m_pTargetChar->GetLevel());
  const float targetDefSkill = static_cast<float>(m_pTargetChar->GetDefSkill(bBackAttack));
  int attackProb = static_cast<int>(((attSkillUnit - (targetLevel + targetDefSkill)) / 4.0f) + 95.0f);
  attackProb -= static_cast<int>(m_pTargetChar->GetAvoidRate());
  if (m_pTargetChar->m_bMove)
  {
    attackProb = static_cast<int>(static_cast<float>(attackProb) * 0.5f);
  }
  if (attackProb >= 5)
  {
    if (attackProb > 95)
    {
      return 95;
    }
  }
  else
  {
    return 5;
  }
  return static_cast<unsigned int>(attackProb);
}

int CMonster::IsValidPlayer()
{
  if (!m_pTargetChar)
  {
    return 0;
  }
  if (!m_pTargetChar->m_bLive)
  {
    m_pTargetChar = nullptr;
    return 0;
  }
  if (m_pTargetChar->m_bCorpse)
  {
    m_pTargetChar = nullptr;
    return 0;
  }
  if (m_pTargetChar->m_pCurMap != m_pCurMap)
  {
    m_pTargetChar = nullptr;
    return 0;
  }
  if (m_pTargetChar->m_bMapLoading)
  {
    m_pTargetChar = nullptr;
    return 0;
  }

  if (!m_pTargetChar->IsBeAttackedAble(true))
  {
    m_pTargetChar = nullptr;
    return 0;
  }

  if (m_pTargetChar->m_ObjID.m_byID || !BYTE2(m_pTargetChar[1].m_fCurPos[2]))
  {
    if (m_pTargetChar->m_ObjID.m_byID == 3)
    {
      CAnimus *targetAnimus = static_cast<CAnimus *>(m_pTargetChar);
      if (targetAnimus != nullptr && targetAnimus->m_pMaster != nullptr && targetAnimus->m_pMaster->m_bInGuildBattle)
      {
        return 0;
      }
    }
    if (m_pTargetChar->GetStealth(true) || m_pTargetChar->m_bObserver)
    {
      m_pTargetChar = nullptr;
      return 0;
    }
    return 1;
  }
  return 0;
}

bool CMonster::IsAttackableInTown()
{
  return true;
}

bool CMonster::IsBeAttackedAble(bool bFirst)
{
if (!m_bLive || m_bObserver || m_bCorpse)
    return false;
  if (m_EP.GetEff_State(20))
    return false;
  return !m_EP.GetEff_State(28);
}

char CMonster::IsBeDamagedAble(CCharacter *pAtter)
{
  const unsigned __int8 attackerId = pAtter->m_ObjID.m_byID;
  if (!attackerId)
  {
    if (BYTE2(pAtter[1].m_fCurPos[2]))
    {
      return 0;
    }
    return m_pMonRec->m_nRaceCode == -1
        || (m_pMonRec->m_nRaceCode != 3 && pAtter->GetObjRace() != m_pMonRec->m_nRaceCode);
  }
  if (attackerId == 1)
  {
    return 0;
  }
  if (attackerId != 3)
  {
    return m_pMonRec->m_nRaceCode == -1
        || (m_pMonRec->m_nRaceCode != 3 && pAtter->GetObjRace() != m_pMonRec->m_nRaceCode);
  }
  CAnimus *attackerAnimus = static_cast<CAnimus *>(pAtter);
  return attackerAnimus->m_pMaster == nullptr || !attackerAnimus->m_pMaster->m_bInGuildBattle;
}

char CMonster::IsRecvableContEffect()
{
  return static_cast<char>(!m_EP.GetEff_State(20) && !m_EP.GetEff_State(28));
}

bool CMonster::IsRewardExp()
{
  return m_bRewardExp;
}

bool CMonster::SF_AllContHelpSkillRemove_Once(CCharacter *pDstObj)
{
  const float roll = static_cast<float>(rand() % 100);
  if (pDstObj->m_EP.GetEff_Plus(38) > roll)
  {
    return false;
  }

  int removedCount = 0;
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &pDstObj->m_SFCont[1][j];
    if (cont->m_bExist && !cont->m_byEffectCode)
    {
      pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(j), false, false);
      ++removedCount;
    }
  }
  return removedCount > 0;
}

bool CMonster::SF_AllContHelpForceRemove_Once(CCharacter *pDstObj)
{
  const float roll = static_cast<float>(rand() % 100);
  if (pDstObj->m_EP.GetEff_Plus(38) > roll)
  {
    return false;
  }

  int removedCount = 0;
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &pDstObj->m_SFCont[1][j];
    if (cont->m_bExist && cont->m_byEffectCode == 1)
    {
      pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(j), false, false);
      ++removedCount;
    }
  }
  return removedCount > 0;
}

bool CMonster::SF_HPInc_Once(CCharacter *pDstObj, float fEffectValue)
{
  bool effectable = false;
  if ((!pDstObj->m_ObjID.m_byID || pDstObj->m_ObjID.m_byID == 1) && !pDstObj->m_bCorpse)
  {
    effectable = true;
  }
  if (!effectable)
  {
    return false;
  }

  const int curHp = static_cast<int>(pDstObj->GetHP());
  const int maxHp = static_cast<int>(pDstObj->GetMaxHP());
  const float effectRate = pDstObj->m_EP.GetEff_Rate(18);
  const int addHp = static_cast<int>(static_cast<float>(maxHp) * (fEffectValue * effectRate));
  pDstObj->SetHP(addHp + curHp, false);

  if ((static_cast<float>(curHp) / static_cast<float>(maxHp)) > 0.80000001f)
  {
    return false;
  }
  pDstObj->SendMsg_SetHPInform();
  return true;
}

bool CMonster::SF_LateContDamageRemove_Once(CCharacter *pDstObj)
{
  int latestIndex = -1;
  _sf_continous *latestCont = nullptr;
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &pDstObj->m_SFCont[0][j];
    if (cont->m_bExist)
    {
      if (latestIndex == -1 || cont->m_dwStartSec > latestCont->m_dwStartSec)
      {
        latestIndex = j;
        latestCont = cont;
      }
    }
  }

  if (latestIndex == -1)
  {
    return false;
  }
  pDstObj->RemoveSFContEffect(0u, static_cast<unsigned __int16>(latestIndex), false, false);
  return true;
}

bool CMonster::SF_LateContHelpSkillRemove_Once(CCharacter *pDstObj)
{
  const float roll = static_cast<float>(rand() % 100);
  if (pDstObj->m_EP.GetEff_Plus(38) > roll)
  {
    return false;
  }

  int latestIndex = -1;
  _sf_continous *latestCont = nullptr;
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &pDstObj->m_SFCont[1][j];
    if (cont->m_bExist && !cont->m_byEffectCode)
    {
      if (latestIndex == -1 || cont->m_dwStartSec > latestCont->m_dwStartSec)
      {
        latestIndex = j;
        latestCont = cont;
      }
    }
  }

  if (latestIndex == -1)
  {
    return false;
  }
  pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(latestIndex), false, false);
  return true;
}

bool CMonster::SF_LateContHelpForceRemove_Once(CCharacter *pDstObj)
{
  const float roll = static_cast<float>(rand() % 100);
  if (pDstObj->m_EP.GetEff_Plus(38) > roll)
  {
    return false;
  }

  int latestIndex = -1;
  _sf_continous *latestCont = nullptr;
  for (int j = 0; j < 8; ++j)
  {
    _sf_continous *cont = &pDstObj->m_SFCont[1][j];
    if (cont->m_bExist && cont->m_byEffectCode == 1)
    {
      if (latestIndex == -1 || cont->m_dwStartSec > latestCont->m_dwStartSec)
      {
        latestIndex = j;
        latestCont = cont;
      }
    }
  }

  if (latestIndex == -1)
  {
    return false;
  }
  pDstObj->RemoveSFContEffect(1u, static_cast<unsigned __int16>(latestIndex), false, false);
  return true;
}

void CMonster::UpdateSFCont()
{
  if (!m_tmrSFCont.CountingTimer())
  {
    return;
  }

  const unsigned int now = _sf_continous::GetSFContCurTime();
  for (int contCode = 0; contCode < 2; ++contCode)
  {
    for (int slot = 0; slot < 8; ++slot)
    {
      _sf_continous *cont = &m_SFCont[contCode][slot];
      if (!cont->m_bExist)
      {
        continue;
      }

      const unsigned int elapsed = now - cont->m_dwStartSec;
      if (elapsed < cont->m_wDurSec)
      {
        // Base CGameObject handler is a no-op in IDA; monster path does not need per-tick notify.
      }
      else
      {
        RemoveSFContEffect(static_cast<unsigned __int8>(contCode), static_cast<unsigned __int16>(slot), false, false);
      }
    }
  }

  if (!m_bLastContEffectUpdate)
  {
    return;
  }

  unsigned int latestEffSerial = 0;
  _sf_continous *latestCont = nullptr;
  for (int contCode = 0; contCode < 2; ++contCode)
  {
    for (int slot = 0; slot < 8; ++slot)
    {
      _sf_continous *cont = &m_SFCont[contCode][slot];
      if (cont->m_bExist && latestEffSerial <= cont->m_dwEffSerial)
      {
        latestEffSerial = cont->m_dwEffSerial;
        latestCont = cont;
      }
    }
  }

  const unsigned __int16 oldLastContEffect = m_wLastContEffect;
  if (latestCont)
  {
    m_wLastContEffect = static_cast<unsigned __int16>(CalcEffectBit(latestCont->m_byEffectCode, latestCont->m_wEffectIndex));
  }
  else
  {
    m_wLastContEffect = static_cast<unsigned __int16>(-1);
  }

  if (oldLastContEffect != m_wLastContEffect)
  {
    SendMsg_LastEffectChangeInform();
  }
  m_bLastContEffectUpdate = false;
}

char CMonster::CheckRespawnProcess()
{
  const unsigned int loopTime = GetLoopTime();
  if (m_pActiveRec
      && !m_bDungeon
      && m_pMonRec->m_bMonsterCondition != 1
      && !m_MonHierarcy.GetParent()
      && !m_MonHierarcy.ChildKindCount())
  {
    const unsigned int lifeElapsed = loopTime - m_LifeCicle;
    if (lifeElapsed > m_LifeMax)
    {
      if (GetSqrt(m_fCurPos, m_fCreatePos) >= 100.0f || std::fabs(m_fCurPos[1] - m_fCreatePos[1]) >= 50.0f)
      {
        _monster_create_setdata data;
        memcpy_0(data.m_fStartPos, m_fCreatePos, sizeof(data.m_fStartPos));
        data.m_nLayerIndex = m_wMapLayerIndex;
        data.m_pMap = m_pCurMap;
        data.m_pRecordSet = m_pRecordSet;
        data.pActiveRec = m_pActiveRec;
        data.bDungeon = m_bDungeon;
        data.pDumPosition = m_pDumPosition;
        data.pParent = m_MonHierarcy.GetParent();
        data.bRobExp = m_bRobExp;

        Destroy(1u, nullptr);
        Create(&data);
        return 1;
      }

      m_LifeCicle = loopTime;
    }
  }

  return 0;
}

void CMonster::CheckMonsterRotate()
{
  if (m_bRotateMonster
      && m_fCreatePos[0] == m_fCurPos[0]
      && m_fCreatePos[2] == m_fCurPos[2]
      && (m_fStartLookAtPos[0] != m_fLookAtPos[0] || m_fStartLookAtPos[2] != m_fLookAtPos[2]))
  {
    UpdateLookAtPos(m_fStartLookAtPos);
    memcpy_0(m_fLookAtPos, m_fStartLookAtPos, sizeof(m_fLookAtPos));
    SendMsg_Change_MonsterRotate();
  }
}

void CMonster::CheckAutoRecoverHP()
{
  if (m_pMonRec->m_fHPRecDelay > 0.0f && m_pMonRec->m_fHPRecUnit > 0.0f && m_pMonRec->m_bMonsterCondition == 1)
  {
    const int hp = static_cast<int>(GetHP());
    const int maxHP = static_cast<int>(GetMaxHP());
    if (hp < maxHP
        && static_cast<float>(static_cast<int>(GetLoopTime() - m_dwLastRecoverTime))
             >= m_pMonRec->m_fHPRecDelay * 1000.0f)
    {
      m_dwLastRecoverTime = GetLoopTime();
      const float nextHP = static_cast<float>(GetHP()) + m_pMonRec->m_fHPRecUnit;
      SetHP(static_cast<int>(nextHP), false);
    }
  }

  AutoRecover();
}

char CMonster::CheckDelayDestroy()
{
  if (m_dwDestroyNextTime == static_cast<unsigned int>(-1) || GetLoopTime() <= m_dwDestroyNextTime)
  {
    return 0;
  }

  Destroy(1u, nullptr);
  m_dwDestroyNextTime = static_cast<unsigned int>(-1);
  return 1;
}

void CMonster::AutoRecover()
{
  const int hp = static_cast<int>(GetHP());
  int delta = 0;

  if (m_EP.GetEff_Plus(32) != 0.0f)
  {
    delta = static_cast<int>(static_cast<float>(delta) + m_EP.GetEff_Plus(32));
  }

  if (!delta)
  {
    return;
  }

  if (delta < 0)
  {
    const int hpLimit = static_cast<int>(GetMaxHP()) / 10;
    if (delta + hp <= hpLimit)
    {
      delta = 0;
    }
  }

  if (delta)
  {
    SetHP(delta + hp, false);
  }
}

CCharacter *CMonster::GetAttackTarget()
{
  return m_pTargetChar;
}

void CMonster::SetAttackTarget(CCharacter *p)
{
  if (m_pTargetChar && p && p != m_pTargetChar)
  {
    CheckEventEmotionPresentation(7u, p);
  }
  m_pTargetChar = p;
  SendMsg_Change_MonsterTarget(m_pTargetChar);
}

void CMonster::Loop()
{
  if (!m_bLive)
  {
    return;
  }

  UpdateSFCont();
  if (m_bLive && !CheckRespawnProcess() && m_bLive)
  {
    if (CheckMonsterStateData())
    {
      SendMsg_Change_MonsterState();
    }

    if (!m_EP.GetEff_State(20) && !m_EP.GetEff_State(28))
    {
      if (!m_bStun)
      {
        if (m_bMove)
        {
          if (m_EP.GetEff_State(6))
          {
            const float moveSpeed = GetMoveSpeed();
            Move(moveSpeed);
            MoveBreak(GetMoveSpeed());
            Stop();
            SendMsg_BreakStop();
          }
          else
          {
            Move(GetMoveSpeed());
            UpdateLookAtPos();
          }
        }
        else
        {
          CheckMonsterRotate();
        }
      }

      if (m_bLive)
      {
        m_AggroMgr.Process();
        m_SFContDamageTolerance.Update();
        if (m_bLive)
        {
          if (!m_bStun)
          {
            m_AI.OnProcess(GetLoopTime());
          }
          CheckEmotionPresentation();
          if (m_bLive)
          {
            CheckAutoRecoverHP();
            if (m_bLive)
            {
              m_MonHierarcy.OnChildRegenLoop();
              if (m_bLive)
              {
                CheckDelayDestroy();
              }
            }
          }
        }
      }
    }
  }
}

void CMonster::OutOfSec()
{
  Destroy(1u, nullptr);
}

bool CMonster::IsMovable()
{
  return m_pMonRec->m_fMovSpd > 0.0f || m_pMonRec->m_fWarMovSpd > 0.0f;
}

CPlayer *CMonster::SearchNearPlayer()
{
  return CMonsterHelper::SearchNearPlayer(this, 0);
}

__int64 CMonster::AttackObject(int nDamage, CGameObject *pOri)
{
m_LifeCicle = GetLoopTime();
  return 1;
}

void CMonster::ChangeApparition(bool bApparition, unsigned int dwAfterKillTerm)
{
  m_bApparition = bApparition;
  if (dwAfterKillTerm != static_cast<unsigned int>(-1))
  {
    m_dwDestroyNextTime = dwAfterKillTerm + GetLoopTime();
  }
}

void CMonster::SendMsg_Change_MonsterTarget(CCharacter *pChar)
{
  unsigned int msg[2];
  msg[0] = m_dwObjSerial;
  msg[1] = static_cast<unsigned int>(-1);
  if (pChar && !pChar->m_ObjID.m_byID)
  {
    msg[1] = pChar->m_dwObjSerial;
  }
  unsigned __int8 type[2];
  type[0] = 11;
  type[1] = static_cast<unsigned __int8>(-103);
  CircleReport(type, reinterpret_cast<char *>(msg), 8, false);
}

bool CMonster::CheckEventEmotionPresentation(unsigned __int8 byCheckType, CCharacter *pTarget)
{
  return m_EmotionPresentationCheck.CheckEmotionState(this, byCheckType, pTarget);
}

__int64 CMonster::GetOffensiveType()
{
  return static_cast<unsigned int>(m_pMonRec->m_nOffensiveType);
}

__int64 CMonster::GetHelpMeCase()
{
  return 0;
}

bool CMonster::IsPreAttackAbleMon()
{
  return m_pMonRec->m_fEmoType > 5.0f;
}

float CMonster::GeEmotionImpStdTime()
{
  return m_pMonRec->m_fEmoImpStdTime;
}

float CMonster::GetSkillDelayTime(CMonsterSkill *pSkill)
{
  if (!pSkill)
  {
    return 0.0f;
  }

  const float nextDelay = static_cast<float>(static_cast<int>(pSkill->GetNextActionDelayTime()));
  const int skillType = static_cast<int>(pSkill->GetType());
  if (skillType)
  {
    if (skillType == 1)
    {
      const float effPlus = m_EP.GetEff_Plus(13);
      return nextDelay + (effPlus * 2.0f);
    }
  }
  else
  {
    const float effPlus = m_EP.GetEff_Plus(12);
    return nextDelay + (effPlus * 2.0f);
  }
  return nextDelay;
}

int CMonster::AssistSF(CCharacter *pDst, CMonsterSkill *pskill)
{
  if (!pskill || !pDst)
  {
    return 0;
  }
  UpdateLookAtPos(pDst->m_fCurPos);
  const int useType = static_cast<int>(pskill->GetUseType());
  switch (useType)
  {
    case 1:
      return _AssistSF_Cont_Dmg(pDst, pskill);
    case 2:
      return _AssistSF_Cont_Support(pDst, pskill);
    case 3:
      return _AssistSF_Cont_Temp(pDst, pskill);
    default:
      break;
  }
  return 0;
}

int CMonster::_AssistSF_Cont_Dmg(CCharacter *pDst, CMonsterSkill *pskill)
{
  unsigned int result = 0;
  if (!pskill || !pDst)
  {
    return result;
  }

  const int skillType = static_cast<int>(pskill->GetType());
  if (skillType > 0)
  {
    if (skillType <= 2)
    {
      if (m_EP.GetEff_State(4))
      {
        return 0;
      }
      _skill_fld *skillFld = static_cast<_skill_fld *>(pskill->GetFld());
      if (skillFld)
      {
        unsigned __int8 errorCode[32];
        errorCode[0] = 0;
        bool upMty = false;
        const int skillLv = static_cast<int>(pskill->GetSFLv());
        const int effectCode = (pskill->GetType() == 1) ? 0 : 2;
        if (AssistSkill(pDst, effectCode, skillFld, skillLv, errorCode, &upMty))
        {
          CheckEventEmotionPresentation(6u, nullptr);
          SendMsg_Assist_Skill(static_cast<char>(errorCode[0]), effectCode, pDst, skillFld, static_cast<char>(skillLv));
          pskill->Use(GetLoopTime(), true);
          result = 1;
        }
        else
        {
          pskill->Use(GetLoopTime(), false);
          result = 0;
        }
      }
    }
    else if (skillType == 3)
    {
      if (m_EP.GetEff_State(4))
      {
        return 0;
      }
      _force_fld *forceFld = static_cast<_force_fld *>(pskill->GetFld());
      if (forceFld)
      {
        unsigned __int8 errorCode[32];
        errorCode[0] = 0;
        bool upMty = false;
        const int forceLv = static_cast<int>(pskill->GetSFLv());
        if (AssistForce(pDst, forceFld, forceLv, errorCode, &upMty))
        {
          CheckEventEmotionPresentation(6u, nullptr);
          SendMsg_Assist_Force(static_cast<char>(errorCode[0]), pDst, forceFld, static_cast<char>(forceLv));
          pskill->Use(GetLoopTime(), true);
          result = 1;
        }
        else
        {
          pskill->Use(GetLoopTime(), false);
          result = 0;
        }
      }
    }
  }

  if (m_bMove && result)
  {
    Stop();
  }
  return result;
}

int CMonster::_AssistSF_Cont_Support(CCharacter *pDst, CMonsterSkill *pskill)
{
  return _AssistSF_Cont_Dmg(pDst, pskill);
}

int CMonster::_AssistSF_Cont_Temp(CCharacter *pDst, CMonsterSkill *pskill)
{
  unsigned int result = 0;
  if (!pskill || !pDst)
  {
    return result;
  }

  const int skillType = static_cast<int>(pskill->GetType());
  if (skillType > 0)
  {
    if (skillType <= 2)
    {
      _skill_fld *skillFld = static_cast<_skill_fld *>(pskill->GetFld());
      if (skillFld)
      {
        unsigned __int8 errorCode[32];
        errorCode[0] = 0;
        bool upMty = false;
        const int skillLv = static_cast<int>(pskill->GetSFLv());
        const int effectCode = (pskill->GetType() == 1) ? 0 : 2;
        if (AssistSkill(pDst, effectCode, skillFld, skillLv, errorCode, &upMty))
        {
          SendMsg_Assist_Skill(static_cast<char>(errorCode[0]), effectCode, pDst, skillFld, static_cast<char>(skillLv));
          pskill->Use(GetLoopTime(), true);
          result = 1;
        }
        else
        {
          pskill->Use(GetLoopTime(), false);
          result = 0;
        }
      }
    }
    else if (skillType == 3)
    {
      _force_fld *forceFld = static_cast<_force_fld *>(pskill->GetFld());
      if (forceFld)
      {
        unsigned __int8 errorCode[32];
        errorCode[0] = 0;
        bool upMty = false;
        const int forceLv = static_cast<int>(pskill->GetSFLv());
        if (AssistForce(pDst, forceFld, forceLv, errorCode, &upMty))
        {
          SendMsg_Assist_Force(static_cast<char>(errorCode[0]), pDst, forceFld, static_cast<char>(forceLv));
          pskill->Use(GetLoopTime(), true);
          result = 1;
        }
        else
        {
          pskill->Use(GetLoopTime(), false);
          result = 0;
        }
      }
    }
  }

  if (m_bMove && result)
  {
    Stop();
  }
  return result;
}

__int64 CMonster::Attack(CCharacter *pDst, CMonsterSkill *pskill)
{
  unsigned int result = 0;
  if (!pskill)
  {
    return result;
  }
  if (pDst)
  {
    UpdateLookAtPos(pDst->m_fCurPos);
  }
  if (!pskill->IsAttackAble())
  {
    return result;
  }

  const int type = static_cast<int>(pskill->GetType());
  switch (type)
  {
    case 0:
    {
      _attack_param param;
      make_gen_attack_param(pDst, &param);
      CMonsterAttack attack(this);
      attack.AttackMonsterGen(&param, m_bApparition);
      SendMsg_Attack_Gen(&attack);
      for (int j = 0; j < attack.m_nDamagedObjNum; ++j)
      {
        const int level = static_cast<int>(GetLevel());
        CCharacter *target = attack.m_DamList[j].m_pChar;
        target->SetDamage(
          attack.m_DamList[j].m_nDamage,
          this,
          level,
          attack.m_bIsCrtAtt,
          -1,
          0,
          true);
      }
      result = 1;
      pskill->Use(GetLoopTime(), true);
      goto ATTACK_END;
    }
    case 1:
    {
      _attack_param param;
      if (!make_skill_attack_param(pDst, pskill, 0, &param))
      {
        return 0;
      }
      CMonsterAttack attack(this);
      attack.AttackMonsterSkill(&param);
      SendMsg_Attack_Skill(&attack);
      for (int k = 0; k < attack.m_nDamagedObjNum; ++k)
      {
        _base_fld *skillFld = pskill->GetFld();
        const int level = static_cast<int>(GetLevel());
        CCharacter *target = attack.m_DamList[k].m_pChar;
        target->SetDamage(
          attack.m_DamList[k].m_nDamage,
          this,
          level,
          attack.m_bIsCrtAtt,
          0,
          skillFld->m_dwIndex,
          false);
      }
      break;
    }
    case 2:
    {
      _attack_param param;
      if (!make_skill_attack_param(pDst, pskill, 2, &param))
      {
        return 0;
      }
      CMonsterAttack attack(this);
      attack.AttackMonsterSkill(&param);
      SendMsg_Attack_Skill(&attack);
      for (int m = 0; m < attack.m_nDamagedObjNum; ++m)
      {
        _base_fld *skillFld = pskill->GetFld();
        const int level = static_cast<int>(GetLevel());
        CCharacter *target = attack.m_DamList[m].m_pChar;
        target->SetDamage(
          attack.m_DamList[m].m_nDamage,
          this,
          level,
          attack.m_bIsCrtAtt,
          2,
          skillFld->m_dwIndex,
          false);
      }
      break;
    }
    case 3:
    {
      if (m_EP.GetEff_State(1))
      {
        return 0;
      }
      _attack_param param;
      make_force_attack_param(pDst, pskill, &param);
      CMonsterAttack attack(this);
      attack.AttackMonsterForce(&param);
      SendMsg_Attack_Force(&attack);
      for (int n = 0; n < attack.m_nDamagedObjNum; ++n)
      {
        const int level = static_cast<int>(GetLevel());
        CCharacter *target = attack.m_DamList[n].m_pChar;
        target->SetDamage(
          attack.m_DamList[n].m_nDamage,
          this,
          level,
          false,
          -1,
          0,
          true);
      }
      result = 1;
      pskill->Use(GetLoopTime(), true);
      goto ATTACK_END;
    }
    default:
      break;
  }

  pskill->Use(GetLoopTime(), true);

ATTACK_END:
  if (m_bMove && result)
  {
    Stop();
  }
  return result;
}

void CMonster::make_gen_attack_param(CCharacter *pDst, _attack_param *pAP)
{
  CMonsterSkill *monSkill = m_MonsterSkillPool.GetMonSkillKind(0);
  if (monSkill)
  {
    pAP->pDst = pDst;
    if (pDst)
    {
      pAP->nPart = static_cast<int>(pDst->GetAttackRandomPart());
    }
    else
    {
      pAP->nPart = static_cast<int>(GetAttackPart());
    }
    pAP->nClass = m_pMonRec->m_bAttRangeType;
    pAP->nTol = static_cast<int>(monSkill->GetElement());
    pAP->nMinAF = static_cast<int>(monSkill->GetMinDmg());
    pAP->nMaxAF = static_cast<int>(monSkill->GetMaxDmg());
    pAP->nMinSel = static_cast<int>(monSkill->GetMinProb());
    pAP->nMaxSel = static_cast<int>(monSkill->GetMaxProb());
  }
  else
  {
    pAP->pDst = pDst;
    if (pDst)
    {
      pAP->nPart = static_cast<int>(pDst->GetAttackRandomPart());
    }
    else
    {
      pAP->nPart = static_cast<int>(GetAttackPart());
    }
    pAP->nClass = m_pMonRec->m_bAttRangeType;
    pAP->nTol = -1;
    pAP->nMinAF = 0;
    pAP->nMaxAF = 500;
    pAP->nMinSel = 0;
    pAP->nMaxSel = 100;
  }

  pAP->bPassCount = (m_pMonRec->m_bMonsterCondition == 1);
  if (m_pMonRec->m_nAttType > 2)
  {
    pAP->nAttactType = 6;
    pAP->nExtentRange = 90;
  }

  if (pDst)
  {
    memcpy_0(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
  }
}

char CMonster::make_skill_attack_param(CCharacter *pDst, CMonsterSkill *pSkill, int nEffectType, _attack_param *pAP)
{
  if (!pSkill)
  {
    return 0;
  }
  const int skillType = static_cast<int>(pSkill->GetType());
  if (skillType != 1 && skillType != 2)
  {
    return 0;
  }

  pAP->pDst = pDst;
  if (pDst)
  {
    pAP->nPart = static_cast<int>(pDst->GetAttackRandomPart());
  }
  else
  {
    pAP->nPart = static_cast<int>(GetAttackRandomPart());
  }
  pAP->nClass = m_pMonRec->m_bAttRangeType;
  pAP->nTol = static_cast<int>(pSkill->GetElement());
  pAP->nMinAF = static_cast<int>(pSkill->GetMinDmg());
  pAP->nMaxAF = static_cast<int>(pSkill->GetMaxDmg());
  pAP->nMinSel = static_cast<int>(pSkill->GetMinProb());
  pAP->nMaxSel = static_cast<int>(pSkill->GetMaxProb());
  pAP->nExtentRange = 20;
  pAP->nShotNum = 1;
  pAP->nAddAttPnt = 0;
  pAP->pFld = pSkill->GetFld();

  if (nEffectType)
  {
    pAP->byEffectCode = 2;
    pAP->nLevel = 1;
    pAP->nMastery = 99;
  }
  else
  {
    pAP->byEffectCode = 0;
    pAP->nLevel = static_cast<int>(pSkill->GetSFLv());
    pAP->nLevel = static_cast<int>(static_cast<float>(pAP->nLevel) + m_EP.GetEff_Plus(19));
    if (pAP->nLevel > 7)
    {
      pAP->nLevel = 7;
    }
    pAP->nMastery = 99;
  }

  if (pDst)
  {
    memcpy_0(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
  }
  else
  {
    memcpy_0(pAP->fArea, m_fCurPos, sizeof(pAP->fArea));
  }
  pAP->nMaxAttackPnt = 0;
  return 1;
}

void CMonster::make_force_attack_param(CCharacter *pDst, CMonsterSkill *pSkill, _attack_param *pAP)
{
  if (pSkill && pSkill->GetType() == 3)
  {
    pAP->pDst = pDst;
    if (pDst)
    {
      pAP->nPart = static_cast<int>(pDst->GetAttackRandomPart());
    }
    else
    {
      pAP->nPart = static_cast<int>(GetAttackRandomPart());
    }
    pAP->nClass = m_pMonRec->m_bAttRangeType;
    pAP->nTol = static_cast<int>(pSkill->GetElement());
    pAP->nMinAF = static_cast<int>(pSkill->GetMinDmg());
    pAP->nMaxAF = static_cast<int>(pSkill->GetMaxDmg());
    pAP->nMinSel = static_cast<int>(pSkill->GetMinProb());
    pAP->nMaxSel = static_cast<int>(pSkill->GetMaxProb());
    pAP->pFld = pSkill->GetFld();
    pAP->byEffectCode = 1;
    pAP->nLevel = static_cast<int>(pSkill->GetSFLv());
    pAP->nMastery = 1;
    if (pDst)
    {
      memcpy_0(pAP->fArea, pDst->m_fCurPos, sizeof(pAP->fArea));
    }
    else
    {
      memcpy_0(pAP->fArea, m_fCurPos, sizeof(pAP->fArea));
    }
    pAP->nMaxAttackPnt = 0;
  }
}

void CMonster::SendMsg_Attack_Gen(CMonsterAttack *pAT)
{
  _attack_gen_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byAttackPart = static_cast<unsigned __int8>(pAT->m_pp->nPart);
  msg.bCritical = pAT->m_bIsCrtAtt;
  msg.wBulletIndex = static_cast<unsigned __int16>(-1);
  msg.byListNum = static_cast<unsigned __int8>(pAT->m_nDamagedObjNum);
  for (int j = 0; j < pAT->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAT->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAT->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAT->m_DamList[j].m_nDamage);
  }
  unsigned __int8 type[2];
  type[0] = 5;
  type[1] = 7;
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), 0);
}

void CMonster::SendMsg_Attack_Skill(CMonsterAttack *pAt)
{
  _attack_skill_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byEffectCode = pAt->m_pp->byEffectCode;
  msg.wSkillIndex = static_cast<unsigned __int16>(pAt->m_pp->pFld->m_dwIndex);
  msg.bySkillLv = static_cast<unsigned __int8>(pAt->m_pp->nLevel);
  msg.byAttackPart = static_cast<unsigned __int8>(pAt->m_pp->nPart);
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.wBulletIndex = static_cast<unsigned __int16>(-1);
  msg.zAttackPos[0] = static_cast<__int16>(pAt->m_pp->fArea[0]);
  msg.zAttackPos[1] = static_cast<__int16>(pAt->m_pp->fArea[2]);
  msg.bWPActive = pAt->m_bActiveSucc;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
    msg.DamList[j].bActive = 0;
    msg.DamList[j].wActiveDamage = 0;
  }
  unsigned __int8 type[2];
  type[0] = 5;
  type[1] = 8;
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), 1);
}

void CMonster::SendMsg_Attack_Force(CMonsterAttack *pAt)
{
  _attack_force_result_zocl msg;
  msg.byAtterID = m_ObjID.m_byID;
  msg.dwAtterSerial = m_dwObjSerial;
  msg.byForceIndex = static_cast<unsigned __int8>(pAt->m_pp->pFld->m_dwIndex);
  msg.byForceLv = static_cast<unsigned __int8>(pAt->m_pp->nLevel);
  msg.zAreaPos[0] = static_cast<__int16>(pAt->m_pp->fArea[0]);
  msg.zAreaPos[1] = static_cast<__int16>(pAt->m_pp->fArea[2]);
  msg.byAttackPart = static_cast<unsigned __int8>(pAt->m_pp->nPart);
  msg.bCritical = pAt->m_bIsCrtAtt;
  msg.byListNum = static_cast<unsigned __int8>(pAt->m_nDamagedObjNum);
  for (int j = 0; j < pAt->m_nDamagedObjNum; ++j)
  {
    msg.DamList[j].byDstID = pAt->m_DamList[j].m_pChar->m_ObjID.m_byID;
    msg.DamList[j].dwDstSerial = pAt->m_DamList[j].m_pChar->m_dwObjSerial;
    msg.DamList[j].wDamage = static_cast<unsigned __int16>(pAt->m_DamList[j].m_nDamage);
  }
  unsigned __int8 type[2];
  type[0] = 5;
  type[1] = 9;
  CircleReport(type, reinterpret_cast<char *>(&msg), static_cast<unsigned __int16>(msg.size()), 1);
}

void CMonster::SendMsg_Move()
{
  if (m_bMove && m_bOper)
  {
    _monster_move_zocl moveMsg{};
    moveMsg.dwSerial = m_dwObjSerial;
    FloatToShort(m_fCurPos, moveMsg.zCur, 3);
    moveMsg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
    moveMsg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));

    unsigned __int8 type[2]{};
    type[0] = 4;
    type[1] = 5;
    CircleReport(type, reinterpret_cast<char *>(&moveMsg), static_cast<unsigned __int16>(sizeof(moveMsg)), 0);
  }
}

void CMonster::SendMsg_Create()
{
  unsigned __int8 msg[17];
  *reinterpret_cast<unsigned __int16 *>(msg) = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  *reinterpret_cast<unsigned __int16 *>(msg + 2) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 4) = m_dwObjSerial;
  __int16 *pos = reinterpret_cast<__int16 *>(msg + 8);
  FloatToShort(m_fCurPos, pos, 3);
  msg[14] = GetYAngleByte();
  *reinterpret_cast<unsigned __int16 *>(msg + 15) = GetMonStateInfo();
  unsigned __int8 type[2];
  type[0] = 3;
  type[1] = 16;
  CircleReport(type, reinterpret_cast<char *>(msg), 17, 0);
}

void CMonster::SendMsg_Destroy(unsigned __int8 byDestroyCode)
{
  unsigned __int8 msg[7];
  *reinterpret_cast<unsigned __int16 *>(msg) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 2) = m_dwObjSerial;
  msg[6] = byDestroyCode;
  unsigned __int8 type[2];
  type[0] = 3;
  type[1] = 24;
  CircleReport(type, reinterpret_cast<char *>(msg), 7, 0);
}

void CMonster::SendMsg_Change_MonsterState()
{
  char payload[6]{};
  *reinterpret_cast<unsigned int *>(payload) = m_dwObjSerial;
  *reinterpret_cast<unsigned __int16 *>(payload + 4) = static_cast<unsigned __int16>(GetMonStateInfo());

  unsigned __int8 type[2] = {11, static_cast<unsigned __int8>(-105)};
  CircleReport(type, payload, 6, false);
}

void CMonster::SendMsg_Change_MonsterRotate()
{
  char payload[5]{};
  *reinterpret_cast<unsigned int *>(payload) = m_dwObjSerial;
  payload[4] = static_cast<char>(GetYAngleByte());

  unsigned __int8 type[2] = {11, static_cast<unsigned __int8>(-104)};
  CircleReport(type, payload, 5, false);
}

void CMonster::SendMsg_FixPosition(int n)
{

  _monster_fixpositon_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wLastEffectCode = m_wLastContEffect;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.bYAngle = GetYAngleByte();
  msg.wStateInfo = static_cast<unsigned __int16>(GetMonStateInfo());

  unsigned __int8 type[2] = {4, 11};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CMonster::SendMsg_RealMovePoint(int n)
{

  _monster_real_move_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  msg.wLastEffectCode = m_wLastContEffect;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.zTar[0] = static_cast<__int16>(static_cast<int>(m_fTarPos[0]));
  msg.zTar[1] = static_cast<__int16>(static_cast<int>(m_fTarPos[2]));
  msg.wStateInfo = static_cast<unsigned __int16>(GetMonStateInfo());

  unsigned __int8 type[2] = {4, 22};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

void CMonster::DisableStdItemLoot()
{
  m_bStdItemLoot = false;
}

void CMonster::LinkEventRespawn(_event_respawn *pEventRespawn)
{
  m_pEventRespawn = pEventRespawn;
}

void CMonster::LinkEventSet(_event_set *pEventSet)
{
  m_pEventSet = pEventSet;
}

void CMonster::SendMsg_Assist_Skill(
  char byErrCode,
  int nEffectCode,
  CCharacter *pDst,
  _skill_fld *pSkill_fld,
  char nSFLv)
{
  if (!pDst || !pSkill_fld)
  {
    return;
  }
  if (m_nCirclePlayerNum > 500 && pDst == this)
  {
    return;
  }
  if (byErrCode && byErrCode != 100)
  {
    return;
  }

  if (nEffectCode)
  {
    if (nEffectCode == 2)
    {
      unsigned __int8 msg[18];
      msg[0] = static_cast<unsigned __int8>(byErrCode);
      msg[1] = m_ObjID.m_byID;
      *reinterpret_cast<unsigned __int16 *>(msg + 2) = m_ObjID.m_wIndex;
      *reinterpret_cast<unsigned int *>(msg + 4) = m_dwObjSerial;
      msg[8] = pDst->m_ObjID.m_byID;
      *reinterpret_cast<unsigned __int16 *>(msg + 9) = pDst->m_ObjID.m_wIndex;
      *reinterpret_cast<unsigned int *>(msg + 11) = pDst->m_dwObjSerial;
      *reinterpret_cast<unsigned __int16 *>(msg + 15) = static_cast<unsigned __int16>(pSkill_fld->m_dwIndex);
      unsigned __int8 type[2];
      type[0] = 17;
      type[1] = 9;
      CircleReport(type, reinterpret_cast<char *>(msg), 18, 0);
    }
    return;
  }

  unsigned __int8 msg[18];
  msg[0] = static_cast<unsigned __int8>(byErrCode);
  msg[1] = m_ObjID.m_byID;
  *reinterpret_cast<unsigned __int16 *>(msg + 2) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 4) = m_dwObjSerial;
  msg[8] = pDst->m_ObjID.m_byID;
  *reinterpret_cast<unsigned __int16 *>(msg + 9) = pDst->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 11) = pDst->m_dwObjSerial;
  msg[15] = static_cast<unsigned __int8>(pSkill_fld->m_dwIndex);
  msg[16] = static_cast<unsigned __int8>(nSFLv);
  unsigned __int8 type[2];
  type[0] = 17;
  type[1] = 6;
  CircleReport(type, reinterpret_cast<char *>(msg), 18, 0);
}

void CMonster::SendMsg_Assist_Force(
  char byErrCode,
  CCharacter *pDst,
  _force_fld *pForc_fld,
  char nSFLv)
{
  if (!pDst || !pForc_fld)
  {
    return;
  }
  if (m_nCirclePlayerNum > 500 && pDst == this)
  {
    return;
  }
  if (byErrCode && byErrCode != 100)
  {
    return;
  }

  unsigned __int8 msg[18];
  msg[0] = static_cast<unsigned __int8>(byErrCode);
  msg[1] = static_cast<unsigned __int8>(pForc_fld->m_dwIndex);
  msg[2] = static_cast<unsigned __int8>(nSFLv);
  msg[3] = m_ObjID.m_byID;
  *reinterpret_cast<unsigned __int16 *>(msg + 4) = m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 6) = m_dwObjSerial;
  msg[10] = pDst->m_ObjID.m_byID;
  *reinterpret_cast<unsigned __int16 *>(msg + 11) = pDst->m_ObjID.m_wIndex;
  *reinterpret_cast<unsigned int *>(msg + 13) = pDst->m_dwObjSerial;
  unsigned __int8 type[2];
  type[0] = 17;
  type[1] = 3;
  CircleReport(type, reinterpret_cast<char *>(msg), 18, 0);
}

void CMonster::SendMsg_Emotion_Presentation(
  unsigned __int8 bylhw,
  unsigned __int16 wSubIndex,
  unsigned __int16 wRandIndex,
  unsigned int nSendTargetIndex)
{
  unsigned __int8 msg[9];
  *reinterpret_cast<unsigned int *>(msg) = m_dwObjSerial;
  msg[4] = bylhw;
  *reinterpret_cast<unsigned __int16 *>(msg + 5) = wSubIndex;
  *reinterpret_cast<unsigned __int16 *>(msg + 7) = wRandIndex;
  unsigned __int8 type[2];
  type[0] = 11;
  type[1] = static_cast<unsigned __int8>(-102);
  if (nSendTargetIndex == static_cast<unsigned int>(-1))
  {
    CircleReport(type, reinterpret_cast<char *>(msg), 9, 0);
  }
  else
  {
    g_Network.m_pProcess[0]->LoadSendMsg(
      static_cast<unsigned int>(nSendTargetIndex),
      type,
      reinterpret_cast<char *>(msg),
      9u);
  }
}

void CMonster::ClearEmotionPresentation()
{
  m_EmotionPresentationCheck.ReSet();
}

void CMonster::CheckEmotionPresentation()
{
  if (m_EmotionPresentationCheck.m_bIsSet)
  {
    int sendTargetIndex = -1;
    if (m_EmotionPresentationCheck.m_pTarget)
    {
      sendTargetIndex = m_EmotionPresentationCheck.m_pTarget->m_ObjID.m_wIndex;
    }
    SendMsg_Emotion_Presentation(
      m_EmotionPresentationCheck.m_byType,
      m_EmotionPresentationCheck.m_wIndex,
      m_EmotionPresentationCheck.m_wRandIndex,
      sendTargetIndex);
    m_EmotionPresentationCheck.ReSet();
  }
}


bool CMonster::IsBossMonster()
{
  return m_pMonRec && m_pMonRec->m_bMonsterCondition == 1;
}

void CMonster::_BossBirthWriteLog()
{
  if (m_pMonRec->m_bMonsterCondition == 1)
  {
    CMonster::s_logTrace_Boss_BirthAndDeath.Write(
      "Birth %s >> %s Map(%s) Pos(%d, %d, %d)",
      m_pMonRec->m_strCode,
      m_pMonRec->m_strName,
      m_pCurMap->m_pMapSet->m_strCode,
      static_cast<int>(m_fCurPos[0]),
      static_cast<int>(m_fCurPos[1]),
      static_cast<int>(m_fCurPos[2]));
  }
}

void CMonster::_BossDieWriteLog_End()
{
  if (m_pMonRec->m_bMonsterCondition == 1)
  {
    CMonster::s_logTrace_Boss_Looting.Write( "\r\n");
  }
}

__int64 CMonster::GetMyDMGSFContCount()
{
  unsigned int count = 0;
  for (int j = 0; j < 2; ++j)
  {
    for (int k = 0; k < 8; ++k)
    {
      _sf_continous *cont = &m_SFCont[j][k];
      if (cont->m_bExist)
      {
        ++count;
      }
    }
  }
  return count;
}

__int64 CMonster::GetMaxDMGSFContCount()
{
  int maxCount = m_pMonRec->m_nInjuryLimit;
  if (maxCount > 8)
  {
    maxCount = 8;
  }
  if (maxCount < 0)
  {
    return 0;
  }
  return static_cast<unsigned int>(maxCount);
}

unsigned __int16 _GetMonsterContTime(unsigned __int8 byEffectCode, unsigned __int8 byLv)
{
  return g_MonsterSetInfoData.GetLevelContSFTime(byEffectCode, byLv);
}

__int64 CMonster::CreateAI(int nType)
{
  CRFMonsterAIMgr *aiMgr = CRFMonsterAIMgr::Instance();
  if (aiMgr)
  {
    UsPoint<UsStateTBL> stateTable;
    aiMgr->GetStateTBL(&stateTable, nType);
    if (stateTable.operator UsStateTBL *())
    {
      UsStateTBL *state = stateTable.operator->();
      const unsigned int result = state->SetHFSM(&m_AI, this);
      return result;
    }
  }

  SetEmotionState(0);
  SetCombatState(0);
  return 0;
}

void CMonster::SetDefPart(_monster_fld *pRecordSet)
{
  const int weakPart = static_cast<int>(pRecordSet->m_fWeakPart + 1.0f);
  const float stdDef = pRecordSet->m_fStdDefFc;
  if (weakPart)
  {
    if (weakPart <= 6)
    {
      for (int j = 0; j < 5; ++j)
      {
        const float roll = static_cast<float>(rand() % 20) / 100.0f;
        m_DefPart[j] = static_cast<int>((stdDef * roll) + stdDef);
      }
      if (weakPart != 6)
      {
        m_DefPart[weakPart - 1] = static_cast<int>(stdDef);
      }
    }
  }
  else
  {
    for (int k = 0; k < 5; ++k)
    {
      m_DefPart[k] = static_cast<int>(stdDef);
    }
  }
}

void CMonster::CheckLootItem(CPlayer *pOwner)
{
  if (g_Main.m_pTimeLimitMgr->GetPlayerStatus( pOwner->m_id.wIndex) == 99)
  {
    pOwner->SendMsg_TLStatusPenalty( 4u);
  }
  else
  {
    bool looted = false;
    if (_LootItem_Std(pOwner))
    {
      looted = true;
    }
    if (_LootItem_Rwp(pOwner))
    {
      looted = true;
    }
    if (_LootItem_EventSet(pOwner))
    {
      looted = true;
    }
    if (_LootItem_Qst(pOwner))
    {
      looted = true;
    }

    if (looted)
    {
      if (pOwner->m_pPartyMgr->IsPartyMode())
      {
        pOwner->m_pPartyMgr->SetNextLootAuthor();
      }
    }
  }
}

char CMonster::AddEventItem(_event_loot_item *pItem)
{
  if (m_nEventItemNum >= 16)
  {
    return 0;
  }
  memcpy_0(&m_eventItem[m_nEventItemNum], pItem, sizeof(m_eventItem[m_nEventItemNum]));
  ++m_nEventItemNum;
  return 1;
}

char CMonster::_LootItem_Rwp(CPlayer *pOwner)
{
  if (!m_pEventRespawn)
  {
    return 0;
  }

  bool noAuthor = false;
  CPlayer *lootOwner = pOwner;
  unsigned int partyBossSerial = static_cast<unsigned int>(-1);
  if (pOwner->m_pPartyMgr->IsPartyMode())
  {
    partyBossSerial = pOwner->m_pPartyMgr->m_pPartyBoss->m_id.dwSerial;
    lootOwner = pOwner->m_pPartyMgr->GetLootAuthor();
    if (!lootOwner)
    {
      noAuthor = true;
    }
  }

  char looted = 0;
  for (int j = 0; j < m_pEventRespawn->nUseRewardItemNum; ++j)
  {
    _event_respawn::_reward_item *reward = &m_pEventRespawn->RewardItem[j];
    if ((!reward->pDstMonFld || reward->pDstMonFld == m_pMonRec) && reward->nProb > rand() % 100)
    {
      _STORAGE_LIST::_db_con *item = MakeLoot(reward->byItemTableCode, reward->pItemFld->m_dwIndex);
      if (item)
      {
        if (CreateItemBox(
              item,
              lootOwner,
              partyBossSerial,
              noAuthor,
              this,
              0,
              m_pCurMap,
              m_wMapLayerIndex,
              m_fCurPos,
              0))
        {
          looted = 1;
          if (m_pMonRec->m_bMonsterCondition == 1)
          {
            CMonster::s_logTrace_Boss_Looting.Write( "\t LootItem : %s", reward->pItemFld->m_strCode);
          }
          looted = 1;
        }
      }
    }
  }

  return looted;
}

char CMonster::_LootItem_EventSet(CPlayer *pOwner)
{
  char looted = 0;
  if (!m_pEventSet)
  {
    return looted;
  }

  _event_set_looting *eventLoot = g_MonsterEventSet.GetEvenSetLooting(m_pMonRec->m_strCode);
  if (eventLoot)
  {
    for (int j = 0; j < eventLoot->nItemCount; ++j)
    {
      char *itemCode = eventLoot->stEventItemList[j].strCode;
      const int prob = eventLoot->stEventItemList[j].byProb;
      if (prob > rand() % 100)
      {
        const unsigned __int8 itemTableCode = GetItemTableCode(itemCode);
        _base_fld *record = g_Main.m_tblItemData[itemTableCode].GetRecord(itemCode);
        const int itemCount = eventLoot->wMagnifications * *(reinterpret_cast<unsigned __int16 *>(itemCode) + 32);
        for (int k = 0; k < itemCount; ++k)
        {
          unsigned int durPoint = 0;
          if (IsOverLapItem(itemTableCode))
          {
            durPoint = *(reinterpret_cast<unsigned __int16 *>(itemCode) + 33);
          }
          else
          {
            durPoint = GetItemDurPoint(itemTableCode, record->m_dwIndex);
          }

          const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(itemTableCode, record->m_dwIndex);
          unsigned __int8 socketNum = 0;
          if (defSocketNum)
          {
            socketNum = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
          }
          const unsigned int bitSocket = GetBitAfterSetLimSocket(socketNum);

          _STORAGE_LIST::_db_con item;
          item.m_byTableCode = itemTableCode;
          item.m_wItemIndex = record->m_dwIndex;
          item.m_dwDur = durPoint;
          item.m_dwLv = bitSocket;

          float randPos[3]{};
          CMapData *map = m_pCurMap;
          if (map->GetRandPosVirtualDumExcludeStdRange( m_fCurPos, eventLoot->wRange, 0, randPos))
          {
            if (CreateItemBox(
                  &item,
                  7u,
                  map,
                  m_wMapLayerIndex,
                  randPos,
                  0,
                  pOwner,
                  eventLoot->bWithHolyScanner,
                  eventLoot->byLootAuth))
            {
              looted = 1;
              g_Main.m_logEvent.Write( "Event Set Item >> %s, %d", itemCode, static_cast<int>(item.m_dwDur));
            }
          }
        }
      }
    }
  }

  return looted;
}

char CMonster::_LootItem_Qst(CPlayer *pOwner)
{
  char looted = 0;
  for (int j = 0; j < m_nEventItemNum; ++j)
  {
    _event_loot_item *loot = &m_eventItem[j];
    unsigned int durPoint = GetItemDurPoint(loot->byItemTable, m_eventItem[j].wItemIndex);
    unsigned int durValue = 0xFFFFFFF;
    const unsigned __int8 itemKind = GetItemKindCode(loot->byItemTable);
    if (itemKind)
    {
      if (itemKind != 1)
      {
        continue;
      }
      durValue = GetMaxParamFromExp(loot->wItemIndex, durPoint);
    }
    else
    {
      const unsigned __int8 defSocketNum = GetDefItemUpgSocketNum(loot->byItemTable, loot->wItemIndex);
      unsigned __int8 socketNum = 0;
      if (defSocketNum)
      {
        socketNum = static_cast<unsigned __int8>(rand() % defSocketNum + 1);
      }
      durValue = GetBitAfterSetLimSocket(socketNum);
    }

    _STORAGE_LIST::_db_con item;
    item.m_byTableCode = loot->byItemTable;
    item.m_wItemIndex = loot->wItemIndex;
    item.m_dwDur = durPoint;
    item.m_dwLv = durValue;

    const _TimeItem_fld *timeRec = TimeItem::FindTimeRec(loot->byItemTable, loot->wItemIndex);
    if (timeRec && timeRec->m_nCheckType)
    {
      __time32_t now[9]{};
      _time32(now);
      item.m_byCsMethod = static_cast<unsigned __int8>(timeRec->m_nCheckType);
      item.m_dwT = timeRec->m_nUseTime + now[0];
      item.m_dwLendRegdTime = now[0];
    }

    if (CreateItemBox(&item, 5u, m_pCurMap, m_wMapLayerIndex, m_fCurPos, 0, nullptr, 0, 3u))
    {
      looted = 1;
    }
  }
return looted;
}

char CMonster::_LootItem_Std(CPlayer *pOwner)
{
  if (!m_bStdItemLoot)
  {
    return 0;
  }

  const _monster_loot_index lootIndex = CMonster::s_idxMonsterLoot[m_pRecordSet->m_dwIndex];
  if (lootIndex.nStartRecIndex == -1)
  {
    return 0;
  }

  const bool isLootFree = pOwner->m_bLootFree;
  const double playerPenalty = g_Main.m_pTimeLimitMgr->GetPlayerPenalty(pOwner->m_id.wIndex);

  bool noAuthor = false;
  CPlayer *lootOwner = pOwner;
  unsigned int partyBossSerial = static_cast<unsigned int>(-1);
  if (pOwner->m_pPartyMgr->IsPartyMode())
  {
    partyBossSerial = pOwner->m_pPartyMgr->m_pPartyBoss->m_id.dwSerial;
    lootOwner = pOwner->m_pPartyMgr->GetLootAuthor();
    if (!lootOwner)
    {
      noAuthor = true;
      lootOwner = pOwner;
    }
  }

  char looted = 0;
  for (int n = lootIndex.nStartRecIndex; n <= lootIndex.nEndRecIndex; ++n)
  {
    _ItemLooting_fld *lootRecord =
      reinterpret_cast<_ItemLooting_fld *>(g_Main.m_tblItemLoot.m_tblLoot.GetRecord(n));
    if (!lootRecord)
    {
      continue;
    }

    float dropRate = 0.0f;
    if (!pOwner->IsApplyPcbangPrimium() || IsBossMonster())
    {
      dropRate = ITEM_ROOT_RATE;
    }
    else
    {
      dropRate = PCBANG_PRIMIUM_FAVOR::ITEM_DROP;
    }

    if (!IsBossMonster())
    {
      if (pOwner->IsRidingUnit())
      {
        pOwner->m_EP.SetLock(0);
      }
      dropRate = dropRate + (pOwner->m_EP.GetEff_Have( 8) - 1.0f);
      dropRate = dropRate + (pOwner->m_EP.GetEff_Rate( 36) - 1.0f);
      dropRate = dropRate + (pOwner->m_EP.GetEff_Have( 73) - 1.0f);
      if (pOwner->IsApplyPcbangPrimium())
      {
        if (pOwner->m_EP.GetEff_Have( 74) > PCBANG_PRIMIUM_FAVOR::ITEM_DROP)
        {
          dropRate = dropRate + (pOwner->m_EP.GetEff_Have( 74) - PCBANG_PRIMIUM_FAVOR::ITEM_DROP);
        }
        if (pOwner->m_EP.GetEff_Have( 75) > PCBANG_PRIMIUM_FAVOR::ITEM_DROP)
        {
          dropRate = dropRate + (pOwner->m_EP.GetEff_Have( 75) - PCBANG_PRIMIUM_FAVOR::ITEM_DROP);
        }
      }
      else
      {
        dropRate = dropRate + (pOwner->m_EP.GetEff_Have( 74) - 1.0f);
        dropRate = dropRate + (pOwner->m_EP.GetEff_Have( 75) - 1.0f);
      }
      if (pOwner->IsRidingUnit())
      {
        pOwner->m_EP.SetLock(1);
      }
    }

    int operationCount = static_cast<int>((static_cast<float>(lootRecord->m_nOperationCount) * dropRate) * playerPenalty);
    const int rounding =
      static_cast<int>((static_cast<float>(lootRecord->m_nOperationCount) * dropRate) * playerPenalty * 10.0f) % 10;
    if (rounding >= 5)
    {
      ++operationCount;
    }

    for (int j = 0; j < operationCount; ++j)
    {
      unsigned int randValue = 0;
      if (!isLootFree)
      {
        const int high = rand();
        const int low = rand();
        randValue = static_cast<unsigned int>(low + (high << 16));
      }

      _monster_fld *monsterRecord = nullptr;
      if (m_pRecordSet)
      {
        monsterRecord = reinterpret_cast<_monster_fld *>(m_pRecordSet);
        int diffLevel = 11;
        if (pOwner)
        {
          if (pOwner->IsRidingUnit())
          {
            pOwner->m_EP.SetLock(0);
          }
          if (GetLevel() <= pOwner->GetLevel())
          {
            const float diff = static_cast<float>(GetLevel() - pOwner->GetLevel())
                             + pOwner->m_EP.GetEff_Have( 52);
            diffLevel = static_cast<int>(diff);
          }
          else
          {
            const float diff = static_cast<float>(GetLevel())
                             - pOwner->m_EP.GetEff_Have( 52)
                             - static_cast<float>(pOwner->GetLevel());
            diffLevel = static_cast<int>(diff);
          }
          if (pOwner->IsRidingUnit())
          {
            pOwner->m_EP.SetLock(1);
          }
        }

        unsigned int lootRate = static_cast<unsigned int>(lootRecord->m_nLootRate);
        if (monsterRecord->m_nUpLooting != -1 && monsterRecord->m_nUpLooting != 0)
        {
          diffLevel -= monsterRecord->m_nUpLooting;
          unsigned int monsterDropRate = g_MonsterSetInfoData.GetMonsterDropRate(diffLevel);
          if (!monsterDropRate)
          {
            monsterDropRate = 1;
          }
          lootRate = monsterDropRate * (lootRate / 0x64);
        }

        if (isLootFree || randValue < lootRate)
        {
          bool found = false;
          CItemLootTable::_linker_code *link = nullptr;
          for (int k = 0; k < 10; ++k)
          {
            if (lootRecord->m_nLootListCount > 0)
            {
              const int pick = rand() % lootRecord->m_nLootListCount;
              link = &g_Main.m_tblItemLoot.m_ppLinkCode[lootRecord->m_dwIndex][pick];
              if (link->bExist)
              {
                found = true;
                break;
              }
            }
          }

          if (found)
          {
            _STORAGE_LIST::_db_con *item = MakeLoot(link->byTableCode, link->wItemIndex);
            if (item)
            {
              if (CreateItemBox(
                    item,
                    lootOwner,
                    partyBossSerial,
                    noAuthor,
                    this,
                    0,
                    m_pCurMap,
                    m_wMapLayerIndex,
                    m_fCurPos,
                    0))
              {
                looted = 1;
                if (m_pMonRec->m_bMonsterCondition == 1)
                {
                  _base_fld *itemRec = g_Main.m_tblItemData[link->byTableCode].GetRecord(link->wItemIndex);
                  CMonster::s_logTrace_Boss_Looting.Write( "\t LootItem : %s", itemRec->m_strCode);
                }

                bool statOK = true;
                if (g_Main.IsReleaseServiceMode() && pOwner->m_byUserDgr)
                {
                  statOK = false;
                }
                if (statOK && link->byTableCode < 8u)
                {
                  CGameStatistics::_DAY *day = g_GameStatistics.CurWriteData();
                  if (GetItemGrade(link->byTableCode, link->wItemIndex))
                  {
                    ++day->dwDropRareItem_Evt;
                  }
                  else
                  {
                    ++day->dwDropStdItem_Evt;
                  }
                }
              }
            }
          }
        }
      }
    }
  }

  return looted;
}

void CMonster::_BossDieWriteLog_Start(unsigned __int8 byDestroyCode, CPlayer *pAttObj)
{
  if (m_pMonRec->m_bMonsterCondition != 1)
  {
    return;
  }

    CMonster::s_logTrace_Boss_BirthAndDeath.Write(
      "Death %s >> %s Map(%s) Pos(%d, %d, %d)",
      m_pMonRec->m_strCode,
      m_pMonRec->m_strName,
      m_pCurMap->m_pMapSet->m_strCode,
      static_cast<int>(m_fCurPos[0]),
      static_cast<int>(m_fCurPos[1]),
      static_cast<int>(m_fCurPos[2]));

  if (byDestroyCode)
  {
    return;
  }

  CMonster::s_logTrace_Boss_Looting.Write( " ");

  const int posX = static_cast<int>(m_fCurPos[0]);
  const int posY = static_cast<int>(m_fCurPos[1]);
  const int posZ = static_cast<int>(m_fCurPos[2]);
  const char *mapCode = m_pCurMap->m_pMapSet->m_strCode;
  const int curMin = GetCurrentMin();
  const int curHour = GetCurrentHour();
  const int curDay = GetCurrentDay();
  const int curMonth = GetCurrentMonth();

    CMonster::s_logTrace_Boss_Looting.Write(
      "%s >> %s CreatTime: %d/%d %d:%d DeadTime: %d/%d %d:%d (Map:%s, Pos:%d %d %d)",
      m_pMonRec->m_strCode,
      m_pMonRec->m_strName,
      m_byCreateDate[0],
      m_byCreateDate[1],
      m_byCreateDate[2],
      m_byCreateDate[3],
      curMonth,
      curDay,
      curHour,
      curMin,
      mapCode,
      posX,
      posY,
      posZ);

  CMonster::s_logTrace_Boss_Looting.Write( " ");

  if (pAttObj)
  {
    for (int j = 0; j < m_LootMgr.m_byUserNode; ++j)
    {
      CLootingMgr::_list *entry = &m_LootMgr.m_AtterList[j];
      if (entry->pAtter)
      {
        double damageValue = static_cast<double>(entry->dwDamage * entry->dwAttCount);
        char buffer[136] = {};
        std::sprintf(buffer, "%d * %d = %.0f", entry->dwAttCount, entry->dwDamage, damageValue);
        if (!j && m_LootMgr.m_bFirst)
        {
          damageValue = static_cast<double>(entry->dwDamage * entry->dwAttCount) * 1.2;
          std::sprintf(buffer, "%d * %d * 1.2 = %.0f", entry->dwAttCount, entry->dwDamage, damageValue);
        }

        if (entry->pAtter == &sPlayerDum)
        {
          CMonster::s_logTrace_Boss_Looting.Write( "\t system guard tower : %s", buffer);
        }
        else if (entry->pAtter->m_bLive && entry->pAtter->m_dwObjSerial == entry->dwAtterSerial)
        {
          const char *name = entry->pAtter->m_Param.GetCharNameA();
          CMonster::s_logTrace_Boss_Looting.Write( "\t %s (%d) : %s", name, entry->dwAtterSerial, buffer);
        }
        else
        {
          CMonster::s_logTrace_Boss_Looting.Write( "\t disconnect!! (%d) : %s", entry->dwAtterSerial, buffer);
        }
      }
    }

    CMonster::s_logTrace_Boss_Looting.Write( " ");

    if (pAttObj != &sPlayerDum)
    {
      const DWORD sec = timeGetTime() / 1000;
      const char *name = pAttObj->m_Param.GetCharNameA();
        CMonster::s_logTrace_Boss_Looting.Write(
          "\t <<TAKER>> %s (%d) Sec: %d",
          name,
          pAttObj->m_dwObjSerial,
          sec);

      if (pAttObj->m_pPartyMgr->IsPartyMode())
      {
        CPartyPlayer **members = pAttObj->m_pPartyMgr->GetPtrPartyMember();
        if (members)
        {
          char destination[1284] = {};
          std::sprintf(destination, "<<PARTY>> ");
          for (int k = 0; k < 8 && members[k]; ++k)
          {
            CPlayer *member = &g_Player[members[k]->m_wZoneIndex];
            char source[56] = {};
            std::sprintf(source, " %d ", member->m_dwObjSerial);
            strcat_0(destination, source);
          }
          CMonster::s_logTrace_Boss_Looting.Write( "\t %s", destination);
        }
      }
    }
  }
  else
  {
    CMonster::s_logTrace_Boss_Looting.Write( "\t NOBODY LOOT");
  }

  CMonster::s_logTrace_Boss_Looting.Write( " ");
}

unsigned __int8 CMonster::InsertSFContEffect(
  unsigned __int8 byContCode,
  unsigned __int8 byEffectCode,
  unsigned int dwEffectIndex,
  unsigned __int16 wDurSec,
  unsigned __int8 byLv,
  bool *pbUpMty,
  CPlayer *pActChar)
{
  (void)pActChar;
  *pbUpMty = true;

  const int myCount = static_cast<int>(GetMyDMGSFContCount());
  const int maxCount = static_cast<int>(GetMaxDMGSFContCount());
  if (myCount >= maxCount)
  {
    return 7;
  }
  if (byContCode >= 2)
  {
    return 13;
  }

  if (!m_SFContDamageTolerance.IsSFContDamage() || byContCode)
  {
    const unsigned __int16 contTime = _GetMonsterContTime(byEffectCode, byLv);
    const unsigned int startSec = _sf_continous::GetSFContCurTime();
    for (int j = 0; j < 8; ++j)
    {
      _sf_continous *cont = &m_SFCont[byContCode][j];
      if (cont->m_bExist && cont->m_byEffectCode == byEffectCode && cont->m_wEffectIndex == dwEffectIndex)
      {
        if (cont->m_byLv > byLv)
        {
          return 11;
        }
        const unsigned int elapsed = startSec - cont->m_dwStartSec;
        const int leftSec = cont->m_wDurSec - elapsed;
        if (cont->m_wDurSec && (static_cast<float>(leftSec) / static_cast<float>(contTime)) > 0.1f)
        {
          return 12;
        }
        RemoveSFContEffect(byContCode, j, false, false);
        break;
      }
    }

    _sf_continous *targetCont = nullptr;
    unsigned __int8 listIndex = static_cast<unsigned __int8>(-1);
    for (int j = 0; j < 8; ++j)
    {
      _sf_continous *cont = &m_SFCont[byContCode][j];
      if (!cont->m_bExist)
      {
        targetCont = cont;
        listIndex = static_cast<unsigned __int8>(j);
        break;
      }
    }

    if (!targetCont)
    {
      targetCont = m_SFCont[byContCode];
      listIndex = 0;
      for (int j = 1; j < 8; ++j)
      {
        _sf_continous *cont = &m_SFCont[byContCode][j];
        if (cont->m_dwEffSerial < targetCont->m_dwEffSerial)
        {
          targetCont = cont;
          listIndex = static_cast<unsigned __int8>(j);
        }
      }

      const unsigned int elapsed = startSec - targetCont->m_dwStartSec;
      const int leftSec = targetCont->m_wDurSec - elapsed;
      if (targetCont->m_wDurSec && (static_cast<float>(leftSec) / static_cast<float>(contTime)) > 0.1f)
      {
        *pbUpMty = false;
      }
      RemoveSFContEffect(byContCode, listIndex, false, false);
    }

    float effRate = FLOAT_1_0;
    if (!byContCode && byEffectCode == 1)
    {
      effRate = m_EP.GetEff_Rate(8);
    }

    unsigned __int16 contEffectSec = contTime;
    if (m_nContEffectSec == -1)
    {
      contEffectSec = static_cast<unsigned __int16>(static_cast<int>(static_cast<float>(contEffectSec) * effRate));
      const int maxDur = 2 * contTime;
      if (contEffectSec > maxDur)
      {
        contEffectSec = static_cast<unsigned __int16>(maxDur);
      }
    }
    else
    {
      contEffectSec = static_cast<unsigned __int16>(m_nContEffectSec);
    }

    _set_sf_cont(targetCont, byEffectCode, static_cast<unsigned __int16>(dwEffectIndex), byLv, startSec, contEffectSec, 0);
    SFContInsertMessage(byContCode, listIndex, false);
    m_bLastContEffectUpdate = true;
    m_SFContDamageTolerance.SetSFDamageToleracne_Variation(-0.050000001f);
    CheckEventEmotionPresentation(5u, nullptr);
    return 0;
  }

  const float tolProb = m_SFContDamageTolerance.GetToleranceProb();
  if (tolProb > 1.0f || tolProb <= 0.69999999f)
  {
    if (tolProb > 0.69999999f || tolProb <= 0.34999999f)
    {
      return 31;
    }
    return 30;
  }
  return 29;
}

CLuaSignalReActor *CMonster::GetSignalReActor()
{
  return &m_LuaSignalReActor;
}

__int64 CMonster::SetDamage(
  int nDam,
  CCharacter *pDst,
  int nDstLv,
  bool bCrt,
  int nAttackType,
  unsigned int dwAttackSerial,
  bool bJadeReturn)
{
  (void)nDstLv;
  (void)bJadeReturn;

  if (pDst != nullptr)
  {
    AttackObject(nDam, static_cast<CGameObject *>(pDst));
    if (nDam >= 0)
    {
      if (!pDst->m_ObjID.m_byID)
      {
        m_LootMgr.PushDamage(static_cast<CPlayer *>(pDst), static_cast<unsigned __int16>(nDam));
        m_AggroMgr.SetAggro(pDst, nDam, nAttackType, dwAttackSerial, 0, 0);
      }
      else if (pDst->m_ObjID.m_byID == 3)
      {
        CAnimus *animus = static_cast<CAnimus *>(pDst);
        if (animus->m_pMaster)
        {
          m_LootMgr.PushDamage(animus->m_pMaster, static_cast<unsigned __int16>(nDam));
          m_AggroMgr.SetAggro(pDst, nDam, nAttackType, dwAttackSerial, 0, 0);
        }
      }
      else if (pDst->m_ObjID.m_byID == 4)
      {
        CGuardTower *tower = static_cast<CGuardTower *>(pDst);
        if (tower->m_pMasterTwr)
        {
          m_LootMgr.PushDamage(tower->m_pMasterTwr, static_cast<unsigned __int16>(nDam));
          m_AggroMgr.SetAggro(pDst, nDam, nAttackType, dwAttackSerial, 0, 0);
        }
        else
        {
          m_LootMgr.PushDamage(&sPlayerDum, static_cast<unsigned __int16>(nDam));
        }
      }
    }
  }

  if (nDam >= 1)
  {
    m_nHP -= nDam;
    if (m_nHP < 0)
      m_nHP = 0;
  }

  if (m_nHP == 0)
  {
    ClearEmotionPresentation();
    CheckEventEmotionPresentation(8u, nullptr);
    CPlayer *looter = m_LootMgr.GetLooter(m_pCurMap, m_fCurPos, &sPlayerDum);
    Destroy(0, looter);
    if (pDst)
    {
      pDst->RecvKillMessage(this);
    }
  }

  if (bCrt && !m_pMonRec->m_bMonsterCondition)
    SetStun(true);

  m_AI.SendExternMsg(0, pDst, nDam);
  return static_cast<unsigned int>(m_nHP);
}

char CMonster::SetHP(int nHP, bool bOver)
{
  int newHP = nHP;
  if (newHP < 0)
    newHP = 0;

  if (!bOver)
  {
    const int maxHP = static_cast<int>(GetMaxHP());
    if (newHP > maxHP)
      newHP = maxHP;
  }

  if (m_nHP == newHP)
    return 0;

  m_nHP = newHP;
  return 1;
}

void CMonster::SetStun(bool bStun)
{
  m_bStun = bStun;
  if (!m_bStun)
    return;

  m_dwNextFreeStunTime = static_cast<unsigned int>(static_cast<float>(GetLoopTime() + 1000) + m_pMonRec->m_fCrtMoTime);
  if (m_bMove)
  {
    MoveBreak(GetMoveSpeed());
    Stop();
  }
}

CMonster *SearchEmptyMonster(bool bWithoutFail)
{
  for (unsigned int index = 0; index < MAX_MONSTER; ++index)
  {
    if (!g_Monster[index].m_bLive)
    {
      return &g_Monster[index];
    }
  }

  if (!bWithoutFail)
  {
    return nullptr;
  }

  for (unsigned int index = 0; index < MAX_MONSTER; ++index)
  {
    CMonster *mon = &g_Monster[index];
    if (!mon->m_bLive)
    {
      return mon;
    }

    if (!mon->m_pCurMap->m_pMapSet->m_nMapType
        && !mon->m_MonHierarcy.ChildKindCount()
        && !mon->m_MonHierarcy.GetParent()
        && mon->m_pMonRec->m_bMonsterCondition != 1
        && !mon->GetEmotionState())
    {
      mon->Destroy(1u, nullptr);
      return mon;
    }
  }

  for (unsigned int index = 0; index < MAX_MONSTER; ++index)
  {
    CMonster *mon = &g_Monster[index];
    if (!mon->m_bLive)
    {
      return mon;
    }

    if (!mon->m_pCurMap->m_pMapSet->m_nMapType
        && !mon->m_MonHierarcy.ChildKindCount()
        && !mon->m_MonHierarcy.GetParent()
        && mon->m_pMonRec->m_bMonsterCondition != 1
        && !mon->m_pTargetChar)
    {
      mon->Destroy(1u, nullptr);
      return mon;
    }
  }

  return nullptr;
}

CMonster *CreateRepMonster(
  CMapData *pMap,
  unsigned __int16 wLayer,
  float *fPos,
  char *pszMonsterCode,
  CMonster *pParent,
  bool bRobExp,
  bool bRewardExp,
  bool bDungeon,
  bool bWithoutFail,
  bool bApplyRopExpField)
{
  CMonster *mon = SearchEmptyMonster(bWithoutFail);
  if (!mon)
  {
    return nullptr;
  }

  _monster_create_setdata data;
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = g_Main.m_tblMonster.GetRecord(pszMonsterCode);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }

  memcpy_0(data.m_fStartPos, fPos, sizeof(data.m_fStartPos));
  data.pActiveRec = nullptr;
  data.pDumPosition = nullptr;
  data.pParent = pParent;
  data.bDungeon = bDungeon;
  if (bApplyRopExpField)
  {
    const _monster_fld *monRec = static_cast<const _monster_fld *>(data.m_pRecordSet);
    data.bRobExp = monRec->m_bExpDown != 0;
  }
  else
  {
    data.bRobExp = bRobExp;
  }
  data.bRewardExp = bRewardExp;

  mon->Create(&data);
  return mon;
}

CMonster *CreateRespawnMonster(
  CMapData *pMap,
  unsigned __int16 wLayer,
  int nMonsterIndex,
  _mon_active *pActiveRec,
  _dummy_position *pDumPosition,
  bool bRobExp,
  bool bRewardExp,
  bool bDungeon,
  bool bWithoutFail,
  bool bApplyRopExpField)
{
  CMonster *mon = SearchEmptyMonster(bWithoutFail);
  if (!mon)
  {
    return nullptr;
  }

  _monster_create_setdata data;
  data.m_pMap = pMap;
  data.m_nLayerIndex = wLayer;
  data.m_pRecordSet = g_Main.m_tblMonster.GetRecord(nMonsterIndex);
  if (!data.m_pRecordSet)
  {
    return nullptr;
  }

  if (pActiveRec && pActiveRec->m_pBlk && pActiveRec->m_pBlk->m_bRotate && pDumPosition)
  {
    memcpy_0(data.m_fStartPos, pDumPosition->m_fCenterPos, sizeof(data.m_fStartPos));
  }
  else if (!pMap->GetRandPosInDummy(pDumPosition, data.m_fStartPos, false))
  {
    return nullptr;
  }

  data.pActiveRec = pActiveRec;
  data.pDumPosition = pDumPosition;
  data.bDungeon = bDungeon;
  if (bApplyRopExpField)
  {
    const _monster_fld *monRec = static_cast<const _monster_fld *>(data.m_pRecordSet);
    data.bRobExp = monRec->m_bExpDown != 0;
  }
  else
  {
    data.bRobExp = bRobExp;
  }
  data.bRewardExp = bRewardExp;

  if (mon->Create(&data))
  {
    return mon;
  }
  return nullptr;
}


