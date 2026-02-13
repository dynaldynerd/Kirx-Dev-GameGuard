#include "pch.h"

#include "CTrap.h"

#include "CNetProcess.h"
#include "CPlayer.h"
#include "GlobalObjects.h"

#include <cstring>
#include <cstdio>
#include <mmsystem.h>

namespace
{
char s_trapObjectName[256]{};
}

__int64 CTrap::GetAttackDP()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[28]);
}

float CTrap::GetAttackRange()
{
  return *reinterpret_cast<float *>(&m_pRecordSet[5].m_strCode[24]);
}

__int64 CTrap::GetDefFC(int nAttactPart, CCharacter *pAttChar, int *pnConvertPart)
{
  (void)nAttactPart;
  (void)pAttChar;
  (void)pnConvertPart;
  return *reinterpret_cast<unsigned int *>(m_pRecordSet[6].m_strCode);
}

float CTrap::GetDefFacing(int nPart)
{
  (void)nPart;
  return *reinterpret_cast<float *>(&m_pRecordSet[6].m_strCode[8]);
}

float CTrap::GetDefGap(int nPart)
{
  (void)nPart;
  return *reinterpret_cast<float *>(&m_pRecordSet[6].m_strCode[4]);
}

__int64 CTrap::GetDefSkill(bool bBackAttackDamage)
{
  (void)bBackAttackDamage;
  return m_pRecordSet[6].m_dwIndex;
}

__int64 CTrap::GetFireTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[7].m_strCode[12]);
}

__int64 CTrap::GetGenAttackProb(CCharacter *pDst, int nPart, bool bBackAttack)
{
  (void)nPart;

  int hitRate = static_cast<int>(
    static_cast<double>(*reinterpret_cast<int *>(&m_pRecordSet[5].m_strCode[40]))
    - (static_cast<float>(pDst->GetLevel()) * 10.0f + static_cast<float>(pDst->GetDefSkill(bBackAttack))) / 4.0f
    + 95.0f);
  hitRate -= static_cast<int>(pDst->GetAvoidRate());
  if (hitRate < 5)
    hitRate = 5;
  if (hitRate > 95)
    return 95;
  return static_cast<unsigned int>(hitRate);
}

__int64 CTrap::GetHP()
{
  return static_cast<unsigned int>(m_nHP);
}

__int64 CTrap::GetLevel()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[4].m_strCode[56]);
}

__int64 CTrap::GetMaxHP()
{
  return static_cast<unsigned int>(static_cast<int>(*reinterpret_cast<float *>(&m_pRecordSet[5].m_strCode[60])));
}

char *CTrap::GetObjName()
{
  std::snprintf(
    s_trapObjectName,
    sizeof(s_trapObjectName),
    "[Trap][%d] >> %s (pos: %s {%d, %d, %d}) (master: %s)",
    static_cast<int>(GetObjRace()),
    reinterpret_cast<const char *>(m_pRecordSet + 3),
    m_pCurMap->m_pMapSet->m_strCode,
    static_cast<int>(m_fCurPos[0]),
    static_cast<int>(m_fCurPos[1]),
    static_cast<int>(m_fCurPos[2]),
    m_aszMasterName);
  return s_trapObjectName;
}

__int64 CTrap::GetObjRace()
{
  return m_byRaceCode;
}

__int64 CTrap::GetSoilTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[7].m_strCode[20]);
}

__int64 CTrap::GetWaterTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[7].m_strCode[16]);
}

float CTrap::GetWeaponAdjust()
{
  return 1.0f;
}

__int64 CTrap::GetWeaponClass()
{
  return 1;
}

float CTrap::GetWidth()
{
  return 5.0f;
}

__int64 CTrap::GetWindTol()
{
  return *reinterpret_cast<unsigned int *>(&m_pRecordSet[7].m_strCode[24]);
}

bool CTrap::IsBeAttackedAble(bool bFirst)
{
  (void)bFirst;
  return true;
}

void CTrap::Init(_object_id *pID)
{
  CCharacter::Init(pID);
  m_dwLastDestroyTime = 0;
  m_dwMasterSerial = static_cast<unsigned int>(-1);
  m_nHP = 0;
  m_pMaster = nullptr;
  m_byRaceCode = static_cast<unsigned __int8>(-1);
  std::memset(m_wszMasterName, 0, sizeof(m_wszMasterName));
  std::memset(m_aszMasterName, 0, sizeof(m_aszMasterName));
  m_dMasterPvPPoint = 0.0;
  m_dwStartMakeTime = 0;
  m_bComplete = false;
  m_bBreakTransparBuffer = false;
  m_nTrapMaxAttackPnt = 0;
}

void CTrap::MasterNetClose(long double dPvPPoint)
{
  m_pMaster = nullptr;
  m_dMasterPvPPoint = dPvPPoint;
}

void CTrap::MasterReStart(CPlayer *pMaster)
{
  m_pMaster = pMaster;
  m_dMasterPvPPoint = pMaster->m_Param.GetPvPPoint();
}

void CTrap::Loop()
{
  const DWORD now = timeGetTime();

  if (m_bComplete)
  {
    const unsigned int elapsed = now - m_dwStartMakeTime;
    const unsigned int trapLifetime = 60000u * (*reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[12]));
    if (elapsed > trapLifetime)
      OutOfSec();
  }
  else
  {
    const unsigned int elapsed = now - m_dwStartMakeTime;
    unsigned int buildMs = 1000u * (*reinterpret_cast<unsigned int *>(&m_pRecordSet[5].m_strCode[4]));
    if (!g_Main.IsReleaseServiceMode())
      buildMs = 10000;

    if (elapsed > buildMs)
    {
      m_bComplete = true;
      if (m_pMaster != nullptr)
        m_pMaster->Emb_CheckActForQuest(16, m_pRecordSet->m_strCode, 1u, false);
    }
  }
}

void CTrap::OutOfSec()
{
  m_dwLastDestroyTime = timeGetTime();
  m_dwObjSerial = static_cast<unsigned int>(-1);
  CCharacter::Destroy();
}

void CTrap::SendMsg_FixPosition(int n)
{
#pragma pack(push, 1)
  struct TrapFixPosMsg
  {
    unsigned __int16 wRecordIndex;
    unsigned __int16 wIndex;
    unsigned int dwObjSerial;
    __int16 pos[3];
    __int16 nBuildSec;
    unsigned int dwMasterSerial;
    unsigned __int8 byNotBreakTranspar;
    unsigned __int8 byRaceCode;
  };
#pragma pack(pop)

  TrapFixPosMsg msg{};
  msg.wRecordIndex = static_cast<unsigned __int16>(m_pRecordSet->m_dwIndex);
  msg.wIndex = m_ObjID.m_wIndex;
  msg.dwObjSerial = m_dwObjSerial;
  FloatToShort(m_fCurPos, msg.pos, 3);
  msg.dwMasterSerial = m_dwMasterSerial;
  msg.byNotBreakTranspar = static_cast<unsigned __int8>(!m_bBreakTransparBuffer);
  msg.byRaceCode = m_byRaceCode;

  if (m_bComplete)
    msg.nBuildSec = 0;
  else
    msg.nBuildSec = static_cast<__int16>((timeGetTime() - m_dwStartMakeTime) / 1000);

  unsigned __int8 type[2] = {4, 168};
  g_Network.m_pProcess[0]->LoadSendMsg(n, type, reinterpret_cast<char *>(&msg), sizeof(msg));
}

__int64 CTrap::SetDamage(int nDam, CCharacter *pDst, int nDstLv)
{
  (void)pDst;
  (void)nDstLv;

  if (nDam > 1)
  {
    m_nHP -= nDam;
    if (m_nHP < 0)
      m_nHP = 0;
  }

  if (m_nHP > 0)
  {
    SetBreakTranspar(true);
  }
  else
  {
    OutOfSec();
  }

  return static_cast<unsigned int>(m_nHP);
}

__int64 CTrap::SetDamage(
  int nDam,
  CCharacter *pDst,
  int nDstLv,
  bool bCrt,
  int nAttackType,
  unsigned int dwAttackSerial,
  bool bJadeReturn)
{
  (void)bCrt;
  (void)nAttackType;
  (void)dwAttackSerial;
  (void)bJadeReturn;
  return SetDamage(nDam, pDst, nDstLv);
}

