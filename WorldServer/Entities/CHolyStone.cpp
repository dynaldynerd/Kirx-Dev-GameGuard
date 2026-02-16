#include "pch.h"
#include "CHolyStone.h"

#include "CHolyStoneSystem.h"
#include "CNetProcess.h"
#include "CGuardTower.h"
#include "CPlayer.h"
#include "CPlayerDB.h"
#include "GlobalObjects.h"

#include <cstdio>

namespace
{
char s_stoneObjectName[256]{};
}

int CHolyStone::s_nLiveNum = 0;
unsigned int CHolyStone::s_dwSerialCnt = 0;

CHolyStone::CHolyStone()
{
}

CHolyStone::~CHolyStone()
{
}

__int64 CHolyStone::GetAttackDP()
{
  return static_cast<unsigned int>(m_pRec->m_nAttack_DP);
}

__int64 CHolyStone::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
if (nAttactPart == -1)
    return static_cast<unsigned int>(m_nDefPart[rand() % 5]);
  return static_cast<unsigned int>(m_nDefPart[nAttactPart]);
}

float CHolyStone::GetDefFacing(int nPart)
{
return m_pRec->m_fDefFacing;
}

float CHolyStone::GetDefGap(int nPart)
{
return m_pRec->m_fDefGap;
}

__int64 CHolyStone::GetDefSkill(bool bBackAttackDamage)
{
return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fDefSklUnit));
}

__int64 CHolyStone::GetFireTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fFireTol));
}

__int64 CHolyStone::GetLevel()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fLevel));
}

__int64 CHolyStone::GetMaxHP()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fMaxHP));
}

char *CHolyStone::GetObjName()
{
  std::snprintf(
    s_stoneObjectName,
    sizeof(s_stoneObjectName),
    "[STONE][%d] >> %s (pos: %s {%d, %d, %d})",
    static_cast<int>(GetObjRace()),
    m_pRec->m_strName,
    m_pCurMap->m_pMapSet->m_strCode,
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]));
  return s_stoneObjectName;
}

__int64 CHolyStone::GetObjRace()
{
  return m_byMasterRaceCode;
}

__int64 CHolyStone::GetSoilTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fSoilTol));
}

__int64 CHolyStone::GetWaterTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fWaterTol));
}

float CHolyStone::GetWeaponAdjust()
{
  return m_pRec->m_fDefGap;
}

float CHolyStone::GetWidth()
{
  return m_pRec->m_fWidth;
}

__int64 CHolyStone::GetWindTol()
{
  return static_cast<unsigned int>(static_cast<int>(m_pRec->m_fWindTol));
}

bool CHolyStone::IsBeAttackedAble(bool bFirst)
{
// this is not a stub
  return true;
}

char CHolyStone::IsBeDamagedAble(CCharacter *pAtter)
{
  if (pAtter->m_ObjID.m_byID != 0 && pAtter->m_ObjID.m_byID != 3)
    return false;

  CCharacter *owner = nullptr;
  if (pAtter->m_ObjID.m_byID == 0)
    owner = pAtter;
  else if (pAtter->m_ObjID.m_byID == 3)
    owner = static_cast<CGuardTower *>(pAtter)->m_pMasterTwr;

  if (owner == nullptr)
    return false;

  if (g_Main.IsReleaseServiceMode() && owner->m_ObjID.m_byID == 0 && static_cast<CPlayer *>(owner)->m_byUserDgr != 0)
    return false;
  if (static_cast<unsigned __int8>(static_cast<CPlayer *>(owner)->m_Param.GetRaceCode()) == m_byMasterRaceCode)
    return false;

  float path[8]{};
  return static_cast<char>(owner->m_pCurMap->m_Level.mBsp->CanYouGoThere(owner->m_fCurPos, m_fCurPos, reinterpret_cast<float (*)[3]>(path)));
}

void CHolyStone::Loop()
{
  if (m_pRec->m_fHPRecDelay > 0.0f)
  {
    const unsigned int now = GetLoopTime();
    const unsigned int recoverDelay = static_cast<unsigned int>(m_pRec->m_fHPRecDelay * 1000.0f);
    if (now - m_dwLastRecoverTime >= recoverDelay)
    {
      m_dwLastRecoverTime = now;
      const int recoverHp = static_cast<int>(m_pRec->m_fHPRecUnit);
      SetHP(m_nHP + recoverHp, true);
    }
  }
}

void CHolyStone::OutOfSec()
{
  m_dwObjSerial = static_cast<unsigned int>(-1);
  --s_nLiveNum;
  CCharacter::Destroy();
}

void CHolyStone::SendMsg_FixPosition(int n)
{

  _stone_fixpositon_zocl msg{};
  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zCur, 3);
  msg.byMasterRace = m_byMasterRaceCode;
  msg.bOper = m_bOper;
  msg.bChip = m_nHP > 0;
  msg.nControlLeftSec = -1;
  msg.nControlLeftSec = g_HolySys.GetControlLeftTime();

  unsigned __int8 type[2] = {4, 166};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

__int64 CHolyStone::SetDamage(int nDam, CCharacter *pDst, int nDstLv)
{
if (g_HolySys.GetSceneCode() != 1)
    return static_cast<unsigned int>(m_nHP);

  CCharacter *attacker = nullptr;
  if (pDst->m_ObjID.m_byID == 0)
    attacker = pDst;
  else if (pDst->m_ObjID.m_byID == 3)
    attacker = static_cast<CGuardTower *>(pDst)->m_pMasterTwr;

  if (attacker == nullptr)
    return static_cast<unsigned int>(m_nHP);
  if (g_Main.IsReleaseServiceMode() && static_cast<CPlayer *>(attacker)->m_byUserDgr != 0)
    return static_cast<unsigned int>(m_nHP);

  if (nDam > 1)
  {
    m_nHP -= nDam;
    if (m_nHP < 0)
      m_nHP = 0;
  }

  if (m_nHP == 0)
    OutOfSec();

  return static_cast<unsigned int>(m_nHP);
}

__int64 CHolyStone::SetDamage(
  int nDam,
  CCharacter *pDst,
  int nDstLv,
  bool bCrt,
  int nAttackType,
  unsigned int dwAttackSerial,
  bool bJadeReturn)
{
return SetDamage(nDam, pDst, nDstLv);
}

bool CHolyStone::Create(_stone_create_setdata *pData)
{
  if (!CCharacter::Create(pData))
    return false;

  m_pRec = (_monster_fld *)m_pRecordSet;
  m_bOper = false;
  m_nHP = static_cast<int>(m_pRec->m_fMaxHP);
  m_nOldRate = 10000;
  m_dwLastRecoverTime = GetLoopTime();
  m_pRec->m_fHPRecDelay = 3.0f;
  m_pRec->m_fHPRecUnit = 1000.0f;
  m_nMaxHP = static_cast<int>(m_pRec->m_fMaxHP);
  m_pDum = pData->pDumPosition;
  m_byMasterRaceCode = pData->byMasterRace;
  for (int j = 0; j < 5; ++j)
    m_nDefPart[j] = static_cast<int>(m_pRec->m_fStdDefFc);
  m_dwObjSerial = CHolyStone::GetNewStoneSerial();
  SendMsg_Create();
  ++CHolyStone::s_nLiveNum;
  return true;
}

unsigned int CHolyStone::GetNewStoneSerial()
{
  return s_dwSerialCnt++;
}

void CHolyStone::SendMsg_Create()
{
  _stone_create_zocl msg{};

  msg.wRecIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.zPos, 3);
  msg.byMasterRace = m_byMasterRaceCode;

  unsigned __int8 pbyType[28]{};
  pbyType[0] = 3;
  pbyType[1] = static_cast<unsigned __int8>(-45);
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 15, false);
}

void CHolyStone::SendMsg_Destroy(unsigned __int8 byDestroyCode, unsigned int dwDestroySerial)
{
  char payload[9]{};
  *reinterpret_cast<unsigned int *>(payload) = m_dwObjSerial;
  payload[4] = static_cast<char>(byDestroyCode);
  *reinterpret_cast<unsigned int *>(payload + 5) = dwDestroySerial;

  unsigned __int8 type[2] = {3, static_cast<unsigned __int8>(-47)};
  CircleReport(type, payload, 9, false);
}

void CHolyStone::SetOper(bool bOper, float fHPRate)
{
  if (m_bOper != bOper)
  {
    m_bOper = bOper;
    if (m_bOper)
    {
      m_nHP = static_cast<int>(m_pRec->m_fMaxHP * fHPRate);
      if (m_nHP <= 0)
        m_nHP = 1;
      m_nMaxHP = m_nHP;
    }
    SendMsg_StoneAlterOper();
  }
}

void CHolyStone::SendMsg_StoneAlterOper()
{
  _holystone_alter_oper_circle_inform_zocl msg{};

  msg.dwSerial = m_dwObjSerial;
  msg.bOper = m_bOper;
  msg.wCurHPRate = static_cast<unsigned __int16>(CalcCurHPRate());

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 25;
  pbyType[1] = 14;
  CircleReport(pbyType, reinterpret_cast<char *>(&msg), 7, false);
}

unsigned int CHolyStone::CalcCurHPRate()
{
  return static_cast<unsigned int>(static_cast<int>((static_cast<float>(m_nHP) / static_cast<float>(m_nMaxHP)) * 10000.0f));
}

char CHolyStone::IsChangedHP(unsigned __int16 wAlterRate)
{
  if (!wAlterRate)
  {
    return 0;
  }

  const int currentRate = static_cast<int>(CalcCurHPRate());
  if (std::abs(currentRate - m_nOldRate) < wAlterRate)
  {
    return 0;
  }

  m_nOldRate = currentRate;
  return 1;
}

__int64 CHolyStone::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

char CHolyStone::SetHP(int nHP, bool bOver)
{
  int targetHP = nHP;
  if (targetHP < 0)
    targetHP = 0;
  if (!bOver && targetHP > m_nMaxHP)
    targetHP = m_nMaxHP;
  if (m_nHP == targetHP)
    return 0;
  m_nHP = targetHP;
  return 1;
}

bool CHolyStone::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_bOper = false;
  m_nHP = 0;
  m_nMaxHP = 0;
  m_pRec = nullptr;
  m_pDum = nullptr;
  m_byMasterRaceCode = static_cast<unsigned __int8>(-1);
  for (int j = 0; j < 5; ++j)
    m_nDefPart[j] = 0;
  m_nOldRate = 0;
  m_dwLastRecoverTime = 0;
  m_nCurrLootIndex = -1;
  m_nEndLootIndex = -1;
  m_nCurrDropIndex = -1;
  m_wMagnifications = 0;
  m_wRange = 0;
  m_wDropCntOnce = 0;
  m_wAddCountWithPlayer = 0;
  return true;
}
