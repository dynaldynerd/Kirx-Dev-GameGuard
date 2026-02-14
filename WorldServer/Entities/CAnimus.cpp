#include "pch.h"

#include "CAnimus.h"
#include "animus_fld.h"
#include "CGameObject.h"
#include "CNetProcess.h"
#include "GlobalObjects.h"

#include <cstdio>
#include <cstring>
#include <mmsystem.h>

CRecordData CAnimus::s_tblParameter[8];
unsigned int CAnimus::MAX_EXP[8]{};
int CAnimus::s_nLiveNum = 0;

namespace
{
  struct __unaligned __declspec(align(1)) _animus_fixpos_packet
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wObjIndex;
    unsigned int dwObjSerial;
    short pos[3];
    char byLevel;
    unsigned __int16 wContEffect;
    unsigned int dwMasterSerial;
  };

  struct __unaligned __declspec(align(1)) _animus_realmove_packet
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wObjIndex;
    unsigned int dwObjSerial;
    short posAndTarget[5];
    char byLevel;
    unsigned __int16 wContEffect;
    unsigned int dwMasterSerial;
    float fMoveSpeed;
  };

  static char s_animusObjectName[256]{};
}

_animus_fld *GetAnimusFldFromLv(int nAnimusClass, unsigned int dwLv)
{
  CRecordData *table = &CAnimus::s_tblParameter[nAnimusClass];
  for (int n = 0; n < 65; ++n)
  {
    _animus_fld *record = reinterpret_cast<_animus_fld *>(table->GetRecord(n));
    if (record == nullptr)
    {
      return nullptr;
    }
    if (record->m_nLevel == dwLv)
    {
      return record;
    }
  }
  const int recordNum = static_cast<int>(table->GetRecordNum());
  return reinterpret_cast<_animus_fld *>(table->GetRecord(recordNum - 1));
}

bool CAnimus::SetStaticMember()
{
  static const char *kFiles[8] = {
    ".\\script\\PaimonCharacter.dat",
    ".\\script\\HecateCharacter.dat",
    ".\\script\\InannaCharacter.dat",
    ".\\script\\IsisCharacter.dat",
    ".\\script\\PaimonCharacter_S.dat",
    ".\\script\\HecateCharacter_S.dat",
    ".\\script\\InannaCharacter_S.dat",
    ".\\script\\IsisCharacter_S.dat",
  };

  char errMsg[136]{};
  for (int nAnimusClass = 0; nAnimusClass < 8; ++nAnimusClass)
  {
    if (!CAnimus::s_tblParameter[nAnimusClass].ReadRecord(kFiles[nAnimusClass], 0x1B8u, errMsg))
    {
      MyMessageBox("CAnimus::SetStaticMember", errMsg);
      return false;
    }

    _animus_fld *record = GetAnimusFldFromLv(nAnimusClass, 0x40u);
    if (record == nullptr)
    {
      MyMessageBox("CAnimus::SetStaticMember", "::GetAnimusFldFromLv(...) Fail!");
      return false;
    }

    CAnimus::MAX_EXP[nAnimusClass] = static_cast<unsigned int>(record->m_nForLvUpExp - 1);
  }
  return true;
}

void CAnimus::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_byClassCode = static_cast<unsigned __int8>(-1);
  m_nHP = 0;
  m_nFP = 0;
  m_dwExp = 0;
  m_pMaster = nullptr;
  m_dwMasterSerial = 0;
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
  std::memset(m_aszMasterName, 0, sizeof(m_aszMasterName));
  m_byRoleCode = static_cast<unsigned __int8>(-1);
  m_fMoveSpeed = 0.0f;
  m_byPosRaceTown = static_cast<unsigned __int8>(-1);
  m_pBeforeTownCheckMap = nullptr;
  m_fBeforeTownCheckPos[0] = 0.0f;
  m_fBeforeTownCheckPos[1] = 0.0f;
  m_dwStunTime = 0;
  m_dwBeAttackedTargetTime = 0;
  m_pNextTarget = nullptr;
  m_nMaxAttackPnt = 0;
  m_pRecord = nullptr;
  m_nMaxHP = 0;
  m_nMaxFP = 0;
  m_Mightiness = 0.0f;
  for (int j = 0; j < 5; ++j)
  {
    m_DefPart[j] = 0;
  }
  m_dwAIMode = 0;
  m_pTarget = nullptr;
}

bool CAnimus::Destroy()
{
  m_dwLastDestroyTime = timeGetTime();
  SendMsg_Destroy();
  m_pMaster = nullptr;
  m_dwObjSerial = static_cast<unsigned int>(-1);
  m_tmNextEatMasterFP = static_cast<unsigned int>(-1);
  --CAnimus::s_nLiveNum;
  return CCharacter::Destroy();
}

void CAnimus::SendMsg_Destroy()
{
  char szMsg[7]{};
  memcpy_0(szMsg, &m_ObjID.m_wIndex, sizeof(m_ObjID.m_wIndex));
  memcpy_0(szMsg + 2, &m_dwObjSerial, sizeof(m_dwObjSerial));
  szMsg[6] = (m_nHP <= 0);

  unsigned __int8 pbyType[36]{};
  pbyType[0] = 3;
  pbyType[1] = 26;
  CircleReport(pbyType, szMsg, 7, false);
}

void CAnimus::MasterAttack_MasterInform(CCharacter *pDst)
{
  if (!m_pTarget)
  {
    m_pTarget = pDst;
  }
}

__int64 CAnimus::GetAttackDP()
{
  return static_cast<unsigned int>(m_pRecord->m_nAttack_DP);
}

float CAnimus::GetAttackRange()
{
  return static_cast<float>(m_pRecord->m_nAttExt);
}

__int64 CAnimus::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
int baseDefFc = m_pRecord->m_nStdDefFc;
  if (m_pMaster != nullptr && m_byRoleCode == 1)
  {
    baseDefFc = static_cast<int>(static_cast<float>(baseDefFc) * m_pMaster->m_EP.GetEff_Rate(30));
  }
  return static_cast<unsigned int>(baseDefFc);
}

float CAnimus::GetDefFacing(int nPart)
{
return m_pRecord->m_fDefFacing;
}

float CAnimus::GetDefGap(int nPart)
{
return m_pRecord->m_fDefGap;
}

__int64 CAnimus::GetDefSkill(bool bBackAttackDamage)
{
return static_cast<unsigned int>(m_pRecord->m_nDefSklUnit);
}

__int64 CAnimus::GetFireTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nFireTol);
}

__int64 CAnimus::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
const double attackSkill = static_cast<double>(m_pRecord->m_nAttSklUnit);
  const float targetLevel = static_cast<float>(pDst->GetLevel());
  const int targetDefSkill = static_cast<int>(m_pTarget->GetDefSkill(bBackAttack));
  int probability = static_cast<int>(attackSkill - (targetLevel + static_cast<float>(targetDefSkill)) / 4.0f + 95.0f);
  probability -= static_cast<int>(pDst->GetAvoidRate());

  if (probability < 5)
  {
    return 5;
  }
  if (probability > 95)
  {
    return 95;
  }
  return static_cast<unsigned int>(probability);
}

__int64 CAnimus::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CAnimus::GetLevel()
{
  return static_cast<unsigned int>(m_pRecord->m_nLevel);
}

__int64 CAnimus::GetMaxHP()
{
  if (m_nMaxHP <= 1)
  {
    return 1;
  }
  return static_cast<unsigned int>(m_nMaxHP);
}

char *CAnimus::GetObjName()
{
  const int raceCode = static_cast<int>(GetObjRace());
  const int posX = static_cast<int>(m_fCurPos[0]);
  const int posY = static_cast<int>(m_fCurPos[1]);
  const int posZ = static_cast<int>(m_fCurPos[2]);
  sprintf(
    s_animusObjectName,
    "[ANIMUS][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)",
    raceCode,
    m_pRecord->m_strCode,
    m_pCurMap->m_pMapSet->m_strCode,
    posX,
    posY,
    posZ,
    m_aszMasterName);
  return s_animusObjectName;
}

__int64 CAnimus::GetObjRace()
{
  if (m_pMaster != nullptr)
  {
    return m_pMaster->GetObjRace();
  }
  return static_cast<unsigned int>(-1);
}

__int64 CAnimus::GetSoilTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nSoilTol);
}

__int64 CAnimus::GetWaterTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nWaterTol);
}

float CAnimus::GetWeaponAdjust()
{
  return m_pRecord->m_fAttGap;
}

__int64 CAnimus::GetWeaponClass()
{
  return m_byRoleCode != 1;
}

float CAnimus::GetWidth()
{
  return static_cast<float>(m_pRecord->m_nWidth);
}

__int64 CAnimus::GetWindTol()
{
  return static_cast<unsigned int>(m_pRecord->m_nWindTol);
}

bool CAnimus::IsBeAttackedAble(bool bFirst)
{
// this is not a stub
  return true;
}

void CAnimus::Loop()
{
  if (m_dwStunTime != 0)
  {
    const unsigned int elapsed = GetLoopTime() - m_dwStunTime;
    if (elapsed > static_cast<unsigned int>(m_pRecord->m_nCrtMoTime + 1000))
      m_dwStunTime = 0;
  }
}

void CAnimus::OutOfSec()
{
  Destroy();
}

void CAnimus::SendMsg_FixPosition(int n)
{
  _animus_fixpos_packet packet{};
  packet.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  packet.wObjIndex = m_ObjID.m_wIndex;
  packet.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, packet.pos, 3);
  packet.byLevel = m_pRecord != nullptr ? static_cast<char>(m_pRecord->m_nLevel) : 1;
  packet.wContEffect = m_wLastContEffect;
  packet.dwMasterSerial = m_pMaster->m_dwObjSerial;

  unsigned __int8 packetType[2] = {4, 13};
  g_Network.m_pProcess[0]->LoadSendMsg(n, packetType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

void CAnimus::SendMsg_RealMovePoint(int n)
{
  _animus_realmove_packet packet{};
  packet.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  packet.wObjIndex = m_ObjID.m_wIndex;
  packet.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, packet.posAndTarget, 3);
  packet.posAndTarget[3] = static_cast<short>(static_cast<int>(m_fTarPos[0]));
  packet.posAndTarget[4] = static_cast<short>(static_cast<int>(m_fTarPos[2]));
  packet.byLevel = m_pRecord != nullptr ? static_cast<char>(m_pRecord->m_nLevel) : 1;
  packet.wContEffect = m_wLastContEffect;
  packet.dwMasterSerial = m_pMaster->m_dwObjSerial;
  packet.fMoveSpeed = m_fMoveSpeed;

  unsigned __int8 packetType[2] = {4, 24};
  g_Network.m_pProcess[0]->LoadSendMsg(n, packetType, reinterpret_cast<char *>(&packet), sizeof(packet));
}

__int64 CAnimus::SetDamage(int nDam, CCharacter *pDst, int nDstLv, bool bCrt)
{
if (nDam > 1)
  {
    m_nHP -= nDam;
    if (m_nHP <= 0)
      m_nHP = 0;

    if (bCrt)
    {
      m_dwStunTime = GetLoopTime();
      if (m_bMove)
      {
        MoveBreak(m_fMoveSpeed);
        Stop();
      }
    }
  }

  if (m_nHP == 0)
  {
    Destroy();
  }

  return static_cast<unsigned int>(m_nHP);
}
